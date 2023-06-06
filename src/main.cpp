#include <algorithm>
#include <fstream>
#include <cstring>
#include <vector>
#include "jit.h"

using namespace llvm;

cl::opt<std::string> vertPath("vert", cl::Required, cl::desc("Specify vertex shader path."));
cl::opt<std::string> fragPath("frag", cl::Required, cl::desc("Specify fragment shader path."));
cl::opt<std::string> outputPath("o", cl::init("result.ppm"), cl::desc("Specify output path."));

class Image
{
public:
    Image(int width, int height, int channel = 3) : _width(width), _height(height), _channel(channel)
    {
        data = new char[width * height * channel];
        memset(data, 0, sizeof(char) * width * height * channel);
    }
    ~Image() { delete[] data; }
    void dump(std::ostream &out)
    {
        out << "P6\n"
            << width() << " " << height() << "\n255\n";
        out.write(data, width() * height() * channel());
    }

    bool setPixel(int x, int y, std::vector<float> color)
    {
		float alpha = 1;
		if (color.size() >= 4) {
            alpha = color[3];
            if (alpha > 1) alpha = 1;
            if (alpha < 0) alpha = 0;
        } 
        if (x < 0 || x >= width())
            return false;
        if (y < 0 || y >= height())
            return false;
        if (color.size() < channel())
            return false;
        int base_loc = y * width() * channel() + x * channel();
        for (int i = 0; i < channel(); i++)
        {
            float res = color[i];
            if (res > 1) res = 1;
            if (res < 0) res = 0;
            data[base_loc + i] = int(res * 255 + 0.5) * alpha + data[base_loc + i] * (1 - alpha);
        }
        return true;
    }
    int width() { return _width; }
    int height() { return _height; }
    int channel() { return _channel; }

private:
    int _width, _height, _channel;
    char *data;
};

JITHelper *helper = nullptr;

void (*vertShaderImpl)() = nullptr;
void (*fragShaderImpl)() = nullptr;

std::vector<float> vertShader(const std::vector<float> &point)
{
	helper->setVector<float>("aPos", point);
	helper->setVector<float>("gl_Position", { 0.0, 0.0, 0.0, 0.0 });
	vertShaderImpl();
	return helper->getVector<float>("gl_Position");
}

std::vector<float> fragShader(const std::vector<float> &point)
{
	helper->setVector<float>("gl_Position", point);
	fragShaderImpl();
	return helper->getVector<float>("FragColor");
}

void renderPixel(Image &image, int x, int y)
{
    std::vector<float> tPos = vertShader({float(x), float(y), 0});
    std::vector<float> color = fragShader(tPos);
    image.setPixel(x, y, color);
}

void renderLine(Image &image, int l, int r, int y)
{
    if (l > r)
        std::swap(l, r);
    for (int i = std::max(0, l); i <= std::min(r, image.width() - 1); i++)
        renderPixel(image, i, y);
}

void renderUpTriangle(Image &image, int tx, int ty, float llx, float lrx, int ly)
{
	float curxl = tx, curxr = tx;
	float ldx = (llx - tx) / (ly - ty);
	float rdx = (lrx - tx) / (ly - ty);
	for (int i = ty; i <= ly; i++) {
		renderLine(image, int(curxl + 0.5f), int(curxr + 0.5f), i);
		curxl += ldx;
		curxr += rdx;
	}
}

void renderDownTriangle(Image &image, int tx, int ty, float llx, float lrx, int ly)
{
	float curxl = tx, curxr = tx;
	float ldx = (llx - tx) / (ly - ty);
	float rdx = (lrx - tx) / (ly - ty);
	for (int i = ty; i >= ly; i--) {
		renderLine(image, int(curxl + 0.5f), int(curxr + 0.5f), i);
		curxl -= ldx;
		curxr -= rdx;
	}
}

void renderTriangle(Image &image, int ax, int ay, int bx, int by, int cx, int cy)
{
	std::vector<std::pair<int, int>> points;
	points.push_back(std::make_pair(ax, ay));
	points.push_back(std::make_pair(bx, by));
	points.push_back(std::make_pair(cx, cy));
	sort(points, [](std::pair<int, int> a, std::pair<int, int> b) {
		return a.second == b.second ? a.first < b.first : a.second < b.second;
		});
	if (points[1].second == points[0].second) {
		renderDownTriangle(image, points[2].first, points[2].second, points[0].first, points[1].first, points[1].second);
	}
	else if (points[1].second == points[2].second) {
		renderUpTriangle(image, points[0].first, points[0].second, points[2].first, points[1].first, points[1].second);
	}
	else {
		int dx = points[2].first - points[0].first, dy = points[2].second - points[0].second;
		float k = float(dx) / dy;
		float midx = points[0].first + k * (points[1].second - points[0].second);
		renderDownTriangle(image, points[2].first, points[2].second, 
			std::min(midx, float(points[1].first)), std::max(midx, float(points[1].first)), points[1].second);
		renderUpTriangle(image, points[0].first, points[0].second,
			std::min(midx, float(points[1].first)), std::max(midx, float(points[1].first)), points[1].second);
	}
}

void renderTriangle(Image &image, std::vector<std::vector<float>> v) {
	for (int i = 0; i <= v.size() - 3; i++) {
		renderTriangle(image, 
			v[i + 0][0], v[i + 0][1], 
			v[i + 1][0], v[i + 1][1],
			v[i + 2][0], v[i + 2][1]
		);
	}
}

void prepare(){
	helper = new JITHelper();
	LLVMContext &C = helper->getContext();
	Type *floatTy = Type::getFloatTy(C);
	ExitOnErr(helper->addIRFile(vertPath, "vert_"));
	ExitOnErr(helper->addIRFile(fragPath, "frag_"));
	helper->addVector("aPos", floatTy, 3);
	helper->addVector("FragColor", floatTy, 4);
	helper->addVector("gl_Position", floatTy, 4);
	helper->startJIT();
	vertShaderImpl = (void(*)())(helper->getFunction("vert_main"));
	fragShaderImpl = (void(*)())(helper->getFunction("frag_main"));
}

int main(int argc, const char **argv)
{
	INIT_LLVM();
    prepare();
    Image image(512, 512);
	std::vector<std::vector<float>> points = {
		{ 256, 69, 0 },
		{ 64, 443, 0 },
		{ 448, 443, 0 },
	};
	std::vector<std::vector<float>> tPoints;
	for (std::vector<float> &i : points) {
		tPoints.push_back(vertShader(i));
	}
    renderTriangle(image, tPoints);
    std::ofstream fout("result.ppm", std::ios::binary);
    image.dump(fout);
    fout.close();
    return 0;
}

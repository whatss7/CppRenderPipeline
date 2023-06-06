#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
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

JITHelper *vertHelper = nullptr;
JITHelper *fragHelper = nullptr;

void (*vertShaderImpl)() = nullptr;
void (*fragShaderImpl)() = nullptr;

struct VertexInfo {
	std::vector<float> gl_Position;
	std::vector<float> color;
	VertexInfo() {}
	VertexInfo(std::vector<float> gl_Position, std::vector<float> color) : gl_Position(gl_Position), color(color) {}
};

VertexInfo vertShader(const std::vector<float> &point)
{
	vertHelper->setVector<float>("aPos", point);
	vertHelper->setVector<float>("color", { 0.0, 0.0, 0.0, 0.0 });
	vertHelper->setVector<float>("gl_Position", { 0.0, 0.0, 0.0, 0.0 });
	vertShaderImpl();
	return VertexInfo(vertHelper->getVector<float>("gl_Position"), vertHelper->getVector<float>("color"));
}

std::vector<float> fragShader(const std::vector<float> &color)
{
	fragHelper->setVector<float>("color", color);
	fragHelper->setVector<float>("FragColor", { 0.0, 0.0, 0.0, 0.0 });
	fragShaderImpl();
	return fragHelper->getVector<float>("FragColor");
}

struct Fragment {
	VertexInfo ainfo, binfo, cinfo;
	float ax() const { return ainfo.gl_Position[0]; }
	float ay() const { return ainfo.gl_Position[1]; }
	float bx() const { return binfo.gl_Position[0]; }
	float by() const { return binfo.gl_Position[1]; }
	float cx() const { return cinfo.gl_Position[0]; }
	float cy() const { return cinfo.gl_Position[1]; }
};

Fragment getFragment(const std::vector<std::vector<float>> &points, unsigned offset = 0) {
	auto runOnPoint = [&points](unsigned index) {
		std::vector<float> v = points[index];
		while (v.size() < 3) v.push_back(0.0);
		return vertShader(v);
	};
	Fragment frag;
	frag.ainfo = runOnPoint(offset + 0);
	frag.binfo = runOnPoint(offset + 1);
	frag.cinfo = runOnPoint(offset + 2);
	return frag;
}

float getarea(float ax, float ay, float bx, float by, float cx, float cy) {
	return 0.5f * abs((ax * (by - cy)) + (bx * (cy - ay)) + (cx * (ay - by)));
}

void renderPixel(Image &image, const Fragment &frag, int x, int y)
{
	std::vector<float> color;
	float area = getarea(frag.ax(), frag.ay(), frag.bx(), frag.by(), frag.cx(), frag.cy());
	if (area < 1e-8) color = frag.ainfo.color;
	else {
		float aw = getarea(x, y, frag.bx(), frag.by(), frag.cx(), frag.cy()) / area;
		float bw = getarea(frag.ax(), frag.ay(), x, y, frag.cx(), frag.cy()) / area;
		float cw = getarea(frag.ax(), frag.ay(), frag.bx(), frag.by(), x, y) / area;
		for (int i = 0; i < 4; i++) {
			color.push_back(0.0);
			color[i] += aw * frag.ainfo.color[i];
			color[i] += bw * frag.binfo.color[i];
			color[i] += cw * frag.cinfo.color[i];
		}
	}
	color = fragShader(color);
	image.setPixel(x, y, color);
}

void renderLine(Image &image, const Fragment &frag, float l, float r, int y)
{
	if (l > r)
		std::swap(l, r);
	for (int i = std::max(0.0f, ceilf(l)); i <= std::min(float(image.width() - 1), floorf(r)); i++)
		renderPixel(image, frag, i, y);
}

void renderUpTriangle(Image &image, const Fragment &frag, float tx, float ty, float llx, float lrx, float ly)
{
	float curxl = tx, curxr = tx;
	float ldx = (llx - tx) / (ly - ty);
	float rdx = (lrx - tx) / (ly - ty);
	int init_i = ceil(ty);
	curxl += (init_i - ty) * ldx;
	curxr += (init_i - ty) * rdx;
	for (int i = ty; i <= ly; i++) {
		renderLine(image, frag, int(curxl + 0.5f), int(curxr + 0.5f), i);
		curxl += ldx;
		curxr += rdx;
	}
}

void renderDownTriangle(Image &image, const Fragment &frag, float tx, float ty, float llx, float lrx, float ly)
{
	float curxl = llx, curxr = lrx;
	float ldx = (llx - tx) / (ly - ty);
	float rdx = (lrx - tx) / (ly - ty);
	int init_i = ceil(ty);
	curxl += (init_i - ty) * ldx;
	curxr += (init_i - ty) * rdx;
	for (int i = ty; i <= ly; i++) {
		renderLine(image, frag, int(curxl + 0.5f), int(curxr + 0.5f), i);
		curxl += ldx;
		curxr += rdx;
	}
}

void renderTriangle(Image &image, const Fragment &frag)
{
	float ax = frag.ax(), ay = frag.ay();
	float bx = frag.bx(), by = frag.by();
	float cx = frag.cx(), cy = frag.cy();

	std::vector<std::pair<float, float>> points;

	points.push_back(std::make_pair(ax, ay));
	points.push_back(std::make_pair(bx, by));
	points.push_back(std::make_pair(cx, cy));
	sort(points, [](std::pair<float, float> a, std::pair<float, float> b) {
		return a.second == b.second ? a.first < b.first : a.second < b.second;
		});
	if (points[1].second == points[0].second) {
		renderDownTriangle(image, frag, points[2].first, points[2].second, points[0].first, points[1].first, points[1].second);
	}
	else if (points[1].second == points[2].second) {
		renderUpTriangle(image, frag, points[0].first, points[0].second, points[2].first, points[1].first, points[1].second);
	}
	else {
		int dx = points[2].first - points[0].first, dy = points[2].second - points[0].second;
		float k = float(dx) / dy;
		float midx = points[0].first + k * (points[1].second - points[0].second);
		renderDownTriangle(image, frag, points[2].first, points[2].second,
			std::min(midx, float(points[1].first)), std::max(midx, float(points[1].first)), points[1].second);
		renderUpTriangle(image, frag, points[0].first, points[0].second,
			std::min(midx, float(points[1].first)), std::max(midx, float(points[1].first)), points[1].second);
	}
}

void prepareVert() {
	vertHelper = new JITHelper();
	LLVMContext &C = vertHelper->getContext();
	Type *floatTy = Type::getFloatTy(C);
	ExitOnErr(vertHelper->addIRFile(vertPath));
	vertHelper->addVector("aPos", floatTy, 3);
	vertHelper->addVector("color", floatTy, 4);
	vertHelper->addVector("gl_Position", floatTy, 4);
	vertHelper->startJIT();
	vertShaderImpl = (void(*)())(vertHelper->getFunction("main"));
}

void prepareFrag() {
	fragHelper = new JITHelper();
	LLVMContext &C = fragHelper->getContext();
	Type *floatTy = Type::getFloatTy(C);
	ExitOnErr(fragHelper->addIRFile(fragPath));
	fragHelper->addVector("color", floatTy, 4);
	fragHelper->addVector("FragColor", floatTy, 4);
	fragHelper->startJIT();
	fragShaderImpl = (void(*)())(fragHelper->getFunction("main"));
}

int main(int argc, const char **argv)
{
	INIT_LLVM();
	prepareVert();
	prepareFrag();
	Image image(512, 512);
	std::vector<std::vector<float>> points = {
		{ 256, 69, 0 },
		{ 64, 443, 0 },
		{ 448, 443, 0 },
	};
	renderTriangle(image, getFragment(points));
	std::ofstream fout(outputPath, std::ios::binary);
	image.dump(fout);
	fout.close();
	return 0;
}

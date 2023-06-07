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

struct SharedInfo {
	std::vector<float> color;
	std::vector<float> texCoord;
	SharedInfo() {}
	SharedInfo(std::vector<float> color, std::vector<float> texCoord) : color(color), texCoord(texCoord) {}
};

std::pair<SharedInfo, std::vector<float>> vertShader(const std::vector<float> &point)
{
	vertHelper->setVector<float>("aPos", point);
	vertHelper->setVector<float>("color", { 0.0, 0.0, 0.0, 0.0 });
	vertHelper->setVector<float>("texCoord", { 0.0, 0.0, 0.0, 0.0 });
	vertHelper->setVector<float>("gl_Position", { 0.0, 0.0, 0.0, 0.0 });
	vertShaderImpl();
	return std::make_pair(
		SharedInfo(vertHelper->getVector<float>("color"), vertHelper->getVector<float>("texCoord")),
		vertHelper->getVector<float>("gl_Position")
	);
}

std::vector<float> fragShader(SharedInfo info)
{
	fragHelper->setVector<float>("color", info.color);
	fragHelper->setVector<float>("texCoord", info.texCoord);
	fragHelper->setVector<float>("FragColor", { 0.0, 0.0, 0.0, 0.0 });
	fragShaderImpl();
	return fragHelper->getVector<float>("FragColor");
}

struct Fragment {
	SharedInfo ainfo, binfo, cinfo;
	std::vector<float> aPos, bPos, cPos;
	float ax() const { return aPos[0]; }
	float ay() const { return aPos[1]; }
	float bx() const { return bPos[0]; }
	float by() const { return bPos[1]; }
	float cx() const { return cPos[0]; }
	float cy() const { return cPos[1]; }
};

Fragment getFragment(const std::vector<std::vector<float>> &points, unsigned offset = 0) {
	auto runOnPoint = [&points](unsigned index, SharedInfo &info, std::vector<float> &pos) {
		std::vector<float> v = points[index];
		while (v.size() < 3) v.push_back(0.0);
		auto res = vertShader(v);
		info = res.first;
		pos = res.second;
	};
	Fragment frag;
	runOnPoint(offset + 0, frag.ainfo, frag.aPos);
	runOnPoint(offset + 1, frag.binfo, frag.bPos);
	runOnPoint(offset + 2, frag.cinfo, frag.cPos);
	return frag;
}

float getarea(float ax, float ay, float bx, float by, float cx, float cy) {
	return 0.5f * abs((ax * (by - cy)) + (bx * (cy - ay)) + (cx * (ay - by)));
}

void renderPixel(Image &image, const Fragment &frag, int x, int y)
{
	std::vector<float> color, texCoord;
	auto tx = [&image](float x) { return (x + 1) / 2 * image.width(); };
	auto ty = [&image](float y) { return (y + 1) / 2 * image.height(); };
	float ax = tx(frag.ax()), ay = ty(frag.ay());
	float bx = tx(frag.bx()), by = ty(frag.by());
	float cx = tx(frag.cx()), cy = ty(frag.cy());
	float area = getarea(ax, ay, bx, by, cx, cy);
	if (area < 1e-8) {
		color = frag.ainfo.color;
		texCoord = frag.ainfo.texCoord;
	}
	else {
		float aw = getarea(x, y, bx, by, cx, cy);
		float bw = getarea(ax, ay, x, y, cx, cy);
		float cw = getarea(ax, ay, bx, by, x, y);
		float sum = aw + bw + cw;
		aw /= sum;
		bw /= sum;
		cw /= sum;
		for (int i = 0; i < 4; i++) {
			color.push_back(0.0);
			color[i] += aw * frag.ainfo.color[i];
			color[i] += bw * frag.binfo.color[i];
			color[i] += cw * frag.cinfo.color[i];
			texCoord.push_back(0.0);
			texCoord[i] += aw * frag.ainfo.texCoord[i];
			texCoord[i] += bw * frag.binfo.texCoord[i];
			texCoord[i] += cw * frag.cinfo.texCoord[i];
		}
	}
	color = fragShader(SharedInfo(color, texCoord));
	image.setPixel(x, y, color);
}

void renderLine(Image &image, const Fragment &frag, float l, float r, int y)
{
	if (l > r)
		std::swap(l, r);
	for (int i = std::max(0.0f, ceilf(l)); i < std::min(float(image.width() - 1), floorf(r)); i++)
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
	for (int i = ty; i < ly; i++) {
		renderLine(image, frag, curxl, curxr, i);
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
	for (int i = ly; i < ty; i++) {
		renderLine(image, frag, int(curxl + 0.5f), int(curxr + 0.5f), i);
		curxl += ldx;
		curxr += rdx;
	}
}

void renderTriangle(Image &image, const Fragment &frag)
{
	auto tx = [&image](float x) { return (x + 1) / 2 * image.width(); };
	auto ty = [&image](float y) { return (y + 1) / 2 * image.height(); };
	float ax = tx(frag.ax()), ay = ty(frag.ay());
	float bx = tx(frag.bx()), by = ty(frag.by());
	float cx = tx(frag.cx()), cy = ty(frag.cy());

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
		renderUpTriangle(image, frag, points[0].first, points[0].second, points[1].first, points[2].first, points[2].second);
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
	vertHelper->addVector("texCoord", floatTy, 4);
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
	fragHelper->addVector("texCoord", floatTy, 4);
	fragHelper->addVector("FragColor", floatTy, 4);
	fragHelper->startJIT();
	fragShaderImpl = (void(*)())(fragHelper->getFunction("main"));
}

void renderFineTriangle(Image &image, std::vector<std::vector<float>> points, int depth) {
	if (depth <= 0) {
		renderTriangle(image, getFragment(points));
	}
	else {
		const std::vector<float> &a = points[0];
		const std::vector<float> &b = points[1];
		const std::vector<float> &c = points[2];
		std::vector<float> ab = { (a[0] + b[0]) / 2, (a[1] + b[1]) / 2 };
		std::vector<float> ac = { (a[0] + c[0]) / 2, (a[1] + c[1]) / 2 };
		std::vector<float> bc = { (b[0] + c[0]) / 2, (b[1] + c[1]) / 2 };
		renderFineTriangle(image, { a, ab, ac }, depth - 1);
		renderFineTriangle(image, { b, ab, bc }, depth - 1);
		renderFineTriangle(image, { c, ac, bc }, depth - 1);
		renderFineTriangle(image, { ab, ac, bc }, depth - 1);
	}
}

int main(int argc, const char **argv)
{
	INIT_LLVM();
	prepareVert();
	prepareFrag();
	Image image(512, 512);
	std::vector<std::vector<float>> points = {
		{ 0, -0.4f * sqrtf(3), 0 },
		{ -0.8f, 0.4f * sqrtf(3), 0},
		{ 0.8f, 0.4f * sqrtf(3), 0}
	};
	renderTriangle(image, getFragment(points));
	// renderFineTriangle(image, points, 5);
	std::ofstream fout(outputPath, std::ios::binary);
	image.dump(fout);
	fout.close();
	return 0;
}

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
        if (x < 0 || x >= width())
            return false;
        if (y < 0 || y >= height())
            return false;
        if (color.size() < channel())
            return false;
        int base_loc = y * width() * channel() + x * channel();
        for (int i = 0; i < channel(); i++)
        {
            data[base_loc + i] = int(color[i] * 255);
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

void renderTriangle(Image &image)
{
    float xl = 64, xm = 256, xr = 448;
    int yu = 69, yd = 443;
    float curxl = xm, curxr = xm;
    float dx = (xm - xl) / (yd - yu);
    for (int i = yu; i <= yd; i++) {
        renderLine(image, int(curxl + 0.5f), int(curxr + 0.5f), i);
        curxl -= dx;
        curxr += dx;
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
    renderTriangle(image);
    std::ofstream fout("result.ppm", std::ios::binary);
    image.dump(fout);
    fout.close();
    return 0;
}

# CppRenderPipeline

## 简体中文

这是一个简单的渲染管线，可以（且只可以）渲染一个三角形。

此渲染管线加载 LLVM IR 作为顶点着色器和片段着色器。

### 构建

此渲染管线基于 [LLVM](https://github.com/llvm/llvm-project).

```bash
cmake -B build -DLLVM_DIR="<path-to-llvm-build>/lib/cmake/llvm"
cmake --build build
```

### 使用方式

``` bash
CppRenderPipeline -vert="<顶点着色器位置>" -frag="<片段着色器位置>" -o "<输出位置>"
```

### 例子

在每个文件夹下，均有 5 个文件组成的一个例子，其中：

- `test.vert` 和 `test.frag` 是一对示例着色器。

- `test.vert.ll` 和 `test.frag.ll` 由上述着色器编译得到。
- `result.ppm` 由上述着色器渲染得到。

将工作目录设为 `<项目文件夹>/build/bin` 或 `<项目文件夹>/build/Debug` 后，你可以使用以下指令渲染得到一个橙色的三角形。对其他例子，可以使用相似的指令进行渲染。

```shell
CppRenderPipeline -vert="../../test/easy/code.vert.ll" -frag="../../test/easy/code.frag.ll" -o "result.ppm"
```

渲染得到的 ppm 图像可以使用 [OpenSeeIt](https://sourceforge.net/projects/openseeit/) 打开。

### 注意事项

在顶点着色器中，仅支持以下变量：

```glsl
in vec3 aPos;		// 顶点的三维坐标，其中z坐标未被使用
out vec4 color;		// 传递给片段着色器的中间变量
out vec4 texCoord;	// 传递给片段着色器的中间变量
```

在片段着色器中，仅支持以下变量:

```glsl
in vec4 color;		// 由顶点着色器传递的中间变量
in vec4 texCoord;	// 由顶点着色器传递的中间变量
out vec4 FragColor;	// 输出片元的颜色
```

内置变量中，仅支持 `gl_Position`.

## English

A simple pipeline which can (and can only) render a triangle.

This pipeline loads LLVM IR files as vertex shader and fragment shader.

### Building

This tool relies on [LLVM](https://github.com/llvm/llvm-project) binaries.

```bash
cmake -B build -DLLVM_DIR="<path-to-llvm-build>/lib/cmake/llvm"
cmake --build build
```

### Usage

``` bash
CppRenderPipeline -vert="<path-to-vertex-shader>" -frag="<path-to-fragment-shader>" -o "<output-path>"
```

### Example

Each folder in `test` folder contains an example consisting of 5 files.

- `test/test.vert` and `test/test.frag` are a pair of example shaders.

- `test/test.vert.ll` and `test/test.frag.ll` are compiled from the above shaders.
- `result.ppm` are rendered using the above shaders.

You can render an orange triangle by executing the following command, assuming current working directory is `<project-dir>/build/bin` or `<project-dir>/build/Debug`. Other examples can be rendered by similar ways.

```shell
CppRenderPipeline -vert="../../test/easy/code.vert.ll" -frag="../../test/easy/code.frag.ll" -o "result.ppm"
```

You can use [OpenSeeIt](https://sourceforge.net/projects/openseeit/) to view the rendered picture.

### Note

Only following variables are supported in vertex shader:

```glsl
in vec3 aPos;		// 3D position of vertices, in which z component is unused.
out vec4 color;		// Imtermediate variable passed to fragment shader.
out vec4 texCoord;	// Imtermediate variable passed to fragment shader.
```

Only following variables are supported in fragment shader:

```glsl
in vec4 color;		// Imtermediate variable passed to vertex shader.
in vec4 texCoord;	// Imtermediate variable passed to vertex shader.
out vec4 FragColor;	// Color of this fragment.
```

Only `gl_Position` is available as a builtin variable.
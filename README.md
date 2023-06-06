# CppRenderPipeline

A simple pipeline which can (and can only) render a triangle.

This pipeline loads LLVM IR files as vertex shader and fragment shader.

## Building

This tool relies on [LLVM](https://github.com/llvm/llvm-project) binaries.

```bash
cmake -B build -DLLVM_DIR="<path-to-llvm-build>/lib/cmake/llvm"
cmake --build build
```

## Usage

``` bash
./CppRenderPipeline -vert="<path-to-vertex-shader>" -frag="<path-to-fragment-shader>" -o "<output-path>"
```


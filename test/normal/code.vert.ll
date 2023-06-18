
@gl_Position = external global <4 x float>
@aPos = external global <3 x float>
@color = external global <4 x float>

define void @main() {
entry:
  %0 = alloca <4 x float>, align 16
  %1 = load <4 x float>, ptr %0, align 4
  %2 = shufflevector <4 x float> %1, <4 x float> <float 1.000000e+00, float undef, float undef, float undef>, <4 x i32> <i32 0, i32 1, i32 2, i32 4>
  store <4 x float> %2, ptr %0, align 4
  %3 = alloca <3 x float>, align 16
  %4 = load <3 x float>, ptr @aPos, align 16
  store <3 x float> %4, ptr %3, align 16
  %5 = load <3 x float>, ptr @aPos, align 4
  %6 = extractelement <3 x float> %5, i64 1
  %7 = insertelement <1 x float> undef, float %6, i64 0
  %8 = extractelement <1 x float> %7, i64 0
  %9 = load <4 x float>, ptr %0, align 4
  %10 = insertelement <4 x float> undef, float %8, i64 0
  %11 = shufflevector <4 x float> %9, <4 x float> %10, <4 x i32> <i32 4, i32 1, i32 2, i32 3>
  store <4 x float> %11, ptr %0, align 4
  %12 = alloca <3 x float>, align 16
  %13 = load <3 x float>, ptr @aPos, align 16
  store <3 x float> %13, ptr %12, align 16
  %14 = load <3 x float>, ptr @aPos, align 4
  %15 = extractelement <3 x float> %14, i64 0
  %16 = insertelement <1 x float> undef, float %15, i64 0
  %17 = extractelement <1 x float> %16, i64 0
  %18 = load <4 x float>, ptr %0, align 4
  %19 = insertelement <4 x float> undef, float %17, i64 0
  %20 = shufflevector <4 x float> %18, <4 x float> %19, <4 x i32> <i32 0, i32 4, i32 2, i32 3>
  store <4 x float> %20, ptr %0, align 4
  %21 = alloca <3 x float>, align 16
  %22 = load <3 x float>, ptr @aPos, align 16
  store <3 x float> %22, ptr %21, align 16
  %23 = load <3 x float>, ptr @aPos, align 4
  %24 = extractelement <3 x float> %23, i64 2
  %25 = insertelement <1 x float> undef, float %24, i64 0
  %26 = extractelement <1 x float> %25, i64 0
  %27 = load <4 x float>, ptr %0, align 4
  %28 = insertelement <4 x float> undef, float %26, i64 0
  %29 = shufflevector <4 x float> %27, <4 x float> %28, <4 x i32> <i32 0, i32 1, i32 4, i32 3>
  store <4 x float> %29, ptr %0, align 4
  %30 = load <4 x float>, ptr %0, align 4
  %31 = alloca <4 x float>, align 16
  store <4 x float> %30, ptr %31, align 4
  %32 = load <4 x float>, ptr %31, align 16
  store <4 x float> %32, ptr @gl_Position, align 16
  %33 = alloca <4 x float>, align 16
  %34 = load <4 x float>, ptr %33, align 4
  %35 = shufflevector <4 x float> %34, <4 x float> <float 1.000000e+00, float undef, float undef, float undef>, <4 x i32> <i32 0, i32 1, i32 2, i32 4>
  store <4 x float> %35, ptr %33, align 4
  %36 = alloca <3 x float>, align 16
  %37 = load <3 x float>, ptr @aPos, align 16
  store <3 x float> %37, ptr %36, align 16
  %38 = load <3 x float>, ptr @aPos, align 4
  %39 = extractelement <3 x float> %38, i64 0
  %40 = fadd float %39, 1.000000e+00
  %41 = fdiv float %40, 2.000000e+00
  %42 = insertelement <1 x float> undef, float %41, i64 0
  %43 = extractelement <1 x float> %42, i64 0
  %44 = load <4 x float>, ptr %33, align 4
  %45 = insertelement <4 x float> undef, float %43, i64 0
  %46 = shufflevector <4 x float> %44, <4 x float> %45, <4 x i32> <i32 4, i32 1, i32 2, i32 3>
  store <4 x float> %46, ptr %33, align 4
  %47 = alloca <3 x float>, align 16
  %48 = load <3 x float>, ptr @aPos, align 16
  store <3 x float> %48, ptr %47, align 16
  %49 = load <3 x float>, ptr @aPos, align 4
  %50 = extractelement <3 x float> %49, i64 1
  %51 = fadd float %50, 1.000000e+00
  %52 = fdiv float %51, 2.000000e+00
  %53 = insertelement <1 x float> undef, float %52, i64 0
  %54 = extractelement <1 x float> %53, i64 0
  %55 = load <4 x float>, ptr %33, align 4
  %56 = insertelement <4 x float> undef, float %54, i64 0
  %57 = shufflevector <4 x float> %55, <4 x float> %56, <4 x i32> <i32 0, i32 4, i32 2, i32 3>
  store <4 x float> %57, ptr %33, align 4
  %58 = alloca <3 x float>, align 16
  %59 = load <3 x float>, ptr @aPos, align 16
  store <3 x float> %59, ptr %58, align 16
  %60 = load <3 x float>, ptr @aPos, align 4
  %61 = extractelement <3 x float> %60, i64 2
  %62 = fadd float %61, 1.000000e+00
  %63 = fdiv float %62, 2.000000e+00
  %64 = insertelement <1 x float> undef, float %63, i64 0
  %65 = extractelement <1 x float> %64, i64 0
  %66 = load <4 x float>, ptr %33, align 4
  %67 = insertelement <4 x float> undef, float %65, i64 0
  %68 = shufflevector <4 x float> %66, <4 x float> %67, <4 x i32> <i32 0, i32 1, i32 4, i32 3>
  store <4 x float> %68, ptr %33, align 4
  %69 = load <4 x float>, ptr %33, align 4
  %70 = alloca <4 x float>, align 16
  store <4 x float> %69, ptr %70, align 4
  %71 = load <4 x float>, ptr %70, align 16
  store <4 x float> %71, ptr @color, align 16
  ret void
}


@gl_Position = external global <4 x float>
@aPos = external global <3 x float>
@color = external global <4 x float>
@texCoord = external global <4 x float>

define void @main() {
entry:
  %0 = alloca <3 x float>, align 16
  %1 = alloca <3 x float>, align 16
  %2 = alloca <3 x float>, align 16
  %3 = load <3 x float>, ptr @aPos, align 16
  store <3 x float> %3, ptr %2, align 16
  %4 = load <3 x float>, ptr @aPos, align 4
  %5 = fadd <3 x float> %4, <float 1.000000e+00, float 1.000000e+00, float 1.000000e+00>
  %6 = fdiv <3 x float> %5, <float 2.000000e+00, float 2.000000e+00, float 2.000000e+00>
  store <3 x float> %6, ptr %1, align 4
  %7 = load <3 x float>, ptr %1, align 4
  store <3 x float> %7, ptr %0, align 4
  %8 = alloca <4 x float>, align 16
  %9 = load <4 x float>, ptr %8, align 4
  %10 = shufflevector <4 x float> %9, <4 x float> <float 1.000000e+00, float undef, float undef, float undef>, <4 x i32> <i32 0, i32 1, i32 2, i32 4>
  store <4 x float> %10, ptr %8, align 4
  %11 = load <3 x float>, ptr %0, align 4
  %12 = extractelement <3 x float> %11, i64 0
  %13 = insertelement <1 x float> undef, float %12, i64 0
  %14 = extractelement <1 x float> %13, i64 0
  %15 = load <4 x float>, ptr %8, align 4
  %16 = insertelement <4 x float> undef, float %14, i64 0
  %17 = shufflevector <4 x float> %15, <4 x float> %16, <4 x i32> <i32 4, i32 1, i32 2, i32 3>
  store <4 x float> %17, ptr %8, align 4
  %18 = load <3 x float>, ptr %0, align 4
  %19 = extractelement <3 x float> %18, i64 1
  %20 = insertelement <1 x float> undef, float %19, i64 0
  %21 = extractelement <1 x float> %20, i64 0
  %22 = load <4 x float>, ptr %8, align 4
  %23 = insertelement <4 x float> undef, float %21, i64 0
  %24 = shufflevector <4 x float> %22, <4 x float> %23, <4 x i32> <i32 0, i32 4, i32 2, i32 3>
  store <4 x float> %24, ptr %8, align 4
  %25 = load <3 x float>, ptr %0, align 4
  %26 = extractelement <3 x float> %25, i64 0
  %27 = load <3 x float>, ptr %0, align 4
  %28 = extractelement <3 x float> %27, i64 1
  %29 = fadd float %26, %28
  %30 = fdiv float %29, 2.000000e+00
  %31 = insertelement <1 x float> undef, float %30, i64 0
  %32 = extractelement <1 x float> %31, i64 0
  %33 = load <4 x float>, ptr %8, align 4
  %34 = insertelement <4 x float> undef, float %32, i64 0
  %35 = shufflevector <4 x float> %33, <4 x float> %34, <4 x i32> <i32 0, i32 1, i32 4, i32 3>
  store <4 x float> %35, ptr %8, align 4
  %36 = load <4 x float>, ptr %8, align 4
  %37 = alloca <4 x float>, align 16
  store <4 x float> %36, ptr %37, align 4
  %38 = load <4 x float>, ptr %37, align 16
  store <4 x float> %38, ptr @color, align 16
  %39 = alloca <4 x float>, align 16
  %40 = load <4 x float>, ptr %39, align 4
  %41 = shufflevector <4 x float> %40, <4 x float> <float 1.000000e+00, float undef, float undef, float undef>, <4 x i32> <i32 0, i32 1, i32 2, i32 4>
  store <4 x float> %41, ptr %39, align 4
  %42 = alloca <3 x float>, align 16
  %43 = load <3 x float>, ptr @aPos, align 16
  store <3 x float> %43, ptr %42, align 16
  %44 = load <3 x float>, ptr @aPos, align 4
  %45 = extractelement <3 x float> %44, i64 0
  %46 = insertelement <1 x float> undef, float %45, i64 0
  %47 = extractelement <1 x float> %46, i64 0
  %48 = load <4 x float>, ptr %39, align 4
  %49 = insertelement <4 x float> undef, float %47, i64 0
  %50 = shufflevector <4 x float> %48, <4 x float> %49, <4 x i32> <i32 4, i32 1, i32 2, i32 3>
  store <4 x float> %50, ptr %39, align 4
  %51 = alloca <3 x float>, align 16
  %52 = load <3 x float>, ptr @aPos, align 16
  store <3 x float> %52, ptr %51, align 16
  %53 = load <3 x float>, ptr @aPos, align 4
  %54 = extractelement <3 x float> %53, i64 1
  %55 = insertelement <1 x float> undef, float %54, i64 0
  %56 = extractelement <1 x float> %55, i64 0
  %57 = load <4 x float>, ptr %39, align 4
  %58 = insertelement <4 x float> undef, float %56, i64 0
  %59 = shufflevector <4 x float> %57, <4 x float> %58, <4 x i32> <i32 0, i32 4, i32 2, i32 3>
  store <4 x float> %59, ptr %39, align 4
  %60 = alloca <3 x float>, align 16
  %61 = load <3 x float>, ptr @aPos, align 16
  store <3 x float> %61, ptr %60, align 16
  %62 = load <3 x float>, ptr @aPos, align 4
  %63 = extractelement <3 x float> %62, i64 2
  %64 = insertelement <1 x float> undef, float %63, i64 0
  %65 = extractelement <1 x float> %64, i64 0
  %66 = load <4 x float>, ptr %39, align 4
  %67 = insertelement <4 x float> undef, float %65, i64 0
  %68 = shufflevector <4 x float> %66, <4 x float> %67, <4 x i32> <i32 0, i32 1, i32 4, i32 3>
  store <4 x float> %68, ptr %39, align 4
  %69 = load <4 x float>, ptr %39, align 4
  %70 = alloca <4 x float>, align 16
  store <4 x float> %69, ptr %70, align 4
  %71 = load <4 x float>, ptr %70, align 16
  store <4 x float> %71, ptr @gl_Position, align 16
  %72 = alloca <4 x float>, align 16
  %73 = load <4 x float>, ptr @gl_Position, align 16
  store <4 x float> %73, ptr %72, align 16
  %74 = load <4 x float>, ptr @gl_Position, align 4
  %75 = alloca <4 x float>, align 16
  store <4 x float> %74, ptr %75, align 4
  %76 = load <4 x float>, ptr %75, align 16
  store <4 x float> %76, ptr @texCoord, align 16
  ret void
}

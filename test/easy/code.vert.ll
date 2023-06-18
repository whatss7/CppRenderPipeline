
@gl_Position = external global <4 x float>
@aPos = external global <3 x float>

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
  %6 = extractelement <3 x float> %5, i64 0
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
  %15 = extractelement <3 x float> %14, i64 1
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
  ret void
}

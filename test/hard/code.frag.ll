
@color = external global <4 x float>
@texCoord = external global <4 x float>
@FragColor = external global <4 x float>

define void @main() {
entry:
  %0 = alloca <4 x float>, align 16
  %1 = alloca <4 x float>, align 16
  %2 = alloca <4 x float>, align 16
  %3 = load <4 x float>, ptr @color, align 16
  store <4 x float> %3, ptr %2, align 16
  %4 = load <4 x float>, ptr @color, align 4
  store <4 x float> %4, ptr %1, align 4
  %5 = load <4 x float>, ptr %1, align 4
  store <4 x float> %5, ptr %0, align 4
  %6 = alloca <4 x float>, align 16
  %7 = load <4 x float>, ptr @texCoord, align 16
  store <4 x float> %7, ptr %6, align 16
  %8 = load <4 x float>, ptr @texCoord, align 4
  %9 = extractelement <4 x float> %8, i64 0
  %10 = fcmp olt float 0.000000e+00, %9
  br i1 %10, label %if.then, label %if.else

if.then:                                          ; preds = %entry
  %11 = alloca <4 x float>, align 16
  %12 = load <4 x float>, ptr %0, align 16
  store <4 x float> %12, ptr %11, align 16
  %13 = load <4 x float>, ptr %0, align 4
  %14 = extractelement <4 x float> %13, i64 0
  %15 = insertelement <1 x float> undef, float %14, i64 0
  %16 = shufflevector <1 x float> %15, <1 x float> undef, <4 x i32> zeroinitializer
  %17 = extractelement <4 x float> %16, i64 2
  %18 = alloca <4 x float>, align 16
  %19 = load <4 x float>, ptr %0, align 16
  store <4 x float> %19, ptr %18, align 16
  %20 = load <4 x float>, ptr %0, align 4
  %21 = insertelement <4 x float> undef, float %17, i64 0
  %22 = shufflevector <4 x float> %20, <4 x float> %21, <4 x i32> <i32 0, i32 1, i32 4, i32 3>
  %23 = alloca <4 x float>, align 16
  store <4 x float> %22, ptr %23, align 4
  %24 = load <4 x float>, ptr %23, align 16
  store <4 x float> %24, ptr %0, align 16
  br label %if.end

if.else:                                          ; preds = %entry
  %25 = alloca <4 x float>, align 16
  %26 = load <4 x float>, ptr %0, align 16
  store <4 x float> %26, ptr %25, align 16
  %27 = load <4 x float>, ptr %0, align 4
  %28 = extractelement <4 x float> %27, i64 1
  %29 = insertelement <1 x float> undef, float %28, i64 0
  %30 = shufflevector <1 x float> %29, <1 x float> undef, <4 x i32> zeroinitializer
  %31 = extractelement <4 x float> %30, i64 2
  %32 = alloca <4 x float>, align 16
  %33 = load <4 x float>, ptr %0, align 16
  store <4 x float> %33, ptr %32, align 16
  %34 = load <4 x float>, ptr %0, align 4
  %35 = insertelement <4 x float> undef, float %31, i64 0
  %36 = shufflevector <4 x float> %34, <4 x float> %35, <4 x i32> <i32 0, i32 1, i32 4, i32 3>
  %37 = alloca <4 x float>, align 16
  store <4 x float> %36, ptr %37, align 4
  %38 = load <4 x float>, ptr %37, align 16
  store <4 x float> %38, ptr %0, align 16
  br label %if.end

if.end:                                           ; preds = %if.else, %if.then
  %39 = load <4 x float>, ptr %0, align 4
  %40 = alloca <4 x float>, align 16
  store <4 x float> %39, ptr %40, align 4
  %41 = load <4 x float>, ptr %40, align 16
  store <4 x float> %41, ptr @FragColor, align 16
  ret void
}

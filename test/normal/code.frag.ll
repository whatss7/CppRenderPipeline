
@color = external global <4 x float>
@FragColor = external global <4 x float>

define void @main() {
entry:
  %0 = alloca <4 x float>, align 16
  %1 = load <4 x float>, ptr @color, align 16
  store <4 x float> %1, ptr %0, align 16
  %2 = load <4 x float>, ptr @color, align 4
  %3 = alloca <4 x float>, align 16
  store <4 x float> %2, ptr %3, align 4
  %4 = load <4 x float>, ptr %3, align 16
  store <4 x float> %4, ptr @FragColor, align 16
  ret void
}

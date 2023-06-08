
@FragColor = external global <4 x float>

define void @main() {
entry:
  %0 = alloca <4 x float>, align 16
  store <4 x float> <float 1.000000e+00, float 5.000000e-01, float 0x3FC99999A0000000, float 1.000000e+00>, <4 x float>* %0, align 4
  %1 = load <4 x float>, <4 x float>* %0, align 16
  store <4 x float> %1, <4 x float>* @FragColor, align 16
  ret void
}

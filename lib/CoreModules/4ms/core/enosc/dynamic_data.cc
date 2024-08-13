#include "dynamic_data.hh"

namespace EnOsc
{

using namespace easiglib;

/* triangles */
Buffer<Buffer<s8_0, 9>, 8> triangles_12ths = {{{
  {{{-12._s8_0, -9._s8_0, -6._s8_0, -3._s8_0, 0._s8_0, 3._s8_0, 6._s8_0, 9._s8_0, 12._s8_0, }}},
  {{{-12._s8_0, -12._s8_0, -8._s8_0, -4._s8_0, 0._s8_0, 4._s8_0, 8._s8_0, 12._s8_0, 12._s8_0, }}},
  {{{-12._s8_0, -12._s8_0, -12._s8_0, -6._s8_0, 0._s8_0, 6._s8_0, 12._s8_0, 12._s8_0, 12._s8_0, }}},
  {{{-12._s8_0, -12._s8_0, -12._s8_0, -12._s8_0, 0._s8_0, 12._s8_0, 12._s8_0, 12._s8_0, 12._s8_0, }}},
  {{{-12._s8_0, -6._s8_0, -12._s8_0, -6._s8_0, 0._s8_0, 6._s8_0, 12._s8_0, 6._s8_0, 12._s8_0, }}},
  {{{-12._s8_0, -6._s8_0, -0._s8_0, -12._s8_0, 0._s8_0, 12._s8_0, 0._s8_0, 6._s8_0, 12._s8_0, }}}, 
  {{{-12._s8_0, -6._s8_0, 12._s8_0, -12._s8_0, 0._s8_0, 12._s8_0, -12._s8_0, 6._s8_0, 12._s8_0, }}},
  {{{12._s8_0, -12._s8_0, 12._s8_0, -12._s8_0, 0._s8_0, 12._s8_0, -12._s8_0, 12._s8_0, -12._s8_0, }}},
}}};


Buffer<std::pair<s1_15, s1_15>, sine_size> DynamicData::sine;
Buffer<Buffer<f, cheby_size>, cheby_tables> DynamicData::cheby;
Buffer<std::pair<f, f>, fold_size> DynamicData::fold;
Buffer<f, (fold_size-1)/2 + 1> DynamicData::fold_max;
Buffer<Buffer<f, 9>, 8> DynamicData::triangles;

DynamicData::DynamicData() {

  // sine + difference
  { easiglib::MagicSine magic(1_f / f(sine_size-1));
    s1_15 previous = s1_15::inclusive(magic.Process());
    for (auto& [v, d] : sine) {
      v = previous;
      previous = s1_15::inclusive(magic.Process());
      d = previous - v;
    }
  }

  // cheby[1] = [-1..1]
  for (int i=0; i<cheby_size; i++)
    cheby[0][i] = f(i * 2)/f(cheby_size-1) - 1_f;

  // cheby[2] = 2 * cheby[1] * cheby[1] - 1
  for (int i=0; i<cheby_size; i++)
    cheby[1][i] = 2_f * cheby[0][i] * cheby[0][i] - 1_f;

  // cheby[n] = 2 * cheby[1] * cheby[n-1] - cheby[n-2]
  for (int n=2; n<cheby_tables; n++)
    for (int i=0; i<cheby_size; i++)
      cheby[n][i] = 2_f * cheby[0][i] * cheby[n-1][i] - cheby[n-2][i];

  // fold
  { f folds = 6_f;
    f previous = 0_f;
    for (int i=0; i<fold_size; ++i) {
      // TODO: this -3 make the wavefolding curve symmetrical; why?
      f x = f(i) / f(fold_size-3); // 0..1
      x = folds * (2_f * x - 1_f); // -folds..folds
      f g = 1_f / (1_f + x.abs()); // 0..0.5
      f p = 16_f / (2_f * Math::pi) * x * g;
      while(p > 1_f) p--;
      while(p < 0_f) p++;
      x = - g * (x + Math::fast_sine(p));
      fold[i] = i==0 ? std::pair(x, 0_f) : std::pair(previous, x - previous);
      previous = x;
    }
  }

  // fold_max
  { f max = 0_f;
    int start = (fold_size-1) / 2;
    for (int i=0; i<fold_max.size(); ++i) {
      max = fold[i+start].first.abs().max(max);
      // the attenuation factor accounts for interpolation error, so
      // we don't overestimate the 1/x curve and amplify to clipping
      fold_max[i] = 0.92_f / (max + 0.00001_f);
    }
  }

  { //60 bytes of code
    for (int i=0; i<8; i++)
      for (int j=0; j<9; j++)
        triangles[i][j] = (f)(triangles_12ths[i][j])/12.0_f;
  }
}

}

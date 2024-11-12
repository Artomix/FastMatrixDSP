#include "./projects/FastThreeBandEQ.h"

iplug::sample FastThreeBandEQ::ProcessBlock(iplug::sample sample, double low, double mid, double high, double samplerate)
{
  if (sample == 0) {

    f1p0 = 0;
    f1p1 = 0;
    f1p2 = 0;
    f1p3 = 0;

    f2p0 = 0;
    f2p1 = 0;
    f2p2 = 0;
    f2p3 = 0;
  }

  lg = low;
  mg = mid;
  hg = high;

  const double lowfreq = 800;
  const double highfreq = 5000;

  lf = 2 * sin(M_PI * ((double)lowfreq / (double)samplerate));
  hf = 2 * sin(M_PI * ((double)highfreq / (double)samplerate));

  double l, m, h;    

  // LP

  f1p0 += (lf * (sample - f1p0));
  f1p1 += (lf * (f1p0 - f1p1));
  f1p2 += (lf * (f1p1 - f1p2));
  f1p3 += (lf * (f1p2 - f1p3));

  l = f1p3;

  // HP

  f2p0 += (hf * (sample - f2p0));
  f2p1 += (hf * (f2p0 - f2p1));
  f2p2 += (hf * (f2p1 - f2p2));
  f2p3 += (hf * (f2p2 - f2p3));

  h = sdm3 - f2p3;

  // MID

  m = sdm3 - (h + l);

  l *= lg;
  m *= mg;
  h *= hg;

  // Buffer

  sdm3 = sdm2;
  sdm2 = sdm1;
  sdm1 = sample;

  return(l + m + h);
}
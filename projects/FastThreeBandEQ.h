#pragma once

# define M_PI 3.14159265358979323846
#include "IPlug_include_in_plug_hdr.h"
#include <vcruntime_string.h>
#include <math.h>

class FastThreeBandEQ {
public:

  // Low band

  double  lf;       // Frequency
  double  f1p0;     // Poles
  double  f1p1;
  double  f1p2;
  double  f1p3;

  // High band

  double  hf;       // Frequency
  double  f2p0;     // Poles
  double  f2p1;
  double  f2p2;
  double  f2p3;

  double  sdm1;     // History
  double  sdm2;    
  double  sdm3;     

  double  lg;       // low  gain
  double  mg;       // mid  gain
  double  hg;       // high gain

  double ProcessBlock(iplug::sample sample, double low, double mid, double high, double samplerate);
};

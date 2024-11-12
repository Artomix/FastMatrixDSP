#pragma once

#include "./projects/FastLPFilter.h"
#include "./projects/FastHPFilter.h"
#include "./projects/FastDist.h"
#include "./projects/FastThreeBandEQ.h"
#include "IPlug_include_in_plug_hdr.h"
#include "IControls.h"

const int kNumPresets = 1;

enum EParams
{
  k_LPF_CUT = 0,
  k_LPF_TYPE = 1,
  k_LPF_EXTRA = 2,

  k_HPF_CUT = 3,
  k_HPF_TYPE = 4,
  k_HPF_EXTRA = 5,

  k_DIST_EXTRA = 6,
  k_DIST_TYPE = 7,

  k_3BAND_L = 8,
  k_3BAND_M = 9,
  k_3BAND_H = 10,

  kNumParams
};

using namespace iplug;
using namespace igraphics;

class FastMatrix final : public Plugin
{

private:
  bool setupComplete = false;
  FastLPFilter* ff_main_lp = new FastLPFilter();
  FastHPFilter* ff_main_hp = new FastHPFilter();
  FastDist* fd_main = new FastDist();
  FastThreeBandEQ* band_r = new FastThreeBandEQ();
  FastThreeBandEQ* band_l = new FastThreeBandEQ();

public:
  FastMatrix(const InstanceInfo& info);

#if IPLUG_DSP // http://bit.ly/2S64BDd
  void ProcessBlock(sample** inputs, sample** outputs, int nFrames) override;
#endif
};

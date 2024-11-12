#pragma once
#include "IPlug_include_in_plug_hdr.h"

class FastDist {
public:
  enum Mode {
    Off,
    HardClipping,
    QFilter,
    SoftClipping,
    WaveFolding,
    BitCrushing,
    HighPassFilter,
  };

  void ProcessBlock(iplug::sample** inputs, iplug::sample** outputs, int nFrames, float sampleRate, int mode, float extraparam);

private:
  inline iplug::sample HardClip(iplug::sample sample, float threshold);
  inline iplug::sample QCompression(iplug::sample sample, float threshold);
  inline iplug::sample SoftClip(iplug::sample sample, float threshold);
  inline iplug::sample WaveFold(iplug::sample sample, float foldThreshold);
  inline iplug::sample BitCrush(iplug::sample sample, float bitDepth);
};

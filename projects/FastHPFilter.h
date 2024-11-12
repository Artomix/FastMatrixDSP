
#include <IPlugConstants.h>
#pragma once

# define M_PI 3.14159265358979323846
# define M_PI_34 2.35619449019

class FastHPFilter {
public:
  void ProcessBlock(iplug::sample** inputs, iplug::sample** outputs, int nFrames, float cutoffFreq, float sampleRate, int mode, float extraparam);

private:
  static constexpr int numChannels = 2; 

  // Temps
  float lastOutputs_0[numChannels];
  float lastInputs_0[numChannels];
  float lastOutputs_2[numChannels][2];
  float lastInputs_2[numChannels][2];
};

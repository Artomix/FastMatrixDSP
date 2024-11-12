#include <vector>
#include "FastHPFilter.h"

void FastHPFilter::ProcessBlock(iplug::sample** inputs, iplug::sample** outputs, int nFrames, float cutoffFreq, float sampleRate, int mode, float extraparam) {

  const float velocity = cutoffFreq * cutoffFreq;

  switch (mode)
  {
  case 0: // Single Smooth
  {
    const float RC = 1.0f / (2.0f * M_PI * (velocity * sampleRate));
    const float dt = 1.0f / sampleRate;
    const float alpha = RC / (RC + dt);

    for (int s = 0; s < nFrames; s++) {
      for (int c = 0; c < 2; c++) {
        outputs[c][s] = alpha * (lastOutputs_0[c] + inputs[c][s] - lastInputs_0[c]);

        if (inputs[c][s] == 0.0) {
          lastInputs_0[c] = 0;
          lastOutputs_0[c] = 0;
        }
        else
        {
          lastInputs_0[c] = inputs[c][s];
          lastOutputs_0[c] = outputs[c][s]; 
        }
      }
    }
  }
  break;

  case 1:  // Bi-quad filter
  {
    const int Q = 1 + (extraparam * M_PI);

    const float w0 = M_PI_34 * velocity;
    const float cosW0 = cos(w0);
    const float sinW0 = sin(w0);
    const float alpha = sinW0 / (2.0f * Q); 

    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cosW0;
    float a2 = 1.0f - alpha;
    float b0 = (1.0f + cosW0) / 2.0f;
    float b1 = -(1.0f + cosW0);
    float b2 = (1.0f + cosW0) / 2.0f;

    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
    a1 /= a0;
    a2 /= a0;

    for (int s = 0; s < nFrames; s++) {
      for (int c = 0; c < 2; c++) {
        float inputSample = inputs[c][s];
        float outputSample = b0 * inputSample + b1 * lastInputs_2[c][0] + b2 * lastInputs_2[c][1]
          - a1 * lastOutputs_2[c][0] - a2 * lastOutputs_2[c][1];

        outputs[c][s] = outputSample;

        if (inputs[c][s] == 0.0) {
          lastInputs_2[c][1] = 0;
          lastInputs_2[c][0] = 0;
          lastOutputs_2[c][1] = 0;
          lastOutputs_2[c][0] = 0;
        }
        else {
          lastInputs_2[c][1] = lastInputs_2[c][0];
          lastInputs_2[c][0] = inputSample;
          lastOutputs_2[c][1] = lastOutputs_2[c][0];
          lastOutputs_2[c][0] = outputSample;
        }
      }
    }
  }
  break;
  }
}

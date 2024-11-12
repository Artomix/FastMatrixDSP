#include "FastDist.h"
#include <cmath>
#include <algorithm>

void FastDist::ProcessBlock(iplug::sample** inputs, iplug::sample** outputs, int nFrames, float sampleRate, int mode, float extraparam) {
  switch (mode) {
    case Off: {
      for (int frame = 0; frame < nFrames; frame++) {
        outputs[0][frame] = inputs[0][frame];
        outputs[1][frame] = inputs[1][frame];
      }
      break;
    }

    case HardClipping:
      for (int frame = 0; frame < nFrames; frame++) {
        outputs[0][frame] = HardClip(inputs[0][frame], extraparam);
        outputs[1][frame] = HardClip(inputs[1][frame], extraparam);
      }
      break;

    case QFilter:
      for (int frame = 0; frame < nFrames; frame++) {
        outputs[0][frame] = QCompression(inputs[0][frame], extraparam);
        outputs[1][frame] = QCompression(inputs[1][frame], extraparam);
      }
      break;

    case SoftClipping:
      for (int frame = 0; frame < nFrames; frame++) {
        outputs[0][frame] = SoftClip(inputs[0][frame], extraparam);
        outputs[1][frame] = SoftClip(inputs[1][frame], extraparam);
      }
      break;

    case WaveFolding:
      for (int frame = 0; frame < nFrames; frame++) {
        outputs[0][frame] = WaveFold(inputs[0][frame], extraparam);
        outputs[1][frame] = WaveFold(inputs[1][frame], extraparam);
      }
      break;

    case BitCrushing:
      for (int frame = 0; frame < nFrames; frame++) {
        outputs[0][frame] = BitCrush(inputs[0][frame], extraparam);
        outputs[1][frame] = BitCrush(inputs[1][frame], extraparam);
      }
      break;

    default:
      break;
  }
}

inline iplug::sample FastDist::HardClip(iplug::sample sample, float threshold) {
  if (sample > threshold) {
    return threshold;
  }
  else if (sample < -threshold) {
    return -threshold;
  }
  else {
    return sample;
  }
}

inline iplug::sample FastDist::QCompression(iplug::sample sample, float threshold) {
  long i;
  float x2, y;
  const float threehalfs = 1.5F;

  x2 = sample * 0.5F;
  y = static_cast<float>(sample);
  i = *(long*)&y;
  i = 0x5f3759df - (i >> 1);
  y = *(float*)&i;
  y = y * (threehalfs - (x2 * y * y));

  if (y < -threshold) {
    return -threshold;
  }
  if (y > threshold) {
    return threshold;
  }

  return y;
}

inline iplug::sample FastDist::SoftClip(iplug::sample sample, float threshold) {
  if (sample > threshold)
    return HardClip(threshold - (sample - threshold) / (sample + 1.0f), threshold * 1.2);
  else if (sample < -threshold)
    return HardClip(-threshold - (sample + threshold) / (-sample + 1.0f), threshold * 1.2);
  else
    return HardClip(sample, threshold * 1.2);
}

inline iplug::sample FastDist::WaveFold(iplug::sample sample, float foldThreshold) {
  if (sample > foldThreshold) {
    sample = foldThreshold - (sample - foldThreshold);
  }
  if (sample < -foldThreshold) {
    sample = -foldThreshold - (sample + foldThreshold);
  }
  return HardClip(sample, foldThreshold);
}

inline iplug::sample FastDist::BitCrush(iplug::sample sample, float bitDepth) {
  float step = 1.0f / (1 << static_cast<int>(bitDepth));
  return HardClip(std::floor(sample / step) * step, bitDepth);
}

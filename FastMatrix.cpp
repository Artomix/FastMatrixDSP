#include "FastMatrix.h"
#include "IPlug_include_in_plug_src.h"

FastMatrix::FastMatrix(const InstanceInfo& info)
: iplug::Plugin(info, MakeConfig(kNumParams, kNumPresets))
{
  GetParam(k_LPF_CUT)->InitDouble("Cut", 0.01, 0.01, 100.0, 0.01, "%");
  GetParam(k_LPF_EXTRA)->InitDouble("Reso", 0.01, 0.01, 100.0, 0.01, "%");
  GetParam(k_LPF_TYPE)->InitBool("Low-pass", false, "", 0, "", "2-Pole", "1-Pole");

  GetParam(k_HPF_CUT)->InitDouble("Cut", 0.01, 0.01, 100.0, 0.01, "%");
  GetParam(k_HPF_EXTRA)->InitDouble("Reso", 0.01, 0.01, 100.0, 0.01, "%");
  GetParam(k_HPF_TYPE)->InitBool("High-pass", false, "", 0, "", "2-Pole", "1-Pole");

  GetParam(k_DIST_EXTRA)->InitDouble("Dist", 0.01, 0.01, 100.0, 0.01, "%");
  GetParam(k_DIST_TYPE)->InitInt("Type", 0, 0, 5, "Mode");

  GetParam(k_3BAND_L)->InitDouble("Low", 100.0, 0.01, 200.0, 0.01, "%");
  GetParam(k_3BAND_M)->InitDouble("Mids", 100.0, 0.01, 200.0, 0.01, "%");
  GetParam(k_3BAND_H)->InitDouble("High", 100.0, 0.01, 200.0, 0.01, "%");

#if IPLUG_EDITOR // http://bit.ly/2S64BDd
  mMakeGraphicsFunc = [&]() {
    return MakeGraphics(*this, PLUG_WIDTH, PLUG_HEIGHT, PLUG_FPS, GetScaleForScreen(PLUG_WIDTH, PLUG_HEIGHT));
  };
  
  mLayoutFunc = [&](IGraphics* pGraphics) {
    pGraphics->AttachCornerResizer(EUIResizerMode::Scale, false);
    pGraphics->AttachPanelBackground(COLOR_WHITE);
    pGraphics->LoadFont("Roboto-Regular", ROBOTO_FN);
    const IRECT b = pGraphics->GetBounds();
    pGraphics->AttachControl(new ITextControl(b.GetMidVPadded(50).GetVShifted(-200), "Fast Matrix", IText(50)));

    // LOW PASS
    pGraphics->AttachControl(new IVSliderControl(b.GetCentredInside(50, 150).GetVShifted(-100 + 40).GetHShifted(0 - 40), k_LPF_CUT));
    pGraphics->AttachControl(new IVSliderControl(b.GetCentredInside(50, 150).GetVShifted(-100 + 40).GetHShifted(-50 - 40), k_LPF_EXTRA));
    pGraphics->AttachControl(new IVSwitchControl(b.GetCentredInside(100, 50).GetVShifted(-175 + 40).GetHShifted(-25 - 40), k_LPF_TYPE));

    // HIGH PASS
    pGraphics->AttachControl(new IVSliderControl(b.GetCentredInside(50, 150).GetVShifted(-100 + 40).GetHShifted(0 + 40), k_HPF_CUT));
    pGraphics->AttachControl(new IVSliderControl(b.GetCentredInside(50, 150).GetVShifted(-100 + 40).GetHShifted(+50 + 40), k_HPF_EXTRA));
    pGraphics->AttachControl(new IVSwitchControl(b.GetCentredInside(100, 50).GetVShifted(-175 + 40).GetHShifted(+25 + 40), k_HPF_TYPE));

    // DIST
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(75).GetHShifted(+50), k_DIST_EXTRA));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(75).GetHShifted(-50), k_DIST_TYPE));

    // LMH
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(175).GetHShifted(+100), k_3BAND_L));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(175).GetHShifted(0), k_3BAND_M));
    pGraphics->AttachControl(new IVKnobControl(b.GetCentredInside(100).GetVShifted(175).GetHShifted(-100), k_3BAND_H));

  };
#endif
}

#if IPLUG_DSP
void FastMatrix::ProcessBlock(sample** inputs, sample** outputs, int nFrames)
{
  const double _k_LPF_Cutoff = GetParam(k_LPF_CUT)->Value() / 100.;
  const double _k_LPF_Extra = GetParam(k_LPF_EXTRA)->Value() / 100.;
  const bool _k_LPF_Type = GetParam(k_LPF_TYPE)->Value();

  const double _k_HPF_Cutoff = GetParam(k_HPF_CUT)->Value() / 100.;
  const double _k_HPF_Extra = GetParam(k_HPF_EXTRA)->Value() / 100.;
  const bool _k_HPF_Type = GetParam(k_HPF_TYPE)->Value();

  const double _k_DIST_EXTRA = GetParam(k_DIST_EXTRA)->Value() / 100.;
  const int _k_DIST_Type = GetParam(k_DIST_TYPE)->Value();

  const double _k_3BAND_L = GetParam(k_3BAND_L)->Value() / 100.;
  const double _k_3BAND_M = GetParam(k_3BAND_M)->Value() / 100.;
  const double _k_3BAND_H = GetParam(k_3BAND_H)->Value() / 100.;

  const int nChans = NOutChansConnected();
  const float samplerate = GetSampleRate();

  fd_main->ProcessBlock(inputs, outputs, nFrames, samplerate, static_cast<FastDist::Mode>(_k_DIST_Type), _k_DIST_EXTRA);
  ff_main_lp->ProcessBlock(outputs, outputs, nFrames, _k_LPF_Cutoff, samplerate, _k_LPF_Type ? 0 : 1, _k_LPF_Extra);
  ff_main_hp->ProcessBlock(outputs, outputs, nFrames, _k_HPF_Cutoff, samplerate, _k_HPF_Type ? 0 : 1, _k_HPF_Extra);

  for (int j = 0; j < nFrames; j++) {
    outputs[0][j] = band_r->ProcessBlock(outputs[0][j], _k_3BAND_L, _k_3BAND_M, _k_3BAND_H, samplerate);
    outputs[1][j] = band_l->ProcessBlock(outputs[1][j], _k_3BAND_L, _k_3BAND_M, _k_3BAND_H, samplerate);
  }
}
#endif

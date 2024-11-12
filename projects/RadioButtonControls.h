#include <IControl.h>
#include <IPopupMenuControl.h>
#include <IPlugParameter.h>
#include <IPopupMenuControl.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <unordered_map>

#if defined VST3_API || defined VST3C_API
#undef stricmp
#undef strnicmp
#include "pluginterfaces/vst/ivstcontextmenu.h"
#include "base/source/fobject.h"
#endif

#include "IPlugPlatform.h"

#include "wdlstring.h"
#include "ptrlist.h"

#include "IGraphics.h"

BEGIN_IPLUG_NAMESPACE
BEGIN_IGRAPHICS_NAMESPACE

#pragma once
#include "IControl.h"
#include "IPlug_include_in_plug_hdr.h"

class RadioButtonControl : public IControl
{
public:
  RadioButtonControl(IGraphics* pGraphics, IParam* pParam, int idx, IRECT rect)
    : IControl(pGraphics, rect), mParam(pParam), mIdx(idx)
  {
    mValue = pParam->Value() == static_cast<double>(mIdx) ? 1.0 : 0.0;  // Check if this button is selected
  }

  void Draw(IGraphics& g) override
  {
    // Custom drawing logic for a radio button
    g.FillCircle(mRECT, mValue > 0.5 ? COLOR_BLUE : COLOR_WHITE);
    g.DrawText("Radio Button", mRECT, IText(20, COLOR_BLACK), EAlign::Center);
  }

  void OnMouseDown(float x, float y, const IMouseMod& mod) override
  {
    if (mValue == 0.0) {
      mValue = 1.0;
      mParam->SetValue(static_cast<double>(mIdx));  // Update the parameter
    }
    else {
      mValue = 0.0;  // Deactivate this button if already selected (optional)
    }
    SetDirty(true);  // Redraw the control
  }

private:
  IParam* mParam;
  int mIdx;
  float mValue;
};

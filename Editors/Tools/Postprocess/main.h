//---------------------------------------------------------------------------

#ifndef mainH
#define mainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "PERFGRAP.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <ToolWin.hpp>
#include <float_constructor.h>
#include <color_constructor.h>
#include <Dialogs.hpp>
#include <ImgList.hpp>
#define XRCORE_API __declspec(dllimport)

#include <xrCore.h>
#include <vector.h>
#include "PostprocessAnimator.h"
#include "color.h"
#include "single_param.h"
//---------------------------------------------------------------------------
class TMainForm : public TForm
{
__published:	// IDE-managed Components
    TPanel *Panel;
    TToolBar *ToolBar1;
    TStatusBar *StatusBar;
    TImage *Image;
    TToolButton *NewEffectButton;
    TToolButton *ToolButton2;
    TToolButton *LoadButton;
    TToolButton *SaveButton;
    TTabControl *TabControl;
    TSaveDialog *SaveDialog;
    TOpenDialog *OpenDialog;
    TImageList *ImageList;
    void __fastcall FormResize(TObject *Sender);
    void __fastcall NewEffectButtonClick(TObject *Sender);
    void __fastcall FormDestroy(TObject *Sender);
    void __fastcall FormPaint(TObject *Sender);
    void __fastcall ImageMouseUp(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
    void __fastcall GrayColorClick(TObject *Sender);
    void __fastcall TabControlChange(TObject *Sender);
    void __fastcall LoadButtonClick(TObject *Sender);
    void __fastcall SaveButtonClick(TObject *Sender);
private:	// User declarations
    TColorForm     *m_BaseColor;
    TColorForm     *m_AddColor;
    TColorForm     *m_GrayColor;
    TSingleParam   *m_Blur;
    TSingleParam   *m_Gray;
    TSingleParam   *m_DualH;
    TSingleParam   *m_DualV;
    TSingleParam   *m_NoiseI;
    TSingleParam   *m_NoiseG;
    TSingleParam   *m_NoiseF;

    CPostprocessAnimator    *m_Animator;
    SPPInfo         m_Params;
    TForm          *m_ShowForm;
    float           m_Marker;
public:		// User declarations
    __fastcall TMainForm(TComponent* Owner);
    void            SetMarkerPosition   (float time);
    void            UpdateGraph         ();
};
//---------------------------------------------------------------------------
extern PACKAGE TMainForm *MainForm;
//---------------------------------------------------------------------------
#endif

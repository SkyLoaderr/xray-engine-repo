//---------------------------------------------------------------------------
#ifndef FramePSH
#define FramePSH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>

#include "CustomObject.h"
#include "ui_customtools.h"
#include "ExtBtn.hpp"
// refs
class CEditObject;
//---------------------------------------------------------------------------
class TfraPS : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paSelectObject;
    TLabel *Label5;
	TExtBtn *ebSelectByRefs;
	TExtBtn *ebDeselectByRefs;
	TLabel *Label;
	TExtBtn *ExtBtn3;
	TPanel *Panel1;
	TExtBtn *ebCurObj;
	TLabel *APHeadLabel2;
	TExtBtn *ExtBtn2;
	TExtBtn *ebCurrentPSPlay;
	TExtBtn *ebCurrentPSStop;
	TLabel *Label1;
    void __fastcall PaneMinClick(TObject *Sender);
    void __fastcall ebSelectByRefsClick(TObject *Sender);
    void __fastcall ebDeselectByRefsClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
	void __fastcall ebCurObjClick(TObject *Sender);
	void __fastcall ebCurrentPSPlayClick(TObject *Sender);
	void __fastcall ebCurrentPSStopClick(TObject *Sender);
private:	// User declarations
    void __fastcall SelByRef (bool flag);
    void __fastcall PlayPS (bool flag);
    void __fastcall OutCurrentName();
public:		// User declarations
        __fastcall TfraPS(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif

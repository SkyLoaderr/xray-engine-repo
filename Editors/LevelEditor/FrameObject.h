//---------------------------------------------------------------------------
#ifndef FrameObjectH
#define FrameObjectH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>

#include "ui_customtools.h"
#include "ExtBtn.hpp"
// refs
class CEditableObject;
//---------------------------------------------------------------------------
class TfraObject : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paSelectObject;
    TPanel *paCommands;
	TExtBtn *ebResolve;
    TLabel *Label5;
	TExtBtn *ebSelectByRefs;
	TExtBtn *ebDeselectByRefs;
    TPanel *paAddObject;
	TExtBtn *ebCurObj;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TLabel *APHeadLabel2;
	TExtBtn *ExtBtn1;
	TLabel *APHeadLabel3;
	TExtBtn *ExtBtn3;
    void __fastcall PaneMinClick(TObject *Sender);
    void __fastcall ebSelectByRefsClick(TObject *Sender);
    void __fastcall ebDeselectByRefsClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
    void __fastcall ebCurObjClick(TObject *Sender);
private:	// User declarations
    void __fastcall SelByRefObject  ( bool flag );
    void __fastcall OutCurrentName();
public:		// User declarations
        __fastcall TfraObject(TComponent* Owner);
    void SetPosition(CEditableObject* O=0);
    void SetRotation(CEditableObject* O=0);
    void SetScale(CEditableObject* O=0);
};
//---------------------------------------------------------------------------
#endif

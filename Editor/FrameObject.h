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
#include "CloseBtn.hpp"
#include "RXCtrls.hpp"

#include "SceneObject.h"
#include "ui_customtools.h"
// refs
class CEditObject;
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
    void __fastcall ebResolveClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
    void __fastcall ebCurObjClick(TObject *Sender);
private:	// User declarations
    void __fastcall SelByRefObject  ( bool flag );
    void __fastcall ResolveObjects  ( bool bMsg );
    void __fastcall OutCurrentName();
public:		// User declarations
        __fastcall TfraObject(TComponent* Owner);
    void SetPosition(CEditObject* O=0);
    void SetRotation(CEditObject* O=0);
    void SetScale(CEditObject* O=0);
};
//---------------------------------------------------------------------------
#endif

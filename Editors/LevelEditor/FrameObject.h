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
#include "multi_edit.hpp"
#include "mxPlacemnt.hpp"
// refs
class CEditableObject;
//---------------------------------------------------------------------------
class TfraObject : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paSelectObject;
    TPanel *paCommands;
	TExtBtn *ebMultiAppend;
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
	TExtBtn *ebMultiSelectByRefMove;
	TMultiObjSpinEdit *seSelPercent;
	TBevel *Bevel1;
	TFormStorage *fsStorage;
	TLabel *Label1;
	TExtBtn *ebMultiSelectByRefAppend;
    void __fastcall PaneMinClick(TObject *Sender);
    void __fastcall ebSelectByRefsClick(TObject *Sender);
    void __fastcall ebDeselectByRefsClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
    void __fastcall ebCurObjClick(TObject *Sender);
	void __fastcall ebMultiAppendClick(TObject *Sender);
	void __fastcall ebMultiSelectByRefMoveClick(TObject *Sender);
	void __fastcall ebMultiSelectByRefAppendClick(TObject *Sender);
	void __fastcall seSelPercentKeyPress(TObject *Sender, char &Key);
private:	// User declarations
    void __fastcall MultiSelByRefObject ( bool clear_prev );
    void __fastcall SelByRefObject  	( bool flag );
    void __fastcall OutCurrentName();
public:		// User declarations
        __fastcall TfraObject(TComponent* Owner);
    void SetPosition(CEditableObject* O=0);
    void SetRotation(CEditableObject* O=0);
    void SetScale(CEditableObject* O=0);
};
//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
#ifndef FrameShapeH
#define FrameShapeH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>

#include "ui_customtools.h"
#include "multi_edit.hpp"
#include "ExtBtn.hpp"
#include "MXCtrls.hpp"
#include "mxPlacemnt.hpp"
// refs
class CEditObject;
//---------------------------------------------------------------------------
class TfraShape : public TFrame
{
__published:	// IDE-managed Components
	TPanel *paAppend;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TFormStorage *fsStorage;
	TExtBtn *ebTypeSphere;
	TExtBtn *ebTypeBox;
	TPanel *paEdit;
	TLabel *Label1;
	TExtBtn *ExtBtn3;
	TExtBtn *ebAttachShape;
	TExtBtn *ebDetachAllShapes;
    void __fastcall PaneMinClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
	void __fastcall ebDetachAllShapesClick(TObject *Sender);
	void __fastcall ebAttachShapeClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfraShape(TComponent* Owner);
    AnsiString GetCurrentEntity(BOOL bForceSelect=FALSE);
};
//---------------------------------------------------------------------------
#endif

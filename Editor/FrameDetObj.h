//---------------------------------------------------------------------------
#ifndef FrameDetObjH
#define FrameDetObjH
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
#include "multi_edit.hpp"
#include "Placemnt.hpp"
// refs
struct SDOClusterDef;
//---------------------------------------------------------------------------
class TfraDetailObject : public TFrame
{
__published:	// IDE-managed Components
	TPanel *paSelect;
	TExtBtn *ebDORandomScale;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TExtBtn *ebRandomRotate;
	TGroupBox *GroupBox2;
	TRxLabel *RxLabel5;
	TMultiObjSpinEdit *seScaleMinY;
	TMultiObjSpinEdit *seScaleMaxY;
	TRxLabel *RxLabel6;
	TFormStorage *fsStorage;
	TPanel *paCommand;
	TLabel *Label1;
	TExtBtn *ExtBtn3;
	TExtBtn *ExtBtn1;
	TExtBtn *ExtBtn4;
	TExtBtn *ExtBtn5;
	TExtBtn *ExtBtn6;
    void __fastcall PaneMinClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
	void __fastcall ExtBtn1Click(TObject *Sender);
	void __fastcall ExtBtn5Click(TObject *Sender);
	void __fastcall ExtBtn4Click(TObject *Sender);
	void __fastcall ExtBtn6Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfraDetailObject(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif

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
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TFormStorage *fsStorage;
	TPanel *paCommand;
	TLabel *Label1;
	TExtBtn *ExtBtn3;
	TExtBtn *ExtBtn1;
	TExtBtn *ExtBtn5;
	TExtBtn *ebReinitializeSlotObjects;
	TExtBtn *ExtBtn6;
	TExtBtn *ExtBtn4;
    void __fastcall PaneMinClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
	void __fastcall ExtBtn1Click(TObject *Sender);
	void __fastcall ExtBtn5Click(TObject *Sender);
	void __fastcall ebUpdateObjectsClick(TObject *Sender);
	void __fastcall ebReinitializeSlotObjectsClick(TObject *Sender);
	void __fastcall ExtBtn6Click(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfraDetailObject(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif

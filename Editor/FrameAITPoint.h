//---------------------------------------------------------------------------
#ifndef FrameAITPointH
#define FrameAITPointH
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
#include "Placemnt.hpp"
// refs
class CEditObject;
//---------------------------------------------------------------------------
class TfraAITPoint : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paCommands;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TExtBtn *ebAutoLink;
	TExtBtn *ebAddLinks;
	TBevel *Bevel1;
	TExtBtn *ebRemoveLinks;
	TFormStorage *fsStorage;
    void __fastcall PaneMinClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
	void __fastcall ebAddLinksClick(TObject *Sender);
	void __fastcall ebRemoveLinksClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfraAITPoint(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif

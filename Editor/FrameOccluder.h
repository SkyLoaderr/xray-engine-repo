//---------------------------------------------------------------------------


#ifndef FramePClipperH
#define FramePClipperH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <ExtCtrls.hpp>
#include "CloseBtn.hpp"

#include "SceneObject.h"
#include "ui_customtools.h"
#include "multi_edit.hpp"
#include "Placemnt.hpp"
//---------------------------------------------------------------------------
class TfraOccluder : public TFrame
{
__published:	// IDE-managed Components
	TPanel *paCommands;
    TExtBtn *ebModePlane;
    TLabel *Label1;
    TExtBtn *ExtBtn3;
    TExtBtn *ebModePoint;
	TFormStorage *fsStorage;
	TExtBtn *ebOptimize;
	TBevel *Bevel1;
    void __fastcall PanelMinClick(TObject *Sender);
    void __fastcall TopClick(TObject *Sender);
	void __fastcall ebOptimizeClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TfraOccluder(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif

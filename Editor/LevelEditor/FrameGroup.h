//---------------------------------------------------------------------------
#ifndef FrameGroupH
#define FrameGroupH
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
class TfraGroup : public TFrame
{
__published:	// IDE-managed Components
    TPanel *paCommands;
	TExtBtn *ebMakeGroup;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TExtBtn *ebUngroup;
    void __fastcall PaneMinClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
	void __fastcall ebMakeGroupClick(TObject *Sender);
	void __fastcall ebUngroupClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfraGroup(TComponent* Owner);
};
//---------------------------------------------------------------------------
#endif

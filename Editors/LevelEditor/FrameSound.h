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
#include "ESound.h"
// refs
class CEditObject;
//---------------------------------------------------------------------------
class TfraSound : public TFrame
{
__published:	// IDE-managed Components
	TPanel *paAppend;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TFormStorage *fsStorage;
	TExtBtn *ebTypeStaticSource;
	TExtBtn *ebTypeEnvironment;
    void __fastcall PaneMinClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
	__fastcall TfraSound(TComponent* Owner);
    ESound::ESoundType GetCurrentSoundType();
};
//---------------------------------------------------------------------------
#endif

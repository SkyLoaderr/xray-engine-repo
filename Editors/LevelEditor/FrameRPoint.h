//---------------------------------------------------------------------------
#ifndef FrameRPointH
#define FrameRPointH
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

#include "ItemList.h"
// refs
class CEditObject;
//---------------------------------------------------------------------------
class TfraSpawnPoint : public TFrame
{
__published:	// IDE-managed Components
	TPanel *paCurrent;
	TLabel *APHeadLabel1;
	TExtBtn *ExtBtn2;
	TFormStorage *fsStorage;
	TPanel *paCommands;
	TLabel *Label1;
	TExtBtn *ExtBtn1;
	TExtBtn *ebAttachObject;
	TExtBtn *evDetachObject;
	TPanel *paItems;
	TBevel *Bevel1;
    void __fastcall PaneMinClick(TObject *Sender);
    void __fastcall ExpandClick(TObject *Sender);
	void __fastcall ebAttachObjectClick(TObject *Sender);
	void __fastcall evDetachObjectClick(TObject *Sender);
private:	// User declarations
    TItemList* m_Items;
    void __fastcall OnItemFocused	(ListItemsVec& items);
    LPCSTR m_Current;
public:		// User declarations
	__fastcall TfraSpawnPoint(TComponent* Owner);
    void __fastcall OnEnter	();
    void __fastcall OnExit	();
    LPCSTR 			Current	(){return m_Current;}
};
//---------------------------------------------------------------------------
#endif

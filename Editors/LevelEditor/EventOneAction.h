//---------------------------------------------------------------------------

#ifndef EventOneActionH
#define EventOneActionH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <ComCtrls.hpp>

#include "ElHeader.hpp"
#include <Grids.hpp>
#include "ElTree.hpp"
#include <Menus.hpp>
#include "multi_color.hpp"
#include "ElXPThemedControl.hpp"
#include "ExtBtn.hpp"
#include "multi_edit.hpp"

#include "Event.h"
//refs
class TfrmPropertiesEvent;

class TfrmOneEventAction : public TForm
{
__published:	// IDE-managed Components
	TBevel *Bevel2;
	TExtBtn *ebMultiRemove;
	TMultiObjSpinEdit *seCount;
	TLabel *Label1;
	TLabel *Label2;
	TExtBtn *ebTypeEnter;
	TExtBtn *ebTypeLeave;
	TLabel *Label3;
	TLabel *Label4;
	TEdit *edEvent;
	TExtBtn *ebCLSID;
	TExtBtn *ebExpandText;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall ebMultiRemoveClick(TObject *Sender);
	void __fastcall ebCLSIDClick(TObject *Sender);
	void __fastcall OnModified(TObject *Sender);
	void __fastcall ebExpandTextClick(TObject *Sender);
private:	// User declarations
	TfrmPropertiesEvent* m_Parent;
public:
    bool bLoadMode;
public:		// User declarations
    __fastcall TfrmOneEventAction(TComponent* Owner);
    void __fastcall ShowIndex(TfrmPropertiesEvent* parent,CEvent::SAction* action=0);
    void __fastcall ApplyAction(CEvent::SAction* action);
    void __fastcall HideIndex();
};
//---------------------------------------------------------------------------
#endif

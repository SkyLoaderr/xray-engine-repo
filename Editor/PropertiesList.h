//---------------------------------------------------------------------------

#ifndef PropertiesListH
#define PropertiesListH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "ElTree.hpp"
#include "ElXPThemedControl.hpp"
#include "ElTreeStdEditors.hpp"
#include "ElTreeComboBox.hpp"
#include "ElTreeDTPickEdit.hpp"
#include "ElTreeModalEdit.hpp"
#include "ElTreeAdvEdit.hpp"
#include "ElTreeBtnEdit.hpp"
#include "ElTreeCheckBoxEdit.hpp"
#include "ElTreeCurrEdit.hpp"
#include "ElTreeMaskEdit.hpp"
#include "ElTreeMemoEdit.hpp"
#include "ElTreeSpinEdit.hpp"
#include "ElEdits.hpp"
#include "ElSpin.hpp"
#include "ElBtnEdit.hpp"
#include "ElACtrls.hpp"
#include "ElClrCmb.hpp"
#include "ElTreeTreeComboEdit.hpp"
#include "ElTreeCombo.hpp"
#include "ElMenus.hpp"
#include <Menus.hpp>
#include "ElCombos.hpp"
#include "MxMenus.hpp"
#include "mxPlacemnt.hpp"
#include "ElStatBar.hpp"
#include "ExtBtn.hpp"
#include <ExtCtrls.hpp>

#ifdef _LEVEL_EDITOR
 #include "CustomObject.h"
#endif
//---------------------------------------------------------------------------
class TfrmProperties : public TForm
{
__published:	// IDE-managed Components
	TElTree *tvProperties;
	TElTreeInplaceFloatSpinEdit *InplaceFloat;
	TElTreeInplaceSpinEdit *InplaceNumber;
	TMxPopupMenu *pmEnum;
	TFormStorage *fsStorage;
	TElTreeInplaceEdit *InplaceText;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall InplaceNumberBeforeOperation(TObject *Sender,
          bool &DefaultConversion);
	void __fastcall InplaceFloatBeforeOperation(TObject *Sender,
          bool &DefaultConversion);
	void __fastcall tvPropertiesClick(TObject *Sender);
	void __fastcall tvPropertiesItemDraw(TObject *Sender, TElTreeItem *Item,
          TCanvas *Surface, TRect &R, int SectionIndex);
	void __fastcall tvPropertiesMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall InplaceFloatValidateResult(TObject *Sender,
          bool &InputValid);
	void __fastcall InplaceNumberValidateResult(TObject *Sender,
          bool &InputValid);
	void __fastcall InplaceTextValidateResult(TObject *Sender,
          bool &InputValid);
private:	// User declarations
    void __fastcall PMItemClick(TObject *Sender);
	void __fastcall CustomClick(TElTreeItem* item);
	void __fastcall ColorClick(TElTreeItem* item);
	Graphics::TBitmap* m_BMEllipsis;
    bool bModified;
    bool bFillMode;
public:		// User declarations
	__fastcall TfrmProperties		        (TComponent* Owner);
#ifdef _LEVEL_EDITOR
    int  __fastcall ShowPropertiesModal		(ObjectList& lst);
#endif
    void __fastcall ShowProperties			();
    void __fastcall HideProperties			();
    void __fastcall Clear					();
    bool __fastcall IsModified				(){return bModified;}

    void __fastcall BeginFillMode			(const AnsiString& title="Properties");
    void __fastcall EndFillMode				();
	TElTreeItem* __fastcall AddItem			(TElTreeItem* parent, DWORD type, LPCSTR key, LPDWORD value=0, LPDWORD param=0);
};
//---------------------------------------------------------------------------

#define PROP_TYPE 		0x1000
#define PROP_WAVE 		0x1001
#define PROP_FLAG 		0x1002
#define PROP_TOKEN	 	0x1003
#define PROP_INTEGER 	0x1004
#define PROP_FLOAT	 	0x1005
#define PROP_BOOL	  	0x1006
#define PROP_MARKER		0x1007
#define PROP_COLOR		0x1008
#define PROP_TEXT		0x1009
#define PROP_EXEC		0x1010

typedef void PROP_EXEC_CB(LPDWORD p1);
//---------------------------------------------------------------------------
#endif

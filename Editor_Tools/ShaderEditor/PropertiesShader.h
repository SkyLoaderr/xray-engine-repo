//---------------------------------------------------------------------------

#ifndef PropertiesShaderH
#define PropertiesShaderH
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
//---------------------------------------------------------------------------
class TfrmShaderProperties : public TForm
{
__published:	// IDE-managed Components
	TElTree *tvProperties;
	TElTreeInplaceEdit *InplaceEdit;
	TElTreeInplaceButtonEdit *InplaceCustom;
	TElTreeInplaceFloatSpinEdit *InplaceFloat;
	TElTreeInplaceSpinEdit *InplaceNumber;
	TElTreeInplaceModalEdit *InplaceEnum;
	TMxPopupMenu *pmEnum;
	TFormStorage *fsStorage;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall InplaceEditAfterOperation(TObject *Sender, bool &Accepted,
          bool &DefaultConversion);
	void __fastcall InplaceFloatAfterOperation(TObject *Sender,
          bool &Accepted, bool &DefaultConversion);
	void __fastcall InplaceNumberAfterOperation(TObject *Sender,
          bool &Accepted, bool &DefaultConversion);
	void __fastcall InplaceNumberBeforeOperation(TObject *Sender,
          bool &DefaultConversion);
	void __fastcall InplaceFloatBeforeOperation(TObject *Sender,
          bool &DefaultConversion);
	void __fastcall tvPropertiesClick(TObject *Sender);
	void __fastcall tvPropertiesItemDraw(TObject *Sender, TElTreeItem *Item,
          TCanvas *Surface, TRect &R, int SectionIndex);
	void __fastcall tvPropertiesMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall CustomClick(TObject *Sender);
private:	// User declarations
	static TfrmShaderProperties *form;
    void __fastcall PMItemClick(TObject *Sender);
	TElTreeItem* __fastcall AddItem(TElTreeItem* parent, DWORD type, LPCSTR key, LPDWORD value=0);
	static CFS_Memory m_Stream;
public:		// User declarations
	__fastcall TfrmShaderProperties		        (TComponent* Owner);
    static void __fastcall ShowProperties();
    static void __fastcall HideProperties();
    static void __fastcall LoadProperties();
    static void __fastcall SaveProperties();
};
//---------------------------------------------------------------------------
#endif

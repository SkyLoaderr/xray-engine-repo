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
	TElTreeInplaceFloatSpinEdit *InplaceFloat;
	TElTreeInplaceSpinEdit *InplaceNumber;
	TMxPopupMenu *pmEnum;
	TFormStorage *fsStorage;
	TPanel *Panel1;
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
	void __fastcall ebPropertiesApplyClick(TObject *Sender);
private:	// User declarations
    void Modified();
    static bool m_bModified;
    static bool m_bFreeze;
	static TfrmShaderProperties *form;
    void __fastcall PMItemClick(TObject *Sender);
	TElTreeItem* __fastcall AddItem(TElTreeItem* parent, DWORD type, LPCSTR key, LPDWORD value=0, LPDWORD param=0);
	void __fastcall CustomClick(TElTreeItem* item);
	Graphics::TBitmap* m_BMEllipsis;
	void __fastcall AddMatrix(TElTreeItem* parent, LPSTR name);
	void __fastcall RemoveMatrix(TElTreeItem* parent);
	void __fastcall AddConstant(TElTreeItem* parent, LPSTR name);
public:		// User declarations
	__fastcall TfrmShaderProperties		        (TComponent* Owner);
    static void __fastcall ShowProperties();
    static void __fastcall HideProperties();
//    static bool __fastcall IsAutoApply(){ if (form) return form->ebPropertiesAutoApply->Down; else return true; }
    static bool __fastcall IsModified(){ return m_bModified; }
    static void __fastcall SetModified(bool bFlag){ m_bModified=bFlag; }

    static void __fastcall InitProperties();
    static void __fastcall FreezeUpdate(bool val){m_bFreeze=val; if (!val) InitProperties();}
};
//---------------------------------------------------------------------------
#endif

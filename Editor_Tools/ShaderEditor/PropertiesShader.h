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
#include "FrameProperties.h"
//---------------------------------------------------------------------------
class TfrmShaderProperties : public TForm
{
__published:	// IDE-managed Components
	TfraProperties *fraProperties;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall FormShow(TObject *Sender);
private:	// User declarations
	static TfrmShaderProperties *form;
public:		// User declarations
	__fastcall TfrmShaderProperties		        (TComponent* Owner);
    static void __fastcall ShowProperties();
    static void __fastcall HideProperties();
};
//---------------------------------------------------------------------------
#endif

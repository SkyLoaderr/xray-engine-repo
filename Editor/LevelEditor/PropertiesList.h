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
#include "multi_edit.hpp"

#ifdef _LEVEL_EDITOR
 #include "CustomObject.h"
#endif

// refs
struct xr_token;

DEFINE_VECTOR(LPVOID,LPVOIDVec,LPVOIDIt);

struct IntValue{
	int*	val;
	int		lim_mn;
    int		lim_mx;
    int 	inc;
    void*	OnAfterOperation;
    IntValue(int* value, int mn, int mx, int increm, void* after):val(value),lim_mn(mn),lim_mx(mx),inc(increm),OnAfterOperation(after){};
};
struct FloatValue{
	float*	val;
	float	lim_mn;
    float	lim_mx;
    float 	inc;
    int 	dec;
    void*	OnAfterOperation;
    FloatValue(float* value, float mn, float mx, float increment, int decimal, void* after):val(value),lim_mn(mn),lim_mx(mx),inc(increment),dec(decimal),OnAfterOperation(after){};
};
struct FlagValue{
	DWORD*	val;
	DWORD	mask;
    void*	OnAfterOperation;
	FlagValue(DWORD* value, DWORD _mask, void* after):val(value),mask(_mask),OnAfterOperation(after){};
};
struct TokenValue{
	DWORD*	val;
	xr_token* token;
    void*	OnAfterOperation;
	TokenValue(DWORD* value, xr_token* _token, void* after):val(value),token(_token),OnAfterOperation(after){};
};
//---------------------------------------------------------------------------
class TfrmProperties : public TForm
{
__published:	// IDE-managed Components
	TElTree *tvProperties;
	TMxPopupMenu *pmEnum;
	TFormStorage *fsStorage;
	TElTreeInplaceEdit *InplaceText;
	TMultiObjSpinEdit *seNumber;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall tvPropertiesClick(TObject *Sender);
	void __fastcall tvPropertiesItemDraw(TObject *Sender, TElTreeItem *Item,
          TCanvas *Surface, TRect &R, int SectionIndex);
	void __fastcall tvPropertiesMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall InplaceTextValidateResult(TObject *Sender,
          bool &InputValid);
	void __fastcall seNumberExit(TObject *Sender);
	void __fastcall seNumberKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall seNumberLWChange(TObject *Sender, int Val);
private:	// User declarations
    void __fastcall PMItemClick(TObject *Sender);
	void __fastcall CustomClick(TElTreeItem* item);
	void __fastcall TextureClick(TElTreeItem* item);
	void __fastcall ShaderEngineClick(TElTreeItem* item);
	void __fastcall ShaderCompileClick(TElTreeItem* item);
	void __fastcall STextureClick(TElTreeItem* item);
	void __fastcall SShaderEngineClick(TElTreeItem* item);
	void __fastcall SShaderCompileClick(TElTreeItem* item);
	void __fastcall ColorClick(TElTreeItem* item);
	Graphics::TBitmap* m_BMEllipsis;
    bool bModified;
    bool bFillMode;
	// LW style inpl editor
    void HideLWNumber();
    void PrepareLWNumber(TElTreeItem* node);
    void ShowLWNumber(TRect& R);
    void ApplyLWNumber();

    LPVOIDVec m_Params;
public:		// User declarations
	__fastcall TfrmProperties		        (TComponent* Owner);
#ifdef _LEVEL_EDITOR
    int  __fastcall ShowPropertiesModal		(ObjectList& lst);
#endif
	static TfrmProperties* CreateProperties	(TWinControl* parent=0, TAlign align=alNone);
	static void 	DestroyProperties		(TfrmProperties* props);
    void __fastcall ShowProperties			();
    void __fastcall HideProperties			();
    void __fastcall ClearProperties			();
    bool __fastcall IsModified				(){return bModified;}

    void __fastcall BeginFillMode			(const AnsiString& title="Properties");
    void __fastcall FillFromStream			(CFS_Memory& stream, DWORD advance);
    void __fastcall EndFillMode				(bool bFullExpand=true);
	TElTreeItem* __fastcall AddItem			(TElTreeItem* parent, DWORD type, LPCSTR key, LPVOID value=0);

	FlagValue* 		MakeFlagValue			(LPVOID val, DWORD mask, void* after=0)
    {
    	FlagValue* V=new FlagValue((LPDWORD)val,mask,after);
        m_Params.push_back(V);
        return V;
    }
	TokenValue* 	MakeTokenValue			(LPVOID val, xr_token* token, void* after=0)
    {
    	TokenValue* V=new TokenValue((LPDWORD)val,token,after);
        m_Params.push_back(V);
    	return V;
    }
	IntValue* 		MakeIntValue			(LPVOID val, int mn=0, int mx=100, int inc=1, void* after=0)
    {
    	IntValue* V	=new IntValue((int*)val,mn,mx,inc,after);
        m_Params.push_back(V);
    	return V;
    }
	FloatValue* 	MakeFloatValue			(LPVOID val, float mn=0.f, float mx=1.f, float inc=0.01f, int dec=2, void* after=0)
    {
    	FloatValue* V=new FloatValue((float*)val,mn,mx,inc,dec,after);
        m_Params.push_back(V);
    	return V;
    }
};
//---------------------------------------------------------------------------

#define PROP_TYPE 			0x1000
#define PROP_WAVE 			0x1001
#define PROP_FLAG 			0x1002
#define PROP_TOKEN	 		0x1003
#define PROP_INTEGER 		0x1004
#define PROP_FLOAT	 		0x1005
#define PROP_BOOL	  		0x1006
#define PROP_MARKER			0x1007
#define PROP_COLOR			0x1008
#define PROP_TEXT			0x1009
#define PROP_SH_ENGINE		0x1010
#define PROP_SH_COMPILE		0x1011
#define PROP_TEXTURE		0x1012
#define PROP_S_SH_ENGINE	0x1013
#define PROP_S_SH_COMPILE	0x1014
#define PROP_S_TEXTURE		0x1015
//---------------------------------------------------------------------------
#endif

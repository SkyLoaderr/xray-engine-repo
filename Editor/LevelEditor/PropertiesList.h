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
struct xr_list;

DEFINE_VECTOR(LPVOID,LPVOIDVec,LPVOIDIt);
typedef void __fastcall (__closure *TAfterOperation)(LPVOID data);

class PropertiesValue{
public:
	virtual ~PropertiesValue(){};
};

class IntValue: public PropertiesValue{
public:
	int*	val;
	int		lim_mn;
    int		lim_mx;
    int 	inc;
    TAfterOperation	OnAfterOperation;
    IntValue(int* value, int mn, int mx, int increm, TAfterOperation after):val(value),lim_mn(mn),lim_mx(mx),inc(increm),OnAfterOperation(after){};
};
class FloatValue: public PropertiesValue{
public:
	float*	val;
	float	lim_mn;
    float	lim_mx;
    float 	inc;
    int 	dec;
    TAfterOperation	OnAfterOperation;
    FloatValue(float* value, float mn, float mx, float increment, int decimal, TAfterOperation after):val(value),lim_mn(mn),lim_mx(mx),inc(increment),dec(decimal),OnAfterOperation(after){};
};
class FlagValue: public PropertiesValue{
public:
	DWORD*	val;
	DWORD	mask;
    TAfterOperation	OnAfterOperation;
	FlagValue(DWORD* value, DWORD _mask, TAfterOperation after):val(value),mask(_mask),OnAfterOperation(after){};
};
class TokenValue: public PropertiesValue{
public:
	DWORD*	val;
	xr_token* token;
    TAfterOperation	OnAfterOperation;
	TokenValue(DWORD* value, xr_token* _token, TAfterOperation after):val(value),token(_token),OnAfterOperation(after){};
	LPCSTR 	GetValue	(int id);
};
class ListValue: public PropertiesValue{
public:
	LPSTR	val;
	AStringVec items;
    TAfterOperation	OnAfterOperation;
	ListValue(LPSTR value, AStringVec* _items, TAfterOperation after):val(value),items(*_items),OnAfterOperation(after){};
};
//---------------------------------------------------------------------------
DEFINE_VECTOR(PropertiesValue*,PropValVec,PropValIt)

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

    PropValVec m_Params;
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

    void __fastcall BeginFillMode			(const AnsiString& title="Properties", LPCSTR section=0);
    void __fastcall FillFromStream			(CFS_Memory& stream, DWORD advance);
    void __fastcall EndFillMode				(bool bFullExpand=true);
	TElTreeItem* __fastcall AddItem			(TElTreeItem* parent, DWORD type, LPCSTR key, LPVOID value=0);
    void __fastcall GetColumnWidth			(int& c0, int& c1)
    {
    	c0=tvProperties->HeaderSections->Item[0]->Width;
	    c1=tvProperties->HeaderSections->Item[1]->Width;
	}
    void __fastcall SetColumnWidth			(int c0, int c1)
    {
    	tvProperties->HeaderSections->Item[0]->Width=c0;
	    tvProperties->HeaderSections->Item[1]->Width=c1;
	}
    void __fastcall SaveColumnWidth			(TFormStorage* fs)
    {
		fs->WriteInteger("props_column0_width",tvProperties->HeaderSections->Item[0]->Width);
		fs->WriteInteger("props_column1_width",tvProperties->HeaderSections->Item[1]->Width);
    }
    void __fastcall RestoreColumnWidth			(TFormStorage* fs)
    {
		tvProperties->HeaderSections->Item[0]->Width = fs->ReadInteger("props_column0_width",tvProperties->HeaderSections->Item[0]->Width);
		tvProperties->HeaderSections->Item[1]->Width = fs->ReadInteger("props_column1_width",tvProperties->HeaderSections->Item[1]->Width);
    }

	FlagValue* 		MakeFlagValue			(LPVOID val, DWORD mask, TAfterOperation after=0)
    {
    	FlagValue* V=new FlagValue((LPDWORD)val,mask,after);
        m_Params.push_back(V);
        return V;
    }
	TokenValue* 	MakeTokenValue			(LPVOID val, xr_token* token, TAfterOperation after=0)
    {
    	TokenValue* V=new TokenValue((LPDWORD)val,token,after);
        m_Params.push_back(V);
    	return V;
    }
	ListValue* 		MakeListValue			(LPVOID val, AStringVec* lst, TAfterOperation after=0)
    {
    	ListValue* V=new ListValue((LPSTR)val,lst,after);
        m_Params.push_back(V);
    	return V;
    }
	IntValue* 		MakeIntValue			(LPVOID val, int mn=0, int mx=100, int inc=1, TAfterOperation after=0)
    {
    	IntValue* V	=new IntValue((int*)val,mn,mx,inc,after);
        m_Params.push_back(V);
    	return V;
    }
	FloatValue* 	MakeFloatValue			(LPVOID val, float mn=0.f, float mx=1.f, float inc=0.01f, int dec=2, TAfterOperation after=0)
    {
    	FloatValue* V=new FloatValue((float*)val,mn,mx,inc,dec,after);
        m_Params.push_back(V);
    	return V;
    }
};
//---------------------------------------------------------------------------

enum EProperties{
	PROP_TYPE 			= 0x1000,
	PROP_WAVE,
	PROP_FLAG,
	PROP_TOKEN,
	PROP_LIST,
	PROP_INTEGER,
	PROP_FLOAT,
	PROP_BOOL,
	PROP_MARKER,
	PROP_MARKER2,
	PROP_COLOR,
	PROP_TEXT,
	PROP_SH_ENGINE,
	PROP_SH_COMPILE,
	PROP_TEXTURE,
	PROP_S_SH_ENGINE,
	PROP_S_SH_COMPILE,
	PROP_S_TEXTURE
};
//---------------------------------------------------------------------------
#endif

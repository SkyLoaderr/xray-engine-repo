//---------------------------------------------------------------------------

#ifndef PropertiesListH
#define PropertiesListH
//---------------------------------------------------------------------------

#include "ElTree.hpp"
#include "ElTreeStdEditors.hpp"
#include "ElXPThemedControl.hpp"
#include "multi_edit.hpp"
#include "MxMenus.hpp"
#include "mxPlacemnt.hpp"
#include <Classes.hpp>
#include <Controls.hpp>
#include <Menus.hpp>
#include <StdCtrls.hpp>

// refs
struct 	xr_token;
class PropValue;

typedef void 	__fastcall (__closure *TBeforeEdit)		(PropValue* sender, LPVOID edit_val);
typedef void 	__fastcall (__closure *TAfterEdit)		(PropValue* sender, LPVOID edit_val);
typedef void 	__fastcall (__closure *TOnDrawValue)	(PropValue* sender, LPVOID draw_val);
typedef void 	__fastcall (__closure *TOnModifiedEvent)(void);

class PropValue{
public:
    TAfterEdit			OnAfterEdit;
    TBeforeEdit			OnBeforeEdit;
    TOnDrawValue		OnDrawValue;
public:
						PropValue		(TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):OnAfterEdit(after),OnBeforeEdit(before),OnDrawValue(draw){};
	virtual 			~PropValue		(){};
    virtual LPCSTR		GetText			()=0;
};

class IntValue: public PropValue{
public:
	int*				val;
	int					lim_mn;
    int					lim_mx;
    int 				inc;
    					IntValue		(int* value, int mn, int mx, int increm, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):val(value),lim_mn(mn),lim_mx(mx),inc(increm),PropValue(after,before,draw){};
    virtual LPCSTR		GetText			()
    {
    	int draw_val 	= *val;
        if (OnDrawValue)OnDrawValue(this, &draw_val);
		static AnsiString draw_text	= draw_val;
        return draw_text.c_str();
    }
};
class FloatValue: public PropValue{
public:
	float*				val;
	float				lim_mn;
    float				lim_mx;
    float 				inc;
    int 				dec;
    					FloatValue		(float* value, float mn, float mx, float increment, int decimal, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):val(value),lim_mn(mn),lim_mx(mx),inc(increment),dec(decimal),PropValue(after,before,draw){};
    virtual LPCSTR		GetText			()
    {
    	float draw_val 	= *val;
        if (OnDrawValue)OnDrawValue(this, &draw_val);
        static AnsiString draw_text;
		AnsiString fmt; fmt.sprintf("%%.%df",dec);
        draw_text.sprintf(fmt.c_str(),draw_val);
		return draw_text.c_str();
    }
};
class FlagValue: public PropValue{
public:
	DWORD*				val;
	DWORD				mask;
						FlagValue		(DWORD* value, DWORD _mask, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):val(value),mask(_mask),PropValue(after,before,draw){};
    virtual LPCSTR		GetText			();
};
class TokenValue: public PropValue{
public:
	DWORD*				val;
	xr_token* 			token;
						TokenValue		(DWORD* value, xr_token* _token, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):val(value),token(_token),PropValue(after,before,draw){};
	virtual LPCSTR 		GetText			();
};
class TokenValue2: public PropValue{
public:
	DWORD*				val;
	AStringVec 			items;
						TokenValue2		(DWORD* value, AStringVec* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):val(value),items(*_items),PropValue(after,before,draw){};
	virtual LPCSTR 		GetText			();
};
class ListValue: public PropValue{
public:
	LPSTR				val;
	AStringVec 			items;
						ListValue		(LPSTR value, AStringVec* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):val(value),items(*_items),PropValue(after,before,draw){};
	virtual LPCSTR		GetText			();
};
//---------------------------------------------------------------------------
DEFINE_VECTOR(PropValue*,PropValVec,PropValIt)

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
//	Graphics::TBitmap* m_BMEllipsis;
    bool bModified;
    bool bFillMode;
	// LW style inpl editor
    void HideLWNumber();
    void PrepareLWNumber(TElTreeItem* node);
    void ShowLWNumber(TRect& R);
    void ApplyLWNumber();
    void CancelLWNumber();

    PropValVec m_Params;
    TOnModifiedEvent OnModifiedEvent;
    void Modified(){bModified=true; if (OnModifiedEvent) OnModifiedEvent();}
public:		// User declarations
	__fastcall TfrmProperties		        (TComponent* Owner);
#ifdef _LEVEL_EDITOR
    int  __fastcall ShowPropertiesModal		(ObjectList& lst);
#endif
	static TfrmProperties* CreateProperties	(TWinControl* parent=0, TAlign align=alNone, TOnModifiedEvent modif=0);
	static void 	DestroyProperties		(TfrmProperties* props);
    void __fastcall ShowProperties			();
    void __fastcall HideProperties			();
    void __fastcall ClearProperties			();
    bool __fastcall IsModified				(){return bModified;}
    void __fastcall RefreshProperties		(){tvProperties->Repaint();}

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
		fs->WriteInteger(AnsiString().sprintf("%s_column0_width",Name.c_str()),tvProperties->HeaderSections->Item[0]->Width);
		fs->WriteInteger(AnsiString().sprintf("%s_column1_width",Name.c_str()),tvProperties->HeaderSections->Item[1]->Width);
    }
    void __fastcall RestoreColumnWidth			(TFormStorage* fs)
    {
		tvProperties->HeaderSections->Item[0]->Width = fs->ReadInteger(AnsiString().sprintf("%s_column0_width",Name.c_str()),tvProperties->HeaderSections->Item[0]->Width);
		tvProperties->HeaderSections->Item[1]->Width = fs->ReadInteger(AnsiString().sprintf("%s_column1_width",Name.c_str()),tvProperties->HeaderSections->Item[1]->Width);
    }

	FlagValue* 		MakeFlagValue			(LPVOID val, DWORD mask, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
    	FlagValue* V=new FlagValue((LPDWORD)val,mask,after,before,draw);
        m_Params.push_back(V);
        return V;
    }
	TokenValue* 	MakeTokenValue			(LPVOID val, xr_token* token, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
    	TokenValue* V=new TokenValue((LPDWORD)val,token,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	TokenValue2* 	MakeTokenValue2			(LPVOID val, AStringVec* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
    	TokenValue2* V=new TokenValue2((LPDWORD)val,lst,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	ListValue* 		MakeListValue			(LPVOID val, AStringVec* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
    	ListValue* V=new ListValue((LPSTR)val,lst,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	IntValue* 		MakeIntValue			(LPVOID val, int mn=0, int mx=100, int inc=1, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
    	IntValue* V	=new IntValue((int*)val,mn,mx,inc,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	FloatValue* 	MakeFloatValue			(LPVOID val, float mn=0.f, float mx=1.f, float inc=0.01f, int dec=2, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
    	FloatValue* V=new FloatValue((float*)val,mn,mx,inc,dec,after,before,draw);
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
	PROP_TOKEN2,
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

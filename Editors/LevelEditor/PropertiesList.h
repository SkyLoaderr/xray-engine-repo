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
#include <ExtCtrls.hpp>
#include "ElTreeAdvEdit.hpp"
#include <Mask.hpp>

enum EPropType{
	PROP_WAVE 			= 0x1000,
	PROP_FLAG,
	PROP_TOKEN,
	PROP_TOKEN2,
    PROP_TOKEN3,
	PROP_LIST,
	PROP_INTEGER,
	PROP_FLOAT,
    PROP_VECTOR,
	PROP_BOOL,
	PROP_MARKER,
	PROP_MARKER2,
	PROP_COLOR,
	PROP_TEXT,
    PROP_ANSI_TEXT,
	PROP_SH_ENGINE,
	PROP_SH_COMPILE,
	PROP_TEXTURE,
	PROP_TEXTURE2,
	PROP_ANSI_SH_ENGINE,
	PROP_ANSI_SH_COMPILE,
	PROP_ANSI_TEXTURE
};
// refs
struct 	xr_token;
class PropValue;

typedef void 	__fastcall (__closure *TBeforeEdit)		(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
typedef void 	__fastcall (__closure *TAfterEdit)		(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
typedef void 	__fastcall (__closure *TOnDrawValue)	(PropValue* sender, LPVOID draw_val);
typedef void 	__fastcall (__closure *TOnModifiedEvent)(void);
typedef void 	__fastcall (__closure *TOnItemFocused)	(TElTreeItem* item);

class PropValue{
public:
    TAfterEdit			OnAfterEdit;
    TBeforeEdit			OnBeforeEdit;
    TOnDrawValue		OnDrawValue;
public:
						PropValue		(TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):OnAfterEdit(after),OnBeforeEdit(before),OnDrawValue(draw){};
	virtual 			~PropValue		(){};
    virtual LPCSTR		GetText			()=0;
    virtual bool		IsChanged		()=0;
    virtual void		Reset			()=0;
};
class TextValue: public PropValue{
	AnsiString			init_val;
public:
	int					lim;
	LPSTR				val;
						TextValue		(LPSTR value, int _lim, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):init_val(value),val(value),lim(_lim),PropValue(after,before,draw){};
    virtual LPCSTR		GetText			();
    virtual bool		IsChanged		(){return (init_val==val);}
    virtual void		Reset			(){strcpy(val,init_val.c_str());}
};
class AnsiTextValue: public PropValue{
	AnsiString			init_val;
public:
	AnsiString*			val;
						AnsiTextValue	(AnsiString* value, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):init_val(*value),val(value),PropValue(after,before,draw){};
    virtual LPCSTR		GetText			();
    virtual bool		IsChanged		(){return (init_val==*val);}
    virtual void		Reset			(){*val=init_val;}
};
class IntValue: public PropValue{
	int					init_val;
public:
	int*				val;
	int					lim_mn;
    int					lim_mx;
    int 				inc;
    					IntValue		(int* value, int mn, int mx, int increm, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):init_val(*value),val(value),lim_mn(mn),lim_mx(mx),inc(increm),PropValue(after,before,draw){};
    virtual LPCSTR		GetText			()
    {
    	int draw_val 	= *val;
        if (OnDrawValue)OnDrawValue(this, &draw_val);
		static AnsiString draw_text;
        draw_text		= draw_val;
        return draw_text.c_str();
    }
    virtual bool		IsChanged		(){return (init_val==*val);}
    virtual void		Reset			(){*val=init_val;}
};
class FloatValue: public PropValue{
	float				init_val;
public:
	float*				val;
	float				lim_mn;
    float				lim_mx;
    float 				inc;
    int 				dec;
    					FloatValue		(float* value, float mn, float mx, float increment, int decimal, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):init_val(*value),val(value),lim_mn(mn),lim_mx(mx),inc(increment),dec(decimal),PropValue(after,before,draw){};
    virtual LPCSTR		GetText			()
    {
    	float draw_val 	= *val;
        if (OnDrawValue)OnDrawValue(this, &draw_val);
        static AnsiString draw_text;
		AnsiString fmt; fmt.sprintf("%%.%df",dec);
        draw_text.sprintf(fmt.c_str(),draw_val);
		return draw_text.c_str();
    }
    virtual bool		IsChanged		(){return (init_val==*val);}
    virtual void		Reset			(){*val=init_val;}
};
class VectorValue: public PropValue{
	Fvector				init_val;
public:
	Fvector*			val;
	float				lim_mn;
    float				lim_mx;
    float 				inc;
    int 				dec;
						VectorValue		(Fvector* value, float mn, float mx, float increment, int decimal, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):init_val(*value),val(value),lim_mn(mn),lim_mx(mx),inc(increment),dec(decimal),PropValue(after,before,draw){};
    virtual LPCSTR		GetText			()
    {
		Fvector draw_val 	= *val;
        if (OnDrawValue)OnDrawValue(this, &draw_val);
        static AnsiString draw_text;
		AnsiString fmt; fmt.sprintf("{%%.%df, %%.%df, %%.%df}",dec,dec,dec);
        draw_text.sprintf(fmt.c_str(),draw_val.x,draw_val.y,draw_val.z);
		return draw_text.c_str();
    }
    virtual bool		IsChanged		(){return (init_val.similar(*val));}
    virtual void		Reset			(){val->set(init_val);}
};
class FlagValue: public PropValue{
	DWORD				init_val;
public:
	DWORD*				val;
	DWORD				mask;
						FlagValue		(DWORD* value, DWORD _mask, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):init_val(*value),val(value),mask(_mask),PropValue(after,before,draw){};
    virtual LPCSTR		GetText			(){return 0;}
    virtual bool		IsChanged		(){return (init_val==*val);}
    virtual void		Reset			(){*val=init_val;}
};
class TokenValue: public PropValue{
	DWORD				init_val;
public:
	DWORD*				val;
	xr_token* 			token;
						TokenValue		(DWORD* value, xr_token* _token, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):init_val(*value),val(value),token(_token),PropValue(after,before,draw){};
	virtual LPCSTR 		GetText			();
    virtual bool		IsChanged		(){return (init_val==*val);}
    virtual void		Reset			(){*val=init_val;}
};
class TokenValue2: public PropValue{
	DWORD				init_val;
public:
	DWORD*				val;
	AStringVec 			items;
						TokenValue2		(DWORD* value, AStringVec* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):init_val(*value),val(value),items(*_items),PropValue(after,before,draw){};
	virtual LPCSTR 		GetText			();
    virtual bool		IsChanged		(){return (init_val==*val);}
    virtual void		Reset			(){*val=init_val;}
};
class TokenValue3: public PropValue{
	DWORD				init_val;
public:
	struct Item {
		DWORD		ID;
		string64	str;
	};
	DWORD*				val;
	DWORD				cnt;
    const Item*			items;
						TokenValue3		(DWORD* value, DWORD _cnt, const Item* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):init_val(*value),val(value),cnt(_cnt),items(_items),PropValue(after,before,draw){};
	virtual LPCSTR 		GetText			();
    virtual bool		IsChanged		(){return (init_val==*val);}
    virtual void		Reset			(){*val=init_val;}
};
class ListValue: public PropValue{
	LPSTR				init_val;
public:
	LPSTR				val;
	AStringVec 			items;
						ListValue		(LPSTR value, AStringVec* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):init_val(value),val(value),items(*_items),PropValue(after,before,draw){};
						ListValue		(LPSTR value, DWORD cnt, LPCSTR* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):init_val(value),val(value),PropValue(after,before,draw){items.resize(cnt); int i=0; for (AStringIt it=items.begin(); it!=items.end(); it++,i++) *it=_items[i]; };
	virtual LPCSTR		GetText			();
    virtual bool		IsChanged		(){return (strcmp(init_val,val));}
    virtual void		Reset			(){strcpy(val,init_val);}
};
//---------------------------------------------------------------------------
DEFINE_VECTOR(PropValue*,PropValVec,PropValIt)

class TfrmProperties : public TForm
{
__published:	// IDE-managed Components
	TElTree *tvProperties;
	TMxPopupMenu *pmEnum;
	TFormStorage *fsStorage;
	TMultiObjSpinEdit *seNumber;
	TEdit *edText;
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall tvPropertiesClick(TObject *Sender);
	void __fastcall tvPropertiesItemDraw(TObject *Sender, TElTreeItem *Item,
          TCanvas *Surface, TRect &R, int SectionIndex);
	void __fastcall tvPropertiesMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
	void __fastcall seNumberExit(TObject *Sender);
	void __fastcall seNumberKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall edTextExit(TObject *Sender);
	void __fastcall edTextKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
	void __fastcall tvPropertiesItemFocused(TObject *Sender);
	void __fastcall edTextDblClick(TObject *Sender);
	void __fastcall tvPropertiesHeaderColumnResize(TObject *Sender,
          int SectionIndex);
	void __fastcall FormDeactivate(TObject *Sender);
private:	// User declarations
    void __fastcall PMItemClick(TObject *Sender);
	void __fastcall CustomClick(TElTreeItem* item);
	void __fastcall ColorClick(TElTreeItem* item);
	void __fastcall VectorClick(TElTreeItem* item);

	void __fastcall CustomTextClick(TElTreeItem* item);
	void __fastcall CustomAnsiTextClick(TElTreeItem* item);

	Graphics::TBitmap* m_BMCheck;
	Graphics::TBitmap* m_BMDot;
	Graphics::TBitmap* m_BMEllipsis;
    bool bModified;
    int iFillMode;
    AnsiString last_selected_item;
	// LW style inpl editor
    void HideLWNumber();
    void PrepareLWNumber(TElTreeItem* node);
    void ShowLWNumber(TRect& R);
    void ApplyLWNumber();
    void CancelLWNumber();
	// text inplace editor
    void HideLWText();
    void PrepareLWText(TElTreeItem* node);
    void ShowLWText(TRect& R);
    void ApplyLWText();
    void CancelLWText();

    PropValVec m_Params;
    TOnModifiedEvent 	OnModifiedEvent;
    TOnItemFocused      OnItemFocused;
    void Modified(){bModified=true; if (OnModifiedEvent) OnModifiedEvent();}
    void ClearParams(TElTreeItem* node=0);
    void ApplyEditControl();
    void CancelEditControl();
public:		// User declarations
	__fastcall TfrmProperties		        (TComponent* Owner);
	static TfrmProperties* CreateProperties	(TWinControl* parent=0, TAlign align=alNone, TOnModifiedEvent modif=0, TOnItemFocused focused=0);
	static void 	DestroyProperties		(TfrmProperties*& props);
    void __fastcall ShowPropertiesModal		();
    void __fastcall ShowProperties			();
    void __fastcall HideProperties			();                                   
    void __fastcall ClearProperties			();
    void __fastcall ResetProperties			();
    bool __fastcall IsModified				(){return bModified;}
    void __fastcall ResetModified			(){bModified = false;}
    void __fastcall RefreshProperties		(){tvProperties->Repaint();}

    void __fastcall SetModifiedEvent		(TOnModifiedEvent modif=0){OnModifiedEvent=modif;}
    void __fastcall BeginFillMode			(const AnsiString& title="Properties", LPCSTR section=0);
    void __fastcall EndFillMode				(bool bFullExpand=true);
    TElTreeItem* __fastcall BeginEditMode	(LPCSTR section=0);
    void __fastcall EndEditMode				(TElTreeItem* expand_node=0);
	TElTreeItem* __fastcall AddItem			(TElTreeItem* parent, EPropType type, LPCSTR key, LPVOID value=0);
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
	TokenValue3* 	MakeTokenValue3			(LPVOID val, DWORD cnt, const TokenValue3::Item* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
    	TokenValue3* V=new TokenValue3((LPDWORD)val,cnt,lst,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	ListValue* 		MakeListValue			(LPVOID val, AStringVec* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
    	ListValue* V=new ListValue((LPSTR)val,lst,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	ListValue* 		MakeListValueA			(LPVOID val, DWORD cnt, LPCSTR* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
    	ListValue* V=new ListValue((LPSTR)val,cnt,lst,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	TextValue* 		MakeTextValue			(LPVOID val, int lim,TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
    	TextValue* V=new TextValue((LPSTR)val,lim,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	AnsiTextValue* 	MakeAnsiTextValue		(LPVOID val, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
    	AnsiTextValue* V=new AnsiTextValue((AnsiString*)val,after,before,draw);
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
	VectorValue* 	MakeVectorValue			(LPVOID val, float mn=0.f, float mx=1.f, float inc=0.01f, int dec=2, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
    	VectorValue* V=new VectorValue((Fvector*)val,mn,mx,inc,dec,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
    void 			IsUpdating				(bool bVal)
    {
    	tvProperties->IsUpdating = bVal;
    }
	static IC LPVOID		GetItemData		(TElTreeItem* item){return item->Data;}
	static IC bool 			IsItemType		(TElTreeItem* item, EPropType type){return item->Tag==type;}
	static IC EPropType 	GetItemType		(TElTreeItem* item){return (EPropType)item->Tag;}
	static IC LPCSTR	 	GetItemColumn	(TElTreeItem* item, int col){
    	static AnsiString t;
        if (col<item->ColumnText->Count) t=item->ColumnText->Strings[col];
        return t.c_str();
    }
};
//---------------------------------------------------------------------------
#endif

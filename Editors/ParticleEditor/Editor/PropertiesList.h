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

#include "PropertiesListTypes.h"


class TProperties : public TForm
{
__published:	// IDE-managed Components
	TElTree 			*tvProperties;
	TMxPopupMenu 		*pmEnum;
	TFormStorage 		*fsStorage;
	TMultiObjSpinEdit 	*seNumber;
	TEdit 				*edText;
	TBevel 				*Bevel1;
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
    void __fastcall PMItemClick		(TObject *Sender);
	void __fastcall WaveFormClick	(TElTreeItem* item);
	void __fastcall ColorClick		(TElTreeItem* item);
	void __fastcall VectorClick		(TElTreeItem* item);
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

    PropItemVec m_Params;
    TOnModifiedEvent 	OnModifiedEvent;
    TOnItemFocused      OnItemFocused;
    void Modified(){bModified=true; if (OnModifiedEvent) OnModifiedEvent();}
    void ClearParams(TElTreeItem* node=0);
    void ApplyEditControl();
    void CancelEditControl();

	MarkerItem* 	MakeMarker				(LPCSTR val)
    {
    	MarkerItem* V=new MarkerItem((LPCSTR)val);
        m_Params.push_back(V);
        return V;
    }
	FlagValue* 		MakeFlagValue			(LPDWORD val, DWORD mask, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw)
    {
    	FlagValue* V=new FlagValue(val,mask,after,before,draw);
        m_Params.push_back(V);
        return V;
    }
	TokenValue* 	MakeTokenValue			(LPDWORD val, xr_token* token, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw)
    {
    	TokenValue* V=new TokenValue(val,token,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	TokenValue2* 	MakeTokenValue2			(LPDWORD val, AStringVec* lst, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw)
    {
    	TokenValue2* V=new TokenValue2(val,lst,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	TokenValue3* 	MakeTokenValue3			(LPDWORD val, DWORD cnt, const TokenValue3::Item* lst, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw)
    {
    	TokenValue3* V=new TokenValue3(val,cnt,lst,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	ListValue* 		MakeListValue			(LPSTR val, AStringVec* lst, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw)
    {
    	ListValue* V=new ListValue(val,lst,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	ListValue* 		MakeListValueA			(LPSTR val, DWORD cnt, LPCSTR* lst, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw)
    {
    	ListValue* V=new ListValue(val,cnt,lst,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	TextValue* 		MakeTextValue			(LPSTR val, int lim,TAfterEdit after, TBeforeEdit before, TOnDrawValue draw)
    {
    	TextValue* V=new TextValue(val,lim,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	AnsiTextValue* 	MakeAnsiTextValue		(AnsiString* val, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw)
    {
    	AnsiTextValue* V=new AnsiTextValue((AnsiString*)val,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	IntValue* 		MakeIntValue			(int* val, int mn, int mx, int inc, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw)
    {
    	IntValue* V	=new IntValue(val,mn,mx,inc,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	DWORDValue* 	MakeDWORDValue			(LPDWORD val, DWORD mx, DWORD inc, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw)
    {
    	DWORDValue* V	=new DWORDValue(val,mx,inc,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	BOOLValue* 		MakeBOOLValue			(LPBOOL val, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw)
    {
    	BOOLValue* V	=new BOOLValue(val,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	FloatValue* 	MakeFloatValue			(float* val, float mn, float mx, float inc, int dec, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw)
    {
    	FloatValue* V=new FloatValue(val,mn,mx,inc,dec,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	VectorValue* 	MakeVectorValue			(Fvector* val, float mn, float mx, float inc, int decim, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw)
    {
    	VectorValue* V=new VectorValue(val,mn,mx,inc,decim,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	WaveValue* 		MakeWaveValue			(WaveForm* val, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw)
    {
    	WaveValue* V	=new WaveValue(val,after,before,draw);
        m_Params.push_back(V);
    	return V;
    }
	PropItem* __fastcall AddItem			(TElTreeItem* parent, EPropType type, LPCSTR key, PropItem* item);
public:		// User declarations
	__fastcall TProperties		        	(TComponent* Owner);
	static TProperties* CreateForm			(TWinControl* parent=0, TAlign align=alNone, TOnModifiedEvent modif=0, TOnItemFocused focused=0);
	static void 	DestroyForm				(TProperties*& props);
    void __fastcall ShowPropertiesModal		();
    void __fastcall ShowProperties			();
    void __fastcall HideProperties			();                                   
    void __fastcall ClearProperties			();
    bool __fastcall IsModified				(){return bModified;}
    void __fastcall ResetModified			(){bModified = false;}
    void __fastcall RefreshProperties		(){tvProperties->Repaint();}

    void __fastcall SetModifiedEvent		(TOnModifiedEvent modif=0){OnModifiedEvent=modif;}
    void __fastcall BeginFillMode			(const AnsiString& title="Properties", LPCSTR section=0);
    void __fastcall EndFillMode				(bool bFullExpand=true);
    TElTreeItem* __fastcall BeginEditMode	(LPCSTR section=0);
    void __fastcall EndEditMode				(TElTreeItem* expand_node=0);
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
    void __fastcall RestoreColumnWidth		(TFormStorage* fs)
    {
		tvProperties->HeaderSections->Item[0]->Width = fs->ReadInteger(AnsiString().sprintf("%s_column0_width",Name.c_str()),tvProperties->HeaderSections->Item[0]->Width);
		tvProperties->HeaderSections->Item[1]->Width = fs->ReadInteger(AnsiString().sprintf("%s_column1_width",Name.c_str()),tvProperties->HeaderSections->Item[1]->Width);
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

//	void __fastcall AddItems				(TElTreeItem* parent, CStream& F);
    IC MarkerItem* 			AddMarkerItem	(TElTreeItem* parent, LPCSTR key, LPCSTR value=0){
        return (MarkerItem*)AddItem 		(parent,PROP_MARKER,key,MakeMarker(value));
    }
    IC IntValue* 			AddIntItem		(TElTreeItem* parent, LPCSTR key, int* value, int mn=0, int mx=100, int inc=1, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
        return (IntValue*)	AddItem 		(parent,PROP_INTEGER,key,MakeIntValue(value,mn,mx,inc,after,before,draw));
    }
    IC DWORDValue* 			AddDWORDItem	(TElTreeItem* parent, LPCSTR key, LPDWORD value, int mx=100, int inc=1, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
        return (DWORDValue*)AddItem 		(parent,PROP_DWORD,key,MakeDWORDValue(value,mx,inc,after,before,draw));
    }
    IC FloatValue* 			AddFloatItem	(TElTreeItem* parent, LPCSTR key, float* value, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
        return (FloatValue*)AddItem 		(parent,PROP_FLOAT,key,MakeFloatValue(value,mn,mx,inc,decim,after,before,draw));
    }
    IC BOOLValue* 			AddBOOLItem		(TElTreeItem* parent, LPCSTR key, BOOL* value, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
        return (BOOLValue*)	AddItem 		(parent,PROP_BOOLEAN,key,MakeBOOLValue(value,after,before,draw));
    }
    IC FlagValue* 			AddFlagItem		(TElTreeItem* parent, LPCSTR key, DWORD* value, DWORD mask, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
        return (FlagValue*)	AddItem 		(parent,PROP_FLAG,key,MakeFlagValue(value,mask,after,before,draw));
    }
    IC VectorValue* 		AddVectorItem	(TElTreeItem* parent, LPCSTR key, Fvector* value, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
        return (VectorValue*)AddItem 		(parent,PROP_VECTOR,key,MakeVectorValue(value,mn,mx,inc,decim,after,before,draw));
    }
	IC TokenValue* 			AddTokenItem	(TElTreeItem* parent, LPCSTR key, LPDWORD value, xr_token* token, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
    	return (TokenValue*)AddItem	    	(parent,PROP_TOKEN,key,MakeTokenValue(value,token,after,before,draw));
    }
	IC TokenValue2* 		AddTokenItem	(TElTreeItem* parent, LPCSTR key, LPDWORD value, AStringVec* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
    	return (TokenValue2*)AddItem	    (parent,PROP_TOKEN2,key,MakeTokenValue2(value,lst,after,before,draw));
    }
	IC TokenValue3* 	   	AddTokenItem	(TElTreeItem* parent, LPCSTR key, LPDWORD value, DWORD cnt, const TokenValue3::Item* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
    	return (TokenValue3*)AddItem    	(parent,PROP_TOKEN3,key,MakeTokenValue3(value,cnt,lst,after,before,draw));
    }
	IC ListValue* 			AddListItem		(TElTreeItem* parent, LPCSTR key, LPSTR value, AStringVec* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
    	return (ListValue*)	AddItem	    	(parent,PROP_LIST,key,MakeListValue(value,lst,after,before,draw));
    }
	IC ListValue* 			AddListItemA	(TElTreeItem* parent, LPCSTR key, LPSTR value, DWORD cnt, LPCSTR* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
    	return (ListValue*)	AddItem	    	(parent,PROP_LIST,key,MakeListValueA(value,cnt,lst,after,before,draw));
    }
    IC DWORDValue* 			AddColorItem	(TElTreeItem* parent, LPCSTR key, LPDWORD value, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
        return (DWORDValue*)AddItem 		(parent,PROP_COLOR,key,MakeDWORDValue(value,0xffffffff,1,after,before,draw));
    }
	IC TextValue* 			AddTextItem		(TElTreeItem* parent, LPCSTR key, LPSTR value, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
    	return (TextValue*)	AddItem	    	(parent,PROP_TEXT,key,MakeTextValue(value,lim,after,before,draw));
    }
	IC AnsiTextValue* 		AddAnsiTextItem	(TElTreeItem* parent, LPCSTR key, AnsiString* value, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
    	return (AnsiTextValue*)	AddItem	   	(parent,PROP_ANSI_TEXT,key,MakeAnsiTextValue(value,after,before,draw));
    }
	IC TextValue* 			AddEShaderItem	(TElTreeItem* parent, LPCSTR key, LPSTR value, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
    	return (TextValue*)	AddItem	    	(parent,PROP_SH_ENGINE,key,MakeTextValue(value,lim,after,before,draw));
    }
	IC TextValue* 			AddCShaderItem	(TElTreeItem* parent, LPCSTR key, LPSTR value, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
    	return (TextValue*)	AddItem	    	(parent,PROP_SH_COMPILE,key,MakeTextValue(value,lim,after,before,draw));
    }
	IC TextValue* 			AddTextureItem 	(TElTreeItem* parent, LPCSTR key, LPSTR value, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
    	return (TextValue*)	AddItem	    	(parent,PROP_TEXTURE,key,MakeTextValue(value,lim,after,before,draw));
    }
	IC TextValue* 			AddTexture2Item	(TElTreeItem* parent, LPCSTR key, LPSTR value, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
    	return (TextValue*)	AddItem	    	(parent,PROP_TEXTURE2,key,MakeTextValue(value,lim,after,before,draw));
    }
	IC AnsiTextValue* 		AddAnsiEShaderItem(TElTreeItem* parent, LPCSTR key, AnsiString* value, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
    	return (AnsiTextValue*)	AddItem	   	(parent,PROP_ANSI_SH_ENGINE,key,MakeAnsiTextValue(value,after,before,draw));
    }
	IC AnsiTextValue* 		AddAnsiCShaderItem(TElTreeItem* parent, LPCSTR key, AnsiString* value, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
    	return (AnsiTextValue*)	AddItem	   	(parent,PROP_ANSI_SH_COMPILE,key,MakeAnsiTextValue(value,after,before,draw));
    }
	IC AnsiTextValue* 		AddAnsiTextureItem(TElTreeItem* parent, LPCSTR key, AnsiString* value, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
    	return (AnsiTextValue*)	AddItem	   	(parent,PROP_ANSI_TEXTURE,key,MakeAnsiTextValue(value,after,before,draw));
    }
	IC TextValue* 			AddLightAnimItem(TElTreeItem* parent, LPCSTR key, LPSTR value, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
    	return (TextValue*)	AddItem	    	(parent,PROP_LIGHTANIM,key,MakeTextValue(value,lim,after,before,draw));
    }
	IC TextValue* 			AddLibObjectItem(TElTreeItem* parent, LPCSTR key, LPSTR value, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
    	return (TextValue*)	AddItem	    	(parent,PROP_LIBOBJECT,key,MakeTextValue(value,lim,after,before,draw));
    }
	IC TextValue* 			AddEntityItem	(TElTreeItem* parent, LPCSTR key, LPSTR value, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
    	return (TextValue*)	AddItem	    	(parent,PROP_ENTITY,key,MakeTextValue(value,lim,after,before,draw));
    }
	IC WaveValue* 			AddWaveItem		(TElTreeItem* parent, LPCSTR key, WaveForm* value, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0){
    	return (WaveValue*)	AddItem	    	(parent,PROP_WAVE,key,MakeWaveValue(value,after,before,draw));
    }               
/*
	AnsiTextValue* 	MakeAnsiTextValue		(LPVOID val, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
	BOOLValue* 		MakeBOOLValue			(LPVOID val, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)

    PROP_LIGHTANIM,
    PROP_LIBOBJECT,
    PROP_ENTITY,
	PROP_WAVE 	
*/
};
//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------
#ifndef PropertiesListTypesH
#define PropertiesListTypesH
//---------------------------------------------------------------------------
#include "ElTree.hpp"
//---------------------------------------------------------------------------

enum EPropType{
	PROP_MARKER	= 0x1000,
	PROP_INTEGER,
	PROP_DWORD,
	PROP_FLOAT,
	PROP_BOOLEAN,
	PROP_FLAG,
    PROP_VECTOR,
	PROP_TOKEN,
	PROP_TOKEN2,
    PROP_TOKEN3,
	PROP_LIST,
	PROP_COLOR,
	PROP_TEXT,
    PROP_ANSI_TEXT,
	PROP_SH_ENGINE,
	PROP_SH_COMPILE,
	PROP_TEXTURE,
	PROP_TEXTURE2,
	PROP_ANSI_SH_ENGINE,
	PROP_ANSI_SH_COMPILE,
	PROP_ANSI_TEXTURE,
    PROP_LIGHTANIM,
    PROP_LIBOBJECT,
    PROP_ENTITY,
	PROP_WAVE 			
};
// refs
struct 	xr_token;
class PropItem;

//------------------------------------------------------------------------------
typedef void 	__fastcall (__closure *TBeforeEdit)		(TElTreeItem* item, PropItem* sender, LPVOID edit_val);
typedef void 	__fastcall (__closure *TAfterEdit)		(TElTreeItem* item, PropItem* sender, LPVOID edit_val);
typedef void 	__fastcall (__closure *TOnDrawValue)	(PropItem* sender, LPVOID draw_val);
typedef void 	__fastcall (__closure *TOnModifiedEvent)(void);
typedef void 	__fastcall (__closure *TOnItemFocused)	(TElTreeItem* item);
//------------------------------------------------------------------------------

class PropItem{
public:
	TElTreeItem*		item;
    EPropType			type;
public:
						PropItem		():item(0){};
	virtual 			~PropItem		(){};
    virtual LPCSTR		GetText			()=0;
    virtual void		InitNext		(LPVOID value)=0;
//	virtual void		Load			(CStream& F){;}
//	virtual void		Save			(CFS_Base& F){;}
};

class MarkerItem: public PropItem{
	AnsiString			value;
public:
						MarkerItem		(LPCSTR val):PropItem(),value(val){};
    virtual LPCSTR		GetText			(){return value.c_str();}
    virtual void		InitNext		(LPVOID value){};
};

class PropValue: public PropItem{
public:
	bool				bDiff;
    TAfterEdit			OnAfterEdit;
    TBeforeEdit			OnBeforeEdit;
    TOnDrawValue		OnDrawValue;
public:
						PropValue		(TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):PropItem(),bDiff(false),OnAfterEdit(after),OnBeforeEdit(before),OnDrawValue(draw){};
	virtual 			~PropValue		(){};
    virtual bool		IsDiffValues	(){return bDiff;}
};
//------------------------------------------------------------------------------

class TextValue: public PropValue{
	LPSTRVec			values;
public:
	int					lim;
						TextValue		(LPSTR value, int _lim, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):lim(_lim),PropValue(after,before,draw){values.push_back(value);};
    virtual LPCSTR		GetText			();
    virtual void		InitNext		(LPVOID value){if (0!=strcmp((LPSTR)value,values.front())) bDiff=true; values.push_back((LPSTR)value);}
    void				ApplyValue		(LPCSTR value){bDiff=false;for (LPSTRIt it=values.begin();it!=values.end();it++) strcpy(*it,value);}
    LPCSTR				GetValue		(){return values.front();}
};
//------------------------------------------------------------------------------

class AnsiTextValue: public PropValue{
	LPAStringVec		values;
public:
						AnsiTextValue	(AnsiString* value, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):PropValue(after,before,draw){values.push_back(value);};
    virtual LPCSTR		GetText			();
    virtual void		InitNext		(LPVOID value){if (*(AnsiString*)value!=*values.front()) bDiff=true; values.push_back((AnsiString*)value);}
    void				ApplyValue		(const AnsiString& value){bDiff=false;for (LPAStringIt it=values.begin();it!=values.end();it++) **it=value;}
    const AnsiString&	 GetValue		(){return *values.front();}
};
//------------------------------------------------------------------------------

class BOOLValue: public PropValue{
	LPBOOLVec			values;
public:
						BOOLValue		(BOOL* value, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):PropValue(after,before,draw){values.push_back(value);};
    virtual LPCSTR		GetText			();
    virtual void		InitNext		(LPVOID value){if (*(BOOL*)value!=*values.front()) bDiff=true; values.push_back((BOOL*)value);}
    void				ApplyValue		(BOOL value){bDiff=false;for (LPBOOLIt it=values.begin();it!=values.end();it++) **it = value;}
    BOOL 				GetValue		(){return *values.front();}
};
//------------------------------------------------------------------------------

DEFINE_VECTOR(WaveForm*,LPWaveFormVec,LPWaveFormIt);

class WaveValue: public PropValue{
	LPWaveFormVec		values;
public:
						WaveValue		(WaveForm* value, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):PropValue(after,before,draw){values.push_back(value);};
    virtual LPCSTR		GetText			(){return "[Wave]";}
    virtual void		InitNext		(LPVOID value){if (!((WaveForm*)value)->Similar(*values.front())) bDiff=true; values.push_back((WaveForm*)value);}
    void				ApplyValue		(const WaveForm& value){bDiff=false;for (LPWaveFormIt it=values.begin();it!=values.end();it++) **it = value;}
    const WaveForm& 	GetValue		(){return *values.front();}
};
//------------------------------------------------------------------------------

class DWORDValue: public PropValue{
	LPDWORDVec			values;
public:
    DWORD				lim_mx;
    DWORD 				inc;
public:
						DWORDValue		(DWORD* value, DWORD mx, DWORD increm, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):lim_mx(mx),inc(increm),PropValue(after,before,draw){clamp(*value,0ul,lim_mx); values.push_back(value);};
    virtual LPCSTR		GetText			(){
    	DWORD draw_val 	= GetValue();
        if (OnDrawValue)OnDrawValue(this, &draw_val);
		static AnsiString draw_text;
        draw_text		= draw_val;
        return draw_text.c_str();
    }
    virtual void		InitNext		(LPVOID value){clamp(*(DWORD*)value,0ul,lim_mx); if (*(DWORD*)value!=*values.front()) bDiff=true; values.push_back((DWORD*)value);}
    void				ApplyValue		(DWORD value){bDiff=false;for (LPDWORDIt it=values.begin();it!=values.end();it++) **it = value;}
    DWORD 				GetValue		(){return *values.front();}
};
//------------------------------------------------------------------------------

class IntValue: public PropValue{
	LPIntVec			values;
public:
	int					lim_mn;
    int					lim_mx;
    int 				inc;
    					IntValue		(int* value, int mn, int mx, int increm, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):lim_mn(mn),lim_mx(mx),inc(increm),PropValue(after,before,draw){clamp(*value,lim_mn,lim_mx); values.push_back(value);};
    virtual LPCSTR		GetText			()
    {
    	int draw_val 	= GetValue();
        if (OnDrawValue)OnDrawValue(this, &draw_val);
		static AnsiString draw_text;
        draw_text		= draw_val;
        return draw_text.c_str();
    }
    virtual void		InitNext		(LPVOID value){clamp(*(int*)value,lim_mn,lim_mx); if (*(int*)value!=*values.front()) bDiff=true; values.push_back((int*)value);}
    void				ApplyValue		(int value){bDiff=false;for (LPIntIt it=values.begin();it!=values.end();it++) **it = value;}
    int 				GetValue		(){return *values.front();}
};
//------------------------------------------------------------------------------

class FloatValue: public PropValue{
	LPFloatVec			values;
public:
	float				lim_mn;
    float				lim_mx;
    float 				inc;
    int 				dec;
    					FloatValue		(float* value, float mn, float mx, float increment, int decimal, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):lim_mn(mn),lim_mx(mx),inc(increment),dec(decimal),PropValue(after,before,draw){clamp(*value,lim_mn,lim_mx); values.push_back(value);}
    virtual LPCSTR		GetText			()
    {
    	float draw_val 	= GetValue();
        if (OnDrawValue)OnDrawValue(this, &draw_val);
        static AnsiString draw_text;
		AnsiString fmt; fmt.sprintf("%%.%df",dec);
        draw_text.sprintf(fmt.c_str(),draw_val);
		return draw_text.c_str();
    }
    virtual void		InitNext		(LPVOID value){clamp(*(float*)value,lim_mn,lim_mx); if (*(float*)value!=*values.front()) bDiff=true; values.push_back((float*)value);}
    void				ApplyValue		(float value){bDiff=false;for (LPFloatIt it=values.begin();it!=values.end();it++) **it = value;}
    float 				GetValue		(){return *values.front();}
};
//------------------------------------------------------------------------------

class VectorValue: public PropValue{
	LPFvectorVec		values;
public:
	float				lim_mn;
    float				lim_mx;
    float 				inc;
    int 				dec;
						VectorValue		(Fvector* value, float mn, float mx, float increment, int decimal, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):lim_mn(mn),lim_mx(mx),inc(increment),dec(decimal),PropValue(after,before,draw){values.push_back(value);}
    virtual LPCSTR		GetText			()
    {
		Fvector draw_val 	= GetValue();
        if (OnDrawValue)OnDrawValue(this, &draw_val);
        static AnsiString draw_text;
		AnsiString fmt; fmt.sprintf("{%%.%df, %%.%df, %%.%df}",dec,dec,dec);
        draw_text.sprintf(fmt.c_str(),draw_val.x,draw_val.y,draw_val.z);
		return draw_text.c_str();
    }
    virtual void		InitNext		(LPVOID value){if (!((Fvector*)value)->similar(*values.front())) bDiff=true; values.push_back((Fvector*)value);}
    void				ApplyValue		(const Fvector& value){bDiff=false;for (LPFvectorIt it=values.begin();it!=values.end();it++) **it = value;}
    const Fvector&		GetValue		(){return *values.front();}
};
//------------------------------------------------------------------------------

class FlagValue: public PropValue{
	LPDWORDVec			values;
public:
	DWORD				mask;
						FlagValue		(DWORD* value, DWORD _mask, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):mask(_mask),PropValue(after,before,draw){values.push_back(value);}
    virtual LPCSTR		GetText			(){return 0;}
    virtual void		InitNext		(LPVOID value){bDiff=false; bool a=(*(DWORD*)value)&mask; bool b=(*values.front())&mask; if (a!=b) bDiff=true; values.push_back((DWORD*)value);}
    void				ApplyValue		(bool value){for (LPDWORDIt it=values.begin();it!=values.end();it++) if (value) **it|=mask; else **it&=~mask; }
    bool	 			GetValue		(){return (*values.front())&mask;}
};
//------------------------------------------------------------------------------

class TokenValue: public PropValue{
	LPDWORDVec			values;
public:
	xr_token* 			token;
						TokenValue		(DWORD* value, xr_token* _token, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):token(_token),PropValue(after,before,draw){values.push_back(value);}
	virtual LPCSTR 		GetText			();
    virtual void		InitNext		(LPVOID value)	{if (*(DWORD*)value!=*values.front()) bDiff=true; values.push_back((DWORD*)value);}
    void				ApplyValue		(DWORD value)	{bDiff=false;for (LPDWORDIt it=values.begin();it!=values.end();it++) **it = value;}
    DWORD 				GetValue		(){return *values.front();}
};
//------------------------------------------------------------------------------

class TokenValue2: public PropValue{
	LPDWORDVec			values;
public:
	AStringVec 			items;
						TokenValue2		(DWORD* value, AStringVec* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):items(*_items),PropValue(after,before,draw){values.push_back(value);}
	virtual LPCSTR 		GetText			();
    virtual void		InitNext		(LPVOID value)	{if (*(DWORD*)value!=*values.front()) bDiff=true; values.push_back((DWORD*)value);}
    void				ApplyValue		(DWORD value)	{bDiff=false;for (LPDWORDIt it=values.begin();it!=values.end();it++) **it = value;}
    DWORD 				GetValue		(){return *values.front();}
};
//------------------------------------------------------------------------------

class TokenValue3: public PropValue{
	LPDWORDVec			values;
public:
	struct Item {
		DWORD		ID;
		string64	str;
	};
	DWORD				cnt;
    const Item*			items;
						TokenValue3		(DWORD* value, DWORD _cnt, const Item* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):cnt(_cnt),items(_items),PropValue(after,before,draw){values.push_back(value);};
	virtual LPCSTR 		GetText			();
    virtual void		InitNext		(LPVOID value)	{if (*(DWORD*)value!=*values.front()) bDiff=true; values.push_back((DWORD*)value);}
    void				ApplyValue		(DWORD value)	{bDiff=false;for (LPDWORDIt it=values.begin();it!=values.end();it++) **it = value;}
    DWORD 				GetValue		(){return *values.front();}
};
//------------------------------------------------------------------------------

class ListValue: public PropValue{
	LPSTRVec			values;
public:
	AStringVec 			items;
						ListValue		(LPSTR value, AStringVec* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):items(*_items),PropValue(after,before,draw){values.push_back(value);};
						ListValue		(LPSTR value, DWORD cnt, LPCSTR* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):PropValue(after,before,draw){values.push_back(value); items.resize(cnt); int i=0; for (AStringIt it=items.begin(); it!=items.end(); it++,i++) *it=_items[i]; };
	virtual LPCSTR		GetText			();
    virtual void		InitNext		(LPVOID value){if (0!=strcmp((LPSTR)value,values.front())) bDiff=true; values.push_back((LPSTR)value);}
    void				ApplyValue		(LPCSTR value){bDiff=false;for (LPSTRIt it=values.begin();it!=values.end();it++) strcpy(*it,value);}
    LPCSTR				GetValue		(){return values.front();}
};
//------------------------------------------------------------------------------
DEFINE_VECTOR(PropItem*,PropItemVec,PropItemIt)

//---------------------------------------------------------------------------
#endif

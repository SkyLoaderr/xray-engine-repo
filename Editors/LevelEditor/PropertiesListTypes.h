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
	PROP_FCOLOR,
	PROP_TEXT,
    PROP_A_TEXT,
	PROP_ESHADER,
	PROP_CSHADER,
	PROP_TEXTURE,
	PROP_TEXTURE2,
	PROP_A_ESHADER,
	PROP_A_CSHADER,
	PROP_A_TEXTURE,
    PROP_LIGHTANIM,
    PROP_LIBOBJECT,
    PROP_ENTITY,
	PROP_WAVE 			
};
// refs
struct 	xr_token;
class PropValue;

//------------------------------------------------------------------------------
typedef void 	__fastcall (__closure *TBeforeEdit)		(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
typedef void 	__fastcall (__closure *TAfterEdit)		(TElTreeItem* item, PropValue* sender, LPVOID edit_val);
typedef void 	__fastcall (__closure *TOnDrawValue)	(PropValue* sender, LPVOID draw_val);
typedef void 	__fastcall (__closure *TOnModifiedEvent)(void);
typedef void 	__fastcall (__closure *TOnItemFocused)	(TElTreeItem* item);
//------------------------------------------------------------------------------

class PropValue{
protected:
	bool				bDiff;			// internal use only
    LPSTR				cKey;
public:
	// internal use only
    EPropType			type;
	PropValue*			parent;
	TElTreeItem*		item;
    bool				bEnabled;
public:
	// base events
    TAfterEdit			OnAfterEdit;
    TBeforeEdit			OnBeforeEdit;
    TOnDrawValue		OnDrawValue;
public:
						PropValue		(LPCSTR key, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):
                        				item(0),bEnabled(true),bDiff(false),OnAfterEdit(after),
                                        OnBeforeEdit(before),OnDrawValue(draw){cKey=strdup(key);}
	virtual 			~PropValue		(){_FREE(cKey);}
    virtual LPCSTR		GetText			()=0;
    virtual void		FirstInit		(LPVOID value)=0;
    virtual void		NextInit		(LPVOID value)=0;
    virtual void		ResetValue		()=0;
    bool				IsDiffValues	(){return bDiff;}
    TElTreeItem*		GetParentItem	(){return parent?parent->item:0;}
    LPCSTR				GetKey			(){return cKey;}
};
//------------------------------------------------------------------------------

class MarkerItem: public PropValue{
	AnsiString			value;
public:
						MarkerItem		(LPCSTR key, LPCSTR val):PropValue(key,TAfterEdit(0),TBeforeEdit(0),TOnDrawValue(0)),value(val){};
    virtual LPCSTR		GetText			(){return value.c_str();}
    virtual void		InitNext		(LPVOID value){};
    virtual	void		ResetValue		(){;}
};

class TextValue: public PropValue{
	AStringVec			init_values;
	LPSTRVec			values;
    void				AppendValue		(LPSTR value){values.push_back(value);init_values.push_back(value);}
public:
	int					lim;
						TextValue		(LPCSTR key, LPSTR value, int _lim, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):lim(_lim),PropValue(key,after,before,draw){AppendValue(value);};
    virtual LPCSTR		GetText			();
    virtual void		InitNext		(LPVOID value){if (0!=strcmp((LPSTR)value,values.front())) bDiff=true; AppendValue((LPSTR)value);}
    bool				ApplyValue		(LPCSTR value)
    {
    	bool bChanged	= false;
        for (LPSTRIt it=values.begin();it!=values.end();it++){
        	if (0!=strcmp(*it,value)){
	        	strcpy(*it,value);
                bChanged= true;
            }
        }
        if (bChanged) 	bDiff = false;
        return bChanged;
    }
    LPCSTR				GetValue		(){return values.front();}
    void				ResetValue		(){
    	AStringIt src=init_values.begin(); for (LPSTRIt it=values.begin();it!=values.end();it++,src++) strcpy(*it,src->c_str());
    }
};
//------------------------------------------------------------------------------

class ATextValue: public PropValue{
	AStringVec			init_values;
	LPAStringVec		values;
    void				AppendValue		(AnsiString* value){values.push_back(value);init_values.push_back(*value);}
public:
						ATextValue		(LPCSTR key, AnsiString* value, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):PropValue(key,after,before,draw){AppendValue(value);};
    virtual LPCSTR		GetText			();
    virtual void		InitNext		(LPVOID value){if (*(AnsiString*)value!=*values.front()) bDiff=true; AppendValue((AnsiString*)value);}
    bool				ApplyValue		(const AnsiString& value)
    {
    	bool bChanged	= false;
        for (LPAStringIt it=values.begin();it!=values.end();it++){
        	if (**it!=value){
	        	**it=value;
                bChanged= true;
            }
        }
        if (bChanged) 	bDiff = false;
        return bChanged;
    }
    const AnsiString&	 GetValue		(){return *values.front();}
    void				ResetValue		(){AStringIt src=init_values.begin(); for (LPAStringIt it=values.begin();it!=values.end();it++,src++) **it=*src;}
};
//------------------------------------------------------------------------------

class BOOLValue: public PropValue{
	BOOLVec				init_values;
	LPBOOLVec			values;
    void				AppendValue		(LPBOOL value){values.push_back(value);init_values.push_back(*value);}
public:
						BOOLValue		(LPCSTR key, LPBOOL value, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):PropValue(key,after,before,draw){AppendValue(value);};
    virtual LPCSTR		GetText			(){return 0;}
    virtual void		InitNext		(LPVOID value){if (*(BOOL*)value!=*values.front()) bDiff=true; AppendValue((BOOL*)value);}
    virtual bool		ApplyValue		(BOOL value)
    {	
    	bool bChanged	= false;
	    for (LPBOOLIt it=values.begin();it!=values.end();it++){
        	if (**it!=value){
	        	**it 	= value;
                bChanged= true;
            }
        }
        if (bChanged) 	bDiff = false;
        return bChanged;
    }
    BOOL 				GetValue		(){return *values.front();}
    void				ResetValue		(){BOOLIt src=init_values.begin(); for (LPBOOLIt it=values.begin();it!=values.end();it++,src++) **it = *src;}
};
//------------------------------------------------------------------------------

DEFINE_VECTOR(WaveForm,WaveFormVec,WaveFormIt);
DEFINE_VECTOR(WaveForm*,LPWaveFormVec,LPWaveFormIt);

class WaveValue: public PropValue{
	WaveFormVec			init_values;
	LPWaveFormVec		values;
    void				AppendValue		(WaveForm* value){values.push_back(value);init_values.push_back(*value);}
public:
						WaveValue		(LPCSTR key, WaveForm* value, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):PropValue(key,after,before,draw){AppendValue(value);};
    virtual LPCSTR		GetText			(){return "[Wave]";}
    virtual void		InitNext		(LPVOID value){if (!((WaveForm*)value)->Similar(*values.front())) bDiff=true; AppendValue((WaveForm*)value);}
    bool				ApplyValue		(const WaveForm& value)
    {
    	bool bChanged	= false;
        for (LPWaveFormIt it=values.begin();it!=values.end();it++){
        	if (!(*it)->Similar(value)){
	        	**it 	= value;
                bChanged= true;
            }
        }
        if (bChanged) 	bDiff = false;
        return bChanged;
    }
    const WaveForm& 	GetValue		(){return *values.front();}
    void				ResetValue		(){WaveFormIt src=init_values.begin(); for (LPWaveFormIt it=values.begin();it!=values.end();it++,src++) **it = *src;}
};
//------------------------------------------------------------------------------

class DWORDValue: public PropValue{
	DWORDVec			init_values;
	LPDWORDVec			values;
    void				AppendValue		(LPDWORD value){values.push_back(value);init_values.push_back(*value);}
public:
    DWORD				lim_mx;
    DWORD 				inc;
public:
						DWORDValue		(LPCSTR key, DWORD* value, DWORD mx, DWORD increm, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):lim_mx(mx),inc(increm),PropValue(key,after,before,draw){clamp(*value,0ul,lim_mx); AppendValue(value);};
    virtual LPCSTR		GetText			(){
    	DWORD draw_val 	= GetValue();
        if (OnDrawValue)OnDrawValue(this, &draw_val);
		static AnsiString draw_text;
        draw_text		= draw_val;
        return draw_text.c_str();
    }
    virtual void		InitNext		(LPVOID value){clamp(*(DWORD*)value,0ul,lim_mx); if (*(DWORD*)value!=*values.front()) bDiff=true; AppendValue((DWORD*)value);}
    virtual bool		ApplyValue		(DWORD value)
    {
    	bool bChanged	= false;
        for (LPDWORDIt it=values.begin();it!=values.end();it++){
        	if (**it!=value){
	        	**it 	= value;
                bChanged= true;
            }
        }
        if (bChanged) 	bDiff = false;
        return bChanged;
    }
    DWORD 				GetValue		(){return *values.front();}
    void				ResetValue		(){DWORDIt src=init_values.begin(); for (LPDWORDIt it=values.begin();it!=values.end();it++,src++) **it = *src;}
};
//------------------------------------------------------------------------------

class IntValue: public PropValue{
	IntVec				init_values;
	LPIntVec			values;
    void				AppendValue		(int* value){values.push_back(value);init_values.push_back(*value);}
public:
	int					lim_mn;
    int					lim_mx;
    int 				inc;
    					IntValue		(LPCSTR key, int* value, int mn, int mx, int increm, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):lim_mn(mn),lim_mx(mx),inc(increm),PropValue(key,after,before,draw){clamp(*value,lim_mn,lim_mx); AppendValue(value);};
    virtual LPCSTR		GetText			()
    {
    	int draw_val 	= GetValue();
        if (OnDrawValue)OnDrawValue(this, &draw_val);
		static AnsiString draw_text;
        draw_text		= draw_val;
        return draw_text.c_str();
    }
    virtual void		InitNext		(LPVOID value){clamp(*(int*)value,lim_mn,lim_mx); if (*(int*)value!=*values.front()) bDiff=true; AppendValue((int*)value);}
    virtual bool		ApplyValue		(int value)
    {
    	bool bChanged	= false;
        for (LPIntIt it=values.begin();it!=values.end();it++){
        	if (**it!=value){
	        	**it 	= value;
                bChanged= true;
            }
        }
        if (bChanged) 	bDiff = false;
        return bChanged;
    }
    int 				GetValue		(){return *values.front();}
    void				ResetValue		(){IntIt src=init_values.begin(); for (LPIntIt it=values.begin();it!=values.end();it++,src++) **it = *src;}
};
//------------------------------------------------------------------------------

class FloatValue: public PropValue{
	FloatVec			init_values;
	LPFloatVec			values;
    void				AppendValue		(float* value){values.push_back(value);init_values.push_back(*value);}
public:
	float				lim_mn;
    float				lim_mx;
    float 				inc;
    int 				dec;
    					FloatValue		(LPCSTR key, float* value, float mn, float mx, float increment, int decimal, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):lim_mn(mn),lim_mx(mx),inc(increment),dec(decimal),PropValue(key,after,before,draw){clamp(*value,lim_mn,lim_mx); AppendValue(value);}
    virtual LPCSTR		GetText			()
    {
    	float draw_val 	= GetValue();
        if (OnDrawValue)OnDrawValue(this, &draw_val);
        static AnsiString draw_text;
		AnsiString fmt; fmt.sprintf("%%.%df",dec);
        draw_text.sprintf(fmt.c_str(),draw_val);
		return draw_text.c_str();
    }
    virtual void		InitNext		(LPVOID value){clamp(*(float*)value,lim_mn,lim_mx); if (*(float*)value!=*values.front()) bDiff=true; AppendValue((float*)value);}
    virtual bool		ApplyValue		(float value)
    {	
    	bool bChanged	= false;
        for (LPFloatIt it=values.begin();it!=values.end();it++){
        	if (**it!=value){
	        	**it 	= value;
                bChanged= true;
            }
        }
        if (bChanged) 	bDiff = false;
        return bChanged;
    }
    float 				GetValue		(){return *values.front();}
    void				ResetValue		(){FloatIt src=init_values.begin(); for (LPFloatIt it=values.begin();it!=values.end();it++,src++) **it = *src;}
};
//------------------------------------------------------------------------------

class ColorValue: public PropValue{
	FcolorVec			init_values;
	LPFcolorVec			values;
    void				AppendValue		(Fcolor* value){values.push_back(value);init_values.push_back(*value);}
public:
						ColorValue		(LPCSTR key, Fcolor* value, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):PropValue(key,after,before,draw){AppendValue(value);}
    virtual LPCSTR		GetText			(){return 0;}
    virtual void		InitNext		(LPVOID value){if (!((Fcolor*)value)->similar_rgba(*values.front())) bDiff=true; AppendValue((Fcolor*)value);}
    virtual bool		ApplyValue		(const Fcolor& value)
    {
    	bool bChanged	= false;
        for (LPFcolorIt it=values.begin();it!=values.end();it++){
        	if (!(*it)->similar_rgba(value)){
	        	**it 	= value;
                bChanged= true;
            }
        }
        if (bChanged) 	bDiff = false;
        return bChanged;
    }
    const Fcolor&		GetValue		(){return *values.front();}
    void				ResetValue		(){FcolorIt src=init_values.begin(); for (LPFcolorIt it=values.begin();it!=values.end();it++,src++) **it = *src;}
};
//------------------------------------------------------------------------------


class VectorValue: public PropValue{
	FvectorVec			init_values;
	LPFvectorVec		values;
    void				AppendValue		(Fvector* value){values.push_back(value);init_values.push_back(*value);}
public:
	float				lim_mn;
    float				lim_mx;
    float 				inc;
    int 				dec;
						VectorValue		(LPCSTR key, Fvector* value, float mn, float mx, float increment, int decimal, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):lim_mn(mn),lim_mx(mx),inc(increment),dec(decimal),PropValue(key,after,before,draw){AppendValue(value);}
    virtual LPCSTR		GetText			()
    {
		Fvector draw_val 	= GetValue();
        if (OnDrawValue)OnDrawValue(this, &draw_val);
        static AnsiString draw_text;
		AnsiString fmt; fmt.sprintf("{%%.%df, %%.%df, %%.%df}",dec,dec,dec);
        draw_text.sprintf(fmt.c_str(),draw_val.x,draw_val.y,draw_val.z);
		return draw_text.c_str();
    }
    virtual void		InitNext		(LPVOID value){if (!((Fvector*)value)->similar(*values.front())) bDiff=true; AppendValue((Fvector*)value);}
    virtual bool		ApplyValue		(const Fvector& value)
    {
    	bool bChanged	= false;
        for (LPFvectorIt it=values.begin();it!=values.end();it++){
        	if (!(*it)->similar(value)){
	        	**it 	= value;
                bChanged= true;
            }
        }
        if (bChanged) 	bDiff = false;
        return bChanged;
    }
    const Fvector&		GetValue		(){return *values.front();}
    void				ResetValue		(){FvectorIt src=init_values.begin(); for (LPFvectorIt it=values.begin();it!=values.end();it++,src++) **it = *src;}
};
//------------------------------------------------------------------------------

class FlagValue: public PropValue{
	DWORDVec			init_values;
	LPDWORDVec			values;
    void				AppendValue		(LPDWORD value){values.push_back(value);init_values.push_back(*value);}
public:
	DWORD				mask;
						FlagValue		(LPCSTR key, LPDWORD value, DWORD _mask, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):mask(_mask),PropValue(key,after,before,draw){AppendValue(value);}
    virtual LPCSTR		GetText			(){return 0;}
    virtual void		InitNext		(LPVOID value){bDiff=false; bool a=(*(DWORD*)value)&mask; bool b=(*values.front())&mask; if (a!=b) bDiff=true; AppendValue((DWORD*)value);}
    virtual bool		ApplyValue		(bool value)
    {
    	bool bChanged	= false;
    	for (LPDWORDIt it=values.begin();it!=values.end();it++){
        	if (value!=(!!(**it&mask))){
	        	if (value) **it|=mask; else **it&=~mask; 
                bChanged= true;
            }
        }
        if (bChanged) 	bDiff = false;
        return bChanged;
    }
    bool	 			GetValue		(){return (*values.front())&mask;}
    void				ResetValue		(){DWORDIt src=init_values.begin(); for (LPDWORDIt it=values.begin();it!=values.end();it++,src++) if ((*src)&mask) **it|=mask; else **it&=~mask;}
};
//------------------------------------------------------------------------------

class TokenValue: public PropValue{
	DWORDVec			init_values;
	LPDWORDVec			values;
    void				AppendValue		(LPDWORD value){values.push_back(value);init_values.push_back(*value);}
public:
	xr_token* 			token;
						TokenValue		(LPCSTR key, LPDWORD value, xr_token* _token, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):token(_token),PropValue(key,after,before,draw){AppendValue(value);}
	virtual LPCSTR 		GetText			();
    virtual void		InitNext		(LPVOID value)	{if (*(DWORD*)value!=*values.front()) bDiff=true; AppendValue((DWORD*)value);}
    bool				ApplyValue		(DWORD value)
    {
    	bool bChanged	= false;
        for (LPDWORDIt it=values.begin();it!=values.end();it++){
        	if (**it!=value){
	        	**it 	= value;
                bChanged= true;
            }
        }
        if (bChanged) 	bDiff = false;
        return bChanged;
    }
    DWORD 				GetValue		(){return *values.front();}
    void				ResetValue		(){DWORDIt src=init_values.begin(); for (LPDWORDIt it=values.begin();it!=values.end();it++,src++) **it = *src;}
};
//------------------------------------------------------------------------------

class TokenValue2: public PropValue{
	DWORDVec			init_values;
	LPDWORDVec			values;
    void				AppendValue		(LPDWORD value){values.push_back(value);init_values.push_back(*value);}
public:
	AStringVec 			items;
						TokenValue2		(LPCSTR key, LPDWORD value, AStringVec* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):items(*_items),PropValue(key,after,before,draw){AppendValue(value);}
	virtual LPCSTR 		GetText			();
    virtual void		InitNext		(LPVOID value)	{if (*(DWORD*)value!=*values.front()) bDiff=true; AppendValue((DWORD*)value);}
    bool				ApplyValue		(DWORD value)
    {
    	bool bChanged	= false;
        for (LPDWORDIt it=values.begin();it!=values.end();it++){
        	if (**it!=value){
	        	**it 	= value;
                bChanged= true;
            }
        }
        if (bChanged) 	bDiff = false;
        return bChanged;
    }
    DWORD 				GetValue		(){return *values.front();}
    void				ResetValue		(){DWORDIt src=init_values.begin(); for (LPDWORDIt it=values.begin();it!=values.end();it++,src++) **it = *src;}
};
//------------------------------------------------------------------------------

class TokenValue3: public PropValue{
	DWORDVec			init_values;
	LPDWORDVec			values;
    void				AppendValue		(LPDWORD value){values.push_back(value);init_values.push_back(*value);}
public:
	struct Item {
		DWORD		ID;
		string64	str;
	};
	DWORD				cnt;
    const Item*			items;
						TokenValue3		(LPCSTR key, LPDWORD value, DWORD _cnt, const Item* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):cnt(_cnt),items(_items),PropValue(key,after,before,draw){AppendValue(value);};
	virtual LPCSTR 		GetText			();
    virtual void		InitNext		(LPVOID value)	{if (*(DWORD*)value!=*values.front()) bDiff=true; AppendValue((DWORD*)value);}
    bool				ApplyValue		(DWORD value)
    {
    	bool bChanged	= false;
        for (LPDWORDIt it=values.begin();it!=values.end();it++){
        	if (**it!=value){
	        	**it 	= value;
                bChanged= true;
            }
        }
        if (bChanged) 	bDiff = false;
        return bChanged;
    }
    DWORD 				GetValue		(){return *values.front();}
    void				ResetValue		(){DWORDIt src=init_values.begin(); for (LPDWORDIt it=values.begin();it!=values.end();it++,src++) **it = *src;}
};
//------------------------------------------------------------------------------

class ListValue: public PropValue{
	AStringVec			init_values;
	LPSTRVec			values;
    void				AppendValue		(LPSTR value){values.push_back(value);init_values.push_back(value);}
public:
	AStringVec 			items;
						ListValue		(LPCSTR key, LPSTR value, AStringVec* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):items(*_items),PropValue(key,after,before,draw){AppendValue(value);};
						ListValue		(LPCSTR key, LPSTR value, DWORD cnt, LPCSTR* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):PropValue(key,after,before,draw){AppendValue(value); items.resize(cnt); int i=0; for (AStringIt it=items.begin(); it!=items.end(); it++,i++) *it=_items[i]; };
	virtual LPCSTR		GetText			();
    virtual void		InitNext		(LPVOID value){if (0!=strcmp((LPSTR)value,values.front())) bDiff=true; AppendValue((LPSTR)value);}
    virtual bool		ApplyValue		(LPCSTR value)
    {
    	bool bChanged	= false;
        for (LPSTRIt it=values.begin();it!=values.end();it++){
        	if (0!=strcmp(*it,value)){
	        	strcpy(*it,value);
                bChanged= true;
            }
        }
        if (bChanged) 	bDiff = false;
        return bChanged;
    }
    LPCSTR				GetValue		(){return values.front();}
    void				ResetValue		(){AStringIt src=init_values.begin(); for (LPSTRIt it=values.begin();it!=values.end();it++,src++) strcpy(*it,src->c_str());}
};
//------------------------------------------------------------------------------
DEFINE_MAP(LPSTR,PropValue*,PropValueMap,PropValuePairIt)

//---------------------------------------------------------------------------
namespace PROP{
    MarkerItem*			CreateMarkerValue	(PropValue* parent, LPCSTR key, LPCSTR value=0)
    {
        MarkerItem* V	= new MarkerItem(key,value);
        V->type			= PROP_MARKER;
        V->parent		= parent;
        return V;
    }
    IntValue* 			CreateIntValueValue	(PropValue* parent, LPCSTR key, int* value, int mn=0, int mx=100, int inc=1, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        IntValue* V		= new IntValue(key,value,mn,mx,inc,after,before,draw);
        V->type			= PROP_INTEGER;
        V->parent		= parent;
        return V;
    }
    DWORDValue* 		CreateDWORDValue	(PropValue* parent, LPCSTR key, LPDWORD value, int mx=100, int inc=1, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        DWORDValue* V	= new DWORDValue(key,value,mx,inc,after,before,draw);
        V->type			= PROP_DWORD;
        V->parent		= parent;
        return V;
    }
    FloatValue* 		CreateFloatValue	(PropValue* parent, LPCSTR key, float* value, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        FloatValue* V	= new FloatValue(key,value,mn,mx,inc,decim,after,before,draw);
        V->type			= PROP_FLOAT;
        V->parent		= parent;
        return V;
    }           	
    BOOLValue* 			CreateBOOLValue	(PropValue* parent, LPCSTR key, BOOL* value, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        BOOLValue* V	=new BOOLValue(key,value,after,before,draw);
        V->type			= PROP_BOOLEAN;
        V->parent		= parent;
        return V;
    }
    FlagValue* 			CreateFlagValue	(PropValue* parent, LPCSTR key, DWORD* value, DWORD mask, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        FlagValue* V	= new FlagValue(key,value,mask,after,before,draw);
        V->type			= PROP_FLAG;
        V->parent		= parent;
        return V;
    }
    VectorValue* 		CreateVectorValue(PropValue* parent, LPCSTR key, Fvector* value, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        VectorValue* V	= new VectorValue(key,value,mn,mx,inc,decim,after,before,draw);
        V->type			= PROP_VECTOR;
        V->parent		= parent;
        return V;
    }
	TokenValue* 		CreateTokenValue	(PropValue* parent, LPCSTR key, LPDWORD value, xr_token* token, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TokenValue* V	= new TokenValue(key,value,token,after,before,draw);
        V->type			= PROP_TOKEN;
        V->parent		= parent;
        return V;
    }
	TokenValue2* 		CreateTokenValue	(PropValue* parent, LPCSTR key, LPDWORD value, AStringVec* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TokenValue2* V	= new TokenValue2(key,value,lst,after,before,draw);
        V->type			= PROP_TOKEN2;
        V->parent		= parent;
        return V;
    }
	TokenValue3* 		CreateTokenValue	(PropValue* parent, LPCSTR key, LPDWORD value, DWORD cnt, const TokenValue3::Item* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TokenValue3* V	= new TokenValue3(key,value,cnt,lst,after,before,draw);
        V->type			= PROP_TOKEN3;
        V->parent		= parent;
        return V;
    }
	ListValue* 			CreateListValue	(PropValue* parent, LPCSTR key, LPSTR value, AStringVec* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        ListValue* V	= new ListValue(key,value,lst,after,before,draw);
        V->type			= PROP_LIST;
        V->parent		= parent;
        return V;
    }
	ListValue* 			CreateListValueA	(PropValue* parent, LPCSTR key, LPSTR value, DWORD cnt, LPCSTR* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        ListValue* V	= new ListValue(key,value,cnt,lst,after,before,draw);
        V->type			= PROP_LIST;
        V->parent		= parent;
        return V;
    }
    DWORDValue* 		CreateColorValue	(PropValue* parent, LPCSTR key, LPDWORD value, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        DWORDValue* V	= new DWORDValue(key,value,0xffffffff,0,after,before,draw);
        V->type			= PROP_COLOR;
        V->parent		= parent;
        return V;
    }
    ColorValue*			CreateColorValue	(PropValue* parent, LPCSTR key, Fcolor* value, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        ColorValue* V	= new ColorValue(key,value,after,before,draw);
        V->type			= PROP_COLOR;
        V->parent		= parent;
        return V;
    }
	TextValue* 			CreateTextValue	(PropValue* parent, LPCSTR key, LPSTR value, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TextValue* V	= new TextValue(key,value,lim,after,before,draw);
        V->type			= PROP_TEXT;
        V->parent		= parent;
        return V;
    }
	ATextValue* 		CreateATextValue	(PropValue* parent, LPCSTR key, AnsiString* value, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        ATextValue* V	= new ATextValue(key,value,after,before,draw);
        V->type			= PROP_A_TEXT;
        V->parent		= parent;
        return V;
    }
	TextValue* 			CreateEShaderValue(PropValue* parent, LPCSTR key, LPSTR value, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TextValue* V	= new TextValue(key,value,lim,after,before,draw);
        V->type			= PROP_ESHADER;
        V->parent		= parent;
        return V;
    }
	TextValue* 			CreateCShaderValue(PropValue* parent, LPCSTR key, LPSTR value, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TextValue* V	= new TextValue(key,value,lim,after,before,draw);
        V->type			= PROP_CSHADER;
        V->parent		= parent;
        return V;
    }
	TextValue* 			CreateTextureValue(PropValue* parent, LPCSTR key, LPSTR value, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TextValue* V	= new TextValue(key,value,lim,after,before,draw);
        V->type			= PROP_TEXTURE;
        V->parent		= parent;
        return V;
    }
	TextValue* 			CreateTexture2Value(PropValue* parent, LPCSTR key, LPSTR value, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TextValue* V	= new TextValue(key,value,lim,after,before,draw);
        V->type			= PROP_TEXTURE2;
        V->parent		= parent;
        return V;
    }
	ATextValue* 		CreateAEShaderValue(PropValue* parent, LPCSTR key, AnsiString* value, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        ATextValue* V	= new ATextValue(key,value,after,before,draw);
        V->type			= PROP_A_ESHADER;
        V->parent		= parent;
        return V;
    }
	ATextValue* 		CreateACShaderValue(PropValue* parent, LPCSTR key, AnsiString* value, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        ATextValue* V	= new ATextValue(key,value,after,before,draw);
        V->type			= PROP_A_CSHADER;
        V->parent		= parent;
        return V;
    }
	ATextValue* 		CreateATextureValue(PropValue* parent, LPCSTR key, AnsiString* value, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        ATextValue* V	= new ATextValue(key,value,after,before,draw);
        V->type			= PROP_A_TEXTURE;
        V->parent		= parent;
        return V;
    }
	TextValue*			CreateLightAnimValue(PropValue* parent, LPCSTR key, LPSTR value, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TextValue* V	= new TextValue(key,value,lim,after,before,draw);
        V->type			= PROP_LIGHTANIM;
        V->parent		= parent;
        return V;
    }
	TextValue* 			CreateLibObjectValue(PropValue* parent, LPCSTR key, LPSTR value, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TextValue* V	= new TextValue(key,value,lim,after,before,draw);
        V->type			= PROP_LIBOBJECT;
        V->parent		= parent;
        return V;
    }
	TextValue* 			CreateEntityValue	(PropValue* parent, LPCSTR key, LPSTR value, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TextValue* V	= new TextValue(key,value,lim,after,before,draw);
        V->type			= PROP_ENTITY;
        V->parent		= parent;
        return V;
    }
	WaveValue* 			CreateWaveValue		(PropValue* parent, LPCSTR key, WaveForm* value, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        WaveValue* V	= new WaveValue(key,value,after,before,draw);
        V->type			= PROP_WAVE;
        V->parent		= parent;
        return V;
    }     
    LPCSTR 				MakeFullKey			(PropValue* v, AnsiString& dest)
    {
    	for (PropValue* p=v; p; p=p->parent){
        	if (!dest.IsEmpty()) dest.Insert("\\",1);
        	dest.Insert	(p->GetKey(),1);
        }
        return dest.c_str();
    }
    PropValue* 			FirstInit			(PropValueMap& values,	LPCSTR key,	LPVOID val, PropValue* v)
    {
    	AnsiString		key;
        pair<PropValuePairIt, bool> R;
    	R=values.insert	(make_pair(strdup(MakeFullKey(v,key)),v));
        if (!R.second)	Device.Fatal("PropValue: '%s' already exist.",key.c_str());
        return v;
    }
}
//---------------------------------------------------------------------------
#endif


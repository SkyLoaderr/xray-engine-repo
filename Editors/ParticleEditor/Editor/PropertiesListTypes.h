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
public:
	// internal use only
    LPSTR				key;
    EPropType			type;
	TElTreeItem*		item; 
    bool				bEnabled;
public:
	// base events
    TAfterEdit			OnAfterEdit;
    TBeforeEdit			OnBeforeEdit;
    TOnDrawValue		OnDrawValue;
public:
						PropValue		(TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):
                        				item(0),key(0),/*parent(0),*/bEnabled(true),bDiff(false),OnAfterEdit(after),
                                        OnBeforeEdit(before),OnDrawValue(draw){};
	virtual 			~PropValue		(){_FREE(key);};
    virtual LPCSTR		GetText			()=0;
    virtual void		InitFirst		(LPVOID value)=0;
    virtual void		InitNext		(LPVOID value)=0;
    virtual void		ResetValue		()=0;
    bool				IsDiffValues	(){return bDiff;}
    void				SetName			(LPCSTR name){key=strdup(name);}
//    TElTreeItem*		GetParentItem	(){return parent?parent->item:0;}
};
//------------------------------------------------------------------------------

class MarkerItem: public PropValue{
	AStringVec			init_values;
	AStringVec			values;
    void				AppendValue		(LPCSTR value){values.push_back(value);init_values.push_back(value);}
public:
						MarkerItem		():PropValue(TAfterEdit(0),TBeforeEdit(0),TOnDrawValue(0)){};
    virtual LPCSTR		GetText			(){return values.front().c_str();}
    virtual void		InitFirst		(LPVOID value){R_ASSERT(values.empty()); AppendValue((LPCSTR)value);}
    virtual void		InitNext		(LPVOID value){};
    virtual	void		ResetValue		(){;}
};

class TextValue: public PropValue{
	AStringVec			init_values;
	LPSTRVec			values;
    void				AppendValue		(LPSTR value){values.push_back(value);init_values.push_back(value);}
public:
	int					lim;
						TextValue		(int _lim, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):lim(_lim),PropValue(after,before,draw){};
    virtual LPCSTR		GetText			();
    virtual void		InitFirst		(LPVOID value){R_ASSERT(values.empty()); AppendValue((LPSTR)value);}
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
						ATextValue		(TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):PropValue(after,before,draw){};
    virtual LPCSTR		GetText			();
    virtual void		InitFirst		(LPVOID value){R_ASSERT(values.empty()); AppendValue((AnsiString*)value);}
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
						BOOLValue		(TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):PropValue(after,before,draw){};
    virtual LPCSTR		GetText			(){return 0;}
    virtual void		InitFirst		(LPVOID value){R_ASSERT(values.empty()); AppendValue((BOOL*)value);}
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
						WaveValue		(TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):PropValue(after,before,draw){};
    virtual LPCSTR		GetText			(){return "[Wave]";}
    virtual void		InitFirst		(LPVOID value){R_ASSERT(values.empty()); AppendValue((WaveForm*)value);}
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
    void				AppendValue		(LPDWORD value){clamp(*(DWORD*)value,0ul,lim_mx); values.push_back(value);init_values.push_back(*value);}
public:
    DWORD				lim_mx;
    DWORD 				inc;
public:
						DWORDValue		(DWORD mx, DWORD increm, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):lim_mx(mx),inc(increm),PropValue(after,before,draw){};
    virtual LPCSTR		GetText			(){
    	DWORD draw_val 	= GetValue();
        if (OnDrawValue)OnDrawValue(this, &draw_val);
		static AnsiString draw_text;
        draw_text		= draw_val;
        return draw_text.c_str();
    }
    virtual void		InitFirst		(LPVOID value){R_ASSERT(values.empty()); AppendValue((DWORD*)value);}
    virtual void		InitNext		(LPVOID value){if (*(DWORD*)value!=*values.front()) bDiff=true; AppendValue((DWORD*)value);}
    virtual bool		ApplyValue		(DWORD value)
    {
    	clamp(value,0ul,lim_mx); 
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
    void				AppendValue		(int* value){clamp(*(int*)value,lim_mn,lim_mx); values.push_back(value);init_values.push_back(*value);}
public:
	int					lim_mn;
    int					lim_mx;
    int 				inc;
    					IntValue		(int mn, int mx, int increm, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):lim_mn(mn),lim_mx(mx),inc(increm),PropValue(after,before,draw){};
    virtual LPCSTR		GetText			()
    {
    	int draw_val 	= GetValue();
        if (OnDrawValue)OnDrawValue(this, &draw_val);
		static AnsiString draw_text;
        draw_text		= draw_val;
        return draw_text.c_str();
    }
    virtual void		InitFirst		(LPVOID value){R_ASSERT(values.empty()); AppendValue((int*)value);}
    virtual void		InitNext		(LPVOID value){if (*(int*)value!=*values.front()) bDiff=true; AppendValue((int*)value);}
    virtual bool		ApplyValue		(int value)
    {
    	clamp(value,lim_mn,lim_mx); 
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
    void				AppendValue		(float* value){clamp(*(float*)value,lim_mn,lim_mx); values.push_back(value);init_values.push_back(*value);}
public:
	float				lim_mn;
    float				lim_mx;
    float 				inc;
    int 				dec;
    					FloatValue		(float mn, float mx, float increment, int decimal, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):lim_mn(mn),lim_mx(mx),inc(increment),dec(decimal),PropValue(after,before,draw){};
    virtual LPCSTR		GetText			()
    {
    	float draw_val 	= GetValue();
        if (OnDrawValue)OnDrawValue(this, &draw_val);
        static AnsiString draw_text;
		AnsiString fmt; fmt.sprintf("%%.%df",dec);
        draw_text.sprintf(fmt.c_str(),draw_val);
		return draw_text.c_str();
    }
    virtual void		InitFirst		(LPVOID value){R_ASSERT(values.empty()); AppendValue((float*)value);}
    virtual void		InitNext		(LPVOID value){if (*(float*)value!=*values.front()) bDiff=true; AppendValue((float*)value);}
    virtual bool		ApplyValue		(float value)
    {	
	    clamp(value,lim_mn,lim_mx); 
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
						ColorValue		(TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):PropValue(after,before,draw){};
    virtual LPCSTR		GetText			(){return 0;}
    virtual void		InitFirst		(LPVOID value){R_ASSERT(values.empty()); AppendValue((Fcolor*)value);}
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
    void				AppendValue		(Fvector* value)
    {
    	clamp(value->x,lim_mn,lim_mx); 
    	clamp(value->y,lim_mn,lim_mx); 
    	clamp(value->z,lim_mn,lim_mx); 
        values.push_back(value);
        init_values.push_back(*value);
    }
public:
	float				lim_mn;
    float				lim_mx;
    float 				inc;
    int 				dec;
						VectorValue		(float mn, float mx, float increment, int decimal, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):lim_mn(mn),lim_mx(mx),inc(increment),dec(decimal),PropValue(after,before,draw){};
    virtual LPCSTR		GetText			()
    {
		Fvector draw_val 	= GetValue();
        if (OnDrawValue)OnDrawValue(this, &draw_val);
        static AnsiString draw_text;
		AnsiString fmt; fmt.sprintf("{%%.%df, %%.%df, %%.%df}",dec,dec,dec);
        draw_text.sprintf(fmt.c_str(),draw_val.x,draw_val.y,draw_val.z);
		return draw_text.c_str();
    }
    virtual void		InitFirst		(LPVOID value){R_ASSERT(values.empty()); AppendValue((Fvector*)value);}
    virtual void		InitNext		(LPVOID value){if (!((Fvector*)value)->similar(*values.front())) bDiff=true; AppendValue((Fvector*)value);}
    virtual bool		ApplyValue		(const Fvector& val)
    {
    	Fvector value	= val;
    	clamp(value.x,lim_mn,lim_mx); 
    	clamp(value.y,lim_mn,lim_mx); 
    	clamp(value.z,lim_mn,lim_mx); 
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
						FlagValue		(DWORD _mask, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):mask(_mask),PropValue(after,before,draw){};
    virtual LPCSTR		GetText			(){return 0;}
    virtual void		InitFirst		(LPVOID value){R_ASSERT(values.empty()); AppendValue((DWORD*)value);}
    virtual void		InitNext		(LPVOID value){bDiff=false; bool a=(*(DWORD*)value)&mask; bool b=(*values.front())&mask; if (a!=b) bDiff=true; AppendValue((DWORD*)value);}
    virtual bool		ApplyValue		(BOOL value)
    {
    	bool bChanged	= false;
    	for (LPDWORDIt it=values.begin();it!=values.end();it++){
        	if ((!!value)!=(!!(**it&mask))){
	        	if (value) **it|=mask; else **it&=~mask; 
                bChanged= true;
            }
        }
        if (bChanged) 	bDiff = false;
        return bChanged;
    }
    BOOL	 			GetValue		(){return !!((*values.front())&mask);}
    void				ResetValue		(){DWORDIt src=init_values.begin(); for (LPDWORDIt it=values.begin();it!=values.end();it++,src++) if ((*src)&mask) **it|=mask; else **it&=~mask;}
};
//------------------------------------------------------------------------------

class TokenValue: public PropValue{
	DWORDVec			init_values;
	LPDWORDVec			values;
    void				AppendValue		(LPDWORD value){values.push_back(value);init_values.push_back(*value);}
public:
	xr_token* 			token;
						TokenValue		(xr_token* _token, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):token(_token),PropValue(after,before,draw){};
	virtual LPCSTR 		GetText			();
    virtual void		InitFirst		(LPVOID value){R_ASSERT(values.empty()); AppendValue((DWORD*)value);}
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
						TokenValue2		(AStringVec* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):items(*_items),PropValue(after,before,draw){};
	virtual LPCSTR 		GetText			();
    virtual void		InitFirst		(LPVOID value){R_ASSERT(values.empty()); AppendValue((DWORD*)value);}
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
						TokenValue3		(DWORD _cnt, const Item* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):cnt(_cnt),items(_items),PropValue(after,before,draw){};
	virtual LPCSTR 		GetText			();
    virtual void		InitFirst		(LPVOID value){R_ASSERT(values.empty()); AppendValue((DWORD*)value);}
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
						ListValue		(AStringVec* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):items(*_items),PropValue(after,before,draw){};
						ListValue		(DWORD cnt, LPCSTR* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):PropValue(after,before,draw){items.resize(cnt); int i=0; for (AStringIt it=items.begin(); it!=items.end(); it++,i++) *it=_items[i]; };
	virtual LPCSTR		GetText			();
    virtual void		InitFirst		(LPVOID value){R_ASSERT(values.empty()); AppendValue((LPSTR)value);}
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
DEFINE_VECTOR(PropValue*,PropValueVec,PropValueIt);

//---------------------------------------------------------------------------
namespace PROP{
    MarkerItem*			CreateMarkerValue	()
    {
        MarkerItem* V	= new MarkerItem();
        V->type			= PROP_MARKER;
        return V;
    }
    IntValue* 			CreateIntValue		(int mn=0, int mx=100, int inc=1, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        IntValue* V		= new IntValue(mn,mx,inc,after,before,draw);
        V->type			= PROP_INTEGER;
        return V;
    }
    DWORDValue* 		CreateDWORDValue	(int mx=100, int inc=1, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        DWORDValue* V	= new DWORDValue(mx,inc,after,before,draw);
        V->type			= PROP_DWORD;
        return V;
    }
    FloatValue* 		CreateFloatValue	(float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        FloatValue* V	= new FloatValue(mn,mx,inc,decim,after,before,draw);
        V->type			= PROP_FLOAT;
        return V;
    }           	
    BOOLValue* 			CreateBOOLValue		(TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        BOOLValue* V	=new BOOLValue(after,before,draw);
        V->type			= PROP_BOOLEAN;
        return V;
    }
    FlagValue* 			CreateFlagValue		(DWORD mask, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        FlagValue* V	= new FlagValue(mask,after,before,draw);
        V->type			= PROP_FLAG;
        return V;
    }
    VectorValue* 		CreateVectorValue	(float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        VectorValue* V	= new VectorValue(mn,mx,inc,decim,after,before,draw);
        V->type			= PROP_VECTOR;
        return V;
    }
	TokenValue* 		CreateTokenValue	(xr_token* token, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TokenValue* V	= new TokenValue(token,after,before,draw);
        V->type			= PROP_TOKEN;
        return V;
    }
	TokenValue2* 		CreateTokenValue2	(AStringVec* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TokenValue2* V	= new TokenValue2(lst,after,before,draw);
        V->type			= PROP_TOKEN2;
        return V;
    }
	TokenValue3* 		CreateTokenValue3	(DWORD cnt, const TokenValue3::Item* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TokenValue3* V	= new TokenValue3(cnt,lst,after,before,draw);
        V->type			= PROP_TOKEN3;
        return V;
    }
	ListValue* 			CreateListValue		(AStringVec* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        ListValue* V	= new ListValue(lst,after,before,draw);
        V->type			= PROP_LIST;
        return V;
    }
	ListValue* 			CreateListValueA	(DWORD cnt, LPCSTR* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        ListValue* V	= new ListValue(cnt,lst,after,before,draw);
        V->type			= PROP_LIST;
        return V;
    }
    DWORDValue* 		CreateColorValue	(TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        DWORDValue* V	= new DWORDValue(0xffffffff,0,after,before,draw);
        V->type			= PROP_COLOR;
        return V;
    }
    ColorValue*			CreateFColorValue	(TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        ColorValue* V	= new ColorValue(after,before,draw);
        V->type			= PROP_FCOLOR;
        return V;
    }
	TextValue* 			CreateTextValue		(int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TextValue* V	= new TextValue(lim,after,before,draw);
        V->type			= PROP_TEXT;
        return V;
    }
	ATextValue* 		CreateATextValue	(TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        ATextValue* V	= new ATextValue(after,before,draw);
        V->type			= PROP_A_TEXT;
        return V;
    }
	TextValue* 			CreateEShaderValue	(int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TextValue* V	= new TextValue(lim,after,before,draw);
        V->type			= PROP_ESHADER;
        return V;
    }
	TextValue* 			CreateCShaderValue	(int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TextValue* V	= new TextValue(lim,after,before,draw);
        V->type			= PROP_CSHADER;
        return V;
    }
	TextValue* 			CreateTextureValue	(int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TextValue* V	= new TextValue(lim,after,before,draw);
        V->type			= PROP_TEXTURE;
        return V;
    }
	TextValue* 			CreateTexture2Value	(int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TextValue* V	= new TextValue(lim,after,before,draw);
        V->type			= PROP_TEXTURE2;
        return V;
    }
	ATextValue* 		CreateAEShaderValue	(TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        ATextValue* V	= new ATextValue(after,before,draw);
        V->type			= PROP_A_ESHADER;
        return V;
    }
	ATextValue* 		CreateACShaderValue	(TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        ATextValue* V	= new ATextValue(after,before,draw);
        V->type			= PROP_A_CSHADER;
        return V;
    }
	ATextValue* 		CreateATextureValue	(TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        ATextValue* V	= new ATextValue(after,before,draw);
        V->type			= PROP_A_TEXTURE;
        return V;
    }
	TextValue*			CreateLightAnimValue(int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TextValue* V	= new TextValue(lim,after,before,draw);
        V->type			= PROP_LIGHTANIM;
        return V;
    }
	TextValue* 			CreateLibObjectValue(int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TextValue* V	= new TextValue(lim,after,before,draw);
        V->type			= PROP_LIBOBJECT;
        return V;
    }
	TextValue* 			CreateEntityValue	(int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        TextValue* V	= new TextValue(lim,after,before,draw);
        V->type			= PROP_ENTITY;
        return V;
    }
	WaveValue* 			CreateWaveValue		(TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0)
    {
        WaveValue* V	= new WaveValue(after,before,draw);
        V->type			= PROP_WAVE;
        return V;
    }     
//------------------------------------------------------------------------------
    IC PropValue* 		InitFirst			(PropValueVec& values,	LPCSTR key,	LPVOID val, PropValue* v)
    {
    	R_ASSERT		(v);
    	v->SetName		(key);
    	values.push_back(v);
        v->InitFirst	(val);
        return v;
    }
    IC PropValue* 		FindProp			(PropValueVec& values,	LPCSTR key)
    {
    	for (PropValueIt it=values.begin(); it!=values.end(); it++)
        	if (0==strcmp((*it)->key,key)) return *it;
        return 0;
    }
    IC PropValue* 		InitNext			(PropValue* v, LPVOID val)
	{
    	R_ASSERT		(v);
        v->InitNext		(val);
        return v;
    }
}
#define FILL_PROP(A,K,V,P){PropValue* prop=PROP::FindProp(A,K); if (prop) PROP::InitNext(prop,V); else PROP::InitFirst(A,K,V,P);}
#define FILL_PROP_EX(A,X,K,V,P){AnsiString XK=AnsiString(X)+"\\"+AnsiString(K); PropValue* prop=PROP::FindProp(A,XK.c_str()); if (prop) PROP::InitNext(prop,V); else PROP::InitFirst(A,XK.c_str(),V,P);}
//---------------------------------------------------------------------------
#endif





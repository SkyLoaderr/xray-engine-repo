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
	bool				bDiff;
public:
	TElTreeItem*		item;
    EPropType			type;
public:
    TAfterEdit			OnAfterEdit;
    TBeforeEdit			OnBeforeEdit;
    TOnDrawValue		OnDrawValue;
public:
						PropValue		(TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):
                        				item(0),bDiff(false),OnAfterEdit(after),
                                        OnBeforeEdit(before),OnDrawValue(draw){};
	virtual 			~PropValue		(){};
    virtual LPCSTR		GetText			()=0;
    virtual void		InitNext		(LPVOID value)=0;
    virtual void		ResetValue		()=0;
    bool				IsDiffValues	(){return bDiff;}
};
//------------------------------------------------------------------------------

class MarkerItem: public PropValue{
	AnsiString			value;
public:
						MarkerItem		(LPCSTR val):PropValue(TAfterEdit(0),TBeforeEdit(0),TOnDrawValue(0)),value(val){};
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
						TextValue		(LPSTR value, int _lim, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):lim(_lim),PropValue(after,before,draw){AppendValue(value);};
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

class AnsiTextValue: public PropValue{
	AStringVec			init_values;
	LPAStringVec		values;
    void				AppendValue		(AnsiString* value){values.push_back(value);init_values.push_back(*value);}
public:
						AnsiTextValue	(AnsiString* value, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):PropValue(after,before,draw){AppendValue(value);};
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
						BOOLValue		(LPBOOL value, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):PropValue(after,before,draw){AppendValue(value);};
    virtual LPCSTR		GetText			();
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
						WaveValue		(WaveForm* value, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):PropValue(after,before,draw){AppendValue(value);};
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
						DWORDValue		(DWORD* value, DWORD mx, DWORD increm, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):lim_mx(mx),inc(increm),PropValue(after,before,draw){clamp(*value,0ul,lim_mx); AppendValue(value);};
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
    					IntValue		(int* value, int mn, int mx, int increm, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):lim_mn(mn),lim_mx(mx),inc(increm),PropValue(after,before,draw){clamp(*value,lim_mn,lim_mx); AppendValue(value);};
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
    					FloatValue		(float* value, float mn, float mx, float increment, int decimal, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):lim_mn(mn),lim_mx(mx),inc(increment),dec(decimal),PropValue(after,before,draw){clamp(*value,lim_mn,lim_mx); AppendValue(value);}
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
						ColorValue		(Fcolor* value, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):PropValue(after,before,draw){AppendValue(value);}
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
						VectorValue		(Fvector* value, float mn, float mx, float increment, int decimal, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):lim_mn(mn),lim_mx(mx),inc(increment),dec(decimal),PropValue(after,before,draw){AppendValue(value);}
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
						FlagValue		(LPDWORD value, DWORD _mask, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):mask(_mask),PropValue(after,before,draw){AppendValue(value);}
    virtual LPCSTR		GetText			(){return 0;}
    virtual void		InitNext		(LPVOID value){bDiff=false; bool a=(*(DWORD*)value)&mask; bool b=(*values.front())&mask; if (a!=b) bDiff=true; AppendValue((DWORD*)value);}
    virtual bool		ApplyValue		(bool value)
    {
    	bool bChanged	= false;
    	for (LPDWORDIt it=values.begin();it!=values.end();it++){
        	if (value!=(**it&mask)){
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
						TokenValue		(LPDWORD value, xr_token* _token, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):token(_token),PropValue(after,before,draw){AppendValue(value);}
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
						TokenValue2		(LPDWORD value, AStringVec* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):items(*_items),PropValue(after,before,draw){AppendValue(value);}
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
						TokenValue3		(LPDWORD value, DWORD _cnt, const Item* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):cnt(_cnt),items(_items),PropValue(after,before,draw){AppendValue(value);};
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
						ListValue		(LPSTR value, AStringVec* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):items(*_items),PropValue(after,before,draw){AppendValue(value);};
						ListValue		(LPSTR value, DWORD cnt, LPCSTR* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw):PropValue(after,before,draw){AppendValue(value); items.resize(cnt); int i=0; for (AStringIt it=items.begin(); it!=items.end(); it++,i++) *it=_items[i]; };
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
DEFINE_VECTOR(PropValue*,PropValueVec,PropValueIt)

//---------------------------------------------------------------------------
#endif

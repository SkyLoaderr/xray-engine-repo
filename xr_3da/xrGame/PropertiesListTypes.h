//---------------------------------------------------------------------------
#ifndef PropertiesListTypesH
#define PropertiesListTypesH

#include "ElTree.hpp"
//---------------------------------------------------------------------------
enum EPropType{
	PROP_MARKER	= 0x1000,
	PROP_S8,
	PROP_S16,
	PROP_S32,
	PROP_U8,
	PROP_U16,
	PROP_U32,
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
    PROP_LIBSOUND,
    PROP_GAMEOBJECT,
    PROP_ENTITY,
	PROP_WAVE 			
};
// refs
struct 	xr_token;
class PropValue;

//------------------------------------------------------------------------------
typedef void 	__fastcall (__closure *TBeforeEdit)		(PropValue* sender, LPVOID edit_val);
typedef void 	__fastcall (__closure *TAfterEdit)		(PropValue* sender, LPVOID edit_val);
typedef void 	__fastcall (__closure *TOnDrawValue)	(PropValue* sender, LPVOID draw_val);
typedef void 	__fastcall (__closure *TOnChange)		(PropValue* sender);
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
    int 				tag;
public:
	// base events
    TAfterEdit			OnAfterEdit;
    TBeforeEdit			OnBeforeEdit;
    TOnDrawValue		OnDrawValue;
    TOnChange			OnChange;
public:
						PropValue		(TAfterEdit after, TBeforeEdit before, TOnDrawValue draw, TOnChange change):
                        				item(0),key(0),tag(0),bEnabled(true),bDiff(false),OnAfterEdit(after),
                                        OnBeforeEdit(before),OnDrawValue(draw),OnChange(change){};
	virtual 			~PropValue		(){_FREE(key);};
    virtual LPCSTR		GetText			()=0;
    virtual void		InitFirst		(LPVOID value)=0;
    virtual void		InitNext		(LPVOID value)=0;
    virtual void		ResetValue		()=0;
    bool				IsDiffValues	(){return bDiff;}
    void				SetName			(LPCSTR name){key=xr_strdup(name);}
//    TElTreeItem*		GetParentItem	(){return parent?parent->item:0;}
};
//------------------------------------------------------------------------------

class MarkerItem: public PropValue{
	AStringVec			init_values;
	AStringVec			values;
    void				AppendValue		(LPCSTR value){values.push_back(value);init_values.push_back(value);}
public:
						MarkerItem		():PropValue(TAfterEdit(0),TBeforeEdit(0),TOnDrawValue(0),TOnChange(0)){};
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
						TextValue		(int _lim, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw, TOnChange change):lim(_lim),PropValue(after,before,draw,change){};
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
    virtual void		ResetValue		(){
    	AStringIt src=init_values.begin(); for (LPSTRIt it=values.begin();it!=values.end();it++,src++) strcpy(*it,src->c_str());
    }
};
//------------------------------------------------------------------------------

class ATextValue: public PropValue{
	AStringVec			init_values;
	LPAStringVec		values;
    void				AppendValue		(AnsiString* value){values.push_back(value);init_values.push_back(*value);}
public:
						ATextValue		(TAfterEdit after, TBeforeEdit before, TOnDrawValue draw, TOnChange change):PropValue(after,before,draw,change){};
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
    const AnsiString&	GetValue		(){return *values.front();}
    virtual void		ResetValue		(){AStringIt src=init_values.begin(); for (LPAStringIt it=values.begin();it!=values.end();it++,src++) **it=*src;}
};
//------------------------------------------------------------------------------

class BOOLValue: public PropValue{
	BOOLVec				init_values;
	LPBOOLVec			values;
    void				AppendValue		(LPBOOL value){values.push_back(value);init_values.push_back(*value);}
public:
						BOOLValue		(TAfterEdit after, TBeforeEdit before, TOnDrawValue draw, TOnChange change):PropValue(after,before,draw,change){};
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
    virtual void		ResetValue		(){BOOLIt src=init_values.begin(); for (LPBOOLIt it=values.begin();it!=values.end();it++,src++) **it = *src;}
};
//------------------------------------------------------------------------------

DEFINE_VECTOR(WaveForm,WaveFormVec,WaveFormIt);
DEFINE_VECTOR(WaveForm*,LPWaveFormVec,LPWaveFormIt);

class WaveValue: public PropValue{
	WaveFormVec			init_values;
	LPWaveFormVec		values;
    void				AppendValue		(WaveForm* value){values.push_back(value);init_values.push_back(*value);}
public:
						WaveValue		(TAfterEdit after, TBeforeEdit before, TOnDrawValue draw, TOnChange change):PropValue(after,before,draw,change){};
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
    virtual void		ResetValue		(){WaveFormIt src=init_values.begin(); for (LPWaveFormIt it=values.begin();it!=values.end();it++,src++) **it = *src;}
};
//------------------------------------------------------------------------------

template <class T>
class IntegerValue
{
    void				AppendValue		(T* value)
    {
    	clamp(*value,lim_mn,lim_mx); 
        values.push_back(value);
        init_values.push_back(*value);
    }
public:
	vector<T>			init_values;
	vector<T*>			values;
    T					lim_mn;
    T					lim_mx;
    T 					inc;
public:    
						IntegerValue	(T mn, T mx, T increm):lim_mn(mn),lim_mx(mx),inc(increm){};
    LPCSTR				_GetText		(PropValue* prop,TOnDrawValue OnDraw)
    {
    	T draw_val 		= 	_GetValue();
        if (OnDraw)		OnDraw(prop, &draw_val);
		static AnsiString draw_text;
        draw_text		= draw_val;
        return draw_text.c_str();
    }
    void				_InitFirst		(T* value)
    {
    	R_ASSERT(values.empty()); 
        AppendValue((T*)value);
    }
    void				_InitNext		(T* value, bool& bDiff)
    {
    	if (*value!=*values.front()) bDiff=true; 
        AppendValue(value);
    }
    bool				_ApplyValue		(T value, bool& bDiff)
    {
    	clamp(value,lim_mn,lim_mx); 
    	bool bChanged	= false;
        for (vector<T*>::iterator it=values.begin();it!=values.end();it++){
        	if (**it!=value){
	        	**it 	= value;
                bChanged= true;
            }
        }
        if (bChanged) 	bDiff = false;
        return bChanged;
    }
    T 					_GetValue		()
    {
    	return *values.front();
    }
    void				_ResetValue		()
    {
    	vector<T>::iterator src=init_values.begin(); 
        for (vector<T*>::iterator it=values.begin();it!=values.end();it++,src++) 
        	**it = *src;
    }
};

class U8Value: public PropValue, public IntegerValue<u8> {
public:
						U8Value			(u8 mn, u8 mx, u8 increm, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw, TOnChange change):IntegerValue<u8>(mn,mx,increm),PropValue(after,before,draw,change){};
    virtual LPCSTR		GetText			()				{ return _GetText(this,OnDrawValue); }
    virtual void		InitFirst		(LPVOID value)	{ _InitFirst((u8*)value); }
    virtual void		InitNext		(LPVOID value)	{ _InitNext((u8*)value,bDiff); }
    virtual bool		ApplyValue		(u8 value)		{ return _ApplyValue(value,bDiff);}
    u8 					GetValue		()				{ return _GetValue(); }
    virtual void		ResetValue		()    			{ _ResetValue(); }
};
//------------------------------------------------------------------------------
class U16Value: public PropValue, public IntegerValue<u16> {
public:
						U16Value	 	(u16 mn, u16 mx, u16 increm, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw, TOnChange change):IntegerValue<u16>(mn,mx,increm),PropValue(after,before,draw,change){};
    virtual LPCSTR		GetText			()				{ return _GetText(this,OnDrawValue); }
    virtual void		InitFirst		(LPVOID value)	{ _InitFirst((u16*)value); }
    virtual void		InitNext		(LPVOID value)	{ _InitNext((u16*)value,bDiff); }
    virtual bool		ApplyValue		(u16 value)		{ return _ApplyValue(value,bDiff);}
    u16					GetValue		()				{ return _GetValue(); }
    virtual void		ResetValue		()    			{ _ResetValue(); }
};
//------------------------------------------------------------------------------
class U32Value: public PropValue, public IntegerValue<u32> {
public:
						U32Value  		(u32 mn, u32 mx, u32 increm, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw, TOnChange change):IntegerValue<u32>(mn,mx,increm),PropValue(after,before,draw,change){};
    virtual LPCSTR		GetText			()				{ return _GetText(this,OnDrawValue); }
    virtual void		InitFirst		(LPVOID value)	{ _InitFirst((u32*)value); }
    virtual void		InitNext		(LPVOID value)	{ _InitNext((u32*)value,bDiff); }
    virtual bool		ApplyValue		(u32 value)		{ return _ApplyValue(value,bDiff);}
    u32					GetValue		()				{ return _GetValue(); }
    virtual void		ResetValue		()    			{ _ResetValue(); }
};
//------------------------------------------------------------------------------
class S8Value: public PropValue, public IntegerValue<s8> {
public:
						S8Value			(s8 mn, s8 mx, s8 increm, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw, TOnChange change):IntegerValue<s8>(mn,mx,increm),PropValue(after,before,draw,change){};
    virtual LPCSTR		GetText			()				{ return _GetText(this,OnDrawValue); }
    virtual void		InitFirst		(LPVOID value)	{ _InitFirst((s8*)value); }
    virtual void		InitNext		(LPVOID value)	{ _InitNext((s8*)value,bDiff); }
    virtual bool		ApplyValue		(s8 value)		{ return _ApplyValue(value,bDiff);}
    s8 					GetValue		()				{ return _GetValue(); }
    virtual void		ResetValue		()    			{ _ResetValue(); }
};
//------------------------------------------------------------------------------
class S16Value: public PropValue, public IntegerValue<s16> {
public:
						S16Value	 	(s16 mn, s16 mx, s16 increm, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw, TOnChange change):IntegerValue<s16>(mn,mx,increm),PropValue(after,before,draw,change){};
    virtual LPCSTR		GetText			()				{ return _GetText(this,OnDrawValue); }
    virtual void		InitFirst		(LPVOID value)	{ _InitFirst((s16*)value); }
    virtual void		InitNext		(LPVOID value)	{ _InitNext((s16*)value,bDiff); }
    virtual bool		ApplyValue		(s16 value)		{ return _ApplyValue(value,bDiff);}
    s16					GetValue		()				{ return _GetValue(); }
    virtual void		ResetValue		()    			{ _ResetValue(); }
};
//------------------------------------------------------------------------------
class S32Value: public PropValue, public IntegerValue<s32> {
public:
						S32Value  		(s32 mn, s32 mx, s32 increm, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw, TOnChange change):IntegerValue<s32>(mn,mx,increm),PropValue(after,before,draw,change){};
    virtual LPCSTR		GetText			()				{ return _GetText(this,OnDrawValue); }
    virtual void		InitFirst		(LPVOID value)	{ _InitFirst((s32*)value); }
    virtual void		InitNext		(LPVOID value)	{ _InitNext((s32*)value,bDiff); }
    virtual bool		ApplyValue		(s32 value)		{ return _ApplyValue(value,bDiff);}
    s32					GetValue		()				{ return _GetValue(); }
    virtual void		ResetValue		()    			{ _ResetValue(); }
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
    					FloatValue		(float mn, float mx, float increment, int decimal, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw, TOnChange change):lim_mn(mn),lim_mx(mx),inc(increment),dec(decimal),PropValue(after,before,draw,change){};
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
    virtual void		ResetValue		(){FloatIt src=init_values.begin(); for (LPFloatIt it=values.begin();it!=values.end();it++,src++) **it = *src;}
};
//------------------------------------------------------------------------------

class ColorValue: public PropValue{
	FcolorVec			init_values;
	LPFcolorVec			values;
    void				AppendValue		(Fcolor* value){values.push_back(value);init_values.push_back(*value);}
public:
						ColorValue		(TAfterEdit after, TBeforeEdit before, TOnDrawValue draw, TOnChange change):PropValue(after,before,draw,change){};
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
    virtual void		ResetValue		(){FcolorIt src=init_values.begin(); for (LPFcolorIt it=values.begin();it!=values.end();it++,src++) **it = *src;}
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
						VectorValue		(float mn, float mx, float increment, int decimal, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw, TOnChange change):lim_mn(mn),lim_mx(mx),inc(increment),dec(decimal),PropValue(after,before,draw,change){};
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
    virtual void		ResetValue		(){FvectorIt src=init_values.begin(); for (LPFvectorIt it=values.begin();it!=values.end();it++,src++) **it = *src;}
};
//------------------------------------------------------------------------------

class FlagValue: public PropValue{
	U32Vec				init_values;
	LPU32Vec			values;
    void				AppendValue		(u32* value){values.push_back(value);init_values.push_back(*value);}
public:
	u32					mask;
						FlagValue		(u32 _mask, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw, TOnChange change):mask(_mask),PropValue(after,before,draw,change){};
    virtual LPCSTR		GetText			(){return 0;}
    virtual void		InitFirst		(LPVOID value){R_ASSERT(values.empty()); AppendValue((u32*)value);}
    virtual void		InitNext		(LPVOID value){bDiff=false; bool a=(*(u32*)value)&mask; bool b=(*values.front())&mask; if (a!=b) bDiff=true; AppendValue((u32*)value);}
    virtual bool		ApplyValue		(bool value)
    {
    	bool bChanged	= false;
    	for (LPU32It it=values.begin();it!=values.end();it++){
        	if (value!=(!!(**it&mask))){
	        	if (value) **it|=mask; else **it&=~mask; 
                bChanged= true;
            }
        }
        if (bChanged) 	bDiff = false;
        return bChanged;
    }
    bool	 			GetValue		(){return (*values.front())&mask;}
    virtual void		ResetValue		(){U32It src=init_values.begin(); for (LPU32It it=values.begin();it!=values.end();it++,src++) if ((*src)&mask) **it|=mask; else **it&=~mask;}
};
//------------------------------------------------------------------------------

class TokenValue: public PropValue{
	U32Vec				init_values;
	LPU32Vec			values;
    int 				p_size;
    void				AppendValue		(u32* value){values.push_back(value);init_values.push_back(*value);}
public:
	xr_token* 			token;
						TokenValue		(xr_token* _token, int p_sz, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw, TOnChange change):token(_token),p_size(p_sz),PropValue(after,before,draw,change){R_ASSERT((p_size>0)&&(p_size<=4));};
	virtual LPCSTR 		GetText			();
    virtual void		InitFirst		(LPVOID value){R_ASSERT(values.empty()); AppendValue((u32*)value);}
    virtual void		InitNext		(LPVOID value)
    {
       	if (0!=memcmp(values.front(),value,p_size))
        	bDiff=true; 
        AppendValue((u32*)value);
    }
    bool				ApplyValue		(u32 value)
    {
    	bool bChanged	= false;
        for (LPU32It it=values.begin();it!=values.end();it++){
        	if (0!=memcmp(*it,&value,p_size)){
                CopyMemory(*it,&value,p_size);
                bChanged= true;
            }
        }
        if (bChanged) 	bDiff = false;
        return bChanged;
    }
    u32 				GetValue		(){return *values.front();}
    virtual void		ResetValue		()
    {
    	U32It src=init_values.begin(); 
        for (LPU32It it=values.begin();it!=values.end();it++,src++) 
        	CopyMemory(*it,src,p_size);
    }
};
//------------------------------------------------------------------------------

class TokenValue2: public PropValue{
	U32Vec				init_values;
	LPU32Vec			values;
    void				AppendValue		(u32* value){values.push_back(value);init_values.push_back(*value);}
public:
	AStringVec 			items;
						TokenValue2		(AStringVec* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw, TOnChange change):items(*_items),PropValue(after,before,draw,change){};
	virtual LPCSTR 		GetText			();
    virtual void		InitFirst		(LPVOID value){R_ASSERT(values.empty()); AppendValue((u32*)value);}
    virtual void		InitNext		(LPVOID value)	{if (*(u32*)value!=*values.front()) bDiff=true; AppendValue((u32*)value);}
    bool				ApplyValue		(u32 value)
    {
    	bool bChanged	= false;
        for (LPU32It it=values.begin();it!=values.end();it++){
        	if (**it!=value){
	        	**it 	= value;
                bChanged= true;
            }
        }
        if (bChanged) 	bDiff = false;
        return bChanged;
    }
    u32 				GetValue		(){return *values.front();}
    virtual void		ResetValue		(){U32It src=init_values.begin(); for (LPU32It it=values.begin();it!=values.end();it++,src++) **it = *src;}
};
//------------------------------------------------------------------------------

class TokenValue3: public PropValue{
	U32Vec				init_values;
	LPU32Vec			values;
    void				AppendValue		(u32* value){values.push_back(value);init_values.push_back(*value);}
public:
	struct Item {
		u32		ID;
		string64	str;
	};
	u32				cnt;
    const Item*			items;
						TokenValue3		(u32 _cnt, const Item* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw, TOnChange change):cnt(_cnt),items(_items),PropValue(after,before,draw,change){};
	virtual LPCSTR 		GetText			();
    virtual void		InitFirst		(LPVOID value){R_ASSERT(values.empty()); AppendValue((u32*)value);}
    virtual void		InitNext		(LPVOID value)	{if (*(u32*)value!=*values.front()) bDiff=true; AppendValue((u32*)value);}
    bool				ApplyValue		(u32 value)
    {
    	bool bChanged	= false;
        for (LPU32It it=values.begin();it!=values.end();it++){
        	if (**it!=value){
	        	**it 	= value;
                bChanged= true;
            }
        }
        if (bChanged) 	bDiff = false;
        return bChanged;
    }
    u32 				GetValue		(){return *values.front();}
    virtual void		ResetValue		(){U32It src=init_values.begin(); for (LPU32It it=values.begin();it!=values.end();it++,src++) **it = *src;}
};
//------------------------------------------------------------------------------

class ListValue: public PropValue{
	AStringVec			init_values;
	LPSTRVec			values;
    void				AppendValue		(LPSTR value){values.push_back(value);init_values.push_back(value);}
public:
	AStringVec 			items;
						ListValue		(AStringVec* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw, TOnChange change):items(*_items),PropValue(after,before,draw,change){};
						ListValue		(u32 cnt, LPCSTR* _items, TAfterEdit after, TBeforeEdit before, TOnDrawValue draw, TOnChange change):PropValue(after,before,draw,change){items.resize(cnt); int i=0; for (AStringIt it=items.begin(); it!=items.end(); it++,i++) *it=_items[i]; };
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
    virtual void		ResetValue		(){AStringIt src=init_values.begin(); for (LPSTRIt it=values.begin();it!=values.end();it++,src++) strcpy(*it,src->c_str());}
};
//------------------------------------------------------------------------------
DEFINE_VECTOR(PropValue*,PropValueVec,PropValueIt);

//---------------------------------------------------------------------------
class CPropHelper{
public:
    MarkerItem*			CreateMarker	()
    {
        MarkerItem* V	= new MarkerItem();
        V->type			= PROP_MARKER;
        return V;
    }
    S8Value* 			CreateS8		(s8 mn=0, s8 mx=100, int inc=1, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        S8Value* V		= new S8Value(mn,mx,inc,after,before,draw,change);
        V->type			= PROP_S8;
        return V;
    }
    S16Value* 			CreateS16		(s16 mn=0, s16 mx=100, int inc=1, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        S16Value* V		= new S16Value(mn,mx,inc,after,before,draw,change);
        V->type			= PROP_S16;
        return V;
    }
    S32Value* 			CreateS32		(s32 mn=0, s32 mx=100, int inc=1, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        S32Value* V		= new S32Value(mn,mx,inc,after,before,draw,change);
        V->type			= PROP_S32;
        return V;
    }
    U8Value* 			CreateU8		(u8 mn=0, u8 mx=100, int inc=1, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        U8Value* V		= new U8Value(mn,mx,inc,after,before,draw,change);
        V->type			= PROP_U8;
        return V;
    }
    U16Value* 			CreateU16		(u16 mn=0, u16 mx=100, int inc=1, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        U16Value* V		= new U16Value(mn,mx,inc,after,before,draw,change);
        V->type			= PROP_U16;
        return V;
    }
    U32Value* 			CreateU32		(u32 mn=0, u32 mx=100, int inc=1, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        U32Value* V		= new U32Value(mn,mx,inc,after,before,draw,change);
        V->type			= PROP_U32;
        return V;
    }
    FloatValue* 		CreateFloat		(float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        FloatValue* V	= new FloatValue(mn,mx,inc,decim,after,before,draw,change);
        V->type			= PROP_FLOAT;
        return V;
    }           	
    BOOLValue* 			CreateBOOL		(TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        BOOLValue* V	=new BOOLValue(after,before,draw,change);
        V->type			= PROP_BOOLEAN;
        return V;
    }
    FlagValue* 			CreateFlag		(u32 mask, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        FlagValue* V	= new FlagValue(mask,after,before,draw,change);
        V->type			= PROP_FLAG;
        return V;
    }
    VectorValue* 		CreateVector	(float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        VectorValue* V	= new VectorValue(mn,mx,inc,decim,after,before,draw,change);
        V->type			= PROP_VECTOR;
        return V;
    }
	TokenValue* 		CreateToken	(xr_token* token, int p_size, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        TokenValue* V	= new TokenValue(token,p_size,after,before,draw,change);
        V->type			= PROP_TOKEN;
        return V;
    }
	TokenValue2* 		CreateToken2	(AStringVec* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        TokenValue2* V	= new TokenValue2(lst,after,before,draw,change);
        V->type			= PROP_TOKEN2;
        return V;
    }
	TokenValue3* 		CreateToken3	(u32 cnt, const TokenValue3::Item* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        TokenValue3* V	= new TokenValue3(cnt,lst,after,before,draw,change);
        V->type			= PROP_TOKEN3;
        return V;
    }
	ListValue* 			CreateList		(AStringVec* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        ListValue* V	= new ListValue(lst,after,before,draw,change);
        V->type			= PROP_LIST;
        return V;
    }
	ListValue* 			CreateListA	(u32 cnt, LPCSTR* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        ListValue* V	= new ListValue(cnt,lst,after,before,draw,change);
        V->type			= PROP_LIST;
        return V;
    }
    U32Value* 			CreateColor	(TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        U32Value* V		= new U32Value(0x00000000,0xffffffff,0,after,before,draw,change);
        V->type			= PROP_COLOR;
        return V;
    }
    ColorValue*			CreateFColor	(TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        ColorValue* V	= new ColorValue(after,before,draw,change);
        V->type			= PROP_FCOLOR;
        return V;
    }
	TextValue* 			CreateText		(int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        TextValue* V	= new TextValue(lim,after,before,draw,change);
        V->type			= PROP_TEXT;
        return V;
    }
	ATextValue* 		CreateAText	(TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        ATextValue* V	= new ATextValue(after,before,draw,change);
        V->type			= PROP_A_TEXT;
        return V;
    }
	TextValue* 			CreateEShader	(int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        TextValue* V	= new TextValue(lim,after,before,draw,change);
        V->type			= PROP_ESHADER;
        return V;
    }
	TextValue* 			CreateCShader	(int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        TextValue* V	= new TextValue(lim,after,before,draw,change);
        V->type			= PROP_CSHADER;
        return V;
    }
	TextValue* 			CreateTexture	(int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        TextValue* V	= new TextValue(lim,after,before,draw,change);
        V->type			= PROP_TEXTURE;
        return V;
    }
	TextValue* 			CreateTexture2	(int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        TextValue* V	= new TextValue(lim,after,before,draw,change);
        V->type			= PROP_TEXTURE2;
        return V;
    }
	ATextValue* 		CreateAEShader	(TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        ATextValue* V	= new ATextValue(after,before,draw,change);
        V->type			= PROP_A_ESHADER;
        return V;
    }
	ATextValue* 		CreateACShader	(TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        ATextValue* V	= new ATextValue(after,before,draw,change);
        V->type			= PROP_A_CSHADER;
        return V;
    }
	ATextValue* 		CreateATexture	(TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        ATextValue* V	= new ATextValue(after,before,draw,change);
        V->type			= PROP_A_TEXTURE;
        return V;
    }
	TextValue*			CreateLightAnim(int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        TextValue* V	= new TextValue(lim,after,before,draw,change);
        V->type			= PROP_LIGHTANIM;
        return V;
    }
	TextValue* 			CreateLibObject(int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        TextValue* V	= new TextValue(lim,after,before,draw,change);
        V->type			= PROP_LIBOBJECT;
        return V;
    }
	TextValue* 			CreateGameObject(int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        TextValue* V	= new TextValue(lim,after,before,draw,change);
        V->type			= PROP_GAMEOBJECT;
        return V;
    }
    TextValue*			CreateLibSound(int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        TextValue* V	= new TextValue(lim,after,before,draw,change);
		V->type			= PROP_LIBSOUND;
        return V;
    }
	TextValue* 			CreateEntity	(int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        TextValue* V	= new TextValue(lim,after,before,draw,change);
        V->type			= PROP_ENTITY;
        return V;
    }
	WaveValue* 			CreateWave		(TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
        WaveValue* V	= new WaveValue(after,before,draw,change);
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
    IC LPCSTR			PrepareKey			(LPCSTR pref, LPCSTR key)
    {
    	static AnsiString XKey;
    	if (pref)	XKey= AnsiString(pref)+"\\"+AnsiString(key); 
        else		XKey= key;
        return XKey.c_str();
    }
    IC PropValue* 		FindProp			(PropValueVec& values,	LPCSTR pref, LPCSTR key)
    {
    	LPCSTR X		= PrepareKey(pref,key);
    	for (PropValueIt it=values.begin(); it!=values.end(); it++)
        	if (0==strcmp((*it)->key,X)) return *it;
        return 0;
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

//------------------------------------------------------------------------------
// predefind event routines    
    IC void __fastcall 	FvectorRDOnAfterEdit(PropValue* sender, LPVOID edit_val)
    {
        Fvector* V = (Fvector*)edit_val;
        V->x = deg2rad(V->x);
        V->y = deg2rad(V->y);
        V->z = deg2rad(V->z);
    }

    IC void __fastcall 	FvectorRDOnBeforeEdit(PropValue* sender, LPVOID edit_val)
    {
        Fvector* V = (Fvector*)edit_val;
        V->x = rad2deg(V->x);
        V->y = rad2deg(V->y);
        V->z = rad2deg(V->z);
    }

    IC void __fastcall 	FvectorRDOnDraw(PropValue* sender, LPVOID draw_val)
    {
        Fvector* V = (Fvector*)draw_val;
        V->x = rad2deg(V->x);
        V->y = rad2deg(V->y);
        V->z = rad2deg(V->z);
    }
    IC void __fastcall 	floatRDOnAfterEdit(PropValue* sender, LPVOID edit_val)
    {
        *(float*)edit_val = deg2rad(*(float*)edit_val);
    }

    IC void __fastcall 	floatRDOnBeforeEdit(PropValue* sender, LPVOID edit_val)
    {
        *(float*)edit_val = rad2deg(*(float*)edit_val);
    }

    IC void __fastcall 	floatRDOnDraw(PropValue* sender, LPVOID draw_val)
    {
        *(float*)draw_val = rad2deg(*(float*)draw_val);
    }
};
extern CPropHelper PHelper;
#define FILL_PROP(A,K,V,P){PropValue* prop=PHelper.FindProp(A,K); if (prop) PHelper.InitNext(prop,V); else PHelper.InitFirst(A,K,V,P);}
#define FILL_PROP_EX(A,X,K,V,P){AnsiString XK=PHelper.PrepareKey(X,K); PropValue* prop=PHelper.FindProp(A,XK.c_str()); if (prop) PHelper.InitNext(prop,V); else PHelper.InitFirst(A,XK.c_str(),V,P);}
//---------------------------------------------------------------------------
#endif





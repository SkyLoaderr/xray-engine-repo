//---------------------------------------------------------------------------
#ifndef PropertiesListTypesH
#define PropertiesListTypesH

#include "ElTree.hpp"
//---------------------------------------------------------------------------
enum EPropType{
	PROP_CAPTION	= 0x1000,
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
    PROP_A_LIBOBJECT,
    PROP_LIBSOUND,
    PROP_A_LIBSOUND,
    PROP_LIBPS,
    PROP_A_LIBPS,
    PROP_GAMEOBJECT,
    PROP_GAMEMTL,
	PROP_A_GAMEMTL,
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
typedef void 	__fastcall (__closure *TOnClick)		(PropValue* sender);
typedef void 	__fastcall (__closure *TOnCloseEvent)	(void);
typedef void 	__fastcall (__closure *TOnModifiedEvent)(void);
typedef void 	__fastcall (__closure *TOnItemFocused)	(TElTreeItem* item);
//------------------------------------------------------------------------------

class PropItem;

class PropValue{
	friend class		CPropHelper;
public:
	PropItem*			m_Owner;
public:
	// base events
    TAfterEdit			AfterEditEvent;
    TBeforeEdit			BeforeEditEvent;
    TOnDrawValue		DrawValueEvent;
    TOnChange			ChangeEvent;
    TOnClick			OnClickEvent;
public:
						PropValue		():m_Owner(0),AfterEditEvent(0),BeforeEditEvent(0),DrawValueEvent(0),ChangeEvent(0),OnClickEvent(0){;}
    virtual LPCSTR		GetText			()=0;
    virtual void		ResetValue		()=0;
    virtual bool		Equal			(PropValue* prop)=0;
    virtual bool		ApplyValue		(LPVOID val)=0;
    IC PropItem*		Owner			(){return m_Owner;}
};
//------------------------------------------------------------------------------

class PropItem{
	friend class		CPropHelper;
    friend class		TProperties;
    LPSTR				key;
    EPropType			type;
	TElTreeItem*		item; 
    int					subitem;		// multiple selection for each item (SelectTexture for example)
	DEFINE_VECTOR		(PropValue*,PropValueVec,PropValueIt);
    PropValueVec		values;
public:
    int 				tag;
public:
    enum{
    	flDisabled		= (1<<0),
    	flShowCB		= (1<<1),
    	flCBChecked		= (1<<2),
        flMixed			= (1<<3),
    };
    Flags32				m_Flags;
public:
						PropItem		(EPropType _type):type(_type),item(0),key(0),tag(0),subitem(1){m_Flags.zero();}
	virtual 			~PropItem		()
    {
    	for (PropValueIt it=values.begin(); it!=values.end(); it++) 
        	_DELETE		(*it);
    	_FREE			(key);
    };
    void				SetName			(LPCSTR name){key=xr_strdup(name);}
    IC void				ResetValues		()
    { 
    	for (PropValueIt it=values.begin(); it!=values.end(); it++) 
        	(*it)->ResetValue();
        if (!m_Flags.is(flMixed)&&(values.size()>1)){
            PropValueIt F	= values.begin();
        	PropValueIt it	= F; it++;
	    	for (; it!=values.end(); it++){
    	    	if (!(*it)->Equal(*F)){
                	m_Flags.set(flMixed,TRUE);
                    break;
                }
            }
        }
    }
    IC void				AppendValue		(PropValue* value)
    {
    	if (!values.empty()&&!value->Equal(values.front()))
        	m_Flags.set	(flMixed,TRUE);
    	values.push_back(value);
    }
    IC LPCSTR			GetText			(){VERIFY(!values.empty()); return m_Flags.is(flMixed)?"(mixed)":values.front()->GetText();}
    IC bool 			ApplyValue		(LPVOID val)
    {
    	bool bChanged	= false;
    	for (PropValueIt it=values.begin(); it!=values.end(); it++)
        	if ((*it)->ApplyValue(val)) bChanged = true;
        if (bChanged) 	m_Flags.set(flMixed,FALSE);
        return bChanged;
    }
    IC PropValue*		GetValue		(){VERIFY(!values.empty()); return values.front(); };
    IC EPropType		Type			(){return type;}

	IC void				OnBeforeEdit	(LPVOID edit_val)
    {
    	for (PropValueIt it=values.begin(); it!=values.end(); it++)
        	if ((*it)->BeforeEditEvent) (*it)->BeforeEditEvent(*it,edit_val);
    }
	IC void				OnAfterEdit		(LPVOID edit_val)
    {
    	for (PropValueIt it=values.begin(); it!=values.end(); it++)
        	if ((*it)->AfterEditEvent) 	(*it)->AfterEditEvent(*it,edit_val);
    }
	IC void				OnChange		()
    {
    	for (PropValueIt it=values.begin(); it!=values.end(); it++)
        	if ((*it)->ChangeEvent) 		(*it)->ChangeEvent(*it);
    }
};

DEFINE_VECTOR			(PropItem*,PropItemVec,PropItemIt);

//------------------------------------------------------------------------------
// values
//------------------------------------------------------------------------------
class CaptionValue: public PropValue{
	AnsiString			value;
public:
						CaptionValue	(LPCSTR val){value=val;}
    virtual LPCSTR		GetText			(){return value.c_str();}
    virtual	void		ResetValue		(){;}
    virtual	bool		Equal			(PropValue* val){return true;}
    virtual	bool		ApplyValue		(LPVOID val){return false;}
};

class TextValue: public PropValue{
	AnsiString			init_value;
	LPSTR				value;
public:
	int					lim;
						TextValue		(LPSTR val, int _lim):value(val),init_value(val),lim(_lim){};
    virtual LPCSTR		GetText			();
    virtual bool		Equal			(PropValue* prop){ return (0==strcmp((LPSTR)value,((TextValue*)prop)->value)); }
    virtual bool		ApplyValue		(LPVOID val)
    {
        if (0!=strcmp(value,LPCSTR(val))){
            strcpy(value,LPCSTR(val));
            return		true;
        }
        return 			false;
    }
    LPCSTR				GetValue		(){ return value; }
    virtual void		ResetValue		(){ strcpy(value,init_value.c_str());}
};
//------------------------------------------------------------------------------

class ATextValue: public PropValue{
	AnsiString			init_value;
	AnsiString*			value;
public:
						ATextValue		(AnsiString* val):value(val),init_value(*val){};
    virtual LPCSTR		GetText			();
    virtual bool		Equal			(PropValue* prop){ return (*value==*((ATextValue*)prop)->value); }
    virtual bool		ApplyValue		(LPVOID val)
    {
        if (*value!=*(AnsiString*)val){
            *value		= *(AnsiString*)val;
            return 		true;
        }
        return 			false;
    }
    const AnsiString&	GetValue		(){return *value;}
    virtual void		ResetValue		(){*value=init_value;}
};
//------------------------------------------------------------------------------

class WaveValue: public PropValue{
	WaveForm			init_value;
	WaveForm*			value;
public:
						WaveValue		(WaveForm* val):value(val),init_value(*val){};
    virtual LPCSTR		GetText			(){return "[Wave]";}
    virtual bool		Equal			(PropValue* prop){ return (value->Similar(*((WaveValue*)prop)->value)); }
    virtual bool		ApplyValue		(LPVOID val)
    {
        if (value->Similar(*(WaveForm*)val)){
            *value		= *(WaveForm*)val;
            return 		true;
        }
        return 			false;
    }
    const WaveForm&		GetValue		(){return *value;}
    virtual void		ResetValue		(){*value=init_value;}
};
//------------------------------------------------------------------------------


template <class T>
class NumericValue
{
public:
	T					init_value;
	T*					value;
    T					lim_mn;
    T					lim_mx;
    T 					inc;
public:    
						NumericValue	(T* val)
	{
        value			= val;
        init_value		= *value;
    };
						NumericValue	(T* val, T mn, T mx, T increm):lim_mn(mn),lim_mx(mx),inc(increm)
	{
    	clamp			(*val,lim_mn,lim_mx); 
        value			= val;
        init_value		= *value;
    };
    LPCSTR				_GetText		(PropValue* prop,TOnDrawValue OnDraw)
    {
    	T draw_val 		= 	_GetValue();
        if (OnDraw)		OnDraw(prop, &draw_val);
		static AnsiString draw_text;
        draw_text		= draw_val;
        return draw_text.c_str();
    }
    bool				_Equal			(T* val){ return (*value!=*val); }
    bool				_ApplyValue		(T val)
    {
    	clamp			(val,lim_mn,lim_mx); 
        if (*value!=val){
            *value 		= val;
            return		true;
        }
        return 			false;
    }
    T 					_GetValue		(){ return *value; }
    void				_ResetValue		(){ *value = init_value; }
};

class U8Value: public PropValue, public NumericValue<u8> {
public:
						U8Value			(u8* val, u8 mn, u8 mx, u8 increm):NumericValue<u8>(val,mn,mx,increm){};
    virtual LPCSTR		GetText			()				{ return _GetText(this,DrawValueEvent); }
    virtual bool		Equal           (PropValue* prop){return _Equal(((U8Value*)prop)->value);}
    virtual bool		ApplyValue		(LPVOID val)	{ return _ApplyValue(*(u8*)val);}
    u8 					GetValue		()				{ return _GetValue(); }
    virtual void		ResetValue		()    			{ _ResetValue(); }
};
//------------------------------------------------------------------------------
class U16Value: public PropValue, public NumericValue<u16> {
public:
						U16Value	 	(u16* val, u16 mn, u16 mx, u16 increm):NumericValue<u16>(val,mn,mx,increm){};
    virtual LPCSTR		GetText			()				{ return _GetText(this,DrawValueEvent); }
    virtual bool		Equal           (PropValue* prop){return _Equal(((U16Value*)prop)->value);}
    virtual bool		ApplyValue		(LPVOID val)	{ return _ApplyValue(*(u16*)value);}
    u16					GetValue		()				{ return _GetValue(); }
    virtual void		ResetValue		()    			{ _ResetValue(); }
};
//------------------------------------------------------------------------------
class U32Value: public PropValue, public NumericValue<u32> {
public:
						U32Value  		(u32* val, u32 mn, u32 mx, u32 increm):NumericValue<u32>(val,mn,mx,increm){};
    virtual LPCSTR		GetText			()				{ return _GetText(this,DrawValueEvent); }
    virtual bool		Equal           (PropValue* prop){return _Equal(((U32Value*)prop)->value);}
    virtual bool		ApplyValue		(LPVOID val)	{ return _ApplyValue(*(u32*)value);}
    u32					GetValue		()				{ return _GetValue(); }
    virtual void		ResetValue		()    			{ _ResetValue(); }
};
//------------------------------------------------------------------------------
class S8Value: public PropValue, public NumericValue<s8> {
public:
						S8Value			(s8* val, s8 mn, s8 mx, s8 increm):NumericValue<s8>(val,mn,mx,increm){};
    virtual LPCSTR		GetText			()				{ return _GetText(this,DrawValueEvent); }
    virtual bool		Equal           (PropValue* prop){return _Equal(((S8Value*)prop)->value);}
    virtual bool		ApplyValue		(LPVOID val)   	{ return _ApplyValue(*(s8*)value);}
    s8 					GetValue		()				{ return _GetValue(); }
    virtual void		ResetValue		()    			{ _ResetValue(); }
};
//------------------------------------------------------------------------------
class S16Value: public PropValue, public NumericValue<s16> {
public:
						S16Value	 	(s16* val, s16 mn, s16 mx, s16 increm):NumericValue<s16>(val,mn,mx,increm){};
    virtual LPCSTR		GetText			()				{ return _GetText(this,DrawValueEvent); }
    virtual bool		Equal           (PropValue* prop){return _Equal(((S16Value*)prop)->value);}
    virtual bool		ApplyValue		(LPVOID val)	{ return _ApplyValue(*(s16*)value);}
    s16					GetValue		()				{ return _GetValue(); }
    virtual void		ResetValue		()    			{ _ResetValue(); }
};
//------------------------------------------------------------------------------
class S32Value: public PropValue, public NumericValue<s32> {
public:
						S32Value  		(s32* val, s32 mn, s32 mx, s32 increm):NumericValue<s32>(val,mn,mx,increm){};
    virtual LPCSTR		GetText			()				{ return _GetText(this,DrawValueEvent); }
    virtual bool		Equal           (PropValue* prop){return _Equal(((S32Value*)prop)->value);}
    virtual bool		ApplyValue		(LPVOID val)	{ return _ApplyValue(*(s32*)value);}
    s32					GetValue		()				{ return _GetValue(); }
    virtual void		ResetValue		()    			{ _ResetValue(); }
};
//------------------------------------------------------------------------------

class BOOLValue: public PropValue, public NumericValue<BOOL>{
public:
						BOOLValue		(LPBOOL val):NumericValue<BOOL>(val){};
    virtual LPCSTR		GetText			(){return 0;}
    virtual bool		Equal           (PropValue* prop){return _Equal(((BOOLValue*)prop)->value);}
    virtual bool		ApplyValue		(LPVOID val)	{ return _ApplyValue(*(BOOL*)value);}
    BOOL				GetValue		()				{ return _GetValue(); }
    virtual void		ResetValue		()    			{ _ResetValue(); }
};
//------------------------------------------------------------------------------

class FloatValue: public PropValue, public NumericValue<float>{
public:
    int 				dec;
    					FloatValue		(float* val, float mn, float mx, float increment, int decimal):NumericValue<float>(val,mn,mx,increment),dec(decimal){};
    virtual LPCSTR		GetText			()
    {
    	float draw_val 	= GetValue();
        if (DrawValueEvent)DrawValueEvent(this, &draw_val);
        static AnsiString draw_text;
		AnsiString fmt; fmt.sprintf("%%.%df",dec);
        draw_text.sprintf(fmt.c_str(),draw_val);
		return draw_text.c_str();
    }
    virtual bool		Equal           (PropValue* prop){return _Equal(((FloatValue*)prop)->value);}
    virtual bool		ApplyValue		(LPVOID val)	{ return _ApplyValue(*(float*)val);}
    float				GetValue		()				{ return _GetValue(); }
    virtual void		ResetValue		()    			{ _ResetValue(); }
};
//------------------------------------------------------------------------------

class ColorValue: public PropValue, public NumericValue<Fcolor>{
public:
						ColorValue		(Fcolor* val):NumericValue<Fcolor>(val){};
    virtual LPCSTR		GetText			(){return 0;}
    virtual bool		Equal           (PropValue* prop){return _Equal(((ColorValue*)prop)->value);}
    virtual bool		ApplyValue		(LPVOID val)	{ return _ApplyValue(*(Fcolor*)val);}
    const Fcolor		GetValue		()				{ return _GetValue(); }
    virtual void		ResetValue		()    			{ _ResetValue(); }
};
//------------------------------------------------------------------------------

IC bool operator != (const Fvector& A, const Fvector& B)
{	return !A.similar(B); }

IC void clamp(Fvector& V, Fvector& mn, const Fvector& mx)
{
    clamp(V.x,mn.x,mx.x); 
    clamp(V.y,mn.y,mx.y); 
    clamp(V.z,mn.z,mx.z); 
}

class VectorValue: public PropValue, public NumericValue<Fvector>{
public:
	int	dec;
						VectorValue		(Fvector* val, float mn, float mx, float increment, int decimal):NumericValue<Fvector>(val),dec(decimal)
    {
    	lim_mn.set		(mn,mn,mn);
    	lim_mx.set		(mx,mx,mx);
        inc.set			(increment,increment,increment);
    };
    virtual void		_Clamp			(Fvector* val)
    {
        clamp			(value->x,lim_mn.x,lim_mx.x); 
        clamp			(value->y,lim_mn.y,lim_mx.y); 
        clamp			(value->z,lim_mn.z,lim_mx.z); 
    }
    virtual LPCSTR		GetText			()
    {
		Fvector draw_val 	= GetValue();
        if (DrawValueEvent)DrawValueEvent(this, &draw_val);
        static AnsiString draw_text;
		AnsiString fmt; fmt.sprintf("{%%.%df, %%.%df, %%.%df}",dec,dec,dec);
        draw_text.sprintf(fmt.c_str(),draw_val.x,draw_val.y,draw_val.z);
		return draw_text.c_str();
    }
    virtual bool		Equal           (PropValue* prop){return false;/*_Equal(((VectorValue*)prop)->value);*/}
    virtual bool		ApplyValue		(LPVOID val)	{ return _ApplyValue(*(Fvector*)val);}
    const Fvector		GetValue		()				{ return _GetValue(); }
    virtual void		ResetValue		()    			{ _ResetValue(); }
};
//------------------------------------------------------------------------------

class FlagValue: public PropValue{
	Flags32				init_value;
	Flags32*			value;
public:
	u32					mask;
						FlagValue		(Flags32* val, u32 _mask):value(val),init_value(*val),mask(_mask){};
    virtual LPCSTR		GetText			(){return 0;}
    virtual bool		Equal			(PropValue* prop){return value->equal(*((FlagValue*)prop)->value,mask);}
    virtual bool		ApplyValue		(LPVOID val)
    {
        if (*(BOOL*)val!=value->is(mask)){
        	value->set	(mask,(bool)val);
            return 		true;
        }
        return 			false;
    }
    bool	 			GetValue		(){return value->is(mask);}
    virtual void		ResetValue		(){value->set(mask,init_value.is(mask));}
};
//------------------------------------------------------------------------------

class TokenValue: public PropValue{
	u32					init_value;
	u32*				value;
    int 				p_size;
public:
	xr_token* 			token;
						TokenValue		(u32* val, xr_token* _token, int p_sz):value(val),init_value(*val),token(_token),p_size(p_sz){R_ASSERT((p_size>0)&&(p_size<=4));};
	virtual LPCSTR 		GetText			();
	virtual bool		Equal			(PropValue* prop){return (0==memcmp(value,((TokenValue*)prop)->value,p_size));}
    virtual bool		ApplyValue		(LPVOID val)
    {
        if (0!=memcmp(val,value,p_size)){
            CopyMemory(value,val,p_size);
            return		true;
        }
        return 			false;
    }
    u32 				GetValue		(){return *value;}
    virtual void		ResetValue		(){CopyMemory(value,&init_value,p_size);}
};
//------------------------------------------------------------------------------

class TokenValue2: public PropValue{
	u32					init_value;
	u32*				value;
public:
	AStringVec 			items;
						TokenValue2		(u32* val, AStringVec* _items):value(val),init_value(*val),items(*_items){};
	virtual LPCSTR 		GetText			();
	virtual bool		Equal			(PropValue* prop){return *value==*((TokenValue2*)prop)->value;}
    virtual bool		ApplyValue		(LPVOID val)
    {
        if (*value!=*(u32*)val){
            *value 		= *(u32*)val;
            return 		true;
        }
        return 			false;
    }
    u32 				GetValue		(){return *value;}
    virtual void		ResetValue		(){*value=init_value;}
};
//------------------------------------------------------------------------------

class TokenValue3: public PropValue{
	u32					init_value;
	u32*				value;
public:
	struct Item {
		u32		ID;
		string64	str;
	};
	u32				cnt;
    const Item*			items;
						TokenValue3		(u32* val, u32 _cnt, const Item* _items):value(val),init_value(*val),cnt(_cnt),items(_items){};
	virtual LPCSTR 		GetText			();
	virtual bool		Equal			(PropValue* prop){return *value==*((TokenValue3*)prop)->value;}
    virtual bool		ApplyValue		(LPVOID val)
    {
        if (*value!=*(u32*)val){
            *value 		= *(u32*)val;
            return 		true;
        }
        return 			false;
    }
    u32 				GetValue		(){return *value;}
    virtual void		ResetValue		(){*value=init_value;}
};
//------------------------------------------------------------------------------

class ListValue: public PropValue{
	AnsiString			init_value;
	LPSTR				value;
public:
	AStringVec 			items;
						ListValue		(LPSTR val, AStringVec* _items):value(val),init_value(*val),items(*_items){};
						ListValue		(LPSTR val, u32 cnt, LPCSTR* _items):value(val),init_value(*val){items.resize(cnt); int i=0; for (AStringIt it=items.begin(); it!=items.end(); it++,i++) *it=_items[i]; };
	virtual LPCSTR		GetText			();
	virtual bool		Equal			(PropValue* prop){return (0==strcmp(value,((ListValue*)prop)->value));}
    virtual bool		ApplyValue		(LPVOID val)
    {
        if (0!=strcmp((LPCSTR)val,value)){
            strcpy(value,(LPCSTR)val);
            return		true;
        }
    	return 			false;
    }
    LPCSTR				GetValue		(){return value;}
    virtual void		ResetValue		(){strcpy(value,init_value.c_str());}
};
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
class CPropHelper{
	IC PropItem*		CreateItem		(PropItemVec& items, LPCSTR key, EPropType type)
    {
    	PropItem* item	= FindItem(items,key,type);
        if (!item){ 
        	item		= new PropItem	(type);
            item->SetName(key);
            items.push_back(item);
        }
        return			item;
    }
    IC PropValue*		AppendValue		(PropItemVec& items, LPCSTR key, PropValue* val, EPropType type)
    {
    	PropItem* item	= CreateItem(items,key,type);
        val->m_Owner	= item;
        item->AppendValue(val);
        return val;
    }
//------------------------------------------------------------------------------
public:
    IC LPCSTR			PrepareKey		(LPCSTR pref, LPCSTR key)
    {
    	static AnsiString XKey;
    	if (pref)	XKey= AnsiString(pref)+"\\"+AnsiString(key); 
        else		XKey= key;
        return XKey.c_str();
    }
    IC PropItem* 		FindItem		(PropItemVec& items,	LPCSTR key, EPropType type)
    {
    	for (PropItemIt it=items.begin(); it!=items.end(); it++)
        	if (((*it)->type==type)&&(0==strcmp((*it)->key,key))) return *it;
        return 0;
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
public:
    IC CaptionValue*	CreateCaption	(PropItemVec& items, LPCSTR key, LPCSTR val)
    {
    	return			(CaptionValue*)	AppendValue		(items,key,new CaptionValue(val),PROP_CAPTION);
    }
    IC S8Value* 		CreateS8		(PropItemVec& items, LPCSTR key, s8* val, s8 mn=0, s8 mx=100, s8 inc=1, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(S8Value*)		AppendValue		(items,key,new S8Value(val,mn,mx,inc,after,before,draw,change),PROP_S8);
    }
    IC S16Value* 		CreateS16		(PropItemVec& items, LPCSTR key, s16* val, s16 mn=0, s16 mx=100, s16 inc=1, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(S16Value*)		AppendValue		(items,key,new S16Value(val,mn,mx,inc,after,before,draw,change),PROP_S16);
    }
    IC S32Value* 	 	CreateS32		(PropItemVec& items, LPCSTR key, s32* val, s32 mn=0, s32 mx=100, s32 inc=1, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(S32Value*)		AppendValue		(items,key,new S32Value(val,mn,mx,inc,after,before,draw,change),PROP_S32);
    }
    IC U8Value* 		CreateU8		(PropItemVec& items, LPCSTR key, u8* val, u8 mn=0, u8 mx=100, u8 inc=1, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(U8Value*)		AppendValue		(items,key,new U8Value(val,mn,mx,inc,after,before,draw,change),PROP_U8);
    }
    IC U16Value* 		CreateU16		(PropItemVec& items, LPCSTR key, u16* val, u16 mn=0, u16 mx=100, u16 inc=1, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(U16Value*)		AppendValue		(items,key,new U16Value(val,mn,mx,inc,after,before,draw,change),PROP_U16);
    }
    IC U32Value* 	  	CreateU32		(PropItemVec& items, LPCSTR key, u32* val, u32 mn=0, u32 mx=100, u32 inc=1, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(U32Value*)		AppendValue		(items,key,new U32Value(val,mn,mx,inc,after,before,draw,change),PROP_U32);
    }
    FloatValue* 		CreateFloat		(PropItemVec& items, LPCSTR key, float* val, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(FloatValue*)	AppendValue		(items,key,new FloatValue(val,mn,mx,inc,decim,after,before,draw,change),PROP_FLOAT);
    }           	
    IC BOOLValue* 	  	CreateBOOL		(PropItemVec& items, LPCSTR key, BOOL* val, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(BOOLValue*)	AppendValue		(items,key,new BOOLValue(val,after,before,draw,change),PROP_BOOLEAN);
    }
    IC FlagValue* 	  	CreateFlag		(PropItemVec& items, LPCSTR key, Flags32* val, u32 mask, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(FlagValue*)	AppendValue		(items,key,new FlagValue(val,mask,after,before,draw,change),PROP_FLAG);
    }
    IC VectorValue*   	CreateVector	(PropItemVec& items, LPCSTR key, Fvector* val, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(VectorValue*)	AppendValue		(items,key,new VectorValue(val,mn,mx,inc,decim,after,before,draw,change),PROP_VECTOR);
    }
	IC TokenValue* 		CreateToken		(PropItemVec& items, LPCSTR key, u32* val, xr_token* token, int p_size, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(TokenValue*)	AppendValue		(items,key,new TokenValue(val,token,p_size,after,before,draw,change),PROP_TOKEN);
    }
	IC TokenValue2*   	CreateToken2	(PropItemVec& items, LPCSTR key, u32* val, AStringVec* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(TokenValue2*)	AppendValue		(items,key,new TokenValue2(val,lst,after,before,draw,change),PROP_TOKEN2);
    }
	IC TokenValue3*   	CreateToken3	(PropItemVec& items, LPCSTR key, u32* val, u32 cnt, const TokenValue3::Item* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(TokenValue3*)	AppendValue		(items,key,new TokenValue3(val,cnt,lst,after,before,draw,change),PROP_TOKEN3);
    }
	IC ListValue* 	 	CreateList		(PropItemVec& items, LPCSTR key, LPSTR val, AStringVec* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(ListValue*)	AppendValue		(items,key,new ListValue(val,lst,after,before,draw,change),PROP_LIST);
    }
	IC ListValue* 	 	CreateListA		(PropItemVec& items, LPCSTR key, LPSTR val, u32 cnt, LPCSTR* lst, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(ListValue*)	AppendValue		(items,key,new ListValue(val,cnt,lst,after,before,draw,change),PROP_LIST);
    }
    IC U32Value*  		CreateColor		(PropItemVec& items, LPCSTR key, u32* val, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(U32Value*)		AppendValue		(items,key,new U32Value(val,0x00000000,0xffffffff,1,after,before,draw,change),PROP_COLOR);
    }
    IC ColorValue*		CreateFColor	(PropItemVec& items, LPCSTR key, Fcolor* val, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(ColorValue*)	AppendValue		(items,key,new ColorValue(val,after,before,draw,change),PROP_FCOLOR);
    }
	IC TextValue* 	   	CreateText		(PropItemVec& items, LPCSTR key, LPSTR val, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim,after,before,draw,change),PROP_TEXT);
    }
	IC ATextValue* 		CreateAText		(PropItemVec& items, LPCSTR key, AnsiString* val, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(ATextValue*)	AppendValue		(items,key,new ATextValue(val,after,before,draw,change),PROP_A_TEXT);
    }
	IC TextValue* 	 	CreateEShader	(PropItemVec& items, LPCSTR key, LPSTR val, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim,after,before,draw,change),PROP_ESHADER);
    }
	IC TextValue* 	   	CreateCShader	(PropItemVec& items, LPCSTR key, LPSTR val, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim,after,before,draw,change),PROP_CSHADER);
    }
	IC TextValue* 	   	CreateTexture	(PropItemVec& items, LPCSTR key, LPSTR val, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim,after,before,draw,change),PROP_TEXTURE);
    }
	IC TextValue* 	  	CreateTexture2	(PropItemVec& items, LPCSTR key, LPSTR val, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim,after,before,draw,change),PROP_TEXTURE2);
    }
	IC ATextValue* 		CreateAEShader	(PropItemVec& items, LPCSTR key, AnsiString* val, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(ATextValue*)	AppendValue		(items,key,new ATextValue(val,after,before,draw,change),PROP_A_ESHADER);
    }
	IC ATextValue* 		CreateACShader	(PropItemVec& items, LPCSTR key, AnsiString* val, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(ATextValue*)	AppendValue		(items,key,new ATextValue(val,after,before,draw,change),PROP_A_CSHADER);
    }
    IC ATextValue*	   	CreateAGameMtl	(PropItemVec& items, LPCSTR key, AnsiString* val, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(ATextValue*)	AppendValue		(items,key,new ATextValue(val,after,before,draw,change),PROP_A_GAMEMTL);
    }
	IC ATextValue* 		CreateATexture	(PropItemVec& items, LPCSTR key, AnsiString* val, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(ATextValue*)	AppendValue		(items,key,new ATextValue(val,after,before,draw,change),PROP_A_TEXTURE);
    }
	IC TextValue*	 	CreateLightAnim	(PropItemVec& items, LPCSTR key, LPSTR val, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim,after,before,draw,change),PROP_LIGHTANIM);
    }
	IC TextValue* 	 	CreateLibObject	(PropItemVec& items, LPCSTR key, LPSTR val, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim,after,before,draw,change),PROP_LIBOBJECT);
    }
	IC ATextValue* 		CreateALibObject(PropItemVec& items, LPCSTR key, AnsiString* val, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(ATextValue*)	AppendValue		(items,key,new ATextValue(val,after,before,draw,change),PROP_A_LIBOBJECT);
    }
	IC TextValue* 	 	CreateGameObject(PropItemVec& items, LPCSTR key, LPSTR val, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim,after,before,draw,change),PROP_GAMEOBJECT);
    }
    IC TextValue*		CreateLibSound	(PropItemVec& items, LPCSTR key, LPSTR val, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim,after,before,draw,change),PROP_LIBSOUND);
    }
    IC ATextValue*	  	CreateALibSound	(PropItemVec& items, LPCSTR key, AnsiString* val, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(ATextValue*)	AppendValue		(items,key,new ATextValue(val,after,before,draw,change),PROP_A_LIBSOUND);
    }
    IC TextValue*	 	CreateLibPS		(PropItemVec& items, LPCSTR key, LPSTR val, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim,after,before,draw,change),PROP_LIBPS);
    }
    IC ATextValue*	 	CreateALibPS	(PropItemVec& items, LPCSTR key, AnsiString* val, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(ATextValue*)	AppendValue		(items,key,new ATextValue(val,after,before,draw,change),PROP_A_LIBPS);
    }
	IC TextValue* 		CreateEntity	(PropItemVec& items, LPCSTR key, LPSTR val, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim,after,before,draw,change),PROP_ENTITY);
    }
	IC WaveValue* 		CreateWave		(PropItemVec& items, LPCSTR key, WaveForm* val, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(WaveValue*)	AppendValue		(items,key,new WaveValue(val,after,before,draw,change),PROP_WAVE);
    }     
    IC TextValue* 		CreateGameMtl	(PropItemVec& items, LPCSTR key, LPSTR val, int lim, TAfterEdit after=0, TBeforeEdit before=0, TOnDrawValue draw=0, TOnChange change=0)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim,after,before,draw,change),PROP_GAMEMTL);
    }
};
extern CPropHelper PHelper;
//---------------------------------------------------------------------------
#endif





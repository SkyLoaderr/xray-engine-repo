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
	PROP_FLAG8,
	PROP_FLAG16,
	PROP_FLAG32,
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
class PropItem;

//------------------------------------------------------------------------------
typedef void 	__fastcall (__closure *TBeforeEdit)		(PropValue* sender, LPVOID edit_val);
typedef void 	__fastcall (__closure *TAfterEdit)		(PropValue* sender, LPVOID edit_val);
typedef void 	__fastcall (__closure *TOnDrawEvent)	(PropValue* sender, LPVOID draw_val);
typedef void 	__fastcall (__closure *TOnChange)		(PropValue* sender);
typedef void 	__fastcall (__closure *TOnClick)		(PropItem* sender);
typedef void 	__fastcall (__closure *TOnCloseEvent)	(void);
typedef void 	__fastcall (__closure *TOnModifiedEvent)(void);
typedef void 	__fastcall (__closure *TOnItemFocused)	(TElTreeItem* item);
//------------------------------------------------------------------------------

class PropValue{
	friend class		CPropHelper;
	PropItem*			m_Owner;
public:
	// base events
    TAfterEdit			OnAfterEditEvent;
    TBeforeEdit			OnBeforeEditEvent;
    TOnChange			OnChangeEvent;
public:
						PropValue		():m_Owner(0),OnAfterEditEvent(0),OnBeforeEditEvent(0),OnChangeEvent(0){;}
    virtual LPCSTR		GetText			(TOnDrawEvent OnDraw)=0;
    virtual void		ResetValue		()=0;
    virtual bool		Equal			(PropValue* prop){return true;};//=0;
    virtual bool		ApplyValue		(LPVOID val)=0;
    IC PropItem*		Owner			(){return m_Owner;}
};
//------------------------------------------------------------------------------

template <class T>
IC void set_value(T& val, const T& _val)
{
	val = _val;
};

template <class T>
class CustomValue: public PropValue
{
public:
	typedef T			TYPE;
public:
	TYPE				init_value;
	TYPE*				value;
public:    
						CustomValue		(T* val)
	{
    	set_value		(value,val);
    	set_value		(init_value,*val);
    };
    virtual LPCSTR		GetText			(TOnDrawEvent OnDraw){return 0;}
//    virtual bool		Equal			(PropValue* prop){ return (*value!=*val); }
    virtual const T&	GetValue		(){return *value; }
    virtual void		ResetValue		(){set_value(*value,init_value);}
    virtual bool		ApplyValue		(LPVOID _val)
    {
    	TYPE* val		= (TYPE*)_val;
        if (*value!=*val){
        	set_value	(*value,*val);
            return		true;
        }
        return 			false;
    }
};

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
    TOnClick			OnClickEvent;
    TOnDrawEvent		OnDrawEvent;
public:
    enum{
    	flDisabled		= (1<<0),
    	flShowCB		= (1<<1),
    	flCBChecked		= (1<<2),
        flMixed			= (1<<3),
        flDrawBtnBorder	= (1<<4),
    };
    Flags32				m_Flags;
public:
						PropItem		(EPropType _type):type(_type),item(0),key(0),tag(0),subitem(1),OnDrawEvent(0),OnClickEvent(0){m_Flags.zero();}
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
    IC LPCSTR			GetText			(){VERIFY(!values.empty()); return m_Flags.is(flMixed)?"(mixed)":values.front()->GetText(OnDrawEvent);}
    IC bool 			ApplyValue		(LPVOID val)
    {
    	bool bChanged	= false;
    	for (PropValueIt it=values.begin(); it!=values.end(); it++)
        	if ((*it)->ApplyValue(val)) bChanged = true;
        if (bChanged) 	m_Flags.set(flMixed,FALSE);
        return bChanged;
    }
    IC PropValue*		GetFrontValue	(){VERIFY(!values.empty()); return values.front(); };
    IC EPropType		Type			(){return type;}

	IC void				OnBeforeEdit	(LPVOID edit_val)
    {
    	for (PropValueIt it=values.begin(); it!=values.end(); it++)
        	if ((*it)->OnBeforeEditEvent) (*it)->OnBeforeEditEvent(*it,edit_val);
    }
	IC void				OnAfterEdit		(LPVOID edit_val)
    {
    	for (PropValueIt it=values.begin(); it!=values.end(); it++)
        	if ((*it)->OnAfterEditEvent) 	(*it)->OnAfterEditEvent(*it,edit_val);
    }
	IC void				OnChange		()
    {
    	for (PropValueIt it=values.begin(); it!=values.end(); it++)
        	if ((*it)->OnChangeEvent) 		(*it)->OnChangeEvent(*it);
    }
	IC void				OnClick			()
    {
    	if (OnClickEvent)	OnClickEvent(this);
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
    virtual LPCSTR		GetText			(TOnDrawEvent){return value.c_str();}
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
    virtual LPCSTR		GetText			(TOnDrawEvent OnDraw);
    virtual bool		Equal			(PropValue* prop){ return (0==strcmp((LPSTR)value,((TextValue*)prop)->value)); }
    virtual bool		ApplyValue		(LPVOID val)
    {
        if (0!=strcmp(value,LPCSTR(val))){
            strcpy(value,LPCSTR(val));
            return		true;
        }
        return 			false;
    }
    LPCSTR				GetValue		(){return value;}
    virtual void		ResetValue		(){strcpy(value,init_value.c_str());}
};
//------------------------------------------------------------------------------

class ATextValue: public CustomValue<AnsiString>{
public:
						ATextValue		(TYPE* val):CustomValue<AnsiString>(val){};
    virtual LPCSTR		GetText			(TOnDrawEvent OnDraw);
};

typedef CustomValue<BOOL>		BOOLValue;
//------------------------------------------------------------------------------

IC bool operator != (const WaveForm& A, const WaveForm& B){return !A.Similar(B);}
class WaveValue: public CustomValue<WaveForm>{
public:
						WaveValue		(TYPE* val):CustomValue<WaveForm>(val){};
    virtual LPCSTR		GetText			(TOnDrawEvent){return "[Wave]";}
};
//------------------------------------------------------------------------------

IC bool operator != (const Fcolor& A, const Fcolor& B)
{	return !A.similar_rgba(B); }
typedef CustomValue<Fcolor>		ColorValue;
//------------------------------------------------------------------------------

template <class T>
IC AnsiString& astring_sprintf(AnsiString& s, const T& V, int tag)
{	s = V; return s;}

template <class T>
class NumericValue: public CustomValue<T>
{
public:
    T					lim_mn;
    T					lim_mx;
    T					inc;
    int 				dec;
public:    
						NumericValue	(T* val):CustomValue<T>(val)
	{
        value			= val;
        init_value		= *value;
        dec				= 0;
    };
						NumericValue	(T* val, T mn, T mx, T increm, int decim):CustomValue<T>(val),lim_mn(mn),lim_mx(mx),inc(increm),dec(decim)
	{
    	clamp			(*val,lim_mn,lim_mx); 
        value			= val;
        init_value		= *value;
    };
    virtual bool		ApplyValue		(LPVOID _val)
    {
    	T* val			= (T*)_val;	
    	clamp			(*val,lim_mn,lim_mx); 
        return CustomValue<T>::ApplyValue(val);
    }
	virtual LPCSTR 		GetText			(TOnDrawEvent OnDraw)
	{
        T edit_val 		= *value;
        if (OnDraw)OnDraw(this, &edit_val);
        static AnsiString prop_draw_text;
        return astring_sprintf	(prop_draw_text,edit_val,0).c_str();
    }
};
//------------------------------------------------------------------------------

typedef NumericValue<u8>	U8Value;
typedef NumericValue<u16>	U16Value;
typedef NumericValue<u32>	U32Value;
typedef NumericValue<s8>	S8Value;
typedef NumericValue<s16>	S16Value;
typedef NumericValue<s32>	S32Value;
//------------------------------------------------------------------------------

IC AnsiString& astring_sprintf(AnsiString& s, const float& V, int dec)
{
    AnsiString fmt; fmt.sprintf("%%.%df",dec);
    s.sprintf(fmt.c_str(),V);
    return s;
}
typedef NumericValue<float>	FloatValue;
//------------------------------------------------------------------------------

IC bool operator != (const Fvector& A, const Fvector& B)
{	return !A.similar(B); }

IC void clamp(Fvector& V, Fvector& mn, const Fvector& mx)
{
    clamp(V.x,mn.x,mx.x); 
    clamp(V.y,mn.y,mx.y); 
    clamp(V.z,mn.z,mx.z); 
}

IC AnsiString& astring_sprintf(AnsiString& s, const Fvector& V, int dec)
{
	AnsiString fmt; fmt.sprintf("{%%.%df, %%.%df, %%.%df}",dec,dec,dec);
	s.sprintf(fmt.c_str(),V.x,V.y,V.z);
    return s;
}

class VectorValue: public NumericValue<Fvector>{
public:
						VectorValue		(Fvector* val, float mn, float mx, float increment, int decimal):NumericValue<Fvector>(val)
    {
    	lim_mn.set		(mn,mn,mn);
    	lim_mx.set		(mx,mx,mx);
        inc.set			(increment,increment,increment);
        dec				= decimal;
    };
};
//------------------------------------------------------------------------------

template <class T>
class FlagValue: public PropValue
{
public:
	typedef T			TYPE;
public:
	TYPE				init_value;
	TYPE*				value;
    T::TYPE				mask;
public:    
						FlagValue		(T* val, T::TYPE _mask):PropValue(),mask(_mask)
	{
    	value			= val;
    	init_value		= *val;
    };
    virtual LPCSTR		GetText			(TOnDrawEvent OnDraw){return 0;}
//    virtual bool		Equal			(PropValue* prop){return value->equal(*((TYPE*)prop)->value,mask);}
    virtual const T&	GetValue		(){return *value; }
    virtual void		ResetValue		(){value->set(mask,init_value.is(mask));}
    bool				GetValueEx		(){return value->is(mask);}
    virtual bool		ApplyValue		(LPVOID _val)
    {
    	BOOL* val		= (BOOL*)_val;
        if (*val!=value->is(mask)){
        	value->set	(mask,(bool)val);
            return 		true;
        }
        return 			false;
    }
};
//------------------------------------------------------------------------------

typedef FlagValue<Flags8>	Flag8Value;
typedef FlagValue<Flags16>	Flag16Value;
typedef FlagValue<Flags32>	Flag32Value;
//------------------------------------------------------------------------------

class TokenValue: public CustomValue<u32>{
    int 				p_size;
public:
	xr_token* 			token;
						TokenValue		(u32* val, xr_token* _token, int p_sz):CustomValue<u32>(val),token(_token),p_size(p_sz){R_ASSERT((p_size>0)&&(p_size<=4));};
	virtual LPCSTR 		GetText			(TOnDrawEvent OnDraw);
	virtual bool		Equal			(PropValue* prop){return (0==memcmp(value,((TokenValue*)prop)->value,p_size));}
    virtual bool		ApplyValue		(LPVOID val)
    {
        if (0!=memcmp(val,value,p_size)){
            CopyMemory(value,val,p_size);
            return		true;
        }
        return 			false;
    }
    virtual void		ResetValue		(){CopyMemory(value,&init_value,p_size);}
};
//------------------------------------------------------------------------------

class TokenValue2: public CustomValue<u32>{
public:
	AStringVec 			items;
public:
						TokenValue2		(u32* val, AStringVec* _items):CustomValue<u32>(val),items(*_items){};
	virtual LPCSTR 		GetText			(TOnDrawEvent OnDraw);
};
//------------------------------------------------------------------------------

class TokenValue3: public CustomValue<u32>{
public:
	struct Item {
		u32				ID;
		string64		str;
	};
	u32					cnt;
    const Item*			items;
public:
						TokenValue3		(u32* val, u32 _cnt, const Item* _items):CustomValue<u32>(val),cnt(_cnt),items(_items){};
	virtual LPCSTR 		GetText			(TOnDrawEvent OnDraw);
};
//------------------------------------------------------------------------------

class ListValue: public PropValue{
	AnsiString			init_value;
	LPSTR				value;
public:
	AStringVec 			items;
						ListValue		(LPSTR val, AStringVec* _items):value(val),init_value(*val),items(*_items){};
						ListValue		(LPSTR val, u32 cnt, LPCSTR* _items):value(val),init_value(*val){items.resize(cnt); int i=0; for (AStringIt it=items.begin(); it!=items.end(); it++,i++) *it=_items[i]; };
	virtual LPCSTR		GetText			(TOnDrawEvent OnDraw);
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
#endif





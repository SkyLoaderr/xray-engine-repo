//---------------------------------------------------------------------------
#ifndef PropertiesListTypesH
#define PropertiesListTypesH

#include "ElTree.hpp"
//---------------------------------------------------------------------------
enum EPropType{
	PROP_CAPTION	= 0x1000,
	PROP_BUTTON,
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
	PROP_A_TOKEN,
	PROP_TOKEN2,
    PROP_TOKEN3,
    PROP_TOKEN4,
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
    PROP_SOUNDSRC,
    PROP_A_SOUNDSRC,
    PROP_SOUNDENV,
    PROP_A_SOUNDENV,
    PROP_LIBPS,
	PROP_LIBPE,
    PROP_LIBPARTICLES,
    PROP_A_LIBPS,
	PROP_A_LIBPE,
    PROP_A_LIBPARTICLES,
    PROP_GAMEOBJECT,
    PROP_GAMEMTL,
	PROP_A_GAMEMTL,
    PROP_ENTITY,
	PROP_WAVE,
    PROP_SCENE_ITEM,
    PROP_CANVAS,
    PROP_TIME,
};
// refs
struct 	xr_token;        
class PropValue;
class PropItem;

//------------------------------------------------------------------------------
typedef void 	__fastcall (__closure *TBeforeEdit)			(PropItem* sender, LPVOID edit_val);
typedef void 	__fastcall (__closure *TAfterEdit)			(PropItem* sender, LPVOID edit_val);
typedef void 	__fastcall (__closure *TOnDrawTextEvent)	(PropValue* sender, LPVOID draw_val);
typedef void 	__fastcall (__closure *TOnChange)			(PropValue* sender);
typedef void 	__fastcall (__closure *TOnClick)			(PropItem* sender);
typedef void 	__fastcall (__closure *TOnBtnClick)			(PropValue* sender, bool& bDataModified);
typedef void 	__fastcall (__closure *TOnCloseEvent)		(void);
typedef void 	__fastcall (__closure *TOnModifiedEvent)	(void);
typedef void 	__fastcall (__closure *TOnItemFocused)		(TElTreeItem* item);
typedef void 	__fastcall (__closure *TOnPropItemFocused)	(PropItem* sender);
typedef void 	__fastcall (__closure *TOnDrawCanvasEvent)	(PropValue* sender, TCanvas* canvas, const TRect& rect);
typedef bool 	__fastcall (__closure *TOnTestEqual)		(PropValue* a, PropValue* b);
//------------------------------------------------------------------------------

class PropValue{
	friend class		CPropHelper;
    friend class		PropItem;
protected:
	PropItem*			m_Owner;
public:
	LPVOID				tag;
public:
	// base events
    TOnChange			OnChangeEvent;
    TOnTestEqual		OnTestEqual;
public:
						PropValue		():tag(0),m_Owner(0),OnChangeEvent(0),OnTestEqual(0){;}
    virtual LPCSTR		GetText			(TOnDrawTextEvent OnDrawText)=0;
    virtual void		ResetValue		()=0;
    virtual bool		Equal			(PropValue* prop)=0;
    virtual bool		ApplyValue		(LPVOID _val)=0;
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
    virtual LPCSTR		GetText			(TOnDrawTextEvent OnDrawText){return 0;}
    virtual bool		Equal			(PropValue* val)
    {
    	if (OnTestEqual) return OnTestEqual(this,val);
    	CustomValue<T>* prop = (CustomValue<T>*)val;
        return (*value==*prop->value);
    }
    virtual const T&	GetValue		(){return *value; }
    virtual void		ResetValue		(){set_value(*value,init_value);}
    virtual bool		ApplyValue		(LPVOID _val)
    {
    	TYPE* val		= (TYPE*)_val;
        if (!(*value==*val)){
        	set_value	(*value,*val);
            return		true;
        }
        return 			false;
    }
};

class PropItem{
	friend class		CPropHelper;
    friend class		TProperties;
    AnsiString			key;
    EPropType			type;
	TElTreeItem*		item;
	DEFINE_VECTOR		(PropValue*,PropValueVec,PropValueIt);
    PropValueVec		values;
// events
public:
    TAfterEdit			OnAfterEditEvent;
    TBeforeEdit			OnBeforeEditEvent;
    TOnDrawTextEvent	OnDrawTextEvent;
    TOnPropItemFocused	OnItemFocused;
    TOnClick			OnClickEvent;
public:
    TRect				draw_rect;
    int 				tag;
    int					subitem;		// multiple selection for each item (SelectTexture for example)
public:
    enum{
    	flDisabled		= (1<<0),
    	flShowCB		= (1<<1),
    	flCBChecked		= (1<<2),
        flMixed			= (1<<3),
        flShowExtBtn	= (1<<4),
        flDrawThumbnail	= (1<<5),
    };
    Flags32				m_Flags;
public:
						PropItem		(EPropType _type):type(_type),item(0),key(0),tag(0),subitem(1),OnClickEvent(0),OnAfterEditEvent(0),OnBeforeEditEvent(0),OnDrawTextEvent(0),OnItemFocused(0){m_Flags.zero();}
	virtual 			~PropItem		()
    {
    	for (PropValueIt it=values.begin(); it!=values.end(); it++)
        	xr_delete	(*it);
    };
    void				SetItemHeight	(int height){item->OwnerHeight=false; item->Height=height;}
    void				SetName			(const AnsiString& name){key=name;}
    IC void				ResetValues		()
    {
    	for (PropValueIt it=values.begin(); it!=values.end(); it++)
        	(*it)->ResetValue();
        CheckMixed		();
    }
    IC void				AppendValue		(PropValue* value)
    {
    	if (!values.empty()&&!value->Equal(values.front()))
        	m_Flags.set	(flMixed,TRUE);
    	values.push_back(value);
    }
    IC LPCSTR			GetText			()
    {
    	VERIFY(!values.empty()); 
        return m_Flags.is(flMixed)?"(mixed)":values.front()->GetText(OnDrawTextEvent);
    }
	IC void				CheckMixed		()
    {
		m_Flags.set		(flMixed,FALSE);
        if (values.size()>1){
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

    IC bool 			ApplyValue		(LPVOID val)
    {
    	bool bChanged	= false;
    	for (PropValueIt it=values.begin(); it!=values.end(); it++)
        	if ((*it)->ApplyValue(val)){
            	bChanged = true;
                if ((*it)->OnChangeEvent) (*it)->OnChangeEvent(*it);
        	}
        if (bChanged)	m_Flags.set(flMixed,FALSE);
        return bChanged;
    }
    IC PropValueVec&	Values			(){return values;}
    IC PropValue*		GetFrontValue	(){VERIFY(!values.empty()); return values.front(); };
    IC EPropType		Type			(){return type;}
	IC TElTreeItem*		Item			(){return item;}
	IC TElTreeItem**	LPItem			(){return &item;}
	IC LPCSTR			Key				(){return key.c_str();}
    IC void				Enable			(BOOL val){m_Flags.set(flDisabled,!val);}
    IC BOOL				Enabled			(){return !m_Flags.is(flDisabled);}

	IC void				OnBeforeEdit	(LPVOID edit_val)
    {
    	if (OnBeforeEditEvent) OnBeforeEditEvent(this,edit_val);
    }
	IC void				OnAfterEdit		(LPVOID edit_val)
    {
    	if (OnAfterEditEvent) OnAfterEditEvent(this,edit_val);
    }
	IC void				OnChange		()
    {
    	for (PropValueIt it=values.begin(); it!=values.end(); it++)
        	if ((*it)->OnChangeEvent) 	(*it)->OnChangeEvent(*it);
    }
};

DEFINE_VECTOR			(PropItem*,PropItemVec,PropItemIt);

//------------------------------------------------------------------------------
// values
//------------------------------------------------------------------------------
class CaptionValue: public PropValue{
	AnsiString			value;
public:
						CaptionValue	(AnsiString val){value=val;}
    virtual LPCSTR		GetText			(TOnDrawTextEvent){return value.c_str();}
    virtual	void		ResetValue		(){;}
    virtual	bool		Equal			(PropValue* val)
    { 
    	if (OnTestEqual) return OnTestEqual(this,val);
    	return (value==((CaptionValue*)val)->value); 
    }
    virtual	bool		ApplyValue		(LPVOID val){value=*(AnsiString*)val; return false;}
};

class CanvasValue: public PropValue{
	AnsiString			value;
public:
    int					height;
    TOnDrawCanvasEvent	OnDrawCanvasEvent;
public:
						CanvasValue		(AnsiString val, int h):OnDrawCanvasEvent(0),height(h){value=val;}
    virtual LPCSTR		GetText			(TOnDrawTextEvent){return value.c_str();}
    virtual	void		ResetValue		(){;}
    virtual	bool		Equal			(PropValue* val)
    {
    	if (OnTestEqual) return OnTestEqual(this,val);
    	return false;
    }
    virtual	bool		ApplyValue		(LPVOID val){return false;}
};

class ButtonValue: public PropValue{
public:
	AStringVec			value;
    int					btn_num;
    TOnBtnClick			OnBtnClickEvent;
    enum{
    	flFirstOnly		= (1<<0)
    };
    Flags8				m_Flags;
public:
						ButtonValue		(AnsiString val, u32 flags)
	{
    	m_Flags.set		(flags);
    	OnBtnClickEvent	= 0;
    	btn_num			= -1;
    	AnsiString 		v;
        int cnt=_GetItemCount(val.c_str()); 
        for (int k=0; k<cnt; k++)
        	value.push_back(_GetItem(val.c_str(),k,v));
    }
    virtual LPCSTR		GetText			(TOnDrawTextEvent){return 0;}
    virtual	void		ResetValue		(){;}
    virtual	bool		Equal			(PropValue* val)
    {
    	if (OnTestEqual) return OnTestEqual(this,val);
    	return true;
    }
    virtual	bool		ApplyValue		(LPVOID val){return false;}
    bool				OnBtnClick		(){if(OnBtnClickEvent)	{ bool bDModif=true; OnBtnClickEvent(this,bDModif); return bDModif;}else return false;}
};

class TextValue: public PropValue{
	AnsiString			init_value;
	LPSTR				value;
public:
	int					lim;
						TextValue		(LPSTR val, int _lim):value(val),init_value(val),lim(_lim){};
    virtual LPCSTR		GetText			(TOnDrawTextEvent OnDrawText);
    virtual bool		Equal			(PropValue* val)
    { 
    	if (OnTestEqual) return OnTestEqual(this,val);
    	return (0==strcmp((LPSTR)value,((TextValue*)val)->value)); 
    }
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
    virtual LPCSTR		GetText			(TOnDrawTextEvent OnDrawText);
};

typedef CustomValue<BOOL>		BOOLValue;
//------------------------------------------------------------------------------

IC bool operator == (const WaveForm& A, const WaveForm& B){return A.Similar(B);}
class WaveValue: public CustomValue<WaveForm>{
public:
						WaveValue		(TYPE* val):CustomValue<WaveForm>(val){};
    virtual LPCSTR		GetText			(TOnDrawTextEvent){return "[Wave]";}
};
//------------------------------------------------------------------------------

IC bool operator == (const Fcolor& A, const Fcolor& B)
{	return A.similar_rgba(B); }
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
	virtual LPCSTR 		GetText			(TOnDrawTextEvent OnDrawText)
	{
        T edit_val 		= *value;
        if (OnDrawText)OnDrawText(this, &edit_val);
        static AnsiString prop_draw_text;
        return astring_sprintf	(prop_draw_text,edit_val,dec).c_str();
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

IC bool operator == (const Fvector& A, const Fvector& B)
{	return A.similar(B); }
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
    virtual LPCSTR		GetText			(TOnDrawTextEvent OnDrawText){return 0;}
    virtual bool		Equal			(PropValue* val)
    {
    	if (OnTestEqual) return OnTestEqual(this,val);
    	return value->equal(*((FlagValue<T>*)val)->value,mask);
    }
    virtual const T&	GetValue		(){return *value; }
    virtual void		ResetValue		(){value->set(mask,init_value.is(mask));}
    bool				GetValueEx		(){return value->is(mask);}
    virtual bool		ApplyValue		(LPVOID _val)
    {
    	T* val			= (T*)_val;
        if (!val->equal(*value,mask)){
        	value->set	(mask,val->is(mask));
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
	virtual LPCSTR 		GetText			(TOnDrawTextEvent OnDrawText);
	virtual bool		Equal			(PropValue* val)
    {
    	if (OnTestEqual) return OnTestEqual(this,val);
    	return (0==memcmp(value,((TokenValue*)val)->value,p_size));
    }
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

class ATokenValue: public CustomValue<u32>{
    int 				p_size;
public:
	ATokenVec*			token;
						ATokenValue		(u32* val, ATokenVec* _token, int p_sz):CustomValue<u32>(val),token(_token),p_size(p_sz){R_ASSERT((p_size>0)&&(p_size<=4));};
	virtual LPCSTR 		GetText			(TOnDrawTextEvent OnDrawText);
	virtual bool		Equal			(PropValue* val)
    {
    	if (OnTestEqual) return OnTestEqual(this,val);
    	return (0==memcmp(value,((ATokenValue*)val)->value,p_size));
    }
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
	virtual LPCSTR 		GetText			(TOnDrawTextEvent OnDrawText);
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
	virtual LPCSTR 		GetText			(TOnDrawTextEvent OnDrawText);
};
//------------------------------------------------------------------------------

class TokenValue4: public CustomValue<u32>{
    int 				p_size;
public:
	struct Item {
		u32				ID;
		AnsiString		str;
        				Item			(u32 id, const AnsiString& nm){ID=id; str=nm;}
	};
    DEFINE_VECTOR		(Item,ItemVec,ItemIt);
    const ItemVec*		items;
public:
						TokenValue4		(u32* val, const ItemVec* _items, int p_sz):CustomValue<u32>(val),items(_items),p_size(p_sz){R_ASSERT((p_size>0)&&(p_size<=4));};
	virtual LPCSTR 		GetText			(TOnDrawTextEvent OnDrawText);
	virtual bool		Equal			(PropValue* val)
    {
    	if (OnTestEqual) return OnTestEqual(this,val);
    	return (0==memcmp(value,((TokenValue*)val)->value,p_size));
    }
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

class ListValue: public TextValue{
public:                                   
	AStringVec 			items;
public:                                   
						ListValue		(LPSTR val, int lim, AStringVec* _items):TextValue(val,lim),items(*_items){};
						ListValue		(LPSTR val, int lim, u32 cnt, LPCSTR* _items):TextValue(val,lim){items.resize(cnt); int i=0; for (AStringIt it=items.begin(); it!=items.end(); it++,i++) *it=_items[i]; };
	virtual bool		Equal			(PropValue* val)
    {
    	if (OnTestEqual) return OnTestEqual(this,val);
    	AStringIt s_it	= items.begin();
    	AStringIt d_it	= ((ListValue*)val)->items.begin();
    	for (; s_it!=items.end(); s_it++,d_it++)
        	if ((*s_it)!=(*d_it)) {m_Owner->m_Flags.set(PropItem::flDisabled,TRUE); return false;}
        return TextValue::Equal(val);
    }
};
//------------------------------------------------------------------------------

class SceneItemValue: public TextValue{
public:                  	
	EObjClass			clsID;
	AnsiString			specific;
public:
						SceneItemValue	(LPSTR val, int lim, EObjClass class_id, LPCSTR _type):TextValue(val,lim),clsID(class_id){if(_type) specific=_type;};
	virtual bool		Equal			(PropValue* val)
    {
    	if (OnTestEqual) return OnTestEqual(this,val);
    	if ((clsID!=((SceneItemValue*)val)->clsID)||(specific!=((SceneItemValue*)val)->specific)){
        	m_Owner->m_Flags.set(PropItem::flDisabled,TRUE); 
            return false;
        }
        return TextValue::Equal(val);
    }
};
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
#endif





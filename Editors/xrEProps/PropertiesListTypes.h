//---------------------------------------------------------------------------
#ifndef PropertiesListTypesH
#define PropertiesListTypesH

#include "ElTree.hpp"
#include "ChooseTypes.H"

//---------------------------------------------------------------------------
enum EPropType{
	PROP_UNDEF		= -1,
	PROP_CAPTION	= 0x1000,
	PROP_BUTTON,
    PROP_CHOOSE,
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
	PROP_A_TOKEN,
	PROP_TOKEN2,
    PROP_TOKEN3,
    PROP_SH_TOKEN,
	PROP_LIST,
	PROP_COLOR,
	PROP_FCOLOR,
	PROP_VCOLOR,
	PROP_TEXT,
    PROP_A_TEXT,
	PROP_R_TEXT,
	PROP_TEXTURE2,
	PROP_WAVE,
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
typedef void 	__fastcall (__closure *TOnBtnClick)			(PropValue* sender, bool& bDataModified, bool& bSafe);
typedef void 	__fastcall (__closure *TOnCloseEvent)		(void);
typedef void 	__fastcall (__closure *TOnModifiedEvent)	(void);
typedef void 	__fastcall (__closure *TOnItemFocused)		(TElTreeItem* item);
typedef void 	__fastcall (__closure *TOnPropItemFocused)	(PropItem* sender);
typedef void 	__fastcall (__closure *TOnDrawCanvasEvent)	(PropValue* sender, TCanvas* canvas, const TRect& rect);
typedef bool 	__fastcall (__closure *TOnTestEqual)		(PropValue* a, PropValue* b);
typedef void 	__fastcall (__closure *TOnChoose)			(PropValue* sender, ChooseItemVec& lst);
//------------------------------------------------------------------------------
extern AnsiString prop_draw_text;
//------------------------------------------------------------------------------

class ECORE_API PropValue{
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
class ECORE_API CustomValue: public PropValue
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

class ECORE_API PropItem{
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
	TColor				prop_color;
	TColor				val_color;
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
						PropItem		(EPropType _type):type(_type),prop_color(clBlack),val_color(clBlack),item(0),key(0),tag(0),subitem(1),OnClickEvent(0),OnAfterEditEvent(0),OnBeforeEditEvent(0),OnDrawTextEvent(0),OnItemFocused(0){m_Flags.zero();}
	virtual 			~PropItem		()
    {
    	for (PropValueIt it=values.begin(); values.end() != it; ++it)
        	xr_delete	(*it);
    };
    void				SetItemHeight	(int height){item->OwnerHeight=false; item->Height=height;}
    void				SetName			(const AnsiString& name){key=name;}
    IC void				ResetValues		()
    {
    	for (PropValueIt it=values.begin(); values.end() != it; ++it)
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
        	PropValueIt it	= F; ++it;
	    	for (; values.end() != it; ++it){
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
    	for (PropValueIt it=values.begin(); values.end() != it; ++it)
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
    	for (PropValueIt it=values.begin(); values.end() != it; ++it)
        	if ((*it)->OnChangeEvent) 	(*it)->OnChangeEvent(*it);
    }
};

DEFINE_VECTOR			(PropItem*,PropItemVec,PropItemIt);

//------------------------------------------------------------------------------
// values
//------------------------------------------------------------------------------
class ECORE_API CaptionValue: public PropValue{
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

class ECORE_API CanvasValue: public PropValue{
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

class ECORE_API ButtonValue: public PropValue{
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
        for (int k=0; k<cnt; ++k)
        	value.push_back(_GetItem(val.c_str(),k,v));
    }
    virtual LPCSTR		GetText			(TOnDrawTextEvent);
    virtual	void		ResetValue		(){;}
    virtual	bool		Equal			(PropValue* val)
    {
    	if (OnTestEqual) return OnTestEqual(this,val);
    	return true;
    }
    virtual	bool		ApplyValue		(LPVOID val){return false;}
    bool				OnBtnClick		(bool& bSafe){if(OnBtnClickEvent)	{ bool bDModif=true; OnBtnClickEvent(this,bDModif,bSafe); return bDModif;}else return false;}
};

class ECORE_API TextValue: public PropValue{
	AnsiString			init_value;
	LPSTR				value;
public:
	int					lim;
						TextValue		(LPSTR val, int _lim):value(val),init_value(val),lim(_lim){};
    virtual LPCSTR		GetText			(TOnDrawTextEvent OnDrawText);
    virtual bool		Equal			(PropValue* val)
    { 
    	if (OnTestEqual) return OnTestEqual(this,val);
    	return (0==xr_strcmp((LPSTR)value,((TextValue*)val)->value)); 
    }
    virtual bool		ApplyValue		(LPVOID val)
    {
        if (0!=xr_strcmp(value,LPCSTR(val))){
            strcpy(value,LPCSTR(val));
            return		true;
        }
        return 			false;
    }
    LPCSTR				GetValue		(){return value;}
    virtual void		ResetValue		(){strcpy(value,init_value.c_str());}
};
//------------------------------------------------------------------------------

class ECORE_API ATextValue: public CustomValue<AnsiString>{
public:
						ATextValue		(TYPE* val):CustomValue<AnsiString>(val){};
    virtual LPCSTR		GetText			(TOnDrawTextEvent OnDrawText);
    virtual bool		ApplyValue		(LPVOID val)
    {
        if (0!=xr_strcmp(value->c_str(),LPCSTR(val))){
            *value		= LPCSTR(val);
            return		true;
        }
        return 			false;
    }
};

class ECORE_API RTextValue: public CustomValue<ref_str>{
public:
						RTextValue		(TYPE* val):CustomValue<ref_str>(val){};
    virtual LPCSTR		GetText			(TOnDrawTextEvent OnDrawText);
    virtual bool		Equal			(PropValue* val)
    { 
    	if (OnTestEqual) return OnTestEqual(this,val);
    	return (value->equal(*((RTextValue*)val)->value)); 
    }
    virtual bool		ApplyValue		(LPVOID val)
    {
    	ref_str rv		= LPCSTR(val);
        if (!value->equal(rv)){
            *value		= rv;
            return		true;
        }
        return 			false;
    }
};

class ECORE_API ChooseValueCustom{
public:
	EChooseMode			choose_mode;
    TOnChoose			OnChooseEvent;
    AnsiString			start_path;
public:
						ChooseValueCustom	(EChooseMode mode, LPCSTR path):choose_mode(mode),OnChooseEvent(0),start_path(path){;}
};

class ECORE_API ChooseValue: public TextValue, public ChooseValueCustom{
public:
						ChooseValue			(LPSTR val, int len, EChooseMode mode, LPCSTR path):TextValue(val,len),ChooseValueCustom(mode,path){;}
};

class ECORE_API AChooseValue: public ATextValue, public ChooseValueCustom{
public:
						AChooseValue		(AnsiString* val, EChooseMode mode, LPCSTR path):ATextValue(val),ChooseValueCustom(mode,path){;}
};

class ECORE_API RChooseValue: public RTextValue, public ChooseValueCustom{
public:
						RChooseValue		(ref_str* val, EChooseMode mode, LPCSTR path):RTextValue(val),ChooseValueCustom(mode,path){;}
};

typedef CustomValue<BOOL>		BOOLValue;
//------------------------------------------------------------------------------

IC bool operator == (const WaveForm& A, const WaveForm& B){return A.Similar(B);}
class ECORE_API WaveValue: public CustomValue<WaveForm>{
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
class ECORE_API NumericValue: public CustomValue<T>
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
class ECORE_API VectorValue: public NumericValue<Fvector>{
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

class ECORE_API FlagValueCustom: public PropValue
{
public:
    AnsiString			caption[2];
public:
	virtual	bool		HaveCaption		(){return caption[0].Length()&&caption[1].Length();}
    virtual bool		GetValueEx		()=0;
};

template <class T>
class ECORE_API FlagValue: public FlagValueCustom
{
public:
	typedef T			TYPE;
public:
	TYPE				init_value;
	TYPE*				value;
    T::TYPE				mask;
public:
						FlagValue		(T* val, T::TYPE _mask, LPCSTR c0, LPCSTR c1):FlagValueCustom(),mask(_mask)
	{
    	value			= val;
    	init_value		= *val;
        caption[0]		= c0;
        caption[1]		= c1;
    };
    virtual LPCSTR		GetText			(TOnDrawTextEvent OnDrawText)
    {	
    	return HaveCaption()?caption[GetValueEx()?1:0].c_str():0;
    }
    virtual bool		Equal			(PropValue* val)
    {
    	if (OnTestEqual) return OnTestEqual(this,val);
    	return value->equal(*((FlagValue<T>*)val)->value,mask);
    }
    virtual const T&	GetValue		(){return *value; }
    virtual void		ResetValue		(){value->set(mask,init_value.is(mask));}
    virtual bool		GetValueEx		(){return value->is(mask);}
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

class ECORE_API TokenValueCustom{
public:
	xr_token* 			token;
    					TokenValueCustom(xr_token* _token):token(_token){;}
};
template <class T>
class ECORE_API TokenValue: public CustomValue<T>, public TokenValueCustom
{
public:
						TokenValue		(T* val, xr_token* _token):TokenValueCustom(_token),CustomValue<T>(val){};
    virtual LPCSTR		GetText			(TOnDrawTextEvent OnDrawText)
    {
        T draw_val 		= GetValue();
        if (OnDrawText)	OnDrawText(this, &draw_val);
        for(int i=0; token[i].name; i++) if (token[i].id==draw_val) return token[i].name;
        return 0;
    }
};
//------------------------------------------------------------------------------
typedef TokenValue<u8>	Token8Value;
typedef TokenValue<u16>	Token16Value;
typedef TokenValue<u32>	Token32Value;
//------------------------------------------------------------------------------

class ECORE_API ATokenValueCustom{
public:
	ATokenVec*			token;
    					ATokenValueCustom(ATokenVec* _token):token(_token){;}
};
template <class T>
class ECORE_API ATokenValue: public CustomValue<T>, public ATokenValueCustom
{
public:
						ATokenValue		(T* val, ATokenVec* _token):CustomValue<T>(val),ATokenValueCustom(_token){};
    virtual LPCSTR		GetText			(TOnDrawTextEvent OnDrawText)
    {
        u32 draw_val 	= GetValue();
        if (OnDrawText)OnDrawText(this, &draw_val);
        for(ATokenIt it=token->begin(); it!=token->end(); it++) if (it->id==draw_val) return it->name.c_str();
        return 0;
    }
};
//------------------------------------------------------------------------------
typedef ATokenValue<u8>	AToken8Value;
typedef ATokenValue<u16>AToken16Value;
typedef ATokenValue<u32>AToken32Value;
//------------------------------------------------------------------------------

class ECORE_API TokenValue2Custom{
public:
	AStringVec 			items;
    					TokenValue2Custom(AStringVec* _items):items(*_items){;}
};
template <class T>
class ECORE_API TokenValue2: public CustomValue<T>, public TokenValue2Custom
{
public:
						TokenValue2		(T* val, AStringVec* _items):CustomValue<T>(val),TokenValue2Custom(_items){};
    virtual LPCSTR		GetText			(TOnDrawTextEvent OnDrawText)
    {
        T draw_val 		= GetValue();
        if (OnDrawText)OnDrawText(this, &draw_val);
        if (draw_val>=items.size()) return 0;
        return items[draw_val].c_str();
    }
};
//------------------------------------------------------------------------------
typedef TokenValue2<u8>		Token8Value2;
typedef TokenValue2<u16> 	Token16Value2;
typedef TokenValue2<u32> 	Token32Value2;
//------------------------------------------------------------------------------

class ECORE_API TokenValue3Custom{
public:
	struct Item {
		u32				ID;
		AnsiString		str;
        				Item			(u32 id, const AnsiString& nm){ID=id; str=nm;}
	};
    DEFINE_VECTOR		(Item,ItemVec,ItemIt);
    const ItemVec*		items;
    					TokenValue3Custom(const ItemVec* _items):items(_items){;}
};
template <class T>
class ECORE_API TokenValue3: public CustomValue<T>, public TokenValue3Custom 
{
public:
						TokenValue3		(T* val, const ItemVec* _items):CustomValue<T>(val),TokenValue3Custom(_items){};
    virtual LPCSTR		GetText			(TOnDrawTextEvent OnDrawText)
    {
        T draw_val 		= GetValue();
        if (OnDrawText)OnDrawText(this, &draw_val);
        for (ItemVec::const_iterator it=items->begin(); it!=items->end(); it++)
            if (it->ID==draw_val) return it->str.c_str();
        return 0;
    }
};
//------------------------------------------------------------------------------
typedef TokenValue3<u8>		Token8Value3;
typedef TokenValue3<u16> 	Token16Value3;
typedef TokenValue3<u32> 	Token32Value3;
//------------------------------------------------------------------------------

class ECORE_API TokenValueSH: public CustomValue<u32>{
public:
	struct Item {
		u32				ID;
		string64		str;

	};
	u32					cnt;
    const Item*			items;
public:
						TokenValueSH	(u32* val, u32 _cnt, const Item* _items):CustomValue<u32>(val),cnt(_cnt),items(_items){};
	virtual LPCSTR 		GetText			(TOnDrawTextEvent OnDrawText);
};
//------------------------------------------------------------------------------

class ECORE_API ListValue: public TextValue{
public:                                   
	AStringVec 			items;
public:                                   
						ListValue		(LPSTR val, int lim, AStringVec* _items):TextValue(val,lim),items(*_items){};
						ListValue		(LPSTR val, int lim, u32 cnt, LPCSTR* _items):TextValue(val,lim){items.resize(cnt); int i=0; for (AStringIt it=items.begin(); items.end() != it; ++it,++i) *it=_items[i]; };
	virtual bool		Equal			(PropValue* val)
    {
    	if (OnTestEqual) return OnTestEqual(this,val);
    	AStringIt s_it	= items.begin();
    	AStringIt d_it	= ((ListValue*)val)->items.begin();
    	for (; items.end()!=s_it; ++s_it,++d_it)
        	if ((*s_it)!=(*d_it)) {m_Owner->m_Flags.set(PropItem::flDisabled,TRUE); return false;}
        return TextValue::Equal(val);
    }
};
//------------------------------------------------------------------------------

class ECORE_API SceneItemValue: public RTextValue{
public:                  	
	EObjClass			clsID;
	AnsiString			specific;
public:
						SceneItemValue	(ref_str* val, EObjClass class_id, LPCSTR _type):RTextValue(val),clsID(class_id){if(_type) specific=_type;};
	virtual bool		Equal			(PropValue* val)
    {
    	if (OnTestEqual) return OnTestEqual(this,val);
    	if ((clsID!=((SceneItemValue*)val)->clsID)||(specific!=((SceneItemValue*)val)->specific)){
        	m_Owner->m_Flags.set(PropItem::flDisabled,TRUE); 
            return false;
        }
        return RTextValue::Equal(val);
    }
};
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
#endif





//---------------------------------------------------------------------------
#ifndef PropertiesListTypesH
#define PropertiesListTypesH

#include "WaveForm.H"

#ifdef __BORLANDC__            
#	include "ElTree.hpp"
#endif
//---------------------------------------------------------------------------
enum EPropType{
	PROP_UNDEF		= -1,
	PROP_CAPTION	= 0x1000,
	PROP_BUTTON,
    PROP_CHOOSE,
	PROP_NUMERIC,	// {u8,u16,u32,s8,s16,s32,f32}
	PROP_BOOLEAN,
	PROP_FLAG,
    PROP_VECTOR, 
	PROP_TOKEN,
	PROP_RTOKEN,
    PROP_SH_TOKEN,
	PROP_LIST,
	PROP_COLOR,
	PROP_FCOLOR,
	PROP_VCOLOR,
	PROP_RTEXT,
	PROP_TEXTURE2,
	PROP_WAVE,
    PROP_CANVAS,
    PROP_TIME,
};
// refs
struct 	xr_token;        
class PropValue;
class PropItem;
DEFINE_VECTOR			(PropItem*,PropItemVec,PropItemIt);

//------------------------------------------------------------------------------
#include "ChooseTypes.H"     
#include "fastdelegate.h"                         
//------------------------------------------------------------------------------
typedef fastdelegate::FastDelegate2<PropValue*, ref_str&> 		TOnDrawTextEvent; 
typedef fastdelegate::FastDelegate1<PropItem*> 					TOnClick;
//------------------------------------------------------------------------------

class PropValue
{
	friend class		CPropHelper;
    friend class		PropItem;
protected:
	PropItem*			m_Owner;
public:
	u32					tag;
public:
	// base events
	typedef fastdelegate::FastDelegate1<PropValue*> TOnChange;
    TOnChange			OnChangeEvent;
public:
						PropValue		():tag(0),m_Owner(0),OnChangeEvent(0){;}
    virtual ref_str		GetText			(TOnDrawTextEvent OnDrawText)=0;
    virtual void		ResetValue		()=0;
    virtual bool		Equal			(PropValue* prop)=0;
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
	typedef fastdelegate::FastDelegate2<PropValue*, T&> 		TOnBeforeEditEvent;
	typedef fastdelegate::FastDelegate3<PropValue*, T&, bool&> 	TOnAfterEditEvent;
    TOnBeforeEditEvent	OnBeforeEditEvent;
    TOnAfterEditEvent	OnAfterEditEvent;
public:
						CustomValue		(T* val)
	{
    	OnBeforeEditEvent 	= 0;
        OnAfterEditEvent	= 0;
    	set_value		(value,val);
    	set_value		(init_value,*val);
    };
    virtual ref_str		GetText			(TOnDrawTextEvent OnDrawText){return 0;}
    virtual bool		Equal			(PropValue* val)
    {
    	CustomValue<T>* prop = (CustomValue<T>*)val;
        return (*value==*prop->value);
    }
    virtual const T&	GetValue		(){return *value; }
    virtual void		ResetValue		(){set_value(*value,init_value);}
    bool				ApplyValue		(const T& val)
    {
		if (!(*value==val)){
            set_value	(*value,val);
            return		true;
        }
        return 			false;
    }
};

class PropItem
{
	friend class		CPropHelper;
    friend class		TProperties;
    ref_str				key;
    EPropType			type;
	void*				item;
	DEFINE_VECTOR		(PropValue*,PropValueVec,PropValueIt);
    PropValueVec		values;
// events
public:
	typedef fastdelegate::FastDelegate1<PropItem*> 	TOnPropItemFocused;
    TOnDrawTextEvent	OnDrawTextEvent;
    TOnPropItemFocused	OnItemFocused;
    TOnClick			OnClickEvent;
public:
	u32					prop_color;
	u32					val_color;
    Irect				draw_rect;
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
						PropItem		(EPropType _type):type(_type),prop_color(clBlack),val_color(clBlack),item(0),key(0),subitem(1),OnClickEvent(0),OnDrawTextEvent(0),OnItemFocused(0){m_Flags.zero();}
	virtual 			~PropItem		()
    {
    	for (PropValueIt it=values.begin(); values.end() != it; ++it)
        	xr_delete	(*it);
    };
    void				SetName			(const ref_str& name)
    {
    	key=name;
    }
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
    IC ref_str			GetText			()
    {
    	VERIFY(!values.empty()); 
        return m_Flags.is(flMixed)?ref_str("(mixed)"):values.front()->GetText(OnDrawTextEvent);
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

    template <class T1, class T2>
    IC void 			BeforeEdit		(T2& val)
    {
        T1* CV			= dynamic_cast<T1*>(values.front()); VERIFY(CV);
        if (!CV->OnBeforeEditEvent.empty()) CV->OnBeforeEditEvent(CV,val);
    }
    template <class T1, class T2>
    IC bool 			AfterEdit		(T2& val)
    {
    	bool Accepted	= true;
        T1* CV			= dynamic_cast<T1*>(values.front()); VERIFY(CV);
        if (!CV->OnAfterEditEvent.empty()) CV->OnAfterEditEvent(CV,val,Accepted);
        return Accepted;
	}    
    template <class T1, class T2>
    IC bool 			ApplyValue		(const T2& val)
    {
    	bool bChanged	= false;
        m_Flags.set		(flMixed,FALSE);
    	for (PropValueIt it=values.begin(); values.end() != it; ++it){
        	T1* CV		= dynamic_cast<T1*>(*it); VERIFY(CV);
        	if (CV->ApplyValue(val)){
            	bChanged = true;
                if (!CV->OnChangeEvent.empty()) CV->OnChangeEvent(*it);
            }
            if (!CV->Equal(values.front()))
                m_Flags.set	(flMixed,TRUE);
        }
        return bChanged;
    }
    IC PropValueVec&	Values			(){return values;}
    IC PropValue*		GetFrontValue	(){VERIFY(!values.empty()); return values.front(); };
    IC EPropType		Type			(){return type;}
#ifdef __BORLANDC__    
	IC TElTreeItem*		Item			(){return (TElTreeItem*)item;}
#endif
	IC LPCSTR			Key				(){return key.c_str();}
    IC void				Enable			(BOOL val){m_Flags.set(flDisabled,!val);}
    IC BOOL				Enabled			(){return !m_Flags.is(flDisabled);}
	IC void				OnChange		()
    {
    	for (PropValueIt it=values.begin(); values.end() != it; ++it)
        	if (!(*it)->OnChangeEvent.empty()) 	
            	(*it)->OnChangeEvent(*it);
    }
/*    
    template <class T1, class T2>
	IC void				OnBeforeEdit	()
    {
    	for (PropValueIt it=values.begin(); values.end() != it; ++it){
        	T1* CV		= dynamic_cast<T1*>(*it); VERIFY(CV);
        	if (CV->OnChangeEvent) 		CV->OnChangeEvent(*it);
        }
    }
*/
};

//------------------------------------------------------------------------------
// values
//------------------------------------------------------------------------------
class CaptionValue: public PropValue{
	ref_str				value;
public:
						CaptionValue	(const ref_str& val){value=val;}
    virtual ref_str		GetText			(TOnDrawTextEvent)	{return value.c_str();}
    virtual	void		ResetValue		(){;}
    virtual	bool		Equal			(PropValue* val)	{return (value==((CaptionValue*)val)->value);}
    bool				ApplyValue		(const ref_str& val){value=val; return false;}
};

class CanvasValue: public PropValue{
	ref_str				value;
public:
	typedef fastdelegate::FastDelegate3<CanvasValue*,PropValue*,bool&>			TOnTestEqual;
	typedef fastdelegate::FastDelegate3<CanvasValue*,void*/* TCanvas* */, const Irect&>	TOnDrawCanvasEvent;
public:
    int					height;
    TOnTestEqual		OnTestEqual;
    TOnDrawCanvasEvent	OnDrawCanvasEvent;
public:
						CanvasValue		(const ref_str& val, int h):OnDrawCanvasEvent(0),OnTestEqual(0),height(h){value=val;}
    virtual ref_str		GetText			(TOnDrawTextEvent){return value.c_str();}
    virtual	void		ResetValue		(){;}
    virtual	bool		Equal			(PropValue* val)
    {
    	if (!OnTestEqual.empty()){bool res=true; OnTestEqual(this,val,res); return res;}
    	return false;
    }
};

class ButtonValue: public PropValue{
public:
	RStringVec			value;
    int					btn_num;
	typedef fastdelegate::FastDelegate3<PropValue*, bool&, bool&> 	TOnBtnClick;
    TOnBtnClick			OnBtnClickEvent;
    enum{
    	flFirstOnly		= (1<<0)
    };
    Flags8				m_Flags;
public:
						ButtonValue		(const ref_str& val, u32 flags)
	{
    	m_Flags.assign	(flags);
    	OnBtnClickEvent	= 0;
    	btn_num			= -1;
    	std::string 	v;
        int cnt=_GetItemCount(val.c_str()); 
        for (int k=0; k<cnt; ++k)
        	value.push_back(_GetItem(val.c_str(),k,v));
    }
    virtual ref_str		GetText			(TOnDrawTextEvent)
    {
        return 			_ListToSequence(value).c_str();
    }
    virtual	void		ResetValue		(){;}
    virtual	bool		Equal			(PropValue* val)							{return true;}
    bool				OnBtnClick		(bool& bSafe){if(!OnBtnClickEvent.empty())	{bool bDModif=true; OnBtnClickEvent(this,bDModif,bSafe); return bDModif;}else return false;}
};
//------------------------------------------------------------------------------

class RTextValue: public CustomValue<ref_str>{
public:
						RTextValue		(TYPE* val):CustomValue<ref_str>(val){};
    virtual ref_str		GetText			(TOnDrawTextEvent OnDrawText)
    {
        ref_str txt		= GetValue();
        if (!OnDrawText.empty())OnDrawText(this, txt);
        return txt.c_str();
    }
};

class ChooseValue: public RTextValue{
public:
	u32					choose_id;
    ref_str				start_path;
	typedef fastdelegate::FastDelegate1<ChooseItemVec&>	TOnChooseFillProp;
    TOnChooseFillProp	OnChooseFillEvent;
public:
						ChooseValue			(ref_str* val, u32 cid, LPCSTR path):RTextValue(val),choose_id(cid),start_path(path),OnChooseFillEvent(0){}
};

typedef CustomValue<BOOL>		BOOLValue;
//------------------------------------------------------------------------------

IC bool operator == (const WaveForm& A, const WaveForm& B){return A.Similar(B);}
class WaveValue: public CustomValue<WaveForm>{
public:
						WaveValue		(TYPE* val):CustomValue<WaveForm>(val){};
    virtual ref_str		GetText			(TOnDrawTextEvent){return "[Wave]";}
};
//------------------------------------------------------------------------------

IC bool operator == (const Fcolor& A, const Fcolor& B)
{	return A.similar_rgba(B); }
typedef CustomValue<Fcolor>		ColorValue;
//------------------------------------------------------------------------------

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
    bool				ApplyValue		(const T& _val)
    {
    	T val			= _val;
    	clamp			(val,lim_mn,lim_mx);
        return CustomValue<T>::ApplyValue(val);
    }
	virtual ref_str		GetText			(TOnDrawTextEvent OnDrawText)
	{
        ref_str 		draw_val;
        if (!OnDrawText.empty())	OnDrawText(this, draw_val);
        else			rstring_sprintf	(draw_val,*value,dec);
        return draw_val.c_str();
    }
};

//------------------------------------------------------------------------------
template <class T>
IC ref_str rstring_sprintf(ref_str& s, const T& V, int tag)
{	s.sprintf("%d",V); return s;}
//------------------------------------------------------------------------------
IC ref_str rstring_sprintf(ref_str& s, const float& V, int dec)
{
    ref_str fmt; fmt.sprintf("%%.%df",dec);
    s.sprintf(fmt.c_str(),V);
    return s;
}
//------------------------------------------------------------------------------
IC bool operator == (const Fvector& A, const Fvector& B)
{	return A.similar(B); }
IC void clamp(Fvector& V, const Fvector& mn, const Fvector& mx)
{
    clamp(V.x,mn.x,mx.x);
    clamp(V.y,mn.y,mx.y);
    clamp(V.z,mn.z,mx.z);
}
IC ref_str rstring_sprintf(ref_str& s, const Fvector& V, int dec)
{
	ref_str fmt;fmt.sprintf("{%%.%df, %%.%df, %%.%df}",dec,dec,dec);
	s.sprintf	(fmt.c_str(),V.x,V.y,V.z);
    return s;
}
//------------------------------------------------------------------------------
typedef NumericValue<u8>	U8Value;
typedef NumericValue<u16>	U16Value;
typedef NumericValue<u32>	U32Value;
typedef NumericValue<s8>	S8Value;
typedef NumericValue<s16>	S16Value;
typedef NumericValue<s32>	S32Value;
typedef NumericValue<float>	FloatValue;
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

class FlagValueCustom
{
public:
    ref_str				caption[2];
    enum{
    	flInvertedDraw	= (1<<0),
    };
    Flags32				m_Flags;
public:
						FlagValueCustom	(u32 mask, LPCSTR c0, LPCSTR c1)
    {
        caption[0]		= c0;
        caption[1]		= c1;
        m_Flags.assign(mask);
    }
	virtual	bool		HaveCaption		(){return caption[0].size()&&caption[1].size();}
    virtual bool		GetValueEx		()=0;
};

template <class T>
class FlagValue: public CustomValue<T>, public FlagValueCustom
{
public:
	typedef T			TYPE;
public:
    T::TYPE				mask;
public:
						FlagValue		(T* val, T::TYPE _mask, LPCSTR c0, LPCSTR c1, u32 flags):CustomValue<T>(val),FlagValueCustom(flags,c0,c1),mask(_mask){}
    virtual ref_str		GetText			(TOnDrawTextEvent OnDrawText)
    {	
        ref_str 		draw_val;
        if (!OnDrawText.empty())	OnDrawText(this, draw_val);
        else 			return HaveCaption()?caption[GetValueEx()?1:0].c_str():0;
        return			draw_val.c_str();
    }
    virtual bool		Equal			(PropValue* val){return value->equal(*((FlagValue<T>*)val)->value,mask);}
    virtual const T&	GetValue		()				{return *value; }
    virtual void		ResetValue		()				{value->set(mask,init_value.is(mask));}
    virtual bool		GetValueEx		()				{return value->is(mask);}
    bool				ApplyValue		(const T& val)
    {
        if (!val.equal(*value,mask)){
            value->set	(mask,val.is(mask));
            return		true;
        }
        return 			false;
    }
};
//------------------------------------------------------------------------------
typedef FlagValue<Flags8>	Flag8Value;
typedef FlagValue<Flags16>	Flag16Value;
typedef FlagValue<Flags32>	Flag32Value;
//------------------------------------------------------------------------------
template <class T>
bool operator == (_flags<T> const & A, _flags<T>  const & B){return A.flags==B.flags;}
//------------------------------------------------------------------------------

class TokenValueCustom{
public:                                          
	xr_token* 			token;
    					TokenValueCustom(xr_token* _token):token(_token){;}
};
template <class T>
class TokenValue: public CustomValue<T>, public TokenValueCustom
{
public:
						TokenValue		(T* val, xr_token* _token):TokenValueCustom(_token),CustomValue<T>(val){};
    virtual ref_str		GetText			(TOnDrawTextEvent OnDrawText)
    {
        ref_str 		draw_val;
        if (!OnDrawText.empty())	OnDrawText(this, draw_val);
        else			for(int i=0; token[i].name; i++) if (token[i].id==GetValue()) return token[i].name;
        return draw_val.c_str();
    }
};
//------------------------------------------------------------------------------
typedef TokenValue<u8>	Token8Value;
typedef TokenValue<u16>	Token16Value;
typedef TokenValue<u32>	Token32Value;
//------------------------------------------------------------------------------

class RTokenValueCustom{
public:
	RTokenVec*			token;
    					RTokenValueCustom(RTokenVec* _token):token(_token){;}
};
template <class T>
class RTokenValue: public CustomValue<T>, public RTokenValueCustom
{
public:
						RTokenValue		(T* val, RTokenVec* _token):CustomValue<T>(val),RTokenValueCustom(_token){};
    virtual ref_str		GetText			(TOnDrawTextEvent OnDrawText)
    {
        ref_str draw_val;
        if (!OnDrawText.empty())	OnDrawText(this, draw_val);
        else			for(RTokenVecIt it=token->begin(); it!=token->end(); it++) if (it->id==GetValue()) return *it->name;
        return draw_val.c_str();
    }
};
//------------------------------------------------------------------------------
typedef RTokenValue<u8>	RToken8Value;
typedef RTokenValue<u16>RToken16Value;
typedef RTokenValue<u32>RToken32Value;
//------------------------------------------------------------------------------

class TokenValueSH: public CustomValue<u32>{
public:
	struct Item {
		u32				ID;
		string64		str;

	};
	u32					cnt;
    const Item*			items;
public:
						TokenValueSH	(u32* val, u32 _cnt, const Item* _items):CustomValue<u32>(val),cnt(_cnt),items(_items){};
	virtual ref_str		GetText			(TOnDrawTextEvent OnDrawText)
    {
        u32 draw_val 	= GetValue();
        for(u32 i=0; i<cnt; i++) if (items[i].ID==draw_val) return items[i].str;
        return 0;
    }
};
//------------------------------------------------------------------------------

class ListValue: public RTextValue{
public:                                   
	RStringVec*			items;
public:                                   
						ListValue		(ref_str* val, RStringVec* _items):RTextValue(val),items(_items){};
	virtual bool		Equal			(PropValue* val)
    {
        if (items!=((ListValue*)val)->items){
        	m_Owner->m_Flags.set(PropItem::flDisabled,TRUE); 
        	return false;
        }
        return RTextValue::Equal(val);
    }
};
//------------------------------------------------------------------------------

#endif





#ifndef xrEPropsH
#define xrEPropsH

#ifdef XR_EPROPS_EXPORTS
	#define XR_EPROPS_API __declspec(dllexport)
#else
	#define XR_EPROPS_API __declspec(dllimport)
#endif

#include "PropertiesListTypes.h"
#include "ItemListTypes.h"
                                        
enum EItemType{
	TYPE_INVALID= -1,
	TYPE_FOLDER	= 0,
	TYPE_OBJECT	= 1
};

typedef fastdelegate::FastDelegate1<ListItemsVec&> 				TOnILItemsFocused;
typedef fastdelegate::FastDelegate0								TOnILCloseEvent;      
typedef fastdelegate::FastDelegate3<LPCSTR, LPCSTR, EItemType>	TOnItemRename;
typedef fastdelegate::FastDelegate3<LPCSTR, EItemType, bool&>	TOnItemRemove;
typedef fastdelegate::FastDelegate0								TOnItemAfterRemove;
typedef fastdelegate::FastDelegate0 							TOnCloseEvent;
typedef fastdelegate::FastDelegate0 		  					TOnModifiedEvent;

#ifdef __BORLANDC__
#	include "mxPlacemnt.hpp"
	DEFINE_VECTOR	(TElTreeItem*,ElItemsVec,ElItemsIt);
    typedef fastdelegate::FastDelegate1<TElTreeItem*>			TOnILItemFocused;
	//---------------------------------------------------------------------------
	void XR_EPROPS_API CheckWindowPos(TForm* form);
	//---------------------------------------------------------------------------
#endif

//------------------------------------------------------------------------------
// Prepare Key
//------------------------------------------------------------------------------
IC std::string FolderAppend	(LPCSTR val)
{
	std::string	tmp 	= val;
    if (val&&val[0])tmp	+= "\\";
    return 	tmp;
}
IC ref_str PrepareKey	(LPCSTR pref, 	LPCSTR key)
{
    R_ASSERT	(key);
	return 		ref_str(std::string(FolderAppend(pref)+key).c_str());
}
IC ref_str PrepareKey (LPCSTR pref0, 	LPCSTR pref1,	LPCSTR key)
{
    R_ASSERT	(key);
    return 		ref_str(std::string(FolderAppend(pref0)+FolderAppend(pref1)+key).c_str());
}
IC ref_str PrepareKey (LPCSTR pref0,	LPCSTR pref1, 	LPCSTR pref2,	LPCSTR key)
{
    R_ASSERT	(key);
    return 		ref_str(std::string(FolderAppend(pref0)+FolderAppend(pref1)+FolderAppend(pref2)+key).c_str());
}
//------------------------------------------------------------------------------
// Properties
//------------------------------------------------------------------------------
class XR_EPROPS_API IPropHelper{
public:
    virtual PropItem* 			__stdcall	FindItem			(PropItemVec& items, ref_str key, EPropType type=PROP_UNDEF)=0;
public:
//------------------------------------------------------------------------------
// predefind event routines
    virtual void 				__stdcall 	FvectorRDOnAfterEdit(PropValue* sender, Fvector& edit_val, bool& accepted)=0;
    virtual void 				__stdcall 	FvectorRDOnBeforeEdit(PropValue* sender,Fvector& edit_val)=0;
    virtual void 				__stdcall 	FvectorRDOnDraw		(PropValue* sender, ref_str& draw_val)=0;
    virtual void 				__stdcall 	floatRDOnAfterEdit	(PropValue* sender,	float& edit_val, bool& accepted)=0;
    virtual void 				__stdcall 	floatRDOnBeforeEdit	(PropValue* sender,	float& edit_val)=0;
    virtual void 				__stdcall 	floatRDOnDraw		(PropValue* sender, ref_str& draw_val)=0;    
// R-name edit
    virtual void				__stdcall  	NameBeforeEdit		(PropValue* sender, ref_str& edit_val)=0;
    virtual void 				__stdcall  	NameAfterEdit		(PropValue* sender, ref_str& edit_val, bool& accepted)=0;
    virtual void 				__stdcall  	NameDraw			(PropValue* sender, ref_str& draw_val)=0;
// C-name edit
    virtual void 				__stdcall  	CNameAfterEdit		(PropValue* sender, ref_str& edit_val, bool& accepted)=0;
    virtual void 				__stdcall  	CNameDraw			(PropValue* sender, ref_str& draw_val)=0;
public:
    virtual CaptionValue*  		__stdcall	CreateCaption	    (PropItemVec& items, ref_str key, ref_str val)=0;
    virtual CanvasValue*		__stdcall	CreateCanvas	    (PropItemVec& items, ref_str key, ref_str val, int height)=0;
    virtual ButtonValue*		__stdcall	CreateButton	    (PropItemVec& items, ref_str key, ref_str val, u32 flags)=0;
    virtual ChooseValue*		__stdcall	CreateChoose	    (PropItemVec& items, ref_str key, ref_str* val, u32 mode, LPCSTR path=0, void* fill_param=0)=0;
    virtual S8Value* 			__stdcall	CreateS8		    (PropItemVec& items, ref_str key, s8* val, s8 mn=0, s8 mx=100, s8 inc=1)=0;
    virtual S16Value* 			__stdcall	CreateS16		    (PropItemVec& items, ref_str key, s16* val, s16 mn=0, s16 mx=100, s16 inc=1)=0;
    virtual S32Value* 	 		__stdcall	CreateS32		    (PropItemVec& items, ref_str key, s32* val, s32 mn=0, s32 mx=100, s32 inc=1)=0;
    virtual U8Value* 			__stdcall	CreateU8		    (PropItemVec& items, ref_str key, u8* val, u8 mn=0, u8 mx=100, u8 inc=1)=0;
    virtual U16Value* 			__stdcall	CreateU16		    (PropItemVec& items, ref_str key, u16* val, u16 mn=0, u16 mx=100, u16 inc=1)=0;
    virtual U32Value* 	  		__stdcall	CreateU32		    (PropItemVec& items, ref_str key, u32* val, u32 mn=0, u32 mx=100, u32 inc=1)=0;
    virtual FloatValue* 		__stdcall	CreateFloat		    (PropItemVec& items, ref_str key, float* val, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2)=0;
    virtual BOOLValue* 	  		__stdcall	CreateBOOL		    (PropItemVec& items, ref_str key, BOOL* val)=0;
    virtual VectorValue*  	 	__stdcall	CreateVector	    (PropItemVec& items, ref_str key, Fvector* val, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2)=0;
    virtual Flag8Value*			__stdcall	CreateFlag8		    (PropItemVec& items, ref_str key, Flags8* val, u8 mask, LPCSTR c0=0, LPCSTR c1=0, u32 flags=0)=0;
    virtual Flag16Value*		__stdcall	CreateFlag16	    (PropItemVec& items, ref_str key, Flags16* val, u16 mask, LPCSTR c0=0, LPCSTR c1=0, u32 flags=0)=0;
    virtual Flag32Value*		__stdcall	CreateFlag32	    (PropItemVec& items, ref_str key, Flags32* val, u32 mask, LPCSTR c0=0, LPCSTR c1=0, u32 flags=0)=0;
	virtual Token8Value*		__stdcall	CreateToken8	    (PropItemVec& items, ref_str key, u8* val, xr_token* token)=0;
	virtual Token16Value*		__stdcall	CreateToken16	    (PropItemVec& items, ref_str key, u16* val, xr_token* token)=0;
	virtual Token32Value*		__stdcall	CreateToken32	    (PropItemVec& items, ref_str key, u32* val, xr_token* token)=0;
	virtual RToken8Value* 		__stdcall	CreateRToken8	    (PropItemVec& items, ref_str key, u8* val, xr_rtoken* token, u32 t_cnt)=0;
	virtual RToken16Value* 		__stdcall	CreateRToken16	    (PropItemVec& items, ref_str key, u16* val, xr_rtoken* token, u32 t_cnt)=0;
	virtual RToken32Value* 		__stdcall	CreateRToken32	    (PropItemVec& items, ref_str key, u32* val, xr_rtoken* token, u32 t_cnt)=0;
	virtual RListValue* 	 	__stdcall	CreateRList		    (PropItemVec& items, ref_str key, ref_str* val, ref_str* lst, u32 cnt)=0;
    virtual U32Value*  			__stdcall	CreateColor		    (PropItemVec& items, ref_str key, u32* val)=0;
    virtual ColorValue*			__stdcall	CreateFColor	    (PropItemVec& items, ref_str key, Fcolor* val)=0;
    virtual VectorValue*		__stdcall	CreateVColor	    (PropItemVec& items, ref_str key, Fvector* val)=0;
	virtual RTextValue* 		__stdcall	CreateRText		    (PropItemVec& items, ref_str key, ref_str* val)=0;
	virtual WaveValue* 			__stdcall	CreateWave		    (PropItemVec& items, ref_str key, WaveForm* val)=0;
    virtual FloatValue* 		__stdcall	CreateTime		    (PropItemVec& items, ref_str key, float* val, float mn=0.f, float mx=86400.f)=0;

    virtual FloatValue* 		__stdcall	CreateAngle		    (PropItemVec& items, ref_str key, float* val, float mn=0.f, float mx=PI_MUL_2, float inc=0.01f, int decim=2)=0;
    virtual VectorValue* 		__stdcall	CreateAngle3	    (PropItemVec& items, ref_str key, Fvector* val, float mn=0.f, float mx=PI_MUL_2, float inc=0.01f, int decim=2)=0;
    virtual RTextValue* 		__stdcall	CreateName		    (PropItemVec& items, ref_str key, ref_str* val, ListItem* owner)=0;  
	virtual RTextValue* 		__stdcall	CreateNameCB		(PropItemVec& items, ref_str key, ref_str* val, TOnDrawTextEvent=0, RTextValue::TOnBeforeEditEvent=0, RTextValue::TOnAfterEditEvent=0)=0;

	// obsolette    
    virtual CTextValue* 		__stdcall	CreateCText			(PropItemVec& items, ref_str key, LPSTR val, u32 sz)=0;
	virtual CListValue* 	 	__stdcall	CreateCList		    (PropItemVec& items, ref_str key, LPSTR val, u32 sz, ref_str* lst, u32 cnt)=0;
    virtual CTextValue* 		__stdcall	CreateCName		    (PropItemVec& items, ref_str key, LPSTR val, u32 sz, ListItem* owner)=0;
	virtual TokenValueSH*   	__stdcall	CreateTokenSH	    (PropItemVec& items, ref_str key, u32* val, const TokenValueSH::Item* lst, u32 cnt)=0;
    virtual CTextValue* 		__stdcall	CreateTexture		(PropItemVec& items, ref_str key, LPSTR val, u32 sz)=0;
};
//---------------------------------------------------------------------------
#ifdef __BORLANDC__
	extern "C" XR_EPROPS_API IPropHelper& PHelper ();
#else
	extern IPropHelper &PHelper();
#endif
//---------------------------------------------------------------------------

//------------------------------------------------------------------------------
// List
//------------------------------------------------------------------------------
class XR_EPROPS_API IListHelper{
public:
    virtual ListItem* 			__stdcall	FindItem			(ListItemsVec& items,	LPCSTR key)=0;
 	virtual bool 				__stdcall	NameAfterEdit		(ListItem* sender, LPCSTR value, ref_str& edit_val)=0;
public:
	virtual ListItem*			__stdcall	CreateItem			(ListItemsVec& items, LPCSTR key, int type, u32 item_flags=0, void* object=0)=0;
};

#ifdef __BORLANDC__
	extern "C" XR_EPROPS_API IListHelper& LHelper ();
#endif

#endif
 
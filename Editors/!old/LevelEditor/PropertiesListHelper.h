//---------------------------------------------------------------------------
#ifndef PropertiesListHelperH
#define PropertiesListHelperH

// refs
class ListItem;

//---------------------------------------------------------------------------
class CPropHelper:public IPropHelper{
	PropItem*								CreateItem			(PropItemVec& items, const ref_str& key, EPropType type, u32 item_flags=0);
    PropValue*								AppendValue			(PropItemVec& items, const ref_str& key, PropValue* val, EPropType type, u32 item_flags=0);
public:
    virtual PropItem* 			__stdcall	FindItem			(PropItemVec& items, ref_str key, EPropType type=PROP_UNDEF);
public:
//------------------------------------------------------------------------------
// predefind event routines
    virtual void 				__stdcall 	FvectorRDOnAfterEdit(PropValue* sender, Fvector& edit_val, bool& accepted);
    virtual void 				__stdcall 	FvectorRDOnBeforeEdit(PropValue* sender,Fvector& edit_val);
    virtual void 				__stdcall 	FvectorRDOnDraw		(PropValue* sender, ref_str& draw_val);
    virtual void 				__stdcall 	floatRDOnAfterEdit	(PropValue* sender,  float& edit_val, bool& accepted);
    virtual void 				__stdcall 	floatRDOnBeforeEdit	(PropValue* sender,  float& edit_val);
    virtual void 				__stdcall 	floatRDOnDraw		(PropValue* sender,  ref_str& draw_val);
// name edit
    virtual void 				__stdcall  	NameAfterEdit		(PropValue* sender, ref_str& edit_val, bool& accepted);
    virtual void				__stdcall  	NameBeforeEdit		(PropValue* sender, ref_str& edit_val);
    virtual void 				__stdcall  	NameDraw			(PropValue* sender, ref_str& draw_val);
public:
    virtual CaptionValue*  		__stdcall	CreateCaption	    (PropItemVec& items, ref_str key, ref_str val);
    virtual CanvasValue*		__stdcall	CreateCanvas	    (PropItemVec& items, ref_str key, ref_str val, int height);
    virtual ButtonValue*		__stdcall	CreateButton	    (PropItemVec& items, ref_str key, ref_str val, u32 flags);
    virtual ChooseValue*		__stdcall	CreateChoose	    (PropItemVec& items, ref_str key, ref_str* val, u32 mode, LPCSTR path=0, u32 fill_param=0);
    virtual S8Value* 			__stdcall	CreateS8		    (PropItemVec& items, ref_str key, s8* val, s8 mn=0, s8 mx=100, s8 inc=1);
    virtual S16Value* 			__stdcall	CreateS16		    (PropItemVec& items, ref_str key, s16* val, s16 mn=0, s16 mx=100, s16 inc=1);
    virtual S32Value* 	 		__stdcall	CreateS32		    (PropItemVec& items, ref_str key, s32* val, s32 mn=0, s32 mx=100, s32 inc=1);
    virtual U8Value* 			__stdcall	CreateU8		    (PropItemVec& items, ref_str key, u8* val, u8 mn=0, u8 mx=100, u8 inc=1);
    virtual U16Value* 			__stdcall	CreateU16		    (PropItemVec& items, ref_str key, u16* val, u16 mn=0, u16 mx=100, u16 inc=1);
    virtual U32Value* 	  		__stdcall	CreateU32		    (PropItemVec& items, ref_str key, u32* val, u32 mn=0, u32 mx=100, u32 inc=1);
    virtual FloatValue* 		__stdcall	CreateFloat		    (PropItemVec& items, ref_str key, float* val, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2);
    virtual BOOLValue* 	  		__stdcall	CreateBOOL		    (PropItemVec& items, ref_str key, BOOL* val);
    virtual VectorValue*  	 	__stdcall	CreateVector	    (PropItemVec& items, ref_str key, Fvector* val, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2);
    virtual Flag8Value*			__stdcall	CreateFlag8		    (PropItemVec& items, ref_str key, Flags8* val, u8 mask, LPCSTR c0=0, LPCSTR c1=0, u32 flags=0);
    virtual Flag16Value*		__stdcall	CreateFlag16	    (PropItemVec& items, ref_str key, Flags16* val, u16 mask, LPCSTR c0=0, LPCSTR c1=0, u32 flags=0);
    virtual Flag32Value*		__stdcall	CreateFlag32	    (PropItemVec& items, ref_str key, Flags32* val, u32 mask, LPCSTR c0=0, LPCSTR c1=0, u32 flags=0);
	virtual Token8Value*		__stdcall	CreateToken8	    (PropItemVec& items, ref_str key, u8* val,  xr_token* token);
	virtual Token16Value*		__stdcall	CreateToken16	    (PropItemVec& items, ref_str key, u16* val, xr_token* token);
	virtual Token32Value*		__stdcall	CreateToken32	    (PropItemVec& items, ref_str key, u32* val, xr_token* token);
	virtual RToken8Value* 		__stdcall	CreateRToken8	    (PropItemVec& items, ref_str key, u8* val,  xr_rtoken* token, u32 t_cnt);
	virtual RToken16Value* 		__stdcall	CreateRToken16	    (PropItemVec& items, ref_str key, u16* val, xr_rtoken* token, u32 t_cnt);
	virtual RToken32Value* 		__stdcall	CreateRToken32	    (PropItemVec& items, ref_str key, u32* val, xr_rtoken* token, u32 t_cnt);
	virtual TokenValueSH*   	__stdcall	CreateTokenSH	    (PropItemVec& items, ref_str key, u32* val, const TokenValueSH::Item* lst, u32 cnt);
	virtual ListValue* 	 		__stdcall	CreateList		    (PropItemVec& items, ref_str key, ref_str* val, ref_str* lst, u32 cnt);
    virtual U32Value*  			__stdcall	CreateColor		    (PropItemVec& items, ref_str key, u32* val);
    virtual ColorValue*			__stdcall	CreateFColor	    (PropItemVec& items, ref_str key, Fcolor* val);
    virtual VectorValue*		__stdcall	CreateVColor	    (PropItemVec& items, ref_str key, Fvector* val);
	virtual RTextValue* 		__stdcall	CreateRText		    (PropItemVec& items, ref_str key, ref_str* val);
	virtual WaveValue* 			__stdcall	CreateWave		    (PropItemVec& items, ref_str key, WaveForm* val);
    virtual FloatValue* 		__stdcall	CreateTime		    (PropItemVec& items, ref_str key, float* val, float mn=0.f, float mx=86400.f);

    virtual FloatValue* 		__stdcall	CreateAngle		    (PropItemVec& items, ref_str key, float* val, float mn=0.f, float mx=PI_MUL_2, float inc=0.01f, int decim=2);
    virtual VectorValue* 		__stdcall	CreateAngle3	    (PropItemVec& items, ref_str key, Fvector* val, float mn=0.f, float mx=PI_MUL_2, float inc=0.01f, int decim=2);
    virtual RTextValue* 		__stdcall	CreateName		    (PropItemVec& items, ref_str key, ref_str* val, ListItem* owner);
	virtual RTextValue* 		__stdcall	CreateNameCB		(PropItemVec& items, ref_str key, ref_str* val, TOnDrawTextEvent=0, RTextValue::TOnBeforeEditEvent=0, RTextValue::TOnAfterEditEvent=0);
};
//---------------------------------------------------------------------------
#endif

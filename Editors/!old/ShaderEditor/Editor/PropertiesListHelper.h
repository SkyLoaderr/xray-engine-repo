//---------------------------------------------------------------------------
#ifndef PropertiesListHelperH
#define PropertiesListHelperH

// refs
class ListItem;

//---------------------------------------------------------------------------
class XR_EPROPS_API CPropHelper{
	PropItem*		CreateItem		(PropItemVec& items, const AnsiString& key, EPropType type, u32 item_flags=0);
    PropValue*		AppendValue		(PropItemVec& items, const AnsiString& key, PropValue* val, EPropType type, u32 item_flags=0);
public:
    PropItem* 		FindItem		(PropItemVec& items, const AnsiString& key, EPropType type=PROP_UNDEF);
public:
//------------------------------------------------------------------------------
// predefind event routines
    void 				__stdcall 	FvectorRDOnAfterEdit(PropItem* sender, LPVOID edit_val);
    void 				__stdcall 	FvectorRDOnBeforeEdit(PropItem* sender, LPVOID edit_val);
    void 				__stdcall 	FvectorRDOnDraw		(PropValue* sender, LPVOID draw_val);
    void 				__stdcall 	floatRDOnAfterEdit	(PropItem* sender, LPVOID edit_val);
    void 				__stdcall 	floatRDOnBeforeEdit	(PropItem* sender, LPVOID edit_val);
    void 				__stdcall 	floatRDOnDraw		(PropValue* sender, LPVOID draw_val);
// name edit
    void 				__stdcall  	NameAfterEditR_TI	(PropItem* sender, LPVOID edit_val);
    void 				__stdcall  	NameAfterEditR		(PropItem* sender, LPVOID edit_val);
    void 				__stdcall  	NameAfterEdit_TI	(PropItem* sender, LPVOID edit_val);
    void 				__stdcall  	NameAfterEdit		(PropItem* sender, LPVOID edit_val);
    void				__stdcall  	NameBeforeEdit		(PropItem* sender, LPVOID edit_val);
    void 				__stdcall  	NameDraw			(PropValue* sender, LPVOID draw_val);
public:
    CaptionValue*  		__stdcall	CreateCaption	    (PropItemVec& items, AnsiString key, AnsiString val);
    CanvasValue*		__stdcall	CreateCanvas	    (PropItemVec& items, AnsiString key, AnsiString val, int height);
    ButtonValue*		__stdcall	CreateButton	    (PropItemVec& items, AnsiString key, AnsiString val, u32 flags);
    ChooseValue*		__stdcall	CreateChoose	    (PropItemVec& items, AnsiString key, LPSTR val, int len, u32 mode, LPCSTR path=0);
    RChooseValue*		__stdcall	CreateChoose	    (PropItemVec& items, AnsiString key, ref_str* val, u32 mode, LPCSTR path=0);
    S8Value* 			__stdcall	CreateS8		    (PropItemVec& items, AnsiString key, s8* val, s8 mn=0, s8 mx=100, s8 inc=1);
    S16Value* 			__stdcall	CreateS16		    (PropItemVec& items, AnsiString key, s16* val, s16 mn=0, s16 mx=100, s16 inc=1);
    S32Value* 	 		__stdcall	CreateS32		    (PropItemVec& items, AnsiString key, s32* val, s32 mn=0, s32 mx=100, s32 inc=1);
    U8Value* 			__stdcall	CreateU8		    (PropItemVec& items, AnsiString key, u8* val, u8 mn=0, u8 mx=100, u8 inc=1);
    U16Value* 			__stdcall	CreateU16		    (PropItemVec& items, AnsiString key, u16* val, u16 mn=0, u16 mx=100, u16 inc=1);
    U32Value* 	  		__stdcall	CreateU32		    (PropItemVec& items, AnsiString key, u32* val, u32 mn=0, u32 mx=100, u32 inc=1);
    FloatValue* 		__stdcall	CreateFloat		    (PropItemVec& items, AnsiString key, float* val, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2);
    BOOLValue* 	  		__stdcall	CreateBOOL		    (PropItemVec& items, AnsiString key, BOOL* val);
    VectorValue*  	 	__stdcall	CreateVector	    (PropItemVec& items, AnsiString key, Fvector* val, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2);
    Flag8Value*			__stdcall	CreateFlag8		    (PropItemVec& items, AnsiString key, Flags8* val, u8 mask, LPCSTR c0=0, LPCSTR c1=0, u32 flags=0);
    Flag16Value*		__stdcall	CreateFlag16	    (PropItemVec& items, AnsiString key, Flags16* val, u16 mask, LPCSTR c0=0, LPCSTR c1=0, u32 flags=0);
    Flag32Value*		__stdcall	CreateFlag32	    (PropItemVec& items, AnsiString key, Flags32* val, u32 mask, LPCSTR c0=0, LPCSTR c1=0, u32 flags=0);
	Token8Value*		__stdcall	CreateToken8	    (PropItemVec& items, AnsiString key, u8* val, xr_token* token);
	Token16Value*		__stdcall	CreateToken16	    (PropItemVec& items, AnsiString key, u16* val, xr_token* token);
	Token32Value*		__stdcall	CreateToken32	    (PropItemVec& items, AnsiString key, u32* val, xr_token* token);
	RToken8Value* 		__stdcall	CreateRToken8	    (PropItemVec& items, AnsiString key, u8* val, RTokenVec* token);
	RToken16Value* 		__stdcall	CreateRToken16	    (PropItemVec& items, AnsiString key, u16* val, RTokenVec* token);
	RToken32Value* 		__stdcall	CreateRToken32	    (PropItemVec& items, AnsiString key, u32* val, RTokenVec* token);
	TokenValueSH*   	__stdcall	CreateTokenSH	    (PropItemVec& items, AnsiString key, u32* val, u32 cnt, const TokenValueSH::Item* lst);
	ListValue* 	 		__stdcall	CreateList		    (PropItemVec& items, AnsiString key, ref_str* val, RStringVec* lst);
    U32Value*  			__stdcall	CreateColor		    (PropItemVec& items, AnsiString key, u32* val);
    ColorValue*			__stdcall	CreateFColor	    (PropItemVec& items, AnsiString key, Fcolor* val);
    VectorValue*		__stdcall	CreateVColor	    (PropItemVec& items, AnsiString key, Fvector* val);
	TextValue* 	 	  	__stdcall	CreateText		    (PropItemVec& items, AnsiString key, LPSTR val, int lim);
	RTextValue* 		__stdcall	CreateRText		    (PropItemVec& items, AnsiString key, ref_str* val);
	TextValue* 	  		__stdcall	CreateTexture2	    (PropItemVec& items, AnsiString key, LPSTR val, int lim);
	WaveValue* 			__stdcall	CreateWave		    (PropItemVec& items, AnsiString key, WaveForm* val);
    FloatValue* 		__stdcall	CreateTime		    (PropItemVec& items, AnsiString key, float* val, float mn=0.f, float mx=86400.f);
    
    FloatValue* 		__stdcall	CreateAngle		    (PropItemVec& items, AnsiString key, float* val, float mn=0.f, float mx=PI_MUL_2, float inc=0.01f, int decim=2);
    VectorValue* 		__stdcall	CreateAngle3	    (PropItemVec& items, AnsiString key, Fvector* val, float mn=0.f, float mx=PI_MUL_2, float inc=0.01f, int decim=2);
    TextValue* 			__stdcall	CreateName		    (PropItemVec& items, AnsiString key, LPSTR val, int lim, ListItem* owner);  
    RTextValue* 		__stdcall	CreateRName		    (PropItemVec& items, AnsiString key, ref_str* val, ListItem* owner);  
    TextValue* 			__stdcall	CreateName_TI	    (PropItemVec& items, AnsiString key, LPSTR val, int lim, TElTreeItem* owner);  
    RTextValue* 		__stdcall	CreateRName_TI	    (PropItemVec& items, AnsiString key, ref_str* val, TElTreeItem* owner);  
	void 				__stdcall	DrawThumbnail	    (TCanvas *Surface, TRect &R, LPCSTR fname);
};
//---------------------------------------------------------------------------
extern XR_EPROPS_API CPropHelper PHelper;
//---------------------------------------------------------------------------
#endif

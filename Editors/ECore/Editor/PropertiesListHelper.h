//---------------------------------------------------------------------------
#ifndef PropertiesListHelperH
#define PropertiesListHelperH

#include "PropertiesListTypes.h"
#include "FolderLib.h"                 

//---------------------------------------------------------------------------
class CPropHelper{
	IC PropItem*		CreateItem		(PropItemVec& items, const AnsiString& key, EPropType type, u32 item_flags=0)
    {
    	PropItem* item	= FindItem(items,key,type);
        if (!item){
        	item		= xr_new<PropItem>	(type);
            item->SetName(key);
	        item->m_Flags.set(item_flags,TRUE);
            items.push_back(item);
        }
        return			item;
    }
    IC PropValue*		AppendValue		(PropItemVec& items, const AnsiString& key, PropValue* val, EPropType type, u32 item_flags=0)
    {
    	PropItem* item	= CreateItem(items,key,type,item_flags);
        val->m_Owner	= item;
        item->AppendValue(val);
        return val;
    }
public:
    IC PropItem* 		FindItem		(PropItemVec& items, const AnsiString& key, EPropType type)
    {
    	for (PropItemIt it=items.begin(); it!=items.end(); it++)
        	if (((*it)->type==type)&&((*it)->key==key)) return *it;
        return 0;
    }
public:

//------------------------------------------------------------------------------
// predefind event routines
    IC void __fastcall 	FvectorRDOnAfterEdit(PropItem* sender, LPVOID edit_val)
    {
        Fvector* V = (Fvector*)edit_val;
        V->x = deg2rad(V->x);
        V->y = deg2rad(V->y);
        V->z = deg2rad(V->z);
        VectorValue* P = dynamic_cast<VectorValue*>(sender->GetFrontValue()); R_ASSERT(P);
        P->lim_mn.x = deg2rad(P->lim_mn.x);  P->lim_mx.x = deg2rad(P->lim_mx.x);
        P->lim_mn.y = deg2rad(P->lim_mn.y);  P->lim_mx.y = deg2rad(P->lim_mx.y);
        P->lim_mn.z = deg2rad(P->lim_mn.z);  P->lim_mx.z = deg2rad(P->lim_mx.z);
    }

    IC void __fastcall 	FvectorRDOnBeforeEdit(PropItem* sender, LPVOID edit_val)
    {
        Fvector* V = (Fvector*)edit_val;
        V->x = rad2deg(V->x);
        V->y = rad2deg(V->y);
        V->z = rad2deg(V->z);
        VectorValue* P = dynamic_cast<VectorValue*>(sender->GetFrontValue()); R_ASSERT(P);
        P->lim_mn.x = rad2deg(P->lim_mn.x);  P->lim_mx.x = rad2deg(P->lim_mx.x);
        P->lim_mn.y = rad2deg(P->lim_mn.y);  P->lim_mx.y = rad2deg(P->lim_mx.y);
        P->lim_mn.z = rad2deg(P->lim_mn.z);  P->lim_mx.z = rad2deg(P->lim_mx.z);
    }

    IC void __fastcall 	FvectorRDOnDraw(PropValue* sender, LPVOID draw_val)
    {
        Fvector* V = (Fvector*)draw_val;
        V->x = rad2deg(V->x);
        V->y = rad2deg(V->y);
        V->z = rad2deg(V->z);
    }
    IC void __fastcall 	floatRDOnAfterEdit(PropItem* sender, LPVOID edit_val)
    {
        *(float*)edit_val = deg2rad(*(float*)edit_val);
        FloatValue* P 	= dynamic_cast<FloatValue*>(sender->GetFrontValue()); R_ASSERT(P);
        P->lim_mn = deg2rad(P->lim_mn); P->lim_mx = deg2rad(P->lim_mx);
    }

    IC void __fastcall 	floatRDOnBeforeEdit(PropItem* sender, LPVOID edit_val)
    {
        *(float*)edit_val = rad2deg(*(float*)edit_val);
        FloatValue* P 	= dynamic_cast<FloatValue*>(sender->GetFrontValue()); R_ASSERT(P);
        P->lim_mn = rad2deg(P->lim_mn); P->lim_mx = rad2deg(P->lim_mx);
    }

    IC void __fastcall 	floatRDOnDraw(PropValue* sender, LPVOID draw_val)
    {
        *(float*)draw_val = rad2deg(*(float*)draw_val);
    }

    // name
    void __fastcall		NameAfterEditR_TI(PropItem* sender, LPVOID edit_val);
    void __fastcall		NameAfterEditR	(PropItem* sender, LPVOID edit_val);
    void __fastcall		NameAfterEdit_TI(PropItem* sender, LPVOID edit_val);
    void __fastcall		NameAfterEdit	(PropItem* sender, LPVOID edit_val);
    void __fastcall		NameBeforeEdit	(PropItem* sender, LPVOID edit_val);
    void __fastcall		NameDraw		(PropValue* sender, LPVOID draw_val);
public:
    IC CaptionValue*	CreateCaption	(PropItemVec& items, AnsiString key, AnsiString val)
    {	return			(CaptionValue*)	AppendValue		(items,key,xr_new<CaptionValue>(val),PROP_CAPTION);			}
    IC CanvasValue*		CreateCanvas	(PropItemVec& items, AnsiString key, AnsiString val, int height)
    {	return			(CanvasValue*)	AppendValue		(items,key,xr_new<CanvasValue>(val,height),PROP_CANVAS);  	}
    IC ButtonValue*		CreateButton	(PropItemVec& items, AnsiString key, AnsiString val, u32 flags)
    {	return			(ButtonValue*)	AppendValue		(items,key,xr_new<ButtonValue>(val,flags),PROP_BUTTON);		}
    IC ChooseValue*		CreateChoose	(PropItemVec& items, AnsiString key, LPSTR val, int len, EChooseMode mode)
    {	return			(ChooseValue*)	AppendValue		(items,key,xr_new<ChooseValue>(val,len,mode),PROP_CHOOSE);	}
    IC AChooseValue*	CreateChoose	(PropItemVec& items, AnsiString key, AnsiString* val, EChooseMode mode)
    {	return			(AChooseValue*)	AppendValue		(items,key,xr_new<AChooseValue>(val,mode),PROP_CHOOSE);	}
    IC RChooseValue*	CreateChoose	(PropItemVec& items, AnsiString key, ref_str* val, EChooseMode mode)
    {	return			(RChooseValue*)	AppendValue		(items,key,xr_new<RChooseValue>(val,mode),PROP_CHOOSE);	}
    IC S8Value* 		CreateS8		(PropItemVec& items, AnsiString key, s8* val, s8 mn=0, s8 mx=100, s8 inc=1)
    {	return			(S8Value*)		AppendValue		(items,key,xr_new<S8Value>(val,mn,mx,inc,0),PROP_S8);	}
    IC S16Value* 		CreateS16		(PropItemVec& items, AnsiString key, s16* val, s16 mn=0, s16 mx=100, s16 inc=1)
    {	return			(S16Value*)		AppendValue		(items,key,xr_new<S16Value>(val,mn,mx,inc,0),PROP_S16); }
    IC S32Value* 	 	CreateS32		(PropItemVec& items, AnsiString key, s32* val, s32 mn=0, s32 mx=100, s32 inc=1)
    {   return			(S32Value*)		AppendValue		(items,key,xr_new<S32Value>(val,mn,mx,inc,0),PROP_S32); }
    IC U8Value* 		CreateU8		(PropItemVec& items, AnsiString key, u8* val, u8 mn=0, u8 mx=100, u8 inc=1)
    {   return			(U8Value*)		AppendValue		(items,key,xr_new<U8Value>(val,mn,mx,inc,0),PROP_U8);   }
    IC U16Value* 		CreateU16		(PropItemVec& items, AnsiString key, u16* val, u16 mn=0, u16 mx=100, u16 inc=1)
    {   return			(U16Value*)		AppendValue		(items,key,xr_new<U16Value>(val,mn,mx,inc,0),PROP_U16); }
    IC U32Value* 	  	CreateU32		(PropItemVec& items, AnsiString key, u32* val, u32 mn=0, u32 mx=100, u32 inc=1)
    {	return			(U32Value*)		AppendValue		(items,key,xr_new<U32Value>(val,mn,mx,inc,0),PROP_U32);	}
    IC FloatValue* 		CreateFloat		(PropItemVec& items, AnsiString key, float* val, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2)
    {   return			(FloatValue*)	AppendValue		(items,key,xr_new<FloatValue>(val,mn,mx,inc,decim),PROP_FLOAT);}
    IC BOOLValue* 	  	CreateBOOL		(PropItemVec& items, AnsiString key, BOOL* val)
    {   return			(BOOLValue*)	AppendValue		(items,key,xr_new<BOOLValue>(val),PROP_BOOLEAN);        }
    IC VectorValue*   	CreateVector	(PropItemVec& items, AnsiString key, Fvector* val, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2)
    {   return			(VectorValue*)	AppendValue		(items,key,xr_new<VectorValue>(val,mn,mx,inc,decim),PROP_VECTOR);}
    template <class T>
    IC FlagValue<T>*	CreateFlag		(PropItemVec& items, AnsiString key, T* val, T::TYPE mask)
    {   return			(FlagValue<T>*)	AppendValue		(items,key,xr_new<FlagValue<T> >(val,mask),PROP_FLAG);    }
    template <class T>
	IC TokenValue<T>*	CreateToken		(PropItemVec& items, AnsiString key, T* val, xr_token* token)
    {   return			(TokenValue<T>*)AppendValue		(items,key,xr_new<TokenValue<T> >(val,token),PROP_TOKEN);}
    template <class T>
	IC ATokenValue<T>* 	CreateAToken	(PropItemVec& items, AnsiString key, T* val, ATokenVec* token)
    {   return			(ATokenValue<T>*)AppendValue	(items,key,xr_new<ATokenValue<T> >(val,token),PROP_A_TOKEN);}
    template <class T>
	IC TokenValue2<T>* 	CreateToken2	(PropItemVec& items, AnsiString key, T* val, AStringVec* lst)
    {   return			(TokenValue2<T>*)AppendValue	(items,key,xr_new<TokenValue2<T> >(val,lst),PROP_TOKEN2);	}
    template <class T>
	IC TokenValue3<T>* 	CreateToken3	(PropItemVec& items, AnsiString key, T* val, const TokenValue3<T>::ItemVec* lst)
    {   return			(TokenValue3<T>*)AppendValue	(items,key,xr_new<TokenValue3<T> >(val,lst),PROP_TOKEN3);}
	IC TokenValueSH*   	CreateTokenSH	(PropItemVec& items, AnsiString key, u32* val, u32 cnt, const TokenValueSH::Item* lst)
    {   return			(TokenValueSH*)	AppendValue		(items,key,xr_new<TokenValueSH>(val,cnt,lst),PROP_SH_TOKEN);}
	IC ListValue* 	 	CreateList		(PropItemVec& items, AnsiString key, LPSTR val, int lim, AStringVec* lst)
    {   return			(ListValue*)	AppendValue		(items,key,xr_new<ListValue>(val,lim,lst),PROP_LIST);       }
	IC ListValue* 	 	CreateListA		(PropItemVec& items, AnsiString key, LPSTR val, int lim, u32 cnt, LPCSTR* lst)
    {   return			(ListValue*)	AppendValue		(items,key,xr_new<ListValue>(val,lim,cnt,lst),PROP_LIST);	}
    IC U32Value*  		CreateColor		(PropItemVec& items, AnsiString key, u32* val)
    {   return			(U32Value*)		AppendValue		(items,key,xr_new<U32Value>(val,0x00000000,0xffffffff,1,0),PROP_COLOR);}
    IC ColorValue*		CreateFColor	(PropItemVec& items, AnsiString key, Fcolor* val)
    {   return			(ColorValue*)	AppendValue		(items,key,xr_new<ColorValue>(val),PROP_FCOLOR);        }
	IC TextValue* 	   	CreateText		(PropItemVec& items, AnsiString key, LPSTR val, int lim)
    {   return			(TextValue*)	AppendValue		(items,key,xr_new<TextValue>(val,lim),PROP_TEXT);       }
	IC ATextValue* 		CreateAText		(PropItemVec& items, AnsiString key, AnsiString* val)
    {   return			(ATextValue*)	AppendValue		(items,key,xr_new<ATextValue>(val),PROP_A_TEXT);        }
	IC RTextValue* 		CreateRText		(PropItemVec& items, AnsiString key, ref_str* val)
    {   return			(RTextValue*)	AppendValue		(items,key,xr_new<RTextValue>(val),PROP_R_TEXT);        }
	IC TextValue* 	  	CreateTexture2	(PropItemVec& items, AnsiString key, LPSTR val, int lim)
    {	return			(TextValue*)	AppendValue		(items,key,xr_new<TextValue>(val,lim),PROP_TEXTURE2,PropItem::flDrawThumbnail);}
	IC WaveValue* 		CreateWave		(PropItemVec& items, AnsiString key, WaveForm* val)
    {	return			(WaveValue*)	AppendValue		(items,key,xr_new<WaveValue>(val),PROP_WAVE);           }
    IC FloatValue* 		CreateTime		(PropItemVec& items, AnsiString key, float* val, float mn=0.f, float mx=86400.f)
    {	return			(FloatValue*)	AppendValue		(items,key,xr_new<FloatValue>(val,mn,mx,0,0),PROP_TIME);    }
    
    IC FloatValue* 		CreateAngle		(PropItemVec& items, AnsiString key, float* val, float mn=0.f, float mx=PI_MUL_2, float inc=0.01f, int decim=2)
    {   FloatValue* V	= (FloatValue*)	AppendValue		(items,key,xr_new<FloatValue>(val,mn,mx,inc,decim),PROP_FLOAT);
    	V->Owner()->OnAfterEditEvent	= floatRDOnAfterEdit;
        V->Owner()->OnBeforeEditEvent	= floatRDOnBeforeEdit; 
	    V->Owner()->OnDrawTextEvent 	= floatRDOnDraw;
        return V;						
    }
    IC VectorValue* 	CreateAngle3	(PropItemVec& items, AnsiString key, Fvector* val, float mn=0.f, float mx=PI_MUL_2, float inc=0.01f, int decim=2)
    {   VectorValue* V	= (VectorValue*)	AppendValue		(items,key,xr_new<VectorValue>(val,mn,mx,inc,decim),PROP_VECTOR);
    	V->Owner()->OnAfterEditEvent	= FvectorRDOnAfterEdit;
        V->Owner()->OnBeforeEditEvent	= FvectorRDOnBeforeEdit;
	    V->Owner()->OnDrawTextEvent		= FvectorRDOnDraw;
        return V;					
    }
    IC TextValue* 		CreateName		(PropItemVec& items, AnsiString key, LPSTR val, int lim, ListItem* owner)  
    {   TextValue* V	= (TextValue*) 	CreateText	(items,key,val,lim);
        V->Owner()->OnAfterEditEvent   	= NameAfterEdit;
        V->Owner()->OnBeforeEditEvent  	= NameBeforeEdit;
        V->Owner()->OnDrawTextEvent 	= NameDraw;
        V->Owner()->tag					= (int)owner;
	    if (V->Owner()->m_Flags.is(PropItem::flMixed)) V->Owner()->m_Flags.set(PropItem::flDisabled,TRUE);
        return V;					
    }
    IC RTextValue* 		CreateRName		(PropItemVec& items, AnsiString key, ref_str* val, ListItem* owner)  
    {   RTextValue* V	= (RTextValue*) CreateRText	(items,key,val);
        V->Owner()->OnAfterEditEvent   	= NameAfterEditR;
        V->Owner()->OnBeforeEditEvent  	= NameBeforeEdit;
        V->Owner()->OnDrawTextEvent 	= NameDraw;
        V->Owner()->tag					= (int)owner; VERIFY(owner);
	    if (V->Owner()->m_Flags.is(PropItem::flMixed)) V->Owner()->m_Flags.set(PropItem::flDisabled,TRUE);
        return V;					
    }
    IC TextValue* 		CreateName_TI	(PropItemVec& items, AnsiString key, LPSTR val, int lim, TElTreeItem* owner)  
    {   TextValue* V	= (TextValue*) 	CreateText	(items,key,val,lim);
        V->Owner()->OnAfterEditEvent   	= NameAfterEdit_TI;
        V->Owner()->OnBeforeEditEvent  	= NameBeforeEdit;
        V->Owner()->OnDrawTextEvent 	= NameDraw;
        V->Owner()->tag					= (int)owner; VERIFY(owner);
	    if (V->Owner()->m_Flags.is(PropItem::flMixed)) V->Owner()->m_Flags.set(PropItem::flDisabled,TRUE);
        return V;					
    }
    IC RTextValue* 		CreateRName_TI	(PropItemVec& items, AnsiString key, ref_str* val, TElTreeItem* owner)  
    {   RTextValue* V	= (RTextValue*) CreateRText	(items,key,val);
        V->Owner()->OnAfterEditEvent   	= NameAfterEditR_TI;
        V->Owner()->OnBeforeEditEvent  	= NameBeforeEdit;
        V->Owner()->OnDrawTextEvent 	= NameDraw;
        V->Owner()->tag					= (int)owner; VERIFY(owner);
	    if (V->Owner()->m_Flags.is(PropItem::flMixed)) V->Owner()->m_Flags.set(PropItem::flDisabled,TRUE);
        return V;					
    }
	void 				DrawThumbnail	(TCanvas *Surface, TRect &R, LPCSTR fname);
};
//---------------------------------------------------------------------------
extern CPropHelper PHelper;
//---------------------------------------------------------------------------
#endif

#include "stdafx.h"
#pragma hdrstop

#include "PropertiesListHelper.h"
#include "ItemListHelper.h"
#include "ChoseForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
              
CPropHelper PHelper;
//---------------------------------------------------------------------------
PropItem* CPropHelper::CreateItem(PropItemVec& items, const AnsiString& key, EPropType type, u32 item_flags)
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
PropValue* CPropHelper::AppendValue(PropItemVec& items, const AnsiString& key, PropValue* val, EPropType type, u32 item_flags)
{
    PropItem* item	= CreateItem(items,key,type,item_flags);
    val->m_Owner	= item;
    item->AppendValue(val);
    return val;
}
PropItem* CPropHelper::FindItem(PropItemVec& items, const AnsiString& key, EPropType type)
{
    if (type!=PROP_UNDEF){
        for (PropItemIt it=items.begin(); it!=items.end(); it++)
            if (((*it)->type==type)&&((*it)->key==key)) return *it;
    }else{
        for (PropItemIt it=items.begin(); it!=items.end(); it++)
            if ((*it)->key==key) return *it;
    }
    return 0;
}
    IC void __fastcall 	CPropHelper::FvectorRDOnAfterEdit(PropItem* sender, LPVOID edit_val)
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

    IC void __fastcall 	CPropHelper::FvectorRDOnBeforeEdit(PropItem* sender, LPVOID edit_val)
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

    IC void __fastcall 	CPropHelper::FvectorRDOnDraw(PropValue* sender, LPVOID draw_val)
    {
        Fvector* V = (Fvector*)draw_val;
        V->x = rad2deg(V->x);
        V->y = rad2deg(V->y);
        V->z = rad2deg(V->z);
    }
    IC void __fastcall 	CPropHelper::floatRDOnAfterEdit(PropItem* sender, LPVOID edit_val)
    {
        *(float*)edit_val = deg2rad(*(float*)edit_val);
        FloatValue* P 	= dynamic_cast<FloatValue*>(sender->GetFrontValue()); R_ASSERT(P);
        P->lim_mn = deg2rad(P->lim_mn); P->lim_mx = deg2rad(P->lim_mx);
    }

    IC void __fastcall 	CPropHelper::floatRDOnBeforeEdit(PropItem* sender, LPVOID edit_val)
    {
        *(float*)edit_val = rad2deg(*(float*)edit_val);
        FloatValue* P 	= dynamic_cast<FloatValue*>(sender->GetFrontValue()); R_ASSERT(P);
        P->lim_mn = rad2deg(P->lim_mn); P->lim_mx = rad2deg(P->lim_mx);
    }

    IC void __fastcall 	CPropHelper::floatRDOnDraw(PropValue* sender, LPVOID draw_val)
    {
        *(float*)draw_val = rad2deg(*(float*)draw_val);
    }
//---------------------------------------------------------------------------

CaptionValue*  	CPropHelper::CreateCaption	(PropItemVec& items, AnsiString key, AnsiString val)
{	return	   	(CaptionValue*)	AppendValue		(items,key,xr_new<CaptionValue>(val),PROP_CAPTION);			}
CanvasValue*   	CPropHelper::CreateCanvas	(PropItemVec& items, AnsiString key, AnsiString val, int height)
{	return	   	(CanvasValue*)	AppendValue		(items,key,xr_new<CanvasValue>(val,height),PROP_CANVAS);  	}
ButtonValue*   	CPropHelper::CreateButton	(PropItemVec& items, AnsiString key, AnsiString val, u32 flags)
{	return	   	(ButtonValue*)	AppendValue		(items,key,xr_new<ButtonValue>(val,flags),PROP_BUTTON);		}
ChooseValue*   	CPropHelper::CreateChoose	(PropItemVec& items, AnsiString key, LPSTR val, int len, u32 mode, LPCSTR path)
{	return	   	(ChooseValue*)	AppendValue		(items,key,xr_new<ChooseValue>(val,len,mode,path),PROP_CHOOSE,((mode==smTexture)||(mode==smObject))?PropItem::flDrawThumbnail:0);	}	//.
RChooseValue*	CPropHelper::CreateChoose	(PropItemVec& items, AnsiString key, ref_str* val, u32 mode, LPCSTR path)
{	return	   	(RChooseValue*)	AppendValue		(items,key,xr_new<RChooseValue>(val,mode,path),PROP_CHOOSE,((mode==smTexture)||(mode==smObject))?PropItem::flDrawThumbnail:0);	}       //.
S8Value* 		CPropHelper::CreateS8		(PropItemVec& items, AnsiString key, s8* val, s8 mn, s8 mx, s8 inc)
{	return	   	(S8Value*)		AppendValue		(items,key,xr_new<S8Value>(val,mn,mx,inc,0),PROP_S8);	}
S16Value* 		CPropHelper::CreateS16		(PropItemVec& items, AnsiString key, s16* val, s16 mn, s16 mx, s16 inc)
{	return	   	(S16Value*)		AppendValue		(items,key,xr_new<S16Value>(val,mn,mx,inc,0),PROP_S16); }
S32Value* 	 	CPropHelper::CreateS32		(PropItemVec& items, AnsiString key, s32* val, s32 mn, s32 mx, s32 inc)
{   return		(S32Value*)		AppendValue		(items,key,xr_new<S32Value>(val,mn,mx,inc,0),PROP_S32); }
U8Value* 		CPropHelper::CreateU8		(PropItemVec& items, AnsiString key, u8* val, u8 mn, u8 mx, u8 inc)
{   return		(U8Value*)		AppendValue		(items,key,xr_new<U8Value>(val,mn,mx,inc,0),PROP_U8);   }
U16Value* 		CPropHelper::CreateU16		(PropItemVec& items, AnsiString key, u16* val, u16 mn, u16 mx, u16 inc)
{   return		(U16Value*)		AppendValue		(items,key,xr_new<U16Value>(val,mn,mx,inc,0),PROP_U16); }
U32Value* 	  	CPropHelper::CreateU32		(PropItemVec& items, AnsiString key, u32* val, u32 mn, u32 mx, u32 inc)
{	return		(U32Value*)		AppendValue		(items,key,xr_new<U32Value>(val,mn,mx,inc,0),PROP_U32);	}
FloatValue* 	CPropHelper::CreateFloat		(PropItemVec& items, AnsiString key, float* val, float mn, float mx, float inc, int decim)
{   return		(FloatValue*)	AppendValue		(items,key,xr_new<FloatValue>(val,mn,mx,inc,decim),PROP_FLOAT);}
BOOLValue* 	  	CPropHelper::CreateBOOL		(PropItemVec& items, AnsiString key, BOOL* val)
{   return		(BOOLValue*)	AppendValue		(items,key,xr_new<BOOLValue>(val),PROP_BOOLEAN);        }
VectorValue*   	CPropHelper::CreateVector	(PropItemVec& items, AnsiString key, Fvector* val, float mn, float mx, float inc, int decim)
{   return		(VectorValue*)	AppendValue		(items,key,xr_new<VectorValue>(val,mn,mx,inc,decim),PROP_VECTOR);}
Flag8Value*		CPropHelper::CreateFlag8		(PropItemVec& items, AnsiString key, Flags8* val, u8 mask, LPCSTR c0, LPCSTR c1, u32 flags)
{   return		(Flag8Value*)	AppendValue		(items,key,xr_new<Flag8Value>(val,mask,c0,c1,flags),PROP_FLAG);    }
Flag16Value*	CPropHelper::CreateFlag16	(PropItemVec& items, AnsiString key, Flags16* val, u16 mask, LPCSTR c0, LPCSTR c1, u32 flags)
{   return		(Flag16Value*)	AppendValue		(items,key,xr_new<Flag16Value>(val,mask,c0,c1,flags),PROP_FLAG);    }
Flag32Value*	CPropHelper::CreateFlag32	(PropItemVec& items, AnsiString key, Flags32* val, u32 mask, LPCSTR c0, LPCSTR c1, u32 flags)
{   return		(Flag32Value*)	AppendValue		(items,key,xr_new<Flag32Value>(val,mask,c0,c1,flags),PROP_FLAG);    }
Token8Value*	CPropHelper::CreateToken8	(PropItemVec& items, AnsiString key, u8* val, xr_token* token)
{   return		(Token8Value*)	AppendValue		(items,key,xr_new<Token8Value> (val,token),PROP_TOKEN);}
Token16Value*	CPropHelper::CreateToken16	(PropItemVec& items, AnsiString key, u16* val, xr_token* token)
{   return		(Token16Value*)	AppendValue		(items,key,xr_new<Token16Value>(val,token),PROP_TOKEN);}
Token32Value*	CPropHelper::CreateToken32	(PropItemVec& items, AnsiString key, u32* val, xr_token* token)
{   return		(Token32Value*)	AppendValue		(items,key,xr_new<Token32Value>(val,token),PROP_TOKEN);}
RToken8Value* 	CPropHelper::CreateRToken8	(PropItemVec& items, AnsiString key, u8* val, RTokenVec* token)
{   return		(RToken8Value*)	AppendValue	(items,key,xr_new<RToken8Value>(val,token),PROP_RTOKEN);}
RToken16Value* 	CPropHelper::CreateRToken16	(PropItemVec& items, AnsiString key, u16* val, RTokenVec* token)
{   return		(RToken16Value*)AppendValue	(items,key,xr_new<RToken16Value>(val,token),PROP_RTOKEN);}
RToken32Value* 	CPropHelper::CreateRToken32	(PropItemVec& items, AnsiString key, u32* val, RTokenVec* token)
{   return		(RToken32Value*)AppendValue	(items,key,xr_new<RToken32Value>(val,token),PROP_RTOKEN);}
TokenValueSH*   CPropHelper::CreateTokenSH	(PropItemVec& items, AnsiString key, u32* val, u32 cnt, const TokenValueSH::Item* lst)
{   return		(TokenValueSH*)	AppendValue		(items,key,xr_new<TokenValueSH>(val,cnt,lst),PROP_SH_TOKEN);}
ListValue* 	 	CPropHelper::CreateList		(PropItemVec& items, AnsiString key, ref_str* val, RStringVec* lst)
{   return		(ListValue*)	AppendValue		(items,key,xr_new<ListValue>(val,lst),PROP_LIST);       }
U32Value*  		CPropHelper::CreateColor		(PropItemVec& items, AnsiString key, u32* val)
{   return		(U32Value*)		AppendValue		(items,key,xr_new<U32Value>(val,0x00000000,0xffffffff,1,0),PROP_COLOR);}
ColorValue*		CPropHelper::CreateFColor	(PropItemVec& items, AnsiString key, Fcolor* val)
{   return		(ColorValue*)	AppendValue		(items,key,xr_new<ColorValue>(val),PROP_FCOLOR);        }
VectorValue*	CPropHelper::CreateVColor	(PropItemVec& items, AnsiString key, Fvector* val)
{   return		(VectorValue*)	AppendValue		(items,key,xr_new<VectorValue>(val,0.f,1.f,0,0),PROP_VCOLOR);        }
TextValue* 	   	CPropHelper::CreateText		(PropItemVec& items, AnsiString key, LPSTR val, int lim)
{   return		(TextValue*)	AppendValue		(items,key,xr_new<TextValue>(val,lim),PROP_TEXT);       }
RTextValue* 	CPropHelper::CreateRText		(PropItemVec& items, AnsiString key, ref_str* val)
{   return		(RTextValue*)	AppendValue		(items,key,xr_new<RTextValue>(val),PROP_R_TEXT);        }
TextValue* 	  	CPropHelper::CreateTexture2	(PropItemVec& items, AnsiString key, LPSTR val, int lim)
{	return		(TextValue*)	AppendValue		(items,key,xr_new<TextValue>(val,lim),PROP_TEXTURE2,PropItem::flDrawThumbnail);}
WaveValue* 		CPropHelper::CreateWave		(PropItemVec& items, AnsiString key, WaveForm* val)
{	return		(WaveValue*)	AppendValue		(items,key,xr_new<WaveValue>(val),PROP_WAVE);           }
FloatValue* 	CPropHelper::CreateTime		(PropItemVec& items, AnsiString key, float* val, float mn, float mx)
{	return		(FloatValue*)	AppendValue		(items,key,xr_new<FloatValue>(val,mn,mx,0,0),PROP_TIME);    }
    
FloatValue* 	CPropHelper::CreateAngle		(PropItemVec& items, AnsiString key, float* val, float mn, float mx, float inc, int decim)
{   FloatValue* V	= (FloatValue*)	AppendValue		(items,key,xr_new<FloatValue>(val,mn,mx,inc,decim),PROP_FLOAT);
    V->Owner()->OnAfterEditEvent	= floatRDOnAfterEdit;
    V->Owner()->OnBeforeEditEvent	= floatRDOnBeforeEdit; 
    V->Owner()->OnDrawTextEvent 	= floatRDOnDraw;
    return V;						
}
VectorValue* 	CPropHelper::CreateAngle3	(PropItemVec& items, AnsiString key, Fvector* val, float mn, float mx, float inc, int decim)
{   VectorValue* V	= (VectorValue*)	AppendValue		(items,key,xr_new<VectorValue>(val,mn,mx,inc,decim),PROP_VECTOR);
    V->Owner()->OnAfterEditEvent	= FvectorRDOnAfterEdit;
    V->Owner()->OnBeforeEditEvent	= FvectorRDOnBeforeEdit;
    V->Owner()->OnDrawTextEvent		= FvectorRDOnDraw;
    return V;					
}
TextValue* 		CPropHelper::CreateName		(PropItemVec& items, AnsiString key, LPSTR val, int lim, ListItem* owner)  
{   TextValue* V	= (TextValue*) CPropHelper::CreateText	(items,key,val,lim);
    V->Owner()->OnAfterEditEvent   	= NameAfterEdit;
    V->Owner()->OnBeforeEditEvent  	= NameBeforeEdit;
    V->Owner()->OnDrawTextEvent 	= NameDraw;
    V->Owner()->tag					= (int)owner;
    if (V->Owner()->m_Flags.is(PropItem::flMixed)) V->Owner()->m_Flags.set(PropItem::flDisabled,TRUE);
    return V;					
}
RTextValue* 	CPropHelper::CreateRName		(PropItemVec& items, AnsiString key, ref_str* val, ListItem* owner)  
{   RTextValue* V	= (RTextValue*) CreateRText	(items,key,val);
    V->Owner()->OnAfterEditEvent   	= NameAfterEditR;
    V->Owner()->OnBeforeEditEvent  	= NameBeforeEdit;
    V->Owner()->OnDrawTextEvent 	= NameDraw;
    V->Owner()->tag					= (int)owner; VERIFY(owner);
    if (V->Owner()->m_Flags.is(PropItem::flMixed)) V->Owner()->m_Flags.set(PropItem::flDisabled,TRUE);
    return V;					
}
TextValue* 		CPropHelper::CreateName_TI	(PropItemVec& items, AnsiString key, LPSTR val, int lim, TElTreeItem* owner)  
{   TextValue* V	= (TextValue*) CPropHelper::CreateText	(items,key,val,lim);
    V->Owner()->OnAfterEditEvent   	= NameAfterEdit_TI;
    V->Owner()->OnBeforeEditEvent  	= NameBeforeEdit;
    V->Owner()->OnDrawTextEvent 	= NameDraw;
    V->Owner()->tag					= (int)owner; VERIFY(owner);
    if (V->Owner()->m_Flags.is(PropItem::flMixed)) V->Owner()->m_Flags.set(PropItem::flDisabled,TRUE);
    return V;					
}
RTextValue* 	CPropHelper::CreateRName_TI	(PropItemVec& items, AnsiString key, ref_str* val, TElTreeItem* owner)  
{   RTextValue* V	= (RTextValue*) CreateRText	(items,key,val);
    V->Owner()->OnAfterEditEvent   	= NameAfterEditR_TI;
    V->Owner()->OnBeforeEditEvent  	= NameBeforeEdit;
    V->Owner()->OnDrawTextEvent 	= NameDraw;
    V->Owner()->tag					= (int)owner; VERIFY(owner);
    if (V->Owner()->m_Flags.is(PropItem::flMixed)) V->Owner()->m_Flags.set(PropItem::flDisabled,TRUE);
    return V;					
}


void CPropHelper::NameAfterEdit_TI(PropItem* sender, LPVOID edit_val)
{
	FHelper.NameAfterEdit((TElTreeItem*)sender->tag,((TextValue*)sender->GetFrontValue())->GetValue(),*(AnsiString*)edit_val);
}
//---------------------------------------------------------------------------
void CPropHelper::NameAfterEdit(PropItem* sender, LPVOID edit_val)
{
	LHelper.NameAfterEdit((ListItem*)sender->tag,((TextValue*)sender->GetFrontValue())->GetValue(),*(AnsiString*)edit_val);
}
//---------------------------------------------------------------------------

void CPropHelper::NameAfterEditR_TI(PropItem* sender, LPVOID edit_val)
{
	FHelper.NameAfterEdit((TElTreeItem*)sender->tag,*((RTextValue*)sender->GetFrontValue())->GetValue(),*(AnsiString*)edit_val);
}
//---------------------------------------------------------------------------
void CPropHelper::NameAfterEditR(PropItem* sender, LPVOID edit_val)
{
	LHelper.NameAfterEdit((ListItem*)sender->tag,*((RTextValue*)sender->GetFrontValue())->GetValue(),*(AnsiString*)edit_val);
}
//---------------------------------------------------------------------------

void CPropHelper::NameBeforeEdit(PropItem* sender, LPVOID edit_val)
{
	AnsiString& N = *(AnsiString*)edit_val;
	int cnt=_GetItemCount(N.c_str(),'\\');
	N = _SetPos(N.c_str(),cnt-1,'\\');
}
//------------------------------------------------------------------------------
void CPropHelper::NameDraw(PropValue* sender, LPVOID draw_val)
{
	AnsiString& N = *(AnsiString*)draw_val;
	int cnt=_GetItemCount(N.c_str(),'\\');
	N = _SetPos(N.c_str(),cnt-1,'\\');
}
//------------------------------------------------------------------------------




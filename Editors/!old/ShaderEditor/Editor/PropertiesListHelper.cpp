#include "stdafx.h"
#pragma hdrstop

#include "PropertiesListHelper.h"
#include "ItemListHelper.h"
#include "ChoseForm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
              
//---------------------------------------------------------------------------
CPropHelper 	PHelper_impl;
IPropHelper& 	PHelper		(){return PHelper_impl;}
//---------------------------------------------------------------------------
std::string FolderAppend	(LPCSTR val)
{
    return (val&&val[0])?std::string(val)+std::string("\\"):std::string("");
}
ref_str CPropHelper::PrepareKey		(LPCSTR pref, LPCSTR key)
{
    R_ASSERT(key);
    return ref_str(std::string(FolderAppend(pref)+std::string(key)).c_str());
}
ref_str CPropHelper::PrepareKey		(LPCSTR pref0, LPCSTR pref1, LPCSTR key)
{
    R_ASSERT(key);
    return ref_str(std::string(FolderAppend(pref0)+FolderAppend(pref1)+std::string(key)).c_str());
}
ref_str CPropHelper::PrepareKey		(LPCSTR pref0, LPCSTR pref1, LPCSTR pref2, LPCSTR key)
{
    R_ASSERT(key);
    return ref_str(std::string(FolderAppend(pref0)+FolderAppend(pref1)+FolderAppend(pref2)+std::string(key)).c_str());
}
//---------------------------------------------------------------------------
PropItem* CPropHelper::CreateItem(PropItemVec& items, const ref_str& key, EPropType type, u32 item_flags)
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
PropValue* CPropHelper::AppendValue(PropItemVec& items, const ref_str& key, PropValue* val, EPropType type, u32 item_flags)
{
    PropItem* item	= CreateItem(items,key,type,item_flags);
    val->m_Owner	= item;
    item->AppendValue(val);
    return val;
}
PropItem* CPropHelper::FindItem(PropItemVec& items, ref_str key, EPropType type)
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
//---------------------------------------------------------------------------

CaptionValue*  	CPropHelper::CreateCaption	(PropItemVec& items, ref_str key, ref_str val)
{	return	   	(CaptionValue*)	AppendValue	(items,key,xr_new<CaptionValue>(val),PROP_CAPTION);					}
CanvasValue*   	CPropHelper::CreateCanvas	(PropItemVec& items, ref_str key, ref_str val, int height)
{	return	   	(CanvasValue*)	AppendValue	(items,key,xr_new<CanvasValue>(val,height),PROP_CANVAS);  			}
ButtonValue*   	CPropHelper::CreateButton	(PropItemVec& items, ref_str key, ref_str val, u32 flags)
{	return	   	(ButtonValue*)	AppendValue	(items,key,xr_new<ButtonValue>(val,flags),PROP_BUTTON);				}
ChooseValue*	CPropHelper::CreateChoose	(PropItemVec& items, ref_str key, ref_str* val, u32 mode, LPCSTR path)
{	return	   	(ChooseValue*)	AppendValue	(items,key,xr_new<ChooseValue>(val,mode,path),PROP_CHOOSE,((mode==smTexture)||(mode==smObject))?PropItem::flDrawThumbnail:0);	}       //.
S8Value* 		CPropHelper::CreateS8		(PropItemVec& items, ref_str key, s8* val, s8 mn, s8 mx, s8 inc)
{	return	   	(S8Value*)		AppendValue	(items,key,xr_new<S8Value>(val,mn,mx,inc,0),PROP_NUMERIC);			}
S16Value* 		CPropHelper::CreateS16		(PropItemVec& items, ref_str key, s16* val, s16 mn, s16 mx, s16 inc)
{	return	   	(S16Value*)		AppendValue	(items,key,xr_new<S16Value>(val,mn,mx,inc,0),PROP_NUMERIC); 		}
S32Value* 	 	CPropHelper::CreateS32		(PropItemVec& items, ref_str key, s32* val, s32 mn, s32 mx, s32 inc)
{   return		(S32Value*)		AppendValue	(items,key,xr_new<S32Value>(val,mn,mx,inc,0),PROP_NUMERIC); 		}
U8Value* 		CPropHelper::CreateU8		(PropItemVec& items, ref_str key, u8* val, u8 mn, u8 mx, u8 inc)
{   return		(U8Value*)		AppendValue	(items,key,xr_new<U8Value>(val,mn,mx,inc,0),PROP_NUMERIC);   		}
U16Value* 		CPropHelper::CreateU16		(PropItemVec& items, ref_str key, u16* val, u16 mn, u16 mx, u16 inc)
{   return		(U16Value*)		AppendValue	(items,key,xr_new<U16Value>(val,mn,mx,inc,0),PROP_NUMERIC); 		}
U32Value* 	  	CPropHelper::CreateU32		(PropItemVec& items, ref_str key, u32* val, u32 mn, u32 mx, u32 inc)
{	return		(U32Value*)		AppendValue	(items,key,xr_new<U32Value>(val,mn,mx,inc,0),PROP_NUMERIC);			}
FloatValue* 	CPropHelper::CreateFloat	(PropItemVec& items, ref_str key, float* val, float mn, float mx, float inc, int decim)
{   return		(FloatValue*)	AppendValue	(items,key,xr_new<FloatValue>(val,mn,mx,inc,decim),PROP_NUMERIC);	}
BOOLValue* 	  	CPropHelper::CreateBOOL		(PropItemVec& items, ref_str key, BOOL* val)
{   return		(BOOLValue*)	AppendValue	(items,key,xr_new<BOOLValue>(val),PROP_BOOLEAN);        			}
VectorValue*   	CPropHelper::CreateVector	(PropItemVec& items, ref_str key, Fvector* val, float mn, float mx, float inc, int decim)
{   return		(VectorValue*)	AppendValue	(items,key,xr_new<VectorValue>(val,mn,mx,inc,decim),PROP_VECTOR);	}
Flag8Value*		CPropHelper::CreateFlag8   	(PropItemVec& items, ref_str key, Flags8* val, u8 mask, LPCSTR c0, LPCSTR c1, u32 flags)
{   return		(Flag8Value*)	AppendValue	(items,key,xr_new<Flag8Value>(val,mask,c0,c1,flags),PROP_FLAG);    	}
Flag16Value*	CPropHelper::CreateFlag16	(PropItemVec& items, ref_str key, Flags16* val, u16 mask, LPCSTR c0, LPCSTR c1, u32 flags)
{   return		(Flag16Value*)	AppendValue	(items,key,xr_new<Flag16Value>(val,mask,c0,c1,flags),PROP_FLAG);    }
Flag32Value*	CPropHelper::CreateFlag32	(PropItemVec& items, ref_str key, Flags32* val, u32 mask, LPCSTR c0, LPCSTR c1, u32 flags)
{   return		(Flag32Value*)	AppendValue	(items,key,xr_new<Flag32Value>(val,mask,c0,c1,flags),PROP_FLAG);    }
Token8Value*	CPropHelper::CreateToken8	(PropItemVec& items, ref_str key, u8* val, xr_token* token)
{   return		(Token8Value*)	AppendValue	(items,key,xr_new<Token8Value> (val,token),PROP_TOKEN);				}
Token16Value*	CPropHelper::CreateToken16	(PropItemVec& items, ref_str key, u16* val, xr_token* token)
{   return		(Token16Value*)	AppendValue	(items,key,xr_new<Token16Value>(val,token),PROP_TOKEN);				}
Token32Value*	CPropHelper::CreateToken32	(PropItemVec& items, ref_str key, u32* val, xr_token* token)
{   return		(Token32Value*)	AppendValue	(items,key,xr_new<Token32Value>(val,token),PROP_TOKEN);				}
RToken8Value* 	CPropHelper::CreateRToken8	(PropItemVec& items, ref_str key, u8* val, RTokenVec* token)
{   return		(RToken8Value*)	AppendValue	(items,key,xr_new<RToken8Value>(val,token),PROP_RTOKEN);			}
RToken16Value* 	CPropHelper::CreateRToken16	(PropItemVec& items, ref_str key, u16* val, RTokenVec* token)
{   return		(RToken16Value*)AppendValue	(items,key,xr_new<RToken16Value>(val,token),PROP_RTOKEN);			}
RToken32Value* 	CPropHelper::CreateRToken32	(PropItemVec& items, ref_str key, u32* val, RTokenVec* token)
{   return		(RToken32Value*)AppendValue	(items,key,xr_new<RToken32Value>(val,token),PROP_RTOKEN);			}
TokenValueSH*   CPropHelper::CreateTokenSH	(PropItemVec& items, ref_str key, u32* val, u32 cnt, const TokenValueSH::Item* lst)
{   return		(TokenValueSH*)	AppendValue	(items,key,xr_new<TokenValueSH>(val,cnt,lst),PROP_SH_TOKEN);		}
ListValue* 	 	CPropHelper::CreateList		(PropItemVec& items, ref_str key, ref_str* val, RStringVec* lst)
{   return		(ListValue*)	AppendValue	(items,key,xr_new<ListValue>(val,lst),PROP_LIST);       			}
U32Value*  		CPropHelper::CreateColor   	(PropItemVec& items, ref_str key, u32* val)
{   return		(U32Value*)		AppendValue	(items,key,xr_new<U32Value>(val,0x00000000,0xffffffff,1,0),PROP_COLOR);}
ColorValue*		CPropHelper::CreateFColor	(PropItemVec& items, ref_str key, Fcolor* val)
{   return		(ColorValue*)	AppendValue	(items,key,xr_new<ColorValue>(val),PROP_FCOLOR);        			}
VectorValue*	CPropHelper::CreateVColor	(PropItemVec& items, ref_str key, Fvector* val)
{   return		(VectorValue*)	AppendValue	(items,key,xr_new<VectorValue>(val,0.f,1.f,0,0),PROP_VCOLOR);  		}
RTextValue* 	CPropHelper::CreateRText	(PropItemVec& items, ref_str key, ref_str* val)
{   return		(RTextValue*)	AppendValue	(items,key,xr_new<RTextValue>(val),PROP_RTEXT);        				}
WaveValue* 		CPropHelper::CreateWave		(PropItemVec& items, ref_str key, WaveForm* val)
{	return		(WaveValue*)	AppendValue	(items,key,xr_new<WaveValue>(val),PROP_WAVE);           			}
FloatValue* 	CPropHelper::CreateTime		(PropItemVec& items, ref_str key, float* val, float mn, float mx)
{	return		(FloatValue*)	AppendValue	(items,key,xr_new<FloatValue>(val,mn,mx,0,0),PROP_TIME);    		}
//---------------------------------------------------------------------------
    
FloatValue* 	CPropHelper::CreateAngle 	(PropItemVec& items, ref_str key, float* val, float mn, float mx, float inc, int decim)
{   FloatValue* V				= (FloatValue*)	AppendValue		(items,key,xr_new<FloatValue>(val,mn,mx,inc,decim),PROP_NUMERIC);
    V->OnAfterEditEvent			= floatRDOnAfterEdit;
    V->OnBeforeEditEvent		= floatRDOnBeforeEdit; 
    V->Owner()->OnDrawTextEvent.bind(this,&CPropHelper::floatRDOnDraw);
    return V;						
}
VectorValue* 	CPropHelper::CreateAngle3	(PropItemVec& items, ref_str key, Fvector* val, float mn, float mx, float inc, int decim)
{   VectorValue* V				= (VectorValue*)	AppendValue		(items,key,xr_new<VectorValue>(val,mn,mx,inc,decim),PROP_VECTOR);
    V->OnAfterEditEvent			= FvectorRDOnAfterEdit;
    V->OnBeforeEditEvent		= FvectorRDOnBeforeEdit;
    V->Owner()->OnDrawTextEvent.bind(this,&CPropHelper::FvectorRDOnDraw);
    return V;					
}
RTextValue* 	CPropHelper::CreateName		(PropItemVec& items, ref_str key, ref_str* val, ListItem* owner)  
{   RTextValue* V				= (RTextValue*) CreateRText	(items,key,val);
    V->OnAfterEditEvent   		= NameAfterEdit;
    V->OnBeforeEditEvent  		= NameBeforeEdit;
    V->Owner()->OnDrawTextEvent.bind(this,&CPropHelper::NameDraw);
    V->tag						= (u32)owner; VERIFY(owner);
    if (V->Owner()->m_Flags.is(PropItem::flMixed)) V->Owner()->m_Flags.set(PropItem::flDisabled,TRUE);
    return V;					
}
RTextValue* 	CPropHelper::CreateNameCB	(PropItemVec& items, ref_str key, ref_str* val, TOnDrawTextEvent draw, void __stdcall (__closure* before) (PropValue*, ref_str&), void __stdcall (__closure* after) (PropValue*, ref_str&, bool&))  
{   RTextValue* V				= (RTextValue*) CreateRText	(items,key,val);
    V->OnAfterEditEvent   		= after;
    V->OnBeforeEditEvent  		= before;
    V->Owner()->OnDrawTextEvent = draw;
    if (V->Owner()->m_Flags.is(PropItem::flMixed)) V->Owner()->m_Flags.set(PropItem::flDisabled,TRUE);
    return V;					
}
//---------------------------------------------------------------------------

void __fastcall 	CPropHelper::FvectorRDOnBeforeEdit(PropValue* sender, Fvector& edit_val)
{
    edit_val.set	(rad2deg(edit_val.x),rad2deg(edit_val.y),rad2deg(edit_val.z));
    VectorValue* V 	= dynamic_cast<VectorValue*>(sender); R_ASSERT(V);
    V->lim_mn.set	(rad2deg(V->lim_mn.x),rad2deg(V->lim_mn.y),rad2deg(V->lim_mn.z));  
    V->lim_mx.set	(rad2deg(V->lim_mx.x),rad2deg(V->lim_mx.y),rad2deg(V->lim_mx.z));  
}
void __fastcall 	CPropHelper::FvectorRDOnDraw(PropValue* sender, ref_str& draw_val)
{
	VectorValue* V	= dynamic_cast<VectorValue*>(sender); VERIFY(V);
    Fvector val;    val.set	(rad2deg(V->value->x),rad2deg(V->value->y),rad2deg(V->value->z));
    rstring_sprintf	(draw_val,val,V->dec);
}
void __fastcall 	CPropHelper::FvectorRDOnAfterEdit(PropValue* sender, Fvector& edit_val, bool& accepted)
{
    edit_val.set	(deg2rad(edit_val.x),deg2rad(edit_val.y),deg2rad(edit_val.z));
    VectorValue* V 	= dynamic_cast<VectorValue*>(sender); R_ASSERT(V);
    V->lim_mn.set	(deg2rad(V->lim_mn.x),deg2rad(V->lim_mn.y),deg2rad(V->lim_mn.z));  
    V->lim_mx.set	(deg2rad(V->lim_mx.x),deg2rad(V->lim_mx.y),deg2rad(V->lim_mx.z));  
}
//------------------------------------------------------------------------------

void __fastcall 	CPropHelper::floatRDOnBeforeEdit(PropValue* sender, float& edit_val)
{
    edit_val 		= rad2deg(edit_val);
    FloatValue* V 	= dynamic_cast<FloatValue*>(sender); R_ASSERT(V);
    V->lim_mn 		= rad2deg(V->lim_mn); V->lim_mx = rad2deg(V->lim_mx);
}
void __fastcall 	CPropHelper::floatRDOnDraw(PropValue* sender, ref_str& draw_val)
{
	FloatValue* V	= dynamic_cast<FloatValue*>(sender); VERIFY(V);
    float val;    	val = rad2deg(*V->value);
    rstring_sprintf	(draw_val,val,V->dec);
}
void __fastcall 	CPropHelper::floatRDOnAfterEdit(PropValue* sender, float& edit_val, bool& accepted)
{
    edit_val 		= deg2rad(edit_val);
    FloatValue* V 	= dynamic_cast<FloatValue*>(sender); R_ASSERT(V);
    V->lim_mn 		= deg2rad(V->lim_mn); V->lim_mx = deg2rad(V->lim_mx);
}
//------------------------------------------------------------------------------

void CPropHelper::NameBeforeEdit(PropValue* sender, ref_str& edit_val)
{
	int cnt			=_GetItemCount(edit_val.c_str(),'\\');
	edit_val		= _SetPos(edit_val.c_str(),cnt-1,'\\');
}
void CPropHelper::NameDraw(PropValue* sender, ref_str& draw_val)
{
	RTextValue* V	= dynamic_cast<RTextValue*>(sender); VERIFY(V);
	int cnt			=_GetItemCount(V->value->c_str(),'\\');
	draw_val 		= _SetPos(V->value->c_str(),cnt-1,'\\');
}
void CPropHelper::NameAfterEdit(PropValue* sender, ref_str& edit_val, bool& accepted)
{
	RTextValue* V	= dynamic_cast<RTextValue*>(sender); VERIFY(V);
    ListItem* L		= (ListItem*)sender->tag;
	accepted		= LHelper().NameAfterEdit(L,V->GetValue(),edit_val);
}
//---------------------------------------------------------------------------




////////////////////////////////////////////////////////////////////////////
//	Module 		: script_properties_list_helper.cpp
//	Created 	: 14.07.2004
//  Modified 	: 14.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Script properties list helper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_properties_list_helper.h"
#include "script_token_list.h"

void CScriptPropertiesListHelper::FvectorRDOnAfterEdit	(PropValue* sender,  Fvector& edit_val, bool& accepted)
{
	PHelper().FvectorRDOnAfterEdit(sender,edit_val,accepted);
}

void CScriptPropertiesListHelper::FvectorRDOnBeforeEdit	(PropValue* sender, Fvector& edit_val)
{
	PHelper().FvectorRDOnBeforeEdit(sender,edit_val);
}

void CScriptPropertiesListHelper::FvectorRDOnDraw		(PropValue* sender,  LPCSTR& draw_val)
{
	ref_str		temp;
	PHelper().FvectorRDOnDraw(sender,temp);
	draw_val	= *temp;
}

void CScriptPropertiesListHelper::floatRDOnAfterEdit	(PropValue* sender,  float&	 edit_val, bool& accepted)
{
	PHelper().floatRDOnAfterEdit(sender,edit_val,accepted);
}

void CScriptPropertiesListHelper::floatRDOnBeforeEdit	(PropValue* sender,  float&	 edit_val)
{
	PHelper().floatRDOnBeforeEdit(sender,edit_val);
}

void CScriptPropertiesListHelper::floatRDOnDraw			(PropValue* sender,  LPCSTR& draw_val)
{
	ref_str		temp;
	PHelper().floatRDOnDraw(sender,temp);
	draw_val	= *temp;
}

void CScriptPropertiesListHelper::NameAfterEdit			(PropValue* sender,  LPCSTR& edit_val, bool& accepted)
{
	ref_str		temp;
	PHelper().NameAfterEdit(sender,temp,accepted);
	edit_val	= *temp;
}

void CScriptPropertiesListHelper::NameBeforeEdit		(PropValue* sender,  LPCSTR& edit_val)
{
	ref_str		temp;
	PHelper().NameBeforeEdit(sender,temp);
	edit_val	= *temp;
}

void CScriptPropertiesListHelper::NameDraw				(PropValue* sender,  LPCSTR& draw_val)
{
	ref_str		temp;
	PHelper().NameDraw(sender,temp);
	draw_val	= *temp;
}


CaptionValue*	CScriptPropertiesListHelper::CreateCaption	(PropItemVec* items, LPCSTR key, LPCSTR val)
{
	return		(PHelper().CreateCaption(*items,key,val));
}

CanvasValue*	CScriptPropertiesListHelper::CreateCanvas	(PropItemVec* items, LPCSTR key, LPCSTR val, int height)
{
	return		(PHelper().CreateCanvas(*items,key,val,height));
}

ButtonValue*	CScriptPropertiesListHelper::CreateButton	(PropItemVec* items, LPCSTR key, LPCSTR val, u32 flags)
{
	return		(PHelper().CreateButton(*items,key,val,flags));
}

ChooseValue*	CScriptPropertiesListHelper::CreateChoose	(PropItemVec* items, LPCSTR key, LPCSTR* val, u32 mode, LPCSTR path)
{
	ref_str		temp;
	ChooseValue*temp1 = PHelper().CreateChoose(*items,key,&temp,mode,path);
	*val		= *temp;
	return		(temp1);
}

S8Value* CScriptPropertiesListHelper::CreateS8		(PropItemVec* items, LPCSTR key, s8* val,  s8 mn,  s8 mx,  s8 inc)
{
	return		(PHelper().CreateS8(*items,key,val,mn,mx,inc));
}

S16Value* CScriptPropertiesListHelper::CreateS16	(PropItemVec* items, LPCSTR key, s16* val, s16 mn, s16 mx, s16 inc)
{
	return		(PHelper().CreateS16(*items,key,val,mn,mx,inc));
}

S32Value* CScriptPropertiesListHelper::CreateS32	(PropItemVec* items, LPCSTR key, s32* val, s32 mn, s32 mx, s32 inc)
{
	return		(PHelper().CreateS32(*items,key,val,mn,mx,inc));
}

U8Value* CScriptPropertiesListHelper::CreateU8		(PropItemVec* items, LPCSTR key, u8* val,  u8 mn,  u8 mx,  u8 inc)
{																											   
	return		(PHelper().CreateU8(*items,key,val,mn,mx,inc));												   
}																											   

U16Value* CScriptPropertiesListHelper::CreateU16	(PropItemVec* items, LPCSTR key, u16* val, u16 mn, u16 mx, u16 inc)
{																											   
	return		(PHelper().CreateU16(*items,key,val,mn,mx,inc));												   
}																											   

U32Value* CScriptPropertiesListHelper::CreateU32	(PropItemVec* items, LPCSTR key, u32* val, u32 mn, u32 mx, u32 inc)
{
	return		(PHelper().CreateU32(*items,key,val,mn,mx,inc));
}

FloatValue* CScriptPropertiesListHelper::CreateFloat(PropItemVec* items, LPCSTR key, float* val, float mn, float mx, float inc, int decim)
{
	return		(PHelper().CreateFloat(*items,key,val,mn,mx,inc,decim));
}

BOOLValue* CScriptPropertiesListHelper::CreateBOOL(PropItemVec* items, LPCSTR key, bool* val)
{
	BOOL		_val = *val;
	BOOLValue*	temp = PHelper().CreateBOOL(*items,key,&_val);
	*val		= !!_val;
	return		(temp);
}

VectorValue* CScriptPropertiesListHelper::CreateVector(PropItemVec* items, LPCSTR key, Fvector* val, float mn, float mx, float inc, int decim)
{
	return		(PHelper().CreateVector(*items,key,val,mn,mx,inc,decim));
}

Flag8Value*		CScriptPropertiesListHelper::CreateFlag8	(PropItemVec* items, LPCSTR key, Flags8* val, u8 mask, LPCSTR c0, LPCSTR c1, u32 flags)
{
	return		(PHelper().CreateFlag8(*items,key,val,mask,c0,c1,flags));
}

Flag16Value*	CScriptPropertiesListHelper::CreateFlag16	(PropItemVec* items, LPCSTR key, Flags16* val, u16 mask, LPCSTR c0, LPCSTR c1, u32 flags)
{
	return		(PHelper().CreateFlag16(*items,key,val,mask,c0,c1,flags));
}

Flag32Value*	CScriptPropertiesListHelper::CreateFlag32	(PropItemVec* items, LPCSTR key, Flags32* val, u32 mask, LPCSTR c0, LPCSTR c1, u32 flags)
{
	return		(PHelper().CreateFlag32(*items,key,val,mask,c0,c1,flags));
}

Token8Value*	CScriptPropertiesListHelper::CreateToken8	(PropItemVec* items, LPCSTR key, u8* val, CScriptTokenList* token)
{
	return		(PHelper().CreateToken8(*items,key,val,&*token->tokens().begin()));
}

Token16Value*	CScriptPropertiesListHelper::CreateToken16	(PropItemVec* items, LPCSTR key, u16* val, CScriptTokenList* token)
{
	return		(PHelper().CreateToken16(*items,key,val,&*token->tokens().begin()));
}

Token32Value*	CScriptPropertiesListHelper::CreateToken32	(PropItemVec* items, LPCSTR key, u32* val, CScriptTokenList* token)
{
	return		(PHelper().CreateToken32(*items,key,val,&*token->tokens().begin()));
}
/*
RToken8Value*	CScriptPropertiesListHelper::CreateRToken8	(PropItemVec* items, LPCSTR key, u8* val, xr_rtoken* token, u32 t_cnt)
{
	return		(PHelper().CreateRToken8(*items,key,val,token));
}

RToken16Value*	CScriptPropertiesListHelper::CreateRToken16	(PropItemVec* items, LPCSTR key, u16* val, RTokenVec* token)
{
	return		(PHelper().CreateRToken16(*items,key,val,token));
}

RToken32Value*	CScriptPropertiesListHelper::CreateRToken32	(PropItemVec* items, LPCSTR key, u32* val, RTokenVec* token)
{
	return		(PHelper().CreateRToken32(*items,key,val,token));
}
*/
U32Value*  CScriptPropertiesListHelper::CreateColor	(PropItemVec* items, LPCSTR key, u32* val)
{
	return		(PHelper().CreateColor(*items,key,val));
}

ColorValue*	CScriptPropertiesListHelper::CreateFColor	(PropItemVec* items, LPCSTR key, Fcolor* val)
{
	return		(PHelper().CreateFColor(*items,key,val));
}

VectorValue* CScriptPropertiesListHelper::CreateVColor(PropItemVec* items, LPCSTR key, Fvector* val)
{
	return		(PHelper().CreateVColor(*items,key,val));
}

RTextValue*	CScriptPropertiesListHelper::CreateRText	(PropItemVec* items, LPCSTR key, LPCSTR* val)
{
	ref_str		temp;
	RTextValue*	temp1 = PHelper().CreateRText(*items,key,&temp);
	*val		= *temp;
	return		(temp1);
}

FloatValue* CScriptPropertiesListHelper::CreateTime		(PropItemVec* items, LPCSTR key, float* val, float mn, float mx)
{
	return		(PHelper().CreateTime(*items,key,val,mn,mx));
}

FloatValue* CScriptPropertiesListHelper::CreateAngle	(PropItemVec* items, LPCSTR key, float* val, float mn, float mx, float inc, int decim)
{
	return		(PHelper().CreateAngle(*items,key,val,mn,mx,inc,decim));
}

VectorValue* CScriptPropertiesListHelper::CreateAngle3	(PropItemVec* items, LPCSTR key, Fvector* val, float mn, float mx, float inc, int decim)
{
	return		(PHelper().CreateAngle3(*items,key,val,mn,mx,inc,decim));
}
////////////////////////////////////////////////////////////////////////////
//	Module 		: script_properties_list_helper.cpp
//	Created 	: 14.07.2004
//  Modified 	: 14.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Script properties list helper
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "script_properties_list_helper.h"

S8Value* CScriptPropertiesListHelper::CreateS8		(PropItemVec& items, LPCSTR key, s8* val,  s8 mn,  s8 mx,  s8 inc)
{
	return		(PHelper().CreateS8(items,key,val,mn,mx,inc));
}

S16Value* CScriptPropertiesListHelper::CreateS16	(PropItemVec& items, LPCSTR key, s16* val, s16 mn, s16 mx, s16 inc)
{
	return		(PHelper().CreateS16(items,key,val,mn,mx,inc));
}

S32Value* CScriptPropertiesListHelper::CreateS32	(PropItemVec& items, LPCSTR key, s32* val, s32 mn, s32 mx, s32 inc)
{
	return		(PHelper().CreateS32(items,key,val,mn,mx,inc));
}

U8Value* CScriptPropertiesListHelper::CreateU8		(PropItemVec& items, LPCSTR key, u8* val,  u8 mn,  u8 mx,  u8 inc)
{																											   
	return		(PHelper().CreateU8(items,key,val,mn,mx,inc));												   
}																											   

U16Value* CScriptPropertiesListHelper::CreateU16	(PropItemVec& items, LPCSTR key, u16* val, u16 mn, u16 mx, u16 inc)
{																											   
	return		(PHelper().CreateU16(items,key,val,mn,mx,inc));												   
}																											   

U32Value* CScriptPropertiesListHelper::CreateU32	(PropItemVec& items, LPCSTR key, u32* val, u32 mn, u32 mx, u32 inc)
{
	return		(PHelper().CreateU32(items,key,val,mn,mx,inc));
}

FloatValue* CScriptPropertiesListHelper::CreateFloat(PropItemVec& items, LPCSTR key, float* val, float mn, float mx, float inc, int decim)
{
	return		(PHelper().CreateFloat(items,key,val,mn,mx,inc,decim));
}

BOOLValue* CScriptPropertiesListHelper::CreateBOOL(PropItemVec& items, LPCSTR key, bool* val)
{
	BOOL		_val = *val;
	BOOLValue*	temp = PHelper().CreateBOOL(items,key,&_val);
	*val		= !!_val;
	return		(temp);
}

VectorValue* CScriptPropertiesListHelper::CreateVector(PropItemVec& items, LPCSTR key, Fvector* val, float mn, float mx, float inc, int decim)
{
	return		(PHelper().CreateVector(items,key,val,mn,mx,inc,decim));
}

U32Value*  CScriptPropertiesListHelper::CreateColor	(PropItemVec& items, LPCSTR key, u32* val)
{
	return		(PHelper().CreateColor(items,key,val));
}

ColorValue*	CScriptPropertiesListHelper::CreateFColor	(PropItemVec& items, LPCSTR key, Fcolor* val)
{
	return		(PHelper().CreateFColor(items,key,val));
}

VectorValue* CScriptPropertiesListHelper::CreateVColor(PropItemVec& items, LPCSTR key, Fvector* val)
{
	return		(PHelper().CreateVColor(items,key,val));
}

RTextValue*	CScriptPropertiesListHelper::CreateRText	(PropItemVec& items, LPCSTR key, LPCSTR* val)
{
	ref_str		temp;
	RTextValue*	temp1 = PHelper().CreateRText(items,key,&temp);
	*val		= *temp;
	return		(temp1);
}

FloatValue* CScriptPropertiesListHelper::CreateTime		(PropItemVec& items, LPCSTR key, float* val, float mn, float mx)
{
	return		(PHelper().CreateTime(items,key,val,mn,mx));
}

FloatValue* CScriptPropertiesListHelper::CreateAngle	(PropItemVec& items, LPCSTR key, float* val, float mn, float mx, float inc, int decim)
{
	return		(PHelper().CreateAngle(items,key,val,mn,mx,inc,decim));
}

VectorValue* CScriptPropertiesListHelper::CreateAngle3	(PropItemVec& items, LPCSTR key, Fvector* val, float mn, float mx, float inc, int decim)
{
	return		(PHelper().CreateAngle3(items,key,val,mn,mx,inc,decim));
}
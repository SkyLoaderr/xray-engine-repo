////////////////////////////////////////////////////////////////////////////
//	Module 		: script_properties_list_helper.h
//	Created 	: 14.07.2004
//  Modified 	: 14.07.2004
//	Author		: Dmitriy Iassenev
//	Description : Script properties list helper
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "xrCDB.h"
#ifndef XRGAME_EXPORTS
#	include "Sound.h"
#endif
#include "xrEProps.h"
#include "PropertiesListHelper.h"
#include "script_export_space.h"

struct CScriptPropertiesListHelper {
//			void 				FvectorRDOnAfterEdit(PropValue* sender,  Fvector& edit_val, bool& accepted);
//			void 				FvectorRDOnBeforeEdit(PropValue* sender, Fvector& edit_val);
//			void 				FvectorRDOnDraw		(PropValue* sender,  LPCSTR& draw_val);
//			void 				floatRDOnAfterEdit	(PropValue* sender,  float&	 edit_val, bool& accepted);
//			void 				floatRDOnBeforeEdit	(PropValue* sender,  float&	 edit_val);
//			void 				floatRDOnDraw		(PropValue* sender,  LPCSTR& draw_val);
// name edit
//			void 				NameAfterEdit		(PropValue* sender,  LPCSTR& edit_val, bool& accepted);
//			void				NameBeforeEdit		(PropValue* sender,  LPCSTR& edit_val);
//			void 				NameDraw			(PropValue* sender,  LPCSTR& draw_val);
public:
//			CaptionValue*  		CreateCaption	    (PropItemVec& items, LPCSTR key, LPCSTR val);
//			CanvasValue*		CreateCanvas	    (PropItemVec& items, LPCSTR key, LPCSTR val, int height);
//			ButtonValue*		CreateButton	    (PropItemVec& items, LPCSTR key, LPCSTR val, u32 flags);
//			ChooseValue*		CreateChoose	    (PropItemVec& items, LPCSTR key, LPCSTR* val, u32 mode, LPCSTR path=0);
			S8Value* 			CreateS8		    (PropItemVec& items, LPCSTR key, s8* val, s8 mn=0, s8 mx=100, s8 inc=1);
			S16Value* 			CreateS16		    (PropItemVec& items, LPCSTR key, s16* val, s16 mn=0, s16 mx=100, s16 inc=1);
			S32Value* 	 		CreateS32		    (PropItemVec& items, LPCSTR key, s32* val, s32 mn=0, s32 mx=100, s32 inc=1);
			U8Value* 			CreateU8		    (PropItemVec& items, LPCSTR key, u8* val, u8 mn=0, u8 mx=100, u8 inc=1);
			U16Value* 			CreateU16		    (PropItemVec& items, LPCSTR key, u16* val, u16 mn=0, u16 mx=100, u16 inc=1);
			U32Value* 	  		CreateU32		    (PropItemVec& items, LPCSTR key, u32* val, u32 mn=0, u32 mx=100, u32 inc=1);
			FloatValue* 		CreateFloat		    (PropItemVec& items, LPCSTR key, float* val, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2);
			BOOLValue* 	  		CreateBOOL		    (PropItemVec& items, LPCSTR key, bool* val);
			VectorValue*  	 	CreateVector	    (PropItemVec& items, LPCSTR key, Fvector* val, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2);
			Flag8Value*			CreateFlag8		    (PropItemVec& items, LPCSTR key, Flags8* val, u8 mask, LPCSTR c0=0, LPCSTR c1=0, u32 flags=0);
			Flag16Value*		CreateFlag16	    (PropItemVec& items, LPCSTR key, Flags16* val, u16 mask, LPCSTR c0=0, LPCSTR c1=0, u32 flags=0);
			Flag32Value*		CreateFlag32	    (PropItemVec& items, LPCSTR key, Flags32* val, u32 mask, LPCSTR c0=0, LPCSTR c1=0, u32 flags=0);
//			Token8Value*		CreateToken8	    (PropItemVec& items, LPCSTR key, u8* val, xr_token* token);
//			Token16Value*		CreateToken16	    (PropItemVec& items, LPCSTR key, u16* val, xr_token* token);
//			Token32Value*		CreateToken32	    (PropItemVec& items, LPCSTR key, u32* val, xr_token* token);
//			RToken8Value* 		CreateRToken8	    (PropItemVec& items, LPCSTR key, u8* val, RTokenVec* token);
//			RToken16Value* 		CreateRToken16	    (PropItemVec& items, LPCSTR key, u16* val, RTokenVec* token);
//			RToken32Value* 		CreateRToken32	    (PropItemVec& items, LPCSTR key, u32* val, RTokenVec* token);
//			TokenValueSH*   	CreateTokenSH	    (PropItemVec& items, LPCSTR key, u32* val, u32 cnt, const TokenValueSH::Item* lst);
//			ListValue* 	 		CreateList		    (PropItemVec& items, LPCSTR key, LPCSTR* val, RStringVec* lst);
			U32Value*  			CreateColor		    (PropItemVec& items, LPCSTR key, u32* val);
			ColorValue*			CreateFColor	    (PropItemVec& items, LPCSTR key, Fcolor* val);
			VectorValue*		CreateVColor	    (PropItemVec& items, LPCSTR key, Fvector* val);
			RTextValue* 		CreateRText		    (PropItemVec& items, LPCSTR key, LPCSTR* val);
//			WaveValue* 			CreateWave		    (PropItemVec& items, LPCSTR key, WaveForm* val);
			FloatValue* 		CreateTime		    (PropItemVec& items, LPCSTR key, float* val, float mn=0.f, float mx=86400.f);

			FloatValue* 		CreateAngle		    (PropItemVec& items, LPCSTR key, float* val, float mn=0.f, float mx=PI_MUL_2, float inc=0.01f, int decim=2);
			VectorValue* 		CreateAngle3	    (PropItemVec& items, LPCSTR key, Fvector* val, float mn=0.f, float mx=PI_MUL_2, float inc=0.01f, int decim=2);
//			RTextValue* 		CreateName		    (PropItemVec& items, LPCSTR key, LPCSTR* val, ListItem* owner);
//			RTextValue* 		CreateNameCB		(PropItemVec& items, LPCSTR key, LPCSTR* val, TOnDrawTextEvent=0, RTextValue::TOnBeforeEditEvent=0, RTextValue::TOnAfterEditEvent=0);
			DECLARE_SCRIPT_REGISTER_FUNCTION
};
add_to_type_list(CScriptPropertiesListHelper)
#undef script_type_list
#define script_type_list save_type_list(CScriptPropertiesListHelper)

//---------------------------------------------------------------------------
#ifndef PropertiesListHelperH
#define PropertiesListHelperH

#include "PropertiesListTypes.h"

//---------------------------------------------------------------------------
class CPropHelper{
	IC PropItem*		CreateItem		(PropItemVec& items, LPCSTR key, EPropType type)
    {
    	PropItem* item	= FindItem(items,key,type);
        if (!item){ 
        	item		= new PropItem	(type);
            item->SetName(key);
            items.push_back(item);
        }
        return			item;
    }
    IC PropValue*		AppendValue		(PropItemVec& items, LPCSTR key, PropValue* val, EPropType type)
    {
    	PropItem* item	= CreateItem(items,key,type);
        val->m_Owner	= item;
        item->AppendValue(val);
        return val;
    }
//------------------------------------------------------------------------------
	static AnsiString XKey; 
    IC AnsiString		FolderAppend	(LPCSTR val)
    {
    	if (val&&val[0]) return AnsiString(val)+"\\";
        return   		"";
    }
public:
    IC LPCSTR			PrepareKey		(LPCSTR pref, LPCSTR key)
    {
        R_ASSERT(key);
    	XKey			= FolderAppend(pref)+AnsiString(key); 
        return XKey.c_str();
    }
    IC LPCSTR			PrepareKey		(LPCSTR pref0, LPCSTR pref1, LPCSTR key)
    {
        R_ASSERT(key);
    	XKey			= FolderAppend(pref0)+FolderAppend(pref1)+AnsiString(key); 
        return XKey.c_str();
    }
    IC LPCSTR			PrepareKey		(LPCSTR pref0, LPCSTR pref1, LPCSTR pref2, LPCSTR key)
    {
        R_ASSERT(key);
    	XKey			= FolderAppend(pref0)+FolderAppend(pref1)+FolderAppend(pref2)+AnsiString(key); 
        return XKey.c_str();
    }
    IC PropItem* 		FindItem		(PropItemVec& items,	LPCSTR key, EPropType type)
    {
    	for (PropItemIt it=items.begin(); it!=items.end(); it++)
        	if (((*it)->type==type)&&(0==strcmp((*it)->key,key))) return *it;
        return 0;
    }

//------------------------------------------------------------------------------
// predefind event routines    
    IC void __fastcall 	FvectorRDOnAfterEdit(PropValue* sender, LPVOID edit_val)
    {
        Fvector* V = (Fvector*)edit_val;
        V->x = deg2rad(V->x);
        V->y = deg2rad(V->y);
        V->z = deg2rad(V->z);
    }

    IC void __fastcall 	FvectorRDOnBeforeEdit(PropValue* sender, LPVOID edit_val)
    {
        Fvector* V = (Fvector*)edit_val;
        V->x = rad2deg(V->x);
        V->y = rad2deg(V->y);
        V->z = rad2deg(V->z);
    }

    IC void __fastcall 	FvectorRDOnDraw(PropValue* sender, LPVOID draw_val)
    {
        Fvector* V = (Fvector*)draw_val;
        V->x = rad2deg(V->x);
        V->y = rad2deg(V->y);
        V->z = rad2deg(V->z);
    }
    IC void __fastcall 	floatRDOnAfterEdit(PropValue* sender, LPVOID edit_val)
    {
        *(float*)edit_val = deg2rad(*(float*)edit_val);
    }

    IC void __fastcall 	floatRDOnBeforeEdit(PropValue* sender, LPVOID edit_val)
    {
        *(float*)edit_val = rad2deg(*(float*)edit_val);
    }

    IC void __fastcall 	floatRDOnDraw(PropValue* sender, LPVOID draw_val)
    {
        *(float*)draw_val = rad2deg(*(float*)draw_val);
    }
public:
    IC CaptionValue*	CreateCaption	(PropItemVec& items, LPCSTR key, LPCSTR val)
    {
    	return			(CaptionValue*)	AppendValue		(items,key,new CaptionValue(val),PROP_CAPTION);
    }
    IC S8Value* 		CreateS8		(PropItemVec& items, LPCSTR key, s8* val, s8 mn=0, s8 mx=100, s8 inc=1)
    {
    	return			(S8Value*)		AppendValue		(items,key,new S8Value(val,mn,mx,inc,0),PROP_S8);
    }
    IC S16Value* 		CreateS16		(PropItemVec& items, LPCSTR key, s16* val, s16 mn=0, s16 mx=100, s16 inc=1)
    {
    	return			(S16Value*)		AppendValue		(items,key,new S16Value(val,mn,mx,inc,0),PROP_S16);
    }
    IC S32Value* 	 	CreateS32		(PropItemVec& items, LPCSTR key, s32* val, s32 mn=0, s32 mx=100, s32 inc=1)
    {
    	return			(S32Value*)		AppendValue		(items,key,new S32Value(val,mn,mx,inc,0),PROP_S32);
    }
    IC U8Value* 		CreateU8		(PropItemVec& items, LPCSTR key, u8* val, u8 mn=0, u8 mx=100, u8 inc=1)
    {
    	return			(U8Value*)		AppendValue		(items,key,new U8Value(val,mn,mx,inc,0),PROP_U8);
    }
    IC U16Value* 		CreateU16		(PropItemVec& items, LPCSTR key, u16* val, u16 mn=0, u16 mx=100, u16 inc=1)
    {
    	return			(U16Value*)		AppendValue		(items,key,new U16Value(val,mn,mx,inc,0),PROP_U16);
    }
    IC U32Value* 	  	CreateU32		(PropItemVec& items, LPCSTR key, u32* val, u32 mn=0, u32 mx=100, u32 inc=1)
    {
    	return			(U32Value*)		AppendValue		(items,key,new U32Value(val,mn,mx,inc,0),PROP_U32);
    }
    IC FloatValue* 		CreateFloat		(PropItemVec& items, LPCSTR key, float* val, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2)
    {
    	return			(FloatValue*)	AppendValue		(items,key,new FloatValue(val,mn,mx,inc,decim),PROP_FLOAT);
    }           	
    IC BOOLValue* 	  	CreateBOOL		(PropItemVec& items, LPCSTR key, BOOL* val)
    {
    	return			(BOOLValue*)	AppendValue		(items,key,new BOOLValue(val),PROP_BOOLEAN);
    }
    IC Flag8Value* 		CreateFlag8		(PropItemVec& items, LPCSTR key, Flags8* val, u8 mask)
    {
    	return			(Flag8Value*)	AppendValue		(items,key,new Flag8Value(val,mask),PROP_FLAG8);
    }
    IC Flag16Value* 	CreateFlag16	(PropItemVec& items, LPCSTR key, Flags16* val, u16 mask)
    {
    	return			(Flag16Value*)	AppendValue		(items,key,new Flag16Value(val,mask),PROP_FLAG16);
    }
    IC Flag32Value*    	CreateFlag32	(PropItemVec& items, LPCSTR key, Flags32* val, u32 mask)
    {
    	return			(Flag32Value*)	AppendValue		(items,key,new Flag32Value(val,mask),PROP_FLAG32);
    }
    IC VectorValue*   	CreateVector	(PropItemVec& items, LPCSTR key, Fvector* val, float mn=0.f, float mx=1.f, float inc=0.01f, int decim=2)
    {
    	return			(VectorValue*)	AppendValue		(items,key,new VectorValue(val,mn,mx,inc,decim),PROP_VECTOR);
    }
	IC TokenValue* 		CreateToken		(PropItemVec& items, LPCSTR key, LPVOID val, xr_token* token, int p_size)
    {
    	return			(TokenValue*)	AppendValue		(items,key,new TokenValue((u32*)val,token,p_size),PROP_TOKEN);
    }
	IC TokenValue2*   	CreateToken2	(PropItemVec& items, LPCSTR key, u32* val, AStringVec* lst)
    {
    	return			(TokenValue2*)	AppendValue		(items,key,new TokenValue2(val,lst),PROP_TOKEN2);
    }
	IC TokenValue3*   	CreateToken3	(PropItemVec& items, LPCSTR key, u32* val, u32 cnt, const TokenValue3::Item* lst)
    {
    	return			(TokenValue3*)	AppendValue		(items,key,new TokenValue3(val,cnt,lst),PROP_TOKEN3);
    }
	IC ListValue* 	 	CreateList		(PropItemVec& items, LPCSTR key, LPSTR val, AStringVec* lst)
    {
    	return			(ListValue*)	AppendValue		(items,key,new ListValue(val,lst),PROP_LIST);
    }
	IC ListValue* 	 	CreateListA		(PropItemVec& items, LPCSTR key, LPSTR val, u32 cnt, LPCSTR* lst)
    {
    	return			(ListValue*)	AppendValue		(items,key,new ListValue(val,cnt,lst),PROP_LIST);
    }
    IC U32Value*  		CreateColor		(PropItemVec& items, LPCSTR key, u32* val)
    {
    	return			(U32Value*)		AppendValue		(items,key,new U32Value(val,0x00000000,0xffffffff,1,0),PROP_COLOR);
    }
    IC ColorValue*		CreateFColor	(PropItemVec& items, LPCSTR key, Fcolor* val)
    {
    	return			(ColorValue*)	AppendValue		(items,key,new ColorValue(val),PROP_FCOLOR);
    }
	IC TextValue* 	   	CreateText		(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim),PROP_TEXT);
    }
	IC ATextValue* 		CreateAText		(PropItemVec& items, LPCSTR key, AnsiString* val)
    {
    	return			(ATextValue*)	AppendValue		(items,key,new ATextValue(val),PROP_A_TEXT);
    }
	IC TextValue* 	 	CreateEShader	(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim),PROP_ESHADER);
    }
	IC TextValue* 	   	CreateCShader	(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim),PROP_CSHADER);
    }
	IC TextValue* 	   	CreateTexture	(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim),PROP_TEXTURE);
    }
	IC TextValue* 	  	CreateTexture2	(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim),PROP_TEXTURE2);
    }
	IC ATextValue* 		CreateAEShader	(PropItemVec& items, LPCSTR key, AnsiString* val)
    {
    	return			(ATextValue*)	AppendValue		(items,key,new ATextValue(val),PROP_A_ESHADER);
    }
	IC ATextValue* 		CreateACShader	(PropItemVec& items, LPCSTR key, AnsiString* val)
    {
    	return			(ATextValue*)	AppendValue		(items,key,new ATextValue(val),PROP_A_CSHADER);
    }
    IC ATextValue*	   	CreateAGameMtl	(PropItemVec& items, LPCSTR key, AnsiString* val)
    {
    	return			(ATextValue*)	AppendValue		(items,key,new ATextValue(val),PROP_A_GAMEMTL);
    }
	IC ATextValue* 		CreateATexture	(PropItemVec& items, LPCSTR key, AnsiString* val)
    {
    	return			(ATextValue*)	AppendValue		(items,key,new ATextValue(val),PROP_A_TEXTURE);
    }
	IC TextValue*	 	CreateLightAnim	(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim),PROP_LIGHTANIM);
    }
	IC TextValue* 	 	CreateLibObject	(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim),PROP_LIBOBJECT);
    }
	IC ATextValue* 		CreateALibObject(PropItemVec& items, LPCSTR key, AnsiString* val)
    {
    	return			(ATextValue*)	AppendValue		(items,key,new ATextValue(val),PROP_A_LIBOBJECT);
    }
	IC TextValue* 	 	CreateGameObject(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim),PROP_GAMEOBJECT);
    }
    IC TextValue*		CreateLibSound	(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim),PROP_LIBSOUND);
    }
    IC ATextValue*	  	CreateALibSound	(PropItemVec& items, LPCSTR key, AnsiString* val)
    {
    	return			(ATextValue*)	AppendValue		(items,key,new ATextValue(val),PROP_A_LIBSOUND);
    }
    IC TextValue*	 	CreateLibPS		(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim),PROP_LIBPS);
    }
    IC ATextValue*	 	CreateALibPS	(PropItemVec& items, LPCSTR key, AnsiString* val)
    {
    	return			(ATextValue*)	AppendValue		(items,key,new ATextValue(val),PROP_A_LIBPS);
    }
	IC TextValue* 		CreateEntity	(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim),PROP_ENTITY);
    }
	IC WaveValue* 		CreateWave		(PropItemVec& items, LPCSTR key, WaveForm* val)
    {
    	return			(WaveValue*)	AppendValue		(items,key,new WaveValue(val),PROP_WAVE);
    }     
    IC TextValue* 		CreateGameMtl	(PropItemVec& items, LPCSTR key, LPSTR val, int lim)
    {
    	return			(TextValue*)	AppendValue		(items,key,new TextValue(val,lim),PROP_GAMEMTL);
    }
};
extern CPropHelper PHelper;
//---------------------------------------------------------------------------
#endif

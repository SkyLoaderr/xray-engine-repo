//----------------------------------------------------
// file: ObjectOptionPack.h
//----------------------------------------------------

#ifndef _INCDEF_ObjectOptionPack_H_
#define _INCDEF_ObjectOptionPack_H_

#include "FileSystem.h"
//----------------------------------------------------

class ObjectOptionPack{
public:

	int m_CollisionList;   // static.vcf
	int m_StaticList;      // meshlist (all static objects)
	int m_DynamicList;     // [mobileobjects] in LTX

	int m_Progressive;   // when writing OGF build collapse map
	int m_RecShadows;    // lighting option
	int m_CastShadows;   // lighting option

	int m_OGF_specular;  // OGF flag
	int m_OGF_light;     // OGF flag
	int m_OGF_alpha;     // OGF flag
	int m_OGF_fog;       // OGF flag
	int m_OGF_ztest;     // OGF flag
	int m_OGF_zwrite;    // OGF flag

	int m_VertexLighting;   // add vertex colors to OGF
	int m_TextureLighting;  // add lightmaps to OGF

public:

	void Copy( const ObjectOptionPack& source ){

		m_CollisionList = source.m_CollisionList;
		m_StaticList = source.m_StaticList;
		m_DynamicList = source.m_DynamicList;
		m_Progressive = source.m_Progressive;
		m_RecShadows = source.m_RecShadows;
		m_CastShadows = source.m_CastShadows;
		m_OGF_specular = source.m_OGF_specular;
		m_OGF_light = source.m_OGF_light;
		m_OGF_alpha = source.m_OGF_alpha;
		m_OGF_fog = source.m_OGF_fog;
		m_OGF_ztest = source.m_OGF_ztest;
		m_OGF_zwrite = source.m_OGF_zwrite;
		m_VertexLighting = source.m_VertexLighting;
		m_TextureLighting = source.m_TextureLighting;
	};

	void Reset(){

		m_CollisionList = true;
		m_StaticList = true;
		m_DynamicList = false;
		m_Progressive = false;
		m_RecShadows = false;
		m_CastShadows = false;
		m_OGF_specular = 1;
		m_OGF_light = 1;
		m_OGF_alpha = 0;
		m_OGF_fog = 1;
		m_OGF_ztest = 1;
		m_OGF_zwrite = 1;
		m_VertexLighting = true;
		m_TextureLighting = false;
	};

	bool ReadScript( XScrBlock *block );
	bool Load( FSChunkDef *chunk );
	bool Save( int handle );

	bool RunEditor( HINSTANCE instance, HWND parent );

	ObjectOptionPack(){
		Reset();
	};

	ObjectOptionPack( const ObjectOptionPack& source ){
		Copy( source );
	};
};


//----------------------------------------------------
#endif /*_INCDEF_ObjectOptionPack_H_*/


//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "GameMtlLib.h"

CGameMtlLibrary GMLib;

#ifdef GM_NON_GAME
SGameMtlPair::~SGameMtlPair		()
{
}
void SGameMtlPair::Load(IReader& fs)
{
	string128			buf;

	R_ASSERT(fs.find_chunk(GAMEMTLPAIR_CHUNK_PAIR));
    mtl0				= fs.r_u32();
    mtl1				= fs.r_u32();
    ID					= fs.r_u32();
    ID_parent			= fs.r_u32();
    OwnProps.set		(fs.r_u32());

    R_ASSERT(fs.find_chunk(GAMEMTLPAIR_CHUNK_FLOTATION));
    fFlotation			= fs.r_float();

    R_ASSERT(fs.find_chunk(GAMEMTLPAIR_CHUNK_BREAKING));
    fs.r_stringZ			(buf); 	BreakingSounds	= buf;

    R_ASSERT(fs.find_chunk(GAMEMTLPAIR_CHUNK_STEP));
    fs.r_stringZ			(buf);	StepSounds		= buf;

    R_ASSERT(fs.find_chunk(GAMEMTLPAIR_CHUNK_COLLIDE));
    fs.r_stringZ			(buf);	CollideSounds	= buf;

    R_ASSERT(fs.find_chunk(GAMEMTLPAIR_CHUNK_HIT));
    fs.r_stringZ			(buf);	HitSounds		= buf;
    fs.r_stringZ			(buf);	HitParticles	= buf;
    fs.r_stringZ			(buf);	HitMarks		= buf;
}
#endif

void SGameMtl::Load(IReader& fs)
{
	R_ASSERT(fs.find_chunk(GAMEMTL_CHUNK_MAIN));
	ID						= fs.r_u32();
	fs.r_stringZ				(name);

	R_ASSERT(fs.find_chunk(GAMEMTL_CHUNK_FLAGS));
    Flags.set				(fs.r_u32());

	R_ASSERT(fs.find_chunk(GAMEMTL_CHUNK_PHYSICS));
    fPHFriction				= fs.r_float();
    fPHDamping				= fs.r_float();
    fPHSpring				= fs.r_float();
    fPHBounceStartVelocity 	= fs.r_float();
    fPHBouncing				= fs.r_float();

	R_ASSERT(fs.find_chunk(GAMEMTL_CHUNK_FACTORS));
    fShootFactor			= fs.r_float();
    fBounceDamageFactor		= fs.r_float();
    fVisTransparencyFactor	= fs.r_float();
}

void CGameMtlLibrary::Load(LPCSTR name)
{
   	R_ASSERT			(material_pairs_rt.empty());
    R_ASSERT			(material_pairs.empty());
    R_ASSERT			(materials.empty());
    		
	destructor<IReader>	F(FS.r_open(name));
    IReader& fs			= F();

    R_ASSERT(fs.find_chunk(GAMEMTLS_CHUNK_VERSION));
    u16 version			= fs.r_u16();
    if (version!=GAMEMTL_CURRENT_VERSION){
        Log				("CGameMtlLibrary: invalid version. Library can't load.");
    	return;
    }

    R_ASSERT(fs.find_chunk(GAMEMTLS_CHUNK_AUTOINC));
    material_index		= fs.r_u32();
    material_pair_index	= fs.r_u32();

    materials.clear		();
    material_pairs.clear();

    IReader* OBJ 		= fs.open_chunk(GAMEMTLS_CHUNK_MTLS);
    if (OBJ){
        IReader* O   	= OBJ->open_chunk(0);
        for (int count=1; O; count++) {
        	SGameMtl* M	= xr_new<SGameMtl> ();
	        M->Load		(*O);
        	materials.push_back(M);
    	    O->close	();
        	O 			= OBJ->open_chunk(count);
        }
        OBJ->close();
    }

    OBJ 				= fs.open_chunk(GAMEMTLS_CHUNK_MTLS_PAIR);
    if (OBJ){
        IReader* O   	= OBJ->open_chunk(0);
        for (int count=1; O; count++) {
        	SGameMtlPair* M	= xr_new<SGameMtlPair> (this);
	        M->Load		(*O);
        	material_pairs.push_back(M);
    	    O->close	();
        	O 			= OBJ->open_chunk(count);
        }
        OBJ->close();
    }

#ifndef _EDITOR
	material_count		= (u32)materials.size();
    material_pairs_rt.resize(material_count*material_count,0);
    for (GameMtlPairIt p_it=material_pairs.begin(); p_it!=material_pairs.end(); p_it++){
    	SGameMtlPair* S	= *p_it;
    	int idx0		= GetMaterialIdx(S->mtl0)*material_count+GetMaterialIdx(S->mtl1);
    	int idx1		= GetMaterialIdx(S->mtl1)*material_count+GetMaterialIdx(S->mtl0);
	    material_pairs_rt[idx0]=S;
	    material_pairs_rt[idx1]=S;
    }
#endif
}


//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "GameMtlLib.h"

CGameMtlLibrary GMLib;

void SGameMtl::Load(CStream& fs)
{
	R_ASSERT(fs.FindChunk(GAMEMTL_CHUNK_MAIN));
	ID						= fs.Rdword();
	fs.RstringZ				(name);

	R_ASSERT(fs.FindChunk(GAMEMTL_CHUNK_FLAGS));
    Flags.m_Flags			= fs.Rdword();

	R_ASSERT(fs.FindChunk(GAMEMTL_CHUNK_PHYSICS));
    fPHFriction				= fs.Rfloat();
    fPHDumping				= fs.Rfloat();
    fPHSpring				= fs.Rfloat();
    fPHBounceStartVelocity 	= fs.Rfloat();
    fPHBouncing				= fs.Rfloat();

	R_ASSERT(fs.FindChunk(GAMEMTL_CHUNK_FACTORS));
    fShootFactor			= fs.Rfloat();
    fBounceDamageFactor		= fs.Rfloat();
    fVisTransparencyFactor	= fs.Rfloat();
}

void CGameMtlLibrary::Load(LPCSTR name)
{
    CFileStream	fs(name);

    R_ASSERT(fs.FindChunk(GAMEMTLS_CHUNK_VERSION));
    u16 version			= fs.Rword();
    if (version!=GAMEMTL_CURRENT_VERSION){
        Log				("CGameMtlLibrary: invalid version. Library can't load.");
    	return;
    }
    
    R_ASSERT(fs.FindChunk(GAMEMTLS_CHUNK_AUTOINC));
    material_index		= fs.Rdword();
    material_pair_index	= fs.Rdword();

    CStream* OBJ 		= fs.OpenChunk(GAMEMTLS_CHUNK_MTLS);
    if (OBJ){
        CStream* O   	= OBJ->OpenChunk(0);
        for (int count=1; O; count++) {
        	SGameMtl* M	= new SGameMtl();
	        M->Load		(*O);
        	materials.push_back(M);
    	    O->Close	();
        	O 			= OBJ->OpenChunk(count);
        }
        OBJ->Close();
    }

    OBJ 				= fs.OpenChunk(GAMEMTLS_CHUNK_MTLS_PAIR); R_ASSERT(OBJ);
    if (OBJ){
        CStream* O   	= OBJ->OpenChunk(0);
        for (int count=1; O; count++) {
        	SGameMtlPair* M	= new SGameMtlPair();
	        M->Load		(*O);
        	material_pairs.push_back(M);
    	    O->Close	();
        	O 			= OBJ->OpenChunk(count);
        }
        OBJ->Close();
    }

#ifndef _EDITOR    
	material_count		= materials.size();
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


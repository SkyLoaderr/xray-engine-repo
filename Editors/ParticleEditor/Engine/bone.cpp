#include "stdafx.h"
#pragma hdrstop

#include "bone.h"
#include "envelope.h"

#define BONE_VERSION			0x0001
//------------------------------------------------------------------------------
#define BONE_CHUNK_VERSION		0x0001
#define BONE_CHUNK_DEF			0x0002
#define BONE_CHUNK_BIND_POSE	0x0003
#define BONE_CHUNK_MATERIAL		0x0004
#define BONE_CHUNK_SHAPE		0x0005
#define BONE_CHUNK_IK_JOINT		0x0006
#define BONE_CHUNK_MASS			0x0007

CBone::CBone()
{
	flags.zero	();
    name[0]		= 0;
    parent[0]	= 0;
    wmap[0]		= 0;
    rest_length	= 0;
    ResetData	();
}

CBone::~CBone()
{
}

void CBone::ResetData()
{
    IK_data.Reset	();
    strcpy			(game_mtl,"default");
    shape.Reset		();

    mass			= 10.f;;
    center_of_mass.set(0.f,0.f,0.f);
}

void CBone::Save(IWriter& F)
{
#ifdef _LW_EXPORT
	extern char* ReplaceSpace(char* s);
	ReplaceSpace(name);		strlwr(name);
	ReplaceSpace(parent);	strlwr(parent);
#endif
	F.open_chunk	(BONE_CHUNK_VERSION);
    F.w_u16			(BONE_VERSION);
    F.close_chunk	();
    
	F.open_chunk	(BONE_CHUNK_DEF);
	F.w_stringZ		(name);
	F.w_stringZ		(parent);
	F.w_stringZ		(wmap);
    F.close_chunk	();

	F.open_chunk	(BONE_CHUNK_BIND_POSE);
	F.w_fvector3	(rest_offset);
	F.w_fvector3	(rest_rotate);
	F.w_float		(rest_length);
    F.close_chunk	();

    SaveData		(F);
}

void CBone::Load_0(IReader& F)
{
	F.r_stringZ	(name);
	F.r_stringZ	(parent);
	F.r_stringZ	(wmap);
	F.r_fvector3(rest_offset);
	F.r_fvector3(rest_rotate);
	rest_length	= F.r_float();
    Reset		();
}

void CBone::Load_1(IReader& F)
{
	R_ASSERT(F.find_chunk(BONE_CHUNK_VERSION));
	u16	ver			= F.r_u16();

    if (ver!=BONE_VERSION)
    	return;
    
	R_ASSERT(F.find_chunk(BONE_CHUNK_DEF));
	F.r_stringZ		(name);
	F.r_stringZ		(parent);
	F.r_stringZ		(wmap);

	R_ASSERT(F.find_chunk(BONE_CHUNK_BIND_POSE));
	F.r_fvector3	(rest_offset);
	F.r_fvector3	(rest_rotate);
	rest_length		= F.r_float();

    LoadData		(F);
}

void CBone::SaveData(IWriter& F)
{
	F.open_chunk	(BONE_CHUNK_DEF);
	F.w_stringZ		(name);
    F.close_chunk	();

	F.open_chunk	(BONE_CHUNK_MATERIAL);
    F.w_stringZ		(game_mtl);
    F.close_chunk	();

	F.open_chunk	(BONE_CHUNK_SHAPE);
    F.w				(&shape,sizeof(SBoneShape));
    F.close_chunk	();
    
	F.open_chunk	(BONE_CHUNK_IK_JOINT);
	F.w_u32			(IK_data.type);
    F.w				(IK_data.limits,sizeof(SJointLimit)*3);
    F.w_float		(IK_data.spring_factor);
    F.w_float		(IK_data.damping_factor);
    F.close_chunk	();

    F.open_chunk	(BONE_CHUNK_MASS);
    F.w_float		(mass);
	F.w_fvector3	(center_of_mass);
    F.close_chunk	();
}

void CBone::LoadData(IReader& F)
{
	R_ASSERT(F.find_chunk(BONE_CHUNK_DEF));
	F.r_stringZ		(name);

	R_ASSERT(F.find_chunk(BONE_CHUNK_MATERIAL));
    F.r_stringZ		(game_mtl);

	R_ASSERT(F.find_chunk(BONE_CHUNK_SHAPE));
    F.r				(&shape,sizeof(SBoneShape));
    
	R_ASSERT(F.find_chunk(BONE_CHUNK_IK_JOINT));
	IK_data.type			= (EJointType)F.r_u32();
    F.r						(IK_data.limits,sizeof(SJointLimit)*3);
    IK_data.spring_factor	= F.r_float();
    IK_data.damping_factor	= F.r_float();

    if (F.find_chunk(BONE_CHUNK_MASS)){
	    mass		= F.r_float();
		F.r_fvector3(center_of_mass);
    }
}

void CBone::CopyData(CBone* bone)
{
    strcpy			(game_mtl,bone->game_mtl);
    shape			= bone->shape;
	IK_data			= bone->IK_data;
    mass			= bone->mass;
    center_of_mass	= bone->center_of_mass;
}


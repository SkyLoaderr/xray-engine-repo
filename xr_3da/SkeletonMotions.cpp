//---------------------------------------------------------------------------
#include 	"stdafx.h"
#pragma hdrstop

#include 	"SkeletonMotions.h"
#include 	"SkeletonAnimated.h"
#include	"fmesh.h"
#include	"motion.h"

const float			fAA					= 1.5f;	// anim-change acceleration
motions_container*	g_pMotionsContainer	= 0;

u16 find_bone_id(vecBones* bones, shared_str nm)
{
	for (u16 i=0; i<bones->size(); i++)
		if (bones->at(i)->name==nm) return i;
	return BI_NONE;
}

//-----------------------------------------------------------------------
BOOL motions_value::load		(LPCSTR N, IReader *data, vecBones* bones)
{
	bool bRes		= true;
	// Load definitions
	U16Vec rm_bones	(bones->size(),BI_NONE);
	IReader* MP 	= data->open_chunk(OGF_S_SMPARAMS);
	if (MP){
		u16 		vers 			= MP->r_u16();
		u16 		part_bone_cnt	= 0;
		string128 	buf;
		R_ASSERT3(vers==xrOGF_SMParamsVersion,"Invalid OGF/OMF version:",N);
		// partitions
		u16 part_count;
		part_count 				= MP->r_u16();
		for (u16 part_i=0; part_i<part_count; part_i++){
			CPartDef&	PART	= m_partition[part_i];
			MP->r_stringZ		(buf);
			PART.Name			= _strlwr(buf);
			PART.bones.resize	(MP->r_u16());
//			Log					("Part:",buf);
			for (xr_vector<u32>::iterator b_it=PART.bones.begin(); b_it<PART.bones.end(); b_it++){
				MP->r_stringZ	(buf);
				u16 m_idx 		= u16			(MP->r_u32());
				*b_it			= find_bone_id	(bones,buf); 
//				Msg				("Bone: #%2d, ID: %2d, Name: '%s'",b_it-PART.bones.begin(),*b_it,buf);
#ifdef _EDITOR
				if (*b_it==BI_NONE){
					bRes		= false;
					Msg			("!Can't find bone: '%s'",buf);
				}
#else
				VERIFY3			(*b_it!=BI_NONE,"Can't find bone:",buf);
#endif
				if (bRes)		rm_bones[m_idx] = u16(*b_it);
			}
			part_bone_cnt		= part_bone_cnt + (u16)PART.bones.size();
		}

#ifdef _EDITOR
		if (part_bone_cnt!=(u16)bone_count){
			bRes = false;
			Msg("!Different bone count [Object: '%d' <-> Motions: '%d']",bone_count,part_bone_cnt);
		}
#else
		VERIFY3(part_bone_cnt==(u16)bones->size(),"Different bone count '%s'",N);
#endif
		if (bRes){
			// motion defs (cycle&fx)
			u16 mot_count			= MP->r_u16();
			for (u16 mot_i=0; mot_i<mot_count; mot_i++){
				MP->r_stringZ		(buf);
				shared_str nm		= _strlwr		(buf);
				u32 dwFlags			= MP->r_u32		();
				CMotionDef	D;		D.Load			(MP,dwFlags);
				if (dwFlags&esmFX)	m_fx.insert		(mk_pair(nm,D));
				else				m_cycle.insert	(mk_pair(nm,D));
			}
		}
		MP->close();
	}else{
		Debug.fatal	("Old skinned model version unsupported! (%s)",N);
	}

	if (!bRes)	return false;

	// Load animation
	IReader*	MS		= data->open_chunk(OGF_S_MOTIONS);
	if (!MS) 	return false;

	u32			dwCNT	= 0;
	MS->r_chunk_safe	(0,&dwCNT,sizeof(dwCNT));

	// set per bone motion size
	for (u32 i=0; i<bones->size(); i++)
		m_motions[bones->at(i)->name].resize(dwCNT);
	// load motions
	for (u32 m_idx=0; m_idx<dwCNT; m_idx++){
		string128			mname;
		R_ASSERT			(MS->find_chunk(m_idx+1));             
		MS->r_stringZ		(mname);

		shared_str	m_key	= shared_str(strlwr(mname));
		m_motion_map.insert	(mk_pair(m_key,m_idx));

		u32 dwLen			= MS->r_u32();
		for (u32 i=0; i<bones->size(); i++){
			VERIFY2			(rm_bones[i]!=BI_NONE,"Invalid remap index.");
			CMotion&		M	= m_motions[bones->at(rm_bones[i])->name][m_idx];
			M._count			= dwLen;
			u8	t_present		= MS->r_u8	();
			u32 crc_q			= MS->r_u32	();
			M._keysR.create		(crc_q,dwLen,(CKeyQR*)MS->pointer());
			MS->advance			(dwLen * sizeof(CKeyQR));
			if (t_present)	{
				u32 crc_t			= MS->r_u32	();
				M._keysT.create		(crc_t,dwLen,(CKeyQT*)MS->pointer());
				MS->advance			(dwLen * sizeof(CKeyQT));
				MS->r_fvector3		(M._sizeT);
				MS->r_fvector3		(M._initT);
			}else{
				MS->r_fvector3		(M._initT);
			}
		}
	}
	MS->close();
	return bRes;
}
MotionVec* motions_value::motions(shared_str bone_name)
{
	BoneMotionMapIt it			= m_motions.find(bone_name); VERIFY(it!=m_motions.end());
	return &it->second;
}
//-----------------------------------
motions_container::motions_container()
{
}
motions_container::~motions_container()
{
//	clean	(false);
//	clean	(true);
//	dump	();
	VERIFY	(container.empty());
}
motions_value* motions_container::dock(shared_str key, IReader *data, vecBones* bones)
{
	motions_value*	result		= 0	;
	SharedMotionsMapIt	I		= container.find	(key);
	if (I!=container.end())		result = I->second;
	if (0==result)				{
		// loading motions
		result					= xr_new<motions_value>();
		result->m_dwReference	= 0;
		BOOL bres				= result->load	(key.c_str(),data,bones);
		if (bres)				container.insert(mk_pair(key,result));
		else					xr_delete		(result);
	}
	return result;
}
void motions_container::clean(bool force_destroy)
{
	SharedMotionsMapIt it	= container.begin();
	SharedMotionsMapIt _E	= container.end();
	if (force_destroy){
		for (; it!=_E; it++){
			motions_value*	sv = it->second;
			xr_delete		(sv);
		}
		container.clear		();
	}else{
		for (; it!=_E; )	{
			motions_value*	sv = it->second;
			if (0==sv->m_dwReference)	
			{
				SharedMotionsMapIt	i_current	= it;
				SharedMotionsMapIt	i_next		= ++it;
				xr_delete			(sv);
				container.erase		(i_current);
				it					= i_next;
			} else {
				it++;
			}
		}
	}
}
void motions_container::dump()
{
	SharedMotionsMapIt it	= container.begin();
	SharedMotionsMapIt _E	= container.end();
	Log	("--- motion container --- begin:");
	u32 sz					= sizeof(*this);
	for (u32 k=0; it!=_E; k++,it++){
		sz					+= it->second->mem_usage();
		Msg("#%3d: [%3d/%5d Kb] - %s",k,it->second->m_dwReference,it->second->mem_usage()/1024,it->first.c_str());
	}
	Msg ("--- items: %d, mem usage: %d Kb ",container.size(),sz/1024);
	Log	("--- motion container --- end.");
}

//////////////////////////////////////////////////////////////////////////
// High level control
void CMotionDef::Load(IReader* MP, u32 fl)
{
	// params
	bone_or_part= MP->r_u16(); // bCycle?part_id:bone_id;
	motion		= MP->r_u16(); // motion_id
	speed		= Quantize(MP->r_float());
	power		= Quantize(MP->r_float());
	accrue		= Quantize(MP->r_float());
	falloff		= Quantize(MP->r_float());
	flags		= fl;
	if (!(flags&esmFX) && (falloff>=accrue)) falloff = accrue-1;
}

CBlend*	CMotionDef::PlayCycle(CSkeletonAnimated* P, BOOL bMixIn, PlayCallback Callback, LPVOID Callback_Param)
{
	return P->LL_PlayCycle(
		bone_or_part,motion,bMixIn,
		fAA*Dequantize(accrue),
		fAA*Dequantize(falloff),
		Dequantize(speed),
		flags&esmStopAtEnd,
		Callback,Callback_Param
		);
}

CBlend*	CMotionDef::PlayCycle(CSkeletonAnimated* P, u16 part, BOOL bMixIn, PlayCallback Callback, LPVOID Callback_Param)
{
	return P->LL_PlayCycle(
		part,motion,bMixIn,
		fAA*Dequantize(accrue),
		fAA*Dequantize(falloff),
		Dequantize(speed),
		flags&esmStopAtEnd,
		Callback,Callback_Param
		);
}

CBlend*	CMotionDef::PlayFX(CSkeletonAnimated* P, float power_scale)
{
	return P->LL_PlayFX(
		bone_or_part,motion,
		fAA*Dequantize(accrue),
		fAA*Dequantize(falloff),
		Dequantize(speed),
		Dequantize(power)*power_scale
		);
}

#include "stdafx.h"
#pragma hdrstop

#include "motion.h"
#include "envelope.h"

#ifdef LWO_EXPORTS
	#include "fs.h"
#else 
#ifdef _EDITOR
	#include "fs.h"
#else
	#include "..\fs.h"
#endif
#endif

#define EOBJ_OMOTION   			0x1100
#define EOBJ_SMOTION   			0x1200
#define EOBJ_OMOTION_VERSION   	0x0003
#define EOBJ_SMOTION_VERSION   	0x0003

#ifdef LWO_EXPORTS
	extern char* ReplaceSpace(char* s);
#endif
//------------------------------------------------------------------------------------------
// CCustomMotion
//------------------------------------------------------------------------------------------
CCustomMotion::CCustomMotion(){
	name[0]			=0;
    iFrameStart		=0;
    iFrameEnd		=0;
    fFPS			=30.f;
}

CCustomMotion::CCustomMotion(CCustomMotion* source){
	*this			= *source;
}

CCustomMotion::~CCustomMotion(){
}

void CCustomMotion::Save(CFS_Base& F){
#ifdef LWO_EXPORTS
	ReplaceSpace(name);			strlwr(name);
#endif
	F.WstringZ	(name);
	F.Wdword	(iFrameStart);	
	F.Wdword	(iFrameEnd);
	F.Wfloat	(fFPS);
}

bool CCustomMotion::Load(CStream& F){
	F.RstringZ	(name);
	iFrameStart	= F.Rdword();	
	iFrameEnd	= F.Rdword();
	fFPS		= F.Rfloat();
	return true;
}

//------------------------------------------------------------------------------------------
// Object Motion
//------------------------------------------------------------------------------------------
COMotion::COMotion():CCustomMotion(){
	mtype			=mtObject;
	ZeroMemory		(envs,sizeof(CEnvelope*)*ctMaxChannel);
}

COMotion::COMotion(COMotion* source):CCustomMotion(source){
	// bone motions
	for (int ch=0; ch<ctMaxChannel; ch++)
		envs[ch]	= new CEnvelope(envs[ch]);
}

COMotion::~COMotion(){
	for (int ch=0; ch<ctMaxChannel; ch++) _DELETE(envs[ch]);
}

void COMotion::Evaluate(float t, Fvector& T, Fvector& R){
	T.x = envs[ctPositionX]->Evaluate(t);
	T.y = envs[ctPositionY]->Evaluate(t);
	T.z = envs[ctPositionZ]->Evaluate(t);

	R.x = envs[ctRotationH]->Evaluate(t);
	R.y = envs[ctRotationP]->Evaluate(t);
	R.z = envs[ctRotationB]->Evaluate(t);
}

void COMotion::SaveMotion(const char* buf){
	CFS_Memory		F;
	F.open_chunk	(EOBJ_OMOTION);
	Save			(F);
	F.close_chunk	();
	F.SaveTo		(buf,0);
}

bool COMotion::LoadMotion(const char* buf){
	CFileStream		F(buf);
	R_ASSERT(F.FindChunk(EOBJ_OMOTION));
	return Load		(F);
}

void COMotion::Save(CFS_Base& F){
	CCustomMotion::Save(F);
	F.Wword		(EOBJ_OMOTION_VERSION);
	for (int ch=0; ch<ctMaxChannel; ch++)
		envs[ch]->Save(F);
}

bool COMotion::Load(CStream& F){
	CCustomMotion::Load(F);
	WORD vers	= F.Rword();
	if (vers!=EOBJ_OMOTION_VERSION) return false;
	for (int ch=0; ch<ctMaxChannel; ch++){
		envs[ch] = new CEnvelope();
		envs[ch]->Load(F);
	}
	return true;
}



//------------------------------------------------------------------------------------------
// Skeleton Motion
//------------------------------------------------------------------------------------------
CSMotion::CSMotion():CCustomMotion(){
	mtype			=mtSkeleton;
    cStartBone[0] 	=0;
	cBonePart[0]	=0;
    bFX				=FALSE;
	bStopAtEnd		=FALSE;
    fSpeed			=1.0f;
    fAccrue			=2.0f;
    fFalloff		=2.0f;
    fPower			=1.f;
}

CSMotion::CSMotion(CSMotion* source):CCustomMotion(source){
	// bone motions
	st_BoneMotion*	src;
	st_BoneMotion*	dest;
    for(DWORD i=0; i<bone_mots.size(); i++){
    	dest 		= &bone_mots[i];
    	src 		= &source->bone_mots[i];
		for (int ch=0; ch<ctMaxChannel; ch++)
			dest->envs[ch] = new CEnvelope(src->envs[ch]);
    }
}

CSMotion::~CSMotion(){
	Clear();
}

void CSMotion::Clear(){
	for(BoneMotionIt bm_it=bone_mots.begin(); bm_it!=bone_mots.end(); bm_it++)
		for (int ch=0; ch<ctMaxChannel; ch++) _DELETE(bm_it->envs[ch]);
	bone_mots.clear();
}

void CSMotion::CopyMotion(CSMotion* source){
	Clear();

	iFrameStart		= source->iFrameStart;
    iFrameEnd		= source->iFrameEnd;
	fFPS			= source->fFPS;
	st_BoneMotion*	src;
	st_BoneMotion*	dest;
    bone_mots.resize(source->bone_mots.size());
    for(DWORD i=0; i<bone_mots.size(); i++){
    	dest 		= &bone_mots[i];
    	src 		= &source->bone_mots[i];
		for (int ch=0; ch<ctMaxChannel; ch++)
			dest->envs[ch] = new CEnvelope(src->envs[ch]);
    }
}

void CSMotion::Evaluate(int bone_idx, float t, Fvector& T, Fvector& R){
	VERIFY(bone_idx<(int)bone_mots.size());
	CEnvelope** envs = bone_mots[bone_idx].envs;
	T.x = envs[ctPositionX]->Evaluate(t);
	T.y = envs[ctPositionY]->Evaluate(t);
	T.z = envs[ctPositionZ]->Evaluate(t);

	R.x = envs[ctRotationH]->Evaluate(t);
	R.y = envs[ctRotationP]->Evaluate(t);
	R.z = envs[ctRotationB]->Evaluate(t);
}

void CSMotion::SaveMotion(const char* buf){
	CFS_Memory		F;
	F.open_chunk	(EOBJ_SMOTION);
	Save			(F);
	F.close_chunk	();
	F.SaveTo		(buf,0);
}

bool CSMotion::LoadMotion(const char* buf){
	CFileStream		F(buf);
	R_ASSERT(F.FindChunk(EOBJ_SMOTION));
	return Load		(F);
}

void CSMotion::Save(CFS_Base& F){
	CCustomMotion::Save(F);
#ifdef LWO_EXPORTS
	ReplaceSpace(cStartBone);	strlwr(cStartBone);
	ReplaceSpace(cBonePart);	strlwr(cBonePart);
#endif
	F.Wword		(EOBJ_SMOTION_VERSION);
	F.WstringZ	(cStartBone);
	F.WstringZ	(cBonePart);
    F.Wbyte		(bFX);
    F.Wbyte		(bStopAtEnd);
    F.Wfloat	(fSpeed);
    F.Wfloat	(fAccrue);
    F.Wfloat	(fFalloff);
    F.Wfloat	(fPower);
	F.Wdword	(bone_mots.size()); 
	for(BoneMotionIt bm_it=bone_mots.begin(); bm_it!=bone_mots.end(); bm_it++){
	#ifdef LWO_EXPORTS
		ReplaceSpace(bm_it->name);
	#endif
		F.Wdword(bm_it->flag);
		F.WstringZ(bm_it->name);
		for (int ch=0; ch<ctMaxChannel; ch++)
			bm_it->envs[ch]->Save(F);
	}
}

bool CSMotion::Load(CStream& F){
	CCustomMotion::Load(F);
	WORD vers	= F.Rword();
	if (vers!=EOBJ_SMOTION_VERSION) return false;
	F.RstringZ	(cStartBone);
	F.RstringZ	(cBonePart);
    bFX			= F.Rbyte();
    bStopAtEnd	= F.Rbyte();
    fSpeed		= F.Rfloat();
    fAccrue		= F.Rfloat();
    fFalloff	= F.Rfloat();
    fPower		= F.Rfloat();
	bone_mots.resize(F.Rdword());
	for(BoneMotionIt bm_it=bone_mots.begin(); bm_it!=bone_mots.end(); bm_it++){
		bm_it->flag = F.Rdword();
		F.RstringZ(bm_it->name);
		for (int ch=0; ch<ctMaxChannel; ch++){
			bm_it->envs[ch] = new CEnvelope();
			bm_it->envs[ch]->Load(F);
		}
	}
	return true;
}


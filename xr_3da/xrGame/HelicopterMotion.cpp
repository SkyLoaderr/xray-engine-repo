#include "stdafx.h"
#include "HelicopterMotion.h"
#include "../envelope.h"      


CHelicopterMotion::CHelicopterMotion()
{
//	mtype			=mtObject;
	for (int ch=0; ch<ctMaxChannel; ch++)
		envs[ch]	= xr_new<CEnvelope> ();
	
	fFPS			= 30.0f;
	
	m_startTime		= 0.0f;
	m_endTime		= 0.0f;

#ifdef DEBUG
	Device.seqRender.Add(this,REG_PRIORITY_LOW-1);
#endif
}


CHelicopterMotion::~CHelicopterMotion()
{
	Clear			();

#ifdef DEBUG
	Device.seqRender.Remove(this);
#endif
}

void CHelicopterMotion::Clear()
{
	for (int ch=0; ch<ctMaxChannel; ch++) xr_delete(envs[ch]);
}

void CHelicopterMotion::_Evaluate(float t, Fvector& T, Fvector& R)
{
	T.x = envs[ctPositionX]->Evaluate(t);
	T.y = envs[ctPositionY]->Evaluate(t);
	T.z = envs[ctPositionZ]->Evaluate(t);

	R.y = envs[ctRotationH]->Evaluate(t);
	R.x = envs[ctRotationP]->Evaluate(t);
	R.z = envs[ctRotationB]->Evaluate(t);
}

void CHelicopterMotion::CreateKey(float t, const Fvector& P, const Fvector& R)
{
	envs[ctPositionX]->InsertKey(t,P.x);
	envs[ctPositionY]->InsertKey(t,P.y);
	envs[ctPositionZ]->InsertKey(t,P.z);
	envs[ctRotationH]->InsertKey(t,R.y);
	envs[ctRotationP]->InsertKey(t,R.x);
	envs[ctRotationB]->InsertKey(t,R.z);
}

void CHelicopterMotion::DeleteKey(float t)
{
	envs[ctPositionX]->DeleteKey(t);
	envs[ctPositionY]->DeleteKey(t);
	envs[ctPositionZ]->DeleteKey(t);
	envs[ctRotationH]->DeleteKey(t);
	envs[ctRotationP]->DeleteKey(t);
	envs[ctRotationB]->DeleteKey(t);
}

int CHelicopterMotion::KeyCount()
{
	return envs[ctPositionX]->keys.size();
}

void CHelicopterMotion::FindNearestKey(float	t, 
									   float&	min_t, 
									   float&	max_t, 
									   u32&		min_idx, 
									   u32&		max_idx, 
									   float	eps)
{
	KeyIt min_k;
	KeyIt max_k;

	envs[ctPositionX]->FindNearestKey(t, min_k, max_k, eps);

	min_t =		(min_k!=envs[ctPositionX]->keys.end())?(*min_k)->time:t; 
	max_t =		(max_k!=envs[ctPositionX]->keys.end())?(*max_k)->time:t; 

	max_idx =	(u32)std::distance(envs[ctPositionX]->keys.begin(), max_k );
	min_idx =	(u32)std::distance(envs[ctPositionX]->keys.begin(), min_k );
}

float CHelicopterMotion::GetLength(float* mn, float* mx)
{
	float ln,len=0.f; 
	for (int ch=0; ch<ctMaxChannel; ch++)
		if ((ln=envs[ch]->GetLength(mn,mx))>len) len=ln;
	
	return len;
}

BOOL CHelicopterMotion::ScaleKeys(float from_time, float to_time, float scale_factor)
{
	BOOL bRes=TRUE;
	for (int ch=0; ch<ctMaxChannel; ch++)
		if (FALSE==(bRes=envs[ch]->ScaleKeys(from_time, to_time, scale_factor, 1.f/fFPS))) break;
	
	return bRes;
}

BOOL CHelicopterMotion::NormalizeKeys(float from_time, float to_time, float speed)
{
	CEnvelope* E = Envelope(ctPositionX);
	float new_tm	= 0;
	float t0		= E->keys.front()->time;
	FloatVec tms;
	tms.push_back	(t0);
	for (KeyIt it=E->keys.begin()+1; it!=E->keys.end(); it++){
		if (((*it)->time>from_time)&&((*it)->time<to_time)){
			float dist	= 0;
			Fvector PT,T,R;
			_Evaluate	(t0, PT, R);
			for (float tm=t0+1.f/fFPS; tm<=(*it)->time; tm+=EPS_L){
				_Evaluate	(tm, T, R);
				dist		+= PT.distance_to(T);
				PT.set		(T);
			}
			new_tm			+= dist / speed;
			t0				= (*it)->time;
			tms.push_back	(new_tm);
		}
	}
	for (int ch=0; ch<ctMaxChannel; ch++){
		E				= Envelope(EChannelType(ch));
		FloatIt	f_it	= tms.begin();
		for (KeyIt k_it=E->keys.begin(); k_it!=E->keys.end(); k_it++,f_it++)
			if (((*k_it)->time>from_time)&&((*k_it)->time<to_time))
				(*k_it)->time = *f_it;
	}
	return FALSE;
}

#ifdef DEBUG
void CHelicopterMotion::DrawPath(bool bDrawInterpolated, bool bDrawKeys, float dTime)
{
	if(bDrawInterpolated){
	
	FvectorVec path_points;
	FvectorVec path_rot;

	float min_t				= m_startTime;
	float max_t				= m_endTime;

	Fvector 				T,r;
	path_points.clear		();
	for (float t=min_t; (t<max_t)||fsimilar(t,max_t,EPS_L); t+=dTime){
		_Evaluate(t,T,r);
		path_points.push_back(T);
		path_rot.push_back(r);
	}

	float path_box_size = .05f;
	for(u32 i = 0; i<path_points.size (); ++i) {
		RCache.dbg_DrawAABB  (path_points[i],path_box_size,path_box_size,path_box_size,D3DCOLOR_XRGB(0,255,0));
	}	
/*		r.setHP(path_rot[i].y, path_rot[i].x );
		r.mul (3.0f);
		T.add (path_points[i],r);
		RCache.dbg_DrawLINE (Fidentity, path_points[i], T, D3DCOLOR_XRGB(255,0,0));
*/
	}

	if(bDrawKeys){
		float key_box_size = .25f;
		u32 cnt = KeyCount();
		for(u32 ii=0;ii<cnt;++ii) {
			Fvector _t, TT;
			Fvector _r;
			GetKey (ii,_t,_r);
			RCache.dbg_DrawAABB  (_t,key_box_size,key_box_size,key_box_size,D3DCOLOR_XRGB(0,255,255));

//			_r.setHP(_r.y, _r.x );
//			_r.mul (6.0f);
//			TT.add (_t,_r);
//			RCache.dbg_DrawLINE (Fidentity, _t, TT, D3DCOLOR_XRGB(255,0,0));
		}
	}

}
void CHelicopterMotion::OnRender()
{
	DrawPath (true, true);
}
#endif

void CHelicopterMotion::DropTailKeys(u32 cnt)
{
	VERIFY(KeyCount()>(int)cnt+2);

	CEnvelope*	env = Envelope();
	for(u32 i=0; i<cnt; ++i) {
		DeleteKey(env->keys.front()->time);
	};
	GetKeyTime(0, m_startTime);
}

void CHelicopterMotion::DropHeadKeys(u32 cnt)
{
//	VERIFY(KeyCount()>(int)cnt+2);

	CEnvelope*	env = Envelope();
	for(u32 i=0; i<cnt; ++i) {
		DeleteKey(env->keys.back()->time);
	};

//	VERIFY(KeyCount()>4);
	if(KeyCount())
		GetKeyTime(KeyCount()-1, m_endTime);
	else
		m_endTime = 0.0f;
}

void CHelicopterMotion::GetKey(u32 idx, Fvector& T, Fvector& R)
{
	GetKeyT (idx, T);
	GetKeyR (idx, R);
}

void CHelicopterMotion::GetKeyT(u32 idx, Fvector& T)
{
	T.x		= envs[ctPositionX]->keys[idx]->value;
	T.y		= envs[ctPositionY]->keys[idx]->value;
	T.z		= envs[ctPositionZ]->keys[idx]->value;
}

void CHelicopterMotion::GetKeyR(u32 idx, Fvector& R)
{
	R.y		= envs[ctRotationH]->keys[idx]->value;
	R.x		= envs[ctRotationP]->keys[idx]->value;
	R.z		= envs[ctRotationB]->keys[idx]->value;
}

void CHelicopterMotion::GetKeyTime		(u32 idx, float& time)
{
	time	= envs[ctPositionX]->keys[idx]->time; 
}

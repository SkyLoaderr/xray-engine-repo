#include "stdafx.h"
#pragma hdrstop

#include "ObjectAnimator.h"
#include "motion.h"
 
bool motion_sort_pred	(COMotion* a, 	COMotion* b)	{	return xr_strcmp(a->Name(),b->Name())<0;}
bool motion_find_pred	(COMotion* a, 	LPCSTR b) 		{	return xr_strcmp(a->Name(),b)<0;}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CObjectAnimator::CObjectAnimator()
{
	bLoop			= false;
    m_Current		= 0;
}

CObjectAnimator::~CObjectAnimator()
{	
	Clear				();
}

void CObjectAnimator::Clear()
{
	for(MotionIt m_it=m_Motions.begin(); m_it!=m_Motions.end(); m_it++)
		xr_delete		(*m_it);
	m_Motions.clear		();
}

void CObjectAnimator::SetActiveMotion(COMotion* mot)
{
	m_Current			= mot;
    if (m_Current) 		m_MParam.Set(m_Current);
	m_XFORM.identity	();
}

void CObjectAnimator::LoadMotions(const char* fname)
{
	string256			full_path;
	if (!FS.exist( full_path, "$level$", fname ))
		if (!FS.exist( full_path, "$game_anims$", fname ))
			Debug.fatal("Can't find motion file '%s'.",fname);
            
    LPCSTR  ext			= strext(full_path);
    if (ext){
		Clear			();
    	if (0==xr_strcmp(ext,".anm")){
            COMotion* M	= xr_new<COMotion> ();
            if (M->LoadMotion(full_path)) m_Motions.push_back(M);
            else				Debug.fatal("ERROR: Can't load motion. Incorrect file version.");
        }else if (0==xr_strcmp(ext,".anms")){
            IReader* F			= FS.r_open(full_path);
            u32 dwMCnt			= F->r_u32(); VERIFY(dwMCnt);
            for (u32 i=0; i<dwMCnt; i++){
                COMotion* M		= xr_new<COMotion> ();
                bool bRes		= M->Load(*F);
                if (!bRes)		Debug.fatal("ERROR: Can't load motion. Incorrect file version.");
                m_Motions.push_back(M);
            }
            FS.r_close		(F);
        }
        std::sort(m_Motions.begin(),m_Motions.end(),motion_sort_pred);
    }
}

void CObjectAnimator::Load(const char * name)
{
	LoadMotions			(name); 
	SetActiveMotion		(0);
}

void CObjectAnimator::OnFrame()
{
	if (m_Current){
		Fvector R,P;
		m_Current->_Evaluate(m_MParam.Frame(),P,R);
		m_MParam.Update	(Device.fTimeDelta,1.f,bLoop);
		m_XFORM.setXYZi	(R.x,R.y,R.z);
        m_XFORM.translate_over(P);
	}
}

COMotion* CObjectAnimator::Play(bool loop, LPCSTR name)
{
	if (name&&name[0]){
		MotionIt it = std::lower_bound(m_Motions.begin(),m_Motions.end(),name,motion_find_pred);
        if ((it!=m_Motions.end())&&(0==xr_strcmp((*it)->Name(),name))){
            bLoop 		= loop;
            SetActiveMotion(*it);
			m_MParam.Play	();
            return 		*it;
        }else{
            Debug.fatal	("OBJ ANIM::Cycle '%s' not found.",name);
            return NULL;
        }
    }else{
        if (!m_Motions.empty()){
            bLoop 		= loop;
            SetActiveMotion(m_Motions.front());
			m_MParam.Play	();
            return 		m_Motions.front();
        }else{
            Debug.fatal	("OBJ ANIM::Cycle '%s' not found.",name);
            return NULL;
        }
    }
}

void CObjectAnimator::Stop()
{
	SetActiveMotion		(0);
	m_MParam.Stop		();
}

#ifdef _EDITOR

#include "d3dutils.h"
#include "envelope.h"

static FvectorVec path_points;

void CObjectAnimator::DrawPath()
{
    // motion path
	if (m_Current&&EPrefs.object_flags.is(epoDrawAnimPath)){
        float fps 				= m_Current->FPS();
        float min_t				= (float)m_Current->FrameStart()/fps;
        float max_t				= (float)m_Current->FrameEnd()/fps;

        Fvector 				T,r;
        u32 clr					= 0xffffffff;
        path_points.clear		();
        for (float t=min_t; (t<max_t)||fsimilar(t,max_t,EPS_L); t+=1/30.f){
            m_Current->_Evaluate(t,T,r);
            path_points.push_back(T);
        }

        Device.SetShader		(Device.m_WireShader);
        RCache.set_xform_world	(Fidentity);
        if (!path_points.empty())DU.DrawPrimitiveL		(D3DPT_LINESTRIP,path_points.size()-1,path_points.begin(),path_points.size(),clr,true,false);
        CEnvelope* E 			= m_Current->Envelope	();
        for (KeyIt k_it=E->keys.begin(); k_it!=E->keys.end(); k_it++){
            m_Current->_Evaluate((*k_it)->time,T,r);
            if (Device.m_Camera.GetPosition().distance_to_sqr(T)<50.f*50.f){
                DU.DrawCross	(T,0.1f,0.1f,0.1f, 0.1f,0.1f,0.1f, clr,false);
                DU.DrawText		(T,AnsiString().sprintf("K: %3.3f",(*k_it)->time).c_str(),0xffffffff,0x00000000);
            }
        }
    }
}
#endif

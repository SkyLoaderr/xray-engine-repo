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
		m_XFORM.setXYZ	(R);
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
            return 		*it;
        }else{
            Debug.fatal	("OBJ ANIM::Cycle '%s' not found.",name);
            return NULL;
        }
    }else{
        if (!m_Motions.empty()){
            bLoop 		= loop;
            SetActiveMotion(m_Motions.front());
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
}


#ifndef ObjectAnimatorH
#define ObjectAnimatorH
#pragma once

#include "motion.h"

// refs
class ENGINE_API CObjectAnimator
{
private:
	DEFINE_VECTOR		(COMotion*,MotionVec,MotionIt);
protected:
	bool				bLoop;

	Fmatrix				m_XFORM;
    SAnimParams			m_MParam;
	MotionVec			m_Motions;

    COMotion*			m_Current;
	void				LoadMotions		(LPCSTR fname);
	void				SetActiveMotion	(COMotion* mot);
	COMotion*			FindMotionByName(LPCSTR name);

	void				Clear			();
public:
						CObjectAnimator	();
	virtual				~CObjectAnimator();

	void				Load			(LPCSTR name);

	COMotion*			Play			(bool bLoop, LPCSTR name=0);
	void				Stop			();
	IC BOOL				IsPlaying		(){return m_MParam.bPlay;}

    IC const Fmatrix&	XFORM			(){return m_XFORM;}
	
	// Update
	void				OnFrame			();
    void				DrawPath		();
};

#endif //ObjectAnimatorH

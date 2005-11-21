// EffectorFall.h: interface for the CEffectorFall class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EFFECTORPP_H__06AA79FB_8954_457B_B338_039EBF4EE94E__INCLUDED_)
#define AFX_EFFECTORPP_H__06AA79FB_8954_457B_B338_039EBF4EE94E__INCLUDED_
#pragma once

#include "CameraDefs.h"
struct SPPInfo;
// постпроцесс
class ENGINE_API CEffectorPP
{
	EEffectorPPType		eType;
	bool				bFreeOnRemove;
	friend class		CCameraManager;
protected:
	float				fLifeTime;
public:
						CEffectorPP		(EEffectorPPType type, f32 lifeTime, bool free_on_remove=true);
	virtual				~CEffectorPP	();
	virtual	BOOL		Process			(SPPInfo &PPInfo);
};

#endif // !defined(AFX_EFFECTORPP_H__06AA79FB_8954_457B_B338_039EBF4EE94E__INCLUDED_)

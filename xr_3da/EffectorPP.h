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
	friend class		CCameraManager;
	float				fLifeTime;
public:
	virtual	BOOL Process(SPPInfo &PPInfo);

	CEffectorPP(EEffectorPPType type, f32 lifeTime);
	virtual ~CEffectorPP();
};

#endif // !defined(AFX_EFFECTORPP_H__06AA79FB_8954_457B_B338_039EBF4EE94E__INCLUDED_)

// CameraManager.h: interface for the CCameraManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CAMERAMANAGER_H__B11F8AE2_1213_11D4_B4E3_4854E82A090D__INCLUDED_)
#define AFX_CAMERAMANAGER_H__B11F8AE2_1213_11D4_B4E3_4854E82A090D__INCLUDED_

#pragma once

#include "CameraDefs.h"

struct SPPInfo {
	f32 blur, gray;
	struct { f32 h, v; } duality;
	struct {
		f32 intensity, grain;
		struct SColor{
			f32 r, g, b, a;
			IC operator u32() {
				return color_rgba(u32(r*255), u32(g*255), u32(b*255), u32(a*255));
			}
		} color;
	} noise;

	IC SPPInfo& operator += (const SPPInfo &ppi) {
		blur += ppi.blur;
		gray += ppi.gray;
		duality.h += ppi.duality.h; duality.v += ppi.duality.v;
		noise.intensity += ppi.noise.intensity; noise.grain += ppi.noise.grain;
		noise.color.r += ppi.noise.color.r; noise.color.g += ppi.noise.color.g;
		noise.color.b += ppi.noise.color.b; noise.color.a += ppi.noise.color.a;
		return *this;
	}
	void normalize();
};

DEFINE_VECTOR				(CEffector*,EffectorVec,EffectorIt);
DEFINE_VECTOR				(CEffectorPP*,EffectorPPVec,EffectorPPIt);

class ENGINE_API CCameraManager
{
	Fvector					vPosition;
	Fvector					vDirection;
	Fvector					vNormal;
	Fvector					vRight;

	EffectorVec				m_Effectors;
	EffectorPPVec			m_EffectorsPP;

	float					fFov;
	float					fFar;
	float					fAspect;
	
	Fmatrix					unaffected_mView;
	Fvector					unaffected_vPosition;
	Fvector					unaffected_vDirection;
	Fvector					unaffected_vNormal;
	Fvector					unaffected_vRight;
	
	Fvector					affected_vPosition;
	Fvector					affected_vDirection;
	Fvector					affected_vNormal;
	Fvector					affected_vRight;

	SPPInfo					unaffected_PP;
	SPPInfo					affected_PP;
public:
	void					Dump				(void);
	CEffector*				AddEffector			(CEffector* ef);
	CEffector*				GetEffector			(EEffectorType type);
	void					RemoveEffector		(EEffectorType type);

	CEffectorPP*			GetEffector			(EEffectorPPType type);
	CEffectorPP*			AddEffector			(CEffectorPP* ef);
	void					RemoveEffector		(EEffectorPPType type);

	IC Fmatrix&				unaffected_View		()	{ return unaffected_mView;		}
	IC Fvector&				unaffected_Pos		()	{ return unaffected_vPosition;	}
	IC Fvector&				unaffected_Dir		()	{ return unaffected_vDirection;	}
	IC Fvector&				unaffected_Up		()	{ return unaffected_vNormal;	}
	IC Fvector&				unaffected_Right	()	{ return unaffected_vRight;		}
	IC void					unaffected_Matrix	(Fmatrix& M)	
	{	M.set(unaffected_vRight,unaffected_vNormal,unaffected_vDirection,unaffected_vPosition);	}

	IC Fvector&				affected_Pos		()	{ return affected_vPosition;	}
	IC Fvector&				affected_Dir		()	{ return affected_vDirection;	}
	IC Fvector&				affected_Up			()	{ return affected_vNormal;		}
	IC Fvector&				affected_Right		()	{ return affected_vRight;		}
	IC void					affected_Matrix		(Fmatrix& M)	
	{	M.set(affected_vRight,affected_vNormal,affected_vDirection,affected_vPosition);	}

	IC Fmatrix&				render_View			()	{ return Device.mView; }
	IC Fvector&				render_Pos			()	{ return vPosition;	}
	IC Fvector&				render_Dir			()	{ return vDirection;}
	IC Fvector&				render_Up			()	{ return vNormal;	}
	IC Fvector&				render_Right		()	{ return vRight;	}
	IC void					render_Matrix		(Fmatrix& M)	
	{	M.set(vRight,vNormal,vDirection,vPosition);	}

	void					Update				(const Fvector& P, const Fvector& D, const Fvector& N, float fFOV_Dest, float fASPECT_Dest, float fFAR_Dest, u32 flags=0);
	void					Update				(const CCameraBase* C);

	CCameraManager();
	~CCameraManager();
};

ENGINE_API extern float psCamInert;
ENGINE_API extern float psCamSlideInert;

#endif // !defined(AFX_CAMERAMANAGER_H__B11F8AE2_1213_11D4_B4E3_4854E82A090D__INCLUDED_)

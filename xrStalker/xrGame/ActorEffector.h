// ActorEffector.h:	 менеджер эффекторов актера
//////////////////////////////////////////////////////////////////////

#include "../CameraDefs.h"
#include "CameraEffector.h"



DEFINE_VECTOR(CCameraEffector*,CameraEffectorVec,CameraEffectorIt);


class CActor;

class CActorEffector
{
	friend CActor;

	Fvector					vPosition;
	Fvector					vDirection;
	Fvector					vNormal;
	Fvector					vRight;

	CameraEffectorVec		m_Effectors;

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

public:
	CActorEffector();
	virtual ~CActorEffector();


	CCameraEffector*		AddEffector			(CCameraEffector* ef);
	CCameraEffector*		GetEffector			(ECameraEffectorType type);
	void					RemoveEffector		(ECameraEffectorType type);

	IC void					affected_Matrix		(Fmatrix& M)
	{			
		M.set(affected_vRight,affected_vNormal,affected_vDirection,affected_vPosition);	
	}

	
	void					Update				(const Fvector& P, const Fvector& D, const Fvector& N, float fFOV_Dest, float fASPECT_Dest, float fFAR_Dest, u32 flags=0);
	void					Update				(const CCameraBase* C);

	void					ApplyDevice			();

};

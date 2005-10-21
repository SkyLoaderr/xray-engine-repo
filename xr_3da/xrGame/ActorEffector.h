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
public:
							CActorEffector		();
	virtual					~CActorEffector		();


	CCameraEffector*		AddEffector			(CCameraEffector* ef);
	CCameraEffector*		GetEffector			(ECameraEffectorType type);
	void					RemoveEffector		(ECameraEffectorType type);

	IC void					camera_Matrix		(Fmatrix& M)
	{			
		M.set(vRight,vNormal,vDirection,vPosition);	
	}
	
	void					Update				(const Fvector& P, const Fvector& D, const Fvector& N, float fFOV_Dest, float fASPECT_Dest, float fFAR_Dest, u32 flags=0);
	void					Update				(const CCameraBase* C);

	void					ApplyDevice			();

};


class SndShockEffector{
public:
	int		m_snd_length;	//ms
	int		m_cur_length;	//ms
	float	m_stored_volume;
public:
	SndShockEffector	();
	~SndShockEffector	();
	bool Active			();
	void Start			(int snd_length, float power);
	void Update			();
};

class CObjectAnimator;
class CAnimatorCamEffector :public CCameraEffector
{
	typedef		CCameraEffector					inherited;

protected:
	virtual bool		Cyclic					() const		{return true;};
	virtual bool		Unlimited				() const		{return false;}
	CObjectAnimator*							m_objectAnimator;
public:
						CAnimatorCamEffector	(ECameraEffectorType type);
	virtual				~CAnimatorCamEffector	();
			void		Start					(LPCSTR fn);
	virtual	BOOL		Process					(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect);
};

class CAnimatorCamLerpEffector :public CAnimatorCamEffector
{
public:
	typedef fastdelegate::FastDelegate0<float>		GET_KOEFF_FUNC;

protected:
	typedef				CAnimatorCamEffector		inherited;
	GET_KOEFF_FUNC									m_func;
	virtual bool		Unlimited					() const		{return false;}
	
public:
						CAnimatorCamLerpEffector	(ECameraEffectorType type, GET_KOEFF_FUNC f);
	virtual	BOOL		Process						(Fvector &p, Fvector &d, Fvector &n, float& fFov, float& fFar, float& fAspect);
};

class CActorCondition;
class CActorAlcoholCamEffector :public CAnimatorCamLerpEffector
{
	typedef  CAnimatorCamLerpEffector	inherited;
protected:
	virtual bool		Cyclic						() const		{return true;};
	virtual bool		Unlimited					() const		{return true;}
public:
						CActorAlcoholCamEffector	(CActorCondition* c);
};

class CFireHitCamEffector :public CAnimatorCamLerpEffector
{
	typedef  CAnimatorCamLerpEffector	inherited;
	float				m_power;
protected:
	virtual bool		Cyclic						() const		{return false;};
	virtual bool		Unlimited					() const		{return false;}

public:
						CFireHitCamEffector			(ECameraEffectorType type,float power);
	float	xr_stdcall	GetPower					()				{return m_power;}
};

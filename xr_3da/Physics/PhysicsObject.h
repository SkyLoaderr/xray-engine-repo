#ifndef PHYSIC_OBJECT_H
#define PHYSIC_OBJECT_H

class ENGINE_API CPhysicsObject{
	float			fMass, fMassInv;
	Fvector			vOriginal;
	Fmatrix33		mInertiaTensor, mInertiaTensorInv;
	BOOL			bIsAffectedByGravity;
	Fvector			vGravityDirection;
	BOOL			bRespondsToForces;
	Fvector			vLinearDamping;
	Fvector			vAngularDamping;

	struct SConfiguration{
		Fmatrix		mTransform; // includes location and rotation matrix
		Fvector		vLinearAccel; 
		Fvector		vLinearVelocity;
		Fvector		vAngularVelocity;
		Fvector		vForce, vAppliedForce;
		Fvector		vTorque, vAppliedTorque;
		Fquaternion qOrientation;
	}configs[2];
	int				iSourceConfig;
	int				iTargetConfig;

public:	
						CPhysicsObject			();
	virtual				~CPhysicsObject			();

	void 				Init					(const Fvector& orig, float mass,
												 BOOL IsAffectedByGravity, BOOL RespondsToForces,
												 const Fvector& linearDamping, const Fvector& angularDamping,
												 const Fbox& bb);

	IC void				SetGravityDir			(const Fvector& d){vGravityDirection.normalize_safe();}
	IC const Fvector&	GetGravityDir			(){return vGravityDirection;}

	void				ApplyCenterForce		(const Fvector& force, BOOL isAppliedForce, int configIndex);
	void				ApplyCenterImpulse		(const Fvector& pImpulse, int configIndex);
	void				ApplyForce				(const Fvector& force, const Fvector& pRadVec, BOOL isAppliedForce, int configIndex);
	void				ApplyImpulse			(const Fvector& pImpulse, const Fvector& pRadVec, int configIndex);
	void				AddTorque				(const Fvector& torque, BOOL isAppliedForce, int configIndex);
	void				ComputeForces			(int configIndex);
	void				Integrate				(float deltaTime);
	void				ClearAndChangeConfig	();

	IC int				GetSourceConfig			(){return iSourceConfig;}
	IC int				GetTargetConfig			(){return iTargetConfig;}

	IC void				GetTransform			(Fmatrix& T, int configIndex);
	IC void				SetTransform			(const Fmatrix& T, int configIndex);
	IC void				SetOrientation			(const Fmatrix& R, int configIndex);

	IC void				GetLinearVelocity		(Fvector& V, int configIndex);
	IC const Fvector&	GetLinearVelocity		(int configIndex);
	IC void				SetLinearVelocity		(const Fvector& V, int configIndex);

	IC const Fvector&	GetAngularVelocity		(int configIndex);
	IC void				GetAngularVelocity		(Fvector& A, int configIndex);
	IC void				SetAngularVelocity		(const Fvector& A, int configIndex);

	IC void				GetLinearAccel			(Fvector& A, int configIndex){ A.set(configs[configIndex].vLinearAccel);}
	IC const Fvector&	GetLinearDamping		(){return vLinearDamping;}
	IC void				SetLinearDamping		(const Fvector& v){vLinearDamping.set(v);}

	float				GetMassInv				(){return fMassInv;}
	float				GetMass					();
	void				SetMass					(float m);

	void				Dump					(int configIndex);
};

ENGINE_API extern float psSqueezeVelocity;

#endif

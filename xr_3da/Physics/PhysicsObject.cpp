#include "stdafx.h"
#pragma hdrstop

#include "PhysicsObject.h"
#include "..\Environment.h"

// ENGINE_API float psSqueezeVelocity=0.2f;

CPhysicsObject::CPhysicsObject()
{
}

CPhysicsObject::~CPhysicsObject()
{
}

void CPhysicsObject::Init(const Fvector& orig, float mass, BOOL IsAffectedByGravity, BOOL RespondsToForces,
						  const Fvector& linearDamping, const Fvector& angularDamping, const Fbox& bb)
{
	Fvector defaultAxis;
	Fvector	bbScale;

// fill user data with relevant class data
	bRespondsToForces = RespondsToForces;
	bIsAffectedByGravity = IsAffectedByGravity;

	vLinearDamping.set(linearDamping);	
	vAngularDamping.set(angularDamping);

	vOriginal.set(orig);

	VERIFY(mass >= 0.1f);

	fMass = mass;
	fMassInv = 1.f / fMass;	

// initialize Physics state
	defaultAxis.set(0.f,0.f,1.f);
	for (int i=0; i<2; i++){		
		configs[i].vLinearVelocity.set(0,0,0);
		configs[i].vAngularVelocity.set(0,0,0);
		configs[i].vForce.set(0,0,0);
		configs[i].vTorque.set(0,0,0);
		configs[i].vAppliedForce.set(0,0,0);
		configs[i].vAppliedTorque.set(0,0,0);
		configs[i].qOrientation.rotation( defaultAxis, 0.f );
		configs[i].mTransform.identity();
	}

	mInertiaTensor.identity();
	mInertiaTensorInv.identity();

	bbScale.sub(bb.max,bb.min);

// compute Physics inertia tensor and inverse
// we assume the Physics is an axis-aligned box
	mInertiaTensor.m[0][0] = mass / 12.f * (bbScale.y * bbScale.y + bbScale.z * bbScale.z);
	mInertiaTensor.m[1][1] = mass / 12.f * (bbScale.x * bbScale.x + bbScale.z * bbScale.z);
	mInertiaTensor.m[2][2] = mass / 12.f * (bbScale.x * bbScale.x + bbScale.y * bbScale.y);

	VERIFY(mInertiaTensor.m[0][0] > EPS);
	VERIFY(mInertiaTensor.m[1][1] > EPS);
	VERIFY(mInertiaTensor.m[2][2] > EPS);

	mInertiaTensorInv.m[0][0] = 1/mInertiaTensor.m[0][0];
	mInertiaTensorInv.m[1][1] = 1/mInertiaTensor.m[1][1];
	mInertiaTensorInv.m[2][2] = 1/mInertiaTensor.m[2][2];

	vGravityDirection.set(0,-1,0);

	iSourceConfig = 0;
	iTargetConfig = 1;
}

// apply force in global frame with changes taking effect on next iteration
void CPhysicsObject::ApplyForce(const Fvector& force, const Fvector& radiusVector, BOOL isAppliedForce, int configIndex)
{
	Fvector torqueToAdd;
	SConfiguration& cur_config = configs[configIndex];

	if (!isAppliedForce){
		cur_config.vForce.add(force);
		torqueToAdd.crossproduct(force,radiusVector);
		cur_config.vTorque.add(torqueToAdd);
	}else{
		cur_config.vAppliedForce.add(force);
		torqueToAdd.crossproduct(force,radiusVector);
		cur_config.vAppliedTorque.add(torqueToAdd);
	}
}

void CPhysicsObject::ApplyCenterForce(const Fvector& force, BOOL isAppliedForce, int configIndex)
{
	SConfiguration& cur_config = configs[configIndex];

	if (!isAppliedForce)
		cur_config.vForce.add(force);
	else
		cur_config.vAppliedForce.add(force);
}

// add torque
void CPhysicsObject::AddTorque(const Fvector& torque, BOOL isAppliedForce, int configIndex)
{
	SConfiguration& cur_config = configs[configIndex];

	if (!isAppliedForce)
		cur_config.vTorque.add(torque);
	else
		cur_config.vAppliedTorque.add(torque);
}

// apply impulse in global frame with immediate change in velocities
void CPhysicsObject::ApplyImpulse(const Fvector& Impulse, const Fvector& radiusVector, int configIndex)
{
	Fmatrix33 R, Rt;
	Fvector rCrossRW, rCrossRL, dv, dw;
	SConfiguration& cur_config = configs[configIndex];

	dv.mul(Impulse, fMassInv);
	cur_config.vLinearVelocity.add(dv);

	rCrossRW.crossproduct(Impulse,radiusVector);

	R.set(cur_config.mTransform);
	Rt.transpose(R);

	Rt.MxV(rCrossRL,rCrossRW);
	mInertiaTensorInv.MxV(dw,rCrossRL);
	cur_config.vAngularVelocity.add(dw);
}

void CPhysicsObject::ApplyCenterImpulse(const Fvector& Impulse, int configIndex)
{
	Fvector dv;
	SConfiguration& cur_config = configs[configIndex];

	dv.mul(Impulse, fMassInv);
	cur_config.vLinearVelocity.add(dv);
}

void CPhysicsObject::ComputeForces(int configIndex)
{
	SConfiguration& cur_config = configs[configIndex];

	// add damping
	Fvector ldInv,adInv;
	ldInv.mul(vLinearDamping,-1);	ldInv.add(1);
	adInv.mul(vAngularDamping,-1);	adInv.add(1);
	cur_config.vLinearVelocity.mul(ldInv);
	cur_config.vAngularVelocity.mul(adInv);

	// clear force and torque accumulators
	cur_config.vForce.set(0,0,0);
	cur_config.vTorque.set(0,0,0);
	
	// add gravity
	if (bIsAffectedByGravity)
		cur_config.vForce.mul(vGravityDirection,psGravity*fMass);

	// add forces
	if (bRespondsToForces){
		cur_config.vForce.add(cur_config.vAppliedForce);
		cur_config.vTorque.add(cur_config.vAppliedTorque);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// integrate a gePhysicsObject's equations of motion by time step deltaTime
void CPhysicsObject::Integrate(float dt)
{
	Fvector tau;
	Fvector dv;
	Fvector angularMomentum, angularAccel;
	Fvector omega_x_L, term1, dtTimesOmega;
	Fmatrix33 R, Rt;
	
	Fquaternion qdot;
	float G[3][4], Gt[4][3];
//	float dt2 = 0.5f * (dt * dt);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SConfiguration &src_config = configs[iSourceConfig];
	SConfiguration &tgt_config = configs[iTargetConfig];

	tgt_config.vLinearAccel.mul(src_config.vForce, fMassInv);
	dv.mul(tgt_config.vLinearAccel,dt);
	tgt_config.vLinearVelocity.add(src_config.vLinearVelocity,dv);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	mInertiaTensor.MxV(angularMomentum, src_config.vAngularVelocity);
	omega_x_L.crossproduct(angularMomentum,src_config.vAngularVelocity);

	// compute torque in body frame (= R ^ T * torque)
	R.set(src_config.mTransform);
	Rt.transpose(R);
	Rt.MxV(tau, src_config.vTorque);

	term1.sub(tau, omega_x_L);

	mInertiaTensorInv.MxV(angularAccel, term1);

	R.MxV(dtTimesOmega,src_config.vAngularVelocity);
	dtTimesOmega.mul(dt);

	G[0][0] = -src_config.qOrientation.x; 
	G[0][1] = src_config.qOrientation.w; 
	G[0][2] = -src_config.qOrientation.z; 
	G[0][3] = src_config.qOrientation.y;

	G[1][0] = -src_config.qOrientation.y; 
	G[1][1] = src_config.qOrientation.z; 
	G[1][2] = src_config.qOrientation.w; 
	G[1][3] = -src_config.qOrientation.x;

	G[2][0] = -src_config.qOrientation.z; 
	G[2][1] = -src_config.qOrientation.y; 
	G[2][2] = src_config.qOrientation.x; 
	G[2][3] = src_config.qOrientation.w;
	for (int i=0; i<3; i++)
		for (int j=0; j<4; j++)
			Gt[j][i] = 0.5f * G[i][j];

	qdot.w = Gt[0][0] * dtTimesOmega.x + Gt[0][1] * dtTimesOmega.y + Gt[0][2] * dtTimesOmega.z;
	qdot.x = Gt[1][0] * dtTimesOmega.x + Gt[1][1] * dtTimesOmega.y + Gt[1][2] * dtTimesOmega.z;
	qdot.y = Gt[2][0] * dtTimesOmega.x + Gt[2][1] * dtTimesOmega.y + Gt[2][2] * dtTimesOmega.z;
	qdot.z = Gt[3][0] * dtTimesOmega.x + Gt[3][1] * dtTimesOmega.y + Gt[3][2] * dtTimesOmega.z;

	tgt_config.qOrientation.add(src_config.qOrientation, qdot);
	tgt_config.qOrientation.normalize();

	tgt_config.mTransform.rotation(tgt_config.qOrientation);
	
	tgt_config.vAngularVelocity.direct(src_config.vAngularVelocity, angularAccel, dt );	

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	tgt_config.mTransform.c.direct(src_config.mTransform.c, src_config.vLinearVelocity, dt );// + dt2 * a;
}

/*
// compute auxiliary quantities
Target.InverseWorldInertiaTensor = 
	Target.Orientation*m_Cube.InverseBodyInertiaTensor*Transpose(Target.Orientation);

Target.AngularVelocity = 
	Target.InverseWorldInertiaTensor*Target.AngularMomentum;

struct rigid_body
{
    real OneOverMass;
    matrix_3x3 InverseBodyInertiaTensor; // 
    real CoefficientOfRestitution;	// коэффициент восстановления

    int unsigned NumberOfBoundingVertices;
    enum { MaxNumberOfBoundingVertices = 20 };
    vector_3 aBodyBoundingVertices[MaxNumberOfBoundingVertices];

    enum { NumberOfConfigurations = 2 };

    struct configuration
    {
        // primary quantities
        vector_3 CMPosition;		// позиция
        matrix_3x3 Orientation;		// ориентация

        vector_3 CMVelocity;		// скорость
        vector_3 AngularMomentum;	// угловой момент

        vector_3 CMForce;			// сила
        vector_3 Torque;			// вращающий момент

        // auxiliary quantities
        matrix_3x3 InverseWorldInertiaTensor;
        vector_3 AngularVelocity;	// угловая скорость
        
        vector_3 aBoundingVertices[MaxNumberOfBoundingVertices];

    } aConfigurations[NumberOfConfigurations];
};

*/


void CPhysicsObject::ClearAndChangeConfig(){	
	configs[iSourceConfig].vAppliedForce.set(0,0,0);
	configs[iSourceConfig].vAppliedTorque.set(0,0,0);
	iSourceConfig = 1-iSourceConfig;
	iTargetConfig = 1-iTargetConfig;
}

//-----------------------------------------------------------------------------
void CPhysicsObject::GetTransform(Fmatrix& T, int configIndex){
	T.set(configs[configIndex].mTransform);
}
void CPhysicsObject::SetTransform(const Fmatrix& T, int configIndex){
	configs[configIndex].mTransform.set(T);
}
void CPhysicsObject::SetOrientation(const Fmatrix& R, int configIndex){
	configs[configIndex].qOrientation.set(R);
}

//-----------------------------------------------------------------------------
const Fvector& CPhysicsObject::GetLinearVelocity(int configIndex){
	return configs[configIndex].vLinearVelocity;
}
void CPhysicsObject::GetLinearVelocity(Fvector& V, int configIndex){
	V.set(configs[configIndex].vLinearVelocity);
}
void CPhysicsObject::SetLinearVelocity(const Fvector& V, int configIndex){
	configs[configIndex].vLinearVelocity.set(V);
}
//-----------------------------------------------------------------------------

const Fvector& CPhysicsObject::GetAngularVelocity(int configIndex){
	return configs[configIndex].vAngularVelocity;
}
void CPhysicsObject::GetAngularVelocity(Fvector& A, int configIndex){
	A.set(configs[configIndex].vAngularVelocity);
}
void CPhysicsObject::SetAngularVelocity(const Fvector& A, int configIndex){
	configs[configIndex].vAngularVelocity.set(A);
}
//-----------------------------------------------------------------------------

float CPhysicsObject::GetMass(){
	return fMass;
}
void CPhysicsObject::SetMass(float m){
	fMass=m;
	fMassInv = 1.f/fMass;	
}
//-----------------------------------------------------------------------------

void CPhysicsObject::Dump(int configIndex){
	Msg("--------------------------------------------");
	Msg("Source-[%d]  Target-[%d]", iSourceConfig, iTargetConfig);
	Msg("this config        = %d", configIndex);
	Log("XForm              = ", configs[configIndex].mTransform.i);
	Log(" XForm             = ", configs[configIndex].mTransform.j);
	Log(" XForm             = ", configs[configIndex].mTransform.k);
	Log(" XForm             = ", configs[configIndex].mTransform.c);
	Log("vLinearVelocity    = ", configs[configIndex].vLinearVelocity);
	Log("vAngularVelocity   = ", configs[configIndex].vAngularVelocity);
	Log("vForce             = ", configs[configIndex].vForce);
	Log("vAppliedForce      = ", configs[configIndex].vAppliedForce);
	Log("vTorque            = ", configs[configIndex].vTorque);
	Log("vAppliedTorque     = ", configs[configIndex].vAppliedTorque);
	Msg("qOrientation       = [%.2f,%.2f,%.2f,%.2f]", 
		configs[configIndex].qOrientation.x, configs[configIndex].qOrientation.y,
		configs[configIndex].qOrientation.z, configs[configIndex].qOrientation.w);
}

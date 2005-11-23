#include "stdafx.h"
#include "PHInterpolation.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "MathUtils.h"

extern CPHWorld *ph_world;

CPHInterpolation::CPHInterpolation()
{
	m_body=NULL;
	b_frame_mark=0;
//	fTimeDelta=0.f;
}

void CPHInterpolation::SetBody(dBodyID body){
if(!body) return;
m_body=body;
qPositions.fill_in(*((Fvector*) dBodyGetPosition(m_body)));
const dReal* dQ=dBodyGetQuaternion(m_body);
Fquaternion fQ;
fQ.set(-dQ[0],dQ[1],dQ[2],dQ[3]);
qRotations.fill_in(fQ);
}

void CPHInterpolation::UpdatePositions(){
	if(!m_body)
		return;
	///VERIFY2(dV_valid(dBodyGetPosition(m_body)),"invalid body position in update interpolation");
	qPositions.push_back(*((Fvector*) dBodyGetPosition(m_body)));
	b_frame_mark=ph_world->b_frame_mark;
}

void CPHInterpolation::UpdateRotations(){
	if(!m_body)
		return;
	//VERIFY2(dM_valid(dBodyGetRotation(m_body)),"invalid body rotation in update interpolation");
	
	const dReal* dQ=dBodyGetQuaternion(m_body);
	Fquaternion fQ;
	fQ.set(-dQ[0],dQ[1],dQ[2],dQ[3]);
	qRotations.push_back(fQ);
	//Fvector w;
	//twoq_2w(qRotations[0],qRotations[1],fixed_step,w);
	//w=w;
}

void CPHInterpolation::InterpolatePosition(Fvector& pos)
{
	pos.lerp(qPositions[0],qPositions[1],ph_world->FrameTime(b_frame_mark)/fixed_step);
}



void CPHInterpolation::InterpolateRotation(Fmatrix& rot){
	Fquaternion q;
	float	t = ph_world->FrameTime(b_frame_mark)/fixed_step;
	VERIFY		(t>=0.f && t<=1.f);
	q.slerp(qRotations[0],qRotations[1],t);
	rot.rotation(q);
}

void CPHInterpolation::ResetPositions()
{
	VERIFY2(dBodyStateValide(m_body),"Invalid body state");
	qPositions.fill_in(*((Fvector*) dBodyGetPosition(m_body)));
}

void CPHInterpolation::ResetRotations()
{
	VERIFY2(dBodyStateValide(m_body),"Invalid body state");
	const dReal* dQ=dBodyGetQuaternion(m_body);
	Fquaternion fQ;
	fQ.set(-dQ[0],dQ[1],dQ[2],dQ[3]);
	qRotations.fill_in(fQ);
}

void CPHInterpolation::GetRotation(Fquaternion& q, u16 num)
{
	if(!m_body)	return;
	q.set(qRotations[num]);
}

void CPHInterpolation::GetPosition(Fvector& p,u16 num)
{
	if(!m_body)	return;
	p.set(qPositions[num]);
}
void CPHInterpolation::SetPosition(const Fvector& p, u16 num)
{
	if(!m_body)	return;
	qPositions[num].set(p);
}

void CPHInterpolation::SetRotation(const Fquaternion& q, u16 num)
{
	if(!m_body)	return;
	qRotations[num]=q;
}
#include "stdafx.h"
#pragma hdrstop

#include "PHNetState.h"

static void w_vec_q8(NET_Packet& P,const Fvector& vec,const Fvector& min,const Fvector& max)
{
	P.w_float_q8(vec.x,min.x,max.x);
	P.w_float_q8(vec.y,min.y,max.y);
	P.w_float_q8(vec.z,min.z,max.z);
}
static void r_vec_q8(NET_Packet& P,Fvector& vec,const Fvector& min,const Fvector& max)
{
	P.r_float_q8(vec.x,min.x,max.x);
	P.r_float_q8(vec.y,min.y,max.y);
	P.r_float_q8(vec.z,min.z,max.z);
}
static void w_qt_q8(NET_Packet& P,const Fquaternion& q)
{
	Fvector Q;
	Q.set(q.x,q.y,q.z);
	if(q.w<0.f)	Q.invert();
	P.w_float_q8(Q.x,-1.f,1.f);
	P.w_float_q8(Q.y,-1.f,1.f);
	P.w_float_q8(Q.z,-1.f,1.f);
}
static void r_qt_q8(NET_Packet& P,Fquaternion& q)
{
	// x^2 + y^2 + z^2 + w^2 = 1
	P.r_float_q8(q.x,-1.f,1.f);
	P.r_float_q8(q.y,-1.f,1.f);
	P.r_float_q8(q.z,-1.f,1.f);
	q.w=_sqrt(1.f-q.x*q.x-q.y*q.y-q.z*q.z);
}
void	SPHNetState::net_Export(NET_Packet& P)
{
	P.w_vec3(linear_vel);
	//P.w_vec3(angular_vel);
	//P.w_vec3(force);
	//P.w_vec3(torque);
	P.w_vec3(position);
	P.w_vec4(*((Fvector4*)&quaternion));
	//P.w_vec4(*((Fvector4*)&previous_quaternion));
	P.w_u8	((u8)enabled);

}


void	SPHNetState::net_Import(NET_Packet&	P)
{
	P.r_vec3(linear_vel);
	angular_vel.set(0.f,0.f,0.f);		//P.r_vec3(angular_vel);
	force.set(0.f,0.f,0.f);				//P.r_vec3(force);
	torque.set(0.f,0.f,0.f);			//P.r_vec3(torque);
	P.r_vec3(position);
	P.r_vec4(*((Fvector4*)&quaternion));
	previous_quaternion.set(quaternion);//P.r_vec4(*((Fvector4*)&previous_quaternion));
	enabled=!!P.r_u8	();
}

void SPHNetState::net_Save(NET_Packet &P)
{
	net_Export(P);
}

void SPHNetState::net_Load(NET_Packet &P)
{
	net_Import(P);
	previous_position.set(position);
}

void SPHNetState::net_Save(NET_Packet &P,const Fvector& min,const Fvector& max)
{
	//P.w_vec3(linear_vel);
	//P.w_vec3(angular_vel);
	//P.w_vec3(force);
	//P.w_vec3(torque);
	//P.w_vec3(position);
	w_vec_q8(P,position,min,max);
	w_qt_q8(P,quaternion);
	//P.w_vec4(*((Fvector4*)&quaternion));
	//P.w_vec4(*((Fvector4*)&previous_quaternion));
	//P.w_u8	((u8)enabled);
}

void SPHNetState::net_Load(NET_Packet &P,const Fvector& min,const Fvector& max)
{
	linear_vel.set(0.f,0.f,0.f);
	angular_vel.set(0.f,0.f,0.f);
	force.set(0.f,0.f,0.f);
	torque.set(0.f,0.f,0.f);
	r_vec_q8(P,position,min,max);
	previous_position.set(position);
	r_qt_q8(P,quaternion);
	previous_quaternion.set(quaternion);
	enabled=true;

}

SPHBonesData::SPHBonesData()
{
	bones_mask					=u64(-1);
	root_bone					=0;
	min.set(-100.f,-100.f,-100.f);
	max.set(100.f,100.f,100.f);
}
void SPHBonesData::net_Save(NET_Packet &P)
{
	P.w_u64			(bones_mask);
	P.w_u16			(root_bone);
	
	P.w_vec3		(min);
	P.w_vec3		(max);
	P.w_u16			((u16)bones.size());//bones number;
	PHNETSTATE_I	i=bones.begin(),e=bones.end();
	for(;e!=i;i++)
	{
		(*i).net_Save(P,min,max);
	}
	bones.clear();
}

void SPHBonesData::net_Load(NET_Packet &P)
{
	bones_mask					=P.r_u64();
	root_bone					=P.r_u16();
	P.r_vec3					(min);
	P.r_vec3					(max);
	u16 bones_number			=P.r_u16();//bones number /**/
	for(int i=0;i<bones_number;i++)
	{
		SPHNetState	S;
		S.net_Load(P,min,max);
		bones.push_back(S);
	}
}
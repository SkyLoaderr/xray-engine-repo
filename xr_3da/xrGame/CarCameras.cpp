#include "stdafx.h"
#pragma hdrstop

#include "car.h"
#include "actor.h"
#include "cameralook.h"
#include "camerafirsteye.h"

void	CCar::cam_Update			(float dt)
{
	Fvector							P,Da;
	Da.set							(0,0,0);
	if(m_owner)	m_owner->setEnabled(false);
	float yaw_dest,pitch_dest,bank_dest;
	XFORM().getHPB					(yaw_dest,pitch_dest,bank_dest);

	switch(active_camera->tag) {
	case ectFirst:{
		//		angle_lerp					(active_camera->yaw,	-yaw_dest+m_vCamDeltaHP.x,		PI_DIV_2,dt);
		//		angle_lerp					(active_camera->pitch,	-pitch_dest+m_vCamDeltaHP.y,	PI_DIV_2,dt);
		XFORM().transform_tiny		(P,m_camera_position);

		m_owner->Orientation().yaw	= -(active_camera->yaw);//-yaw_dest);
		m_owner->Orientation().pitch= 0;//-active_camera->pitch;
		//		m_vCamDeltaHP.x				= m_vCamDeltaHP.x*(1-PI*dt)+((active_camera->lim_yaw.y+active_camera->lim_yaw.x)/2.f)*PI*dt;
		}break;
	case ectChase:
		XFORM().transform_tiny		(P,m_camera_position);
		break;
	case ectFree:
		XFORM().transform_tiny		(P,m_camera_position);
		break;
	}

	active_camera->Update			(P,Da);
	Level().Cameras.Update			(active_camera);

	if(m_owner)	m_owner->setEnabled(true);
}

void	CCar::OnCameraChange		(int type)
{
	if (!active_camera||active_camera->tag!=type){
		active_camera	= camera[type];
		float Y,P,R;
		XFORM().getHPB			(Y,P,R);
		active_camera->yaw			= -Y;
//		m_vCamDeltaHP.y	= active_camera->pitch;
	}
}


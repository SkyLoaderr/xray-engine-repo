#include "stdafx.h"
#pragma hdrstop

#include "car.h"
#include "actor.h"
#include "cameralook.h"
#include "camerafirsteye.h"
#include "level.h"

void	CCar::cam_Update			(float /**dt/**/)
{
	Fvector							P,Da;
	Da.set							(0,0,0);
	if(Owner())	Owner()->setEnabled(false);
	XFORM().transform_tiny			(P,m_camera_position);

	switch(active_camera->tag) {
	case ectFirst:
		// rotate head
		Owner()->Orientation().yaw	= -active_camera->yaw;
		Owner()->Orientation().pitch= -active_camera->pitch;
		break;
	case ectChase:					break;
	case ectFree:					break;
	}

	active_camera->Update			(P,Da);
	Level().Cameras.Update			(active_camera);

	if(Owner())	Owner()->setEnabled	(true);
}

void	CCar::OnCameraChange		(int type)
{
	if(Owner())
	{
		if	(type==ectFirst)
		{
			Owner()->setVisible(FALSE);
		}
		else if(active_camera->tag==ectFirst)
		{
			Owner()->setVisible(TRUE);
		}
	}
	
	if (!active_camera||active_camera->tag!=type){
		active_camera	= camera[type];
		if (ectFree==type){
			Fvector xyz;
			XFORM().getXYZi(xyz);
			active_camera->yaw		= xyz.y;
		}
	}

}


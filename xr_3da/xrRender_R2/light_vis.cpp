#include "StdAfx.h"
#include "light.h"
#include "..\cl_intersect.h"

const	u32	delay_small_min			= 1;
const	u32	delay_small_max			= 3;
const	u32	delay_large_min			= 10;
const	u32	delay_large_max			= 20;
const	u32	cullfragments			= 4;

void	light::vis_prepare			()
{
	if (indirect_photons!=ps_r2_GI_photons)	gi_generate	();

	//	. test is sheduled for future	= keep old result
	//	. test time comes :)
	//		. camera inside light volume	= visible,	shedule for 'small' interval
	//		. perform testing				= ???,		pending

	u32	frame	= Device.dwFrame;
	if (frame	<	vis.frame2test)		return;

	BOOL	near_intersect				= FALSE;
	{
		Fmatrix& M						= Device.mFullTransform;
		Fvector4 plane;
		plane.x							= -(M._14 + M._13);
		plane.y							= -(M._24 + M._23);
		plane.z							= -(M._34 + M._33);
		plane.w							= -(M._44 + M._43);
		float denom						= -1.0f / _sqrt(_sqr(plane.x)+_sqr(plane.y)+_sqr(plane.z));
		plane.mul						(denom);
		Fplane	P;	P.n.set(plane.x,plane.y,plane.z); P.d = plane.w;
		float	p_dist					= P.classify	(spatial.center) - spatial.radius;
		near_intersect					= (p_dist<=0);
	}
	//Msg	("sc[%f,%f,%f]/c[%f,%f,%f] - sr[%f]/r[%f]",VPUSH(spatial.center),VPUSH(position),spatial.radius,range);
	//Msg	("dist:%f",Device.vCameraPosition.distance_to(spatial.center));
	if (near_intersect || Device.vCameraPosition.distance_to(spatial.center)<=(spatial.radius+VIEWPORT_NEAR+EPS_S))	{	// small error
		vis.visible		=	true;
		vis.pending		=	false;
		vis.frame2test	=	frame	+ ::Random.randI(delay_small_min,delay_small_max);
		return;
	}

	// testing
	vis.pending										= true;
	xform_calc										();
	RCache.set_xform_world							(m_xform);
	vis.query_order	= RImplementation.occq_begin	(vis.query_id);
	RImplementation.Target.draw_volume				(this);
	RImplementation.occq_end						(vis.query_id);
}

void	light::vis_update			()
{
	//	. not pending	->>> return (early out)
	//	. test-result:	visible:
	//		. shedule for 'large' interval
	//	. test-result:	invisible:
	//		. shedule for 'next-frame' interval

	if (!vis.pending)	return;

	u32	frame			= Device.dwFrame;
	u32 fragments		= RImplementation.occq_get	(vis.query_id);
	// Log					("",fragments);
	vis.visible			= (fragments > cullfragments);
	vis.pending			= false;
	if (vis.visible)	{
		vis.frame2test	=	frame	+ ::Random.randI(delay_large_min,delay_large_max);
	} else {
		vis.frame2test	=	frame	+ 1;
	}
}

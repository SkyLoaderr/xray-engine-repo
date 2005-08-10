////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restrictor.cpp
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restrictor
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "space_restrictor.h"
#include "xrServer_Objects_ALife.h"
#include "level.h"
#include "space_restriction_manager.h"
#include "restriction_space.h"
#include "ai_space.h"

CSpaceRestrictor::~CSpaceRestrictor	()
{
}

void CSpaceRestrictor::Center		(Fvector& C) const
{
	XFORM().transform_tiny			(C,CFORM()->getSphere().P);
}

float CSpaceRestrictor::Radius		() const
{
	return							(CFORM()->getRadius());
}

BOOL CSpaceRestrictor::net_Spawn	(CSE_Abstract* data)
{
#ifdef PRECOMPUTED_INSIDE
	actual							(false);
#endif

	CSE_Abstract					*abstract = (CSE_Abstract*)data;
	CSE_ALifeSpaceRestrictor		*se_shape = smart_cast<CSE_ALifeSpaceRestrictor*>(abstract);
	R_ASSERT						(se_shape);

	CCF_Shape						*shape = xr_new<CCF_Shape>(this);
	collidable.model				= shape;

	for (u32 i=0; i < se_shape->shapes.size(); ++i) {
		CShapeData::shape_def		&S = se_shape->shapes[i];
		switch (S.type) {
			case 0 : {
				shape->add_sphere	(S.data.sphere);
				break;
			}
			case 1 : {
				shape->add_box		(S.data.box);
				break;
			}
		}
	}

	shape->ComputeBounds			();

	BOOL							result = inherited::net_Spawn(data);
	
	if (!result)
		return						(FALSE);

	shedule_unregister				();
	setEnabled						(false);
	setVisible						(false);

	if (!ai().get_level_graph() || (RestrictionSpace::ERestrictorTypes(se_shape->m_space_restrictor_type) == RestrictionSpace::eRestrictorTypeNone))
		return						(TRUE);

	Level().space_restriction_manager().register_restrictor(this,RestrictionSpace::ERestrictorTypes(se_shape->m_space_restrictor_type));

	return							(TRUE);
}

bool CCF_Shape_inside				(const CCF_Shape *self, const Fvector &position, float radius)
{
	// Build object-sphere in World-Space
	Fsphere							S;
	S.P								= position;
	S.R								= radius;

	// Get our matrix
	const Fmatrix					&XF	= self->Owner()->XFORM();
	const xr_vector<CCF_Shape::shape_def>	&shapes = self->shapes;

	// Iterate
	for (u32 el=0; el<shapes.size(); el++)
	{
		switch (shapes[el].type)
		{
		case 0: // sphere
			{
				Fsphere		Q;
				const Fsphere&	T		= shapes[el].data.sphere;
				XF.transform_tiny	(Q.P,T.P);
				Q.R					= T.R;
				if (S.intersect(Q))	return true;
			}
			break;
		case 1:	// box
			{
				Fmatrix		Q;
				const Fmatrix&	T		= shapes[el].data.box;
				Q.mul_43			( XF,T);

				// Build points
				Fvector A,B[8];
				Fplane  P;
				A.set(-.5f, -.5f, -.5f);	Q.transform_tiny(B[0],A);
				A.set(-.5f, -.5f, +.5f);	Q.transform_tiny(B[1],A);
				A.set(-.5f, +.5f, +.5f);	Q.transform_tiny(B[2],A);
				A.set(-.5f, +.5f, -.5f);	Q.transform_tiny(B[3],A);
				A.set(+.5f, +.5f, +.5f);	Q.transform_tiny(B[4],A);
				A.set(+.5f, +.5f, -.5f);	Q.transform_tiny(B[5],A);
				A.set(+.5f, -.5f, +.5f);	Q.transform_tiny(B[6],A);
				A.set(+.5f, -.5f, -.5f);	Q.transform_tiny(B[7],A);

				P.build(B[0],B[3],B[5]);	if (P.classify(S.P)>S.R) break;
				P.build(B[1],B[2],B[3]);	if (P.classify(S.P)>S.R) break;
				P.build(B[6],B[5],B[4]);	if (P.classify(S.P)>S.R) break;
				P.build(B[4],B[2],B[1]);	if (P.classify(S.P)>S.R) break;
				P.build(B[3],B[2],B[4]);	if (P.classify(S.P)>S.R) break;
				P.build(B[1],B[0],B[6]);	if (P.classify(S.P)>S.R) break;
				return true;
			}
			break;
		}
	}
	return false;
}

bool CSpaceRestrictor::inside	(const Fvector &position, float radius) const
{
#ifndef PRECOMPUTED_INSIDE
	return		(CCF_Shape_inside((CCF_Shape*)collidable.model,position,radius));
#else
	if (!actual())
		prepare	();
#if 0//def _DEBUG
	bool		value0 = prepared_inside(position, radius);
	bool		value1 = CCF_Shape_inside((CCF_Shape*)collidable.model,position,radius);
	VERIFY		(value0 == value1);
	return		(value0);
#else
	return		(prepared_inside(position, radius));
#endif
#endif
}

BOOL CSpaceRestrictor::UsedAI_Locations	()
{
	return		(FALSE);
}

#ifdef PRECOMPUTED_INSIDE
void CSpaceRestrictor::spatial_move		()
{
	inherited::spatial_move				();
	actual								(false);
}

void CSpaceRestrictor::prepare			() const
{
	Center							(m_selfbounds.P);
	m_selfbounds.R					= Radius();

	m_spheres.resize				(0);
	m_boxes.resize					(0);

	const CCF_Shape					*shape = (const CCF_Shape*)collidable.model;

	typedef xr_vector<CCF_Shape::shape_def> SHAPES;

	SHAPES::const_iterator			I = shape->shapes.begin();
	SHAPES::const_iterator			E = shape->shapes.end();
	for ( ; I != E; ++I) {
		switch ((*I).type) {
			case 0 : { // sphere
				Fsphere					temp;
				const Fsphere			&sphere = (*I).data.sphere;
				XFORM().transform_tiny	(temp.P,sphere.P);
				temp.R					= sphere.R;
				m_spheres.push_back		(temp);
				break;
			}
			case 1 : { // box
				Fmatrix					sphere;
				const Fmatrix			&box = (*I).data.box;
				sphere.mul_43			(XFORM(),box);

				// Build points
				Fvector					A,B[8];
				CPlanes					temp;
				A.set					(-.5f, -.5f, -.5f);	sphere.transform_tiny(B[0],A);
				A.set					(-.5f, -.5f, +.5f);	sphere.transform_tiny(B[1],A);
				A.set					(-.5f, +.5f, +.5f);	sphere.transform_tiny(B[2],A);
				A.set					(-.5f, +.5f, -.5f);	sphere.transform_tiny(B[3],A);
				A.set					(+.5f, +.5f, +.5f);	sphere.transform_tiny(B[4],A);
				A.set					(+.5f, +.5f, -.5f);	sphere.transform_tiny(B[5],A);
				A.set					(+.5f, -.5f, +.5f);	sphere.transform_tiny(B[6],A);
				A.set					(+.5f, -.5f, -.5f);	sphere.transform_tiny(B[7],A);

				temp.m_planes[0].build	(B[0],B[3],B[5]);
				temp.m_planes[1].build	(B[1],B[2],B[3]);
				temp.m_planes[2].build	(B[6],B[5],B[4]);
				temp.m_planes[3].build	(B[4],B[2],B[1]);
				temp.m_planes[4].build	(B[3],B[2],B[4]);
				temp.m_planes[5].build	(B[1],B[0],B[6]);

				m_boxes.push_back		(temp);

				break;
			}
			default : NODEFAULT;
		}
	}

	actual							(true);
}

bool CSpaceRestrictor::prepared_inside	(const Fvector &position, float radius) const
{
	VERIFY							(actual());
	
	Fsphere							sphere	;
	sphere.P						=		position;
	sphere.R						=		radius;

	if (!m_selfbounds.intersect(sphere))	return false	;

	{
		SPHERES::const_iterator		I = m_spheres.begin();
		SPHERES::const_iterator		E = m_spheres.end();
		for ( ; I != E; ++I)
			if (sphere.intersect(*I))
				return				(true);
	}

	{
		BOXES::const_iterator		I = m_boxes.begin();
		BOXES::const_iterator		E = m_boxes.end();
		for ( ; I != E; ++I) {
			for (u32 i=0; i<PLANE_COUNT; ++i)
				if ((*I).m_planes[i].classify(sphere.P) > sphere.R)
					goto continue_loop;
			return					(true);
continue_loop:
			continue;
		}
	}
	return							(false);
}

#endif

#ifdef DEBUG

#include "customzone.h"
#include "hudmanager.h"

extern	Flags32	dbg_net_Draw_Flags;

void CSpaceRestrictor::OnRender	()
{
	if(!bDebug) return;
	if (!(dbg_net_Draw_Flags.is_any((1<<2)))) return;
	RCache.OnFrameEnd();
	Fvector l_half; l_half.set(.5f, .5f, .5f);
	Fmatrix l_ball, l_box;
	xr_vector<CCF_Shape::shape_def> &l_shapes = ((CCF_Shape*)CFORM())->Shapes();
	xr_vector<CCF_Shape::shape_def>::iterator l_pShape;
	
	u32 Color = 0;
	CCustomZone	*custom_zone = smart_cast<CCustomZone*>(this);
	if (custom_zone && custom_zone->IsEnabled())
		Color = D3DCOLOR_XRGB(0,255,255);
	else
		Color = D3DCOLOR_XRGB(255,0,0);

	
	for(l_pShape = l_shapes.begin(); l_shapes.end() != l_pShape; ++l_pShape) 
	{
		switch(l_pShape->type)
		{
		case 0:
			{
                Fsphere &l_sphere = l_pShape->data.sphere;
				l_ball.scale(l_sphere.R, l_sphere.R, l_sphere.R);
				//l_ball.scale(1.f, 1.f, 1.f);
				Fvector l_p; XFORM().transform(l_p, l_sphere.P);
				l_ball.translate_add(l_p);
				//l_ball.mul(XFORM(), l_ball);
				//l_ball.mul(l_ball, XFORM());
				RCache.dbg_DrawEllipse(l_ball, Color);
			}
			break;
		case 1:
			{
				l_box.mul(XFORM(), l_pShape->data.box);
				RCache.dbg_DrawOBB(l_box, l_half, Color);
			}
			break;
		}
	}
	if( Level().CurrentViewEntity()->Position().distance_to(XFORM().c)<100.0f ){
	
//DRAW name

		Fmatrix		res;
		res.mul		(Device.mFullTransform, XFORM());

		Fvector4	v_res;

		float		delta_height = 0.f;

		// get up on 2 meters
		Fvector shift;
		static float gx = 0.0f;
		static float gy = 2.0f;
		static float gz = 0.0f;
		shift.set(gx,gy,gz);
		res.transform(v_res, shift);

// check if the object in sight
		if (v_res.z < 0 || v_res.w < 0)										return;
		if (v_res.x < -1.f || v_res.x > 1.f || v_res.y<-1.f || v_res.y>1.f) return;

		// get real (x,y)
		float x = (1.f + v_res.x)/2.f * (Device.dwWidth);
		float y = (1.f - v_res.y)/2.f * (Device.dwHeight) - delta_height;

		HUD().Font().pFontMedium->SetColor	(0xffff0000);
		HUD().Font().pFontMedium->OutSet	(x, y-=delta_height);
		HUD().Font().pFontMedium->OutNext	( Name() );
	}


}
#endif

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

BOOL CSpaceRestrictor::net_Spawn	(LPVOID data)
{
	CSE_Abstract					*abstract = (CSE_Abstract*)data;
	CSE_ALifeSpaceRestrictor		*se_shape = dynamic_cast<CSE_ALifeSpaceRestrictor*>(abstract);
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

	BOOL							result = inherited::net_Spawn(data);
	
	if (!result)
		return						(FALSE);

	shape->ComputeBounds			();
	Fvector							P;
	XFORM().transform_tiny			(P,CFORM()->getSphere().P);

	shedule_unregister				();
	setEnabled						(false);
	setVisible						(false);

	if (!ai().get_level_graph())
		return						(FALSE);

	Level().space_restriction_manager().register_restrictor(this,RestrictionSpace::EDefaultRestrictorTypes(se_shape->m_default_space_restrictor_type));

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
	return		(CCF_Shape_inside((CCF_Shape*)collidable.model,position,radius));
}

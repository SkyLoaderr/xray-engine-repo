////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restrictor.cpp
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restrictor
////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "space_restrictor.h"
#include "xrServer_Object_Base.h"

CSpaceRestrictor::~CSpaceRestrictor	()
{
}

BOOL CSpaceRestrictor::net_Spawn		(LPVOID data)
{
	CSE_Abstract					*abstract = (CSE_Abstract*)data;
	CShapeData						*se_shape = dynamic_cast<CShapeData*>(abstract);
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
	
	if (result) {
		shape->ComputeBounds		();
		Fvector						P;
		XFORM().transform_tiny		(P,CFORM()->getSphere().P);
	}

	shedule_unregister				();
	setEnabled						(false);
	setVisible						(false);

	return							(result);
}

bool CSpaceRestrictor::inside		(const Fvector &position, float radius) const
{
	// Build object-sphere in World-Space
	Fsphere							S;
	S.P								= position;
	S.R								= radius;

	// Get our matrix
	const Fmatrix					&XF	= XFORM();
	xr_vector<CCF_Shape::shape_def>	&shapes = ((CCF_Shape*)collidable.model)->shapes;

	// Iterate
	for (u32 el=0; el<shapes.size(); el++)
	{
		switch (shapes[el].type)
		{
		case 0: // sphere
			{
				Fsphere		Q;
				Fsphere&	T		= shapes[el].data.sphere;
				XF.transform_tiny	(Q.P,T.P);
				Q.R					= T.R;
				if (S.intersect(Q))	return true;
			}
			break;
		case 1:	// box
			{
				Fmatrix		Q;
				Fmatrix&	T		= shapes[el].data.box;
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

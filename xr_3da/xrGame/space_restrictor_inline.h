////////////////////////////////////////////////////////////////////////////
//	Module 		: space_restrictor_inline.h
//	Created 	: 17.08.2004
//  Modified 	: 17.08.2004
//	Author		: Dmitriy Iassenev
//	Description : Space restrictor inline functions
////////////////////////////////////////////////////////////////////////////

#pragma once

IC	CSpaceRestrictor::CSpaceRestrictor		()
{
}

#ifdef PRECOMPUTED_INSIDE
IC	bool CSpaceRestrictor::actual			() const
{
	return							(m_actuality);
}

IC	void CSpaceRestrictor::actual			(bool value) const
{
	m_actuality						= value;
}

IC	void CSpaceRestrictor::prepare			() const
{
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

IC	bool CSpaceRestrictor::prepared_inside	(const Fvector &position, float radius) const
{
	VERIFY							(actual());
	
	Fsphere							sphere;
	sphere.P						= position;
	sphere.R						= radius;

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

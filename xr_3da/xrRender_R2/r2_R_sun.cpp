#include "stdafx.h"
#include "..\irenderable.h"

const	float	tweak_COP_initial_offs			= - 100000.f;
const	float	tweak_ortho_xform_initial_offs	= - 1000.f	;

//////////////////////////////////////////////////////////////////////////
#define DW_AS_FLT(DW) (*(FLOAT*)&(DW))
#define FLT_AS_DW(F) (*(DWORD*)&(F))
#define FLT_SIGN(F) ((FLT_AS_DW(F) & 0x80000000L))
#define ALMOST_ZERO(F) ((FLT_AS_DW(F) & 0x7f800000L)==0)
#define IS_SPECIAL(F) ((FLT_AS_DW(F) & 0x7f800000L)==0x7f800000L)

//////////////////////////////////////////////////////////////////////////
// OLES: naive builder of infinite volume expanded from base frustum towards 
//		 light source. really slow, but it works for our simple usage :)
// note: normals points to 'outside'
//////////////////////////////////////////////////////////////////////////
class	DumbConvexVolume
{
public:
	struct	_poly
	{
		xr_vector<int>	points;
		Fvector3		planeN;
		float			planeD;
		float			classify	(Fvector3& p)	{	return planeN.dotproduct(p)+planeD; 	}
	};
	struct	_edge
	{
		int				p0,p1;
		int				counter;
						_edge		(int _p0, int _p1, int m) : p0(_p0), p1(_p1), counter(m){ if (p0>p1)	swap(p0,p1); 	}
		bool			equal		(_edge& E)												{ return p0==E.p0 && p1==E.p1;	}
	};
public:
	xr_vector<Fvector3>		points;
	xr_vector<_poly>		polys;
	xr_vector<_edge>		edges;
public:
	void				compute_planes	()
	{
		for (int it=0; it<int(polys.size()); it++)
		{
			_poly&			P	=	polys[it];
			Fvector3		t1,t2;
			t1.sub					(points[P.points[0]], points[P.points[1]]);
			t2.sub					(points[P.points[0]], points[P.points[2]]);
			P.planeN.crossproduct	(t1,t2).normalize();
			P.planeD			= -	P.planeN.dotproduct(points[P.points[0]]);
		}
	}
	void				compute_caster_model	(xr_vector<Fplane>& dest, Fvector3 direction)
	{
		// COG
		Fvector3	cog	= {0,0,0};
		for			(int it=0; it<int(points.size()); it++)	cog.add	(points[it]);
		cog.div		(float(points.size()));

		// planes
		compute_planes	();

		// remove faceforward polys, build list of edges -> find open ones
		for (int it=0; it<int(polys.size()); it++)
		{
			_poly&	base		= polys	[it];
			VERIFY	(base.classify(cog)<0);					// debug

			int		marker		= (base.planeN.dotproduct(direction)<=0)?-1:1;

			// register edges
			xr_vector<int>&	plist		= polys[it].points;
			for (int p=0; p<int(plist.size()); p++)	{
				_edge	E		(plist[p],plist[ (p+1)%plist.size() ], marker);
				bool	found	= false;
				for (int e=0; e<int(edges.size()); e++)	
					if (edges[e].equal(E))	{ edges[e].counter += marker; found=true; break; }
					if		(!found)	edges.push_back	(E);
			}

			// remove if unused
			if (marker>0)	{
				polys.erase	(polys.begin()+it);
				it--;
			}
		}

		// Extend model to infinity, the volume is not capped, so this is indeed up to infinity
		for (int e=0; e<int(edges.size()); e++)
		{
			if	(edges[e].counter != 0)	continue;
			_edge&		E		= edges[e];
			Fvector3	point;
			points.push_back	(point.sub(points[E.p0],direction));
			points.push_back	(point.sub(points[E.p1],direction));
			polys.push_back		(_poly());
			_poly&		P		= polys.back();	
			int			pend	= int(points.size());
			P.points.push_back	(E.p0);
			P.points.push_back	(E.p1);
			P.points.push_back	(pend-1);
			P.points.push_back	(pend-2);
		}

		// Reorient planes (try to write more inefficient code :)
		compute_planes	();
		for (int it=0; it<int(polys.size()); it++)
		{
			_poly&	base				= polys	[it];
			if (base.classify(cog)>0)	std::reverse(base.points.begin(),base.points.end());
		}

		// Export
		compute_planes	();
		for (int it=0; it<int(polys.size()); it++)
		{
			_poly&			P	= polys[it];
			Fplane			pp	= {P.planeN,P.planeD};
			dest.push_back	(pp);
		}
	}
};

void CRender::render_sun				()
{
	light*	fuckingsun					= Lights.sun;

	// 
	Fmatrix		fullxform_inv			;
	D3DXMatrixInverse					((D3DXMATRIX*)&fullxform_inv,0,(D3DXMATRIX*)&Device.mFullTransform);

	// calculate view-frustum bounds in world space
	// note: D3D uses [0..1] range for Z
	static Fvector3		corners [8]			= {
		{ -1, -1,  0 },		{ -1, -1, +1},
		{ -1, +1, +1 },		{ -1, +1,  0},
		{ +1, +1, +1 },		{ +1, +1,  0},
		{ +1, -1, +1},		{ +1, -1,  0}
	};
	static int			facetable[6][4]		= {
		{ 0, 3, 5, 7 },		{ 1, 2, 3, 0 },
		{ 6, 7, 5, 4 },		{ 4, 2, 1, 6 },
		{ 3, 2, 4, 5 },		{ 1, 0, 7, 6 },
	};

	// Lets begin from base frustum
	DumbConvexVolume		hull;
	D3DXVECTOR3				frustumPnts	[8];		// in EYE space
	{
		hull.points.reserve		(8);
		for						(int p=0; p<8; p++)	{
			Fvector3				xf;
			fullxform_inv.transform	(xf,corners[p]);
			hull.points.push_back	(xf);
			Device.mView.transform	(xf);			// move into EYE space
			frustumPnts[p]			= D3DXVECTOR3	(xf.x, xf.y, xf.z);
		}
		for (int plane=0; plane<6; plane++)	{
			hull.polys.push_back(DumbConvexVolume::_poly());
			for (int pt=0; pt<4; pt++)	
				hull.polys.back().points.push_back(facetable[plane][pt]);
		}
	}

	// Compute volume(s) - something like a frustum for infinite directional light
	// Also compute virtual light position and sector it is inside
	CFrustum					cull_frustum;
	xr_vector<Fplane>			cull_planes;
	Fvector3					cull_COP;
	CSector*					cull_sector;
	{
		hull.compute_caster_model	(cull_planes,fuckingsun->direction);

		// Search for default sector - assume "default" or "outdoor" sector is the largest one
		//. hack: need to know real outdoor sector
		CSector*	largest_sector		= 0;
		float		largest_sector_vol	= 0;
		for		(u32 s=0; s<Sectors.size(); s++)
		{
			CSector*			S		= (CSector*)Sectors[s]	;
			IRender_Visual*		V		= S->root()				;
			float				vol		= V->vis.box.getvolume();
			if (vol>largest_sector_vol)	{
				largest_sector_vol		= vol;
				largest_sector			= S;
			}
		}
		cull_sector	= largest_sector;

		// COP - 100 km away
		cull_COP.mad				(Device.vCameraPosition, fuckingsun->direction, tweak_COP_initial_offs	);

		// Create frustum for query
		Msg	("%d:",cull_planes.size());
		for (int p=0; p<cull_planes.size(); p++)
			cull_frustum._add		(cull_planes[p]);
	}

	// Compute approximate ortho transform for light-source
	const D3DXMATRIX&	m_View		= *((D3DXMATRIX*)(&Device.mView));
	D3DXMATRIX	lightSpaceBasis	;  
	D3DXMATRIX	lightSpaceOrtho	;
	D3DXMATRIX	m_LightViewProj	;
	Fbox3		frustumBox		;
	{
		//	Prepare to interact with D3DX code
		D3DXVECTOR3			m_lightDir	= D3DXVECTOR3(fuckingsun->direction.x,fuckingsun->direction.y,fuckingsun->direction.z);

		//  we need to transform the eye into the light's post-projective space.
		//  however, the sun is a directional light, so we first need to find an appropriate
		//  rotate/translate matrix, before constructing an ortho projection.
		//  this matrix is a variant of "light space" from LSPSMs, with the Y and Z axes permuted
		D3DXVECTOR3				leftVector, upVector, viewVector;
		const D3DXVECTOR3		eyeVector	( 0.f, 0.f, -1.f );			//  eye is always -Z in eye space

		//  code copied straight from BuildLSPSMProjectionMatrix
		D3DXVec3TransformNormal	( &upVector,	&m_lightDir, &m_View );	// lightDir is defined in eye space, so xform it
		D3DXVec3Cross			( &leftVector,	&upVector,	&eyeVector	);
		D3DXVec3Normalize		( &leftVector,	&leftVector );
		D3DXVec3Cross			( &viewVector,	&upVector,	&leftVector );

		lightSpaceBasis._11 = leftVector.x; lightSpaceBasis._12 = viewVector.x; lightSpaceBasis._13 = -upVector.x; lightSpaceBasis._14 = 0.f;
		lightSpaceBasis._21 = leftVector.y; lightSpaceBasis._22 = viewVector.y; lightSpaceBasis._23 = -upVector.y; lightSpaceBasis._24 = 0.f;
		lightSpaceBasis._31 = leftVector.z; lightSpaceBasis._32 = viewVector.z; lightSpaceBasis._33 = -upVector.z; lightSpaceBasis._34 = 0.f;
		lightSpaceBasis._41 = 0.f;          lightSpaceBasis._42 = 0.f;          lightSpaceBasis._43 = 0.f;        lightSpaceBasis._44 = 1.f;

		// rotate the view frustum into light space, find AABB
		D3DXVec3TransformCoordArray	( frustumPnts, sizeof(D3DXVECTOR3), frustumPnts, sizeof(D3DXVECTOR3), &lightSpaceBasis, sizeof(frustumPnts)/sizeof(D3DXVECTOR3) );
		frustumBox.invalidate		();
		for (int p=0; p<8; p++)	frustumBox.modify	(frustumPnts[p].x, frustumPnts[p].y, frustumPnts[p].z);

		// build initial ortho-xform
		D3DXMatrixOrthoOffCenterLH	( &lightSpaceOrtho, frustumBox.min.x, frustumBox.max.x, frustumBox.min.y, frustumBox.max.y, frustumBox.min.z-tweak_ortho_xform_initial_offs, frustumBox.max.z);
		D3DXMatrixMultiply			( &m_LightViewProj, &lightSpaceBasis,	&lightSpaceOrtho);
		D3DXMatrixMultiply			( &m_LightViewProj, &m_View,			&m_LightViewProj);	//.?
	}

	// Begin SMAP-render
	{
		HOM.Disable								();
		phase									= PHASE_SMAP_D;
		if (RImplementation.o.Tshadows)	r_pmask	(true,true	);
		else							r_pmask	(true,false	);
		fuckingsun->svis.begin					();
	}

	// Fill the database
	xr_vector<Fbox3>&		s_receivers			= main_coarse_structure;
	xr_vector<Fbox3>		s_casters;			s_casters.reserve	(s_receivers.size());
	set_Recorder								(&s_casters);
	r_dsgraph_render_subspace					(cull_sector, &cull_frustum, *((Fmatrix*)&m_LightViewProj), cull_COP, TRUE);
	set_Recorder								(NULL);

	// Compute REAL sheared xform based on receivers/casters information
	{
		//  transform the shadow caster bounding boxes into light projective space.  
		//  we want to translate along the Z axis so that all shadow casters are 
		//	in front of the near plane.
		float		min_z = 1e32f,	max_z=-1e32f;
		Fmatrix&	minmax_xform	= *((Fmatrix*)&m_LightViewProj);
		for		(int c=0; c<s_casters.size(); c++)
		{
			Fvector3				pt;
			minmax_xform.transform	(pt,s_casters[c].min);
			min_z	= _min			( min_z, pt.z );
			max_z	= _max			( max_z, pt.z );
			minmax_xform.transform	(pt,s_casters[c].max);
			min_z	= _min			( min_z, pt.z );
			max_z	= _max			( max_z, pt.z );
		}
		min_z	= _min	( min_z, frustumBox.min.z );
		max_z	= _max	( max_z, frustumBox.max.z );
		if ( min_z <= 1.f )
		{
			D3DXMATRIX				lightSpaceTranslate;
			D3DXMatrixTranslation	( &lightSpaceTranslate, 0.f, 0.f, -min_z + 1.f );
			max_z					= -min_z + max_z + 1.f;
			min_z					= 1.f;
			D3DXMatrixMultiply			( &lightSpaceBasis, &lightSpaceBasis, &lightSpaceTranslate );

			// update view frustum, find AABB
			D3DXVec3TransformCoordArray	( frustumPnts, sizeof(D3DXVECTOR3), frustumPnts, sizeof(D3DXVECTOR3), &lightSpaceTranslate, sizeof(frustumPnts)/sizeof(D3DXVECTOR3) );
			frustumBox.invalidate		();
			for (int p=0; p<8; p++)		frustumBox.modify	(frustumPnts[p].x, frustumPnts[p].y, frustumPnts[p].z);
		}

		// rebuild ortho-transform + update frustum
		D3DXMatrixOrthoOffCenterLH	( &lightSpaceOrtho, frustumBox.min.x, frustumBox.max.x, frustumBox.min.y, frustumBox.max.y, min_z, max_z );
		D3DXVec3TransformCoordArray	( frustumPnts, sizeof(D3DXVECTOR3), frustumPnts, sizeof(D3DXVECTOR3), &lightSpaceOrtho, sizeof(frustumPnts)/sizeof(D3DXVECTOR3) );

		// search for "center-line"
		D3DXVECTOR2 centerPts	[2];
		centerPts[0].x = 0.25f	* (frustumPnts[0].x + frustumPnts[3].x + frustumPnts[5].x + frustumPnts[7].x);	//  near plane
		centerPts[0].y = 0.25f	* (frustumPnts[0].y + frustumPnts[3].y + frustumPnts[5].y + frustumPnts[7].y);	//  near plane
		centerPts[1].x = 0.25f	* (frustumPnts[1].x + frustumPnts[2].x + frustumPnts[4].x + frustumPnts[6].x);	//  far plane
		centerPts[1].y = 0.25f	* (frustumPnts[1].y + frustumPnts[2].y + frustumPnts[4].y + frustumPnts[6].y);	//  far plane
		D3DXVECTOR2 centerOrig	= (centerPts[0] + centerPts[1])*0.5f;

		//  this matrix transforms the center line to y=0.
		//  since Top and Base are orthogonal to Center, we can skip computing the convex hull, and instead
		//  just find the view frustum X-axis extrema.  The most negative is Top, the most positive is Base
		//  Point Q (trapezoid projection point) will be a point on the y=0 line.
		D3DXMATRIX trapezoid_space;
		D3DXMATRIX xlate_center	(   1.f,           0.f,				0.f, 0.f,
									0.f,           1.f,				0.f, 0.f,
									0.f,           0.f,				1.f, 0.f,
								   -centerOrig.x, -centerOrig.y,	0.f, 1.f );
		float half_center_len	= D3DXVec2Length( &D3DXVECTOR2(centerPts[1] - centerOrig) );
		float x_len = centerPts[1].x - centerOrig.x;
		float y_len = centerPts[1].y - centerOrig.y;

		float cos_theta = x_len / half_center_len;
		float sin_theta = y_len / half_center_len;

		D3DXMATRIX rot_center	(	cos_theta, -sin_theta,	0.f, 0.f,
									sin_theta,  cos_theta,	0.f, 0.f,
									0.f,        0.f,		1.f, 0.f,
									0.f,        0.f,		0.f, 1.f	);

		D3DXMatrixMultiply			( &trapezoid_space, &xlate_center, &rot_center );
		D3DXVec3TransformCoordArray	( frustumPnts, sizeof(D3DXVECTOR3), frustumPnts, sizeof(D3DXVECTOR3), &trapezoid_space, sizeof(frustumPnts)/sizeof(D3DXVECTOR3) );

		//  maximize the area occupied by the bounding box
		Fbox3		frustumAABB2D	;
		frustumAABB2D.invalidate	();
		for (int p=0; p<8; p++)		frustumAABB2D.modify	(frustumPnts[p].x, frustumPnts[p].y, frustumPnts[p].z);

		float x_scale	= _max( _abs(frustumAABB2D.max.x), _abs(frustumAABB2D.min.x) );
		float y_scale	= _max( _abs(frustumAABB2D.max.y), _abs(frustumAABB2D.min.y) );
		x_scale			= 1.f/x_scale;
		y_scale			= 1.f/y_scale;

		D3DXMATRIX scale_center	(	x_scale,	0.f,		0.f, 0.f,
									0.f,		y_scale,	0.f, 0.f,
									0.f,		0.f,		1.f, 0.f,
									0.f,		0.f,		0.f, 1.f	);
		D3DXMatrixMultiply			( &trapezoid_space, &trapezoid_space, &scale_center );

		//  scale the frustum AABB up by these amounts (keep all values in the same space)
		frustumAABB2D.min.x *= x_scale;
		frustumAABB2D.max.x *= x_scale;
		frustumAABB2D.min.y *= y_scale;
		frustumAABB2D.max.y *= y_scale;

		//  compute eta.
		float lambda		= frustumAABB2D.max.x - frustumAABB2D.min.x;
		float delta_proj	= ps_r2_sun_projection * lambda;
		const float xi		= - 0.6f;					// 80% line
		float		eta		= (lambda*delta_proj*(1.f+xi)) / (lambda*(1.f-xi)-2.f*delta_proj);

		//  compute the projection point a distance eta from the top line.  this point is on the center line, y=0
		D3DXVECTOR2 projectionPtQ	( frustumAABB2D.max.x + eta, 0.f );

		//  find the maximum slope from the projection point to any point in the frustum.  this will be the
		//  projection field-of-view
		float max_slope		= -1e32f;
		float min_slope		=  1e32f;
		for ( int i=0; i < sizeof(frustumPnts)/sizeof(D3DXVECTOR3); i++ )
		{
			D3DXVECTOR2 tmp( frustumPnts[i].x*x_scale, frustumPnts[i].y*y_scale );
			float x_dist = tmp.x - projectionPtQ.x;
			if ( !(ALMOST_ZERO(tmp.y) || ALMOST_ZERO(x_dist)))
			{
				max_slope = _max(max_slope, tmp.y/x_dist);
				min_slope = _min(min_slope, tmp.y/x_dist);
			}
		}

		float xn = eta;
		float xf = lambda + eta;

		D3DXMATRIX ptQ_xlate		(	-1.f,				0.f, 0.f, 0.f,
										0.f,				1.f, 0.f, 0.f,
										0.f,				0.f, 1.f, 0.f,
										projectionPtQ.x,	0.f, 0.f, 1.f	);
		D3DXMatrixMultiply			( &trapezoid_space, &trapezoid_space, &ptQ_xlate );

		//  this shear balances the "trapezoid" around the y=0 axis (no change to the projection pt position)
		//  since we are redistributing the trapezoid, this affects the projection field of view (shear_amt)
		float shear_amt		= (max_slope + fabsf(min_slope))*0.5f - max_slope;
		max_slope			= max_slope + shear_amt;
		D3DXMATRIX trapezoid_shear	(	1.f, shear_amt, 0.f, 0.f,
										0.f,       1.f, 0.f, 0.f,
										0.f,       0.f, 1.f, 0.f,
										0.f,       0.f, 0.f, 1.f	);
		D3DXMatrixMultiply			( &trapezoid_space, &trapezoid_space, &trapezoid_shear );


		float z_aspect		= (frustumBox.max.z-frustumBox.min.z) / (frustumAABB2D.max.y-frustumAABB2D.min.y);

		//  perform a 2DH projection to 'unsqueeze' the top line.
		D3DXMATRIX trapezoid_projection(	xf/(xf-xn),     0.f,			0.f,						 1.f,
											0.f,			1.f/max_slope,	0.f,						0.f,
											0.f,			0.f,			1.f/(z_aspect*max_slope),	0.f,
											-xn*xf/(xf-xn),	0.f,			0.f,						0.f		);
		D3DXMatrixMultiply			( &trapezoid_space, &trapezoid_space, &trapezoid_projection );

		//  the x axis is compressed to [0..1] as a result of the projection, so expand it to [-1,1]
		D3DXMATRIX biasedScaleX		(	2.f,	0.f, 0.f, 0.f,
										0.f,	1.f, 0.f, 0.f,
										0.f,	0.f, 1.f, 0.f,
										-1.f,	0.f, 0.f, 1.f	);
		D3DXMatrixMultiply			( &trapezoid_space, &trapezoid_space, &biasedScaleX		);

		D3DXMatrixMultiply			( &m_LightViewProj, &m_View,		  &lightSpaceBasis	);
		D3DXMatrixMultiply			( &m_LightViewProj, &m_LightViewProj, &lightSpaceOrtho	);
		D3DXMatrixMultiply			( &m_LightViewProj, &m_LightViewProj, &trapezoid_space	);
	}

	// perform "refit" or "focusing" on relevant
	if	(ps_r2_ls_flags.test(R2FLAG_SUN_FOCUS))
	{
		Fmatrix&	xform	= *((Fmatrix*)&m_LightViewProj);
		Fbox3		b_casters, b_receivers;

		// casters
		b_casters.invalidate	();
		for		(int c=0; c<s_casters.size(); c++)		{
			Fvector3			pt;
			xform.transform		(pt,s_casters[c].min);
			b_casters.modify	(pt);
			xform.transform		(pt,s_casters[c].max);
			b_casters.modify	(pt);
		}

		// receivers
		b_receivers.invalidate	();
		for		(int c=0; c<s_receivers.size(); c++)	{
			Fvector3			pt;
			xform.transform		(pt,s_receivers[c].min);
			b_receivers.modify	(pt);
			xform.transform		(pt,s_receivers[c].max);
			b_receivers.modify	(pt);
		}

		// because caster points are from coarse representation only allow to "shrink" box, not grow
		// that is the same as if we first clip casters by frustum
		if (b_receivers.min.x<-1)	b_receivers.min.x	=-1;
		if (b_receivers.min.y<-1)	b_receivers.min.y	=-1;
		if (b_casters.min.z<0)		b_casters.min.z		=0;
		if (b_receivers.max.x>+1)	b_receivers.max.x	=+1;
		if (b_receivers.max.y>+1)	b_receivers.max.y	=+1;
		if (b_casters.max.z>+1)		b_casters.max.z		=+1;

		// refit?
		const float EPS				= 0.001f;
		D3DXMATRIX					refit;
		D3DXMatrixOrthoOffCenterLH	( &refit, b_receivers.min.x, b_receivers.max.x, b_receivers.min.y, b_receivers.max.y, b_casters.min.z-EPS, b_casters.max.z+EPS );
		D3DXMatrixMultiply			( &m_LightViewProj, &m_LightViewProj, &refit);
	}

	// Finalize & Cleanup
	fuckingsun->X.D.combine			= *((Fmatrix*)&m_LightViewProj);
	s_receivers.clear				();
	s_casters.clear					();

	// Render shadow-map
	//. !!! We should clip based on shrinked frustum (again)
	{
		bool	bNormal							= mapNormal[0].size() || mapMatrix[0].size();
		bool	bSpecial						= mapNormal[1].size() || mapMatrix[1].size() || mapSorted.size();
		if ( bNormal || bSpecial)	{
			Target.phase_smap_direct			(fuckingsun					);
			RCache.set_xform_world				(Fidentity					);
			RCache.set_xform_view				(Fidentity					);
			RCache.set_xform_project			(fuckingsun->X.D.combine	);	//.
			r_dsgraph_render_graph				(0);
			fuckingsun->X.D.transluent			= FALSE;
			if (bSpecial)						{
				fuckingsun->X.D.transluent			= TRUE;
				Target.phase_smap_direct_tsh		(fuckingsun);
				r_dsgraph_render_graph				(1);			// normal level, secondary priority
				r_dsgraph_render_sorted				( );			// strict-sorted geoms
			}
		}
	}

	// End SMAP-render
	{
		fuckingsun->svis.end					();
		r_pmask									(true,false);
	}

	// Accumulate
	Target.phase_accumulator	();
	Target.accum_direct			();
}

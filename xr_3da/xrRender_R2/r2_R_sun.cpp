#include "stdafx.h"
#include "..\igame_persistent.h"
#include "..\irenderable.h"

const	float	tweak_COP_initial_offs			= 100000.f	;
const	float	tweak_ortho_xform_initial_offs	= 1000.f	;	//. ?
const	float	tweak_guaranteed_range			= 20.f		;	//. ?

//////////////////////////////////////////////////////////////////////////
#define DW_AS_FLT(DW) (*(FLOAT*)&(DW))
#define FLT_AS_DW(F) (*(DWORD*)&(F))
#define FLT_SIGN(F) ((FLT_AS_DW(F) & 0x80000000L))
#define ALMOST_ZERO(F) ((FLT_AS_DW(F) & 0x7f800000L)==0)
#define IS_SPECIAL(F) ((FLT_AS_DW(F) & 0x7f800000L)==0x7f800000L)

//////////////////////////////////////////////////////////////////////////
struct Frustum
{
	Frustum		();
	Frustum		( const D3DXMATRIX* matrix );

	D3DXPLANE	camPlanes	[6];
	int			nVertexLUT	[6];
	D3DXVECTOR3 pntList		[8];
};
struct BoundingBox
{
	D3DXVECTOR3 minPt;
	D3DXVECTOR3 maxPt;

	BoundingBox(): minPt(1e33f, 1e33f, 1e33f), maxPt(-1e33f, -1e33f, -1e33f) { }
	BoundingBox( const BoundingBox& other ): minPt(other.minPt), maxPt(other.maxPt) { }

	explicit BoundingBox( const D3DXVECTOR3* points, UINT n ): minPt(1e33f, 1e33f, 1e33f), maxPt(-1e33f, -1e33f, -1e33f)
	{
		for ( unsigned int i=0; i<n; i++ )
			Merge( &points[i] );
	}

	explicit BoundingBox( const std::vector<D3DXVECTOR3>* points): minPt(1e33f, 1e33f, 1e33f), maxPt(-1e33f, -1e33f, -1e33f)
	{
		for ( unsigned int i=0; i<points->size(); i++ )
			Merge( &(*points)[i] );
	}
	explicit BoundingBox( const std::vector<BoundingBox>* boxes ): minPt(1e33f, 1e33f, 1e33f), maxPt(-1e33f, -1e33f, -1e33f) 
	{
		for (unsigned int i=0; i<boxes->size(); i++) 
		{
			Merge( &(*boxes)[i].maxPt );
			Merge( &(*boxes)[i].minPt );
		}
	}
	void Centroid( D3DXVECTOR3* vec) const { *vec = 0.5f*(minPt+maxPt); }
	void Merge( const D3DXVECTOR3* vec )
	{
		minPt.x = _min(minPt.x, vec->x);
		minPt.y = _min(minPt.y, vec->y);
		minPt.z = _min(minPt.z, vec->z);
		maxPt.x = _max(maxPt.x, vec->x);
		maxPt.y = _max(maxPt.y, vec->y);
		maxPt.z = _max(maxPt.z, vec->z);
	}
	D3DXVECTOR3 Point(int i) const { return D3DXVECTOR3( (i&1)?minPt.x:maxPt.x, (i&2)?minPt.y:maxPt.y, (i&4)?minPt.z:maxPt.z );  }
};

///////////////////////////////////////////////////////////////////////////
BOOL LineIntersection2D( D3DXVECTOR2* result, const D3DXVECTOR2* lineA, const D3DXVECTOR2* lineB )
{
	//  if the lines are parallel, the lines will not intersect in a point
	//  NOTE: assumes the rays are already normalized!!!!
	VERIFY		( _abs(D3DXVec2Dot(&lineA[1], &lineB[1]))<1.f );

	float x[2]	= { lineA[0].x, lineB[0].x };
	float y[2]	= { lineA[0].y, lineB[0].y };
	float dx[2] = { lineA[1].x, lineB[1].x };
	float dy[2] = { lineA[1].y, lineB[1].y };

	float x_diff = x[0] - x[1];
	float y_diff = y[0] - y[1];

	float s		= (x_diff - (dx[1]/dy[1])*y_diff) / ((dx[1]*dy[0]/dy[1])-dx[0]);
	float t		= (x_diff + s*dx[0]) / dx[1];

	*result		= lineA[0] + s*lineA[1];
	return		TRUE;
}
///////////////////////////////////////////////////////////////////////////
//  PlaneIntersection
//    computes the point where three planes intersect
//    returns whether or not the point exists.
static inline BOOL PlaneIntersection( D3DXVECTOR3* intersectPt, const D3DXPLANE* p0, const D3DXPLANE* p1, const D3DXPLANE* p2 )
{
	D3DXVECTOR3 n0( p0->a, p0->b, p0->c );
	D3DXVECTOR3 n1( p1->a, p1->b, p1->c );
	D3DXVECTOR3 n2( p2->a, p2->b, p2->c );

	D3DXVECTOR3 n1_n2, n2_n0, n0_n1;  

	D3DXVec3Cross( &n1_n2, &n1, &n2 );
	D3DXVec3Cross( &n2_n0, &n2, &n0 );
	D3DXVec3Cross( &n0_n1, &n0, &n1 );

	float cosTheta = D3DXVec3Dot( &n0, &n1_n2 );

	if ( ALMOST_ZERO(cosTheta) || IS_SPECIAL(cosTheta) )
		return FALSE;

	float secTheta = 1.f / cosTheta;

	n1_n2 = n1_n2 * p0->d;
	n2_n0 = n2_n0 * p1->d;
	n0_n1 = n0_n1 * p2->d;

	*intersectPt = -(n1_n2 + n2_n0 + n0_n1) * secTheta;
	return TRUE;
}

Frustum::Frustum() 
{
	for (int i=0; i<6; i++)
		camPlanes[i] = D3DXPLANE(0.f, 0.f, 0.f, 0.f);
}

//  build a frustum from a camera (projection, or viewProjection) matrix
Frustum::Frustum(const D3DXMATRIX* matrix)
{
	//  build a view frustum based on the current view & projection matrices...
	D3DXVECTOR4 column4( matrix->_14, matrix->_24, matrix->_34, matrix->_44 );
	D3DXVECTOR4 column1( matrix->_11, matrix->_21, matrix->_31, matrix->_41 );
	D3DXVECTOR4 column2( matrix->_12, matrix->_22, matrix->_32, matrix->_42 );
	D3DXVECTOR4 column3( matrix->_13, matrix->_23, matrix->_33, matrix->_43 );

	D3DXVECTOR4 planes[6];
	planes[0] = column4 - column1;  // left
	planes[1] = column4 + column1;  // right
	planes[2] = column4 - column2;  // bottom
	planes[3] = column4 + column2;  // top
	planes[4] = column4 - column3;  // near
	planes[5] = column4 + column3;  // far
	// ignore near & far plane

	int p;

	for (p=0; p<6; p++)  // normalize the planes
	{
		float dot = planes[p].x*planes[p].x + planes[p].y*planes[p].y + planes[p].z*planes[p].z;
		dot = 1.f / _sqrt(dot);
		planes[p] = planes[p] * dot;
	}

	for (p=0; p<6; p++)
		camPlanes[p] = D3DXPLANE( planes[p].x, planes[p].y, planes[p].z, planes[p].w );

	//  build a bit-field that will tell us the indices for the nearest and farthest vertices from each plane...
	for (int i=0; i<6; i++)
		nVertexLUT[i] = ((planes[i].x<0.f)?1:0) | ((planes[i].y<0.f)?2:0) | ((planes[i].z<0.f)?4:0);

	for (int i=0; i<8; i++)  // compute extrema
	{
		const D3DXPLANE& p0 = (i&1)?camPlanes[4] : camPlanes[5];
		const D3DXPLANE& p1 = (i&2)?camPlanes[3] : camPlanes[2];
		const D3DXPLANE& p2 = (i&4)?camPlanes[0] : camPlanes[1];
		PlaneIntersection( &pntList[i], &p0, &p1, &p2 );
	}
}

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
		for (int it=0; it<int(polys.size()); it++)
		{
			_poly&	base				= polys	[it];
			if (base.classify(cog)>0)	std::reverse(base.points.begin(),base.points.end());
		}

		// remove faceforward polys, build list of edges -> find open ones
		compute_planes	();
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
			P.points.push_back	(pend-1);	//p1 mod
			P.points.push_back	(pend-2);	//p0 mod
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

//////////////////////////////////////////////////////////////////////////
Fvector3		wform	(Fmatrix& m, Fvector3& v)
{
	Fvector4	r;
	r.x			= v.x*m._11 + v.y*m._21 + v.z*m._31 + m._41;
	r.y			= v.x*m._12 + v.y*m._22 + v.z*m._32 + m._42;
	r.z			= v.x*m._13 + v.y*m._23 + v.z*m._33 + m._43;
	r.w			= v.x*m._14 + v.y*m._24 + v.z*m._34 + m._44;
	// VERIFY		(r.w>0.f);
	Fvector3	r3 = { r.x/r.w, r.y/r.w, r.z/r.w };
	return		r3;
}

//////////////////////////////////////////////////////////////////////////
// OLES: naive 3D clipper - roubustness around 0, but works for this sample
// note: normals points to 'outside'
//////////////////////////////////////////////////////////////////////////
const	float	_eps	= 0.000001f;
struct	DumbClipper
{
	xr_vector<D3DXPLANE>	planes;
	BOOL					clip	(D3DXVECTOR3& p0, D3DXVECTOR3& p1)		// returns TRUE if result meaningfull
	{
		float		denum;
		D3DXVECTOR3	D;
		for (int it=0; it<int(planes.size()); it++)
		{
			D3DXPLANE&	P		= planes			[it];
			float		cls0	= D3DXPlaneDotCoord	(&P,&p0);
			float		cls1	= D3DXPlaneDotCoord	(&P,&p1);
			if (cls0>0 && cls1>0)	return			false;	// fully outside

			if (cls0>0)	{
				// clip p0
				D			= p1-p0;
				denum		= D3DXPlaneDotNormal(&P,&D);
				if (denum!=0) p0 += - D * cls0 / denum;
			}
			if (cls1>0)	{
				// clip p1
				D			= p0-p1;
				denum		= D3DXPlaneDotNormal(&P,&D);
				if (denum!=0) p1 += - D * cls1 / denum;
			}
		}
		return	true;
	}
	D3DXVECTOR3			point		(Fbox& bb, int i) const { return D3DXVECTOR3( (i&1)?bb.min.x:bb.max.x, (i&2)?bb.min.y:bb.max.y, (i&4)?bb.min.z:bb.max.z );  }
	Fbox				clipped_AABB(xr_vector<Fbox>& src, Fmatrix& xf)
	{
		Fbox3		result;		result.invalidate		();
		for (int it=0; it<int(src.size()); it++)		{
			Fbox&	bb		= src	[it];
			for (int c0=0; c0<8; c0++)
			{
				for (int c1=0; c1<8; c1++)
				{
					if (c0==c1)			continue;
					D3DXVECTOR3		p0	= point(bb,c0);
					D3DXVECTOR3		p1	= point(bb,c1);
					if (!clip(p0,p1))	continue;
					Fvector			x0	= wform			(xf,*((Fvector*)(&p0)));
					Fvector			x1	= wform			(xf,*((Fvector*)(&p1)));
					result.modify	(x0	);
					result.modify	(x1	);
				}
			}
		}
		return			result;
	}
};

template <class _Tp>
inline const _Tp& min(const _Tp& __a, const _Tp& __b) {
	return __b < __a ? __b : __a;
}
template <class _Tp>
inline const _Tp& max(const _Tp& __a, const _Tp& __b) {
	return  __a < __b ? __b : __a;
}

xr_vector<Fbox>	s_casters;
D3DXVECTOR2 BuildTSMProjectionMatrix_caster_depth_bounds(D3DXMATRIX& lightSpaceBasis)
{
	float		min_z = 1e32f,	max_z=-1e32f;
	D3DXMATRIX	minmax_xf;
	D3DXMatrixMultiply	(&minmax_xf,(D3DXMATRIX*)&Device.mView,&lightSpaceBasis);
	Fmatrix&	minmax_xform = *((Fmatrix*)&minmax_xf);
	for		(u32 c=0; c<s_casters.size(); c++)
	{
		Fvector3	pt;
		for			(int e=0; e<8; e++)	{
			s_casters[c].getpoint	(e,pt);
			pt		= wform			(minmax_xform, pt);
			min_z	= _min			( min_z, pt.z );
			max_z	= _max			( max_z, pt.z );
		}
	}
	return D3DXVECTOR2(min_z,max_z);
}

void CRender::render_sun				()
{
	light*			fuckingsun			= Lights.sun_adapted	;
	D3DXMATRIX		m_LightViewProj		;

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

	Fmatrix	ex_project, ex_full, ex_full_inverse;
	{
		ex_project.build_projection	(deg2rad(Device.fFOV*Device.fASPECT),Device.fASPECT,tweak_guaranteed_range,g_pGamePersistent->Environment.CurrentEnv.far_plane); 
		ex_full.mul					(ex_project,Device.mView);
		D3DXMatrixInverse			((D3DXMATRIX*)&ex_full_inverse,0,(D3DXMATRIX*)&ex_full);
	}

	// Compute volume(s) - something like a frustum for infinite directional light
	// Also compute virtual light position and sector it is inside
	CFrustum					cull_frustum	;
	xr_vector<Fplane>			cull_planes		;
	Fvector3					cull_COP		;
	CSector*					cull_sector		;
	Fmatrix						cull_xform		;
	{
		FPU::m64r					();
		// Lets begin from base frustum
		Fmatrix		fullxform_inv	= ex_full_inverse;
		DumbConvexVolume			hull;
		{
			hull.points.reserve		(8);
			for						(int p=0; p<8; p++)	{
				Fvector3				xf	= wform		(fullxform_inv,corners[p]);
				hull.points.push_back	(xf);
			}
			for (int plane=0; plane<6; plane++)	{
				hull.polys.push_back(DumbConvexVolume::_poly());
				for (int pt=0; pt<4; pt++)	
					hull.polys.back().points.push_back(facetable[plane][pt]);
			}
		}
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
		cull_COP.mad				(Device.vCameraPosition, fuckingsun->direction, -tweak_COP_initial_offs	);

		// Create frustum for query
		cull_frustum._clear			();
		for (int p=0; p<cull_planes.size(); p++)
			cull_frustum._add		(cull_planes[p]);

		// Create approximate ortho-xform
		// view: auto find 'up' and 'right' vectors
		Fmatrix						mdir_View, mdir_Project;
		Fvector						L_dir,L_up,L_right,L_pos;
		L_pos.set					(fuckingsun->position);
		L_dir.set					(fuckingsun->direction).normalize	();
		L_up.set					(0,1,0);					if (_abs(L_up.dotproduct(L_dir))>.99f)	L_up.set(0,0,1);
		L_right.crossproduct		(L_up,L_dir).normalize		();
		L_up.crossproduct			(L_dir,L_right).normalize	();
		mdir_View.build_camera_dir	(L_pos,L_dir,L_up);

		// projection: box
		Fbox	frustum_bb;			frustum_bb.invalidate();
		for (int it=0; it<8; it++)	{
			Fvector	xf	= wform		(mdir_View,hull.points[it]);
			frustum_bb.modify		(xf);
		}
		Fbox&	bb					= frustum_bb;
				bb.grow				(EPS);
		D3DXMatrixOrthoOffCenterLH	((D3DXMATRIX*)&mdir_Project,bb.min.x,bb.max.x,  bb.min.y,bb.max.y,  bb.min.z-tweak_ortho_xform_initial_offs,bb.max.z);

		// full-xform
		cull_xform.mul				(mdir_Project,mdir_View);
		FPU::m24r					();
	}

	// Begin SMAP-render
	{
		HOM.Disable								();
		phase									= PHASE_SMAP_D;
		if (RImplementation.o.Tshadows)	r_pmask	(true,true	);
		else							r_pmask	(true,false	);
//		fuckingsun->svis.begin					();
	}

	// Fill the database
	xr_vector<Fbox3>&		s_receivers			= main_coarse_structure;
	s_casters.reserve							(s_receivers.size());
	set_Recorder								(&s_casters);
	r_dsgraph_render_subspace					(cull_sector, &cull_frustum, cull_xform, cull_COP, TRUE);
//.	r_dsgraph_render_subspace					(cull_sector, cull_xform, cull_COP, TRUE);
	set_Recorder								(NULL);

	//	Prepare to interact with D3DX code
	const D3DXMATRIX&	m_View			= *((D3DXMATRIX*)(&Device.mView));
	const D3DXMATRIX&	m_Projection	= *((D3DXMATRIX*)(&Device.mProject));
	const D3DXVECTOR3	m_lightDir		= -D3DXVECTOR3(fuckingsun->direction.x,fuckingsun->direction.y,fuckingsun->direction.z);

	//  these are the limits specified by the physical camera
	//  gamma is the "tilt angle" between the light and the view direction.
	float m_fCosGamma = m_lightDir.x * m_View._13	+
						m_lightDir.y * m_View._23	+
						m_lightDir.z * m_View._33	;
	float m_fTSM_Delta= ps_r2_sun_projection;

	// Compute REAL sheared xform based on receivers/casters information
	FPU::m64r			();
	if	( _abs(m_fCosGamma) < 0.99f && ps_r2_ls_flags.test(R2FLAG_SUN_TSM))
	{
		//  get the near and the far plane (points) in eye space.
		D3DXVECTOR3 frustumPnts[8];

		Frustum eyeFrustum	( &m_Projection );  // autocomputes all the extrema points

		for ( int i=0; i<4; i++ )
		{
			frustumPnts[i]   = eyeFrustum.pntList[(i<<1)];       // far plane
			frustumPnts[i+4] = eyeFrustum.pntList[(i<<1) | 0x1]; // near plane
		}

		//   we need to transform the eye into the light's post-projective space.
		//   however, the sun is a directional light, so we first need to find an appropriate
		//   rotate/translate matrix, before constructing an ortho projection.
		//   this matrix is a variant of "light space" from LSPSMs, with the Y and Z axes permuted

		D3DXVECTOR3			leftVector, upVector, viewVector;
		const D3DXVECTOR3	eyeVector( 0.f, 0.f, -1.f );  //  eye is always -Z in eye space

		//  code copied straight from BuildLSPSMProjectionMatrix
		D3DXVec3TransformNormal( &upVector, &m_lightDir, &m_View );  // lightDir is defined in eye space, so xform it
		D3DXVec3Cross( &leftVector, &upVector, &eyeVector );
		D3DXVec3Normalize( &leftVector, &leftVector );
		D3DXVec3Cross( &viewVector, &upVector, &leftVector );

		D3DXMATRIX lightSpaceBasis;  
		lightSpaceBasis._11 = leftVector.x; lightSpaceBasis._12 = viewVector.x; lightSpaceBasis._13 = -upVector.x; lightSpaceBasis._14 = 0.f;
		lightSpaceBasis._21 = leftVector.y; lightSpaceBasis._22 = viewVector.y; lightSpaceBasis._23 = -upVector.y; lightSpaceBasis._24 = 0.f;
		lightSpaceBasis._31 = leftVector.z; lightSpaceBasis._32 = viewVector.z; lightSpaceBasis._33 = -upVector.z; lightSpaceBasis._34 = 0.f;
		lightSpaceBasis._41 = 0.f;          lightSpaceBasis._42 = 0.f;          lightSpaceBasis._43 = 0.f;        lightSpaceBasis._44 = 1.f;

		//  rotate the view frustum into light space
		D3DXVec3TransformCoordArray( frustumPnts, sizeof(D3DXVECTOR3), frustumPnts, sizeof(D3DXVECTOR3), &lightSpaceBasis, sizeof(frustumPnts)/sizeof(D3DXVECTOR3) );

		//  build an off-center ortho projection that translates and scales the eye frustum's 3D AABB to the unit cube
		BoundingBox frustumBox	( frustumPnts, sizeof(frustumPnts) / sizeof(D3DXVECTOR3) );

		//  also - transform the shadow caster bounding boxes into light projective space.  we want to translate along the Z axis so that
		//  all shadow casters are in front of the near plane.
		D3DXVECTOR2	depthbounds = BuildTSMProjectionMatrix_caster_depth_bounds	(lightSpaceBasis);

		float min_z = min( depthbounds.x, frustumBox.minPt.z );
		float max_z = max( depthbounds.y, frustumBox.maxPt.z );

		if ( min_z <= 1.f )	//?
		{
			D3DXMATRIX lightSpaceTranslate;
			D3DXMatrixTranslation( &lightSpaceTranslate, 0.f, 0.f, -min_z + 1.f );
			max_z = -min_z + max_z + 1.f;
			min_z = 1.f;
			D3DXMatrixMultiply ( &lightSpaceBasis, &lightSpaceBasis, &lightSpaceTranslate );
			D3DXVec3TransformCoordArray( frustumPnts, sizeof(D3DXVECTOR3), frustumPnts, sizeof(D3DXVECTOR3), &lightSpaceTranslate, sizeof(frustumPnts)/sizeof(D3DXVECTOR3) );
			frustumBox = BoundingBox( frustumPnts, sizeof(frustumPnts)/sizeof(D3DXVECTOR3) );
		}

		D3DXMATRIX lightSpaceOrtho;
		D3DXMatrixOrthoOffCenterLH( &lightSpaceOrtho, frustumBox.minPt.x, frustumBox.maxPt.x, frustumBox.minPt.y, frustumBox.maxPt.y, min_z, max_z );

		//  transform the view frustum by the new matrix
		D3DXVec3TransformCoordArray( frustumPnts, sizeof(D3DXVECTOR3), frustumPnts, sizeof(D3DXVECTOR3), &lightSpaceOrtho, sizeof(frustumPnts)/sizeof(D3DXVECTOR3) );

		D3DXVECTOR2 centerPts	[2];
		//  near plane
		centerPts[0].x = 0.25f * (frustumPnts[4].x + frustumPnts[5].x + frustumPnts[6].x + frustumPnts[7].x);
		centerPts[0].y = 0.25f * (frustumPnts[4].y + frustumPnts[5].y + frustumPnts[6].y + frustumPnts[7].y);
		//  far plane
		centerPts[1].x = 0.25f * (frustumPnts[0].x + frustumPnts[1].x + frustumPnts[2].x + frustumPnts[3].x);
		centerPts[1].y = 0.25f * (frustumPnts[0].y + frustumPnts[1].y + frustumPnts[2].y + frustumPnts[3].y);

		D3DXVECTOR2 centerOrig = (centerPts[0] + centerPts[1])*0.5f;

		D3DXMATRIX trapezoid_space;

		D3DXMATRIX xlate_center(           1.f,           0.f, 0.f, 0.f,
			0.f,           1.f, 0.f, 0.f,
			0.f,           0.f, 1.f, 0.f,
			-centerOrig.x, -centerOrig.y, 0.f, 1.f );

		float half_center_len = D3DXVec2Length( &D3DXVECTOR2(centerPts[1] - centerOrig) );
		float x_len = centerPts[1].x - centerOrig.x;
		float y_len = centerPts[1].y - centerOrig.y;

		float cos_theta = x_len / half_center_len;
		float sin_theta = y_len / half_center_len;

		D3DXMATRIX rot_center( cos_theta, -sin_theta, 0.f, 0.f,
			sin_theta,  cos_theta, 0.f, 0.f,
			0.f,        0.f, 1.f, 0.f,
			0.f,        0.f, 0.f, 1.f );

		//  this matrix transforms the center line to y=0.
		//  since Top and Base are orthogonal to Center, we can skip computing the convex hull, and instead
		//  just find the view frustum X-axis extrema.  The most negative is Top, the most positive is Base
		//  Point Q (trapezoid projection point) will be a point on the y=0 line.
		D3DXMatrixMultiply( &trapezoid_space, &xlate_center, &rot_center );
		D3DXVec3TransformCoordArray( frustumPnts, sizeof(D3DXVECTOR3), frustumPnts, sizeof(D3DXVECTOR3), &trapezoid_space, sizeof(frustumPnts)/sizeof(D3DXVECTOR3) );

		BoundingBox frustumAABB2D( frustumPnts, sizeof(frustumPnts)/sizeof(D3DXVECTOR3) );

		float x_scale = max( fabsf(frustumAABB2D.maxPt.x), fabsf(frustumAABB2D.minPt.x) );
		float y_scale = max( fabsf(frustumAABB2D.maxPt.y), fabsf(frustumAABB2D.minPt.y) );
		x_scale = 1.f/x_scale;
		y_scale = 1.f/y_scale;

		//  maximize the area occupied by the bounding box
		D3DXMATRIX scale_center( x_scale, 0.f,		0.f, 0.f,
			0.f, y_scale,	0.f, 0.f,
			0.f,     0.f,	1.f, 0.f,
			0.f,     0.f,	0.f, 1.f );

		D3DXMatrixMultiply( &trapezoid_space, &trapezoid_space, &scale_center );

		//  scale the frustum AABB up by these amounts (keep all values in the same space)
		frustumAABB2D.minPt.x *= x_scale;
		frustumAABB2D.maxPt.x *= x_scale;
		frustumAABB2D.minPt.y *= y_scale;
		frustumAABB2D.maxPt.y *= y_scale;

		//  compute eta.
		float lambda		= frustumAABB2D.maxPt.x - frustumAABB2D.minPt.x;
		float delta_proj	= m_fTSM_Delta * lambda;	//focusPt.x - frustumAABB2D.minPt.x;
		const float xi		= -0.6;						// - 0.6f;  // 80% line
		float		eta		= (lambda*delta_proj*(1.f+xi)) / (lambda*(1.f-xi)-2.f*delta_proj);

		//  compute the projection point a distance eta from the top line.  this point is on the center line, y=0
		D3DXVECTOR2 projectionPtQ( frustumAABB2D.maxPt.x + eta, 0.f );

		//  find the maximum slope from the projection point to any point in the frustum.  this will be the
		//  projection field-of-view
		float max_slope = -1e32f;
		float min_slope =  1e32f;

		for ( int i=0; i < sizeof(frustumPnts)/sizeof(D3DXVECTOR3); i++ )
		{
			D3DXVECTOR2 tmp( frustumPnts[i].x*x_scale, frustumPnts[i].y*y_scale );
			float x_dist = tmp.x - projectionPtQ.x;
			if ( !(ALMOST_ZERO(tmp.y) || ALMOST_ZERO(x_dist)))
			{
				max_slope = max(max_slope, tmp.y/x_dist);
				min_slope = min(min_slope, tmp.y/x_dist);
			}
		}

		float xn = eta;
		float xf = lambda + eta;

		D3DXMATRIX ptQ_xlate(-1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			projectionPtQ.x, 0.f, 0.f, 1.f );
		D3DXMatrixMultiply( &trapezoid_space, &trapezoid_space, &ptQ_xlate );

		//  this shear balances the "trapezoid" around the y=0 axis (no change to the projection pt position)
		//  since we are redistributing the trapezoid, this affects the projection field of view (shear_amt)
		float shear_amt = (max_slope + fabsf(min_slope))*0.5f - max_slope;
		max_slope = max_slope + shear_amt;

		D3DXMATRIX trapezoid_shear( 1.f, shear_amt, 0.f, 0.f,
			0.f,       1.f, 0.f, 0.f,
			0.f,       0.f, 1.f, 0.f,
			0.f,       0.f, 0.f, 1.f );

		D3DXMatrixMultiply( &trapezoid_space, &trapezoid_space, &trapezoid_shear );


		float z_aspect = (frustumBox.maxPt.z-frustumBox.minPt.z) / (frustumAABB2D.maxPt.y-frustumAABB2D.minPt.y);

		//  perform a 2DH projection to 'unsqueeze' the top line.
		D3DXMATRIX trapezoid_projection(  xf/(xf-xn),          0.f, 0.f, 1.f,
			0.f, 1.f/max_slope, 0.f, 0.f,
			0.f,           0.f, 1.f/(z_aspect*max_slope), 0.f,
			-xn*xf/(xf-xn),           0.f, 0.f, 0.f );

		D3DXMatrixMultiply( &trapezoid_space, &trapezoid_space, &trapezoid_projection );

		//  the x axis is compressed to [0..1] as a result of the projection, so expand it to [-1,1]
		D3DXMATRIX biasedScaleX(	2.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			-1.f, 0.f, 0.f, 1.f );
		D3DXMatrixMultiply	( &trapezoid_space, &trapezoid_space, &biasedScaleX		);

		D3DXMatrixMultiply	( &m_LightViewProj, &m_View,		  &lightSpaceBasis	);
		D3DXMatrixMultiply	( &m_LightViewProj, &m_LightViewProj, &lightSpaceOrtho	);
		D3DXMatrixMultiply	( &m_LightViewProj, &m_LightViewProj, &trapezoid_space	);
	} else {
		m_LightViewProj				= *((D3DXMATRIX*)(&cull_xform));
	}
	FPU::m24r				();

	// perform "refit" or "focusing" on relevant
	if	(ps_r2_ls_flags.test(R2FLAG_SUN_FOCUS))
	{
		FPU::m64r				();

		// create clipper
		DumbClipper	view_clipper;
		Fmatrix&	xform		= *((Fmatrix*)&m_LightViewProj);
		CFrustum	view_planes	; view_planes.CreateFromMatrix(Device.mFullTransform,FRUSTUM_P_ALL);
		for		(u32 p=0; p<view_planes.p_count; p++)
		{
			Fplane&		P	= view_planes.planes	[p];
			view_clipper.planes.push_back(D3DXPLANE(P.n.x,P.n.y,P.n.z,P.d));
		}

		// 
		Fbox3		b_casters, b_receivers;
		Fvector3	pt			;

		// casters
		b_casters.invalidate	();
		for		(u32 c=0; c<s_casters.size(); c++)		{
			for		(int e=0; e<8; e++)
			{
				s_casters[c].getpoint	(e,pt);
				pt				= wform	(xform,pt);
				b_casters.modify		(pt);
			}
		}

		// receivers
		b_receivers.invalidate	();
		b_receivers		= view_clipper.clipped_AABB	(s_receivers,xform);
		Fmatrix	ex_project, ex_full, ex_full_inverse;
		{
			ex_project.build_projection	(deg2rad(Device.fFOV*Device.fASPECT),Device.fASPECT,VIEWPORT_NEAR,tweak_guaranteed_range); 
			ex_full.mul					(ex_project,Device.mView);
			D3DXMatrixInverse			((D3DXMATRIX*)&ex_full_inverse,0,(D3DXMATRIX*)&ex_full);
		}
		for		(int e=0; e<8; e++)
		{
			pt				= wform	(ex_full_inverse,corners[e]);	// world space
			pt				= wform	(xform,pt);						// trapezoid space
			b_receivers.modify		(pt);
		}

		// some tweaking
		b_casters.grow				(EPS);
		b_receivers.grow			(EPS);

		// because caster points are from coarse representation only allow to "shrink" box, not grow
		// that is the same as if we first clip casters by frustum
		if (b_receivers.min.x<-1)	b_receivers.min.x	=-1;
		if (b_receivers.min.y<-1)	b_receivers.min.y	=-1;
		if (b_casters.min.z<0)		b_casters.min.z		=0;
		if (b_receivers.max.x>+1)	b_receivers.max.x	=+1;
		if (b_receivers.max.y>+1)	b_receivers.max.y	=+1;
		if (b_casters.max.z>+1)		b_casters.max.z		=+1;

		// refit?
		/*
		const float EPS				= 0.001f;
		D3DXMATRIX					refit;
		D3DXMatrixOrthoOffCenterLH	( &refit, b_receivers.min.x, b_receivers.max.x, b_receivers.min.y, b_receivers.max.y, b_casters.min.z-EPS, b_casters.max.z+EPS );
		D3DXMatrixMultiply			( &m_LightViewProj, &m_LightViewProj, &refit);
		*/

		float boxWidth  = b_receivers.max.x - b_receivers.min.x;
		float boxHeight = b_receivers.max.y - b_receivers.min.y;
		//  the divide by two's cancel out in the translation, but included for clarity
		float boxX		= (b_receivers.max.x+b_receivers.min.x) / 2.f;
		float boxY		= (b_receivers.max.y+b_receivers.min.y) / 2.f;
		D3DXMATRIX trapezoidUnitCube	( 2.f/boxWidth,			0.f,					0.f, 0.f,
													0.f,		2.f/boxHeight,			0.f, 0.f,
													0.f,		0.f,					1.f, 0.f,
										-2.f*boxX/boxWidth,		-2.f*boxY/boxHeight,	0.f, 1.f );
		D3DXMatrixMultiply			( &m_LightViewProj, &m_LightViewProj, &trapezoidUnitCube);
		//D3DXMatrixMultiply( &trapezoid_space, &trapezoid_space, &trapezoidUnitCube );
		FPU::m24r					();
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
			RCache.set_xform_project			(fuckingsun->X.D.combine	);	
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
//		fuckingsun->svis.end					();
		r_pmask									(true,false);
	}

	// Accumulate
	Target.phase_accumulator	();
	Target.accum_direct			(SE_SUN_FAR);

	// Restore XForms
	RCache.set_xform_world		(Fidentity			);
	RCache.set_xform_view		(Device.mView		);
	RCache.set_xform_project	(Device.mProject	);
}

void CRender::render_sun_near	()
{
	light*			fuckingsun			= Lights.sun_adapted	;
	D3DXMATRIX		m_LightViewProj		;

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

	Fmatrix	ex_project, ex_full, ex_full_inverse;
	{
		ex_project.build_projection	(deg2rad(Device.fFOV*Device.fASPECT),Device.fASPECT,VIEWPORT_NEAR,ps_r2_sun_near); 
		ex_full.mul					(ex_project,Device.mView);
		D3DXMatrixInverse			((D3DXMATRIX*)&ex_full_inverse,0,(D3DXMATRIX*)&ex_full);
	}

	// Compute volume(s) - something like a frustum for infinite directional light
	// Also compute virtual light position and sector it is inside
	CFrustum					cull_frustum;
	xr_vector<Fplane>			cull_planes;
	Fvector3					cull_COP;
	CSector*					cull_sector;
	Fmatrix						cull_xform;
	{
		FPU::m64r					();
		// Lets begin from base frustum
		Fmatrix		fullxform_inv	= ex_full_inverse;
		DumbConvexVolume			hull;
		{
			hull.points.reserve		(9);
			for						(int p=0; p<8; p++)	{
				Fvector3				xf	= wform		(fullxform_inv,corners[p]);
				hull.points.push_back	(xf);
			}
			for (int plane=0; plane<6; plane++)	{
				hull.polys.push_back(DumbConvexVolume::_poly());
				for (int pt=0; pt<4; pt++)	
					hull.polys.back().points.push_back(facetable[plane][pt]);
			}
		}
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
		cull_COP.mad				(Device.vCameraPosition, fuckingsun->direction, -tweak_COP_initial_offs	);

		// Create frustum for query
		cull_frustum._clear			();
		for (int p=0; p<cull_planes.size(); p++)
			cull_frustum._add		(cull_planes[p]);

		// Create approximate ortho-xform
		// view: auto find 'up' and 'right' vectors
		Fmatrix						mdir_View, mdir_Project;
		Fvector						L_dir,L_up,L_right,L_pos;
		L_pos.set					(fuckingsun->position);
		L_dir.set					(fuckingsun->direction).normalize	();
		L_right.set					(1,0,0);					if (_abs(L_right.dotproduct(L_dir))>.99f)	L_right.set(0,0,1);
		L_up.crossproduct			(L_dir,L_right).normalize	();
		L_right.crossproduct		(L_up,L_dir).normalize		();
		mdir_View.build_camera_dir	(L_pos,L_dir,L_up);

		// projection: box
		float	sperical_range		= ps_r2_sun_near*1.414213562373f+EPS;	// sqrt(2)
		Fbox	frustum_bb;			frustum_bb.invalidate	();
		hull.points.push_back		(Device.vCameraPosition);
		for (int it=0; it<9; it++)	{
			Fvector	xf	= wform		(mdir_View,hull.points[it]);
			frustum_bb.modify		(xf);
		}
		float	size_x				= frustum_bb.max.x - frustum_bb.min.x;
		float	size_y				= frustum_bb.max.y - frustum_bb.min.y;
		float	diff_x				= (sperical_range - size_x)/2.f;
		float	diff_y				= (sperical_range - size_y)/2.f;
		frustum_bb.min.x -= diff_x; frustum_bb.max.x += diff_x;
		frustum_bb.min.y -= diff_y; frustum_bb.max.y += diff_y;
		Fbox&	bb					= frustum_bb;
		D3DXMatrixOrthoOffCenterLH	((D3DXMATRIX*)&mdir_Project,bb.min.x,bb.max.x,  bb.min.y,bb.max.y,  bb.min.z-tweak_ortho_xform_initial_offs,bb.max.z);

		// build viewport xform
		float	view_dim			= float(RImplementation.o.smapsize);
		Fmatrix	m_viewport			= {
			view_dim/2.f,	0.0f,				0.0f,		0.0f,
			0.0f,			-view_dim/2.f,		0.0f,		0.0f,
			0.0f,			0.0f,				1.0f,		0.0f,
			view_dim/2.f,	view_dim/2.f,		0.0f,		1.0f
		};
		Fmatrix				m_viewport_inv;
		D3DXMatrixInverse	((D3DXMATRIX*)&m_viewport_inv,0,(D3DXMATRIX*)&m_viewport);

		// snap view-position to pixel
		cull_xform.mul		(mdir_Project,mdir_View	);
		Fvector cam_proj	= wform		(cull_xform,Device.vCameraPosition	);
		Fvector	cam_pixel	= wform		(m_viewport,cam_proj				);
		cam_pixel.x			= floorf	(cam_pixel.x);
		cam_pixel.y			= floorf	(cam_pixel.y);
		Fvector cam_snapped	= wform		(m_viewport_inv,cam_pixel);
		Fvector diff;		diff.sub	(cam_snapped,cam_proj				);
		Fmatrix adjust;		adjust.translate(diff);
		cull_xform.mulA		(adjust);

		// full-xform
		FPU::m24r					();
	}

	// Begin SMAP-render
	{
		HOM.Disable								();
		phase									= PHASE_SMAP_D;
		if (RImplementation.o.Tshadows)	r_pmask	(true,true	);
		else							r_pmask	(true,false	);
//		fuckingsun->svis.begin					();
	}

	// Fill the database
	xr_vector<Fbox3>&		s_receivers			= main_coarse_structure;
	s_casters.reserve							(s_receivers.size());
	set_Recorder								(&s_casters);
	r_dsgraph_render_subspace					(cull_sector, &cull_frustum, cull_xform, cull_COP, TRUE);
	set_Recorder								(NULL);

	// Finalize & Cleanup
	fuckingsun->X.D.combine			= cull_xform;	//*((Fmatrix*)&m_LightViewProj);
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
			RCache.set_xform_project			(fuckingsun->X.D.combine	);	
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
//		fuckingsun->svis.end					();
		r_pmask									(true,false);
	}

	// Accumulate
	Target.phase_accumulator	();
	Target.accum_direct			(SE_SUN_NEAR);

	// Restore XForms
	RCache.set_xform_world		(Fidentity			);
	RCache.set_xform_view		(Device.mView		);
	RCache.set_xform_project	(Device.mProject	);
}

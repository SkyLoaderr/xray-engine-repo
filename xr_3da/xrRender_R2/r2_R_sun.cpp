#include "stdafx.h"
#include "..\irenderable.h"

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
	hull.points.reserve		(8);
	for						(int p=0; p<8; p++)	{
		Fvector3				xf;
		fullxform_inv.transform	(xf,corners[p]);
		hull.points.push_back	(xf);
	}
	for (int plane=0; plane<6; plane++)	{
		hull.polys.push_back(DumbConvexVolume::_poly());
		for (int pt=0; pt<4; pt++)	
			hull.polys.back().points.push_back(facetable[plane][pt]);
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
		cull_COP.mad				(Device.vCameraPosition, fuckingsun->direction, - 100000.f);

		// Create frustum for query
		for (int p=0; p<cull_planes.size(); p++)
			cull_frustum._add		(cull_planes[p]);
	}


	// Fill the database
	{
		HOM.Disable								();
		phase									= PHASE_SMAP_S;
		if (RImplementation.o.Tshadows)	r_pmask	(true,true	);
		else							r_pmask	(true,false	);
		fuckingsun->svis.begin					();
		r_dsgraph_render_subspace				(cull_sector, &cull_frustum, fuckingsun->X.S.combine, cull_COP, TRUE);
		bool	bNormal							= mapNormal[0].size() || mapMatrix[0].size();
		bool	bSpecial						= mapNormal[1].size() || mapMatrix[1].size() || mapSorted.size();
		if ( bNormal || bSpecial)	{
			Target.phase_smap_direct			(fuckingsun		);
			RCache.set_xform_world				(Fidentity		);
			RCache.set_xform_view				(fuckingsun->X.D.view		);
			RCache.set_xform_project			(fuckingsun->X.D.project	);
			r_dsgraph_render_graph				(0);
			fuckingsun->X.D.transluent			= FALSE;
			if (bSpecial)						{
				fuckingsun->X.D.transluent			= TRUE;
				Target.phase_smap_direct_tsh		(fuckingsun);
				r_dsgraph_render_graph				(1);			// normal level, secondary priority
				r_dsgraph_render_sorted				( );			// strict-sorted geoms
			}
		}
		L->svis.end								();
		r_pmask									(true,false);
	}
}

#include "StdAfx.h"
#include "light_render_direct.h"

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
			_poly&			P	= polys[it];
			Fvector3		t1	= points[P.points[0]]-points[P.points[1]], t2 = points[P.points[0]]-points[P.points[2]];
			P.planeN.crossproduct	(t1,t2);
			D3DXVec3Cross		(&P.planeN,&t1,&t2);
			D3DXVec3Normalize	(&P.planeN,&P.planeN);
			P.planeD			= -D3DXVec3Dot(&P.planeN,&points[P.points[0]]);
		}
	}
	void				compute_receiver_model	(DumbClipper& dest)
	{
		// Planes and Export
		compute_planes	();
		for (int it=0; it<int(polys.size()); it++)
		{
			_poly&			P	= polys[it];
			dest.planes.push_back(D3DXPLANE(P.planeN.x,P.planeN.y,P.planeN.z,P.planeD));
		}
	}
	void				compute_caster_model	(DumbClipper& dest, Fvector3 direction)
	{
		// COG
		Fvector3	cog(0,0,0);
		for		(int it=0; it<int(points.size()); it++)	cog+=points[it];
		cog		/= float(points.size());

		// planes
		compute_planes	();

		// remove faceforward polys, build list of edges -> find open ones
		for (int it=0; it<int(polys.size()); it++)
		{
			_poly&	base		= polys	[it];
			assert	(base.classify(cog)<0);					// debug

			int		marker		= (D3DXVec3Dot(&base.planeN,&direction)<=0)?-1:1;

			// register edges
			xr_vector<int>&	plist		= polys[it].points;
			for (int p=0; p<int(plist.size()); p++)
			{
				_edge	E		(plist[p],plist[ (p+1)%plist.size() ], marker);
				bool	found	= false;
				for (int e=0; e<int(edges.size()); e++)	
					if (edges[e].equal(E))	{ edges[e].counter += marker; found=true; break; }
					if		(!found)	edges.push_back	(E);
			}

			// remove if unused
			if (marker>0)	{
				polys.erase(polys.begin()+it);
				it--;
			}
		}

		// Extend model to infinity, the volume is not capped, so this is indeed up to infinity
		for (int e=0; e<int(edges.size()); e++)
		{
			if	(edges[e].counter != 0)	continue;
			_edge&		E		= edges[e];
			points.push_back	(points[E.p0]-direction);
			points.push_back	(points[E.p1]-direction);
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
			if (base.classify(cog)>0)	reverse(base.points.begin(),base.points.end());
		}
		compute_planes	();

		// Export
		for (int it=0; it<int(polys.size()); it++)
		{
			_poly&			P	= polys[it];
			dest.planes.push_back(D3DXPLANE(P.planeN.x,P.planeN.y,P.planeN.z,P.planeD));
		}
	}
};

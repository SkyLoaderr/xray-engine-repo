#include "stdafx.h"
#include "compiler.h"
#include "MgcAppr3DPlaneFit.h"
#include "cl_defs.h"
#include "cl_intersect.h"

const float	RCAST_DepthValid = 0.2f;

IC void BoxQuery(Fbox& BB, bool exact)
{
	if (exact) 	XRC.BBoxMode	(BBOX_TRITEST);
	else		XRC.BBoxMode	(0);
	XRC.BBoxCollide	(precalc_identity,&Level,precalc_identity,BB);
}

struct tri {
	Fvector v[3];
};

BOOL	ValidNode(Node& N)
{
	// *** Query and cache polygons for ray-casting
	Fvector	PointUp;		PointUp.set(N.Pos);		PointUp.y	+= RCAST_Depth/2;
	Fvector	PointDown;		PointDown.set(N.Pos);	PointDown.y	-= RCAST_Depth/2;

	Fbox	BB;				BB.set	(PointUp,PointUp);		BB.grow(g_params.fPatchSize/2);	// box 1
	Fbox	B2;				B2.set	(PointDown,PointDown);	B2.grow(g_params.fPatchSize/2);	// box 2
	BB.merge(B2			);
	BoxQuery(BB,false	);
	DWORD	dwCount = XRC.GetBBoxContactCount();
	if (dwCount==0)	{
		Log("chasm1");
		return FALSE;			// chasm?
	}

	// *** Transfer triangles and compute sector
	R_ASSERT(dwCount<RCAST_MaxTris);
	static svector<tri,RCAST_MaxTris> tris;		tris.clear();
	for (DWORD i=0; i<dwCount; i++)
	{
		tri&		D = tris.last();
		RAPID::tri&	T = *(Level.GetTris()+XRC.BBoxContact[i].id);

		D.v[0].set	(*T.verts[0]);
		D.v[1].set	(*T.verts[1]);
		D.v[2].set	(*T.verts[2]);

		Fvector		N;
		N.mknormal	(D.v[0],D.v[1],D.v[2]);
		if (N.y<=0)	continue;

		tris.inc	();
	}
	if (tris.size()==0)	{
		Log("chasm2");
		return FALSE;			// chasm?
	}

	// *** Perform ray-casts and calculate sector
	Fvector P,D,PLP; D.set(0,-1,0);

	float coeff = 0.5f*g_params.fPatchSize/float(RCAST_Count);

	int num_successed_rays = 0;
	for (int x=-RCAST_Count; x<=RCAST_Count; x++) 
	{
		P.x = N.Pos.x + coeff*float(x);
		for (int z=-RCAST_Count; z<=RCAST_Count; z++) {
			P.z = N.Pos.z + coeff*float(z);
			P.y = N.Pos.y;
			N.Plane.intersectRayPoint(P,D,PLP);	// "project" position
			P.y = PLP.y+RCAST_DepthValid/2;

			float	tri_min_range	= flt_max;
			int		tri_selected	= -1;
			float	range,u,v;
			for (i=0; i<DWORD(tris.size()); i++) 
			{
				if (RAPID::TestRayTri(P,D,tris[i].v,u,v,range,false)) 
				{
					if (range<tri_min_range) {
						tri_min_range	= range;
						tri_selected	= i;
					}
				}
			}
			if (tri_selected>=0) {
				if (range<RCAST_DepthValid)	num_successed_rays++;

			}
		}
	}
	if (float(num_successed_rays)/float(RCAST_Total) < 0.5f) {
		Msg		("Floating node.");
		return	FALSE;
	}
	return TRUE;
}

#define		merge(pt)	if (fsimilar(P.y,REF.y,0.5f)) { c++; pt.add(P); }

void	xrSmoothNodes()
{
	Nodes	smoothed;	smoothed.reserve(g_nodes.size());
	Marks	mark;		mark.assign(g_nodes.size(),false);

	int inv_count = 0;
	for (DWORD i=0; i<g_nodes.size(); i++)
	{
		Node& N = g_nodes[i];

		Fvector	P1,P2,P3,P4,P,REF;
		int		c;

		// smooth point LF
		{
			bool	bCorner = false;

			c=1;	N.PointLF(REF);	P1.set(REF);
			if (N.nLeft()!=InvalidNode) {
				Node& L = g_nodes[N.nLeft()];

				L.PointFR(P);	merge(P1);
				if (L.nForward()!=InvalidNode) {
					bCorner = true;
					Node& C = g_nodes[L.nForward()];

					C.PointRB(P);	merge(P1);
				}
			}
			if (N.nForward()!=InvalidNode) {
				Node& F = g_nodes[N.nForward()];

				F.PointBL(P);	merge(P1);
				if ((!bCorner) && (F.nLeft()!=InvalidNode)) {
					bCorner = true;

					Node& C = g_nodes[F.nLeft()];
					C.PointRB(P);	merge(P1);
				}
			}
			R_ASSERT(c<=4);
			P1.div(float(c));
		}

		// smooth point FR
		{
			bool	bCorner = false;

			c=1;	N.PointFR(REF); P2.set(REF);
			if (N.nForward()!=InvalidNode) {
				Node& F = g_nodes[N.nForward()];

				F.PointRB(P);	merge(P2);
				if (F.nRight()!=InvalidNode) {
					bCorner = true;
					Node& C = g_nodes[F.nRight()];

					C.PointBL(P);	merge(P2);
				}
			}
			if (N.nRight()!=InvalidNode) {
				Node& R = g_nodes[N.nRight()];

				R.PointLF(P);	merge(P2);
				if ((!bCorner) && (R.nForward()!=InvalidNode)) {
					bCorner = true;

					Node& C = g_nodes[R.nForward()];
					C.PointBL(P);	merge(P2);
				}
			}
			R_ASSERT(c<=4);
			P2.div(float(c));
		}

		// smooth point RB
		{
			bool	bCorner = false;

			c=1;	N.PointRB(REF); P3.set(REF);
			if (N.nRight()!=InvalidNode) {
				Node& R = g_nodes[N.nRight()];

				R.PointBL(P);	merge(P3);
				if (R.nBack()!=InvalidNode) {
					bCorner = true;
					Node& C = g_nodes[R.nBack()];

					C.PointLF(P);	merge(P3);
				}
			}
			if (N.nBack()!=InvalidNode) {
				Node& B = g_nodes[N.nBack()];

				B.PointFR(P);	merge(P3);
				if ((!bCorner) && (B.nRight()!=InvalidNode)) {
					bCorner = true;

					Node& C = g_nodes[B.nRight()];
					C.PointLF(P);	merge(P3);
				}
			}
			R_ASSERT(c<=4);
			P3.div(float(c));
		}

		// smooth point BL
		{
			bool	bCorner = false;

			c=1;	N.PointBL(REF); P4.set(REF);
			if (N.nBack()!=InvalidNode) {
				Node& B = g_nodes[N.nBack()];

				B.PointLF(P);	merge(P4);
				if (B.nLeft()!=InvalidNode) {
					bCorner = true;
					Node& C = g_nodes[B.nLeft()];

					C.PointFR(P);	merge(P4);
				}
			}
			if (N.nLeft()!=InvalidNode) {
				Node& L = g_nodes[N.nLeft()];

				L.PointRB(P);	merge(P4);
				if ((!bCorner) && (L.nBack()!=InvalidNode)) {
					bCorner = true;

					Node& C = g_nodes[L.nBack()];
					C.PointFR(P);	merge(P4);
				}
			}
			R_ASSERT(c<=4);
			P4.div(float(c));
		}

		// align plane
		Fvector data[4]; data[0]=P1; data[1]=P2; data[2]=P3; data[3]=P4;
		Fvector vOffs,vNorm,D;
		vNorm.set(N.Plane.n);
		vOffs.set(N.Pos);
		Mgc::OrthogonalPlaneFit(
			4,(Mgc::Vector3*)data,
			*((Mgc::Vector3*)&vOffs),
			*((Mgc::Vector3*)&vNorm)
		);
		if (vNorm.y<0) vNorm.invert();

		// create new node
		Node NEW = N;
		NEW.Plane.build	(vOffs,vNorm);
		D.set			(0,1,0);
		N.Plane.intersectRayPoint(N.Pos,D,NEW.Pos);	// "project" position
		smoothed.push_back	(NEW);

		// verify placement
		/*
		mark[i]			= !!ValidNode	(NEW);

		if (!mark[i])	inv_count++;
		*/
	}
	g_nodes = smoothed;

	if (inv_count) Msg("%d invalid nodes detected",inv_count);
}

#include "stdafx.h"
#pragma hdrstop

#include "xrCDB.h"

namespace CDB
{
	u32		Collector::VPack	(Fvector& V, float eps)
	{
		xr_vector<Fvector>::iterator I,E;
		I=verts.begin(); E=verts.end();
		for (;I!=E;I++) if (I->similar(V,eps)) return (I-verts.begin());
		verts.push_back(V);
		return verts.size()-1;
	}

	void	Collector::add_face	(
		Fvector& v0, Fvector& v1, Fvector& v2,	// vertices
		u32 e01, u32 e12, u32 e20,				// edges
		WORD material, WORD sector, u32 dummy	// misc
		)
	{
		TRI T;
		T.IDverts()	[0] = verts.size();
		T.IDverts()	[1] = verts.size()+1;
		T.IDverts()	[2] = verts.size()+2;
		T.IDadj()	[0]	= e01;
		T.IDadj()	[1]	= e12;
		T.IDadj()	[2]	= e20;
		T.material		= material;
		T.sector		= sector;
		T.dummy			= dummy;

		verts.push_back(v0);
		verts.push_back(v1);
		verts.push_back(v2);
		faces.push_back(T);
	}

	void	Collector::add_face_packed	(
		Fvector& v0, Fvector& v1, Fvector& v2,	// vertices
		u32 e01, u32 e12, u32 e20,				// edges
		WORD material, WORD sector, u32 dummy,	// misc
		float eps
		)
	{
		TRI T;
		T.IDverts()	[0] = VPack(v0,eps);
		T.IDverts()	[1] = VPack(v1,eps);
		T.IDverts()	[2] = VPack(v2,eps);
		T.IDadj()	[0]	= e01;
		T.IDadj()	[1]	= e12;
		T.IDadj()	[2]	= e20;
		T.material		= material;
		T.sector		= sector;
		T.dummy			= dummy;
		faces.push_back(T);
	}

	void	Collector::calc_adjacency()
	{
		// Dumb algorithm O(N^2) :)
		for (u32 f=0; f<faces.size(); f++)
		{
			for (u32 t=0; t<faces.size(); t++)
			{
				if (t==f)	continue;

				for (u32 f_e=0; f_e<3; f_e++)
				{
					u32 f1	= faces[f].IDverts()[(f_e+0)%3];
					u32 f2	= faces[f].IDverts()[(f_e+1)%3];
					if (f1>f2)	std::swap(f1,f2);

					for (u32 t_e=0; t_e<3; t_e++)
					{
						u32 t1	= faces[t].IDverts()[(t_e+0)%3];
						u32 t2	= faces[t].IDverts()[(t_e+1)%3];
						if (t1>t2)	std::swap(t1,t2);

						if (f1==t1 && f2==t2)
						{
							// f.edge[f_e] linked to t.edge[t_e]
							faces[f].IDadj()[f_e]	= t;
							break;
						}
					}
				}
			}
		}
	}


	CollectorPacked::CollectorPacked(const Fbox &bb, int apx_vertices, int apx_faces)
	{
		// Params
		VMscale.set		(bb.max.x-bb.min.x, bb.max.y-bb.min.y, bb.max.z-bb.min.z);
		VMmin.set		(bb.min);
		VMeps.set		(VMscale.x/clpMX/2,VMscale.y/clpMY/2,VMscale.z/clpMZ/2);
		VMeps.x			= (VMeps.x<EPS_L)?VMeps.x:EPS_L;
		VMeps.y			= (VMeps.y<EPS_L)?VMeps.y:EPS_L;
		VMeps.z			= (VMeps.z<EPS_L)?VMeps.z:EPS_L;

		// Preallocate memory
		verts.reserve	(apx_vertices);
		faces.reserve	(apx_faces);

		int		_size	= (clpMX+1)*(clpMY+1)*(clpMZ+1);
		int		_average= (apx_vertices/_size)/2;
		for (int ix=0; ix<clpMX+1; ix++)
			for (int iy=0; iy<clpMY+1; iy++)
				for (int iz=0; iz<clpMZ+1; iz++)
					VM[ix][iy][iz].reserve	(_average);
	}

	void	CollectorPacked::add_face(
		Fvector& v0, Fvector& v1, Fvector& v2,	// vertices
		u32 e01, u32 e12, u32 e20,				// edges
		WORD material, WORD sector, u32 dummy	// misc
		)
	{
		TRI T;
		T.IDverts()	[0] = VPack(v0);
		T.IDverts()	[1] = VPack(v1);
		T.IDverts()	[2] = VPack(v2);
		T.IDadj()	[0]	= e01;
		T.IDadj()	[1]	= e12;
		T.IDadj()	[2]	= e20;
		T.material		= material;
		T.sector		= sector;
		T.dummy			= dummy;
		faces.push_back(T);
	}

	u32		CollectorPacked::VPack(Fvector& V)
	{
		u32 P = 0xffffffff;

		u32 ix,iy,iz;
		ix = iFloor(float(V.x-VMmin.x)/VMscale.x*clpMX);
		iy = iFloor(float(V.y-VMmin.y)/VMscale.y*clpMY);
		iz = iFloor(float(V.z-VMmin.z)/VMscale.z*clpMZ);

		//		R_ASSERT(ix<=clpMX && iy<=clpMY && iz<=clpMZ);
		clamp(ix,(u32)0,clpMX);	clamp(iy,(u32)0,clpMY);	clamp(iz,(u32)0,clpMZ);

		{
			DWORDList* vl;
			vl = &(VM[ix][iy][iz]);
			for(DWORDIt it=vl->begin();it!=vl->end(); it++)
				if( verts[*it].similar(V) )	{
					P = *it;
					break;
				}
		}
		if (0xffffffff==P)
		{
			P = verts.size();
			verts.push_back(V);

			VM[ix][iy][iz].push_back(P);

			u32 ixE,iyE,izE;
			ixE = iFloor(float(V.x+VMeps.x-VMmin.x)/VMscale.x*clpMX);
			iyE = iFloor(float(V.y+VMeps.y-VMmin.y)/VMscale.y*clpMY);
			izE = iFloor(float(V.z+VMeps.z-VMmin.z)/VMscale.z*clpMZ);

			//			R_ASSERT(ixE<=clpMX && iyE<=clpMY && izE<=clpMZ);
			clamp(ixE,(u32)0,clpMX);	clamp(iyE,(u32)0,clpMY);	clamp(izE,(u32)0,clpMZ);

			if (ixE!=ix)							VM[ixE][iy][iz].push_back	(P);
			if (iyE!=iy)							VM[ix][iyE][iz].push_back	(P);
			if (izE!=iz)							VM[ix][iy][izE].push_back	(P);
			if ((ixE!=ix)&&(iyE!=iy))				VM[ixE][iyE][iz].push_back	(P);
			if ((ixE!=ix)&&(izE!=iz))				VM[ixE][iy][izE].push_back	(P);
			if ((iyE!=iy)&&(izE!=iz))				VM[ix][iyE][izE].push_back	(P);
			if ((ixE!=ix)&&(iyE!=iy)&&(izE!=iz))	VM[ixE][iyE][izE].push_back	(P);
		}
		return P;
	}
};

// cl_collector.cpp: implementation of the Collector class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "cl_collector.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace CDB 
{
	void Collector::calc_adjacency()
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
					if (f1>f2)	swap(f1,f2);

					for (u32 t_e=0; t_e<3; t_e++)
					{
						u32 t1	= faces[t].IDverts()[(t_e+0)%3];
						u32 t2	= faces[t].IDverts()[(t_e+1)%3];
						if (t1>t2)	swap(t1,t2);
						
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
	
	u32	CollectorPacked::VPack(Fvector& V)
	{
		u32 P = 0xffffffff;
		
		u32 ix,iy,iz;
		ix = iFloor(float(V.x-VMmin.x)/VMscale.x*clpMX);
		iy = iFloor(float(V.y-VMmin.y)/VMscale.y*clpMY);
		iz = iFloor(float(V.z-VMmin.z)/VMscale.z*clpMZ);
		R_ASSERT(ix<=clpMX && iy<=clpMY && iz<=clpMZ);
		
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
			
			R_ASSERT(ixE<=clpMX && iyE<=clpMY && izE<=clpMZ);
			
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

//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "GeometryCollector.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)

//------------------------------------------------------------------------------
// VCPacked
//------------------------------------------------------------------------------
VCPacked::VCPacked(const Fbox &bb, float _eps, u32 _clpMX, u32 _clpMY, u32 _clpMZ, int apx_vertices)
{
	eps				= _eps;
	clpMX			= _clpMX;
	clpMY			= _clpMY;
	clpMZ			= _clpMZ;
	// prepare hash table
	VM.resize		(clpMX);
    u32 ix,iy,iz;
    for (ix=0; ix<clpMX+1; ix++) VM[ix].resize(clpMY);
    for (ix=0; ix<clpMY+1; ix++) 
	    for (iy=0; iy<clpMY+1; iy++) 
	    	VM[ix][iy].resize(clpMZ);
            
    // Params
    VMscale.set		(bb.max.x-bb.min.x, bb.max.y-bb.min.y, bb.max.z-bb.min.z);
    VMmin.set		(bb.min);
    VMeps.set		(VMscale.x/clpMX/2,VMscale.y/clpMY/2,VMscale.z/clpMZ/2);
    VMeps.x			= (VMeps.x<EPS_L)?VMeps.x:EPS_L;
    VMeps.y			= (VMeps.y<EPS_L)?VMeps.y:EPS_L;
    VMeps.z			= (VMeps.z<EPS_L)?VMeps.z:EPS_L;

    // Preallocate memory
    verts.reserve	(apx_vertices);

    int		_size	= (clpMX+1)*(clpMY+1)*(clpMZ+1);
    int		_average= (apx_vertices/_size)/2;
    for (ix=0; ix<clpMX+1; ix++)
        for (iy=0; iy<clpMY+1; iy++)
            for (iz=0; iz<clpMZ+1; iz++)
                VM[ix][iy][iz].reserve	(_average);
}

u32		VCPacked::add_vert(const Fvector& V)
{
    u32 P = 0xffffffff;

    u32 ix,iy,iz;
    ix = iFloor(float(V.x-VMmin.x)/VMscale.x*clpMX);
    iy = iFloor(float(V.y-VMmin.y)/VMscale.y*clpMY);
    iz = iFloor(float(V.z-VMmin.z)/VMscale.z*clpMZ);

    clamp(ix,(u32)0,clpMX);	
    clamp(iy,(u32)0,clpMY);	
    clamp(iz,(u32)0,clpMZ);

    {
        U32Vec& vl = VM[ix][iy][iz];
        for(U32It it=vl.begin();it!=vl.end(); it++)
            if( verts[*it].pos.similar(V,eps) )	{
                P = *it;
                break;
            }
    }
    if (0xffffffff==P)
    {
        P = verts.size();
        verts.push_back(GCVertex(V));

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

void	VCPacked::clear()
{
    verts.clear_and_free	();
    for (u32 _x=0; _x<=clpMX; _x++)
        for (u32 _y=0; _y<=clpMY; _y++)
            for (u32 _z=0; _z<=clpMZ; _z++)
                VM[_x][_y][_z].clear_and_free	();
}

//------------------------------------------------------------------------------
// GCPacked
//------------------------------------------------------------------------------
void	GCPacked::add_face(const Fvector& v0, const Fvector& v1, const Fvector& v2)
{
	GCFace T;
    T.verts	[0] 	= add_vert(v0);
    T.verts	[1] 	= add_vert(v1);
    T.verts	[2] 	= add_vert(v2);
    faces.push_back	(T);
    validate		(T);
}

void	GCPacked::clear()
{
	GCPacked::clear	();
    faces.clear_and_free	();
}


// DynamicHeightMap.cpp: implementation of the CDynamicHeightMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DynamicHeightMap.h"

const int	tasksPerFrame	= 3;
const float limit_up		= 100.f;
const float limit_down		= 20.f;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CHM_Static::Update	()
{
	Fvector&	view	= Device.vCameraPosition;
	int			v_x		= iFloor(view.x/dhm_size);
	int			v_z		= iFloor(view.z/dhm_size);
	
	// *****	SCROLL
	if (v_x!=c_x)	{
		if (v_x>c_x)	{
			// scroll matrix to left
			for (int z=0; z<dhm_matrix; z++)
			{
				Slot*	S	= data[z][0];
				if (S->bReady)	{	S->bReady = FALSE; task.push_back(S); }
				for (int x=1; x<dhm_matrix; x++)	data[z][x-1] = data[z][x];
				data[z][dhm_matrix-1] = S;
				S->x = dhm_matrix-1; S->z = z;
			}
			c_x ++;
		} else {
			// scroll matrix to right
			for (int z=0; z<dhm_matrix; z++)
			{
				Slot*	S	= data[z][dhm_matrix-1];
				if (S->bReady)	{	S->bReady = FALSE; task.push_back(S); }
				for (int x=dhm_matrix-1; x>0; x--)	data[z][x] = data[z][x-1];
				data[z][0]	= S;
				S->x = 0; S->z = z;
			}
			c_x --;
		}
	}
	if (v_z!=c_z)	{
		if (v_z>c_z)	{
			// scroll matrix down a bit
			for (int x=0; x<dhm_matrix; x++)
			{
				Slot*	S	= data[dhm_matrix-1][x];
				if (S->bReady)	{	S->bReady = FALSE; task.push_back(S); }
				for (int z=dhm_matrix-1; z>0; z--)	data[z][x] = data[z-1][x];
				data[0][x]	= S;
				S->x = x; S->z = 0;
			}
			c_z ++;
		} else {
			// scroll matrix up
			for (int x=0; x<dhm_matrix; x++)
			{
				Slot*	S = data[0][x];
				if (S->bReady)	{	S->bReady = FALSE; task.push_back(S); }
				for (int z=0; z<dhm_matrix; z++)	data[z-1][x] = data[z][x];
				data[dhm_matrix-1][x]	= S;
				S->x = x; S->z = dhm_matrix-1;
			}
			c_z --;
		}
	}
	
	// *****	perform TASKs
	for (int taskid=0; taskid<tasksPerFrame; taskid++)
	{
		Slot*	S	= task.back	();	task.pop_back();
		S->bReady	= TRUE;

		// Build BBox
		Fbox				bb;
		bb.min.set			(S->x*dhm_size,		view.y-limit_down,	S->z*dhm_size);
		bb.max.set			(bb.min.x+dhm_size,	view.y+limit_up,	S->min.z+dhm_size);
		bb.grow				(EPS_L);
		
		// Select polygons
		XRC.BBoxMode		(0); // BBOX_TRITEST
		XRC.BBoxCollide		(precalc_identity,pCreator->ObjectSpace.GetStaticModel(),precalc_identity,bb);
		DWORD	triCount	= XRC.GetBBoxContactCount();
		if (0==triCount)	continue;
		RAPID::tri* tris	= pCreator->ObjectSpace.GetStaticTris();

		// Perform testing
		for (int z=0; z<dhm_precision; z++)
		{
			for (int x=0; x<dhm_precision; x++)
			{
				float	rx	= (float(x)/float(dhm_precision))*dhm_size + bb.min.x;
				float	rz	= (float(z)/float(dhm_precision))*dhm_size + bb.min.z;
				float	ry	= bb.min.y-5;
				Fvector pos; pos.set(rx,bb.max.y,rz);
				Fvector	dir; dir.set(0,-1,0);
				
				float	r_u,r_v,r_range;
				for (DWORD tid=0; tid<triCount; tid++)
				{
					RAPID::tri&	T		= tris[XRC.BBoxContact[tid].id];
					if (RAPID::TestRayTri(pos,dir,T.verts,r_u,r_v,r_range,TRUE))
					{
						if (r_range>=0)	{
							float y_test	= pos.y - r_range;
							if (y_test>ry)	ry = y_test;
						}
					}
				}
				S->data[z][x] = ry;
			}
		}
	}
}

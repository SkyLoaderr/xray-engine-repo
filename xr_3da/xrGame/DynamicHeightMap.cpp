// DynamicHeightMap.cpp: implementation of the CDynamicHeightMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DynamicHeightMap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CHM_Static::Update	()
{
	Fvector&	view	= Device.vCameraPosition;
	int			v_x		= iFloor(view.x/dhm_size);
	int			v_z		= iFloor(view.z/dhm_size);
	
	if (v_x!=c_x)	{
		if (v_x>c_x)	{
			// scroll matrix to left
			for (int z=0; z<dhm_matrix; z++)
			{
				Slot*	S	= data[z][0];
				if (S->bReady)	{	S->bReady = FALSE; task.push_back(S); }
				for (int x=1; x<dhm_matrix; x++)	data[z][x-1] = data[z][x];
				data[z][dhm_matrix-1] = S;
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
				data[0]		= S;
			}
			c_z++;
		} else {
			// scroll matrix up
			for (int x=0; x<dhm_matrix; x++)
			{
				Slot*	S = data[0][x];
				if (S->bReady)	{	S->bReady = FALSE; task.push_back(S); }
				for (int z=0; z<dhm_matrix; z++)	data[z-1][x] = data[z][x];
				data[dhm_matrix-1][x]	= S;
			}
			c_z--;
		}
	}
}

#include "stdafx.h"
#pragma hdrstop

#include "detailmanager.h"

const DWORD	vs_size				= 3000;

void CDetailManager::soft_Load		()
{
	// Vertex Stream
	soft_VS	= Device.Shader._CreateVS(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
}

void CDetailManager::soft_Unload	()
{
	Device.Shader._DeleteVS			(soft_VS);
}

void CDetailManager::soft_Render	()
{
	// Render itself
	// float	fPhaseRange	= PI/16;
	// float	fPhaseX		= sinf(Device.fTimeGlobal*0.1f)	*fPhaseRange;
	// float	fPhaseZ		= sinf(Device.fTimeGlobal*0.11f)*fPhaseRange;

	// Get index-stream
	_IndexStream&	_IS		= Device.Streams.Index;
	_VertexStream&	_VS		= Device.Streams.Vertex;

	for (DWORD O=0; O<objects.size(); O++)
	{
		vector<SlotItem*>&	vis = visible	[O];
		if (vis.empty())	continue;

		CDetail&	Object		= *objects[O];
		DWORD	vCount_Object	= Object.number_vertices;
		DWORD	iCount_Object	= Object.number_indices;
		DWORD	vCount_Total	= vis.size()*vCount_Object;

		// calculate lock count needed
		DWORD	lock_count		= vCount_Total/vs_size;
		if	(vCount_Total>(lock_count*vs_size))	lock_count++;

		// calculate objects per lock
		DWORD	o_total			= vis.size();
		DWORD	o_per_lock		= o_total/lock_count;
		if  (o_total > (o_per_lock*lock_count))	o_per_lock++;

		// Fill VB (and flush it as nesessary)
		Device.Shader.set_Shader		(Object.shader);

		Fmatrix		mXform;
		for (DWORD L_ID=0; L_ID<lock_count; L_ID++)
		{
			// Calculate params 
			DWORD	item_start	= L_ID*o_per_lock;
			DWORD	item_end	= item_start+o_per_lock;
			if (item_end>o_total)	item_end = o_total;
			if (item_end<=item_start)	break;
			DWORD	item_range	= item_end-item_start;

			// Calc Lock params
			DWORD	vCount_Lock	= item_range*vCount_Object;
			DWORD	iCount_Lock = item_range*iCount_Object;
	
			// Lock buffers
			DWORD	vBase,iBase,iOffset=0;
			CDetail::fvfVertexOut* vDest	= (CDetail::fvfVertexOut*)	_VS.Lock(vCount_Lock,soft_VS->dwStride,vBase);
			WORD*	iDest					= (WORD*)					_IS.Lock(iCount_Lock,iBase);

			// Filling itself
			for (DWORD item=item_start; item<item_end; item++)
			{
				SlotItem&	Instance	= *(vis[item]);
				float	scale			= Instance.scale_calculated;
				
				// Build matrix
				Fmatrix& M = Instance.mRotY;
				mXform._11=M._11*scale;	mXform._12=M._12*scale;	mXform._13=M._13*scale;	mXform._14=M._14;
				mXform._21=M._21*scale;	mXform._22=M._22*scale;	mXform._23=M._23*scale;	mXform._24=M._24;
				mXform._31=M._31*scale;	mXform._32=M._32*scale;	mXform._33=M._33*scale;	mXform._34=M._34;
				mXform._41=M._41;		mXform._42=M._42;		mXform._43=M._43;		mXform._44=1;
				
				// Transfer vertices
				{
					DWORD					C = Instance.C_dw;
					CDetail::fvfVertexIn	*srcIt = Object.vertices, *srcEnd = Object.vertices+Object.number_vertices;
					CDetail::fvfVertexOut	*dstIt = vDest;

					//
					float	tm			= Device.fTimeGlobal; 
					float	height		= Object.bv_bb.max.y-Object.bv_bb.min.y;
					float	cx			= 1.f/5.f;
					float	cy			= 1.f/7.f;
					float	cz			= 1.f/3.f;
					Fvector&	B		= mXform.c;
					Fvector dir2D;
					dir2D.set			(0,0,1);
					/*
					dir2D.x				= sinf(B.x*cx)+cosf(B.z*cz)+2*cosf(B.y*cy);
					dir2D.y				= 0;
					dir2D.z				= cosf(B.x*cx)+sinf(B.z*cz)+2*sinf(B.y*cy);
					*/
					dir2D.normalize		();

					for	(; srcIt!=srcEnd; srcIt++, dstIt++)
					{
						Fvector& src		= srcIt->P;

						//
						Fvector pos;		mXform.transform_tiny	(pos,src);			// normal coords
						Fvector pos2D;		pos2D.set				(pos.x,0,pos.z);	// 2D pos
						float	H			= pos.y - mXform.c.y;						// height of vertex (scaled)
						float	frac		= src.y/height;								// fraction of model height
						float	inten		= H * sinf	(tm + pos.x*cx+pos.y*cy+pos.z*cz);

						//
						Fvector ctrl1;		ctrl1.set	(0,				0,		0				);
						Fvector ctrl2;		ctrl2.set	(0,				H/2,	0				);
						Fvector ctrl3;		ctrl3.set	(dir2D.x*inten,	H,		dir2D.z*inten	);

						//
						Fvector temp;		temp.lerp	(ctrl1, ctrl2, frac);
						Fvector temp2;		temp2.lerp	(ctrl2, ctrl3, frac);
						Fvector result;		result.lerp	(temp,	temp2, frac);
						pos2D.add			(pos2D,result);
						pos2D.y				+=	mXform.c.y;
						dstIt->P			=	pos2D;

						// 
						/*
						mXform.transform_tiny	(dstIt->P,srcIt->P);
						*/
						dstIt->C	= C;
						dstIt->u	= srcIt->u;
						dstIt->v	= srcIt->v;
					}
				}
				
				// Transfer indices (in 32bit lines)
				VERIFY	(iOffset<65535);
				{
					DWORD	item	= (iOffset<<16) | iOffset;
					DWORD	count	= Object.number_indices/2;
					LPDWORD	sit		= LPDWORD(Object.indices);
					LPDWORD	send	= sit+count;
					LPDWORD	dit		= LPDWORD(iDest);
					for		(; sit!=send; dit++,sit++)	*dit=*sit+item;
					if		(Object.number_indices&1)	
						iDest[Object.number_indices-1]=Object.indices[Object.number_indices-1]+WORD(iOffset);
				}
				
				// Increment counters
				vDest					+=	vCount_Object;
				iDest					+=	iCount_Object;
				iOffset					+=	vCount_Object;
			}
			_VS.Unlock		(vCount_Lock,soft_VS->dwStride);
			_IS.Unlock		(iCount_Lock);
			
			// Render
			Device.Primitive.setVertices	(soft_VS->dwHandle,soft_VS->dwStride,_VS.Buffer());
			Device.Primitive.setIndices		(vBase, _IS.Buffer());
			DWORD	dwNumPrimitives			= iCount_Lock/3;
			Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,vCount_Lock,iBase,dwNumPrimitives);
		}
		
		// Clean up
		vis.clear	();
	}
}
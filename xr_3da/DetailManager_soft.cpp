#include "stdafx.h"
#include "detailmanager.h"

const DWORD	vs_size				= 3000;

void CDetailManager::soft_Load		()
{
	soft_VS	= Device.Streams.Create	(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1, vs_size);
	
	// Header
	dtFS->ReadChunkSafe	(0,&dtH,sizeof(dtH));
	R_ASSERT			(dtH.version == DETAIL_VERSION);
	DWORD m_count		= dtH.object_count;
	
	// Models
	CStream* m_fs	= dtFS->OpenChunk(1);
	for (DWORD m_id = 0; m_id < m_count; m_id++)
	{
		CDetail				dt;
		CStream* S			= m_fs->OpenChunk(m_id);
		dt.Load				(S);
		objects.push_back	(dt);
		S->Close			();
	}
	m_fs->Close		();
}

void CDetailManager::soft_Unload	()
{
}

void CDetailManager::soft_Render	()
{
	// Render itself
	float	fPhaseRange	= PI/16;
	float	fPhaseX		= sinf(Device.fTimeGlobal*0.1f)	*fPhaseRange;
	float	fPhaseZ		= sinf(Device.fTimeGlobal*0.11f)*fPhaseRange;

	// Get index-stream
	CIndexStream*	IS	= Device.Streams.Get_IB();

	for (DWORD O=0; O<dm_max_objects; O++)
	{
		CList<SlotItem*>&	vis = visible	[O];
		if (vis.empty())	continue;

		CDetail&	Object		= objects[O];
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

		Fmatrix		mXform,mRotXZ;
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
			CDetail::fvfVertexOut* vDest	= (CDetail::fvfVertexOut*)	VS->Lock(vCount_Lock,vBase);
			WORD*	iDest					= (WORD*)					IS->Lock(iCount_Lock,iBase);
			WORD*	dbgIndices				= iDest;

			// Filling itself
			for (DWORD item=item_start; item<item_end; item++)
			{
				SlotItem&	Instance	= *(vis[item]);
				float	scale			= Instance.scale_calculated;
				
				// Build matrix
				if (scale>0.7f)	
				{
					mRotXZ.setXYZ			(Instance.phase_x+fPhaseX,0,Instance.phase_z+fPhaseZ);
					mXform.mul_43			(mRotXZ,Instance.mRotY);
					mXform._11				*= scale;
					mXform._22				*= scale;
					mXform._33				*= scale;
					mXform.translate_over	(Instance.P);
				} else {
					Fmatrix& M = Instance.mRotY;
					Fvector& P = Instance.P;
					mXform._11=M._11*scale;	mXform._12=M._12;		mXform._13=M._13;		mXform._14=M._14;
					mXform._21=M._21;		mXform._22=M._22*scale;	mXform._23=M._23;		mXform._24=M._24;
					mXform._31=M._31;		mXform._32=M._32;		mXform._33=M._33*scale;	mXform._34=M._34;
					mXform._41=P.x;			mXform._42=P.y;			mXform._43=P.z;			mXform._44=1;
				}
#pragma todo("DetailManager: set matrix!!!") 
				
				// Transfer vertices
				{
					DWORD					C = Instance.C;
					CDetail::fvfVertexIn	*srcIt = Object.vertices, *srcEnd = Object.vertices+Object.number_vertices;
					CDetail::fvfVertexOut	*dstIt = vDest;
					for	(; srcIt!=srcEnd; srcIt++, dstIt++)
					{
						mXform.transform_tiny	(dstIt->P,srcIt->P);
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
			VS->Unlock	(vCount_Lock);
			IS->Unlock	(iCount_Lock);
			
			// Render
			Device.Primitive.setVertices	(VS->getFVF(),VS->getStride(),VS->getBuffer());
			Device.Primitive.setIndicesUC	(vBase, IS->getBuffer());
			DWORD	dwNumPrimitives			= iCount_Lock/3;
			Device.Primitive.Render			(D3DPT_TRIANGLELIST,0,vCount_Lock,iBase,dwNumPrimitives);
			UPDATEC							(vCount_Lock,dwNumPrimitives,2);
		}
		
		// Clean up
		vis.clear	();
	}
}
/* Copyright (C) Tom Forsyth, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Tom Forsyth, 2001"
 */

#include "object.h"
#include "quad.h"


#include "xrCore.h"
#include "MxQMetric.h"
#pragma comment(lib,"x:/xrCore.lib")
#pragma comment(lib,"x:/xrQSlim.lib")

#define QUAD_SIZE 5

int		g_iNumOfObjectVertsDrawn	= 0;
int		g_iMaxNumTrisDrawn			= -1;
long	g_bShowVIPMInfo				= FALSE;
long	g_bUseFastButBadOptimise	= FALSE;

Object::Object()
{
	pNextCollapse = &CollapseRoot;
	iCurSlidingWindowLevel = 0;
}


Object::~Object()
{
	BinCurrentObject();

	while ( CollapseRoot.ListNext() != NULL )
	{
		delete ( CollapseRoot.ListNext() );
	}

	// Bin any mesh data.
	while ( PermTriRoot.ListNext() != NULL )
	{
		delete ( PermTriRoot.ListNext() );
	}
	while ( PermEdgeRoot.ListNext() != NULL )
	{
		delete ( PermEdgeRoot.ListNext() );
	}
	while ( PermPtRoot.ListNext() != NULL )
	{
		delete ( PermPtRoot.ListNext() );
	}

}

// Check that this is sensible.
void Object::CheckObject ( void )
{
	MeshEdge *edge = PermEdgeRoot.ListNext();
	while ( edge != NULL )
	{
		// All the pts had better be the same material.
		edge = edge->ListNext();
	}

	MeshTri *tri = PermTriRoot.ListNext();
	while ( tri != NULL )
	{
		// All the pts had better be the same material.
		tri = tri->ListNext();
	}

	edge = CurEdgeRoot.ListNext();
	while ( edge != NULL )
	{
		// All the pts had better be the same material.
		edge = edge->ListNext();
	}

	tri = CurTriRoot.ListNext();
	while ( tri != NULL )
	{
		// And all tris should be the current level or the next.
		ASSERT ( ( tri->mytri.iSlidingWindowLevel == iCurSlidingWindowLevel ) || 
				 ( tri->mytri.iSlidingWindowLevel == iCurSlidingWindowLevel + 1 ) );
		tri = tri->ListNext();
	}
}


// Bins all the current data.
void Object::BinCurrentObject ( void )
{
	while ( CurTriRoot.ListNext() != NULL )
	{
		delete ( CurTriRoot.ListNext() );
	}
	while ( CurEdgeRoot.ListNext() != NULL )
	{
		delete ( CurEdgeRoot.ListNext() );
	}
	while ( CurPtRoot.ListNext() != NULL )
	{
		delete ( CurPtRoot.ListNext() );
	}
}

// Creates the current data from the permanent data.
void Object::MakeCurrentObjectFromPerm ( void )
{
	BinCurrentObject();

	// Copy the points.
	for ( MeshPt *ppt = PermPtRoot.ListNext(); ppt != NULL; ppt = ppt->ListNext() )
	{
		// Temporarily link the current and permanent points for when I construct the tris.
		ppt->mypt.pTempPt = new MeshPt ( &CurPtRoot );
		ppt->mypt.pTempPt->mypt = ppt->mypt;
		ppt->mypt.pTempPt->mypt.pTempPt = ppt;
	}
	// Copy the edges.
	for ( MeshEdge *pedge = PermEdgeRoot.ListNext(); pedge != NULL; pedge = pedge->ListNext() )
	{
		MeshEdge *pNewEdge = new MeshEdge (	pedge->pPt1->mypt.pTempPt, 
											pedge->pPt2->mypt.pTempPt, 
											&CurEdgeRoot );
		pNewEdge->myedge = pedge->myedge;
	}
	// Copy the tris.
	int iNumTris = 0;
	for ( MeshTri *ptri = PermTriRoot.ListNext(); ptri != NULL; ptri = ptri->ListNext() )
	{
		MeshTri *pNewTri = new MeshTri (	ptri->pPt1->mypt.pTempPt, 
											ptri->pPt2->mypt.pTempPt, 
											ptri->pPt3->mypt.pTempPt, 
											&CurTriRoot );
		pNewTri->mytri = ptri->mytri;
		iNumTris++;
	}

	ASSERT ( iNumTris == iFullNumTris );

	iNumCollapses = 0;
	iCurSlidingWindowLevel = 0;
	SetNewLevel ( iCurSlidingWindowLevel );
}

// Creates and performs a collapse of pptBinned to pptKept.
// Make sure they actually share an edge!
// Make sure the object is fully collapsed already.
void Object::CreateEdgeCollapse ( MeshPt *pptBinned, MeshPt *pptKept )
{
	CheckObject();

	// The thing had better be fully collapsed.
	ASSERT ( pNextCollapse == &CollapseRoot );

	MeshEdge *pedge = pptBinned->FindEdge ( pptKept );
	ASSERT ( pedge != NULL );
	GeneralCollapseInfo *pGCI = new GeneralCollapseInfo ( &CollapseRoot );

	pGCI->fError = FindCollapseError ( pptBinned, pptKept->FindEdge ( pptBinned ) );

	// Because deleting a tri breaks the FirstTri, NextTri sequence,
	// we need to find the tris & just store their pointers.
	// Then we delete them.

	pGCI->pptBin = pptBinned;
	pGCI->pptKeep = pptKept;

	const int c_iMaxNumTris = 100;	// Grow as needed.
	MeshTri *pBinnedTris[c_iMaxNumTris];

	MeshTri *ptri;
	int iNumTrisCollapsed = 0;
	long bNeedNewLevel = FALSE;
	for ( ptri = pptBinned->FirstTri(); ptri != NULL; ptri = pptBinned->NextTri() )
	{
		ASSERT ( iNumTrisCollapsed < c_iMaxNumTris );	// Grow c_iMaxNumTris as needed.
		pBinnedTris[iNumTrisCollapsed++] = ptri;
		if ( ptri->mytri.iSlidingWindowLevel != iCurSlidingWindowLevel )
		{
			ASSERT ( ptri->mytri.iSlidingWindowLevel == iCurSlidingWindowLevel + 1 );
			// Need to set a new level before doing this collapse.
			bNeedNewLevel = TRUE;
		}
	}

	if ( bNeedNewLevel )
	{
		// Store which tris were already on this level.
		for ( MeshTri *pTri = CurTriRoot.ListNext(); pTri != NULL; pTri = pTri->ListNext() )
		{
			if ( pTri->mytri.iSlidingWindowLevel != iCurSlidingWindowLevel )
			{
				ASSERT ( pTri->mytri.iSlidingWindowLevel == iCurSlidingWindowLevel + 1 );
				GeneralTriInfo *pTriInfoNew = pGCI->TriNextLevel.AddItem();
				pTriInfoNew->ppt[0] = pTri->pPt1;
				pTriInfoNew->ppt[1] = pTri->pPt2;
				pTriInfoNew->ppt[2] = pTri->pPt3;
			}
		}

		// And set the new level.
		iCurSlidingWindowLevel++;
		SetNewLevel ( iCurSlidingWindowLevel );
	}

	pGCI->iSlidingWindowLevel = iCurSlidingWindowLevel;


	// Now bin them.
	// Store the tris so that pptBinned is always ppt[0].
	// This makes some of the optimised mesh versions simpler to code.
	// Also, make two passes and store the changed tris first, then the
	// binned ones. Again, this makes life easier later on.
	// So all the tris in TriCollapsed should correspond with the
	// same-numbered item in TriOriginal.
	int i;
	for ( i = iNumTrisCollapsed - 1; i >= 0; i-- )
	{
		MeshPt *ppt[3];
		ppt[0] = pBinnedTris[i]->pPt1;
		ppt[1] = pBinnedTris[i]->pPt2;
		ppt[2] = pBinnedTris[i]->pPt3;
		while ( ppt[0] != pptBinned )
		{
			MeshPt *pptTemp = ppt[0];
			ppt[0] = ppt[1];
			ppt[1] = ppt[2];
			ppt[2] = pptTemp;
		}
		ASSERT ( iCurSlidingWindowLevel == pBinnedTris[i]->mytri.iSlidingWindowLevel );
		
		if ( ( ppt[0] == pptKept ) || 
			 ( ppt[1] == pptKept ) || 
			 ( ppt[2] == pptKept ) )
		{
			// This tri will be binned. Store it next time round.
		}
		else
		{
			GeneralTriInfo *pTriInfo = pGCI->TriOriginal.AddItem();
			pTriInfo->ppt[0] = ppt[0];
			pTriInfo->ppt[1] = ppt[1];
			pTriInfo->ppt[2] = ppt[2];

			pBinnedTris[i]->Delete ( TRUE );
			pBinnedTris[i] = NULL;
		}
	}
	// Do it again, adding the binned tris this time.
	int iNumBinned = 0;
	for ( i = iNumTrisCollapsed - 1; i >= 0; i-- )
	{
		if ( pBinnedTris[i] != NULL )
		{
			iNumBinned++;

			MeshPt *ppt[3];
			ppt[0] = pBinnedTris[i]->pPt1;
			ppt[1] = pBinnedTris[i]->pPt2;
			ppt[2] = pBinnedTris[i]->pPt3;
			while ( ppt[0] != pptBinned )
			{
				MeshPt *pptTemp = ppt[0];
				ppt[0] = ppt[1];
				ppt[1] = ppt[2];
				ppt[2] = pptTemp;
			}
			ASSERT ( iCurSlidingWindowLevel == pBinnedTris[i]->mytri.iSlidingWindowLevel );
			
			ASSERT ( ( ppt[0] == pptKept ) || ( ppt[1] == pptKept ) || ( ppt[2] == pptKept ) );

			GeneralTriInfo *pTriInfo = pGCI->TriOriginal.AddItem();
			pTriInfo->ppt[0] = ppt[0];
			pTriInfo->ppt[1] = ppt[1];
			pTriInfo->ppt[2] = ppt[2];

			pBinnedTris[i]->Delete ( TRUE );
			pBinnedTris[i] = NULL;
		}
	}

	// And add in the new tris.
	for ( i = 0; i < iNumTrisCollapsed; i++ )
	{
		GeneralTriInfo *pTriInfo = pGCI->TriOriginal.Item ( i );

		// ppt[0] should always be the binned pt.
		ASSERT ( pTriInfo->ppt[0] == pptBinned );

		// Now, are either of the other two the kept point?
		// If so, these are tris that get binned, rather than remapped.
		if ( ( pTriInfo->ppt[1] == pptKept ) ||
			 ( pTriInfo->ppt[2] == pptKept ) )
		{
			// Binned tri - these should be the last few.
			ASSERT ( i >= iNumTrisCollapsed - iNumBinned );
		}
		else
		{
			// A remapped tri.
			ASSERT ( pGCI->TriCollapsed.Size() == i );
			GeneralTriInfo *pTriInfoNew = pGCI->TriCollapsed.AddItem();
			pTriInfoNew->ppt[0] = pptKept;
			pTriInfoNew->ppt[1] = pTriInfo->ppt[1];
			pTriInfoNew->ppt[2] = pTriInfo->ppt[2];

			// And actually create this tri.
			MeshTri *pTri = new MeshTri ( pTriInfoNew->ppt[0], pTriInfoNew->ppt[1], pTriInfoNew->ppt[2], &CurTriRoot, &CurEdgeRoot );
			ASSERT ( pTri != NULL );
			pTri->mytri.iSlidingWindowLevel = iCurSlidingWindowLevel + 1;
		}
	}


	pGCI->iNumTris = 0;
	for ( MeshTri *pTri = CurTriRoot.ListNext(); pTri != NULL; pTri = pTri->ListNext() )
	{
		pGCI->iNumTris++;
	}

	iNumCollapses++;

}

// Bin the last collapse.
// Returns TRUE if these was a last collapse to do.
long Object::BinEdgeCollapse ( void )
{
	GeneralCollapseInfo *pGCI = CollapseRoot.ListNext();
	if ( pGCI == NULL )
	{
		// No collapses to bin.
		ASSERT ( iNumCollapses == 0 );
		return FALSE;
	}
	else
	{
		iNumCollapses--;
		if ( pNextCollapse == &CollapseRoot )
		{
			// Fully collapsed - uncollapse once.
			UndoCollapse();
		}
		if ( pNextCollapse == pGCI )
		{
			// This is the next collapse to be done.
			pNextCollapse = &CollapseRoot;
		}
		pGCI->ListDel();
		delete pGCI;

		return TRUE;
	}
}

// Returns TRUE if a collapse was undone.
long Object::UndoCollapse ( void )
{
	if ( pNextCollapse->ListNext() == NULL )
	{
		// No more to undo.
		return FALSE;
	}
	else
	{
		pNextCollapse = pNextCollapse->ListNext();

		ASSERT ( pNextCollapse->iSlidingWindowLevel == iCurSlidingWindowLevel );

		int i;
		for ( i = 0; i < pNextCollapse->TriCollapsed.Size(); i++ )
		{
			GeneralTriInfo *pTriInfo = pNextCollapse->TriCollapsed.Item(i);
			MeshTri *pTri = pTriInfo->ppt[0]->FindTri ( pTriInfo->ppt[1], pTriInfo->ppt[2] );
			ASSERT ( pTri != NULL );
			pTri->Delete ( TRUE );
		}

		for ( i = 0; i < pNextCollapse->TriOriginal.Size(); i++ )
		{
			GeneralTriInfo *pTriInfo = pNextCollapse->TriOriginal.Item(i);
			MeshTri *pTri = new MeshTri ( pTriInfo->ppt[0], pTriInfo->ppt[1], pTriInfo->ppt[2], &CurTriRoot, &CurEdgeRoot );
			ASSERT ( pTri != NULL );
			pTri->mytri.iSlidingWindowLevel = iCurSlidingWindowLevel;
		}


		// Now see if the _previous_ collapse is on a different level.
		if ( pNextCollapse->ListNext() != NULL )
		{
			if ( pNextCollapse->ListNext()->iSlidingWindowLevel != iCurSlidingWindowLevel )
			{
				// Need to go back a level.
				ASSERT ( pNextCollapse->ListNext()->iSlidingWindowLevel == iCurSlidingWindowLevel - 1 );
				iCurSlidingWindowLevel--;
				SetNewLevel ( iCurSlidingWindowLevel );

				// Except that some tris will already be at the lower level.
				for ( int i = 0; i < pNextCollapse->TriNextLevel.Size(); i++ )
				{
					GeneralTriInfo *pTriInfo = pNextCollapse->TriNextLevel.Item(i);
					MeshTri *pTri = pTriInfo->ppt[0]->FindTri ( pTriInfo->ppt[1], pTriInfo->ppt[2] );
					ASSERT ( pTri != NULL );
					pTri->mytri.iSlidingWindowLevel = iCurSlidingWindowLevel + 1;
				}
			}
		}

		return TRUE;
	}
}

// Returns TRUE if a collapse was done.
long Object::DoCollapse ( void )
{
	if ( pNextCollapse == &CollapseRoot )
	{
		// No more to do.
		return FALSE;
	}
	else
	{
		if ( pNextCollapse->iSlidingWindowLevel != iCurSlidingWindowLevel )
		{
			// Need to start a new level.
			ASSERT ( pNextCollapse->TriNextLevel.Size() > 0 );
			ASSERT ( pNextCollapse->iSlidingWindowLevel == iCurSlidingWindowLevel + 1 );
			iCurSlidingWindowLevel++;
			SetNewLevel ( iCurSlidingWindowLevel );
		}
		else
		{
			// No new level to start.
			ASSERT ( pNextCollapse->TriNextLevel.Size() == 0 );
		}


		int i;
		for ( i = 0; i < pNextCollapse->TriOriginal.Size(); i++ )
		{
			GeneralTriInfo *pTriInfo = pNextCollapse->TriOriginal.Item(i);
			MeshTri *pTri = pTriInfo->ppt[0]->FindTri ( pTriInfo->ppt[1], pTriInfo->ppt[2] );
			ASSERT ( pTri != NULL );

			ASSERT ( pTri->mytri.iSlidingWindowLevel == iCurSlidingWindowLevel );

			pTri->Delete ( TRUE );
		}

		for ( i = 0; i < pNextCollapse->TriCollapsed.Size(); i++ )
		{
			GeneralTriInfo *pTriInfo = pNextCollapse->TriCollapsed.Item(i);
			MeshTri *pTri = new MeshTri ( pTriInfo->ppt[0], pTriInfo->ppt[1], pTriInfo->ppt[2], &CurTriRoot, &CurEdgeRoot );
			ASSERT ( pTri != NULL );
			pTri->mytri.iSlidingWindowLevel = iCurSlidingWindowLevel + 1;
		}

		pNextCollapse = pNextCollapse->ListPrev();

		return TRUE;
	}
}



void Object::SetNewLevel ( int iLevel )
{
	for ( MeshTri *pTri = CurTriRoot.ListNext(); pTri != NULL; pTri = pTri->ListNext() )
	{
		pTri->mytri.iSlidingWindowLevel = iLevel;
	}
}

long Object::CollapseAllowedForLevel ( MeshPt *pptBinned, int iLevel )
{
	// All the tris that use the binned point must be at the current level.
	long bRes = TRUE;
	for ( MeshTri *pTri = pptBinned->FirstTri(); pTri != NULL; pTri = pptBinned->NextTri() )
	{
		if ( iLevel != pTri->mytri.iSlidingWindowLevel )
		{
			bRes = FALSE;
		}
	}
	return bRes;
}

// Return the error from this edge collapse.
// Set bTryToCacheResult=TRUE if you can pass pptBinned in multiple times.
// Make sure you call this with bTryToCacheResult=FALSE if any data changes,
//	or you'll confuse the poor thing.
void pack_to_vector(MxVector& tgt, const D3DXVECTOR3& src_p, float src_u, float src_v)
{
	tgt[0] = src_p.x;
	tgt[1] = src_p.y;
	tgt[2] = src_p.z;
	if (QUAD_SIZE>3){
		tgt[3] = src_u;
		tgt[4] = src_v;
	}
}

void Object::compute_face_quadric(MeshTri* tri, MxQuadric& Q)
{
	MxVector v1	(QUAD_SIZE);
	MxVector v2	(QUAD_SIZE);
	MxVector v3	(QUAD_SIZE);

	pack_to_vector(v1,tri->pPt1->mypt.vPos,tri->pPt1->mypt.fU,tri->pPt1->mypt.fV);
	pack_to_vector(v2,tri->pPt2->mypt.vPos,tri->pPt2->mypt.fU,tri->pPt2->mypt.fV);
	pack_to_vector(v3,tri->pPt3->mypt.vPos,tri->pPt3->mypt.fU,tri->pPt3->mypt.fV);

	Q			= MxQuadric(v1, v2, v3, 0.f);
}
void Object::collect_quadrics()
{
	__quadrics.resize(iFullNumPts);
	for(unsigned int j=0; j<quadric_count(); j++)
		__quadrics[j] = new MxQuadric(QUAD_SIZE);

	MeshTri *tri = PermTriRoot.ListNext();
	while ( tri != NULL )
	{
		MxQuadric Q			(QUAD_SIZE);
		compute_face_quadric(tri, Q);

		MyPt& p1 = tri->pPt1->mypt;
		MyPt& p2 = tri->pPt2->mypt;
		MyPt& p3 = tri->pPt3->mypt;
		
		quadric(p1.dwIndex) += Q;
		quadric(p2.dwIndex) += Q;
		quadric(p3.dwIndex) += Q;
		// All the pts had better be the same material.
		tri = tri->ListNext();
	}
}
/*
void Object::compute_target_placement(edge_info *info)
{
	MxVertexID i=info->v1, j=info->v2;

	const MxQuadric &Qi=quadric(i), &Qj=quadric(j);
	MxQuadric Q=Qi;  Q+=Qj;

	double e_min;

	if( placement_policy==MX_PLACE_OPTIMAL && Q.optimize(info->target)){
		e_min = Q(info->target);
	}else{
		// Fall back only on endpoints
		MxVector vi(dim()), vj(dim());
		MxVector best(dim());

		pack_to_vector(i, vi);
		pack_to_vector(j, vj);

		double ei=Q(vi), ej=Q(vj);

		if( ei<ej )	{ e_min = ei; best = vi; }
		else		{ e_min = ej; best = vj; }

		if( placement_policy>=MX_PLACE_ENDORMID ){
			MxVector mid(dim());
			mid			= vi;
			mid			+=vj;
			mid			/=2.f;
			double e_mid= Q(mid);

			if( e_mid < e_min ) { e_min = e_mid; best = mid; }
		}
		info->target	= best;
	}

	if( weighting_policy == MX_WEIGHT_AREA_AVG )
		e_min /= Q.area();

	info->heap_key(float(-e_min));
}
*/
float Object::FindCollapseError ( MeshPt *pptBinned, MeshEdge *pedgeCollapse, long bTryToCacheResult /*= FALSE*/ )
{
	if (1){
		static MeshPt		*pptLast;
		static MxQuadric	qLast(QUAD_SIZE);
		if ( pptBinned == NULL )
		{
			pptLast			= 0;
			return			0.0f;
		}

		MxQuadric			qSum(QUAD_SIZE);
		if ( bTryToCacheResult && ( pptLast == pptBinned ) ){
			qSum			= qLast;
		}else{
			for ( MeshTri *ptri = pptBinned->FirstTri(); ptri != NULL; ptri = pptBinned->NextTri() ){
				MxQuadric	qCur(QUAD_SIZE);
				compute_face_quadric(ptri,qCur);
				qSum		+= qCur;
			}
			if ( bTryToCacheResult ){
				qLast		= qSum;
				pptLast		= pptBinned;
			}else{
				pptLast		= NULL;
			}
		}

		MeshPt *pptKept		= pedgeCollapse->OtherPt ( pptBinned );
		ASSERT ( pptKept != NULL );

		MxVector pos(QUAD_SIZE);
		pack_to_vector(pos,pptKept->mypt.vPos,pptKept->mypt.fU,pptKept->mypt.fV);

		return qSum.evaluate(pos);
	}else{
		static MeshPt *pptLast;
		static Quad qLast;

		if ( pptBinned == NULL ){
			// You can call it like this to flush the cache.
			pptLast = NULL;
			return 0.0f;
		}


		MeshPt *pptKept = pedgeCollapse->OtherPt ( pptBinned );
		ASSERT ( pptKept != NULL );


		Quad qSum;
		if ( bTryToCacheResult && ( pptLast == pptBinned ) ){
			qSum = qLast;
		}else{
			// Find the sum of the QEMs of the tris that will be binned.
			for ( MeshTri *ptri = pptBinned->FirstTri(); ptri != NULL; ptri = pptBinned->NextTri() )
				qSum += Quad ( ptri->pPt1->mypt.vPos, ptri->pPt2->mypt.vPos, ptri->pPt3->mypt.vPos );

			if ( bTryToCacheResult ){
				qLast = qSum;
				pptLast = pptBinned;
			}else{
				pptLast = NULL;
			}
		}

		// And find the error once the collapse has happened.
		return qSum.FindError ( pptKept->mypt.vPos );
	}
}


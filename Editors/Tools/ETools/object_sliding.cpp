/* Copyright (C) Tom Forsyth, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Tom Forsyth, 2001"
 */
#include "stdafx.h"
#pragma hdrstop

#include "object.h"
#include "object_sliding.h"

BOOL g_bOptimiseVertexOrder = FALSE;

// Call this to reorder the tris in this trilist to get good vertex-cache coherency.
// *pwList is modified (but obviously not changed in size or memory location).
// void OptimiseVertexCoherencyTriList ( WORD *pwList, int iHowManyTris );

///////// SLIDING WINDOW VIPM ////////////
OMSlidingWindow::OMSlidingWindow ( Object *pObject )
{
	pObj = pObject;
//	pVB = NULL;
//	pIB = NULL;
	bOptimised = FALSE;
}

OMSlidingWindow::~OMSlidingWindow ( void )
{
	pObj = NULL;
//	VERIFY ( pVB == NULL );
//	VERIFY ( pIB == NULL );
}

void OMSlidingWindow::Update ( void )
{
	if ( !bDirty && ( bOptimised == g_bOptimiseVertexOrder ) )
	{
		// Nothing to do!
		return;
	}

	bDirty = FALSE;
	iVersion++;			// Just for luck.

	bOptimised = g_bOptimiseVertexOrder;

//	SAFE_RELEASE ( pVB );
//	SAFE_RELEASE ( pIB );
	swrRecords.SizeTo ( 0 );

	MeshPt *pt;
	MeshTri *tri;

	// Undo all the collapses, so we start from the maximum mesh.
	while ( pObj->UndoCollapse() ) {}

	// How many vertices are we looking at?
	iNumVerts = 0;
	for ( pt = pObj->CurPtRoot.ListNext(); pt != NULL; pt = pt->ListNext() ){
		pt->mypt.dwNewIndex = -1;
		iNumVerts++;
	}
	// Er... word indices, guys... nothing supports 32-bit indices yet.
	VERIFY ( iNumVerts < 65535 );

	// How many tris are we looking at?
	int iNumTris = 0;
	for ( tri = pObj->CurTriRoot.ListNext(); tri != NULL; tri = tri->ListNext() ){
		tri->mytri.dwNewIndex = -1;
		iNumTris++;
	}
	// A lot of cards have this annoying limit - see D3DCAPS8.MaxPrimitiveCount, which is
	// exactly 65535 for DX7 and previous devices. So might as well stick with that number.
	VERIFY ( iNumTris < 65535 );

	// Create a place to put indices while we build up the list. Because we don't know
	// how many we need yet, we can't create the IB until the end. Then we'll copy this into it.
	ArbitraryList<WORD> wIndices;
	wIndices.SizeTo ( 0 );

	vertices.resize	(iNumVerts);
	ETOOLS::QSVert *pCurVertex	= &vertices.front();

	// Now do all the collapses, so we start from the minimum mesh.
	// Along the way, mark the vertices in reverse order.
	int iCurVerts = iNumVerts;
	int iCurCollapse = 0;
	int iCurSlidingWindowLevel = 0;
	while ( TRUE ){
		GeneralCollapseInfo *pCollapse = pObj->pNextCollapse;
		if ( pObj->pNextCollapse == &(pObj->CollapseRoot) ) break;

		iCurSlidingWindowLevel = pCollapse->iSlidingWindowLevel;
		iCurCollapse++;
		iCurVerts--;
		pCollapse->pptBin->mypt.dwNewIndex = iCurVerts;
		pObj->DoCollapse();
	}

	iNumCollapses = iCurCollapse;

	// Add the remaining existing pts in any old order.
	WORD wCurIndex = 0;
	for ( pt = pObj->CurPtRoot.ListNext(); pt != NULL; pt = pt->ListNext() )
	{
		if ( pt->mypt.dwNewIndex == (DWORD)-1 )
		{
			// Not binned in a collapse.
			pt->mypt.dwNewIndex	= wCurIndex;
			pCurVertex->pt		= pt->mypt.vPos;
//.			pCurVertex->norm	= pt->mypt.vNorm;
			pCurVertex->uv.x	= pt->mypt.fU;
			pCurVertex->uv.y	= pt->mypt.fV;

			pCurVertex++;
			wCurIndex++;
		}
	}

	// Should meet in the middle!
	VERIFY ( wCurIndex == iCurVerts );

	// And count the tris that are left.
	int iCurNumTris = 0;
	for ( tri = pObj->CurTriRoot.ListNext(); tri != NULL; tri = tri->ListNext() )
		iCurNumTris++;



	// Reserve space for the collapse table - this is stored so that
	// "number of collapses" is the index. So we'll be adding from the
	// back.
	iCurCollapse++;		// Implicit "last collapse" is state after last collapse.
	swrRecords.SizeTo ( iCurCollapse );
	// And add the last one.
	iCurCollapse--;
	SlidingWindowRecord *swr = swrRecords.Item ( iCurCollapse );
	swr->dwFirstIndexOffset = 0;
	swr->wNumTris = (WORD)iCurNumTris;
	swr->wNumVerts = wCurIndex;

	// Now go through each level in turn.

	int iCurTriBinned = 0;

	// Useful thing.
	ArbitraryList<WORD> wTempIndices;

	int iMaxSlidingWindowLevel = iCurSlidingWindowLevel;

	while ( TRUE )
	{
		// Now we go through the collapses for this level, undoing them.
		// As we go, we add the binned tris to the start of the index list,
		// 

		// This coming list will be three sections:
		// 1. the tris that get binned by the splits.
		// 2. the tris that aren't affected.
		// 3. the tris that are added by the splits.
		//
		// We know that at the moment, we are just rendering
		// 1+2, which must equal the number of tris.
		// So 3 starts iCurNumTris on from the start of 1.
		int iCurTriAdded = iCurTriBinned + iCurNumTris;
		wIndices.SizeTo ( iCurTriAdded * 3 );



		int iJustCheckingNumTris = 0;
		for ( tri = pObj->CurTriRoot.ListNext(); tri != NULL; tri = tri->ListNext() )
		{
			tri->mytri.dwNewIndex = -1;		// Mark them as not being in a collapse.
			iJustCheckingNumTris++;
		}
		VERIFY ( iJustCheckingNumTris == iCurNumTris );

		BOOL bJustStartedANewLevel = TRUE;

		// Now undo all the collapses for this level in turn, adding vertices,
		// binned tris, and SlidingWindowRecords as we go.
		while ( ( pObj->pNextCollapse->ListNext() != NULL ) &&
			    ( pObj->pNextCollapse->ListNext()->iSlidingWindowLevel == iCurSlidingWindowLevel ) )
		{
			GeneralCollapseInfo *pCollapse = pObj->pNextCollapse->ListNext();


			// If we've just started a new level, EXCEPT on the last level,
			// we don't need to store the post-collapse version of the tris,
			// since we'll just switch down to the next level instead.
			if ( !bJustStartedANewLevel || ( iCurSlidingWindowLevel == iMaxSlidingWindowLevel  ) )
			{
				// These tris will be binned by the split.
				if ( pCollapse->TriCollapsed.Size() > 0 )
				{
					wTempIndices.SizeTo(pCollapse->TriCollapsed.Size() * 3);
					for ( int i = 0; i < pCollapse->TriCollapsed.Size(); i++ )
					{
						GeneralTriInfo *pTriInfo = pCollapse->TriCollapsed.Item(i);
						MeshTri *tri = pTriInfo->ppt[0]->FindTri ( pTriInfo->ppt[1], pTriInfo->ppt[2] );
						VERIFY ( tri != NULL );
						VERIFY ( tri->mytri.dwNewIndex == -1 );	// Should not have been in a collapse this level.

						VERIFY ( pTriInfo->ppt[0]->mypt.dwNewIndex < wCurIndex );
						VERIFY ( pTriInfo->ppt[1]->mypt.dwNewIndex < wCurIndex );
						VERIFY ( pTriInfo->ppt[2]->mypt.dwNewIndex < wCurIndex );
						*wTempIndices.Item(i*3+0) = (WORD)pTriInfo->ppt[0]->mypt.dwNewIndex;
						*wTempIndices.Item(i*3+1) = (WORD)pTriInfo->ppt[1]->mypt.dwNewIndex;
						*wTempIndices.Item(i*3+2) = (WORD)pTriInfo->ppt[2]->mypt.dwNewIndex;
						iCurNumTris--;
					}

					// Now try to order them as best you can.
//.					if ( g_bOptimiseVertexOrder )
//.						OptimiseVertexCoherencyTriList ( wTempIndices.Ptr(), pCollapse->TriCollapsed.Size() );

					// And write them to the index list.
					wIndices.CopyFrom ( iCurTriBinned * 3, wTempIndices, 0, 3 * pCollapse->TriCollapsed.Size() );
					//memcpy ( wIndices.Item ( iCurTriBinned * 3 ), wTempIndices.Ptr(), sizeof(WORD) * 3 * pCollapse->TriCollapsed.Size() );
					iCurTriBinned += pCollapse->TriCollapsed.Size();
				}
			}else{
				// Keep the bookkeeping happy.
				iCurNumTris -= pCollapse->TriCollapsed.Size();
				// And move the added tris pointer back, because it didn't know we weren't
				// going to store these.
				iCurTriAdded -= pCollapse->TriCollapsed.Size();
			}
			bJustStartedANewLevel = FALSE;
			// Do the uncollapse.
			pObj->UndoCollapse();
			// Add the unbinned vertex.
			MeshPt *pPt = pCollapse->pptBin;
			VERIFY ( pPt->mypt.dwNewIndex == wCurIndex );
			pCurVertex->pt	= pPt->mypt.vPos;
//.			pCurVertex->norm = pPt->mypt.vNorm;
			pCurVertex->uv.x = pPt->mypt.fU;
			pCurVertex->uv.y = pPt->mypt.fV;
			pCurVertex++;
			wCurIndex++;

			// These tris will be added by the split.
			if ( pCollapse->TriOriginal.Size() > 0 ){
				wTempIndices.SizeTo(pCollapse->TriOriginal.Size() * 3);
				for ( int i = 0; i < pCollapse->TriOriginal.Size(); i++ ){
					GeneralTriInfo *pTriInfo = pCollapse->TriOriginal.Item(i);
					MeshTri *tri = pTriInfo->ppt[0]->FindTri ( pTriInfo->ppt[1], pTriInfo->ppt[2] );
					VERIFY ( tri != NULL );
					tri->mytri.dwNewIndex = 1;		// Mark it has having been in a collapse.

					VERIFY ( pTriInfo->ppt[0]->mypt.dwNewIndex < wCurIndex );
					VERIFY ( pTriInfo->ppt[1]->mypt.dwNewIndex < wCurIndex );
					VERIFY ( pTriInfo->ppt[2]->mypt.dwNewIndex < wCurIndex );
					*wTempIndices.Item(i*3+0) = (WORD)pTriInfo->ppt[0]->mypt.dwNewIndex;
					*wTempIndices.Item(i*3+1) = (WORD)pTriInfo->ppt[1]->mypt.dwNewIndex;
					*wTempIndices.Item(i*3+2) = (WORD)pTriInfo->ppt[2]->mypt.dwNewIndex;
					iCurNumTris++;
				}

				// Now try to order them as best you can.
//.				if ( g_bOptimiseVertexOrder )
//.					OptimiseVertexCoherencyTriList ( wTempIndices.Ptr(), pCollapse->TriOriginal.Size() );

				// And write them to the index list.
				wIndices.SizeTo ( ( iCurTriAdded + pCollapse->TriOriginal.Size() ) * 3 );
				wIndices.CopyFrom ( iCurTriAdded * 3, wTempIndices, 0, 3 * pCollapse->TriOriginal.Size() );
				//memcpy ( wIndices.Item ( iCurTriAdded * 3 ), wTempIndices.Ptr(), sizeof(WORD) * 3 * pCollapse->TriOriginal.Size() );
				iCurTriAdded += pCollapse->TriOriginal.Size();
			}

			// Add the collapse record.
			iCurCollapse--;
			SlidingWindowRecord *swr = swrRecords.Item ( iCurCollapse );
			swr->dwFirstIndexOffset = iCurTriBinned * 3;
			swr->wNumTris = (WORD)iCurNumTris;
			swr->wNumVerts = wCurIndex;
		}

		// OK, finished this level. Any tris that are left with an index of -1
		// were not added during this level, and therefore need to be
		// added into the middle of the list.
		iJustCheckingNumTris = 0;
		int iTempTriNum = 0;
		for ( tri = pObj->CurTriRoot.ListNext(); tri != NULL; tri = tri->ListNext() ){
			iJustCheckingNumTris++;
			if ( tri->mytri.dwNewIndex == -1 ){
				// This tri has not been created by a collapse this level.
				wTempIndices.SizeTo ( iTempTriNum * 3 + 3 );
				*(wTempIndices.Item(iTempTriNum*3+0)) = tri->pPt1->mypt.dwNewIndex;
				*(wTempIndices.Item(iTempTriNum*3+1)) = tri->pPt2->mypt.dwNewIndex;
				*(wTempIndices.Item(iTempTriNum*3+2)) = tri->pPt3->mypt.dwNewIndex;
				iTempTriNum++;
			}
		}
		VERIFY ( iJustCheckingNumTris == iCurNumTris );

		// Now try to order them as best you can.
//.		if ( g_bOptimiseVertexOrder )
//.			OptimiseVertexCoherencyTriList ( wTempIndices.Ptr(), iTempTriNum );

		// And write them to the index list.
		wIndices.CopyFrom ( iCurTriBinned * 3, wTempIndices, 0, 3 * iTempTriNum );
		//memcpy ( wIndices.Item ( iCurTriBinned * 3 ), wTempIndices.Ptr(), sizeof(WORD) * 3 * iTempTriNum );

		if ( pObj->pNextCollapse->ListNext() == NULL ){
			// No more collapses.
			VERIFY ( iCurCollapse == 0 );
			VERIFY ( iCurSlidingWindowLevel == 0 );
			break;
		}

		// Start a new level by skipping past all the indices so far.
		iCurTriBinned += iCurNumTris;
		// Check the maths.
		VERIFY ( iCurTriBinned == iCurTriAdded );

	    iCurSlidingWindowLevel = pObj->pNextCollapse->ListNext()->iSlidingWindowLevel;
	}

	// And now check everything is OK.
	VERIFY ( swrRecords.Size() == iNumCollapses + 1 );
	for ( int i = 0; i <= iNumCollapses; i++ ){
		SlidingWindowRecord *swr = swrRecords.Item ( i );
		for ( int j = 0; j < swr->wNumTris * 3; j++ ){
			VERIFY ( *(wIndices.Item ( j + swr->dwFirstIndexOffset )) < swr->wNumVerts );
		}
	}

	// Create the index buffer.
	indices.resize(wIndices.Size()/3);

	// Copy to the index buffer
	for (u32 i_idx=0; i_idx<indices.size(); i_idx++)
		indices[i_idx].set(*wIndices.Item(i_idx*3+0),*wIndices.Item(i_idx*3+1),*wIndices.Item(i_idx*3+2));
}


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

//BOOL g_bUseFastButBadOptimise = FALSE;

// Call this to reorder the tris in this trilist to get good vertex-cache coherency.
// *pwList is modified (but obviously not changed in size or memory location).
void OptimiseVertexCoherencyTriList ( WORD *pwList, int iHowManyTris, u32 mode);

void CalculateSW(Object* object, VIPM_Result* result, u32 optimize_vertex_order)
{
	result->swr_records.resize(0);

	MeshPt *pt;
	MeshTri *tri;

	// Undo all the collapses, so we start from the maximum mesh.
	while ( object->UndoCollapse() ) {}

	// How many vertices are we looking at?
	u32 iNumVerts = 0;
	for ( pt = object->CurPtRoot.ListNext(); pt != NULL; pt = pt->ListNext() ){
		pt->mypt.dwNewIndex = INVALID_INDEX;
		iNumVerts++;
	}
	// Er... word indices, guys... nothing supports 32-bit indices yet.
	VERIFY ( iNumVerts < 65535 );

	// How many tris are we looking at?
	int iNumTris = 0;
	for ( tri = object->CurTriRoot.ListNext(); tri != NULL; tri = tri->ListNext() ){
		tri->mytri.dwNewIndex = INVALID_INDEX;		// Mark them as not being in a collapse.
		iNumTris++;
	}
	// A lot of cards have this annoying limit - see D3DCAPS8.MaxPrimitiveCount, which is
	// exactly 65535 for DX7 and previous devices. So might as well stick with that number.
	VERIFY ( iNumTris < 65535 );

	// Create a place to put indices while we build up the list. Because we don't know
	// how many we need yet, we can't create the IB until the end. Then we'll copy this into it.
	result->indices.resize ( 0 );

	// permutation table
	result->permute_verts.resize(iNumVerts);
	for (u32 k=0; k<result->permute_verts.size(); k++) result->permute_verts[0]=u16(-1);

	// Now do all the collapses, so we start from the minimum mesh.
	// Along the way, mark the vertices in reverse order.
	int iCurVerts = iNumVerts;
	int iCurCollapse = 0;
	int iCurSlidingWindowLevel = 0;
	while ( TRUE ){
		GeneralCollapseInfo *pCollapse = object->pNextCollapse;
		if ( object->pNextCollapse == &(object->CollapseRoot) ) break;

		iCurSlidingWindowLevel = pCollapse->iSlidingWindowLevel;
		iCurCollapse++;
		iCurVerts--;
		pCollapse->pptBin->mypt.dwNewIndex = iCurVerts;
		object->DoCollapse();
	}

	int iNumCollapses = iCurCollapse;

	// Add the remaining existing pts in any old order.
	WORD wCurIndex = 0;
	for ( pt = object->CurPtRoot.ListNext(); pt != NULL; pt = pt->ListNext() )
	{
		if ( pt->mypt.dwNewIndex == INVALID_INDEX )
		{
			// Not binned in a collapse.
			result->permute_verts[pt->mypt.dwIndex]=wCurIndex;
			pt->mypt.dwNewIndex	= wCurIndex;
			wCurIndex++;
		}
	}

	// Should meet in the middle!
	VERIFY ( wCurIndex == iCurVerts );

	// And count the tris that are left.
	int iCurNumTris = 0;
	for ( tri = object->CurTriRoot.ListNext(); tri != NULL; tri = tri->ListNext() )
		iCurNumTris++;



	// Reserve space for the collapse table - this is stored so that
	// "number of collapses" is the index. So we'll be adding from the
	// back.
	iCurCollapse++;		// Implicit "last collapse" is state after last collapse.
	result->swr_records.resize ( iCurCollapse );
	// And add the last one.
	iCurCollapse--;
	VIPM_SWR *swr	= result->swr_records.item ( iCurCollapse );
	swr->offset		= 0;
	swr->num_tris	= (u16)iCurNumTris;
	swr->num_verts	= wCurIndex;

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
		result->indices.resize ( iCurTriAdded * 3 );



		int iJustCheckingNumTris = 0;
		for ( tri = object->CurTriRoot.ListNext(); tri != NULL; tri = tri->ListNext() )
		{
			tri->mytri.dwNewIndex = INVALID_INDEX;		// Mark them as not being in a collapse.
			iJustCheckingNumTris++;
		}
		VERIFY ( iJustCheckingNumTris == iCurNumTris );

		BOOL bJustStartedANewLevel = TRUE;

		// Now undo all the collapses for this level in turn, adding vertices,
		// binned tris, and SlidingWindowRecords as we go.
		while ( ( object->pNextCollapse->ListNext() != NULL ) &&
			    ( object->pNextCollapse->ListNext()->iSlidingWindowLevel == iCurSlidingWindowLevel ) )
		{
			GeneralCollapseInfo *pCollapse = object->pNextCollapse->ListNext();


			// If we've just started a new level, EXCEPT on the last level,
			// we don't need to store the post-collapse version of the tris,
			// since we'll just switch down to the next level instead.
			if ( !bJustStartedANewLevel || ( iCurSlidingWindowLevel == iMaxSlidingWindowLevel  ) )
			{
				// These tris will be binned by the split.
				if ( pCollapse->TriCollapsed.size() > 0 )
				{
					wTempIndices.resize(pCollapse->TriCollapsed.size() * 3);
					for ( u32 i = 0; i < pCollapse->TriCollapsed.size(); i++ )
					{
						GeneralTriInfo *pTriInfo = pCollapse->TriCollapsed.item(i);
						MeshTri *tri = pTriInfo->ppt[0]->FindTri ( pTriInfo->ppt[1], pTriInfo->ppt[2] );
						VERIFY ( tri != NULL );
						VERIFY ( tri->mytri.dwNewIndex == INVALID_INDEX );	// Should not have been in a collapse this level.

						VERIFY ( pTriInfo->ppt[0]->mypt.dwNewIndex < wCurIndex );
						VERIFY ( pTriInfo->ppt[1]->mypt.dwNewIndex < wCurIndex );
						VERIFY ( pTriInfo->ppt[2]->mypt.dwNewIndex < wCurIndex );
						*wTempIndices.item(i*3+0) = (WORD)pTriInfo->ppt[0]->mypt.dwNewIndex;
						*wTempIndices.item(i*3+1) = (WORD)pTriInfo->ppt[1]->mypt.dwNewIndex;
						*wTempIndices.item(i*3+2) = (WORD)pTriInfo->ppt[2]->mypt.dwNewIndex;
						iCurNumTris--;
					}

					// Now try to order them as best you can.
					if ( optimize_vertex_order )
						OptimiseVertexCoherencyTriList ( wTempIndices.ptr(), pCollapse->TriCollapsed.size(), optimize_vertex_order );

					// And write them to the index list.
					result->indices.insert ( iCurTriBinned * 3, wTempIndices, 0, 3 * pCollapse->TriCollapsed.size() );
					//memcpy ( result->indices.item ( iCurTriBinned * 3 ), wTempIndices.ptr(), sizeof(WORD) * 3 * pCollapse->TriCollapsed.size() );
					iCurTriBinned += pCollapse->TriCollapsed.size();
				}
			}else{
				// Keep the bookkeeping happy.
				iCurNumTris -= pCollapse->TriCollapsed.size();
				// And move the added tris pointer back, because it didn't know we weren't
				// going to store these.
				iCurTriAdded -= pCollapse->TriCollapsed.size();
			}
			bJustStartedANewLevel = FALSE;
			// Do the uncollapse.
			object->UndoCollapse();
			// Add the unbinned vertex.
			MeshPt *pPt = pCollapse->pptBin;
			VERIFY ( pPt->mypt.dwNewIndex == wCurIndex );
			result->permute_verts[pPt->mypt.dwIndex]=wCurIndex;
/*
			pCurVertex->pt	= pPt->mypt.vPos;
//.			pCurVertex->norm = pPt->mypt.vNorm;
			pCurVertex->uv.x = pPt->mypt.fU;
			pCurVertex->uv.y = pPt->mypt.fV;
			pCurVertex++;
*/
			wCurIndex++;

			// These tris will be added by the split.
			if ( pCollapse->TriOriginal.size() > 0 ){
				wTempIndices.resize(pCollapse->TriOriginal.size() * 3);
				for ( u32 i = 0; i < pCollapse->TriOriginal.size(); i++ ){
					GeneralTriInfo *pTriInfo = pCollapse->TriOriginal.item(i);
					MeshTri *tri = pTriInfo->ppt[0]->FindTri ( pTriInfo->ppt[1], pTriInfo->ppt[2] );
					VERIFY ( tri != NULL );
					tri->mytri.dwNewIndex = 1;		// Mark it has having been in a collapse.

					VERIFY ( pTriInfo->ppt[0]->mypt.dwNewIndex < wCurIndex );
					VERIFY ( pTriInfo->ppt[1]->mypt.dwNewIndex < wCurIndex );
					VERIFY ( pTriInfo->ppt[2]->mypt.dwNewIndex < wCurIndex );
					*wTempIndices.item(i*3+0) = (WORD)pTriInfo->ppt[0]->mypt.dwNewIndex;
					*wTempIndices.item(i*3+1) = (WORD)pTriInfo->ppt[1]->mypt.dwNewIndex;
					*wTempIndices.item(i*3+2) = (WORD)pTriInfo->ppt[2]->mypt.dwNewIndex;
					iCurNumTris++;
				}

				// Now try to order them as best you can.
				if ( optimize_vertex_order )
					OptimiseVertexCoherencyTriList ( wTempIndices.ptr(), pCollapse->TriOriginal.size(), optimize_vertex_order );

				// And write them to the index list.
				result->indices.resize ( ( iCurTriAdded + pCollapse->TriOriginal.size() ) * 3 );
				result->indices.insert ( iCurTriAdded * 3, wTempIndices, 0, 3 * pCollapse->TriOriginal.size() );
				//memcpy ( result->indices.item ( iCurTriAdded * 3 ), wTempIndices.ptr(), sizeof(WORD) * 3 * pCollapse->TriOriginal.size() );
				iCurTriAdded += pCollapse->TriOriginal.size();
			}

			// Add the collapse record.
			iCurCollapse--;
			VIPM_SWR *swr	= result->swr_records.item ( iCurCollapse );
			swr->offset		= iCurTriBinned * 3;
			swr->num_tris	= (u16)iCurNumTris;
			swr->num_verts	= wCurIndex;
		}

		// OK, finished this level. Any tris that are left with an index of -1
		// were not added during this level, and therefore need to be
		// added into the middle of the list.
		iJustCheckingNumTris = 0;
		int iTempTriNum = 0;
		for ( tri = object->CurTriRoot.ListNext(); tri != NULL; tri = tri->ListNext() ){
			iJustCheckingNumTris++;
			if ( tri->mytri.dwNewIndex == INVALID_INDEX ){
				// This tri has not been created by a collapse this level.
				wTempIndices.resize ( iTempTriNum * 3 + 3 );
				*(wTempIndices.item(iTempTriNum*3+0)) = (u16)tri->pPt1->mypt.dwNewIndex;
				*(wTempIndices.item(iTempTriNum*3+1)) = (u16)tri->pPt2->mypt.dwNewIndex;
				*(wTempIndices.item(iTempTriNum*3+2)) = (u16)tri->pPt3->mypt.dwNewIndex;
				iTempTriNum++;
			}
		}
		VERIFY ( iJustCheckingNumTris == iCurNumTris );

		// Now try to order them as best you can.
		if ( optimize_vertex_order )
			OptimiseVertexCoherencyTriList ( wTempIndices.ptr(), iTempTriNum, optimize_vertex_order );

		// And write them to the index list.
		result->indices.insert ( iCurTriBinned * 3, wTempIndices, 0, 3 * iTempTriNum );
		//memcpy ( result->indices.item ( iCurTriBinned * 3 ), wTempIndices.ptr(), sizeof(WORD) * 3 * iTempTriNum );

		if ( object->pNextCollapse->ListNext() == NULL ){
			// No more collapses.
			VERIFY ( iCurCollapse == 0 );
			VERIFY ( iCurSlidingWindowLevel == 0 );
			break;
		}

		// Start a new level by skipping past all the indices so far.
		iCurTriBinned += iCurNumTris;
		// Check the maths.
		VERIFY ( iCurTriBinned == iCurTriAdded );

	    iCurSlidingWindowLevel = object->pNextCollapse->ListNext()->iSlidingWindowLevel;
	}

	// And now check everything is OK.
	VERIFY ( result->swr_records.size() == u32(iNumCollapses + 1) );
	for ( int i = 0; i <= iNumCollapses; i++ ){
		VIPM_SWR *swr = result->swr_records.item ( i );
		for ( int j = 0; j < swr->num_tris * 3; j++ ){
			VERIFY ( *(result->indices.item(j+swr->offset)) < swr->num_verts );
		}
	}
}
//-----------------------------------------------------------------------------------------

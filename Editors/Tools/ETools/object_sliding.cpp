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

BOOL g_bOptimiseVertexOrder = FALSE;

// Call this to reorder the tris in this trilist to get good vertex-cache coherency.
// *pwList is modified (but obviously not changed in size or memory location).
void OptimiseVertexCoherencyTriList ( WORD *pwList, int iHowManyTris );

// Finds the weighted average number of verts read when rendering this list.
float GetNumVerticesLoadedTriList ( WORD *pwList, int iHowManyTris );

///////// SLIDING WINDOW VIPM ////////////
// A table of these exists, one per collapse.
struct SlidingWindowRecord
{
	DWORD	dwFirstIndexOffset;			// Offset of the first index in the index buffer to start at (note! no retrictions. Can easily be >64k)
	WORD	wNumTris;					// Number of tris to render (most cards can't do more than 65536)
	WORD	wNumVerts;					// Number of vertices to render with (using WORD indices)
};

class OptimisedMesh
{
protected:
	OptimisedMesh ( void );

	int iVersion;				// Current version number.
	BOOL bDirty;				// TRUE if dirty.
	BOOL bWillGetInfo;
public:



	virtual ~OptimisedMesh ( void ) = 0;

	// Tell this method that the underlying mesh has changed.
	// bWillGetInfo = TRUE if you are going to call any of the Info* functions.
	// This causes a speed hit, so only do it when necessary.
	virtual void MarkAsDirty ( BOOL bWillGetInfo );

	// Actually updates the OptimisedMesh to match the real mesh.
	virtual void Update ( void ) = 0;
};

class OMSlidingWindow: public OptimisedMesh
{
	friend class OMISlidingWindow;

	Object *pObj;

	int										iNumVerts;			// Number of verts at full rez.
	int										iNumCollapses;		// Total number of collapses.
	int										iNumLevels;			// Total number of levels.
	int										iSizeOfIB;			// In indices. Just for info.
	ArbitraryList<SlidingWindowRecord>		swrRecords;			// The records of the collapses.

	BOOL									bOptimised;			// Have the tris been reordered optimally?

//	LPDIRECT3DVERTEXBUFFER8		pVB;							// The VB with the vertices in.
//	LPDIRECT3DINDEXBUFFER8		pIB;							// The index list, all levels clumped together. Always using TRILISTs.

public:
	OMSlidingWindow ( Object *pObject );
	virtual ~OMSlidingWindow ( void );
	virtual void Update ( void );
};



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
		pt->mypt.dwIndex = -1;
		iNumVerts++;
	}
	// Er... word indices, guys... nothing supports 32-bit indices yet.
	VERIFY ( iNumVerts < 65535 );

	// How many tris are we looking at?
	int iNumTris = 0;
	for ( tri = pObj->CurTriRoot.ListNext(); tri != NULL; tri = tri->ListNext() ){
		tri->mytri.dwIndex = -1;
		iNumTris++;
	}
	// A lot of cards have this annoying limit - see D3DCAPS8.MaxPrimitiveCount, which is
	// exactly 65535 for DX7 and previous devices. So might as well stick with that number.
	VERIFY ( iNumTris < 65535 );

	// Create a place to put indices while we build up the list. Because we don't know
	// how many we need yet, we can't create the IB until the end. Then we'll copy this into it.
	ArbitraryList<WORD> wIndices;
	wIndices.SizeTo ( 0 );
/*
	// Create the vertex buffer.
	HRESULT hres = g_pd3dDevice->CreateVertexBuffer ( iNumVerts * sizeof ( STDVERTEX ),
										D3DUSAGE_WRITEONLY,
										STDVERTEX_FVF,
										D3DPOOL_DEFAULT,
										&pVB );
	VERIFY ( SUCCEEDED ( hres ) );

	STDVERTEX *pVertices, *pCurVertex;
	hres = pVB->Lock ( 0, 0, (BYTE **)(&pVertices), 0 );
	VERIFY ( SUCCEEDED ( hres ) );

	pCurVertex = pVertices;
*/
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
		pCollapse->pptBin->mypt.dwIndex = iCurVerts;
		pObj->DoCollapse();
	}

	iNumCollapses = iCurCollapse;

/*
	// Add the remaining existing pts in any old order.
	WORD wCurIndex = 0;
	for ( pt = pObj->CurPtRoot.ListNext(); pt != NULL; pt = pt->ListNext() )
	{
		if ( pt->mypt.dwIndex == (DWORD)-1 )
		{
			// Not binned in a collapse.
			pt->mypt.dwIndex = wCurIndex;
			pCurVertex->v = pt->mypt.vPos;
			pCurVertex->norm = pt->mypt.vNorm;
			pCurVertex->tu = pt->mypt.fU;
			pCurVertex->tv = pt->mypt.fV;

			pCurVertex++;
			wCurIndex++;
		}
	}

	// Should meet in the middle!
	VERIFY ( wCurIndex == iCurVerts );
*/

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
			tri->mytri.dwIndex = -1;		// Mark them as not being in a collapse.
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
						VERIFY ( tri->mytri.dwIndex == -1 );	// Should not have been in a collapse this level.

						VERIFY ( pTriInfo->ppt[0]->mypt.dwIndex < wCurIndex );
						VERIFY ( pTriInfo->ppt[1]->mypt.dwIndex < wCurIndex );
						VERIFY ( pTriInfo->ppt[2]->mypt.dwIndex < wCurIndex );
						*wTempIndices.Item(i*3+0) = (WORD)pTriInfo->ppt[0]->mypt.dwIndex;
						*wTempIndices.Item(i*3+1) = (WORD)pTriInfo->ppt[1]->mypt.dwIndex;
						*wTempIndices.Item(i*3+2) = (WORD)pTriInfo->ppt[2]->mypt.dwIndex;
						iCurNumTris--;
					}

					// Now try to order them as best you can.
					if ( g_bOptimiseVertexOrder )
					{
						OptimiseVertexCoherencyTriList ( wTempIndices.Ptr(), pCollapse->TriCollapsed.Size() );
					}

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
			VERIFY ( pPt->mypt.dwIndex == wCurIndex );
			pCurVertex->v = pPt->mypt.vPos;
			pCurVertex->norm = pPt->mypt.vNorm;
			pCurVertex->tu = pPt->mypt.fU;
			pCurVertex->tv = pPt->mypt.fV;
			pCurVertex++;
			wCurIndex++;

			// These tris will be added by the split.
			if ( pCollapse->TriOriginal.Size() > 0 ){
				wTempIndices.SizeTo(pCollapse->TriOriginal.Size() * 3);
				for ( int i = 0; i < pCollapse->TriOriginal.Size(); i++ ){
					GeneralTriInfo *pTriInfo = pCollapse->TriOriginal.Item(i);
					MeshTri *tri = pTriInfo->ppt[0]->FindTri ( pTriInfo->ppt[1], pTriInfo->ppt[2] );
					VERIFY ( tri != NULL );
					tri->mytri.dwIndex = 1;		// Mark it has having been in a collapse.

					VERIFY ( pTriInfo->ppt[0]->mypt.dwIndex < wCurIndex );
					VERIFY ( pTriInfo->ppt[1]->mypt.dwIndex < wCurIndex );
					VERIFY ( pTriInfo->ppt[2]->mypt.dwIndex < wCurIndex );
					*wTempIndices.Item(i*3+0) = (WORD)pTriInfo->ppt[0]->mypt.dwIndex;
					*wTempIndices.Item(i*3+1) = (WORD)pTriInfo->ppt[1]->mypt.dwIndex;
					*wTempIndices.Item(i*3+2) = (WORD)pTriInfo->ppt[2]->mypt.dwIndex;
					iCurNumTris++;
				}

				// Now try to order them as best you can.
				if ( g_bOptimiseVertexOrder )
					OptimiseVertexCoherencyTriList ( wTempIndices.Ptr(), pCollapse->TriOriginal.Size() );

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
			if ( tri->mytri.dwIndex == -1 ){
				// This tri has not been created by a collapse this level.
				wTempIndices.SizeTo ( iTempTriNum * 3 + 3 );
				*(wTempIndices.Item(iTempTriNum*3+0)) = tri->pPt1->mypt.dwIndex;
				*(wTempIndices.Item(iTempTriNum*3+1)) = tri->pPt2->mypt.dwIndex;
				*(wTempIndices.Item(iTempTriNum*3+2)) = tri->pPt3->mypt.dwIndex;
				iTempTriNum++;
			}
		}
		VERIFY ( iJustCheckingNumTris == iCurNumTris );

		// Now try to order them as best you can.
		if ( g_bOptimiseVertexOrder )
			OptimiseVertexCoherencyTriList ( wTempIndices.Ptr(), iTempTriNum );

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

	hres = pVB->Unlock();
	VERIFY ( SUCCEEDED ( hres ) );

	// And now check everything is OK.
	VERIFY ( swrRecords.Size() == iNumCollapses + 1 );
	for ( int i = 0; i <= iNumCollapses; i++ ){
		SlidingWindowRecord *swr = swrRecords.Item ( i );
		for ( int j = 0; j < swr->wNumTris * 3; j++ ){
			VERIFY ( *(wIndices.Item ( j + swr->dwFirstIndexOffset )) < swr->wNumVerts );
		}
	}

	DWORD dwFlags = 0;
// If doing extra debugging, we're always going to do reads from the IB.
	if ( !bWillGetInfo ){
		// We don't need to do reads from the IB to get vertex-cache info.
		dwFlags |= D3DUSAGE_WRITEONLY;
	}

	// Create the index buffer.
	iSizeOfIB = wIndices.Size();
	hres = g_pd3dDevice->CreateIndexBuffer ( sizeof(WORD) * wIndices.Size(),
										dwFlags,
										D3DFMT_INDEX16,
										D3DPOOL_DEFAULT,
										&pIB );
	VERIFY ( SUCCEEDED ( hres ) );

	// Copy to the index buffer
	WORD *pwDest;
	hres = pIB->Lock ( 0, sizeof(WORD) * wIndices.Size(), (BYTE**)&pwDest, 0 );
	VERIFY ( SUCCEEDED ( hres ) );

	memcpy ( pwDest, wIndices.Ptr(), sizeof(WORD) * wIndices.Size() );

	hres = pIB->Unlock();
	VERIFY ( SUCCEEDED ( hres ) );

	wIndices.SizeTo ( 0 );

}


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

int		g_iNumOfObjectVertsDrawn	= 0;
int		g_iMaxNumTrisDrawn			= -1;
BOOL	g_bShowVIPMInfo				= FALSE;
BOOL	g_bUseFastButBadOptimise	= FALSE;

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


void Object::CreateTestObject ( LPDIRECT3DDEVICE8 pd3dDevice )
{
	ASSERT ( PermPtRoot.ListNext() == NULL );
	ASSERT ( PermTriRoot.ListNext() == NULL );
	ASSERT ( PermEdgeRoot.ListNext() == NULL );

#if 0
	// Make a cube.
	MeshPt *ppt000 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt001 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt010 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt011 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt100 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt101 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt110 = new MeshPt ( &PermPtRoot );
	MeshPt *ppt111 = new MeshPt ( &PermPtRoot );

	ppt000->mypt.vPos = D3DXVECTOR3 ( -1.0f,  1.0f, -1.0f );
	ppt001->mypt.vPos = D3DXVECTOR3 (  1.0f,  1.0f, -1.0f );
	ppt010->mypt.vPos = D3DXVECTOR3 ( -1.0f,  1.0f,  1.0f );
	ppt011->mypt.vPos = D3DXVECTOR3 (  1.0f,  1.0f,  1.0f );
	ppt100->mypt.vPos = D3DXVECTOR3 ( -1.0f, -1.0f, -1.0f );
	ppt101->mypt.vPos = D3DXVECTOR3 (  1.0f, -1.0f, -1.0f );
	ppt110->mypt.vPos = D3DXVECTOR3 ( -1.0f, -1.0f,  1.0f );
	ppt111->mypt.vPos = D3DXVECTOR3 (  1.0f, -1.0f,  1.0f );

	// Dodgy normals.
	MeshPt *pt = PermPtRoot.ListNext();
	DWORD dwIndex = 0;
	while ( pt != NULL )
	{
		//pt->mypt.dwIndex = dwIndex++;
		D3DXVec3Normalize ( &(pt->mypt.vNorm), &(pt->mypt.vPos) );

		pt = pt->ListNext();
	}


	MeshTri *ptri;
	// Top.
	ptri = new MeshTri ( ppt000, ppt010, ppt011, &PermTriRoot, &PermEdgeRoot );
	ptri = new MeshTri ( ppt000, ppt011, ppt001, &PermTriRoot, &PermEdgeRoot );
	// Bottom.
	ptri = new MeshTri ( ppt100, ppt111, ppt110, &PermTriRoot, &PermEdgeRoot );
	ptri = new MeshTri ( ppt100, ppt101, ppt111, &PermTriRoot, &PermEdgeRoot );
	// Left.
	ptri = new MeshTri ( ppt000, ppt110, ppt010, &PermTriRoot, &PermEdgeRoot );
	ptri = new MeshTri ( ppt000, ppt100, ppt110, &PermTriRoot, &PermEdgeRoot );
	// Right.
	ptri = new MeshTri ( ppt001, ppt011, ppt111, &PermTriRoot, &PermEdgeRoot );
	ptri = new MeshTri ( ppt001, ppt111, ppt101, &PermTriRoot, &PermEdgeRoot );
	// Front
	ptri = new MeshTri ( ppt000, ppt001, ppt101, &PermTriRoot, &PermEdgeRoot );
	ptri = new MeshTri ( ppt000, ppt101, ppt100, &PermTriRoot, &PermEdgeRoot );
	// Back
	ptri = new MeshTri ( ppt010, ppt111, ppt011, &PermTriRoot, &PermEdgeRoot );
	ptri = new MeshTri ( ppt010, ppt110, ppt111, &PermTriRoot, &PermEdgeRoot );
#else

	HRESULT hres;

	// Make a teapotahedron.
	LPD3DXMESH pmeshTeapot;
	ASSERT ( pd3dDevice != NULL );		// Slight fudge - shame we need a D3D device.
	//hres = D3DXCreateTeapot ( pd3dDevice, &pmeshTeapot, NULL );
	// These are just some simpler test meshes
	//hres = D3DXCreatePolygon ( pd3dDevice, 1.0f, 6, &pmeshTeapot, NULL );
	//hres = D3DXCreateSphere ( pd3dDevice, 1.0f, 12, 6, &pmeshTeapot, NULL );
	hres = D3DXCreateSphere ( pd3dDevice, 1.0f, 30, 15, &pmeshTeapot, NULL );


	// OK, now extract the data.
	int iNumVerts = pmeshTeapot->GetNumVertices();
	int iNumFaces = pmeshTeapot->GetNumFaces();

	LPDIRECT3DVERTEXBUFFER8 pVertexBuffer;
	hres = pmeshTeapot->GetVertexBuffer ( &pVertexBuffer );
	D3DVERTEXBUFFER_DESC vbdesc;
	hres = pVertexBuffer->GetDesc ( &vbdesc );
	// Create my "smart" pointer.
	MyFVFPointer pFVF ( vbdesc.FVF );
	BYTE *pbData;
	hres = pVertexBuffer->Lock ( 0, pFVF.GetFVFSize() * iNumVerts, &pbData, D3DLOCK_READONLY );
	pFVF.SetCurVertex ( pbData );

	// The de-index list.
	MeshPt **ppPts = new MeshPt*[iNumVerts];

	for ( int i = 0; i < iNumVerts; i++ )
	{
		ppPts[i] = new MeshPt ( &PermPtRoot );
		ppPts[i]->mypt.vPos		= pFVF.Position();
		ppPts[i]->mypt.vNorm	= pFVF.Normal();
		//ppPts[i]->mypt.fU		= pFVF.U0();
		//ppPts[i]->mypt.fV		= pFVF.V0();

		//ppPts[i]->mypt.dwIndex = i;

		++pFVF;
	}

	hres = pVertexBuffer->Unlock();
	pVertexBuffer->Release();


	// And now the index buffer.
	LPDIRECT3DINDEXBUFFER8 pIndexBuffer;
	hres = pmeshTeapot->GetIndexBuffer ( &pIndexBuffer );
	D3DINDEXBUFFER_DESC ibdesc;
	hres = pIndexBuffer->GetDesc ( &ibdesc );
	// Unlikely to get any 32bpp indices, but check, just in case.
	// If you do - well, I leave that as an exercise for the reader :-)

	// Oh - just found this comment in the docs:
	//
	// D3DXMESH_32BIT 
	//   The mesh has 32-bit indices instead of 16-bit indices.
	//   A 32-bit mesh can support up to 2^32-1 faces and vertices.
	//   This flag is not supported and should not be used. 
	//
	// So, that answers that question!
	ASSERT ( ibdesc.Format == D3DFMT_INDEX16 );

	// Also, assume that this defines a trilist. Not sure if the mesh tells us
	// what the primitive type is anywhere.
	WORD *pIndex;
	ASSERT ( sizeof (*pIndex) * iNumFaces * 3 == ibdesc.Size );
	hres = pIndexBuffer->Lock ( 0, ibdesc.Size, (BYTE**)&pIndex, D3DLOCK_READONLY );

	for ( int j = 0; j < iNumFaces; j++ )
	{
		MeshPt *ppt[3];
		for ( int i = 0; i < 3; i++ )
		{
			ASSERT ( *pIndex < iNumVerts );
			ppt[i] = ppPts[*pIndex];
			pIndex++;
		}

		MeshTri *ptri = new MeshTri ( ppt[0], ppt[1], ppt[2], &PermTriRoot, &PermEdgeRoot );
	}

	hres = pIndexBuffer->Unlock();
	pIndexBuffer->Release();


	delete[] ppPts;


	// And finally bin the thing.
	pmeshTeapot->Release();
#endif

	iFullNumTris = 0;
	MeshTri *tri = PermTriRoot.ListNext();
	while ( tri != NULL )
	{
		// All the pts had better be the same material.
		tri = tri->ListNext();
		iFullNumTris++;
	}

	MeshEdge *edge = PermEdgeRoot.ListNext();
	while ( edge != NULL )
	{
		edge = edge->ListNext();
	}

	iCurSlidingWindowLevel = 0;
	SetNewLevel ( iCurSlidingWindowLevel );
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


struct STDVERTEX
{
	D3DXVECTOR3 v;
	D3DXVECTOR3 norm;
	FLOAT       tu, tv;
};

#define STDVERTEX_FVF (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

// Renders the given material of the current state of the object.
// Set iSlidingWindowLevel to -1 if you don't care about level numbers.
void Object::RenderCurrentObject ( LPDIRECT3DDEVICE8 pd3ddev, int iSlidingWindowLevel )
{
	STDVERTEX vert[3];

	HRESULT hres;

	hres = pd3ddev->SetVertexShader ( STDVERTEX_FVF );

	MeshTri *tri = CurTriRoot.ListNext();
	while ( tri != NULL )
	{
		if ( ( iSlidingWindowLevel == -1 ) ||
				( iSlidingWindowLevel == tri->mytri.iSlidingWindowLevel ) )
		{
			// Draw this one.
			vert[0].v		= tri->pPt1->mypt.vPos;
			vert[0].norm	= tri->pPt1->mypt.vNorm;
			vert[0].tu		= tri->pPt1->mypt.fU;
			vert[0].tv		= tri->pPt1->mypt.fV;

			vert[1].v		= tri->pPt2->mypt.vPos;
			vert[1].norm	= tri->pPt2->mypt.vNorm;
			vert[1].tu		= tri->pPt2->mypt.fU;
			vert[1].tv		= tri->pPt2->mypt.fV;

			vert[2].v		= tri->pPt3->mypt.vPos;
			vert[2].norm	= tri->pPt3->mypt.vNorm;
			vert[2].tu		= tri->pPt3->mypt.fU;
			vert[2].tv		= tri->pPt3->mypt.fV;

			hres = pd3ddev->DrawPrimitiveUP ( D3DPT_TRIANGLELIST, 1, vert, sizeof(vert[0]) );
		}
		tri = tri->ListNext();
	}
}

void Object::RenderCurrentEdges ( LPDIRECT3DDEVICE8 pd3ddev )
{
	STDVERTEX vert[2];

	HRESULT hres;

	hres = pd3ddev->SetVertexShader ( STDVERTEX_FVF );

	MeshEdge *edge = CurEdgeRoot.ListNext();
	while ( edge != NULL )
	{
		// Draw this one.
		vert[0].v		= edge->pPt1->mypt.vPos;
		vert[0].norm	= edge->pPt1->mypt.vNorm;
		vert[0].tu		= edge->pPt1->mypt.fU;
		vert[0].tv		= edge->pPt1->mypt.fV;

		vert[1].v		= edge->pPt2->mypt.vPos;
		vert[1].norm	= edge->pPt2->mypt.vNorm;
		vert[1].tu		= edge->pPt2->mypt.fU;
		vert[1].tv		= edge->pPt2->mypt.fV;

		hres = pd3ddev->DrawPrimitiveUP ( D3DPT_LINELIST, 1, vert, sizeof(vert[0]) );

		edge = edge->ListNext();
	}
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
	BOOL bNeedNewLevel = FALSE;
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
BOOL Object::BinEdgeCollapse ( void )
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
BOOL Object::UndoCollapse ( void )
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
BOOL Object::DoCollapse ( void )
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

BOOL Object::CollapseAllowedForLevel ( MeshPt *pptBinned, int iLevel )
{
	// All the tris that use the binned point must be at the current level.
	BOOL bRes = TRUE;
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
float Object::FindCollapseError ( MeshPt *pptBinned, MeshEdge *pedgeCollapse, BOOL bTryToCacheResult /*= FALSE*/ )
{
	static MeshPt *pptLast;
	static Quad qLast;

	if ( pptBinned == NULL )
	{
		// You can call it like this to flush the cache.
		pptLast = NULL;
		return 0.0f;
	}


	MeshPt *pptKept = pedgeCollapse->OtherPt ( pptBinned );
	ASSERT ( pptKept != NULL );


	Quad qSum;
	if ( bTryToCacheResult && ( pptLast == pptBinned ) )
	{
		qSum = qLast;
	}
	else
	{
		// Find the sum of the QEMs of the tris that will be binned.
		for ( MeshTri *ptri = pptBinned->FirstTri(); ptri != NULL; ptri = pptBinned->NextTri() )
		{
			qSum += Quad ( ptri->pPt1->mypt.vPos, ptri->pPt2->mypt.vPos, ptri->pPt3->mypt.vPos );
		}

		if ( bTryToCacheResult )
		{
			qLast = qSum;
			pptLast = pptBinned;
		}
		else
		{
			pptLast = NULL;
		}
	}

	// And find the error once the collapse has happened.
	return qSum.FindError ( pptKept->mypt.vPos );
}


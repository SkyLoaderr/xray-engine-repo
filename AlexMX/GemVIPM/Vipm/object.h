/* Copyright (C) Tom Forsyth, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Tom Forsyth, 2001"
 */
#ifndef objectH
#define objectH

#include "TomsD3DLib.h"
#include "quad.h"

extern LPDIRECT3DDEVICE8 g_pd3dDevice;


// Incremented by the draw routs. Display + zero whenever you want.
extern int g_iMaxNumTrisDrawn;
extern BOOL g_bOptimiseVertexOrder;
extern BOOL g_bShowVIPMInfo;
extern BOOL g_bUseFastButBadOptimise;

struct STDVERTEX
{
    D3DXVECTOR3 v;
    D3DXVECTOR3 norm;
    FLOAT       tu, tv;
};

#define STDVERTEX_FVF (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)



// The data that gets stored inside mesh.h's tris, pts and edges.

class MeshPt;
class MeshEdge;
class MeshTri;

struct MyPt
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vNorm;
	float fU, fV;

	DWORD dwIndex;

	// Temporary data.
	MeshPt *pTempPt;	// Temporary data.
};

struct MyEdge
{
	// Temporary data.
};

struct MyTri
{
	// Temporary data.
	int iSlidingWindowLevel;			// Which sliding window level this tri belongs to.

	DWORD dwIndex;
	MeshTri *pOriginalTri;
};


// The data that gets stored inside mesh.h's tris, pts and edges.
#define MESHTRI_APP_DEFINED		MyTri	mytri;
#define MESHEDGE_APP_DEFINED	MyEdge	myedge;
#define MESHPT_APP_DEFINED		MyPt	mypt;

#include "mesh.h"

struct GeneralTriInfo
{
	MeshPt		*ppt[3];
};

struct GeneralCollapseInfo
{
	DlinkDefine(GeneralCollapseInfo,List);
	
	ArbitraryList<GeneralTriInfo>		TriOriginal;
	ArbitraryList<GeneralTriInfo>		TriCollapsed;

	int			iSlidingWindowLevel;					// Which sliding window level the binned tris will belong to.
	ArbitraryList<GeneralTriInfo>		TriNextLevel;	// On collapses that change levels, lists the tris that were on the next level.

	MeshPt		*pptBin;
	MeshPt		*pptKeep;

	float		fError;					// Error of this collapse.
	int			iNumTris;				// Number of tris after this collapse has been made.

	DlinkMethods(GeneralCollapseInfo,List);

	GeneralCollapseInfo()
	{
		ListInit();
	}

	GeneralCollapseInfo ( GeneralCollapseInfo *pPrev )
	{
		ListInit();
		ListAddAfter ( pPrev );
	}

	~GeneralCollapseInfo()
	{
		ListDel();
	}
};

struct ObjectInstance;
struct Object;


class OptimisedMeshInstance;

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

	// Create an instance of this optimised mesh, and returns the pointer to it.
	// Pass in the object instance you wish to associate it with.
	virtual OptimisedMeshInstance *CreateInstance ( ObjectInstance *pObjectInstance ) = 0;

	// Actually updates the OptimisedMesh to match the real mesh.
	virtual void Update ( void ) = 0;

	// Debugging check.
	virtual void Check ( void ) = 0;

	// Call before changing D3D device.
	virtual void AboutToChangeDevice ( void ) = 0;


	// Creates the given type of optimised mesh, and returns the pointer to it.
	static OptimisedMesh *Create ( Object *pObject );
};


// This is an instance of an OptimisedMesh.
class OptimisedMeshInstance
{
protected:

	int iVersion;		// Current version number;

	OptimisedMeshInstance ( void );

public:
	virtual ~OptimisedMeshInstance ( void ) = 0;

	// Renders the given material of the object with the given level of detail.
	virtual void RenderCurrentObject ( LPDIRECT3DDEVICE8 pd3ddev, int iLoD ) = 0;

	// True if this instance needs to redo its data, because the related optimesh has changed.
	virtual BOOL bNeedsUpdate ( void ) = 0;

	// Update this instance to match the related optimesh, if it has changed.
	virtual void Update ( void ) = 0;

	// Call before changing D3D device.
	virtual void AboutToChangeDevice ( void ) = 0;

	

	// Gets some info about this instance.
	// Returns data about the last-rendered version.
	// Should only be called if bWillGetInfo was TRUE
	// in the last MarkAsDirty() call.

	// *pdwMemoryUsed will be the total amount of memory used by the global data.
	// *pdwOfWhichAGP will the amount of that memory in index and vertex buffers (i.e. all the data the card needs to see).
	virtual const void InfoGetGlobal ( DWORD *pdwMemoryUsed, DWORD *pdwOfWhichAGP ) = 0;

	// *pdwRealTrisDrawn will contain the number of non-degenerate tris drawn.
	virtual const void InfoGetInstance ( DWORD *pdwMemoryUsed, DWORD *pdwOfWhichAGP, DWORD *pdwVerticesLoaded, DWORD *pdwRealTrisDrawn, DWORD *pdwTotalVertices ) = 0;


	// Debugging check.
	virtual void Check ( void ) = 0;
};

struct Object
{
	// The permanent shape.
	MeshPt		PermPtRoot;
	MeshTri		PermTriRoot;
	MeshEdge	PermEdgeRoot;

	// The collapse list is ordered backwards,
	// so ptr->ListNext() is the _previous_ collapse to ptr.
	GeneralCollapseInfo		CollapseRoot;

	// The current shape.
	MeshPt		CurPtRoot;
	MeshTri		CurTriRoot;
	MeshEdge	CurEdgeRoot;

	// pNextCollapse points to the _next_ collapse to do.
	// pNextCollapse->ListNext() is the collapse that's just been done.
	// &CollapseRoot = no more collapses to do.
	GeneralCollapseInfo		*pNextCollapse;

	int			iFullNumTris;		// How many tris with no collapses.
	int			iNumCollapses;		// Total number of collapses.


	int			iCurSlidingWindowLevel;

	BOOL			bSomethingHappened;		// Set to TRUE when interesting things happen. Cleared by main app.



	OptimisedMesh		*pOptMesh;


	Object();

	~Object();

	// Anooyingly, this requires a D3D object, even if only temporarily.
	void Object::CreateTestObject ( LPDIRECT3DDEVICE8 pd3dDevice );

	// Check that this is sensible.
	void CheckObject ( void );

	// Bins all the current data.
	void BinCurrentObject ( void );

	// Creates the current data from the permanent data.
	void MakeCurrentObjectFromPerm ( void );

	// Renders the given material of the current state of the object.
	// Set iSlidingWindowLevel to -1 if you don't care about level numbers.
	void RenderCurrentObject ( LPDIRECT3DDEVICE8 pd3ddev, int iSlidingWindowLevel = -1 );

	void RenderCurrentEdges ( LPDIRECT3DDEVICE8 pd3ddev);

	// Creates and performs a collapse of pptBinned to pptKept.
	// Make sure they actually share an edge!
	// Make sure the object is fully collapsed already.
	void CreateEdgeCollapse ( MeshPt *pptBinned, MeshPt *pptKept );

	// Bin the last collapse.
	// Returns TRUE if these was a last collapse to do.
	BOOL BinEdgeCollapse ( void );

	// Returns TRUE if a collapse was undone.
	BOOL UndoCollapse ( void );

	// Returns TRUE if a collapse was done.
	BOOL DoCollapse ( void );

	
	void SetNewLevel ( int iLevel );

	BOOL CollapseAllowedForLevel ( MeshPt *pptBinned, int iLevel );

	// Return the error from this edge collapse.
	// Set bTryToCacheResult=TRUE if you can pass pptBinned in multiple times.
	// Make sure you call this with bTryToCacheResult=FALSE if any data changes,
	//	or you'll confuse the poor thing.
	float FindCollapseError ( MeshPt *pptBinned, MeshEdge *pedgeCollapse, BOOL bTryToCacheResult = FALSE );

	// Call this if you make a change to the mesh.
	// It will mark all the OptimisedMeshes hanging off it as dirty.
	void MarkAsDirty ( void );

	// Call before D3D leaves.
	void AboutToChangeDevice ( void );

};

struct ObjectInstance
{
	DlinkDefine(ObjectInstance,List);

	// Orientation of instance;
	D3DXMATRIX		matOrn;
	// The parent object.
	Object			*pObj;

	int				iCurCollapses;		// Current number of collapses done (from full-rez).


	OptimisedMeshInstance		*pOptMeshInst;



	DlinkMethods(ObjectInstance,List);


	ObjectInstance ( Object *pObject = NULL, ObjectInstance *pRoot = NULL );

	~ObjectInstance ( void );


	void RenderCurrentObject ( LPDIRECT3DDEVICE8 pd3ddev, int iSlidingWindowLevel = -1, BOOL bShowOptiMesh = FALSE );

	void RenderCurrentEdges ( LPDIRECT3DDEVICE8 pd3ddev )
	{
		pObj->RenderCurrentEdges ( pd3ddev );
	}

	void SetNumCollapses ( int iNum );

	int GetNumCollapses ( void );


	// This shortcut breaks data-hiding. Careful.
	OptimisedMeshInstance *GetOpiMeshInst ( void )
	{
		return pOptMeshInst;
	}

	// Call before D3D leaves.
	void AboutToChangeDevice ( void );

};

#endif // objectH

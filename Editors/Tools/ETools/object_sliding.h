#ifndef Object_SlidingH
#define Object_SlidingH

#include "PropSlimTools.h"
///////// SLIDING WINDOW VIPM ////////////
class OptimisedMesh
{
protected:
	OptimisedMesh ( void )
	{
		iVersion = 1234;
		bDirty = TRUE;
		bWillGetInfo = FALSE;
	}
	int iVersion;				// Current version number.
	BOOL bDirty;				// TRUE if dirty.
	BOOL bWillGetInfo;
public:
	virtual ~OptimisedMesh ( void )=0{}
	// Tell this method that the underlying mesh has changed.
	// bWillGetInfo = TRUE if you are going to call any of the Info* functions.
	// This causes a speed hit, so only do it when necessary.
	virtual void MarkAsDirty ( BOOL bNewWillGetInfo )
	{
		iVersion++;
		bDirty = TRUE;
		bWillGetInfo = bNewWillGetInfo;
	}
	// Actually updates the OptimisedMesh to match the real mesh.
	virtual void Update ( void ) = 0;
};

// A table of these exists, one per collapse.
struct SlidingWindowRecord
{
	DWORD	dwFirstIndexOffset;			// Offset of the first index in the index buffer to start at (note! no retrictions. Can easily be >64k)
	WORD	wNumTris;					// Number of tris to render (most cards can't do more than 65536)
	WORD	wNumVerts;					// Number of vertices to render with (using WORD indices)
};

class OMSlidingWindow: public OptimisedMesh
{
	Object *pObj;

public:
	int										iNumVerts;			// Number of verts at full rez.
	int										iNumCollapses;		// Total number of collapses.
	int										iNumLevels;			// Total number of levels.
	ArbitraryList<SlidingWindowRecord>		swrRecords;			// The records of the collapses.

	BOOL									bOptimised;			// Have the tris been reordered optimally?

	xr_vector<ETOOLS::QSVert>				vertices;
	xr_vector<ETOOLS::QSFace>				indices;
public:
	OMSlidingWindow ( Object *pObject );
	virtual ~OMSlidingWindow ( void );
	virtual void Update ( void );
};
#endif // Object_SlidingH
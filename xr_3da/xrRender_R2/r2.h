#pragma once

#include "..\modelpool.h"
#include "..\hom.h"

#include "r2_types.h"
#include "r2_scenegraph.h"
#include "r2_rendertarget.h"

#include "portal.h"
#include "detailmanager.h"

// definition
class CRender	:	public IRender_interface
{
public:
	// Sector detection and visibility
	CSector*												pLastSector;
	Fvector													vLastCameraPos;
	vector<IRender_Portal*>									Portals;
	vector<IRender_Sector*>									Sectors;
	CDB::MODEL*												rmPortals;
	CHOM													HOM;

	// Global vertex-buffer container
	typedef svector<D3DVERTEXELEMENT9,MAX_FVF_DECL_SIZE>	VertexDeclarator;
	vector<VertexDeclarator>								DCL;
	vector<IDirect3DVertexBuffer9*>							VB;
	vector<IDirect3DIndexBuffer9*>							IB;
	vector<IVisual*>										Visuals;

	CDetailManager											Details;
	CModelPool												Models;

	CRenderTarget											Target;			// Render-target
private:

#ifndef _RENDER_H_
#define _RENDER_H_

#include "frustum.h"

// refs
class ENGINE_API	CStream;
class ENGINE_API	CSector;
class ENGINE_API	CPortal;
class ENGINE_API	CVisual;
class ENGINE_API	CObject;

// definition
class	ENGINE_API	CRender_interface	:
	public pureDeviceCreate,
	public pureDeviceDestroy
{
public:
	// Data
	Fmatrix*						pTransform;
	int								iLightLevel;
	CFrustum						ViewBase;
	CFrustum*						View;
public:
	// Loading / Unloading
	virtual	void					level_Load				();
	virtual void					level_Unload			();

	// Information
	virtual int						getVisualsCount			();
	virtual CPortal*				getPortal				(int id);
	virtual CSector*				getSector				(int id);
	virtual CSector*				getSectorActive			();
	virtual CVisual*				getVisual				(int id);
	virtual DWORD					getFVF					(int id);
	virtual IDirect3DVertexBuffer8*	getVB					(int id);
	virtual CSector*				detectSector			(Fvector& P);
	
	// Main 
	IC		void					set_Frustum				(CFrustum*	O	)			{ VERIFY(O);	View = O;			}
	IC		void					set_Transform			(Fmatrix*	M	)			{ VERIFY(M);	pTransform = M;		}
	IC		void					set_LightLevel			(int		L	)			{ iLightLevel = L;					}
	virtual void					set_Object				(CObject*	O	);
	virtual void					add_Visual				(CVisual*	V	);			// add visual leaf	(no culling performed at all)
	virtual void					add_Geometry			(CVisual*	V	);			// add visual(s)	(all culling performed)
	virtual void					add_Lights				(vector<WORD> &V);
	virtual void					add_Glows				(vector<WORD> &V);
	virtual void					add_Patch				(Shader* S, Fvector& P1, float s, float a, BOOL bNearer);

	// Models
	virtual CVisual*				model_CreatePS			(LPCSTR name, PS::SEmitter* E);
	virtual CVisual*				model_Create			(LPCSTR name);
	virtual CVisual*				model_Create			(CStream* data);
	virtual CVisual*				model_Duplicate			(CVisual* V);
	virtual void					model_Delete			(CVisual* &V);
	
	// Main
	virtual void					Calculate				();
	virtual void					Render					();
	virtual void					Screenshot				();

	// Render mode
	virtual void					rmNear					();
	virtual void					rmFar					();
	virtual void					rmNormal				();

	// Constructor/destructor
	CRender_interface();
	virtual ~CRender_interface();
};

extern ENGINE_API CRender_interface*	Render;
	
#endif
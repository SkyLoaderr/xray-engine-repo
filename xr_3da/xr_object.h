#ifndef __XR_OBJECT_H__
#define __XR_OBJECT_H__

#include "fbasicvisual.h"
#include "xr_collide_form.h"
#include "fcontroller.h"
#include "xrSheduler.h"

// refs
class	ENGINE_API CCFModel;
class	ENGINE_API CInifile;
class	ENGINE_API CSector;
class	ENGINE_API CCustomHUD;
class	ENGINE_API NET_Packet;
struct	ENGINE_API NodeCompressed;

enum ESectorMode						{EPM_AT_LOAD, EPM_AUTO };

//-----------------------------------------------------------------------------------------------------------
//	CObject
//-----------------------------------------------------------------------------------------------------------
class	ENGINE_API						CObject :	
public CEventBase,
public CSheduled,
public DLL_Pure, 
public CController,
public pureDeviceDestroy,
public pureDeviceCreate
{
public:
	struct SavedPosition
	{
		DWORD	dwTime;
		Fvector	vPosition;
	};
protected:
	// some property variables
	bool								rbEnabled;
	bool								rbVisible;
	BOOL								bActive;		// was it activated or not - sleeping, not updating, no network messages etc.
	LPSTR								NameObject;
	LPSTR								NameSection;
protected:
	// Geometric (transformation)
	Fvector								vPosition;
	Fvector								vScale;
	Fmatrix								mRotate;
	CCFModel*							cfModel;
	svector<SavedPosition,4>			PositionStack;
	
	// Model
	LPSTR								pVisualName;
	FBasicVisual*						pVisual;

	// Visibility detection
	CSector*							pSector;
	ESectorMode							SectorMode;

	// Shadow
	Shader*								sh_Shader;
	float								sh_Size;

	// Information and status
	void								StatusBegin		();

	// Network
	DWORD								net_ID;
	BOOL								net_Local;
	BOOL								net_Ready;

	// Geometry xform
	Fmatrix								svTransform;
	Fmatrix								clTransform;
public:
	// Network
	IC BOOL								Local()			{ return net_Local;		}
	IC BOOL								Remote()		{ return !net_Local;	}
	IC DWORD							ID()			{ return net_ID;		}
	virtual BOOL						Ready()			{ return net_Ready; }		
	
	// Geometry xform
	void								UpdateTransform	(void);
	IC void								svCenter		(Fvector& C) const	{ VERIFY(pVisual); svTransform.transform_tiny(C,pVisual->bv_Position);	}
	IC void								clCenter		(Fvector& C) const	{ VERIFY(pVisual); clTransform.transform_tiny(C,pVisual->bv_Position);	}
	IC const Fmatrix&					svXFORM			()			 const	{ return svTransform;	}
	IC const Fmatrix&					clXFORM			()			 const	{ return clTransform;	}
	
	IC CSector*							Sector			()					{ return pSector; }

	IC float							Radius			() const			{ VERIFY(pVisual); return pVisual->bv_Radius;}
	virtual Fvector&					Position		() 					{ return vPosition; }
	IC Fvector&							Direction		() 					{ return mRotate.k; }
	IC Fmatrix&							Rotation		()					{ return mRotate;	}
	IC FBasicVisual*					Visual			()					{ return pVisual;   }
	IC CCFModel*						CFORM			() const			{ return cfModel;	}
	IC void								ForcePosition	(Fvector& P)		{ vPosition.set(P); UpdateTransform(); }
	IC Shader*							shadowShader	()					{ return sh_Shader;	}
	IC float							shadowSize		()					{ return sh_Size;	}

	// Name management
	IC LPCSTR							cName			()					{ return NameObject;	}
	void								cName_set		(LPCSTR N);
	IC LPCSTR							cNameSect		()					{ return NameSection;	}
	void								cNameSect_set	(LPCSTR N);
	
	// Visible property
	PropertyGP(getVisible,setVisible)	BOOL bVisible;
	void								setVisible		(bool _Visible)		{ rbVisible = _Visible; }
	BOOL								getVisible		()					{ return rbVisible;	}

	// Enabled property
	PropertyGP(getEnable,setEnable)		BOOL bEnabled;
	IC void								setEnable		(bool _enabled)		{ rbEnabled = _enabled; if (cfModel) cfModel->Enable(_enabled); }
	IC BOOL								getEnable		()					{ return rbEnabled;	}

	//---------------------------------------------------------------------
										CObject			();
	virtual								~CObject		();

	virtual void						Load			(CInifile* ini, const char * section);
	virtual BOOL						Spawn			(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags);
	virtual void						Sector_Detect	();
	virtual void						Sector_Move		(CSector* P);
	
	// Update
	virtual void						OnVisible		(void);								// returns lighting level
	virtual void						Update			(DWORD dt);							// Called by sheduler
	virtual void						UpdateCL		();									// Called each frame, so no need for dt
	virtual void						net_Export		(NET_Packet* P) {};					// export to server
	virtual void						net_Import		(NET_Packet* P) {};					// import from server
	virtual BOOL						net_Relevant	()				{ return FALSE; };	// relevant for export to server

	// Position stack
	IC DWORD							ps_Size			()				{ return PositionStack.size(); }
	virtual	SavedPosition				ps_Element		(DWORD ID);

	// Collision
	virtual void						OnNear			(CObject* near)		{};

	// HUD
	virtual void						OnHUDDraw		(CCustomHUD* hud)	{};

	// Active/non active
	virtual void						OnActivate		();
	virtual void						OnDeactivate	();
	
	// Device dependance
	virtual void						OnDeviceDestroy	();
	virtual void						OnDeviceCreate	();
	virtual void						OnEvent			(EVENT E, DWORD P1, DWORD P2) {};
};

#endif //__XR_OBJECT_H__

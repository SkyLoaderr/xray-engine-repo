#ifndef __XR_OBJECT_H__
#define __XR_OBJECT_H__

#include "fcontroller.h"
#include "xrSheduler.h"

// refs
class	ENGINE_API CVisual;
class	ENGINE_API CCFModel;
class	ENGINE_API CInifile;
class	ENGINE_API CSector;
class	ENGINE_API CCustomHUD;
class	ENGINE_API CLightTrack;
class	ENGINE_API NET_Packet;
struct	ENGINE_API NodeCompressed;

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
	enum ESectorMode					{EPM_AT_LOAD, EPM_AUTO, EPM_NONE };
	
	struct SavedPosition
	{
		DWORD	dwTime;
		Fvector	vPosition;
	};
protected:
	// some property variables
	BOOL								bEnabled;
	BOOL								bVisible;
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
	CVisual*							pVisual;

	// Visibility detection
	CSector*							pSector;
	ESectorMode							SectorMode;
	CLightTrack*						pLights;

	// Information and status
	void								StatusBegin		();

	// Network
	DWORD								net_ID;
	BOOL								net_Local;
	BOOL								net_Ready;

	// Geometry xform
	Fmatrix								svTransform;
	Fmatrix								clTransform;

	// Parentness
	CObject*							Parent;
public:
	// Network
	IC BOOL								Local			()					{ return net_Local;		}
	IC BOOL								Remote			()					{ return !net_Local;	}
	IC DWORD							ID				()					{ return net_ID;		}
	virtual BOOL						Ready			()					{ return net_Ready;		}
	virtual float						shedule_Scale	()					{ return Device.vCameraPosition.distance_to(Position())/200.f; }

	// Parentness
	IC CObject*							H_Parent		()					{ return Parent;		}
	IC CObject*							H_Root			()					{ return Parent?Parent->H_Root():this;	}

	// Geometry xform
	void								UpdateTransform	(void);
	void								svCenter		(Fvector& C) const;
	void								clCenter		(Fvector& C) const;
	IC const Fmatrix&					svXFORM			()			 const	{ return svTransform;	}
	IC const Fmatrix&					clXFORM			()			 const	{ return clTransform;	}
	
	IC CSector*							Sector			()					{ return pSector;		}
	IC CLightTrack*						Lights			()					{ return pLights;		}
	virtual float						Ambient			()					{ return 0.f;		 	}
	virtual BOOL						ShadowGenerate	()					{ return TRUE;			}
	virtual BOOL						ShadowReceive	()					{ return TRUE;			}
	
	virtual float						Radius			() const;
	virtual Fvector&					Position		() 					{ return vPosition;		}
	IC Fvector&							Direction		() 					{ return mRotate.k;		}
	IC Fmatrix&							Rotation		()					{ return mRotate;		}
	IC CVisual*							Visual			()					{ return pVisual;		}
	IC CCFModel*						CFORM			() const			{ return cfModel;		}

	// Name management
	virtual LPCSTR						cName			()					{ return NameObject;	}
	void								cName_set		(LPCSTR N);
	IC LPCSTR							cNameSect		()					{ return NameSection;	}
	void								cNameSect_set	(LPCSTR N);
	
	// Visible property
	IC void								setVisible		(BOOL _visible)		{ bVisible = _visible;	}
	IC BOOL								getVisible		()					{ return bVisible;		}

	// Enabled property
	void								setEnabled		(BOOL _enabled);
	IC BOOL								getEnabled		()					{ return bEnabled;		}

	//---------------------------------------------------------------------
										CObject			();
	virtual								~CObject		();

	virtual void						Load			(LPCSTR section);
	virtual void						Sector_Detect	();
	virtual void						Sector_Move		(CSector* P);
	
	// Update
	virtual void						OnVisible		(void);								// returns lighting level
	virtual void						Update			(DWORD dt);							// Called by sheduler
	virtual void						UpdateCL		();									// Called each frame, so no need for dt
	virtual BOOL						net_Spawn		(BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags);
	virtual void						net_Destroy		();
	virtual void						net_Export		(NET_Packet& P) {};					// export to server
	virtual void						net_Import		(NET_Packet& P) {};					// import from server
	virtual BOOL						net_Relevant	()				{ return FALSE; };	// relevant for export to server

	// Position stack
	IC DWORD							ps_Size			()				{ return PositionStack.size(); }
	virtual	SavedPosition				ps_Element		(DWORD ID);

	// Collision/Feedback/Interaction
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

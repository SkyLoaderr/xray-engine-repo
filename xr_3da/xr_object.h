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
	union ObjectFlags
	{
		struct 
		{
			DWORD	bEnabled		:	1;
			DWORD	bVisible		:	1;
			DWORD	bActive			:	1;
			DWORD	bDestroy		:	1;
			DWORD	net_Local		:	1;
			DWORD	net_Ready		:	1;
		};
		DWORD	storage;
	};
private:
	ObjectFlags							FLAGS;
	DWORD								net_ID;

	// Some property variables
	LPSTR								NameObject;
	LPSTR								NameSection;
	LPSTR								NameVisual;

	// Visibility detection
	CSector*							pSector;
	ESectorMode							SectorMode;
	CLightTrack*						pLights;
protected:
	// Geometric (transformation)
	Fvector								vPosition;
	Fvector								vScale;
	Fmatrix								mRotate;
	CCFModel*							cfModel;
	svector<SavedPosition,4>			PositionStack;
	
	// Model
	CVisual*							pVisual;

	// Information and status
	void								StatusBegin			();

	// Geometry xform
	Fmatrix								svTransform;
	Fmatrix								clTransform;

	// Parentness
	CObject*							Parent;
public:
	DWORD								dwFrame_UpdateCL;

	// Network
	IC BOOL								Local				()					{ return FLAGS.net_Local;	}
	IC BOOL								Remote				()					{ return !FLAGS.net_Local;	}
	IC DWORD							ID					()					{ return net_ID;			}
	IC void								setID				(DWORD _ID)			{ net_ID = _ID;				}
	virtual BOOL						Ready				()					{ return FLAGS.net_Ready;	}
	virtual float						shedule_Scale		()					{ return Device.vCameraPosition.distance_to(Position())/200.f; }

	// Parentness
	IC CObject*							H_Parent			()					{ return Parent;		}
	IC CObject*							H_Root				()					{ return Parent?Parent->H_Root():this;		}
	virtual CObject*					H_SetParent			(CObject* O);

	// Geometry xform
	void								UpdateTransform		(void);
	void								svCenter			(Fvector& C) const;
	void								clCenter			(Fvector& C) const;
	IC const Fmatrix&					svXFORM				()			 const	{ return svTransform;		}
	IC const Fmatrix&					clXFORM				()			 const	{ return clTransform;		}

	IC Fvector&							Direction			() 					{ return mRotate.k;			}
	IC Fmatrix&							Rotation			()					{ return mRotate;			}
	virtual float						Radius				() const;
	virtual Fvector&					Position			() 					{ return vPosition;			}
	
	IC CSector*							Sector				()					{ return H_Root()->pSector;	}
	IC CLightTrack*						Lights				()					{ return pLights;			}
	virtual float						Ambient				()					{ return 0.f;		 		}
	virtual BOOL						ShadowGenerate		()					{ return TRUE;				}
	virtual BOOL						ShadowReceive		()					{ return TRUE;				}

	// SLS
	virtual void						SLS_Save			(CFS_Base& fs)		{};
	virtual void						SLS_Load			(CStream& fs)		{};
	
	// Accessors
	IC CVisual*							Visual				()					{ return pVisual;			}
	IC CCFModel*						CFORM				() const			{ return cfModel;			}

	// Name management
	virtual LPCSTR						cName				()					{ return NameObject;		}
	void								cName_set			(LPCSTR N);
	IC LPCSTR							cNameSect			()					{ return NameSection;		}
	void								cNameSect_set		(LPCSTR N);
	IC LPCSTR							cNameVisual			()					{ return NameVisual;		}
	void								cNameVisual_set		(LPCSTR N);
	
	// Properties
	IC void								setVisible			(BOOL _visible)		{ FLAGS.bVisible = _visible?1:0;	}
	IC BOOL								getVisible			()					{ return FLAGS.bVisible;			}
	void								setEnabled			(BOOL _enabled);
	IC BOOL								getEnabled			()					{ return FLAGS.bEnabled;			}
	IC void								setActive			(BOOL _active)		{ FLAGS.bActive  = _active?1:0;		}
	IC BOOL								getActive			()					{ return FLAGS.bActive;				}
	IC void								setDestroy			(BOOL _destroy)		{ FLAGS.bDestroy = _destroy?1:0;	}
	IC BOOL								getDestroy			()					{ return FLAGS.bDestroy;			}
	IC void								setLocal			(BOOL _local)		{ FLAGS.net_Local = _local?1:0;		}
	IC BOOL								getLocal			()					{ return FLAGS.net_Local;			}
	IC void								setReady			(BOOL _ready)		{ FLAGS.net_Ready = _ready?1:0;		}
	IC BOOL								getReady			()					{ return FLAGS.net_Ready;			}

	//---------------------------------------------------------------------
										CObject				();
	virtual								~CObject			();

	virtual void						Load				(LPCSTR section);
	virtual void						Sector_Detect		();
	virtual void						Sector_Move			(CSector* P);
	
	// Update
	virtual void						OnVisible			(void);								// returns lighting level
	virtual void						Update				(DWORD dt);							// Called by sheduler
	virtual void						UpdateCL			();									// Called each frame, so no need for dt
	virtual BOOL						net_Spawn			(LPVOID data);
	virtual void						net_Destroy			();
	virtual void						net_Export			(NET_Packet& P) {};					// export to server
	virtual void						net_Import			(NET_Packet& P) {};					// import from server
	virtual BOOL						net_Relevant		()				{ return FALSE; };	// relevant for export to server
	virtual void						net_MigrateInactive	(NET_Packet& P)	{ FLAGS.net_Local = FALSE;		};
	virtual void						net_MigrateActive	(NET_Packet& P)	{ FLAGS.net_Local = TRUE;		};

	// Position stack
	IC DWORD							ps_Size				()				{ return PositionStack.size(); }
	virtual	SavedPosition				ps_Element			(DWORD ID);

	// HUD
	virtual void						OnHUDDraw			(CCustomHUD* hud)	{};

	// Active/non active
	virtual void						OnActivate			();
	virtual void						OnDeactivate		();
	virtual void						OnH_B_Chield		();		// before
	virtual void						OnH_B_Independent	();
	virtual void						OnH_A_Chield		();		// after
	virtual void						OnH_A_Independent	();
	
	// Device dependance
	virtual void						OnDeviceDestroy		();
	virtual void						OnDeviceCreate		();
	virtual void						OnEvent				(EVENT E, DWORD P1, DWORD P2) {};
};

#endif //__XR_OBJECT_H__

#ifndef __XR_OBJECT_H__
#define __XR_OBJECT_H__

#include "ispatial.h"
#include "isheduled.h"
//#include "iinputreceiver.h"
#include "irenderable.h"
#include "icollidable.h"

// refs
class	ENGINE_API IRender_Visual;
class	ENGINE_API IRender_Sector;
class	ENGINE_API IRender_ObjectSpecific;
class	ENGINE_API CCustomHUD;
class	ENGINE_API NET_Packet;
struct	ENGINE_API NodeCompressed;

//-----------------------------------------------------------------------------------------------------------
//	CObject
//-----------------------------------------------------------------------------------------------------------
#pragma pack(push,4)
class	ENGINE_API						CObject :	
	public DLL_Pure,
	public ISpatial,
	public ISheduled,
	public IRenderable,
	public ICollidable	//,
	//public IInputReceiver,
	//public IEventReceiver
{
public:
	struct	SavedPosition
	{
		u32			dwTime;
		Fvector		vPosition;
	};
	union	ObjectProperties
	{
		struct 
		{
			u32	net_ID			:	16;
			u32	bActiveCounter	:	8;
			u32	bEnabled		:	1;
			u32	bVisible		:	1;
			u32	bDestroy		:	1;
			u32	net_Local		:	1;
			u32	net_Ready		:	1;
			u32 net_SV_Update	:	1;
		};
		u32	storage;
	};
private:
	// Some property variables
	ObjectProperties					Props;
	shared_str							NameObject;
	shared_str							NameSection;
	shared_str							NameVisual;
protected:
	// Parentness
	CObject*							Parent;

	// Geometric (transformation)
	svector<SavedPosition,4>			PositionStack;
public:
#ifdef DEBUG
	u32									dbg_update_cl;
#endif
	u32									dwFrame_UpdateCL;

	// Network
	IC BOOL								Local				()			const	{ return Props.net_Local;	}
	IC BOOL								Remote				()			const	{ return !Props.net_Local;	}
	IC u16								ID					()			const	{ return Props.net_ID;		}
	IC void								setID				(u16 _ID)			{ Props.net_ID = _ID;		}
	virtual BOOL						Ready				()					{ return Props.net_Ready;	}
	virtual float						shedule_Scale		()					{ return Device.vCameraPosition.distance_to(Position())/200.f; }

	// Parentness
	IC CObject*							H_Parent			()					{ return Parent;						}
	IC const CObject*					H_Parent			()			const	{ return Parent;						}
	CObject*							H_Root				()					{ return Parent?Parent->H_Root():this;	}
	const CObject*						H_Root				()			const	{ return Parent?Parent->H_Root():this;	}
	virtual CObject*					H_SetParent			(CObject* O);

	// Geometry xform
	virtual void						Center				(Fvector& C) const;
	IC const Fmatrix&					XFORM				()			 const	{ VERIFY(_valid(renderable.xform));	return renderable.xform;	}
	IC Fmatrix&							XFORM				()					{ return renderable.xform;			}
	virtual void						spatial_register	();
	virtual void						spatial_unregister	();
	virtual void						spatial_move		();

	IC Fvector&							Direction			() 					{ return renderable.xform.k;		}
	IC const Fvector&					Direction			() 			const	{ return renderable.xform.k;		}
	IC Fvector&							Position			() 					{ return renderable.xform.c;		}
	IC const Fvector&					Position			() 			const	{ return renderable.xform.c;		}
	virtual float						Radius				()			const;
	virtual const Fbox&					BoundingBox			()			const;
	
	IC IRender_Sector*					Sector				()					{ return H_Root()->spatial.sector;	}
	IC IRender_ObjectSpecific*			ROS					()					{ return renderable.ROS;			}
	virtual float						renderable_Ambient			()			{ return 0.f;		 				}
	virtual BOOL						renderable_ShadowGenerate	()			{ return TRUE;						}
	virtual BOOL						renderable_ShadowReceive	()			{ return TRUE;						}

	// Accessors and converters
	IC IRender_Visual*					Visual				()					{ return renderable.visual;			}
	IC ICollisionForm*					CFORM				() const			{ return collidable.model;			}
	virtual		CObject*				dcast_CObject		()					{ return this;						}
	virtual		IRenderable*			dcast_Renderable	()					{ return this;						}

	// Name management
	IC shared_str							cName				()			const	{ return NameObject;				}
	void								cName_set			(shared_str N);
	IC shared_str							cNameSect			()			const	{ return NameSection;				}
	void								cNameSect_set		(shared_str N);
	IC shared_str							cNameVisual			()			const	{ return NameVisual;				}
	void								cNameVisual_set		(shared_str N);
	
	// Properties
	void								processing_activate		();				// request	to enable	UpdateCL
	void								processing_deactivate	();				// request	to disable	UpdateCL
	bool								processing_enabled		()				{ return 0!=Props.bActiveCounter;	}

	void								setVisible			(BOOL _visible);
	IC BOOL								getVisible			()			const	{ return Props.bVisible;			}
	void								setEnabled			(BOOL _enabled);
	IC BOOL								getEnabled			()			const	{ return Props.bEnabled;			}
	IC void								setDestroy			(BOOL _destroy)		{ Props.bDestroy = _destroy?1:0; if(_destroy)	processing_activate(); }
	IC BOOL								getDestroy			()			const	{ return Props.bDestroy;			}
	IC void								setLocal			(BOOL _local)		{ Props.net_Local = _local?1:0;		}
	IC BOOL								getLocal			()			const	{ return Props.net_Local;			}
	IC void								setSVU				(BOOL _svu)			{ Props.net_SV_Update	= _svu?1:0;	}
	IC BOOL								getSVU				()			const	{ return Props.net_SV_Update;		}
	IC void								setReady			(BOOL _ready)		{ Props.net_Ready = _ready?1:0;		}
	IC BOOL								getReady			()			const	{ return Props.net_Ready;			}

	//---------------------------------------------------------------------
										CObject				();
	virtual								~CObject			();

	virtual void						Load				(LPCSTR section);
	
	// Update
	virtual void						shedule_Update		(u32 dt);							// Called by sheduler
	virtual void						renderable_Render	();

	virtual void						UpdateCL			();									// Called each frame, so no need for dt
	virtual BOOL						net_Spawn			(LPVOID data);
	virtual void						net_Destroy			();
	virtual void						net_Export			(NET_Packet& P) {};					// export to server
	virtual void						net_Import			(NET_Packet& P) {};					// import from server
	virtual	void						net_ImportInput		(NET_Packet& P)	{};
	virtual BOOL						net_Relevant		()				{ return FALSE; };	// relevant for export to server
	virtual void						net_MigrateInactive	(NET_Packet& P)	{ Props.net_Local = FALSE;		};
	virtual void						net_MigrateActive	(NET_Packet& P)	{ Props.net_Local = TRUE;		};
	virtual void						net_Relcase			(CObject*	 O) { };				// destroy all links to another objects

	// Position stack
	IC u32								ps_Size				()			const	{ return PositionStack.size(); }
	virtual	SavedPosition				ps_Element			(u32 ID)	const;
	virtual void						ForceTransform		(const Fmatrix& m)	{};

	// HUD
	virtual void						OnHUDDraw			(CCustomHUD* hud)	{};

	// Active/non active
	virtual void						OnH_B_Chield		();		// before
	virtual void						OnH_B_Independent	();
	virtual void						OnH_A_Chield		();		// after
	virtual void						OnH_A_Independent	();
	
	// Device dependance
	// virtual void						OnEvent				(EVENT E, u64 P1, u64 P2) {};
};

#pragma pack(pop)

#endif //__XR_OBJECT_H__

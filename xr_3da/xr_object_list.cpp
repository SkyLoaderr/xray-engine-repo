#include "stdafx.h"
#include "igame_level.h"
#include "igame_persistent.h"

#include "xrSheduler.h"
#include "xr_object_list.h"
#include "std_classes.h"

#include "xr_object.h"
#include "net_utils.h"

class fClassEQ {
	CLASS_ID cls;
public:
	fClassEQ(CLASS_ID C) : cls(C) {};
	IC bool operator() (CObject* O) { return cls==O->CLS_ID; }
};

CObjectList::CObjectList	( )
{
}

CObjectList::~CObjectList	( )
{
	R_ASSERT(objects_active.empty()		);
	R_ASSERT(objects_sleeping.empty()	);
	R_ASSERT(destroy_queue.empty()		);
	R_ASSERT(map_NETID.empty()			);
}

CObject*	CObjectList::FindObjectByName	( shared_str name )
{
	for (xr_vector<CObject*>::iterator I=objects_active.begin(); I!=objects_active.end(); I++)
		if ((*I)->cName().equal(name))	return (*I);
	for (xr_vector<CObject*>::iterator I=objects_sleeping.begin(); I!=objects_sleeping.end(); I++)
		if ((*I)->cName().equal(name))	return (*I);
	return	NULL;
}
CObject*	CObjectList::FindObjectByName	( LPCSTR name )
{
	return	FindObjectByName				(shared_str(name));
}

CObject*	CObjectList::FindObjectByCLS_ID	( CLASS_ID cls )
{
	{
		xr_vector<CObject*>::iterator O	= std::find_if(objects_active.begin(),objects_active.end(),fClassEQ(cls));
		if (O!=objects_active.end())	return *O;
	}
	{
		xr_vector<CObject*>::iterator O	= std::find_if(objects_sleeping.begin(),objects_sleeping.end(),fClassEQ(cls));
		if (O!=objects_sleeping.end())	return *O;
	}

	return	NULL;
}


void	CObjectList::o_remove		( xr_vector<CObject*>&	v,  CObject* O)
{
	xr_vector<CObject*>::iterator _i	= std::find(v.begin(),v.end(),O);
	VERIFY					(_i!=v.end());
	v.erase					(_i);
}

void	CObjectList::o_activate		( CObject*		O		)
{
	VERIFY						(O && O->processing_enabled());
	o_remove					(objects_sleeping,O);
	objects_active.push_back	(O);
}
void	CObjectList::o_sleep		( CObject*		O		)
{
	VERIFY	(O && !O->processing_enabled());
	o_remove					(objects_active,O);
	objects_sleeping.push_back	(O);
}

void	CObjectList::SingleUpdate	(CObject* O)
{
	if (O->processing_enabled() &&	(Device.dwFrame != O->dwFrame_UpdateCL))
	{
		if (O->H_Parent())		SingleUpdate(O->H_Parent());
		Device.Statistic.UpdateClient_active	++;
		O->dwFrame_UpdateCL		=				Device.dwFrame;
		O->UpdateCL				();
		VERIFY3					(O->dbg_update_cl == Device.dwFrame, "Broken sequence of calls to 'UpdateCL'",*O->cName());
		if (O->getDestroy() && !O->shedule.b_locked)	destroy_queue.push_back(O);
		else if (O->H_Parent() && (O->H_Parent()->getDestroy() || O->H_Root()->getDestroy()) && !O->shedule.b_locked)	{
			// Push to destroy-queue if it isn't here already
			Msg	("! ERROR: incorrect destroy sequence for object[%d:%s], section[%s]",O->ID(),*O->cName(),*O->cNameSect());
			if (std::find(destroy_queue.begin(),destroy_queue.end(),O)==destroy_queue.end())
				destroy_queue.push_back	(O);
		}
	}
}

void CObjectList::Update		()
{
	// Clients
	if (Device.fTimeDelta>EPS_S)			{
		Device.Statistic.UpdateClient_active	= objects_active.size	();
		Device.Statistic.UpdateClient_total		= objects_active.size	() + objects_sleeping.size();
		Device.Statistic.UpdateClient.Begin		();
		for (xr_vector<CObject*>::iterator O=objects_active.begin(); O!=objects_active.end(); O++) 
			SingleUpdate	(*O);
		Device.Statistic.UpdateClient.End		();
	}

	// Destroy
	if (!destroy_queue.empty()) 
	{
		// Info
		for (xr_vector<CObject*>::iterator oit=objects_active.begin(); oit!=objects_active.end(); oit++)
			for (int it = destroy_queue.size()-1; it>=0; it--)	(*oit)->net_Relcase	(destroy_queue[it]);
		for (xr_vector<CObject*>::iterator oit=objects_sleeping.begin(); oit!=objects_sleeping.end(); oit++)
			for (int it = destroy_queue.size()-1; it>=0; it--)	(*oit)->net_Relcase	(destroy_queue[it]);

		// Destroy
		for (int it = destroy_queue.size()-1; it>=0; it--)
		{
			CObject*		O	= destroy_queue[it];
			O->net_Destroy	( );
			Destroy			(O);
		}
		destroy_queue.clear	();
	}
}

void CObjectList::net_Register		(CObject* O)
{
	R_ASSERT		(O);
	map_NETID.insert(mk_pair(O->ID(),O));
	//Msg			("-------------------------------- Register: %s",O->cName());
}

void CObjectList::net_Unregister	(CObject* O)
{
	xr_map<u32,CObject*>::iterator	it = map_NETID.find(O->ID());
	if ((it!=map_NETID.end()) && (it->second == O))	{
		// Msg			("-------------------------------- Unregster: %s",O->cName());
		map_NETID.erase(it);
	}
}

u32	CObjectList::net_Export			(NET_Packet* _Packet,	u32 start, u32 count	)
{
	NET_Packet& Packet	= *_Packet;
	u32			position;
	for (; start<objects_active.size() + objects_sleeping.size(); start++)			{
		CObject* P = (start<objects_active.size()) ? objects_active[start] : objects_sleeping[start-objects_active.size()];
		if (P->net_Relevant() && !P->getDestroy())	{
			Packet.w_u16			(u16(P->ID())	);
			Packet.w_chunk_open8	(position);
			P->net_Export			(Packet);
#ifdef DEBUG
			u32 size				= u32		(Packet.w_tell()-position)-sizeof(u8);
			if				(size>=256)			{
				Debug.fatal	("Object [%s][%d] exceed network-data limit\n size=%d, Pend=%d, Pstart=%d",
					*P->cName(), P->ID(), size, Packet.w_tell(), position);
			}
#endif
			Packet.w_chunk_close8	(position);
			if (0==(--count))		break;
		}
	}
	return	start;
}

void CObjectList::net_Import		(NET_Packet* Packet)
{
	while (!Packet->r_eof())
	{
		u16 ID;		Packet->r_u16	(ID);
		u8  size;	Packet->r_u8	(size);
		CObject* P  = net_Find		(u32(ID));
		if (P)		P->net_Import	(*Packet);
		else		Packet->r_advance(size);
	}
}

CObject* CObjectList::net_Find			(u32 ID)
{
	xr_map<u32,CObject*>::iterator	it = map_NETID.find(ID);
	return (it==map_NETID.end())?0:it->second;
}

void CObjectList::Load		()
{
	R_ASSERT				(map_NETID.empty() && objects_active.empty() && destroy_queue.empty() && objects_sleeping.empty());
}

void CObjectList::Unload	( )
{
	// Destroy objects
	while (objects_sleeping.size())
	{
		CObject*	O	= objects_sleeping.back	();
		O->net_Destroy	(   );
		Destroy			( O );
	}
	while (objects_active.size())
	{
		CObject*	O	= objects_active.back	();
		O->net_Destroy	(   );
		Destroy			( O );
	}
}

CObject*	CObjectList::Create				( LPCSTR	name	)
{
	CObject*	O				= g_pGamePersistent->ObjectPool.create(name);
	objects_sleeping.push_back	(O);
	return						O;
}

void		CObjectList::Destroy			( CObject*	O		)
{
	if (0==O)								return;
	net_Unregister							(O);

	xr_vector<CObject*>::iterator _i		= std::find(objects_active.begin(),objects_active.end(),O);
	if	(_i!=objects_active.end())			objects_active.erase	(_i);
	else {
		xr_vector<CObject*>::iterator _ii	= std::find(objects_sleeping.begin(),objects_sleeping.end(),O);
		if	(_ii!=objects_sleeping.end())	objects_sleeping.erase	(_ii);
		else	Device.fatal	("! Unregistered object being destroyed");
	}
	g_pGamePersistent->ObjectPool.destroy	(O);
}

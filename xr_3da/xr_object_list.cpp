#include "stdafx.h"
#include "xr_object_list.h"
#include "xr_tokens.h"
#include "std_classes.h"
#include "xr_creator.h"
#include "xr_ini.h"

#include "xr_object.h"
#include "net_utils.h"

class fNameEQ {
	LPCSTR	name;
public:
	fNameEQ(LPCSTR N) : name(N) {};
	IC bool operator() (CObject* O) { return strcmp(O->cName(),name)==0; }
};
class fClassEQ {
	CLASS_ID cls;
public:
	fClassEQ(CLASS_ID C) : cls(C) {};
	IC bool operator() (CObject* O) { return cls==O->SUB_CLS_ID; }
};

CObjectList::CObjectList	( )
{
	Device.seqDevCreate.Add		(this, REG_PRIORITY_LOW);
	Device.seqDevDestroy.Add	(this, REG_PRIORITY_LOW);
}

CObjectList::~CObjectList	( )
{
	Device.seqDevCreate.Remove	(this);
	Device.seqDevDestroy.Remove	(this);
	R_ASSERT(objects.size()==0);
}

void CObjectList::Unload	( )
{
	for (u32 i=0; i<objects.size(); i++) {
		CObject *pObject = objects[i];
		_DELETE	( pObject );
	}
	objects.clear();
}

CObject* CObjectList::LoadOne( LPCSTR Name )
{
	CLASS_ID CLS		= pSettings->ReadCLSID(Name,"class");
	CObject* pObject	= (CObject*) NEW_INSTANCE(CLS);
	pObject->Load		(Name);
	objects.push_back	(pObject);
	return				pObject;
}

CObject*	CObjectList::FindObjectByName	( LPCSTR name )
{
	OBJ_IT O=find_if(objects.begin(),objects.end(),fNameEQ(name));
	if (O!=objects.end())	return *O;
	else					return NULL;
}

CObject*	CObjectList::FindObjectByCLS_ID	( CLASS_ID cls )
{
	OBJ_IT O=find_if(objects.begin(),objects.end(),fClassEQ(cls));
	if (O!=objects.end())	return *O;
	else					return NULL;
}

void CObjectList::DestroyObject	( CObject *pObject )
{
	if (0==pObject)			return;
	net_Unregister			(pObject);
	OBJ_IT it				= find(objects.begin(),objects.end(),pObject);
	if (it!=objects.end())	objects.erase(it);
	DEL_INSTANCE			(pObject);
}

void CObjectList::DestroyObject	( u32 ID )
{
	DestroyObject(net_Find(ID));
}

void CObjectList::SingleUpdate	(CObject* O)
{
	if (Device.dwFrame != O->dwFrame_UpdateCL)
	{
		if (O->H_Parent())		SingleUpdate(O->H_Parent());
		O->dwFrame_UpdateCL		= Device.dwFrame;
		O->UpdateCL				();
		if (O->getDestroy() && !O->shedule_Locked)								destroy_queue.push_back(O);
		if (O->H_Parent() && O->H_Parent()->getDestroy() && !O->shedule_Locked)	
		{
			// Push to destroy-queue if it isn't here already
			if (find(destroy_queue.begin(),destroy_queue.end(),O)==destroy_queue.end())
				destroy_queue.push_back	(O);
		}
	}
}

void CObjectList::OnMove		()
{
	// Clients
	Device.Statistic.UpdateClient.Begin		();
	for (OBJ_IT O=objects.begin(); O!=objects.end(); O++) 
		SingleUpdate(*O);
	Device.Statistic.UpdateClient.End		();

	// Destroy
	if (!destroy_queue.empty()) 
	{
		// Info
		for (OBJ_IT oit=objects.begin(); oit!=objects.end(); oit++)
		{
			for (int it = destroy_queue.size()-1; it>=0; it--)
				(*oit)->net_Relcase	(destroy_queue[it]);
		}

		// Destroy
		for (int it = destroy_queue.size()-1; it>=0; it--)
		{
			CObject*		O	= destroy_queue[it];
			O->net_Destroy	();
			DestroyObject	(O);
		}
		destroy_queue.clear	();
	}
}

void CObjectList::net_Register	(CObject* O)
{
	map_NETID.insert(make_pair(O->ID(),O));
}

void CObjectList::net_Unregister(CObject* O)
{
	map<u32,CObject*>::iterator	it = map_NETID.find(O->ID());
	if ((it!=map_NETID.end()) && (it->second == O))	map_NETID.erase(it);
}

void CObjectList::net_Export	(NET_Packet* _Packet)
{
	NET_Packet& Packet	= *_Packet;
	u32			position;
	for (OBJ_IT O=objects.begin(); O!=objects.end(); O++) 
	{
		CObject* P = *O;
		if (P->net_Relevant() && !P->getDestroy())	
		{
			Packet.w_u16			(u16(P->ID())	);
			Packet.w_chunk_open8	(position);
			P->net_Export			(Packet);
			Packet.w_chunk_close8	(position);
		}
	}
}

void CObjectList::net_Import		(NET_Packet* Packet)
{
	while (!Packet->r_eof())
	{
		u16 ID;		Packet->r_u16	(ID);
		u8  size;	Packet->r_u8	(size);
		CObject* P  = net_Find		(u32(ID));
		if (P)	P->net_Import		(*Packet);
		else	Packet->r_advance	(size);
	}
}

CObject* CObjectList::net_Find			(u32 ID)
{
	map<u32,CObject*>::iterator	it = map_NETID.find(ID);
	return (it==map_NETID.end())?0:it->second;
}

void	CObjectList::SLS_Save			(CFS_Base&	fs		)
{
	for (u32 i=0; i<objects.size(); i++) 
	{
		CObject *O		= objects[i];
		fs.open_chunk	(i);
		fs.Wword		(u16(O->ID()));
		O->SLS_Save		(fs);
		fs.close_chunk	();
	}
}

void	CObjectList::SLS_Load			(CStream&	fs		)
{
	u32 ID	= 0; 
	while (fs.FindChunk(ID)) 
	{
		u16 net_ID		= fs.Rword();
		CObject* O		= net_Find(net_ID);
		if (0==O)		Msg("! SLS_Load: Invalid ID found. (Object not spawned up to this moment?)");
		else			O->SLS_Load(fs);
		ID ++;
	}
}

void	CObjectList::OnDeviceCreate	()
{
	for (u32 i=0; i<objects.size(); i++) 
	{
		CObject *O			= objects[i];
		O->OnDeviceCreate	();
	}
}

void	CObjectList::OnDeviceDestroy()
{
	for (u32 i=0; i<objects.size(); i++) 
	{
		CObject *O			= objects[i];
		O->OnDeviceDestroy	();
	}
}

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
}

CObjectList::~CObjectList	( )
{
	R_ASSERT(objects.size()==0);
}

void CObjectList::Unload( ){
	for (DWORD i=0; i<objects.size(); i++) {
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

VOID CObjectList::DestroyObject	( CObject *pObject )
{
	if (0==pObject)			return;
	net_Unregister			(pObject);
	OBJ_IT it				= find(objects.begin(),objects.end(),pObject);
	if (it!=objects.end())	objects.erase(it);
	DEL_INSTANCE			(pObject);
}

VOID CObjectList::OnMove	( )
{
	// Sheduled
	Device.Statistic.UpdateSheduled.Begin	();
	sheduled.Update			( );
	Device.Statistic.UpdateSheduled.End		();

	// Clients
	Device.Statistic.UpdateClient.Begin		();
	for (OBJ_IT O=objects.begin(); O!=objects.end(); O++) 
		(*O)->UpdateCL();
	Device.Statistic.UpdateClient.End		();
}

VOID CObjectList::net_Register	(CObject* O)
{
	map_NETID.insert(make_pair(O->ID(),O));
}
VOID CObjectList::net_Unregister(CObject* O)
{
	map<u32,CObject*>::iterator	it = map_NETID.find(O->ID());
	if ((it!=map_NETID.end()) && (it->second == O))	map_NETID.erase(it);
}

VOID CObjectList::net_Export	(NET_Packet* Packet)
{
	for (OBJ_IT O=objects.begin(); O!=objects.end(); O++) 
	{
		CObject* P = *O;
		if (P->net_Relevant())	{
			Packet->w_u16			(u16(P->ID()));
			P->net_Export			(Packet);
		}
	}
}

VOID CObjectList::net_Import	(NET_Packet* Packet)
{
	while (!Packet->r_eof())
	{
		u16 ID;		Packet->r_u16	(ID);
		CObject* P  = net_Find		(DWORD(ID));
		if (P) P->net_Import		(Packet);
	}
}

CObject* CObjectList::net_Find	(u32 ID)
{
	map<u32,CObject*>::iterator	it = map_NETID.find(ID);
	return (it==map_NETID.end())?0:it->second;
}

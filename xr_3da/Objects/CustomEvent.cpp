// CustomEvent.cpp: implementation of the CCustomEvent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomEvent.h"

void	EV_LIST::_CreateOne(const char* DEF)
{
	char	Event[128],Param[128];
	Event[0]=0; Param[0]=0;
	sscanf	(DEF,"%[^,],%s",Event,Param);
	if (Event[0]) {
		EV_DEF	D;
		D.E  = Engine.Event.Create(Event); 
		D.P1 = strdup(Param); 
		List.push_back	(D);
	}
}
void	EV_LIST::Create	(const char* DEF)
{
	if (0==DEF || 0==strlen(DEF)) return;
	if (0==strchr(DEF,'}'))	{
		_CreateOne(DEF);
		return;
	}
	
	char	ONE[256];
	int		pos = 0;
	bool	bInside = FALSE;
	for (; *DEF; DEF++) {
		switch (*DEF) {
		case '{':
			{
				R_ASSERT(!bInside);
				bInside = TRUE;
			}
			break;
		case '}':
			{
				R_ASSERT(bInside);
				bInside = FALSE;
				ONE[pos]=0;
				_CreateOne(ONE);
				pos=0;
			}
			break;
		case ',':
			{
				if (bInside) ONE[pos++]=',';
			}
			break;
		default:
			ONE[pos++] = *DEF;
			break;
		}
	}
}
void	EV_LIST::Destroy	()
{
	for (DWORD i=0; i<List.size(); i++)
	{
		Engine.Event.Destroy(List[i].E);
		_FREE(List[i].P1);
	}
	List.clear();
}

void	EV_LIST::Signal	(DWORD P2)
{
	for (DWORD i=0; i<List.size(); i++)
		Engine.Event.Signal(List[i].E,DWORD(List[i].P1),P2); 
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomEvent::CCustomEvent()
{
//	Device.seqRender.Add(this,REG_PRIORITY_LOW-1111);
}

CCustomEvent::~CCustomEvent()
{
//	Device.seqRender.Remove(this);
}

void CCustomEvent::Load(CInifile* ini, const char * section)
{
	// Name
	R_ASSERT					(section);
	_FREE						(ObjectName);
	ObjectName					= strdup(section);
	SUB_CLS_ID					= CLSID_EVENT;
	
	// Geometry and transform
	Fvector dir,norm;
	vPosition					= ini->ReadVECTOR(section,"position");
	vScale						= ini->ReadVECTOR(section,"scale");
	dir							= ini->ReadVECTOR(section,"direction");
	norm						= ini->ReadVECTOR(section,"normal");
	mRotate.rotation			(dir,norm);
	UpdateTransform				();
	
	// General stuff
	pVisual						= NULL;
	rbVisible					= FALSE;
	
	// Collision model
	cfModel						= new CCF_EventBox(this);
	pCreator->ObjectSpace.Object_Register(this);
	cfModel->OnMove				();
	
	// Events
	OnEnter.Create				(ini->ReadSTRING(section,"OnEnter"));
	OnExit.Create				(ini->ReadSTRING(section,"OnExit"));
	
	// Target
	clsid_Target				= ini->ReadCLSID(section,"target_class");
}

void CCustomEvent::Update (DWORD dt)
{
	if (!Contacted.empty()) {
		for (DWORD i=0; i<Contacted.size(); i++) {
			// Check if it is still contact us
			CCF_EventBox* M = (CCF_EventBox*)CFORM(); R_ASSERT	(M);
			if (!M->Contact(Contacted[i])) {
				OnExit.Signal((DWORD)Contacted[i]);
				Contacted.erase(Contacted.begin()+i);
				i--;
			}
		}
	}
}

void CCustomEvent::OnNear( CObject* O )
{
	// check if target
	if (O->SUB_CLS_ID != clsid_Target) return;
	
	// check if not contacted before
	if (find(Contacted.begin(),Contacted.end(),O)!=Contacted.end()) return;
	
	// check if it is actually contacted
	CCF_EventBox* M = (CCF_EventBox*)CFORM(); R_ASSERT	(M);
	if (M->Contact(O)) {
		OnEnter.Signal((DWORD)O);
		Contacted.push_back(O);
		return;
	}
	
	return;
}

void CCustomEvent::OnRender()
{
	if (!bDebug)	return;
	Fvector H1; H1.set(0.5f,0.5f,0.5f);
	Device.Primitive.dbg_DrawOBB(svTransform,H1,D3DCOLOR_XRGB(255,255,255));
}

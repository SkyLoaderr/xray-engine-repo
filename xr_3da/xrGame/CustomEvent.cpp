// CustomEvent.cpp: implementation of the CCustomEvent class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CustomEvent.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCustomEvent::CCustomEvent()
{
}

CCustomEvent::~CCustomEvent		()
{
	_FREE						(ObjectName);
	OnEnter.Destroy				();
	OnExit.Destroy				();
}

void CCustomEvent::Load			(CInifile* ini, const char * section)
{
	// Name
	R_ASSERT					(section);
	cNameSET					(section);
	
	// Geometry and transform
	Fvector dir,norm;
	vPosition					= ini->ReadVECTOR(section,"position");
	vScale						= ini->ReadVECTOR(section,"scale");
	dir							= ini->ReadVECTOR(section,"direction");
	norm						= ini->ReadVECTOR(section,"normal");
	mRotate.rotation			(dir,norm);
	UpdateTransform				();
	
	// General stuff
	pVisualName					= NULL;
	pVisual						= NULL;
	rbVisible					= FALSE;
	
	// Sheduler
	dwMinUpdate					= 300;
	dwMaxUpdate					= 1000;
}

DEF_EVENT CCustomEvent::Parse		(LPCSTR DEF)
{
	DEF_EVENT	D;
	D.E			= 0;
	D.P1		= 0;
	string512	Event,Param;
	Event[0]=0; Param[0]=0;
	sscanf	(DEF,"%[^,],%s",Event,Param);
	if (Event[0]) {
		// Parse param's macroses
		char	Parsed	[1280], sBegin[1280], sName[1280], sEnd[1280], sBuf[128];
		sscanf	(Param,"%[^$]$rp$%[^$]$%s",sBegin,sName,sEnd);
		if (sName[0])	{
			int id		= Level().get_RPID(sName);
			R_ASSERT	(id>=0);
			strconcat	(Parsed,sBegin,itoa(id,sBuf,10),sEnd);
		} else {
			strcpy		(Parsed,Param);
		}
		
		// Create
		D.E  = Engine.Event.Create(Event); 
		D.P1 = strdup	(Parsed); 
	}
	return D;
}

BOOL CCustomEvent::Spawn		( BOOL bLocal, int server_id, Fvector& o_pos, Fvector& o_angle, NET_Packet& P, u16 flags )
{
	inherited::Spawn			(bLocal,server_id,o_pos,o_angle,P,flags);

	// Read CFORM
	{
		CCF_Shape*	shape			= new CCF_Shape	(this);
		cfModel						= shape;
		u8 count;	P.r_u8			(count);
		while (count)	{
			u8 type; P.r_u8	(type);
			switch (type)	{
			case 0:
				{
					Fsphere data;
					P.r					(&data,sizeof(data));
					shape->add_sphere	(data);
				}
				break;
			case 1:
				{
					Fmatrix data;
					P.r_matrix			(data);
					shape->add_box		(data);
				}
				break;
			}
			count--;
		}
		pCreator->ObjectSpace.Object_Register		(this);
		cfModel->OnMove				();
	}
	
	// Read actions
	{
		u8 count;	P.r_u8			(count);
		while (count)	{
			DEF_ACTION			A;
			string256			str;
			P.r_u8				(A.bOnce);
			P.r_u64				(A.CLS);
			P.r_string			(str);		A.OnEnter	= Parse(str);
			P.r_string			(str);		A.OnLeave	= Parse(str);
			A.bEnabled			= TRUE;
			Actions.push_back	(A);
			count--;
		}
	}

	return TRUE;
}

void CCustomEvent::Update (DWORD dt)
{
	if (!Contacted.empty()) {
		for (DWORD i=0; i<Contacted.size(); i++) {
			// Check if it is still contact us
			CCF_Shape* M = (CCF_Shape*)CFORM(); R_ASSERT	(M);
			if (!M->Contact(Contacted[i])) {
				for (DWORD a=0; a<Actions.size(); a++)
				{
					if (DEF_ACTION.bOnce	= 
				}
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

		if (ExecuteOnce)	bEnabled=FALSE;
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

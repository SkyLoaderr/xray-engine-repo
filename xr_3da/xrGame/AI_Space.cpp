// AI_Space.cpp: implementation of the CAI_Space class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "AI_Space.h"
#include "gameobject.h"
#include "..\fstaticrender.h"

// for a* search
#include "ai_a_star.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAI_Space::CAI_Space	()
{
	m_nodes_ptr	= NULL;
	vfs			= NULL;
	sh_debug	= 0;

	Device.seqDevCreate.Add		(this);
	Device.seqDevDestroy.Add	(this);
	OnDeviceCreate				();
}

CAI_Space::~CAI_Space	()
{
	// for a* search
	vfUnloadSearch();
	//

	Device.seqDevCreate.Remove	(this);
	Device.seqDevDestroy.Remove	(this);
	Device.Shader.Delete		(sh_debug);
	OnDeviceDestroy				();

	_FREE	(m_nodes_ptr);
	_DELETE	(vfs);
}

void CAI_Space::OnDeviceCreate()
{
	REQ_CREATE	();
	sh_debug	= Device.Shader.Create	("debug\\ai_nodes","$null");
}
void CAI_Space::OnDeviceDestroy()
{
	REQ_DESTROY	();
	Device.Shader.Delete	(sh_debug);
}

void CAI_Space::Load(LPCSTR name)
{
	FILE_NAME	fName;
	strconcat	(fName,name,"level.ai");
	if (!Engine.FS.Exist(fName))	return;

	vfs = new	CVirtualFileStream(fName);

	// m_header & data
	vfs->Read	(&m_header,sizeof(m_header));
	R_ASSERT	(m_header.version == XRAI_CURRENT_VERSION);
	m_nodes		= (BYTE*) vfs->Pointer();

	// dispatch table
	m_nodes_ptr	= (NodeCompressed**)malloc(m_header.count*sizeof(void*));
	for (DWORD I=0; I<m_header.count; I++)
	{
		m_nodes_ptr[I]	= (NodeCompressed*)vfs->Pointer();

		NodeCompressed	C;
		vfs->Read		(&C,sizeof(C));

		DWORD			L = C.link_count;
		vfs->Advance	(L*sizeof(NodeLink));
	}

	// special query tables
	q_mark.assign		(m_header.count,0);
	q_mark_bit.assign	(m_header.count,false);

	// for a* search
	vfLoadSearch();
	//
}

void CAI_Space::Render()
{
	if (0==vfs)			return;
	if (0==sh_debug)	return;
	
	CGameObject*	O	= (CGameObject*)Level().CurrentEntity();
	Fvector	POSITION	= O->Position();
	POSITION.y += 0.5f;

	// display
	Fvector P			= POSITION;
	pApp->pFont->Out	(0.f,0.5f,"%f,%f,%f",VPUSH(P));

	NodePosition		Local;
	PackPosition		(Local,P);

	DWORD ID			= O->AI_NodeID;

	pApp->pFont->Out(0.f,0.55f,"%3d,%4d,%3d -> %d",
		int(Local.x),int(Local.y),int(Local.z),DWORD(ID));

	svector<DWORD,128>	linked;
	{
		NodeCompressed*	N	=	m_nodes_ptr[ID];
		DWORD	count		=	DWORD(N->link_count);
		BYTE* pData			=	(BYTE*)N;
		pData				+=	sizeof(NodeCompressed);
		NodeLink* it		=	(NodeLink*)pData;
		NodeLink* end		=	it+count;
		for(; it!=end; it++)
			linked.push_back(UnpackLink(*it));
	}

	// render
	float	sc		= m_header.size/16;
	float	st		= 0.98f*m_header.size/2;
	float	tt		= 0.01f;

	Fvector	DUP;	DUP.set(0,1,0);

	Device.Shader.set_Shader(sh_debug);
	pApp->pFont->Color		(D3DCOLOR_RGBA(255,255,255,255));

	for (DWORD Nid=0; Nid<m_header.count; Nid++)
	{
		NodeCompressed&	N	= *m_nodes_ptr[Nid];
		Fvector			P0,P1,PC;
		UnpackPosition	(P0,N.p0);	// p0
		UnpackPosition	(P1,N.p1);	// p1
		PC.add			(P0,P1);
		PC.mul			(0.5f);		// sphere center

		if (Device.vCameraPosition.distance_to(PC)>30) continue;

		float			sr	= P0.distance_to(P1)/2+m_header.size;
		if (::Render.ViewBase.testSphereDirty(PC,sr)) {
			DWORD	LL		= DWORD(N.light);
			DWORD	CC		= D3DCOLOR_XRGB(0,0,255);
			DWORD	CT		= D3DCOLOR_XRGB(LL,LL,LL);
			DWORD	CH		= D3DCOLOR_XRGB(0,128,0);

			BOOL	bHL		= FALSE;
			if (Nid==DWORD(ID))	{ bHL = TRUE; CT = D3DCOLOR_XRGB(0,255,0); }
			else {
				for (DWORD t=0; t<linked.size(); t++) {
					if (linked[t]==Nid) { bHL = TRUE; CT = CH; break; }
				}
			}

			// unpack plane
			Fplane PL; Fvector vNorm;
			pvDecompress(vNorm,N.plane);
			PL.build	(P0,vNorm);

			// create vertices
			Fvector		v,v1,v2,v3,v4;
			v.set(P0.x-st,P0.y,P0.z-st);	PL.intersectRayPoint(v,DUP,v1);	v1.direct(v1,PL.n,tt);	// minX,minZ
			v.set(P1.x+st,P0.y,P0.z-st);	PL.intersectRayPoint(v,DUP,v2);	v2.direct(v2,PL.n,tt);	// maxX,minZ
			v.set(P1.x+st,P1.y,P1.z+st);	PL.intersectRayPoint(v,DUP,v3);	v3.direct(v3,PL.n,tt);	// maxX,maxZ
			v.set(P0.x-st,P1.y,P1.z+st);	PL.intersectRayPoint(v,DUP,v4);	v4.direct(v4,PL.n,tt);	// minX,maxZ

			// render quad
			Device.Primitive.dbg_DrawTRI(precalc_identity,v3,v2,v1,CT);
			Device.Primitive.dbg_DrawTRI(precalc_identity,v1,v4,v3,CT);

			// render center
			Device.Primitive.dbg_DrawAABB(PC,sc,sc,sc,CC);

			// render id
			if (bHL) {
				Fvector		T;
				Fvector4	S;
				T.set		(PC); T.y+=0.3f;
				Device.mFullTransform.transform	(S,T);
				pApp->pFont->Size	(0.05f/sqrtf(fabsf(S.w)));
				pApp->pFont->Out	(S.x,-S.y,"~%d",Nid);
			}
		}
	}
}

int	CAI_Space::q_LoadSearch(const Fvector& pos)
{
	if (0==vfs)	return	0;

	NodePosition	P;
	PackPosition	(P,pos);
	short min_dist	= 32767;
	int selected	= -1;
	for (DWORD I=0; I<m_header.count; I++)
	{
		NodeCompressed& N = *m_nodes_ptr[I];

		if (u_InsideNode(N,P)) 
		{
			int dist = int(P.y)-int(N.p0.y);
			if (dist>=0) {
				if (dist<min_dist) {
					min_dist = dist;
					selected = I;
				}
			}
		}
	}
	return selected;
}

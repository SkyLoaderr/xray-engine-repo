// TestPhysics.cpp: implementation of the CTestPhysics class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestPhysics.h"
#include "..\fstaticrender.h"
#include "..\physics\bkphysix.hpp"

#pragma comment(lib,"c:\\temp\\bk\\bkbase.lib")
#pragma comment(lib,"c:\\temp\\bk\\bkphysix.lib")

static bkPhysicEnvironment	ENV;

static class myTriList : public bkRBShapeTriList {
	vector<bkVector3*>		tris;
public:
	virtual bkU16 GetTris	(bkVector3 **&ppVerts, const bkVector3 &p, const bkVector3 &s, const bkMatrix3 &r)
	{
		RAPID::Model* M = Level().ObjectSpace.GetStaticModel();
		Fmatrix		  X;	X.identity();

		X.i.set(r.x11,r.x12,r.x13); 
		X.j.set(r.x21,r.x22,r.x23); 
		X.k.set(r.x31,r.x32,r.x33); 

		XRC.BBoxMode	(0);
		XRC.BBoxCollide	(precalc_identity,M,  X, *(Fvector*)&p, *(Fvector*)&s);

		tris.clear		();
		tris.reserve	(XRC.BBoxContact.size() * 3);
		for (int i=0; i<XRC.BBoxContact.size(); i++)
		{
			int tri_id = XRC.BBoxContact[i].id;
			RAPID::tri* T = M->GetTris()+tri_id;
			tris.push_back((bkVector3*) T->verts[0]);
			tris.push_back((bkVector3*) T->verts[1]);
			tris.push_back((bkVector3*) T->verts[2]);
		}
		ppVerts			= tris.begin();

		return tris.size();
	}
}		TRIS;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CTestPhysics::CTestPhysics(	Fvector& pos)
{
	V = ::Render.Models.Create	("ball");
	C = new bkRBController;

	bkRBShapeData l_data;
	l_data.type		= BKRB_BSPHERE;
	l_data.center	= bkVector3	(0, 0, 0); // положение относительно центра масс
	l_data.radius	= .5f;
	C->SetMassAndShape(100.f, l_data);

	// Задаем положение тела в пространстве
	mXForm.translate	(pos);
	C->SetBodyPos		(*((const bkMatrix4*)&mXForm));

	// 
	Device.seqFrame.Add	(this);
	Device.seqRender.Add(this);
}

CTestPhysics::~CTestPhysics()
{
	Device.seqFrame.Remove		(this);
	Device.seqRender.Remove		(this);
	::Render.Models.Delete		(V);
	_DELETE (C);
}

void CTestPhysics::OnFrame()
{
	ENV.Update					(Device.fTimeDelta);
}
void CTestPhysics::OnRender()
{
	C->GetBodyPos				(*((bkMatrix4*)&mXForm));
	::Render.set_Transform		(&mXForm);
	::Render.add_leafs_Dynamic	(V);
}
void CTestPhysics::OnEvent(	EVENT E, DWORD P1, DWORD P2)
{
	
}

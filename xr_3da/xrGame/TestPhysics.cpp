// TestPhysics.cpp: implementation of the CTestPhysics class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestPhysics.h"
#include "..\fstaticrender.h"
#include "..\physics\bkphysix.hpp"

#pragma comment(lib,"c:\\temp\\bk\\bkbase.lib")
#pragma comment(lib,"c:\\temp\\bk\\bkphysix.lib")

bkPhysicEnvironment	ENV;

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

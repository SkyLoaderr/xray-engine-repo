// CDemoRecord.cpp: implementation of the CDemoRecord class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "x_ray.h"
#include "xr_smallfont.h"
#include "fDemoRecord.h"
#include "xr_ioconsole.h"
#include "xr_creator.h"
#include "xr_input.h"

CDemoRecord * xrDemoRecord = 0;

#define		g_fSpeed		5.0f
#define		g_fAngularSpeed 1.0f

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDemoRecord::CDemoRecord(const char *name,float life_time):CEffector(cefDemo,life_time)
{
	_unlink(name);
	hFile	= _open(name,O_WRONLY|O_CREAT|O_BINARY, S_IREAD|S_IWRITE);
	if (hFile>0) {
		iCapture();	// capture input
		m_Camera.invert(Device.mView);

		// parse yaw
		Fvector& dir	= m_Camera.k;
		Fvector DYaw;	DYaw.set(dir.x,0.f,dir.z); DYaw.normalize_safe();
		if (DYaw.x>=0)	m_HPB.x = -acosf(DYaw.z);
		else			m_HPB.x = -(2*PI-acosf(DYaw.z));

		// parse pitch
		dir.normalize_safe	();
		m_HPB.y			= asinf(dir.y);
		m_HPB.z			= 0;

		m_Position.set	(m_Camera.c);

		m_vVelocity.set	(0,0,0);
		m_vAngularVelocity.set(0,0,0);
		iCount = 0;
		
		m_vT.set(0,0,0);
		m_vR.set(0,0,0);
	} else {
		pCreator->Cameras.RemoveEffector(cefDemo);
	}
}

CDemoRecord::~CDemoRecord()
{
	if (hFile>0) {
		iRelease();	// release input
		_close(hFile);
	}
}

void CDemoRecord::Process(Fvector &P, Fvector &D, Fvector &N)
{
	if (hFile<=0)	return;

	if ((timeGetTime()/500)%2==0) {
		pApp->pFont->Size(0.02f);
		pApp->pFont->Color(D3DCOLOR_RGBA(255,0,0,255));
		pApp->pFont->Out(0,-.05f,"~%s","RECORDING");
		pApp->pFont->Out(0,+.05f,"~Key frames count: %d",iCount);
	}


	m_vVelocity.lerp		(m_vVelocity,m_vT,0.1f);
	m_vAngularVelocity.lerp	(m_vAngularVelocity,m_vR,0.1f);

	float acc = 1.f, acc_angle = 1.f;
	if (Console.iGetKeyState(DIK_LSHIFT)){ acc=.025f; acc_angle=.025f;}
	else if (Console.iGetKeyState(DIK_LALT)) acc=4.0;
    m_vT.mul				(m_vVelocity, Device.fTimeDelta * g_fSpeed * acc);
    m_vR.mul				(m_vAngularVelocity, Device.fTimeDelta * g_fAngularSpeed * acc_angle);

	m_HPB.x -= m_vR.y;
	m_HPB.y -= m_vR.x;
	m_HPB.z += m_vR.z;

	// move
    Fvector vmove;

    vmove.set				( m_Camera.k );
    vmove.normalize_safe	();
    vmove.mul				( m_vT.z );
    m_Position.add			( vmove );

    vmove.set				( m_Camera.i );
    vmove.normalize_safe	();
    vmove.mul				( m_vT.x );
    m_Position.add			( vmove );
	
    vmove.set				( m_Camera.j );
    vmove.normalize_safe	();
    vmove.mul				( m_vT.y );
    m_Position.add			( vmove );

	m_Camera.setHPB			(m_HPB.x,m_HPB.y,m_HPB.z);
    m_Camera.translate_over	(m_Position);

	// update view
	Device.mView.invert		(m_Camera);

	// update camera
	N.set(m_Camera.j);
	D.set(m_Camera.k);
	P.set(m_Camera.c);

	fLifeTime-=Device.fTimeDelta;

	m_vT.set(0,0,0);
    m_vR.set(0,0,0);
}

void CDemoRecord::OnKeyboardPress	(int dik)
{
	if (dik == DIK_SPACE)	RecordKey();
	if (dik == DIK_RETURN)	pCreator->Cameras.RemoveEffector(cefDemo);
}

void CDemoRecord::OnKeyboardHold	(int dik)
{
	switch(dik){
	case DIK_A:
	case DIK_NUMPAD1:
	case DIK_LEFT:		m_vT.x -= 1.0f; break; // Slide Left
	case DIK_D:
	case DIK_NUMPAD3:
	case DIK_RIGHT:		m_vT.x += 1.0f; break; // Slide Right
	case DIK_S:			m_vT.y -= 1.0f; break; // Slide Down
	case DIK_W:			m_vT.y += 1.0f; break; // Slide Up
//	case DIK_S:			m_vT.z -= 1.0f; break; // Move Forward
//	case DIK_W:			m_vT.z += 1.0f; break; // Move Backward
	// rotate	
	case DIK_NUMPAD2:	m_vR.x -= 1.0f; break; // Pitch Down
	case DIK_NUMPAD8:	m_vR.x += 1.0f; break; // Pitch Up
	case DIK_E:	
	case DIK_NUMPAD6:	m_vR.y += 1.0f; break; // Turn Left
	case DIK_Q:	
	case DIK_NUMPAD4:	m_vR.y -= 1.0f; break; // Turn Right
	case DIK_NUMPAD9:	m_vR.z -= 2.0f; break; // Turn Right
	case DIK_NUMPAD7:	m_vR.z += 2.0f; break; // Turn Right
	}
}

void CDemoRecord::OnMouseMove		(int dx, int dy)
{
	float scale			= .75f;//psMouseSens;
	if (dx||dy){
		m_vR.y			+= float(dx)*scale;
		m_vR.x			+= ((psMouseInvert)?-1:1)*float(dy)*scale*MouseHWScale;
	}
}

void CDemoRecord::OnMouseHold		(int btn)
{
	switch (btn){
	case 0:			m_vT.z += 1.0f; break; // Move Backward
	case 1:			m_vT.z -= 1.0f; break; // Move Forward
	}
}

void CDemoRecord::RecordKey			()
{
	Fmatrix g_matView;
 
	g_matView.invert(m_Camera);
	_write(
		hFile,
		&g_matView,
		sizeof(Fmatrix)
	);
	iCount++;
}

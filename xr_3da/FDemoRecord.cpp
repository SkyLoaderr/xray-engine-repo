// CDemoRecord.cpp: implementation of the CDemoRecord class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "x_ray.h"
#include "xr_smallfont.h"
#include "fDemoRecord.h"
#include "xr_ioconsole.h"
#include "xr_creator.h"

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
		((Fmatrix *)&g_matPosition)->invert(Device.mView);
		g_vecVelocity = g_vecAngularVelocity = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		iCount = 0;
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
		pApp->pFont->Color(D3DCOLOR_RGBA(255,0,0,255));
		pApp->pFont->Out(0,-.05f,"~%s","RECORDING");
		pApp->pFont->Out(0,+.05f,"~Key frames count: %d",iCount);
	}

    D3DXVECTOR3 vecT(0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 vecR(0.0f, 0.0f, 0.0f);

    if(Console.iGetKeyState(DIK_A) || Console.iGetKeyState(DIK_NUMPAD1) || Console.iGetKeyState(DIK_LEFT))  vecT.x -= 1.0f; // Slide Left
    if(Console.iGetKeyState(DIK_D) || Console.iGetKeyState(DIK_NUMPAD3) || Console.iGetKeyState(DIK_RIGHT)) vecT.x += 1.0f; // Slide Right
    if(Console.iGetKeyState(DIK_DOWN))											vecT.y -= 1.0f; // Slide Down
    if(Console.iGetKeyState(DIK_UP))											vecT.y += 1.0f; // Slide Up
    if(Console.iGetKeyState(DIK_S))												vecT.z -= 2.0f; // Move Forward
    if(Console.iGetKeyState(DIK_W))												vecT.z += 2.0f; // Move Backward
    if(Console.iGetKeyState(DIK_NUMPAD2))										vecR.x -= 1.0f; // Pitch Down
    if(Console.iGetKeyState(DIK_NUMPAD8))										vecR.x += 1.0f; // Pitch Up
    if(Console.iGetKeyState(DIK_E) || Console.iGetKeyState(DIK_NUMPAD6))		vecR.y += 1.0f; // Turn Left
    if(Console.iGetKeyState(DIK_Q) || Console.iGetKeyState(DIK_NUMPAD4))		vecR.y -= 1.0f; // Turn Right
    if(Console.iGetKeyState(DIK_NUMPAD9))										vecR.z -= 2.0f; // Roll CW
    if(Console.iGetKeyState(DIK_NUMPAD7))										vecR.z += 2.0f; // Roll CCW

	g_vecVelocity = g_vecVelocity * 0.9f + vecT * 0.1f;
	g_vecAngularVelocity = g_vecAngularVelocity * 0.9f + vecR * 0.1f;

	D3DXMATRIX		matT, matR;
	D3DXQUATERNION	qR;

	float acc;
	if (Console.iGetKeyState(DIK_LSHIFT)) acc=.025f; else acc=1.f;
    vecT = g_vecVelocity * Device.fTimeDelta * g_fSpeed * acc;
    vecR = g_vecAngularVelocity * Device.fTimeDelta * g_fAngularSpeed * acc;

	D3DXMatrixTranslation(&matT, vecT.x, vecT.y, vecT.z);
	D3DXMatrixMultiply(&g_matPosition, &matT, &g_matPosition);

	D3DXQuaternionRotationYawPitchRoll(&qR, vecR.y, vecR.x, vecR.z);
	D3DXMatrixRotationQuaternion(&matR, &qR);
	D3DXMatrixMultiply(&g_matPosition, &matR, &g_matPosition);

//	if(g_matPosition.m31 < 1.0f)	g_matPosition.m31 = 1.0f;
//	CopyMemory(&Device.mView,&g_matPosition,sizeof(D3DMATRIX));
	D3DXMatrixInverse((D3DXMATRIX *)(&Device.mView), NULL, &g_matPosition);

	N.set( g_matPosition._21, g_matPosition._22, g_matPosition._23 );
	D.set( g_matPosition._31, g_matPosition._32, g_matPosition._33 );
	P.set( g_matPosition._41, g_matPosition._42, g_matPosition._43 );

	/*
	a = (-m41 -b*yr-c*zr)/xr
	(-m41 -b*yr-c*zr)*xu/xr+b*yu+c*zu = -m42
	- m41*xu/xr - b*yr*xu/xr - c*zr*xu/xr + b*yu + c*zu + m42 = 0
	- m41*xu/xr - c*zr*xu/xr + c*zu + m42 = b*yr*xu/xr - b*yu
	b = (- m41*xu/xr - c*zr*xu/xr + c*zu + m42)/(yr*xu/xr-yu)
	a = (-m41 + (m41*xu/xr + c*zr*xu/xr - c*zu - m42)/(yr*xu/xr-yu) -c*zr)/xr
	a = (-(c*(xr*(yu*zr-zu)-xu*zr*(yr-1))+m41*(xr*yu+xu*(1-yr))-m42*xr)/(xr*(xr*yu-xu*yr)))

	result:
	c = (-(m41*(xr*(xu*yv-xv*yu)+xu*xv*(yr-1))-xr*(m42*(xr*yv-xv)+m43*(xu*yr-xr*yu)))/(xr*xr*(yu*zv-yv*zu)-xr*(xu*(yr*zv-yv*zr)+xv*(yu*zr-zu))+xu*xv*zr*(yr-1)))
	b = -(c*(xr*zu-xu*zr)+m42*xr-m41*xu)/(xr*yu-xu*yr)
	a = (-m41 -b*yr-c*zr)/xr

	*/

	fLifeTime-=Device.fTimeDelta;
}

void CDemoRecord::OnKeyboardPress	(int dik)
{
	if (dik == DIK_SPACE)	RecordKey();
	if (dik == DIK_RETURN)	pCreator->Cameras.RemoveEffector(cefDemo);
}

void CDemoRecord::RecordKey			()
{
	D3DXMATRIX g_matView;
 
	_write(
		hFile,
		D3DXMatrixInverse((D3DXMATRIX *)(&g_matView), NULL, &g_matPosition),
		sizeof(Fmatrix)
	);
	iCount++;
	/*
    // Start building the matrix. The first three rows contains the basis
    // vectors used to rotate the view to point at the lookat point
    mat._11 = vRight.x;    mat._12 = vUp.x;    mat._13 = vView.x;
    mat._21 = vRight.y;    mat._22 = vUp.y;    mat._23 = vView.y;
    mat._31 = vRight.z;    mat._32 = vUp.z;    mat._33 = vView.z;
    // Do the translation values (rotations are still about the eyepoint)
    mat._41 = - DotProduct( vFrom, vRight );
    mat._42 = - DotProduct( vFrom, vUp );
    mat._43 = - DotProduct( vFrom, vView );
	*/
}

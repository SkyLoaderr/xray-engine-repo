#include "stdafx.h"
#pragma hdrstop

#include "UI_Main.h"
#include "UI_Camera.h"
#include "Scene.h"

CUI_Camera::CUI_Camera(){
	Reset();

	m_Aspect = 1.f;
	m_FOV = deg2rad( 60.f );
	m_Znear = 0.2f;
	m_Zfar = 1500.f;
    m_HPB.set(0,0,0);
    m_Position.set(0,0,0);
    m_CamMat.identity();

	m_FlySpeed = 5.f;
    m_FlyAltitude = 1.8f;
    
    m_bMoving=false;
}

CUI_Camera::~CUI_Camera(){
}

void CUI_Camera::Reset(){
	m_HPB.set(0,0,0);
    m_Position.set(0,5,-20);
    BuildCamera();
}

void CUI_Camera::Set(float h, float p, float b, float x, float y, float z){
	m_HPB.set(h,p,b);
    m_Position.set(x,y,z);
    BuildCamera();
}

void CUI_Camera::Set(const Fvector& hpb, const Fvector& pos){
	m_HPB.set(hpb);
    m_Position.set(pos);
    BuildCamera();
}

void CUI_Camera::BuildCamera(){
	if (m_HPB.x>PI_MUL_2)  m_HPB.x-=PI_MUL_2;	if (m_HPB.x<-PI_MUL_2) m_HPB.x+=PI_MUL_2;
	if (m_HPB.y>PI_MUL_2)  m_HPB.y-=PI_MUL_2;	if (m_HPB.y<-PI_MUL_2) m_HPB.y+=PI_MUL_2;
	if (m_HPB.z>PI_MUL_2)  m_HPB.z-=PI_MUL_2;	if (m_HPB.z<-PI_MUL_2) m_HPB.z+=PI_MUL_2;
	m_CamMat.setHPB(m_HPB.x,m_HPB.y,m_HPB.z);
    m_CamMat.translate_over(m_Position);
    if (UI) UI->OutCameraPos();
}

void CUI_Camera::SetViewport(float _near, float _far, float _fov){
    float fov=deg2rad(_fov);
    if (UI){
    	if (m_Znear!=_near)	{m_Znear=_near; UI->Resize();}
	    if (m_Zfar!=_far)	{m_Zfar=_far; UI->Resize();}
    	if (m_FOV!=fov)		{m_FOV=fov; UI->Resize();}
    }
}

void CUI_Camera::SetSensitivity(float sm, float sr){
    m_SM = 0.2f*(sm*sm);
    m_SR = 0.02f*(sr*sr);
}

static const Fvector dir={0.f,-1.f,0.f};

void CUI_Camera::Update(float dt){
	if (UI&&m_bMoving){
    	bool bLeftDn = m_Shift.Contains(ssLeft);
    	bool bRightDn = m_Shift.Contains(ssRight);
		if ((m_Style==csFreeFly)&&(bLeftDn||bRightDn)&&!(bLeftDn&&bRightDn)){
			Fvector vmove;
	        vmove.set( m_CamMat.k );
			vmove.mul( m_FlySpeed*dt );
    		if (bLeftDn) 		m_Position.add( vmove );
    		else if (bRightDn) 	m_Position.sub( vmove );

            if (m_Shift.Contains(ssCtrl)){
            	Fvector pos;
            	if (UI->PickGround(pos,m_Position,dir,-1))
                	m_Position.y = pos.y+m_FlyAltitude;
            }
            
        	UI->RedrawScene();
	    }
        BuildCamera();
    }
}

void CUI_Camera::Pan(float dx, float dz){
    Fvector vmove;
    vmove.set( m_CamMat.k );  vmove.y = 0;
    vmove.normalize_safe();
    vmove.mul( dz*-m_SM );
    m_Position.add( vmove );

    vmove.set( m_CamMat.i );  vmove.y = 0;
    vmove.normalize_safe();
    vmove.mul( dx*m_SM );
    m_Position.add( vmove );

    BuildCamera();
}

void CUI_Camera::Scale(float dy){
    Fvector vmove;
    vmove.set( 0.f, dy, 0.f );
    vmove.y *= -m_SM;
    m_Position.add( vmove );

    BuildCamera();
}

void CUI_Camera::Rotate(float dx, float dy){
	m_HPB.x-=m_SR*dx;
	m_HPB.y-=m_SR*dy*m_Aspect;

    BuildCamera();
}

bool CUI_Camera::MoveStart(TShiftState Shift){
	if (UI&&!m_bMoving&&Shift.Contains(ssShift)){
		m_CenterPos.x = GetSystemMetrics(SM_CXSCREEN)/2;
	    m_CenterPos.y = GetSystemMetrics(SM_CYSCREEN)/2;
		GetCursorPos(&m_SavePos);
	    ShowCursor( FALSE );
	    SetCursorPos( m_CenterPos.x, m_CenterPos.y );
	    SetCapture( UI->GetD3DWindow() );
		m_bMoving	= true;
		m_Shift = Shift;
        Scene->lock();
        return true;
    }
	m_Shift = Shift;
    return false;
}

bool CUI_Camera::MoveEnd(TShiftState Shift){
	if (!Shift.Contains(ssLeft)||!Shift.Contains(ssShift)){
	    SetCursorPos( m_SavePos.x, m_SavePos.y );
    	ShowCursor( TRUE );
	    ReleaseCapture();
        Scene->unlock();
		m_bMoving	= false;
		m_Shift = Shift;
        return true;
    }
	m_Shift = Shift;
    return false;
}

bool CUI_Camera::Process(TShiftState Shift){
    if (UI&&m_bMoving){
        m_Shift = Shift;
// camera move
		POINT d;
		GetCursorPos(&d);
        d.x -= m_CenterPos.x;
        d.y -= m_CenterPos.y;
        if( d.x || d.y ){
            SetCursorPos(m_CenterPos.x,m_CenterPos.y);
            switch (m_Style){
            case csPlaneMove:
                if (Shift.Contains(ssLeft) && Shift.Contains(ssRight)) Rotate (d.x,d.y);
                else if (Shift.Contains(ssLeft))			Pan		(d.x,d.y);
                		else if(Shift.Contains(ssRight))   	Scale  	(d.y);
            break;
            case csFreeFly:
                if (Shift.Contains(ssLeft)||Shift.Contains(ssRight)) Rotate (d.x,d.y);
//                if (Shift.Contains(ssLeft)) Rotate (d.x,d.y);
//                else if (Shift.Contains(ssRight)) Scale(d.y);
            break;
            }
		    UI->RedrawScene();
        }
        return true;
	}
    return false;
}

bool CUI_Camera::KeyDown(WORD Key, TShiftState Shift){
    if (m_bMoving){
    	switch (Key){
        case VK_CONTROL: m_Shift<<ssCtrl; break;
        default: return false;
        }
	    return true;
    }
	return false;
}

bool CUI_Camera::KeyUp(WORD Key, TShiftState Shift){
    if (m_bMoving){
    	switch (Key){
        case VK_SHIFT: m_Shift>>ssShift; MoveEnd(m_Shift); break;
        case VK_CONTROL: m_Shift>>ssCtrl; break;
        default: return false;
        }
	    return true;
    }
	return false;
}

void CUI_Camera::MouseRayFromPoint( Fvector& start, Fvector& direction, const Fvector2& point ){
    if (!UI) return;

	int halfwidth  = UI->GetRealWidth()*0.5f;
	int halfheight = UI->GetRealHeight()*0.5f;

    if (!halfwidth||!halfheight) return;

    Fvector2 point2;
    point2.set(point.x-halfwidth, halfheight-point.y);

	start.set( m_Position );

	float size_y = m_Znear * tan( m_FOV * 0.5f );
	float size_x = size_y / m_Aspect;

	float r_pt = point2.x * size_x / (float) halfwidth;
	float u_pt = point2.y * size_y / (float) halfheight;

	direction.mul( m_CamMat.k, m_Znear );
	direction.direct( direction, m_CamMat.j, u_pt );
	direction.direct( direction, m_CamMat.i, r_pt );
	direction.normalize();
}

void CUI_Camera::ZoomExtents(const Fbox& bb){
	Fvector C,D,P;
    float R,H;
    bb.getsphere(C,R);
	D.mul(m_CamMat.k,-1);
    H = 2*R*tanf(m_FOV/m_Aspect*0.5f);
    m_Position.direct(C,D,H);

	BuildCamera();
/*    
	eye_k - фокусное расстояние, eye_k=eye_width/2
	camera.alfa:=0;
     camera.beta:=-30*pi/180;
     camera.gama:=0;
     s:=(maxx-minx)*eye_k/eye_width*0.5*0.5;
     camera.posx:=(maxx+minx)/2;
     camera.posy:=maxy+s*tan(30*pi/180);
     camera.posz:=minz-s;
*/
}


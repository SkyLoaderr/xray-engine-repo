//----------------------------------------------------
// file: UI_Mouse.cpp
//----------------------------------------------------

#include "Pch.h"
#pragma hdrstop

#include "NetDeviceLog.h"
#include "UI_Main.h"
#include "MSC_Main.h"
#include "Scene.h"


//----------------------------------------------------

void XRayEditorUI::MouseRayFromPoint(
	IVector *start,
	IVector *direction,
	POINT *point )
{
	_ASSERTE( start );
	_ASSERTE( direction );
	_ASSERTE( point );

	int halfwidth = m_RenderWidth / 2;
	int halfheight = m_RenderHeight / 2;

	POINT point2;
	point2.x = point->x - halfwidth;
	point2.y = halfheight - point->y;

	start->set( m_Camera.c );

	float size_y = m_Znear * tanf( m_FOV * 0.5f );
	float size_x = size_y / m_Aspect;

	float r_pt = point2.x * size_x / (float) halfwidth;
	float u_pt = point2.y * size_y / (float) halfheight;

	direction->mul( m_Camera.k, m_Znear );
	direction->translate( m_Camera.j, u_pt );
	direction->translate( m_Camera.i, r_pt );
	direction->normalize();

	m_LastStart.set( (*start) );
	m_LastDir.set( (*direction) );
}

//----------------------------------------------------

bool XRayEditorUI::MouseBox(
	ICullPlane *planes, // four planes !
	POINT *point1, POINT *point2 )
{
	if( point1->x != point2->x &&
		point1->y != point2->y )
	{
		POINT p[4];
		IVector st1,st2,n1,n2;
		IVector planenormal;

		p[0].x = min( point1->x, point2->x );
		p[0].y = min( point1->y, point2->y );
		p[1].x = max( point1->x, point2->x );
		p[1].y = min( point1->y, point2->y );
		p[2].x = max( point1->x, point2->x );
		p[2].y = max( point1->y, point2->y );
		p[3].x = min( point1->x, point2->x );
		p[3].y = max( point1->y, point2->y );

		for( int i=0; i<4; i++){
			int i1 = (i+1)%4;
			MouseRayFromPoint( &st1, &n1, p+i );
			MouseRayFromPoint( &st2, &n2, p+i1 );
			planenormal.cross( n2, n1 );
			planenormal.safe_normalize();
			planes[i].set( planenormal, st1 );
		}

		return true;
	}

	return false;
}

//----------------------------------------------------

bool XRayEditorUI::PickGround(
	IVector *hitpoint,
	IVector *start,
	IVector *direction )
{
	_ASSERTE( start );
	_ASSERTE( direction );
	_ASSERTE( hitpoint );

	IVector normal;
	normal.set( 0, 1, 0 );

	float clcheck = direction->dot( normal );
	if( is_zero( clcheck ) )
		return false;

	float alpha = - start->dot(normal) / clcheck;
	if( alpha <= 0 )
		return false;

	hitpoint->x = start->x + direction->x * alpha;
	hitpoint->y = start->y + direction->y * alpha;
	hitpoint->z = start->z + direction->z * alpha;

	m_LastPick.set( (*hitpoint) );

	return true;
}

//----------------------------------------------------

void XRayEditorUI::MouseStart( bool m_Alt ){

	if( Scene.locked() ){
		NConsole.print( "Scene sharing violation..." );
		return;
	}

	if( !m_MouseCaptured ){
		if( (m_CurrentMouseCB = 
			MSC_Select( m_Target, m_Action )) )
		{

			if( m_CurrentMouseCB->HiddenMode() ){
				
				m_CurrentMouseCB->m_CenterCpH.x = GetSystemMetrics(SM_CXSCREEN)/2;
				m_CurrentMouseCB->m_CenterCpH.y = GetSystemMetrics(SM_CYSCREEN)/2;
				
				GetCursorPos( &m_CurrentMouseCB->m_StartCpH );
				
				m_CurrentMouseCB->m_DeltaCpH.x = 0;
				m_CurrentMouseCB->m_DeltaCpH.y = 0;
			}
			else{
				
				GetCursorPos( &m_CurrentMouseCB->m_CurrentCp );
				ScreenToClient( m_VisWindow, &m_CurrentMouseCB->m_CurrentCp );
				m_CurrentMouseCB->m_StartCp = m_CurrentMouseCB->m_CurrentCp;

				MouseRayFromPoint(
					&m_CurrentMouseCB->m_StartRStart,
					&m_CurrentMouseCB->m_StartRNorm,
					&m_CurrentMouseCB->m_StartCp );

				MouseRayFromPoint(
					&m_CurrentMouseCB->m_CurrentRStart,
					&m_CurrentMouseCB->m_CurrentRNorm,
					&m_CurrentMouseCB->m_CurrentCp );
			}

			m_CurrentMouseCB->m_Alternate = m_Alt;
			if( m_CurrentMouseCB->Start() ){
				
				if( m_CurrentMouseCB->HiddenMode() ){
					ShowCursor( FALSE );
					SetCursorPos(
						m_CurrentMouseCB->m_CenterCpH.x,
						m_CurrentMouseCB->m_CenterCpH.y );
				}
				
				SetCapture( m_VisWindow );
				m_MouseCaptured = true;
			}
		}
	}
}

void XRayEditorUI::MouseEnd(){
	if( m_MouseCaptured ){

		_ASSERTE( m_CurrentMouseCB );
		
		if( m_CurrentMouseCB->HiddenMode() ){
			GetCursorPos( &m_CurrentMouseCB->m_DeltaCpH );
			m_CurrentMouseCB->m_DeltaCpH.x -= m_CurrentMouseCB->m_CenterCpH.x;
			m_CurrentMouseCB->m_DeltaCpH.y -= m_CurrentMouseCB->m_CenterCpH.y;
		}
		else {
			GetCursorPos( &m_CurrentMouseCB->m_CurrentCp );
			ScreenToClient( m_VisWindow, &m_CurrentMouseCB->m_CurrentCp );

			MouseRayFromPoint(
				&m_CurrentMouseCB->m_CurrentRStart,
				&m_CurrentMouseCB->m_CurrentRNorm,
				&m_CurrentMouseCB->m_CurrentCp );
		}
		if( m_CurrentMouseCB->End() ){
			if( m_CurrentMouseCB->HiddenMode() ){
				SetCursorPos(
					m_CurrentMouseCB->m_StartCpH.x,
					m_CurrentMouseCB->m_StartCpH.y );
				ShowCursor( TRUE );
			}
			ReleaseCapture();
			m_MouseCaptured = false;
		}
	}
}

void XRayEditorUI::MouseProcess(){
	if( m_MouseCaptured ){

		_ASSERTE( m_CurrentMouseCB );

		if( m_CurrentMouseCB->HiddenMode() ){
			
			GetCursorPos( &m_CurrentMouseCB->m_DeltaCpH );
			m_CurrentMouseCB->m_DeltaCpH.x -= m_CurrentMouseCB->m_CenterCpH.x;
			m_CurrentMouseCB->m_DeltaCpH.y -= m_CurrentMouseCB->m_CenterCpH.y;
			if( m_CurrentMouseCB->m_DeltaCpH.x ||
				m_CurrentMouseCB->m_DeltaCpH.y ){
					SetCursorPos(
						m_CurrentMouseCB->m_CenterCpH.x,
						m_CurrentMouseCB->m_CenterCpH.y );
					m_CurrentMouseCB->Move();
			}

		}
		else{
			GetCursorPos( &m_CurrentMouseCB->m_CurrentCp );
			ScreenToClient( m_VisWindow, &m_CurrentMouseCB->m_CurrentCp );
			
			MouseRayFromPoint(
				&m_CurrentMouseCB->m_CurrentRStart,
				&m_CurrentMouseCB->m_CurrentRNorm,
				&m_CurrentMouseCB->m_CurrentCp );

			m_CurrentMouseCB->Move();
		}
	}
}

//----------------------------------------------------

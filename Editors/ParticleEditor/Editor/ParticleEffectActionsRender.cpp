//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleEffectActions.h"
#include "D3DUtils.h"
/*
		pVector p1, p2;		// Box vertices, Sphere center, Cylinder/Cone ends
		pVector u, v;		// Orthonormal basis vectors for Cylinder/Cone
		float radius1;		// Outer radius
		float radius2;		// Inner radius
		float radius1Sqr;	// Used for fast Within test of spheres,
		float radius2Sqr;	// and for mag. of u and v vectors for plane.
*/
void PS::CPEDef::Render()
{
	for (PAPI::PAVecIt it=m_ActionList.begin(); it!=m_ActionList.end(); it++)
    	(*it)->Render();
}

void PAPI::pDomain::Render(u32 clr)
{
    RCache.set_xform_world	(Fidentity);
	switch(type){
    case PDPoint: 	
		Device.SetShader	(Device.m_WireShader);
    	DU.DrawCross		(p1, 0.05f,0.05f,0.05f, 0.05f,0.05f,0.05f, clr);
    break;
	case PDLine: 	
		Device.SetShader	(Device.m_WireShader);
    	DU.DrawCross		(p1, 0.05f,0.05f,0.05f, 0.05f,0.05f,0.05f, clr);
    	DU.DrawCross		(p1+p2, 0.05f,0.05f,0.05f, 0.05f,0.05f,0.05f, clr);
    	DU.DrawLine 		(p1, p1+p2, clr);
    break;
    case PDTriangle: 	
		Device.SetShader	(Device.m_SelectionShader);
        DU.DrawFace			(p1, p1+u, p1+v, clr, true, false);
		Device.SetShader	(Device.m_WireShader);
        DU.DrawFace			(p1, p1+u, p1+v, clr, false, true);
    break;
	case PDPlane:{
		Device.SetShader	(Device.m_SelectionShader);
        Fvector2 sz			= {100.f,100.f};
        DU.DrawPlane		(p1,p2,radius1,sz,clr,true,true,false);
    }break;
	case PDBox: 	
		Device.SetShader	(Device.m_SelectionShader);
    	DU.DrawAABB			(p1, p2, clr, true, false);
		Device.SetShader	(Device.m_WireShader);
    	DU.DrawAABB			(p1, p2, clr, false, true);
    break;
	case PDSphere: 	
		Device.SetShader	(Device.m_SelectionShader);
    	DU.DrawSphere		(Fidentity, p1, radius2, clr, true, false);
    	DU.DrawSphere		(Fidentity, p1, radius1, clr, true, false);
		Device.SetShader	(Device.m_WireShader);
    	DU.DrawSphere		(Fidentity, p1, radius2, clr, false, true);
    	DU.DrawSphere		(Fidentity, p1, radius1, clr, false, true);
    break;
	case PDCylinder:{
    	pVector c,d;
        float h 			= p2.length	();
        c 					= (p1+p1+p2)/2.f;
        d 					= p2/h;
		Device.SetShader	(Device.m_SelectionShader);
		DU.DrawCylinder		(Fidentity, c, d, h, radius1, clr, true, false);
		Device.SetShader	(Device.m_WireShader);
		DU.DrawCylinder		(Fidentity, c, d, h, radius1, clr, false, true);
    }break;
	case PDCone:{ 	
    	pVector d;
        float h = p2.length	();
        d = p2/h;
		Device.SetShader	(Device.m_SelectionShader);
		DU.DrawCone			(Fidentity, p1, d, h, radius2, clr, true, false);
		DU.DrawCone			(Fidentity, p1, d, h, radius1, clr, true, false);
		Device.SetShader	(Device.m_WireShader);
		DU.DrawCone			(Fidentity, p1, d, h, radius2, clr, false, true);
		DU.DrawCone			(Fidentity, p1, d, h, radius1, clr, false, true);
    }break;
	case PDBlob: 	
		Device.SetShader	(Device.m_WireShader);
    	DU.DrawCross		(p1, 0.1f,0.1f,0.1f, 0.1f,0.1f,0.1f, clr);
    break;
	case PDDisc:{
		Device.SetShader	(Device.m_SelectionShader);
		DU.DrawCylinder		(Fidentity, p1, p2, EPS, radius2, clr, true, false);
		DU.DrawCylinder		(Fidentity, p1, p2, EPS, radius1, clr, true, false);
		Device.SetShader	(Device.m_WireShader);
		DU.DrawCylinder		(Fidentity, p1, p2, EPS, radius2, clr, false, true);
		DU.DrawCylinder		(Fidentity, p1, p2, EPS, radius1, clr, false, true);
    }break;
	case PDRectangle: 	
		Device.SetShader	(Device.m_SelectionShader);
        DU.DrawRectangle	(p1, u, v, clr, true, false);
		Device.SetShader	(Device.m_WireShader);
        DU.DrawRectangle	(p1, u, v, clr, false, true);
    break;
    }
}

void 	EPAAvoid::Render			()
{
}

void 	EPABounce::Render			()
{
}

void 	EPACallActionList::Render	()
{
}

void 	EPACopyVertexB::Render  	()
{
}

void 	EPADamping::Render		()
{
}

void 	EPAExplosion::Render		()
{
}

void 	EPAFollow::Render			()
{
}

void 	EPAGravitate::Render		()
{
}

void 	EPAGravity::Render		()
{
}

void 	EPAJet::Render			()
{
}

void 	EPAKillOld::Render		()
{
}

void 	EPAMatchVelocity::Render	()
{
}

void 	EPAMove::Render			()
{
}

void 	EPAOrbitLine::Render		()
{
}

void 	EPAOrbitPoint::Render		()
{
}

void 	EPARandomAccel::Render	()
{
}

void 	EPARandomDisplace::Render	()
{
}

void 	EPARandomVelocity::Render	()
{
}

void 	EPARestore::Render		()
{
}

void 	EPASink::Render			()
{
}

void 	EPASinkVelocity::Render	()
{
}

void 	EPASpeedLimit::Render		()
{
}

void 	EPASource::Render			()
{
	position.Render(0x60FFEBAA);
}

void 	EPATargetColor::Render	()
{
}

void 	EPATargetSize::Render		()
{
}

void 	EPATargetRotate::Render	()
{
}

void 	EPATargetVelocity::Render	()
{
}

void 	EPAVelocityD::Render		()
{
}

void 	EPAVortex::Render			()
{
}

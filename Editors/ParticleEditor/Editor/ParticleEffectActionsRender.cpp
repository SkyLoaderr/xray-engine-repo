//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleEffectActions.h"
#include "D3DUtils.h"

void PS::CPEDef::Render()
{
	for (EPAVecIt it=m_EActionList.begin(); it!=m_EActionList.end(); it++)
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
/*
void 	EPAAvoid::Render			()
{
	position.Render(0x6096FF96);
}

void 	EPABounce::Render			()
{
	position.Render(0x6096FEEC);
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
    RCache.set_xform_world	(Fidentity);
    Device.SetShader		(Device.m_WireShader);
    DU.DrawCross			(center, 0.05f,0.05f,0.05f, 0.05f,0.05f,0.05f, 0x600000ff);
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
    RCache.set_xform_world	(Fidentity);
    Device.SetShader		(Device.m_WireShader);
    DU.DrawCross			(p, 0.05f,0.05f,0.05f, 0.05f,0.05f,0.05f, 0x6000ff00);
    DU.DrawCross			(p+axis, 0.05f,0.05f,0.05f, 0.05f,0.05f,0.05f, 0x6000ff00);
    DU.DrawLine 			(p, p+axis, 0x6000ff00);
}

void 	EPAOrbitPoint::Render		()
{
    RCache.set_xform_world	(Fidentity);
    Device.SetShader		(Device.m_WireShader);
    DU.DrawCross			(center, 0.05f,0.05f,0.05f, 0.05f,0.05f,0.05f, 0x6000ff00);
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
	position.Render(0x60ff0000);
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
    RCache.set_xform_world	(Fidentity);
    Device.SetShader		(Device.m_WireShader);
    DU.DrawCross			(center, 0.05f,0.05f,0.05f, 0.05f,0.05f,0.05f, 0x6000ffff);
}
*/

//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ParticleEffectActions.h"
#include "D3DUtils.h"
#include "ui_particletools.h"

using namespace PAPI;
/*
void RenderDomain(pDomain d, u32 clr)
{
	u32 clr_s = subst_alpha	(clr,0x60);
	u32 clr_w = subst_alpha	(clr,0xff);
    RCache.set_xform_world	(Fidentity);
    
    Device.SetShader		(PTools->m_Flags.is(CParticleTools::flDrawSolid)?Device.m_WireShader:Device.m_SelectionShader);
    
	switch(d.type){
    case PDPoint: 	
    	DU.DrawCross		(d.p1, 0.05f,0.05f,0.05f, 0.05f,0.05f,0.05f, clr_w);
    break;
	case PDLine: 	
    	DU.DrawCross		(d.p1, 0.05f,0.05f,0.05f, 0.05f,0.05f,0.05f, clr_w);
    	DU.DrawCross		(d.p1+d.p2, 0.05f,0.05f,0.05f, 0.05f,0.05f,0.05f, clr_w);
    	DU.DrawLine 		(d.p1, d.p1+d.p2, clr_w);
    break;
    case PDTriangle: 	
        DU.DrawFace			(d.p1, d.p1+d.u, d.p1+d.v, clr_s, clr_w, true, true);
    break;
	case PDPlane:{
        Fvector2 sz			= {25.f,25.f};
        DU.DrawPlane		(d.p1,d.p2,sz,clr_s,clr_w,true,true,true);
    }break;
	case PDBox: 	
    	DU.DrawAABB			(d.p1, d.p2, clr_s, clr_w, true, true);
    break;
	case PDSphere: 	
    	DU.DrawSphere		(Fidentity, d.p1, d.radius2, clr_s, clr_w, true, true);
    	DU.DrawSphere		(Fidentity, d.p1, d.radius1, clr_s, clr_w, true, true);
    break;
	case PDCylinder:{
    	pVector C,D;
        float H 			= d.p2.length	();
        C 					= (d.p1+d.p1+d.p2)/2.f;
        D 					= d.p2/H;
		DU.DrawCylinder		(Fidentity, C, D, H, d.radius1, clr_s, clr_w, true, true);
    }break;
	case PDCone:{ 	
    	pVector D;
        float H 			= d.p2.length	();
        D 					= d.p2/H;
		DU.DrawCone			(Fidentity, d.p1, D, H, d.radius2, clr_s, clr_w, true, true);
		DU.DrawCone			(Fidentity, d.p1, D, H, d.radius1, clr_s, clr_w, true, true);
    }break;
	case PDBlob: 	
    	DU.DrawCross		(d.p1, 0.1f,0.1f,0.1f, 0.1f,0.1f,0.1f, clr_w);
    break;
	case PDDisc:{
		DU.DrawCylinder		(Fidentity, d.p1, d.p2, EPS, d.radius2, clr_s, clr_w, true, true);
		DU.DrawCylinder		(Fidentity, d.p1, d.p2, EPS, d.radius1, clr_s, clr_w, true, true);
    }break;
	case PDRectangle: 	
        DU.DrawRectangle	(d.p1, d.u, d.v, clr_s, clr_w, true, true);
    break;
    }
}

void RenderAction(ParticleAction* pa)
{
	switch (pa->type){                     
    case PAPI::PAAvoidID:			RenderDomain(dynamic_cast<PAAvoid*>			(pa)->position, 0x6096FF96);	break;
    case PAPI::PABounceID:    		RenderDomain(dynamic_cast<PABounce*>		(pa)->position, 0x6096FEEC);	break;
    case PAPI::PACallActionListID:  dynamic_cast<PACallActionList*>	(pa);	break;
    case PAPI::PACopyVertexBID:    	dynamic_cast<PACopyVertexB*>   	(pa);	break;
    case PAPI::PADampingID:    		dynamic_cast<PADamping*>	  	(pa);	break;
    case PAPI::PAExplosionID:    	dynamic_cast<PAExplosion*>		(pa);	break;
    case PAPI::PAFollowID:    		dynamic_cast<PAFollow*>			(pa);	break;
    case PAPI::PAGravitateID:    	dynamic_cast<PAGravitate*>		(pa);	break;
    case PAPI::PAGravityID:    		dynamic_cast<PAGravity*>	   	(pa);	break;
    case PAPI::PAJetID:{
    	PAJet* PA 					= dynamic_cast<PAJet*>		   	(pa);	
        RCache.set_xform_world		(Fidentity);
        Device.SetShader			(Device.m_WireShader);
        DU.DrawCross				(PA->center, 0.05f,0.05f,0.05f, 0.05f,0.05f,0.05f, 0x600000ff);
	}break;
    case PAPI::PAKillOldID:    		dynamic_cast<PAKillOld*>	   	(pa);	break;
    case PAPI::PAMatchVelocityID:   dynamic_cast<PAMatchVelocity*>	(pa);	break;
    case PAPI::PAMoveID:    		dynamic_cast<PAMove*>		   	(pa);	break;
    case PAPI::PAOrbitLineID:{
    	PAOrbitLine* PA				= dynamic_cast<PAOrbitLine*>  	(pa);	
        RCache.set_xform_world		(Fidentity);
        Device.SetShader			(Device.m_WireShader);
        DU.DrawCross				(PA->p, 0.05f,0.05f,0.05f, 0.05f,0.05f,0.05f, 0x6000ff00);
        DU.DrawCross				(PA->p+PA->axis, 0.05f,0.05f,0.05f, 0.05f,0.05f,0.05f, 0x6000ff00);
        DU.DrawLine 				(PA->p, PA->p+PA->axis, 0x6000ff00);
    }break;
    case PAPI::PAOrbitPointID:{
    	PAOrbitPoint* PA			= dynamic_cast<PAOrbitPoint*>	(pa);	
        RCache.set_xform_world		(Fidentity);
        Device.SetShader			(Device.m_WireShader);
        DU.DrawCross				(PA->center, 0.05f,0.05f,0.05f, 0.05f,0.05f,0.05f, 0x6000ff00);
    }break;
    case PAPI::PARandomAccelID:    	dynamic_cast<PARandomAccel*>   	(pa);	break;
    case PAPI::PARandomDisplaceID:  dynamic_cast<PARandomDisplace*>	(pa);	break;
    case PAPI::PARandomVelocityID:  dynamic_cast<PARandomVelocity*>	(pa);	break;
    case PAPI::PARestoreID:    		dynamic_cast<PARestore*>		(pa);	break;
    case PAPI::PASinkID:    		RenderDomain(dynamic_cast<PASink*>		   	(pa)->position, 0x60ff0000);	break;
    case PAPI::PASinkVelocityID:    dynamic_cast<PASinkVelocity*>   (pa);	break;
    case PAPI::PASourceID:    		RenderDomain(dynamic_cast<PASource*>		(pa)->position, 0x60FFEBAA);	break;
    case PAPI::PASpeedLimitID:    	dynamic_cast<PASpeedLimit*>		(pa);	break;
    case PAPI::PATargetColorID:    	dynamic_cast<PATargetColor*> 	(pa);	break;
    case PAPI::PATargetSizeID:    	dynamic_cast<PATargetSize*>		(pa);	break;
    case PAPI::PATargetRotateID:    dynamic_cast<PATargetRotate*> 	(pa);	break;
    case PAPI::PATargetRotateDID:   dynamic_cast<PATargetRotate*> 	(pa);	break;
    case PAPI::PATargetVelocityID:	dynamic_cast<PATargetVelocity*>	(pa);	break;
    case PAPI::PATargetVelocityDID: dynamic_cast<PATargetVelocity*>	(pa);	break;
    case PAPI::PAVortexID:{
    	PAVortex* PA				= dynamic_cast<PAVortex*>		(pa);	
        RCache.set_xform_world		(Fidentity);
        Device.SetShader			(Device.m_WireShader);
        DU.DrawCross				(PA->center, 0.05f,0.05f,0.05f, 0.05f,0.05f,0.05f, 0x6000ffff);
    }break;
    default: NODEFAULT;
    }
}
*/
void PS::CPEDef::Render(const Fmatrix& parent)
{
	Fmatrix trans; trans.translate(parent.c);
	for (EPAVecIt it=m_EActionList.begin(); it!=m_EActionList.end(); it++)
        if ((*it)->flags.is(EParticleAction::flDraw|EParticleAction::flEnabled)){
        	PBool* ar = (*it)->_bool_safe("Allow Rotate");
        	(*it)->Render((ar&&ar->val)?parent:trans);
        }
}


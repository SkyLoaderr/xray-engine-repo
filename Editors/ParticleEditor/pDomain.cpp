#include "stdafx.h"
#pragma hdrstop

#include "d3dutils.h"
#include "ui_main.h"
#include "PropertiesListHelper.h"
#include "ParticleEffectActions.h"

using namespace PAPI;

PDomain::PDomain(EType et, BOOL ra, u32 color, PDomainEnum t,	
										float inA0,	float inA1,	float inA2,	
										float inA3,	float inA4,	float inA5,
										float inA6,	float inA7,	float inA8	)
{
	flags.set(flRenderable,ra);
	e_type = et;
	type = t;
    clr	 = color;
	f[0] = inA0;
	f[1] = inA1;
	f[2] = inA2;    
	f[3] = inA3;
	f[4] = inA4;
	f[5] = inA5;
	f[6] = inA6;
	f[7] = inA7;
	f[8] = inA8;
}

PDomain::PDomain(const PDomain& inDomain)
{
	e_type 	= inDomain.e_type;
    flags	= inDomain.flags;
	type 	= inDomain.type;
    clr	 	= inDomain.clr;
	f[0]	= inDomain.f[0];
	f[1]	= inDomain.f[1];
	f[2]	= inDomain.f[2];
	f[3]	= inDomain.f[3];
	f[4]	= inDomain.f[4];
	f[5]	= inDomain.f[5];
	f[6]	= inDomain.f[6];
	f[7]	= inDomain.f[7];
	f[8]	= inDomain.f[8];
}

PDomain::~PDomain()
{
}

void PDomain::Load(IReader& F)
{
	type		= F.r_u32();
	F.r_fvector3(v[0]);
	F.r_fvector3(v[1]);
	F.r_fvector3(v[2]);
}

void PDomain::Save(IWriter& F)
{
	F.w_u32		(type);
	F.w_fvector3(v[0]);
	F.w_fvector3(v[1]);
	F.w_fvector3(v[2]);
}

/*
void	PDomain::set(PAPI::PDomainEnum t, 		float inA0, float inA1, float inA2,
                                                float inA3, float inA4, float inA5, 
                                                float inA6, float inA7, float inA8	)
{
	type 	= t;
	f[0]	= inA0;
	f[1]	= inA1;
	f[2]	= inA2;
	f[3]	= inA3;
	f[4]	= inA4;
	f[5]	= inA5;
	f[6]	= inA6;
	f[7]	= inA7;
	f[8]	= inA8;
}
*/

//--------------------------------------------------------------------
void 	PDomain::Render		(u32 clr, const Fmatrix& parent)
{
	if (!flags.is(flRenderable)) return;
	u32 clr_s = subst_alpha	(clr,0x60);
	u32 clr_w = subst_alpha	(clr,0xff);
    RCache.set_xform_world	(parent);
	switch(type){
    case PDPoint: 	
		Device.SetShader	(Device.m_WireShader);
    	DU.DrawCross		(v[0], 0.05f,0.05f,0.05f, 0.05f,0.05f,0.05f, clr_w);
    break;
	case PDLine: 	
		Device.SetShader	(Device.m_WireShader);
    	DU.DrawCross		(v[0], 0.05f,0.05f,0.05f, 0.05f,0.05f,0.05f, clr_w);
	  	DU.DrawCross		(v[1], 0.05f,0.05f,0.05f, 0.05f,0.05f,0.05f, clr_w);
    	DU.DrawLine 		(v[0], v[1], clr_w);
    break;
    case PDTriangle: 	
		Device.SetShader	(Device.m_SelectionShader);
        DU.DrawFace			(v[0], v[1], v[2], clr_s, clr_w, true, true);
    break;
	case PDPlane:{
		Device.SetShader	(Device.m_SelectionShader);
        Fvector2 sz			= {100.f,100.f};
        DU.DrawPlane		(v[0],v[1],sz,clr_s,clr_w,true,true,true);
    }break;
	case PDBox: 	
		Device.SetShader	(Device.m_SelectionShader);
    	DU.DrawAABB			(v[0], v[1], clr_s, clr_w, true, true);
    break;
	case PDSphere: 	
    	DU.DrawSphere		(parent, v[0], f[4], clr_s, clr_w, true, true);
    	DU.DrawSphere		(parent, v[0], f[3], clr_s, clr_w, true, true);
    break;                                      
	case PDCylinder:{
    	Fvector c,d;
        float h 			= d.sub(v[1],v[0]).magnitude();
        c.add 				(v[0],v[1]).div(2.f);
        if (!fis_zero(h)){
        	d.div			(h);
			DU.DrawCylinder	(parent, c, d, h, f[6], clr_s, clr_w, true, true);
			DU.DrawCylinder	(parent, c, d, h, f[7], clr_s, clr_w, true, true);
        }
    }break;
	case PDCone:{ 	
    	Fvector d;
        float h 			= d.sub(v[1],v[0]).magnitude();
        if (!fis_zero(h)){
            d.div			(h);
            DU.DrawCone		(parent, v[0], d, h, f[6], clr_s, clr_w, true, true);
            DU.DrawCone		(parent, v[0], d, h, f[7], clr_s, clr_w, true, true);
        }
    }break;
	case PDBlob: 	
		Device.SetShader	(Device.m_WireShader);
    	DU.DrawCross		(v[0], f[3],f[3],f[3], f[3],f[3],f[3], clr);
    break;
	case PDDisc:
        DU.DrawCylinder		(parent, v[0], v[1], 0.f, f[6], clr_s, clr_w, true, true);
        DU.DrawCylinder		(parent, v[0], v[1], 0.f, f[7], clr_s, clr_w, true, true);
    break;
	case PDRectangle: 	
        DU.DrawRectangle	(v[0], v[1], v[2], clr_s, clr_w, true, true);
    break;
   }
}

xr_token					domain_token	[ ]={
	{ "Point",				PDPoint		},
	{ "Line",			   	PDLine		},
	{ "Triangle",			PDTriangle	},
	{ "Plane",			   	PDPlane		},
	{ "Box",				PDBox		},
	{ "Sphere",				PDSphere	},
	{ "Cylinder",			PDCylinder	},
	{ "Cone",		   		PDCone		},
	{ "Blob",		   		PDBlob		},
	{ "Disc",		  		PDDisc		},
	{ "Rectangle",	   		PDRectangle	},
	{ 0,					0		   	}
};

void __fastcall PDomain::OnTypeChange(PropValue* sender)
{
	UI->Command				(COMMAND_UPDATE_PROPERTIES);
}

void 	PDomain::FillProp	(PropItemVec& items, LPCSTR pref)
{
    PropValue* V;
    V=PHelper.CreateToken<u32>(items,pref,(u32*)&type,domain_token);
    V->OnChangeEvent		= OnTypeChange;
    
    switch(e_type){
    case vNum:
        switch(type){
            case PDPoint:
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Center"), 		&v[0],flt_min,flt_max,0.001f,3);
                break;
            case PDLine:
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Point 1"), 	&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Point 2"), 	&v[1],flt_min,flt_max,0.001f,3);
                break;
            case PDTriangle:
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Vertex 1"), 	&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Vertex 2"), 	&v[1],flt_min,flt_max,0.001f,3);
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Vertex 3"), 	&v[2],flt_min,flt_max,0.001f,3);
                break;
            case PDPlane:
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Origin"),	 	&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Normal"), 	 	&v[1],flt_min,flt_max,0.001f,3);
                break;
            case PDBox:
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Min"),		 	&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Max"), 	 		&v[1],flt_min,flt_max,0.001f,3);
                break;
            case PDSphere:
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Center"),	 	&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Inner"),	&f[3],0,flt_max,0.001f,3);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Outer"),	&f[4],0,flt_max,0.001f,3);
                break;
            case PDCylinder:
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Point 1"),	 	&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Point 2"),	 	&v[1],flt_min,flt_max,0.001f,3);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Inner"),	&f[6],0,flt_max,0.001f,3);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Outer"),	&f[7],0,flt_max,0.001f,3);
                break;
            case PDCone:
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Apex"),	 		&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"End Point"),	&v[1],flt_min,flt_max,0.001f,3);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Inner"),	&f[6],0,flt_max,0.001f,3);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Outer"),	&f[7],0,flt_max,0.001f,3);
                break;
            case PDBlob:
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Center"),	 	&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Outer"),	&f[3],0,flt_max,0.001f,3);
                break;
            case PDDisc:
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Center"),	 	&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Normal"),	 	&v[1],flt_min,flt_max,0.001f,3);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Inner"),	&f[6],0,flt_max,0.001f,3);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Outer"),	&f[7],0,flt_max,0.001f,3);
                break;
            case PDRectangle:
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Origin"),	 	&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Basis U"),	 	&v[1],flt_min,flt_max,0.001f,3);
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Basis V"),	 	&v[2],flt_min,flt_max,0.001f,3);
                break;
            default:
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Translate"),	&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Rotate"),	 	&v[1],flt_min,flt_max,0.001f,3);
                PHelper.CreateVector	(items,FHelper.PrepareKey(pref,"Scale"),	 	&v[2],flt_min,flt_max,0.001f,3);
                break;
        }
    break;
    case vAngle:
        switch(type){
            case PDPoint:
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Center"), 		&v[0],flt_min,flt_max,0.001f,3);
                break;
            case PDLine:
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Point 1"), 		&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Point 2"), 		&v[1],flt_min,flt_max,0.001f,3);
                break;
            case PDTriangle:
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Vertex 1"), 	&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Vertex 2"), 	&v[1],flt_min,flt_max,0.001f,3);
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Vertex 3"), 	&v[2],flt_min,flt_max,0.001f,3);
                break;
            case PDPlane:
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Origin"),	 	&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Normal"), 	 	&v[1],flt_min,flt_max,0.001f,3);
                break;
            case PDBox:
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Min"),		 	&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Max"), 	 		&v[1],flt_min,flt_max,0.001f,3);
                break;
            case PDSphere:
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Center"),	 	&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Inner"),	&f[3],0,flt_max,0.001f,3);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Outer"),	&f[4],0,flt_max,0.001f,3);
                break;
            case PDCylinder:
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Point 1"),	 	&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Point 2"),	 	&v[1],flt_min,flt_max,0.001f,3);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Inner"),	&f[6],0,flt_max,0.001f,3);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Outer"),	&f[7],0,flt_max,0.001f,3);
                break;
            case PDCone:
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Apex"),	 		&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"End Point"),	&v[1],flt_min,flt_max,0.001f,3);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Inner"),	&f[6],0,flt_max,0.001f,3);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Outer"),	&f[7],0,flt_max,0.001f,3);
                break;
            case PDBlob:
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Center"),	 	&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Outer"),	&f[3],0,flt_max,0.001f,3);
                break;
            case PDDisc:
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Center"),	 	&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Normal"),	 	&v[1],flt_min,flt_max,0.001f,3);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Inner"),	&f[6],0,flt_max,0.001f,3);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Outer"),	&f[7],0,flt_max,0.001f,3);
                break;
            case PDRectangle:
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Origin"),	 	&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Basis U"),	 	&v[1],flt_min,flt_max,0.001f,3);
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Basis V"),	 	&v[2],flt_min,flt_max,0.001f,3);
                break;
            default:
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Translate"),	&v[0],flt_min,flt_max,0.001f,3);
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Rotate"),	 	&v[1],flt_min,flt_max,0.001f,3);
                PHelper.CreateAngle3	(items,FHelper.PrepareKey(pref,"Scale"),	 	&v[2],flt_min,flt_max,0.001f,3);
                break;
        }
    break;
    case vColor:
        switch(type){
            case PDPoint:
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Center"), 		&v[0]);
                break;
            case PDLine:
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Point 1"), 		&v[0]);
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Point 2"), 		&v[1]);
                break;
            case PDTriangle:
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Vertex 1"), 	&v[0]);
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Vertex 2"), 	&v[1]);
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Vertex 3"), 	&v[2]);
                break;
            case PDPlane:
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Origin"),	 	&v[0]);
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Normal"), 	 	&v[1]);
                break;
            case PDBox:
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Min"),		 	&v[0]);
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Max"), 	 		&v[1]);
                break;
            case PDSphere:
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Center"),	 	&v[0]);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Inner"),	&f[3]);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Outer"),	&f[4]);
                break;
            case PDCylinder:
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Point 1"),	 	&v[0]);
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Point 2"),	 	&v[1]);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Inner"),	&f[6]);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Outer"),	&f[7]);
                break;
            case PDCone:
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Apex"),	 		&v[0]);
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"End Point"),	&v[1]);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Inner"),	&f[6]);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Outer"),	&f[7]);
                break;
            case PDBlob:
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Center"),	 	&v[0]);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Outer"),	&f[3]);
                break;
            case PDDisc:
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Center"),	 	&v[0]);
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Normal"),	 	&v[1]);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Inner"),	&f[6]);
                PHelper.CreateFloat		(items,FHelper.PrepareKey(pref,"Radius Outer"),	&f[7]);
                break;
            case PDRectangle:
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Origin"),	 	&v[0]);
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Basis U"),	 	&v[1]);
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Basis V"),	 	&v[2]);
                break;
            default:
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Translate"),	&v[0]);
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Rotate"),	 	&v[1]);
                PHelper.CreateVColor	(items,FHelper.PrepareKey(pref,"Scale"),	 	&v[2]);
                break;
        }
    break;
    }
}

//--------------------------------------------------------------------
/*
void	PDomain::MoveXYZ(float x, float y, float z)
{
	switch(type)
	{
		case PDPoint:
			a0+=x;	a1+=y;	a2+=z;
			break;
		case PDLine:
			a0+=x;	a1+=y;	a2+=z;
			a3+=x;	a4+=y;	a5+=z;
			break;
		case PDTriangle:
			a0+=x;	a1+=y;	a2+=z;
			a3+=x;	a4+=y;	a5+=z;
			a6+=x;	a7+=y;	a8+=z;
			break;
		case PDPlane:
			a0+=x;	a1+=y;	a2+=z;
			break;
		case PDBox:
			a0+=x;	a1+=y;	a2+=z;
			a3+=x;	a4+=y;	a5+=z;
			break;
		case PDSphere:
			a0+=x;	a1+=y;	a2+=z;
			break;
		case PDCylinder:
			a0+=x;	a1+=y;	a2+=z;
			a3+=x;	a4+=y;	a5+=z;
			break;
		case PDCone:
			a0+=x;	a1+=y;	a2+=z;
			a3+=x;	a4+=y;	a5+=z;
			break;
		case PDBlob:
			a0+=x;	a1+=y;	a2+=z;
			break;
		case PDDisc:
			a0+=x;	a1+=y;	a2+=z;
			break;
	}
	
}

//----------------------------------------------------------
void	PDomain::RotateXYZ	(float x, float y, float z)
{
	MVertex	v1;
	MVertex v2;
	MVertex v3;
	MVertex	r1;
	MVertex r2;
	MVertex r3;
//	MVertex	tmpV;
	MMatrix	trans;
	
	MVertex	c;
	
	trans.setIdentity();
	trans.rotateX(x);
	trans.rotateZ(z);
	trans.rotateY(y);
//	float	xloc;
//	float	yloc;
//	float	zloc;
	switch(type)
	{
		case PDPoint:
			break;
		case PDLine:
			r1 = trans*(v1.set( a0, a1, a2 ));
			r2 = trans*(v2.set( a3, a4, a5 ));
			a0 = r1.X();	a1 = r1.Y();	a2 = r1.Z();
			break;
		case PDTriangle:
			c = GetCenter();
			r1 = trans*(v1.set( (float)a0-c.X(), (float)a1-c.Y(), (float)a2-c.Z() ));
			r2 = trans*(v2.set( (float)a3-c.X(), (float)a4-c.Y(), (float)a5-c.Z() ));
			r3 = trans*(v3.set( (float)a6-c.X(), (float)a7-c.Y(), (float)a8-c.Z() ));
			a0 = r1.X()+c.X();	a1 = r1.Y()+c.Y();   a2 = r1.Z()+c.Z();
			a3 = r2.X()+c.X();	a4 = r2.Y()+c.Y();   a5 = r2.Z()+c.Z();
			a6 = r3.X()+c.X();	a7 = r3.Y()+c.Y();   a8 = r3.Z()+c.Z();
			break;
		case PDPlane:
//			r2 = trans*(v2.set( a3, a4, a5 ));
//			r3 = trans*(v3.set( a6, a7, a8 ));
//			a3 = r2.X();	a4 = r2.Y();	a5 = r2.Z();
//			a6 = r3.X();	a7 = r3.Y();	a8 = r3.Z();
			
			c = GetCenter();
			v1.set(a0-c.X(), a1-c.Y(), a2-c.Z());
			v2.set((float)a0+((float)a3/2.0), (float)a1+((float)a4/2.0), (float)a2+((float)a5/2.0));
			v3.set((float)a0+((float)a6/2.0), (float)a1+((float)a7/2.0), (float)a2+((float)a8/2.0));
			
			r1 = trans*v1;
			r2 = trans*v2;
			r3 = trans*v3;
			
			a0 = r1.X()+c.X();	a1 = r1.Y()+c.Y();   a2 = r1.Z()+c.Z();
			
			
			break;
		case PDBox:
			r1 = trans*(v1.set( a0, a1, a2 ));
			r2 = trans*(v2.set( a3, a4, a5 ));
			a0 = r1.X();	a1 = r1.Y();	a2 = r1.Z();
			a3 = r2.X();	a4 = r2.Y();	a5 = r2.Z();
			break;
		case PDSphere:
			r1 = trans*(v1.set( a0, a1, a2 ));
			a0 = r1.X();	a1 = r1.Y();	a2 = r1.Z();
			break;
		case PDCylinder:
			r1 = trans*(v1.set( a0, a1, a2 ));
			r2 = trans*(v2.set( a3, a4, a5 ));
			a0 = r1.X();	a1 = r1.Y();	a2 = r1.Z();
			a3 = r2.X();	a4 = r2.Y();	a5 = r2.Z();
			break;
		case PDCone:
			r1 = trans*(v1.set( a0, a1, a2 ));
			r2 = trans*(v2.set( a3, a4, a5 ));
			a0 = r1.X();	a1 = r1.Y();	a2 = r1.Z();
			a3 = r2.X();	a4 = r2.Y();	a5 = r2.Z();
			break;
		case PDBlob:
			r1 = trans*(v1.set( a0, a1, a2 ));
			a0 = r1.X();	a1 = r1.Y();	a2 = r1.Z();
			break;
		case PDDisc:
			break;
	}
}
//----------------------------------------------------------
void	PDomain::ScaleXYZ	(float x, float y, float z)
{
	x = x;
	y = y;
	z = z;
	
	switch(type)
	{
		case PDPoint:
			break;
		case PDLine:
			break;
		case PDTriangle:
			break;
		case PDPlane:
			break;
		case PDBox:
			break;
		case PDSphere:
			break;
		case PDCylinder:
			break;
		case PDCone:
			break;
		case PDBlob:
			break;
		case PDDisc:
			break;
	}
}

//----------------------------------------------------------
MVertex&	PDomain::GetCenter()
{
	static MVertex center;
	MVertex	v1;
	MVertex	v2;
	MVertex	v3;
	MVertex	v4;
	
	switch(type)
	{
		case PDPoint:
			center.set(a0, a1, a2);
			break;
		case PDLine:
			break;
		case PDTriangle:
			center.set( ((float)a0+(float)a3+(float)a6)/3.0, 
						((float)a1+(float)a4+(float)a7)/3.0, 
						((float)a2+(float)a5+(float)a8)/3.0 );
			break;
		case PDPlane:
			v1.set(a0, a1, a2);
			v2.set(a0+(a3/2.0f), a1+(a4/2.0f), a2+(a5/2.0f));
			v3.set(a0+(a6/2.0f), a1+(a7/2.0f), a2+(a8/2.0f));
			v4.set(a0+(a6/2.0f)+(a3/2.0f), a1+(a7/2.0f)+(a4/2.0f), a2+(a8/2.0f)+(a5/2.0f));
			center.set(	(v1.X()+v2.X()+v3.X()+v4.X())/4.0,
						(v1.Y()+v2.Y()+v3.Y()+v4.Y())/4.0,
						(v1.Z()+v2.Z()+v3.Z()+v4.Z())/4.0 );
			break;
		case PDBox:
			break;
		case PDSphere:
			break;
		case PDCylinder:
			break;
		case PDCone:
			break;
		case PDBlob:
			break;
		case PDDisc:
			center.set(a0, a1, a2);
			break;
	}
	return center;
}
*/

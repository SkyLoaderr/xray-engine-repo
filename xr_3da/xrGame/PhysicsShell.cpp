#include "stdafx.h"
#include "physicsshell.h"
#include "PHDynamicData.h"
#include "Physics.h"

CPhysicsElement*			P_create_Element		()
{
	CPHElement* element=xr_new<CPHElement>	(ph_world->GetSpace());
	return element;
}

CPhysicsShell*				P_create_Shell			()
{
	CPhysicsShell* shell=xr_new<CPHShell>	();
	return shell;
}

CPhysicsJoint*				P_create_Joint			(CPhysicsJoint::enumType type ,CPhysicsElement* first,CPhysicsElement* second)
{
	CPhysicsJoint* joint=xr_new<CPHJoint>	( type , first, second);
	return joint;
}

void __stdcall PushOutCallback(bool& do_colide,dContact& c)
{
	dBodyID body1=dGeomGetBody(c.geom.g1);
	dBodyID body2=dGeomGetBody(c.geom.g2);
	if(!(body1&&body2)) 
		return;
	//do_colide=false;
	//return;
	if(c.geom.depth>0.005f)
	{
		/*
		dxGeomUserData* usr_data_1=NULL;
		dxGeomUserData* usr_data_2=NULL;
		if(dGeomGetClass(c.geom.g1)==dGeomTransformClass){
			const dGeomID geom=dGeomTransformGetGeom(contacts[i].geom.g1);
			usr_data_1 = dGeomGetUserData(geom);
		}
		else
			usr_data_1 = dGeomGetUserData(c.geom.g1);

		if(dGeomGetClass(c.geom.g2)==dGeomTransformClass){
			const dGeomID geom=dGeomTransformGetGeom(c.geom.g2);
			usr_data_2 = dGeomGetUserData(geom);
		}
		else
			usr_data_2 = dGeomGetUserData(c.geom.g2);
		
		c.surface.soft_erp=2500.f;
		c.surface.soft_cfm=0.000002f;
		if(usr_data_1&&usr_data_2)
			if(usr_data_1->object_callback==PushOutCallback&&usr_data_1->object_callback==PushOutCallback)
			*/
		const dReal erp=0.01f;
		const dReal cfm=0.001f;

		//c.surface.soft_erp=5000.f;
		//c.surface.soft_cfm=0.0001f;

		c.surface.soft_erp=(1.f-erp)/cfm /world_damping;
		c.surface.soft_cfm=erp/cfm/fixed_step /world_spring;
		
	}

	const dReal* pos1=dGeomGetPosition(c.geom.g1);
	const dReal* pos2=dGeomGetPosition(c.geom.g2);


	if(pos1[0]==pos2[0]&&pos1[1]==pos2[1]&&pos1[2]==pos2[2])
	
	{
		do_colide=false;
		const dReal force=1.f;
		
		{	dMass m;
			dBodyGetMass(body1,&m);
			dBodyAddForce(body1,-c.geom.normal[0]*m.mass*force,-c.geom.normal[1]*m.mass*force,-c.geom.normal[2]*m.mass*force);

	
			dBodyGetMass(body2,&m);
			dBodyAddForce(body2,c.geom.normal[0]*m.mass*force,c.geom.normal[1]*m.mass*force,c.geom.normal[2]*m.mass*force);
			do_colide=false;
		}
	
	
	}


//if()
}
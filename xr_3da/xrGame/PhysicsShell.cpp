#include "stdafx.h"
#include "physicsshell.h"
#include "PHDynamicData.h"
#include "Physics.h"

CPhysicsElement*			P_create_Element		(){
//return ph_world->AddElement();
CPHElement* element=new CPHElement(ph_world->GetSpace());
return element;
}
CPhysicsShell*				P_create_Shell			(){
CPhysicsShell* shell=new CPHShell();
return shell;
}

#include "stdafx.h"
#pragma hdrstop

#include "ESceneSectorTools.h"
#include "ui_sectortools.h"
#include "ui_tools.h"
#include "FrameSector.h"
#include "SceneObject.h"
#include "sector.h"

/* TODO 1 -oAlexMX -cTODO: Create tools as AI Map */

void ESceneSectorTools::CreateControls()
{
//	inherited::CreateControls();
    AddControl		(xr_new<TUI_ControlSectorSelect>(estDefault,eaSelect,	this));
    AddControl		(xr_new<TUI_ControlSectorAdd>	(estDefault,eaAdd,		this));
	// frame
    pFrame 			= xr_new<TfraSector>((TComponent*)0);
}
//----------------------------------------------------
 
void ESceneSectorTools::RemoveControls()
{
}
//----------------------------------------------------

void ESceneSectorTools::OnObjectRemove(CCustomObject* O)
{
	inherited::OnObjectRemove(O);

    CSceneObject* obj = dynamic_cast<CSceneObject*>(O);
    if (obj&&!m_Objects.empty()){
	    EditMeshVec* meshes = obj->Meshes();
        for (EditMeshIt m_it= meshes->begin(); m_it!=meshes->end(); m_it++){
	        for(ObjectIt _F=m_Objects.begin();_F!=m_Objects.end();_F++){
    	        CSector* sector = dynamic_cast<CSector*>(*_F); VERIFY(sector);
                if (sector->DelMesh(obj, *m_it)) break;
            }
        }
	}
}
//----------------------------------------------------


#include "stdafx.h"
#pragma hdrstop

#include "ESceneObjectTools.h"
#include "ui_leveltools.h"
#include "ESceneObjectControls.h" 
#include "FrameObject.h"
#include "SceneObject.h"
#include "PropertiesList.h"
#include "PropertiesListHelper.h"
#include "library.h"
#include "Scene.h"
#include "ui_main.h"
#include "EditObject.h"
#include "EditMesh.h"

ESceneObjectTools::ESceneObjectTools():ESceneCustomOTools(OBJCLASS_SCENEOBJECT)
{
    m_AppendRandomMinScale.set		(1.f,1.f,1.f);
    m_AppendRandomMaxScale.set		(1.f,1.f,1.f);
    m_AppendRandomMinRotation.set	(0.f,0.f,0.f);
    m_AppendRandomMaxRotation.set	(0.f,0.f,0.f);
	m_Flags.zero	();
    m_Props			= 0;
}

void ESceneObjectTools::CreateControls()
{
	inherited::CreateDefaultControls(estDefault);
    AddControl		(xr_new<TUI_ControlObjectAdd >(estDefault,etaAdd,		this));
	// frame
    pFrame 			= xr_new<TfraObject>((TComponent*)0,this);
}
//----------------------------------------------------
 
void ESceneObjectTools::RemoveControls()
{
	inherited::RemoveControls();
}
//----------------------------------------------------

bool ESceneObjectTools::Validate()
{
	if (!inherited::Validate()) return false;
    // verify position & refs duplicate
    bool bRes = true;
    CSceneObject *A, *B;
    for (ObjectIt a_it=m_Objects.begin(); a_it!=m_Objects.end(); a_it++){
        A = (CSceneObject*)(*a_it);
	    for (ObjectIt b_it=m_Objects.begin(); b_it!=m_Objects.end(); b_it++){
            B = (CSceneObject*)(*b_it);
        	if (A==B) continue;
            if (A->RefCompare(B->GetReference())){
            	if (A->PPosition.similar(B->PPosition,EPS_L)){
                	bRes = false;
                    ELog.Msg(mtError,"Duplicate object position '%s'-'%s' with reference '%s'.",A->Name,B->Name,A->GetRefName());
                }
            }
        }
    }
    return bRes;
}
//----------------------------------------------------

void ESceneObjectTools::OnChangeAppendRandomFlags(PropValue* prop)
{
    m_Flags.set(flAppendRandomUpdateProps,TRUE);
}

void ESceneObjectTools::FillAppendRandomProperties(bool bUpdateOnly)
{
	if (!bUpdateOnly) m_Props	= TProperties::CreateModalForm("Random Append Properties",true);

    shared_str temp;
   	temp						= _ListToSequence(m_AppendRandomObjects).c_str();

    PropValue* V;           

    PropItemVec items; 
    V=PHelper().CreateFlag32	(items,"Scale",				&m_Flags, flAppendRandomScale);
    V->OnChangeEvent.bind		(this,&ESceneObjectTools::OnChangeAppendRandomFlags);
    if (m_Flags.is(flAppendRandomScale)){
        V=PHelper().CreateFlag32	(items,"Scale\\Proportional",&m_Flags, flAppendRandomScaleProportional);
        V->OnChangeEvent.bind		(this,&ESceneObjectTools::OnChangeAppendRandomFlags);
        if (m_Flags.is(flAppendRandomScaleProportional)){
            PHelper().CreateFloat	(items,"Scale\\Minimum",	&m_AppendRandomMinScale.x,0.001f,1000.f,0.001f,3);
            PHelper().CreateFloat	(items,"Scale\\Maximum",	&m_AppendRandomMaxScale.x,0.001f,1000.f,0.001f,3);
        }else{
            PHelper().CreateVector	(items,"Scale\\Minimum",	&m_AppendRandomMinScale,0.001f,1000.f,0.001f,3);
            PHelper().CreateVector	(items,"Scale\\Maximum",	&m_AppendRandomMaxScale,0.001f,1000.f,0.001f,3);
        }
    }

    V=PHelper().CreateFlag32	(items,"Rotate",&m_Flags, flAppendRandomRotation);
    V->OnChangeEvent.bind		(this,&ESceneObjectTools::OnChangeAppendRandomFlags);
    if (m_Flags.is(flAppendRandomRotation)){
        PHelper().CreateAngle3		(items,"Rotate\\Minimum",&m_AppendRandomMinRotation);
        PHelper().CreateAngle3		(items,"Rotate\\Maximum",&m_AppendRandomMaxRotation);
    }
    V=PHelper().CreateChoose	(items,"Objects",&temp,smObject,0,0,32);

    m_Props->AssignItems		(items);
    
    if (!bUpdateOnly){
        if (mrOk==m_Props->ShowPropertiesModal()){
            _SequenceToList		(m_AppendRandomObjects,*temp);
            Scene->UndoSave		();
        }
        TProperties::DestroyForm(m_Props);
    }
}
//----------------------------------------------------

void ESceneObjectTools::Clear		(bool bSpecific)
{
	inherited::Clear				(bSpecific);
    m_AppendRandomMinScale.set		(1.f,1.f,1.f);
    m_AppendRandomMaxScale.set		(1.f,1.f,1.f);
    m_AppendRandomMinRotation.set	(0.f,0.f,0.f);
    m_AppendRandomMaxRotation.set	(0.f,0.f,0.f);
    m_AppendRandomObjects.clear		();
    m_Flags.zero					();
}

bool ESceneObjectTools::GetBox		(Fbox& bb)
{
	bb.invalidate					();
    Fbox bbo;
    for (ObjectIt a_it=m_Objects.begin(); a_it!=m_Objects.end(); a_it++){
    	(*a_it)->GetBox				(bbo);
        bb.merge					(bbo);
    }
    return bb.is_valid();
}

void ESceneObjectTools::OnFrame		()
{
	inherited::OnFrame				();
	if (m_Flags.is(flAppendRandomUpdateProps)){
    	m_Flags.set					(flAppendRandomUpdateProps,FALSE);
        FillAppendRandomProperties	(true);
    }
}

CCustomObject* ESceneObjectTools::CreateObject(LPVOID data, LPCSTR name)
{
	CCustomObject* O	= xr_new<CSceneObject>(data,name);
    O->ParentTools		= this;
    return O;
}
//----------------------------------------------------

void ESceneObjectTools::HighlightTexture(LPCSTR tex_name)
{
	if (tex_name&&tex_name[0]){
        for (ObjectIt a_it=m_Objects.begin(); a_it!=m_Objects.end(); a_it++){
            CSceneObject* s_obj		= dynamic_cast<CSceneObject*>(*a_it);
            CEditableObject* e_obj	= s_obj->GetReference(); VERIFY(e_obj);
            SurfaceVec& s_vec		= e_obj->Surfaces();
            for (SurfaceIt it=s_vec.begin(); it!=s_vec.end(); it++){
                if (0==stricmp((*it)->_Texture(),tex_name)){
					Fvector 		verts[3];
					for (EditMeshIt mesh_it=e_obj->FirstMesh(); mesh_it!=e_obj->LastMesh(); mesh_it++){
                    	SurfFaces& 		surf_faces	= (*mesh_it)->GetSurfFaces();
                    	SurfFacesPairIt sf_it 		= surf_faces.find(*it);
                        if (sf_it!=surf_faces.end()){
                            IntVec& lst				= sf_it->second;
                            for (IntIt i_it=lst.begin(); i_it!=lst.end(); i_it++){
                                e_obj->GetFaceWorld	(s_obj->_Transform(),*mesh_it,*i_it,verts);
                                Tools->m_Errors.AppendFace(verts[0],verts[1],verts[2]);
                            }
                        }
                    }
                }
            }
        }
    }
}
//----------------------------------------------------


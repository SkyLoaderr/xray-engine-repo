#include "stdafx.h"
#pragma hdrstop

#include "ESceneObjectTools.h"
#include "ui_tools.h"
#include "ui_ObjectTools.h"
#include "FrameObject.h"
#include "SceneObject.h"
#include "PropertiesList.h"
#include "PropertiesListHelper.h"

// chunks
static const u16 OBJECT_TOOLS_VERSION  	= 0x0000;
//----------------------------------------------------
enum{
    CHUNK_VERSION			= 0x1001ul,
    CHUNK_APPEND_RANDOM		= 0x1002ul,
};

ESceneObjectTools::ESceneObjectTools():ESceneCustomOTools(OBJCLASS_SCENEOBJECT)
{
    m_AppendRandomMinScale.set		(1.f,1.f,1.f);
    m_AppendRandomMaxScale.set		(1.f,1.f,1.f);
    m_AppendRandomMinRotation.set	(0.f,0.f,0.f);
    m_AppendRandomMaxRotation.set	(0.f,0.f,0.f);
    m_AppendRandomObjects;
}

void ESceneObjectTools::CreateControls()
{
	inherited::CreateControls();
    AddControl		(xr_new<TUI_ControlObjectAdd >(estDefault,eaAdd,		this));
	// frame
    pFrame 			= xr_new<TfraObject>((TComponent*)0,this);
}
//----------------------------------------------------
 
void ESceneObjectTools::RemoveControls()
{
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

bool ESceneObjectTools::Load(IReader& F)
{
	u16 version 	= 0;
    if(F.r_chunk(CHUNK_VERSION,&version)){
        if( version!=OBJECT_TOOLS_VERSION ){
            ELog.DlgMsg( mtError, "%s tools: Unsupported version.",ClassDesc());
            return false;
        }
    }
	if (!inherited::Load(F)) return false;
    
    if (F.find_chunk(CHUNK_APPEND_RANDOM)){
        F.r_fvector3	(m_AppendRandomMinScale);
        F.r_fvector3	(m_AppendRandomMaxScale);
        F.r_fvector3	(m_AppendRandomMinRotation);
        F.r_fvector3	(m_AppendRandomMaxRotation);
        int cnt		= F.r_u32();
        if (cnt){
        	AnsiString buf;
            for (int i=0; i<cnt; i++){
                F.r_stringZ	(buf);
                CCustomObject* O = FindObjectByName(buf.c_str());
                if (O)	m_AppendRandomObjects.push_back(buf);
            }
        }
    };

    return true;
}
//----------------------------------------------------

void ESceneObjectTools::Save(IWriter& F)
{
	inherited::Save(F);

	F.w_chunk		(CHUNK_VERSION,(u16*)&OBJECT_TOOLS_VERSION,sizeof(OBJECT_TOOLS_VERSION));

    F.open_chunk	(CHUNK_APPEND_RANDOM);
    F.w_fvector3	(m_AppendRandomMinScale);
    F.w_fvector3	(m_AppendRandomMaxScale);
    F.w_fvector3	(m_AppendRandomMinRotation);
    F.w_fvector3	(m_AppendRandomMaxRotation);
    F.w_u32			(m_AppendRandomObjects.size());
    if (m_AppendRandomObjects.size()){
    	for (AStringIt it=m_AppendRandomObjects.begin(); it!=m_AppendRandomObjects.end(); it++)
            F.w_stringZ(it->c_str());
    }
    F.close_chunk	();
}
//----------------------------------------------------

void ESceneObjectTools::FillAppendRandomProperties()
{
    TProperties* m_Props = TProperties::CreateModalForm(true);

    AnsiString temp;
   	temp					= _ListToSequence(m_AppendRandomObjects);

    PropValue* V;           

    PropItemVec items;
    PHelper.CreateFlag32	(items,"Scale",	&m_Flags, flAppendRandomScale);
    PHelper.CreateVector	(items,"Scale\\Minimum",&m_AppendRandomMinScale,0.001f,1000.f,0.001f,3);
    PHelper.CreateVector	(items,"Scale\\Maximum",&m_AppendRandomMaxScale,0.001f,1000.f,0.001f,3);
    PHelper.CreateFlag32	(items,"Rotate",&m_Flags, flAppendRandomRotation);
    PHelper.CreateAngle3	(items,"Rotate\\Minimum",&m_AppendRandomMinRotation);
    PHelper.CreateAngle3	(items,"Rotate\\Maximum",&m_AppendRandomMaxRotation);
    V=PHelper.CreateALibObject(items,"Objects",&temp); V->Owner()->subitem = 32;

    m_Props->AssignItems	(items,true,"Random Append Properties");
    
    if (mrOk==m_Props->ShowPropertiesModal())
    	_SequenceToList		(m_AppendRandomObjects,temp.c_str());
        
    TProperties::DestroyForm(m_Props);
}
//----------------------------------------------------


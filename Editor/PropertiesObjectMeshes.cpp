#include "stdafx.h"
#pragma hdrstop

#include "PropertiesObject.h"
#include "SceneClassList.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "Texture.h"
#include "ui_main.h"
#include "scene.h"
#include "main.h"
//---------------------------------------------------------------------------
// meshes
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::tsMeshesShow(TObject *Sender)
{
	if (!m_EditObject) return;
// Set up meshes
    AnsiString name;

	tvMeshes->Items->Clear();

    EditMeshIt m_it = m_EditObject->m_Meshes.begin();

    // root (object name)
    TElTreeItem* root = tvMeshes->Items->Add(0,"Object");

    for(;m_it!=m_EditObject->m_Meshes.end();m_it++){
        name = (*m_it)->GetName();
        tvMeshes->Items->AddChildObject(root,name,(TObject*)(*m_it));
    }
    tvMeshes->FullExpand();
    tvMeshes->Sort(true);
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::tvMeshesItemFocused(TObject *Sender)
{
	TElTreeItem* Item = tvMeshes->Selected;
    if (Item->Level==0){
		lbVertices->Caption 	= m_EditObject->GetVertexCount();
		lbFaces->Caption 		= m_EditObject->GetFaceCount();
    }else{
        CEditableMesh* M		= (CEditableMesh*)Item->Data;
		lbVertices->Caption 	= M->GetVertexCount();
		lbFaces->Caption 		= M->GetFaceCount();
    }
}
//---------------------------------------------------------------------------


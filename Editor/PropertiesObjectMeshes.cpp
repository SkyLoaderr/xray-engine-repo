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
    TElTreeItem* root = tvMeshes->Items->Add(0,"Meshes");
    root->ParentStyle = false;
    root->Bold = true;

    for(;m_it!=m_EditObject->m_Meshes.end();m_it++){
        name = (*m_it)->GetName();
        tvMeshes->Items->AddChildObject(root,name,(TObject*)(*m_it));
    }
    tvMeshes->FullExpand();
    tvMeshes->Sort(true);
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::tvMeshesItemSelectedChange(
      TObject *Sender, TElTreeItem *Item)
{
	if (Item==tvMeshes->Selected) return;
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

//***************************************************************************************
void __fastcall TfrmPropertiesObject::ebAddMeshClick(TObject *Sender)
{
	THROW;
/*   	char buffer[MAX_PATH]="";
    FS.m_Meshes.Update(buffer);
    odMesh->InitialDir = buffer;
    if (odMesh->Execute()){
        AnsiString new_name = ExtractFileName(odMesh->FileName);
        bool bEq = false;
// find existing mesh
        for ( TElTreeItem* node = tvMeshes->Items->GetFirstNode(); node; node = node->GetNext())
            if (node->Text==new_name){ bEq = true; break; }
// if not found create and append to object
        if (!bEq){
            CEditMesh* _O = new CEditMesh(m_EditObject);
            strcpy(_O->GetName(),new_name.c_str());
            if (_O->Load(new_name.c_str())){
               	m_EditObject->m_Meshes.push_back(_O);
                OnModified(Sender);
                tsMeshesShow(Sender);
            }else{
                MessageDlg("Mesh can't load. See log file.", mtError, TMsgDlgButtons() << mbOK, 0);
                _DELETE(_O);
            }
        }
    }
*/
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebDeleteMeshClick(TObject *Sender)
{
	THROW;
/*    if (MessageDlg("Are you sure to delete mesh from list?", mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, 0)==mrYes){
//        TElTreeItem* node = tvMeshes->Selected;
        for (TElTreeItem* node = tvMeshes->GetNextSelected(0); node; node=tvMeshes->GetNextSelected(node))
            if (node && node->Data){
                CEditMesh* del_mesh = (CEditMesh*)node->Data;
                VERIFY(del_mesh);
                m_EditObject->RemoveMesh(del_mesh);
                OnModified(Sender);
                tsMeshesShow(Sender);
            }
    }
*/
}
//---------------------------------------------------------------------------

void __fastcall TfrmPropertiesObject::ebMergeMeshClick(TObject *Sender)
{
	THROW;
/*    CEditMesh* basic_mesh=0;
    if (MessageDlg("Are you sure to merge meshes?", mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, 0)==mrYes){
        for (TElTreeItem* node = tvMeshes->GetNextSelected(0); node; node=tvMeshes->GetNextSelected(node))
            if (node && node->Data){
                if (basic_mesh){
                    CEditMesh* add_mesh = (CEditMesh*)node->Data;
                    VERIFY(add_mesh);
                    m_EditObject->MergeMeshes(basic_mesh,add_mesh);
                    OnModified(Sender);
                    tsMeshesShow(Sender);
                }
            }
    }
*/
}
//---------------------------------------------------------------------------
void __fastcall TfrmPropertiesObject::ebEditMeshClick(TObject *Sender)
{
/*    if (ebEditMesh->Enabled){
        CEditMesh *mdef;
        TElList* l=new TElList();
        tvMeshes->AllSelected(l);
        if (l->Count){
            if (l->Count>1){
                MessageDlg("MultiSelect not supported. Please select one item and try again.", mtConfirmation, TMsgDlgButtons() << mbOK, 0);
                delete l;
                return;
            }
            TElTreeItem* node = (TElTreeItem*)l->Items[0];
            if (node && node->Data){
                mdef = (CEditMesh*)node->Data;
                if (frmPropertiesSubObject->Edit(mdef)==mrOk) OnModified(Sender);
            }
        }
        delete l;
    }
*/
}
//---------------------------------------------------------------------------


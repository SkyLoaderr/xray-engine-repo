//----------------------------------------------------
// file: texturizer.cpp
//----------------------------------------------------
#include "Pch.h"
#pragma hdrstop

#include "texturizer.h"
#include "ui_main.h"
#include "texture.h"
#include "scene.h"
#include "StaticMesh.h"
#include "SObject2.h"
#include "SelectTexturing.h"
#include "PropertiesSubObject.h"
#include "FileSystem.h"
//---------------------------------------------------------------------------
CTexturizer Texturizer;

CTexturizer::CTexturizer(){
    m_bActive = false;

    m_EditObject = 0;
    m_EditMesh   = 0;
    m_EditLayer  = 0;
}
//---------------------------------------------------------------------------

CTexturizer::~CTexturizer(){
}
//---------------------------------------------------------------------------

void CTexturizer::Reset()
{
    SetEditObject(0);
}
//---------------------------------------------------------------------------

void CTexturizer::SetEditObject(SObject2* obj){
    if (m_EditObject!=obj){
        m_EditObject=obj;
        m_EditMesh=0;
        m_EditLayer=0;
        UpdateSelection();
    }
}
//---------------------------------------------------------------------------

void CTexturizer::SetEditMesh(SObject2Mesh* m){
    if (m_EditMesh!=m){
        if (m&&m_EditObject&&m_EditObject->ContainsMesh(m)){
            m_EditMesh=m;
            m_EditLayer=0;
        }else{
            m_EditMesh=0;
            m_EditLayer=0;
        }
        UpdateSelection();
    }
}
//---------------------------------------------------------------------------

void CTexturizer::SetEditLayer(CTextureLayer* l){
    if (m_EditLayer!=l){
        if (l&&m_EditObject&&m_EditMesh&&m_EditMesh->m_Mesh->ContainsLayer(l)){
            m_EditLayer=l;
            UpdateSelection();
        }else{
            m_EditLayer=0;
        }
        UpdateSelection();
    }
}
//---------------------------------------------------------------------------

const char* CTexturizer::GetObjectName   (){
    return (m_EditObject)?m_EditObject->GetName():0;
}
//---------------------------------------------------------------------------

const char* CTexturizer::GetMeshName     (){
    return (m_EditMesh)?m_EditMesh->GetName():0;
}
//---------------------------------------------------------------------------

const char* CTexturizer::GetLayerName    (){
    return (m_EditLayer)?m_EditLayer->GetName():0;
}
//---------------------------------------------------------------------------

void CTexturizer::UpdateSelection(){
    fraSelectTexturing->UpdateSelection();
}
//---------------------------------------------------------------------------

void CTexturizer::AddNewLayer(){
    if (Valid()){
        char buffer[MAX_PATH]="";
        if (FS.getopenname(&FS.m_Pictures,buffer)){
            AnsiString tname = ExtractFileName(buffer);
            ETexture* m_Texture = new ETexture( tname.c_str() );
            if (m_Texture->valid()){
                Texturizer.SetEditLayer(m_EditMesh->m_Mesh->AddTextureLayer(m_Texture));
            }else{
                MessageDlg("Texture can't load!", mtError, TMsgDlgButtons() << mbOK, 0);
                SAFE_DELETE(m_Texture);
                return;
            }
            MessageDlg("Layer successfully created.", mtConfirmation, TMsgDlgButtons() << mbOK, 0);
        }else{
            if (MessageDlg("Texture can't selected. Create layer without texture.", mtConfirmation, TMsgDlgButtons() << mbYes << mbNo, 0)==mrYes){
                Texturizer.SetEditLayer(m_EditMesh->m_Mesh->AddTextureLayer(0));
                MessageDlg("Layer successfully created.", mtConfirmation, TMsgDlgButtons() << mbOK, 0);
            }
        }
    }
}
//---------------------------------------------------------------------------

void CTexturizer::RemoveEditLayer(){
    if (Valid()){
        m_EditMesh->m_Mesh->RemoveTextureLayer(&m_EditLayer);
        Texturizer.SetEditLayer(0);
    }
}
//---------------------------------------------------------------------------

void CTexturizer::Edit(){
    if (Valid())
        if (frmPropertiesSubObject->Edit(m_EditMesh->m_Mesh)==mrOk)
            Texturizer.SetEditLayer(0);
}
//---------------------------------------------------------------------------

void CTexturizer::Activate(){
    m_bActive = true;
    if (m_EditObject&&!Scene.ContainsObject(m_EditObject)) SetEditObject(0);
    else if (m_EditMesh&&!m_EditObject->ContainsMesh(m_EditMesh)) SetEditMesh(0);
    else if (m_EditLayer&&!m_EditMesh->m_Mesh->ContainsLayer(m_EditLayer)) SetEditLayer(0);
    UI.UpdateScene();
    Scene.UndoPrepare();
}
//---------------------------------------------------------------------------
void CTexturizer::Deactivate(){
    m_bActive = false;
    UI.UpdateScene();
    Scene.UndoPrepare();
}
//---------------------------------------------------------------------------

void CTexturizer::Fit(){
//    m_Gizmo.Fit();
}
//---------------------------------------------------------------------------

void CTexturizer::Render()
{
    if (m_bActive){
        if (m_EditObject&&m_EditMesh){
            m_EditObject->RenderEdge(UI.m_Identity,m_EditMesh);
            if (m_EditLayer) m_EditObject->RenderEdge(UI.m_Identity,m_EditMesh,m_EditLayer);
        }
        if (fraSelectTexturing->ebGizmo->Down)
            m_Gizmo.Render();
    }
}


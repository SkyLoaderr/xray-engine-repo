//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "ShaderTools.h"
#include "EditObject.h"
#include "bottombar.h"
#include "ui_main.h"
#include "PropertiesShader.h"
#include "library.h"
#include "ChoseForm.h"
#include "Blender.h"
#include "LeftBar.h"

//------------------------------------------------------------------------------
CShaderTools SHTools;
//------------------------------------------------------------------------------
CShaderTools::CShaderTools(){
	m_LibObject 		= 0;
	m_EditObject 		= 0;
    m_CurrentBlender 	= 0;
}

CShaderTools::~CShaderTools(){
}
//---------------------------------------------------------------------------

void CShaderTools::OnCreate(){
    Device.seqDevCreate.Add(this);
    Device.seqDevDestroy.Add(this);
	CBlender::CreatePalette(m_TemplatePalette);
    fraLeftBar->InitPalette(m_TemplatePalette);
}

void CShaderTools::OnDestroy(){
    Device.seqDevCreate.Remove(this);
    Device.seqDevDestroy.Remove(this);
	// free palette
	for (TemplateIt it=m_TemplatePalette.begin(); it!=m_TemplatePalette.end(); it++)
    	_DELETE(*it);
    m_TemplatePalette.clear();
    // free constants, matrices, blenders
	// Matrices
	for (MatrixPairIt m=m_Matrices.begin(); m!=m_Matrices.end(); m++)
	{
		R_ASSERT	(0==m->second->dwReference);
		free		(m->first);
		delete		m->second;
	}
	m_Matrices.clear	();

	// Constants
	for (ConstantPairIt c=m_Constants.begin(); c!=m_Constants.end(); c++)
	{
		R_ASSERT	(0==c->second->dwReference);
		free		(c->first);
		delete		c->second;
	}
	m_Constants.clear	();

	// Matrices
	for (BlenderPairIt b=m_Blenders.begin(); b!=m_Blenders.end(); b++)
	{
		free		(b->first);
		delete		b->second;
	}
	m_Blenders.clear	();

    // hide properties window
	TfrmShaderProperties::HideProperties();
    m_Modified = FALSE;
}

bool CShaderTools::IfModified(){
    if (m_Modified && UI->GetEditFileName()[0]){
        int mr = ELog.DlgMsg(mtConfirmation, "The object has been modified. Do you want to save your changes?");
        switch(mr){
        case mrYes: if (!UI->Command(COMMAND_SAVE)) return false; else m_Modified = false; break;
        case mrNo: m_Modified = false; break;
        case mrCancel: return false;
        }
    }
    return true;
}

void CShaderTools::Render(){
	if (m_EditObject)
    	m_EditObject->RenderSingle(precalc_identity);
}

void CShaderTools::Update(){
	if (m_EditObject)
    	m_EditObject->RTL_Update(Device.fTimeDelta);
}

void CShaderTools::ZoomObject(){
	if (m_EditObject){
        Device.m_Camera.ZoomExtents(m_EditObject->GetBox());
    }else{
		Fbox BB;
        BB.set(-5,-5,-5,5,5,5);
        Device.m_Camera.ZoomExtents(BB);
    }
}

void CShaderTools::OnDeviceCreate(){
    // add directional light
    Flight L;
    ZeroMemory(&L,sizeof(Flight));
    L.type = D3DLIGHT_DIRECTIONAL;
    L.diffuse.set(1,1,1,1);
    L.direction.set(1,-1,1); L.direction.normalize();
	Device.SetLight(0,L);
	Device.LightEnable(0,true);

    L.diffuse.set(0.3,0.3,0.3,1);
    L.direction.set(-1,-1,-1); L.direction.normalize();
	Device.SetLight(1,L);
	Device.LightEnable(1,true);

    L.diffuse.set(0.3,0.3,0.3,1);
    L.direction.set(1,-1,-1); L.direction.normalize();
	Device.SetLight(2,L);
	Device.LightEnable(2,true);

    L.diffuse.set(0.3,0.3,0.3,1);
    L.direction.set(-1,-1,1); L.direction.normalize();
	Device.SetLight(3,L);
	Device.LightEnable(3,true);

	L.diffuse.set(1.0,0.8,0.7,1);
    L.direction.set(0,1,0); L.direction.normalize();
	Device.SetLight(4,L);
	Device.LightEnable(4,true);
}

void CShaderTools::OnDeviceDestroy(){
}

void CShaderTools::SelectPreviewObject(int p){
    LPCSTR fn;
    switch(p){
        case 0: fn="$ShaderTest_Plane"; 	break;
        case 1: fn="$ShaderTest_Box"; 		break;
        case 2: fn="$ShaderTest_Sphere"; 	break;
        case 3: fn="$ShaderTest_Teapot";	break;
        case -1: fn=m_EditObject?m_EditObject->GetName():""; fn=TfrmChoseItem::SelectObject(false,true,0,fn); if (!fn) return; break;
        default: THROW2("Failed select test object.");
    }
    m_LibObject = Lib->SearchObject(fn);
    m_EditObject = 0;
    if (m_LibObject) m_EditObject = m_LibObject->GetReference();
    if (!m_LibObject||!m_EditObject)
        ELog.DlgMsg(mtError,"System object '%s.object' can't find in object library. Preview disabled.",fn);
	ZoomObject();
    UI->RedrawScene();
}

void CShaderTools::ResetPreviewObject(){
    m_LibObject 	= 0;
    m_EditObject 	= 0;
    UI->RedrawScene();
}

void CShaderTools::Load(){
	AnsiString fn = "shaders.xr";
    FS.m_GameRoot.Update(fn);

    if (FS.Exist(fn.c_str())){
        CCompressedStream		FS(fn.c_str(),"shENGINE");
        char					name[256];

        // Load constants
        {
            CStream*	fs		= FS.OpenChunk(0);
            while (!fs->Eof())	{
                fs->RstringZ	(name);
                CConstant*		C = new CConstant;
                C->Load			(fs);
                m_Constants.insert(make_pair(strdup(name),C));
            }
            fs->Close();
        }

        // Load matrices
        {
            CStream*	fs		= FS.OpenChunk(1);
            while (!fs->Eof())	{
                fs->RstringZ	(name);
                CMatrix*		M = new CMatrix;
                M->Load			(fs);
                m_Matrices.insert(make_pair(strdup(name),M));
            }
            fs->Close();
        }

        // load blenders
        {
            CStream*	fs		= FS.OpenChunk(2);
            while (!fs->Eof())	{
                CBlender_DESC	desc;
                fs->Read		(&desc,sizeof(desc));
                CBlender*		B = CBlender::Create(desc.CLS);
                fs->Seek		(fs->Tell()-sizeof(desc));
                B->Load			(*fs);
                m_Blenders.insert(make_pair(strdup(desc.cName),B));
            }
            fs->Close();
        }
    }else{
    	ELog.DlgMsg(mtInformation,"Can't find file 'shaders.xr'");
    }
}

void CShaderTools::Save(){
}

CBlender* CShaderTools::FindBlenderByName(LPCSTR name){
	R_ASSERT(name && name[0]);
	LPSTR N = LPSTR(name);
	BlenderPairIt I = m_Blenders.find	(N);
	if (I==m_Blenders.end())return 0;
	else					return I->second;
}

LPCSTR CShaderTools::GenerateBlenderName(LPSTR name, LPCSTR source){
    int cnt = 0;
    if (source) strcpy(name,source); else sprintf(name,"shader_%04d",cnt++);
	while (FindBlenderByName(name))
    	if (source) sprintf(name,"%s_%04d",source,cnt++);
        else sprintf(name,"shader_%04d",cnt++);
	return name;
}

CBlender* CShaderTools::AppendBlender(CLASS_ID cls_id, CBlender* parent){
    CBlender* B = CBlender::Create(cls_id);
    if (parent) *B = *parent;
    char name[128];
    B->getDescription().Setup(GenerateBlenderName(name,parent?parent->getName():0));
	m_Blenders.insert(make_pair(strdup(name),B));
    return B;
}

void CShaderTools::SetCurrentBlender(CBlender* B){
	m_CurrentBlender = B;
    TfrmShaderProperties::LoadProperties();
}

bool CShaderTools::GetCurrentBlender(CFS_Base& data){
	if (m_CurrentBlender)
		m_CurrentBlender->Save(data);
    return !!m_CurrentBlender;
}

void CShaderTools::SetCurrentBlender(CStream& data){
	if (m_CurrentBlender)
		m_CurrentBlender->Load(data);
}


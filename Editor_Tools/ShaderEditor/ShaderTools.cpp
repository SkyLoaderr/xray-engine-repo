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
    Load();
}

void CShaderTools::OnDestroy(){
    Device.seqDevCreate.Remove(this);
    Device.seqDevDestroy.Remove(this);
	// free palette
	for (TemplateIt it=m_TemplatePalette.begin(); it!=m_TemplatePalette.end(); it++)
    	_DELETE(*it);
    m_TemplatePalette.clear();
    // free constants, matrices, blenders
	// Blender
	for (BlenderPairIt b=m_Blenders.begin(); b!=m_Blenders.end(); b++)
	{
		free		(b->first);
		delete		b->second;
	}
	m_Blenders.clear	();

	// Matrices
	for (MatrixPairIt m=m_Matrices.begin(); m!=m_Matrices.end(); m++){
		free		(m->first);
		delete		m->second;
	}
	m_Matrices.clear	();

	// Constants
	for (ConstantPairIt c=m_Constants.begin(); c!=m_Constants.end(); c++){
		free		(c->first);
		delete		c->second;
	}
	m_Constants.clear	();

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
                fraLeftBar->AddBlender(desc.cName);
            }
            fs->Close();
        }

        ExpandReferences();
    }else{
    	ELog.DlgMsg(mtInformation,"Can't find file 'shaders.xr'");
    }
}

void CShaderTools::Save(){
	CollapseReferences();
    // save
    //...
    ExpandReferences();
}

CBlender* CShaderTools::FindBlender(LPCSTR name){
	R_ASSERT(name && name[0]);
	LPSTR N = LPSTR(name);
	BlenderPairIt I = m_Blenders.find	(N);
	if (I==m_Blenders.end())return 0;
	else					return I->second;
}

CMatrix* CShaderTools::FindMatrix(LPCSTR name){
	R_ASSERT(name && name[0]);
	LPSTR N = LPSTR(name);
	MatrixPairIt I = m_Matrices.find	(N);    
	if (I==m_Matrices.end())return 0;
	else					return I->second;
}

CConstant* CShaderTools::FindConstant(LPCSTR name){
	R_ASSERT(name && name[0]);
	LPSTR N = LPSTR(name);
	ConstantPairIt I = m_Constants.find	(N);
	if (I==m_Constants.end())return 0;
	else					return I->second;
}

LPCSTR CShaderTools::GenerateBlenderName(LPSTR name, LPCSTR source){
    int cnt = 0;
	char fld[128]; strcpy(fld,name);
    if (source) strcpy(name,source); else sprintf(name,"%s\shader_%04d",fld,cnt++);
	while (FindBlender(name))
    	if (source) sprintf(name,"%s_%04d",source,cnt++);
        else sprintf(name,"%s\shader_%04d",fld,cnt++);
	return name;
}

LPCSTR CShaderTools::GenerateMatrixName(LPSTR name){
    int cnt = 0;
    do sprintf(name,"%04x",cnt++);
    while(FindMatrix(name));
    return name;
}

LPCSTR CShaderTools::GenerateConstantName(LPSTR name){
    int cnt = 0;
    do sprintf(name,"%04x",cnt++);
    while(FindConstant(name));
    return name;
}

CBlender* CShaderTools::AppendBlender(CLASS_ID cls_id, LPCSTR folder_name, CBlender* parent){
	// append blender
    CBlender* B = CBlender::Create(cls_id);
    if (parent) *B = *parent;
    char name[128];
    strcpy(name,folder_name);
    B->getDescription().Setup(GenerateBlenderName(name,parent?parent->getName():0));
    // append matrix& constant
    CFS_Memory M;
    B->Save(M);
    CStream data(M.pointer(), M.size());
    data.Advance(sizeof(CBlender_DESC));
    DWORD type;
    char key[255];

    while (!data.Eof()){
        int sz=0;
        type = data.Rdword();
        data.RstringZ(key);
        switch(type){
        case BPID_MARKER:	break;
        case BPID_MATRIX:	sz=sizeof(string64); strcpy((LPSTR)data.Pointer(),AppendMatrix()); break;
        case BPID_CONSTANT:	sz=sizeof(string64); strcpy((LPSTR)data.Pointer(),AppendConstant()); break;
        case BPID_TEXTURE: 	sz=sizeof(string64); 	break;
        case BPID_INTEGER: 	sz=sizeof(BP_Integer);	break;
        case BPID_FLOAT: 	sz=sizeof(BP_Float); 	break;
        case BPID_BOOL: 	sz=sizeof(BP_BOOL); 	break;
        default: THROW2("BPID_????");
        }
        data.Advance(sz);
    }
    // insert blender
	m_Blenders.insert(make_pair(strdup(name),B));
	fraLeftBar->AddBlender(name);
    return B;
}

LPCSTR CShaderTools::AppendConstant(CConstant* src){
    CConstant* C = new CConstant();
    if (src) *C = *src;
    C->dwReference = 1;
    char name[128];
    pair<ConstantPairIt, bool> I = m_Constants.insert(make_pair(strdup(GenerateConstantName(name)),C));
    VERIFY(I.second);
    return I.first->first;
}

LPCSTR CShaderTools::AppendMatrix(CMatrix* src){
    CMatrix* M = new CMatrix();
    if (src) *M = *src;
    M->dwReference = 1;
    char name[128];
    pair<MatrixPairIt, bool> I = m_Matrices.insert(make_pair(strdup(GenerateMatrixName(name)),M));
    VERIFY(I.second);
    return I.first->first;
}

void CShaderTools::RemoveBlender(LPCSTR name){
	R_ASSERT(name && name[0]);
	CBlender* B = FindBlender(name);
    _DELETE(B);
	LPSTR N = LPSTR(name);
    m_Blenders.erase(N);
}

void CShaderTools::RemoveMatrix(LPCSTR name){
	R_ASSERT(name && name[0]);
	CMatrix* M = FindMatrix(name); VERIFY(M);
    M->dwReference--;
    if (M->dwReference==0){
	    _DELETE(M);
		LPSTR N = LPSTR(name);
    	m_Matrices.erase(N);
    }
}

void CShaderTools::RemoveConstant(LPCSTR name){
	R_ASSERT(name && name[0]);
	CConstant* C = FindConstant(name); VERIFY(C);
    C->dwReference--;
    if (C->dwReference==0){
	    _DELETE(C);
		LPSTR N = LPSTR(name);
	    m_Constants.erase(N);
    }
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

void CShaderTools::ExpandMatrix(LPSTR name){
	if (name&&name[0]){
    	CMatrix* M = FindMatrix(name); VERIFY(M);
		if (M->dwReference>0)
        	strcpy(name,AppendMatrix(M));
    }else{
    	strcpy(name,AppendMatrix());
    }
}

void CShaderTools::ExpandConstant(LPSTR name){
	if (name&&name[0]){
    	CConstant* C = FindConstant(name); VERIFY(C);
		if (C->dwReference>0)
        	strcpy(name,AppendConstant(C));
    }else{
    	strcpy(name,AppendMatrix());
    }
}

//
class CExpandBlender: public CParseBlender{
public:
	virtual void Parse(DWORD type, LPCSTR key, LPVOID data){
    	switch(type){
        case BPID_MATRIX: 	SHTools.ExpandMatrix((LPSTR)data); break;
        case BPID_CONSTANT: SHTools.ExpandConstant((LPSTR)data); break;
        };
    }
};

class CCollapseBlender: public CParseBlender{
public:
	virtual void Parse(DWORD type, LPCSTR key, LPVOID data){
    	switch(type){
        case BPID_MATRIX: 	SHTools.CollapseMatrix((LPSTR)data); break;
        case BPID_CONSTANT: SHTools.CollapseConstant((LPSTR)data); break;
        };
    }
};

void CShaderTools::CollapseMatrix(LPSTR name){
	R_ASSERT(name&&name[0]);
    CMatrix* M = FindMatrix(name); VERIFY(M);
    for (MatrixPairIt m=m_OptMatrices.begin(); m!=m_OptMatrices.end(); m++){
        if (m->second->Similar(*M)){
            strcpy(name,m->first);
            return;
        }
    }
    // append opt matrix
	m_OptMatrices.insert(make_pair(strdup(name),M));
}

void CShaderTools::CollapseConstant(LPSTR name){
	R_ASSERT(name&&name[0]);
    CConstant* C = FindConstant(name); VERIFY(C);
    for (ConstantPairIt c=m_OptConstants.begin(); c!=m_OptConstants.end(); c++){
        if (c->second->Similar(*C)){
            strcpy(name,c->first);
            return;
        }
    }
    // append opt constant
	m_OptConstants.insert(make_pair(strdup(name),C));
}

static CExpandBlender 	ExpandBlender;
static CCollapseBlender CollapseBlender;

void CShaderTools::ParseBlender(CBlender* B, CParseBlender& P){
    CFS_Memory M;
    B->Save(M);

    CStream data(M.pointer(), M.size());
    data.Advance(sizeof(CBlender_DESC));
    DWORD type;
    char key[255];

    while (!data.Eof()){
        int sz=0;
        type = data.Rdword();
        data.RstringZ(key);
        switch(type){
        case BPID_MARKER:							break;
        case BPID_MATRIX:	sz=sizeof(string64); 	break;
        case BPID_CONSTANT:	sz=sizeof(string64); 	break;
        case BPID_TEXTURE: 	sz=sizeof(string64); 	break;
        case BPID_INTEGER: 	sz=sizeof(BP_Integer);	break;
        case BPID_FLOAT: 	sz=sizeof(BP_Float); 	break;
        case BPID_BOOL: 	sz=sizeof(BP_BOOL); 	break;
        default: THROW2("BPID_????");
        }
        P.Parse(type, key, data.Pointer());
        data.Advance(sz);
    }
}

void CShaderTools::ExpandReferences(){
	for (BlenderPairIt b=m_Blenders.begin(); b!=m_Blenders.end(); b++)
    	ParseBlender(b->second,ExpandBlender);
}

void CShaderTools::CollapseReferences(){
	for (BlenderPairIt b=m_Blenders.begin(); b!=m_Blenders.end(); b++)
    	ParseBlender(b->second,CollapseBlender);
	for (MatrixPairIt m=m_Matrices.begin(); m!=m_Matrices.end(); m++) free(m->first);
	for (ConstantPairIt c=m_Constants.begin(); c!=m_Constants.end(); c++) free(c->first);
	m_Matrices.clear	();
	m_Constants.clear	();
	m_Matrices 			= m_OptMatrices;
    m_Constants 		= m_OptConstants;
	m_OptConstants.clear();
    m_OptMatrices.clear	();
}


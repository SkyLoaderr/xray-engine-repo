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
#include "xr_trims.h"

//------------------------------------------------------------------------------
CShaderTools SHTools;
//------------------------------------------------------------------------------
class CCollapseBlender: public CParseBlender{
public:
	virtual void Parse(DWORD type, LPCSTR key, LPVOID data){
    	switch(type){
        case BPID_MATRIX: 	SHTools.CollapseMatrix((LPSTR)data); break;
        case BPID_CONSTANT: SHTools.CollapseConstant((LPSTR)data); break;
        };
    }
};

class CRefsBlender: public CParseBlender{
public:
	virtual void Parse(DWORD type, LPCSTR key, LPVOID data){
    	switch(type){
        case BPID_MATRIX: 	SHTools.UpdateMatrixRefs	((LPSTR)data); break;
        case BPID_CONSTANT: SHTools.UpdateConstantRefs	((LPSTR)data); break;
        };
    }
};

class CRemoveBlender: public CParseBlender{
public:
	virtual void Parse(DWORD type, LPCSTR key, LPVOID data){
    	switch(type){
        case BPID_MATRIX: 	SHTools.RemoveMatrix((LPSTR)data); break;
        case BPID_CONSTANT: SHTools.RemoveConstant((LPSTR)data); break;
        };
    }
};

static CCollapseBlender ST_CollapseBlender;
static CRefsBlender 	ST_UpdateBlenderRefs;
static CRemoveBlender	ST_RemoveBlender;

//------------------------------------------------------------------------------
CShaderTools::CShaderTools(){
    m_bModified 		= FALSE;
	m_LibObject 		= 0;
	m_EditObject 		= 0;
    m_CurrentBlender 	= 0;
    m_bUpdateCurrent	= false;
    m_BlenderStream.clear();
}

CShaderTools::~CShaderTools(){
}
//---------------------------------------------------------------------------

void CShaderTools::Modified(){
	m_bModified=TRUE;
	UI->Command(COMMAND_UPDATE_CAPTION);
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

    ClearData();

    // hide properties window
	TfrmShaderProperties::HideProperties();
    m_bModified = FALSE;
}

void CShaderTools::ClearData(){
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
}

bool CShaderTools::IfModified(){
    if (m_bModified){
        int mr = ELog.DlgMsg(mtConfirmation, "The shaders has been modified.\nDo you want to save your changes?");
        switch(mr){
        case mrYes: if (!UI->Command(COMMAND_SAVE)) return false; else m_bModified = FALSE; break;
        case mrNo: m_bModified = FALSE; break;
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

void CShaderTools::ApplyChanges(){
    if (m_CurrentBlender){
        CStream data(m_BlenderStream.pointer(), m_BlenderStream.size());
        m_CurrentBlender->Load(data);
		Modified();
    }
}

void CShaderTools::Reload(){
	fraLeftBar->ClearBlenderList();
    ResetCurrentBlender();
    ClearData();
    Load();
}

void CShaderTools::Load(){
	AnsiString fn = "shaders.xr";
    FS.m_GameRoot.Update(fn);

    m_bUpdateCurrent	= false;

    if (FS.Exist(fn.c_str())){
        CCompressedStream		FS(fn.c_str(),"shENGINE");
        char					name[256];

        // Load constants
        {
            CStream*	fs		= FS.OpenChunk(0);
            while (fs&&!fs->Eof())	{
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
            while (fs&&!fs->Eof())	{
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
            while (fs&&!fs->Eof())	{
                CBlender_DESC	desc;
                fs->Read		(&desc,sizeof(desc));
                CBlender*		B = CBlender::Create(desc.CLS);
                fs->Seek		(fs->Tell()-sizeof(desc));
                B->Load			(*fs);
                m_Blenders.insert(make_pair(strdup(desc.cName),B));
                fraLeftBar->AddBlender(desc.cName,true);
            }
            fs->Close();
        }
        UpdateRefCounters		();
        ResetCurrentBlender		();
    }else{
    	ELog.DlgMsg(mtInformation,"Can't find file 'shaders.xr'");
    }
	m_bUpdateCurrent			= true;
}

void CShaderTools::Save(){
    ApplyChanges();
    LPCSTR name					= m_CurrentBlender?m_CurrentBlender->getName():0;
	ResetCurrentBlender			();
	m_bUpdateCurrent			= false;

	CollapseReferences();
    // save
	AnsiString fn = "shaders.xr";
    FS.m_GameRoot.Update(fn);

    CFS_Memory F;

    // Save constants
    {
    	F.open_chunk(0);
		for (ConstantPairIt c=m_Constants.begin(); c!=m_Constants.end(); c++){
        	F.WstringZ(c->first);
        	c->second->Save(&F);
    	}
        F.close_chunk();
    }

    // Save matrices
    {
    	F.open_chunk(1);
		for (MatrixPairIt m=m_Matrices.begin(); m!=m_Matrices.end(); m++){
        	F.WstringZ(m->first);
        	m->second->Save(&F);
        }
        F.close_chunk();
    }

    // Save blenders
    {
    	F.open_chunk(2);
		for (BlenderPairIt b=m_Blenders.begin(); b!=m_Blenders.end(); b++)
        	b->second->Save(F);
        F.close_chunk();
    }
    // copy exist file
    FS.MarkFile(fn);
    // save new file
    F.SaveTo(fn.c_str(), "shENGINE");
	m_bUpdateCurrent		= true;
	SetCurrentBlender		(name);

    m_bModified	= FALSE;
}

CBlender* CShaderTools::FindBlender(LPCSTR name){
	if (name && name[0]){
		LPSTR N = LPSTR(name);
		BlenderPairIt I = m_Blenders.find	(N);
		if (I==m_Blenders.end())return 0;
		else					return I->second;
    }else return 0;
}

CMatrix* CShaderTools::FindMatrix(LPSTR name, bool bDuplicate){
	R_ASSERT(name && name[0]);
	LPSTR N = LPSTR(name);
	MatrixPairIt I = m_Matrices.find	(N);
    CMatrix* M = 0;
	if (I==m_Matrices.end())return M;
	else					M=I->second;
    if (bDuplicate&&M->dwReference>1){
		M->dwReference--;
        strcpy(name,AppendMatrix(M,&M));
    }
    return M;
}

CConstant* CShaderTools::FindConstant(LPSTR name, bool bDuplicate){
	R_ASSERT(name && name[0]);
	LPSTR N = LPSTR(name);
	ConstantPairIt I = m_Constants.find	(N);
    CConstant* C = 0;
	if (I==m_Constants.end())return C;
	else					C=I->second;
    if (bDuplicate&&C->dwReference>1){
		C->dwReference--;
        strcpy(name,AppendConstant(C,&C));
    }
    return C;
}

LPCSTR CShaderTools::GenerateBlenderName(LPSTR name, LPCSTR source){
    int cnt = 0;
	char fld[128]; strcpy(fld,name);
    if (source) strcpy(name,source); else sprintf(name,"%s\shader_%02d",fld,cnt++);
	while (FindBlender(name))
    	if (source) sprintf(name,"%s_%02d",source,cnt++);
        else sprintf(name,"%s\shader_%02d",fld,cnt++);
	return name;
}

LPCSTR CShaderTools::GenerateMatrixName(LPSTR name){
    int cnt = 0;
    do sprintf(name,"%04x",cnt++);
    while(FindMatrix(name,false));
    return name;
}

LPCSTR CShaderTools::GenerateConstantName(LPSTR name){
    int cnt = 0;
    do sprintf(name,"%04x",cnt++);
    while(FindConstant(name,false));
    return name;
}

CBlender* CShaderTools::AppendBlender(CLASS_ID cls_id, LPCSTR folder_name, CBlender* parent){
	// append blender
    CBlender* B = CBlender::Create(cls_id);
    if (parent) *B = *parent;
    char name[128];
    if (folder_name) strcpy(name,folder_name);
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
        case BPID_MATRIX:
        	sz=sizeof(string64);
            if (strcmp((LPSTR)data.Pointer(),"$null")!=0){
	        	if (!parent) strcpy((LPSTR)data.Pointer(),AppendMatrix());
    	        else AddMatrixRef((LPSTR)data.Pointer());
            }
        break;
        case BPID_CONSTANT:
        	sz=sizeof(string64);
            if (strcmp((LPSTR)data.Pointer(),"$null")!=0){
	            if (!parent) strcpy((LPSTR)data.Pointer(),AppendConstant());
    	        else AddConstantRef((LPSTR)data.Pointer());
            }
        break;
        case BPID_TEXTURE: 	sz=sizeof(string64); 	break;
        case BPID_INTEGER: 	sz=sizeof(BP_Integer);	break;
        case BPID_FLOAT: 	sz=sizeof(BP_Float); 	break;
        case BPID_BOOL: 	sz=sizeof(BP_BOOL); 	break;
        default: THROW2("BPID_????");
        }
        data.Advance(sz);
    }
    data.Seek(0);
    B->Load(data);
    // insert blender
	m_Blenders.insert(make_pair(strdup(name),B));
	fraLeftBar->AddBlender(name,false);
    return B;
}

CBlender* CShaderTools::CloneBlender(LPCSTR name){
	CBlender* B = FindBlender(name); R_ASSERT(B);
	return AppendBlender(B->getDescription().CLS,0,B);
}

void CShaderTools::RenameBlender(LPCSTR old_full_name, LPCSTR ren_part, int level){
    LPSTR N = LPSTR(old_full_name);
	BlenderPairIt I = m_Blenders.find	(N);
    R_ASSERT(I!=m_Blenders.end());
    VERIFY(level<_GetItemCount(old_full_name,'\\'));
    char new_name[255];
    _ReplaceItem(old_full_name,level,ren_part,new_name,'\\');
    free(I->first);
    CBlender* B = I->second;
	m_Blenders.erase(I);
	// rename
    B->getDescription().Setup(new_name);
	m_Blenders.insert(make_pair(strdup(new_name),B));
	if (B==m_CurrentBlender) UpdateStreamFromObject();
}

void CShaderTools::AddMatrixRef(LPSTR name){
	CMatrix* M = FindMatrix(name,false); R_ASSERT(M);
    M->dwReference++;
}

void CShaderTools::AddConstantRef(LPSTR name){
	CConstant* C = FindConstant(name,false); R_ASSERT(C);
    C->dwReference++;
}

LPCSTR CShaderTools::AppendConstant(CConstant* src, CConstant** dest){
    CConstant* C = new CConstant();
    if (src) *C = *src;
    C->dwReference = 1;
    char name[128];
    pair<ConstantPairIt, bool> I = m_Constants.insert(make_pair(strdup(GenerateConstantName(name)),C));
    VERIFY(I.second);
    if (dest) *dest = C;
    return I.first->first;
}

LPCSTR CShaderTools::AppendMatrix(CMatrix* src, CMatrix** dest){
    CMatrix* M = new CMatrix();
    if (src) *M = *src;
    M->dwReference = 1;
    char name[128];
    pair<MatrixPairIt, bool> I = m_Matrices.insert(make_pair(strdup(GenerateMatrixName(name)),M));
    VERIFY(I.second);
    if (dest) *dest = M;
    return I.first->first;
}

void CShaderTools::RemoveBlender(LPCSTR name){
	R_ASSERT(name && name[0]);
	CBlender* B = FindBlender(name);
    R_ASSERT(B);
    // remove refs
	ParseBlender(B,ST_RemoveBlender);
	LPSTR N = LPSTR(name);
    BlenderPairIt I = m_Blenders.find	(N);
    free 	(I->first);
    delete	I->second;
    m_Blenders.erase(I);
}

void CShaderTools::RemoveMatrix(LPSTR name){
	if (*name=='$') return;
	R_ASSERT(name && name[0]);
	CMatrix* M = FindMatrix(name,false); VERIFY(M);
    M->dwReference--;
    if (M->dwReference==0){
		LPSTR N = LPSTR(name);
	    MatrixPairIt I = m_Matrices.find	(N);
    	free 	(I->first);
	    delete	I->second;
    	m_Matrices.erase(I);
    }
}

void CShaderTools::RemoveConstant(LPSTR name){
	if (*name=='$') return;
	R_ASSERT(name && name[0]);
	CConstant* C = FindConstant(name,false); VERIFY(C);
    C->dwReference--;
    if (C->dwReference==0){
		LPSTR N = LPSTR(name);
	    ConstantPairIt I = m_Constants.find	(N);
    	free 	(I->first);
	    delete	I->second;
	    m_Constants.erase(I);
    }
}

void CShaderTools::UpdateStreamFromObject(){
    m_BlenderStream.clear();
    if (m_CurrentBlender) m_CurrentBlender->Save(m_BlenderStream);
    TfrmShaderProperties::InitProperties();
}

void CShaderTools::UpdateObjectFromStream(){
    if (m_CurrentBlender){
        CStream data(m_BlenderStream.pointer(), m_BlenderStream.size());
        m_CurrentBlender->Load(data);
    }
}

void CShaderTools::SetCurrentBlender(CBlender* B){
    if (!m_bUpdateCurrent) return;

    UpdateObjectFromStream();
	if (m_CurrentBlender!=B){
        m_CurrentBlender = B;
        UpdateStreamFromObject();
    }
}

void CShaderTools::ResetCurrentBlender(){
	m_CurrentBlender=0;
    UpdateStreamFromObject();
}

void CShaderTools::SetCurrentBlender(LPCSTR name){
	SetCurrentBlender(FindBlender(name));
}

void CShaderTools::CollapseMatrix(LPSTR name){
	if (*name=='$') return;
	R_ASSERT(name&&name[0]);
    CMatrix* M = FindMatrix(name,false); VERIFY(M);
	M->dwReference--;
    for (MatrixPairIt m=m_OptMatrices.begin(); m!=m_OptMatrices.end(); m++){
        if (m->second->Similar(*M)){
            strcpy(name,m->first);
            m->second->dwReference++;
            return;
        }
    }
    // append new optimized matrix
    CMatrix* N = new CMatrix(*M);
    N->dwReference=1;
	m_OptMatrices.insert(make_pair(strdup(name),N));
}

void CShaderTools::CollapseConstant(LPSTR name){
	if (*name=='$') return;
	R_ASSERT(name&&name[0]);
    CConstant* C = FindConstant(name,false); VERIFY(C);
    for (ConstantPairIt c=m_OptConstants.begin(); c!=m_OptConstants.end(); c++){
    	if (C==c->second){ c->second->dwReference++; return; }
        if (c->second->Similar(*C)){
            strcpy(name,c->first);
            _DELETE(C);
            c->second->dwReference++;
            return;
        }
    }
    // append opt constant
	m_OptConstants.insert(make_pair(strdup(name),C));
}

void CShaderTools::UpdateMatrixRefs(LPSTR name){
	if (*name=='$') return;
	R_ASSERT(name&&name[0]);
	CMatrix* M = FindMatrix(name,false); R_ASSERT(M);
	M->dwReference++;
}

void CShaderTools::UpdateConstantRefs(LPSTR name){
	if (*name=='$') return;
	R_ASSERT(name&&name[0]);
	CConstant* C = FindConstant(name,false); R_ASSERT(C);
	C->dwReference++;
}

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

    data.Seek(0);
    B->Load(data);
}

void CShaderTools::CollapseReferences(){
	for (BlenderPairIt b=m_Blenders.begin(); b!=m_Blenders.end(); b++)
    	ParseBlender(b->second,ST_CollapseBlender);
	for (MatrixPairIt m=m_Matrices.begin(); m!=m_Matrices.end(); m++){
    	VERIFY		(m->second->dwReference==0);
		free		(m->first);
		delete		m->second;
	}
	for (ConstantPairIt c=m_Constants.begin(); c!=m_Constants.end(); c++){
    	VERIFY		(c->second->dwReference==0);
		free		(c->first);
		delete		c->second;
	}
	m_Matrices.clear	();
	m_Constants.clear	();
	m_Matrices 			= m_OptMatrices;
    m_Constants 		= m_OptConstants;
	m_OptConstants.clear();
    m_OptMatrices.clear	();
}

void CShaderTools::UpdateRefCounters(){
	for (BlenderPairIt b=m_Blenders.begin(); b!=m_Blenders.end(); b++)
    	ParseBlender(b->second,ST_UpdateBlenderRefs);
}




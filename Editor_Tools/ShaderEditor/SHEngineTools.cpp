//---------------------------------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "SHEngineTools.h"
#include "Blender.h"
#include "UI_Tools.h"
#include "ui_main.h"
#include "LeftBar.h"
#include "PropertiesShader.h"
#include "xr_trims.h"
#include "EditObject.h"

//------------------------------------------------------------------------------
class CCollapseBlender: public CParseBlender{
public:
	virtual void Parse(DWORD type, LPCSTR key, LPVOID data){
    	switch(type){
        case xrPID_MATRIX: 	Tools.Engine.CollapseMatrix		((LPSTR)data); break;
        case xrPID_CONSTANT: Tools.Engine.CollapseConstant	((LPSTR)data); break;
        };
    }
};

class CRefsBlender: public CParseBlender{
public:
	virtual void Parse(DWORD type, LPCSTR key, LPVOID data){
    	switch(type){
        case xrPID_MATRIX: 	Tools.Engine.UpdateMatrixRefs		((LPSTR)data); break;
        case xrPID_CONSTANT: Tools.Engine.UpdateConstantRefs	((LPSTR)data); break;
        };
    }
};

class CRemoveBlender: public CParseBlender{
public:
	virtual void Parse(DWORD type, LPCSTR key, LPVOID data){
    	switch(type){
        case xrPID_MATRIX: 	Tools.Engine.RemoveMatrix((LPSTR)data); break;
        case xrPID_CONSTANT: Tools.Engine.RemoveConstant((LPSTR)data); break;
        };
    }
};

static CCollapseBlender ST_CollapseBlender;
static CRefsBlender 	ST_UpdateBlenderRefs;
static CRemoveBlender	ST_RemoveBlender;

//------------------------------------------------------------------------------
CSHEngineTools::CSHEngineTools(){
    m_bModified 		= FALSE;
    m_CurrentBlender 	= 0;
    m_bUpdateCurrent	= false;
    m_BlenderStream.clear();
}

CSHEngineTools::~CSHEngineTools(){
}
//---------------------------------------------------------------------------

void CSHEngineTools::Modified(){
	m_bModified=TRUE;
	UI.Command(COMMAND_UPDATE_CAPTION);
}
//---------------------------------------------------------------------------

void CSHEngineTools::OnCreate(){
	CBlender::CreatePalette(m_TemplatePalette);
    fraLeftBar->InitPalette(m_TemplatePalette);
    Load();
}

void CSHEngineTools::OnDestroy(){
	// free palette
	for (TemplateIt it=m_TemplatePalette.begin(); it!=m_TemplatePalette.end(); it++)
    	_DELETE(*it);
    m_TemplatePalette.clear();

    ClearData();

    // hide properties window
	TfrmShaderProperties::HideProperties();
    m_bModified = FALSE;
}

void CSHEngineTools::ClearData(){
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

bool CSHEngineTools::IfModified(){
    if (m_bModified){
        int mr = ELog.DlgMsg(mtConfirmation, "The shaders has been modified.\nDo you want to save your changes?");
        switch(mr){
        case mrYes: if (!UI.Command(COMMAND_SAVE)) return false; else m_bModified = FALSE; break;
        case mrNo: m_bModified = FALSE; break;
        case mrCancel: return false;
        }
    }
    return true;
}

void CSHEngineTools::UpdateDeviceShaders()
{
    Save(TRUE);
    Device.Reset(SHADER_FILENAME_TEMP,TRUE);
}


void CSHEngineTools::ApplyChanges(){
    if (m_CurrentBlender&&TfrmShaderProperties::IsModified()){
    	UpdateObjectFromStream();
        // update visual
        Tools.UpdateDeviceShaders();
        // set modified flag
		Modified();
    }
}

void CSHEngineTools::Reload(){
	fraLeftBar->ClearEShaderList();
    ResetCurrentBlender();
    ClearData();
    Load();
}

void CSHEngineTools::Load(){
	AnsiString fn = "shaders.xr";
    FS.m_GameRoot.Update(fn);

	TfrmShaderProperties::FreezeUpdate(true);
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

        // Load blenders
        {
            CStream*	fs		= FS.OpenChunk(2);
            CStream*	chunk	= NULL;
            int			chunk_id= 0;

            while ((chunk=fs->OpenChunk(chunk_id))!=NULL)
            {
                CBlender_DESC	desc;
                chunk->Read		(&desc,sizeof(desc));
                CBlender*		B = CBlender::Create(desc.CLS);
				if (B->getDescription().version!=desc.version){
                	_DELETE		(B);
                    ELog.DlgMsg	(mtError,"Can't load blender '%s'. Unsupported version.",desc.cName);
                    chunk->Close	();
                    chunk_id++;
                	continue;
                }
                chunk->Seek		(0);
                B->Load			(*chunk);
                m_Blenders.insert	(make_pair(strdup(desc.cName),B));
                fraLeftBar->AddBlender(desc.cName,true);
                chunk->Close	();
                chunk_id++;
            }
            fs->Close();
        }

        UpdateRefCounters		();
        ResetCurrentBlender		();
    }else{
    	ELog.DlgMsg(mtInformation,"Can't find file '%s'",fn.c_str());
    }
	m_bUpdateCurrent			= true;
	TfrmShaderProperties::FreezeUpdate(false);
}

void CSHEngineTools::Save(BOOL bForVisUpdate){
	TfrmShaderProperties::FreezeUpdate(true);

    LPCSTR name					= m_CurrentBlender?m_CurrentBlender->getName():0;
	ResetCurrentBlender			();
	m_bUpdateCurrent			= false;

	CollapseReferences();
    // save
	AnsiString fn 				= bForVisUpdate?SHADER_FILENAME_TEMP:SHADER_FILENAME_BASE;
    FS.m_GameRoot.Update		(fn);

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
        int chunk_id=0;
		for (BlenderPairIt b=m_Blenders.begin(); b!=m_Blenders.end(); b++){
			F.open_chunk(chunk_id++);
        	b->second->Save(F);
	        F.close_chunk();
        }
        F.close_chunk();
    }
    // copy exist file
    FS.MarkFile(fn);
    // save new file
    F.SaveTo(fn.c_str(), "shENGINE");
	m_bUpdateCurrent		= true;
	SetCurrentBlender		(name);

	if (!bForVisUpdate){
    	m_bModified	= FALSE;
		TfrmShaderProperties::ResetModified();
    }

	TfrmShaderProperties::FreezeUpdate(false);
}

CBlender* CSHEngineTools::FindBlender(LPCSTR name){
	if (name && name[0]){
		LPSTR N = LPSTR(name);
		BlenderPairIt I = m_Blenders.find	(N);
		if (I==m_Blenders.end())return 0;
		else					return I->second;
    }else return 0;
}

CMatrix* CSHEngineTools::FindMatrix(LPSTR name, bool bDuplicate){
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

CConstant* CSHEngineTools::FindConstant(LPSTR name, bool bDuplicate){
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

LPCSTR CSHEngineTools::GenerateBlenderName(LPSTR name, LPCSTR source){
    int cnt = 0;
	char fld[128]; strcpy(fld,name);
    if (source) strcpy(name,source); else sprintf(name,"%s\shader_%02d",fld,cnt++);
	while (FindBlender(name))
    	if (source) sprintf(name,"%s_%02d",source,cnt++);
        else sprintf(name,"%s\shader_%02d",fld,cnt++);
	return name;
}

LPCSTR CSHEngineTools::GenerateMatrixName(LPSTR name){
    int cnt = 0;
    do sprintf(name,"%04x",cnt++);
    while(FindMatrix(name,false));
    return name;
}

LPCSTR CSHEngineTools::GenerateConstantName(LPSTR name){
    int cnt = 0;
    do sprintf(name,"%04x",cnt++);
    while(FindConstant(name,false));
    return name;
}

CBlender* CSHEngineTools::AppendBlender(CLASS_ID cls_id, LPCSTR folder_name, CBlender* parent){
	// append blender
    char old_name[128]; if (parent) strcpy(old_name,parent->getName());
    CBlender* B = CBlender::Create(cls_id);
    if (parent) *B = *parent;
    char name[128]; name[0]=0;
    if (folder_name) strcpy(name,folder_name);
    B->getDescription().Setup(GenerateBlenderName(name,parent?old_name:0));
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
        case xrPID_MARKER:	break;
        case xrPID_MATRIX:
        	sz=sizeof(string64);
            if (strcmp((LPSTR)data.Pointer(),"$null")!=0){
	        	if (!parent) strcpy((LPSTR)data.Pointer(),AppendMatrix());
    	        else AddMatrixRef((LPSTR)data.Pointer());
            }
        break;
        case xrPID_CONSTANT:
        	sz=sizeof(string64);
            if (strcmp((LPSTR)data.Pointer(),"$null")!=0){
	            if (!parent) strcpy((LPSTR)data.Pointer(),AppendConstant());
    	        else AddConstantRef((LPSTR)data.Pointer());
            }
        break;
        case xrPID_TEXTURE: 	sz=sizeof(string64); 	break;
        case xrPID_INTEGER: 	sz=sizeof(xrP_Integer);	break;
        case xrPID_FLOAT: 	sz=sizeof(xrP_Float); 	break;
        case xrPID_BOOL: 	sz=sizeof(xrP_BOOL); 	break;
        case xrPID_TOKEN: 	sz=sizeof(xrP_TOKEN)+sizeof(xrP_TOKEN::Item)*(((xrP_TOKEN*)data.Pointer())->Count);	break;
        default: THROW2("xrPID_????");
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

CBlender* CSHEngineTools::CloneBlender(LPCSTR name){
	CBlender* B = FindBlender(name); R_ASSERT(B);
	return AppendBlender(B->getDescription().CLS,0,B);
}

void CSHEngineTools::RenameBlender(LPCSTR old_full_name, LPCSTR new_full_name){
    ApplyChanges();
    LPSTR N = LPSTR(old_full_name);
	BlenderPairIt I = m_Blenders.find	(N);
    R_ASSERT(I!=m_Blenders.end());
    free(I->first);
    CBlender* B = I->second;
	m_Blenders.erase(I);
	// rename
    B->getDescription().Setup(new_full_name);
	m_Blenders.insert(make_pair(strdup(new_full_name),B));
	if (B==m_CurrentBlender) UpdateStreamFromObject();
}

void CSHEngineTools::RenameBlender(LPCSTR old_full_name, LPCSTR ren_part, int level){
    VERIFY(level<_GetItemCount(old_full_name,'\\'));
    char new_full_name[255];
    _ReplaceItem(old_full_name,level,ren_part,new_full_name,'\\');
	RenameBlender(old_full_name,new_full_name);
}

void CSHEngineTools::AddMatrixRef(LPSTR name){
	CMatrix* M = FindMatrix(name,false); R_ASSERT(M);
    M->dwReference++;
}

void CSHEngineTools::AddConstantRef(LPSTR name){
	CConstant* C = FindConstant(name,false); R_ASSERT(C);
    C->dwReference++;
}

LPCSTR CSHEngineTools::AppendConstant(CConstant* src, CConstant** dest){
    CConstant* C = new CConstant();
    if (src) *C = *src;
    C->dwReference = 1;
    char name[128];
    pair<ConstantPairIt, bool> I = m_Constants.insert(make_pair(strdup(GenerateConstantName(name)),C));
    VERIFY(I.second);
    if (dest) *dest = C;
    return I.first->first;
}

LPCSTR CSHEngineTools::AppendMatrix(CMatrix* src, CMatrix** dest){
    CMatrix* M = new CMatrix();
    if (src) *M = *src;
    M->dwReference = 1;
    char name[128];
    pair<MatrixPairIt, bool> I = m_Matrices.insert(make_pair(strdup(GenerateMatrixName(name)),M));
    VERIFY(I.second);
    if (dest) *dest = M;
    return I.first->first;
}

void CSHEngineTools::RemoveBlender(LPCSTR name){
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

void CSHEngineTools::RemoveMatrix(LPSTR name){
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

void CSHEngineTools::RemoveConstant(LPSTR name){
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

void CSHEngineTools::UpdateStreamFromObject(){
    m_BlenderStream.clear();
    if (m_CurrentBlender) m_CurrentBlender->Save(m_BlenderStream);
	// properties
	TfrmShaderProperties::InitProperties();
}

void CSHEngineTools::UpdateObjectFromStream(){
    if (m_CurrentBlender){
        CStream data(m_BlenderStream.pointer(), m_BlenderStream.size());
        m_CurrentBlender->Load(data);
    }
}

void CSHEngineTools::SetCurrentBlender(CBlender* B){
    if (!m_bUpdateCurrent) return;

    if (TfrmShaderProperties::IsAutoApply()) ApplyChanges();
	else UpdateObjectFromStream();
	if (m_CurrentBlender!=B){
        m_CurrentBlender = B;
        UpdateStreamFromObject();
        // apply this shader to non custom object
        Tools.UpdateObjectShader();
    }
}

void CSHEngineTools::ResetCurrentBlender(){
	m_CurrentBlender=0;
    UpdateStreamFromObject();
}

void CSHEngineTools::SetCurrentBlender(LPCSTR name){
	SetCurrentBlender(FindBlender(name));
}

void CSHEngineTools::CollapseMatrix(LPSTR name){
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

void CSHEngineTools::CollapseConstant(LPSTR name){
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

void CSHEngineTools::UpdateMatrixRefs(LPSTR name){
	if (*name=='$') return;
	R_ASSERT(name&&name[0]);
	CMatrix* M = FindMatrix(name,false); R_ASSERT(M);
	M->dwReference++;
}

void CSHEngineTools::UpdateConstantRefs(LPSTR name){
	if (*name=='$') return;
	R_ASSERT(name&&name[0]);
	CConstant* C = FindConstant(name,false); R_ASSERT(C);
	C->dwReference++;
}

void CSHEngineTools::ParseBlender(CBlender* B, CParseBlender& P){
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
        case xrPID_MARKER:							break;
        case xrPID_MATRIX:	sz=sizeof(string64); 	break;
        case xrPID_CONSTANT:	sz=sizeof(string64); 	break;
        case xrPID_TEXTURE: 	sz=sizeof(string64); 	break;
        case xrPID_INTEGER: 	sz=sizeof(xrP_Integer);	break;
        case xrPID_FLOAT: 	sz=sizeof(xrP_Float); 	break;
        case xrPID_BOOL: 	sz=sizeof(xrP_BOOL); 	break;
		case xrPID_TOKEN: 	sz=sizeof(xrP_TOKEN)+sizeof(xrP_TOKEN::Item)*(((xrP_TOKEN*)data.Pointer())->Count); break;
        default: THROW2("xrPID_????");
        }
        P.Parse(type, key, data.Pointer());
        data.Advance(sz);
    }

    data.Seek(0);
    B->Load(data);
}

void CSHEngineTools::CollapseReferences(){
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

void CSHEngineTools::UpdateRefCounters(){
	for (BlenderPairIt b=m_Blenders.begin(); b!=m_Blenders.end(); b++)
    	ParseBlender(b->second,ST_UpdateBlenderRefs);
}




//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "SHEngineTools.h"
#include "Blender.h"
#include "UI_Tools.h"
#include "ui_main.h"
#include "LeftBar.h"
#include "xr_trims.h"
#include "folderlib.h"
#include "EditObject.h"
#include "EditMesh.h"
#include "Library.h"
#include "ChoseForm.h"

//------------------------------------------------------------------------------
class CCollapseBlender: public CParseBlender{
public:
	virtual void Parse(CSHEngineTools* owner, DWORD type, LPCSTR key, LPVOID data){
    	switch(type){
        case xrPID_MATRIX: 		owner->CollapseMatrix	((LPSTR)data); break;
        case xrPID_CONSTANT: 	owner->CollapseConstant	((LPSTR)data); break;
        };
    }
};

class CRefsBlender: public CParseBlender{
public:
	virtual void Parse(CSHEngineTools* owner, DWORD type, LPCSTR key, LPVOID data){
    	switch(type){
        case xrPID_MATRIX: 		owner->UpdateMatrixRefs		((LPSTR)data); break;
        case xrPID_CONSTANT: 	owner->UpdateConstantRefs	((LPSTR)data); break;
        };
    }
};

class CRemoveBlender: public CParseBlender{
public:
	virtual void Parse(CSHEngineTools* owner, DWORD type, LPCSTR key, LPVOID data){
    	switch(type){
        case xrPID_MATRIX: 		owner->RemoveMatrix((LPSTR)data); break;
        case xrPID_CONSTANT: 	owner->RemoveConstant((LPSTR)data); break;
        };
    }
};

static CCollapseBlender ST_CollapseBlender;
static CRefsBlender 	ST_UpdateBlenderRefs;
static CRemoveBlender	ST_RemoveBlender;

//------------------------------------------------------------------------------
CSHEngineTools::CSHEngineTools(ISHInit& init):ISHTools(init)
{
	m_EditObject		= 0;
    m_bCustomEditObject	= FALSE;
    m_bFreezeUpdate		= FALSE;
    m_CurrentBlender 	= 0;
    m_BlenderStream.clear();
    m_bNeedResetShaders	= TRUE;
    m_RemoteRenBlender	= FALSE;
}

CSHEngineTools::~CSHEngineTools(){
}
//---------------------------------------------------------------------------

bool CSHEngineTools::OnCreate()
{
	CBlender::CreatePalette(m_TemplatePalette);
    Load();
    return true;
}

void CSHEngineTools::OnDestroy()
{
    Lib.RemoveEditObject(m_EditObject);
	// free palette
	for (TemplateIt it=m_TemplatePalette.begin(); it!=m_TemplatePalette.end(); it++)
    	xr_delete(*it);
    m_TemplatePalette.clear();

    ClearData();

    m_bModified = FALSE;
}

void CSHEngineTools::ClearData()
{
    // free constants, matrices, blenders
	// Blender
	for (BlenderPairIt b=m_Blenders.begin(); b!=m_Blenders.end(); b++)
	{
		xr_free			((LPSTR)b->first);
		xr_delete		(b->second);
	}
	m_Blenders.clear	();

	// Matrices
	for (MatrixPairIt m=m_Matrices.begin(); m!=m_Matrices.end(); m++){
		xr_free			((LPSTR)m->first);
		xr_delete		(m->second);
	}
	m_Matrices.clear	();

	// Constants
	for (ConstantPairIt c=m_Constants.begin(); c!=m_Constants.end(); c++){
		xr_free			((LPSTR)c->first);
		xr_delete		(c->second);
	}
	m_Constants.clear	();
}

void CSHEngineTools::OnFrame()
{
	if (m_bNeedResetShaders)
    	RealResetShaders();
    if (m_RemoteRenBlender){
    	RenameItem(m_RenBlenderOldName.c_str(),m_RenBlenderNewName.c_str());
        m_RemoteRenBlender=FALSE;
    }
	if (m_EditObject) m_EditObject->OnFrame();
}

void CSHEngineTools::OnRender()
{
	if (m_EditObject) m_EditObject->RenderSingle(Fidentity);
}

void CSHEngineTools::ZoomObject(bool bOnlySel)
{
	if (m_EditObject){
        Device.m_Camera.ZoomExtents(m_EditObject->GetBox());
    }else{
    	ISHTools::ZoomObject(bOnlySel);
    }
}
//---------------------------------------------------------------------------

void CSHEngineTools::RealResetShaders() 
{
	Ext.m_ItemProps->LockUpdating();
	// disable props vis update
    m_bFreezeUpdate 	= TRUE;
	// mem current blender
    AnsiString name;
    FHelper.MakeFullName(View()->Selected,0,name);

	UpdateObjectShader	();
	ResetCurrentItem	();
    // save to temp file
    PrepareRender		();
    // reset device shaders from temp file
    IReader data		(m_RenderShaders.pointer(), m_RenderShaders.size());
    Device.Reset		(&data,TRUE);
	// enable props vis update
    m_bFreezeUpdate 	= FALSE;

	// restore current shader
	SetCurrentItem		(name.c_str());

	Ext.m_ItemProps->UnlockUpdating();

    m_bNeedResetShaders	= FALSE;
}

void CSHEngineTools::FillItemList()
{
    View()->IsUpdating 		= true;
	ViewClearItemList();
	for (BlenderPairIt b=m_Blenders.begin(); b!=m_Blenders.end(); b++)
        ViewAddItem(b->first);
    View()->IsUpdating 		= false;
}

void CSHEngineTools::ApplyChanges(bool bForced)
{
    if (m_CurrentBlender&&(Ext.m_ItemProps->IsModified()||bForced)){
    	UpdateObjectFromStream();
		Ext.m_ItemProps->ResetModified();
	    ResetShaders();
    }
}

void CSHEngineTools::Reload()
{
	ViewClearItemList	();
    ResetCurrentItem	();
    ClearData			();
    Load				();
}

void CSHEngineTools::Load()
{
	AnsiString fn;
    FS.update_path				(fn,_game_data_,"shaders.xr");

    m_bFreezeUpdate				= TRUE;
	m_bLockUpdate 				= TRUE;

    if (FS.exist(fn.c_str())){
        IReader* F				= FS.r_open(fn.c_str());
        char					name[256];

        // Load constants
        {
            IReader*	fs		= F->open_chunk(0);
            while (fs&&!fs->eof())	{
                fs->r_stringZ	(name);
                CConstant*		C = xr_new<CConstant>();
                C->Load			(fs);
                m_Constants.insert(mk_pair(xr_strdup(name),C));
            }
            fs->close();
        }

        // Load matrices
        {
            IReader*	fs		= F->open_chunk(1);
            while (fs&&!fs->eof())	{
                fs->r_stringZ	(name);
                CMatrix*		M = xr_new<CMatrix>();
                M->Load			(fs);
                m_Matrices.insert(mk_pair(xr_strdup(name),M));
            }
            fs->close();
        }

        // Load blenders
        {
            IReader*	fs		= F->open_chunk(2);
            IReader*	chunk	= NULL;
            int			chunk_id= 0;

            while ((chunk=fs->open_chunk(chunk_id))!=NULL)
            {
                CBlender_DESC	desc;
                chunk->r		(&desc,sizeof(desc));
                CBlender*		B = CBlender::Create(desc.CLS);
				if	(B->getDescription().version != desc.version)
				{
					Msg			("! Version conflict in shader '%s'",desc.cName);
				}
                chunk->seek		(0);
                B->Load			(*chunk,desc.version);

                LPSTR blender_name = xr_strdup(desc.cName);
                std::pair<BlenderPairIt, bool> I =  m_Blenders.insert(mk_pair(blender_name,B));
                R_ASSERT2		(I.second,"shader.xr - found duplicate name!!!");
                chunk->close	();
                chunk_id++;
            }
            fs->close();
			FillItemList		();
        }

        UpdateRefCounters		();
        ResetCurrentItem		();
    }else{
    	ELog.DlgMsg(mtInformation,"Can't find file '%s'",fn.c_str());
    }
	m_bLockUpdate				= FALSE;
    m_bFreezeUpdate				= FALSE;
}

void CSHEngineTools::Save(CMemoryWriter& F)
{
    // Save constants
    {
    	F.open_chunk(0);
		for (ConstantPairIt c=m_Constants.begin(); c!=m_Constants.end(); c++){
        	F.w_stringZ(c->first);
        	c->second->Save(&F);
    	}
        F.close_chunk();
    }

    // Save matrices
    {
    	F.open_chunk(1);
		for (MatrixPairIt m=m_Matrices.begin(); m!=m_Matrices.end(); m++){
        	F.w_stringZ(m->first);
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
    // Save blender names
    {
    	F.open_chunk(3);
		F.w_u32(m_Blenders.size());
		for (BlenderPairIt b=m_Blenders.begin(); b!=m_Blenders.end(); b++)
        	F.w_stringZ(b->first);
        F.close_chunk();
    }
}

void CSHEngineTools::Save()
{
    // set name
	AnsiString fn;
    FS.update_path				(fn,_game_data_,"shaders.xr");

    // backup file
    EFS.BackupFile				(_game_data_,fn);

    // collapse reference
	CollapseReferences();

    // save to stream
    CMemoryWriter F;

    Save(F);

    // save new file
    EFS.UnlockFile				(0,fn.c_str(),false);
    F.save_to					(fn.c_str());
    EFS.LockFile				(0,fn.c_str(),false);

    m_bModified	= FALSE;
	Ext.m_ItemProps->ResetModified();
    // restore shader
    ResetShaders ();
}

void CSHEngineTools::PrepareRender()
{
	CollapseReferences();
    m_RenderShaders.clear();
    Save(m_RenderShaders);
}

CBlender* CSHEngineTools::FindItem(LPCSTR name){
	if (name && name[0]){
		LPSTR N = LPSTR(name);
		BlenderPairIt I = m_Blenders.find	(N);
		if (I==m_Blenders.end())return 0;
		else					return I->second;
    }else return 0;
}

CMatrix* CSHEngineTools::FindMatrix(LPSTR name, bool bDuplicate)
{
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

LPCSTR CSHEngineTools::GenerateItemName(LPSTR name, LPCSTR pref, LPCSTR source)
{
    int cnt = 0;
    if (source) strcpy(name,source); else sprintf(name,"%sshader_%02d",pref,cnt++);
	while (FindItem(name))
    	if (source) sprintf(name,"%s_%02d",source,cnt++);
        else sprintf(name,"%sshader_%02d",pref,cnt++);
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

LPCSTR CSHEngineTools::AppendItem(LPCSTR folder_name, LPCSTR parent_name)
{
	CBlender* parent 	= FindItem(parent_name);
	CLASS_ID cls_id;
    if (!parent){
        LPCSTR M=0;
        AStringVec lst;
        for (TemplateIt it=m_TemplatePalette.begin(); it!=m_TemplatePalette.end(); it++) lst.push_back((*it)->getComment());
        if (!TfrmChoseItem::SelectItem(TfrmChoseItem::smCustom,M,1,0,false,&lst)||!M) return 0;
        for (it=m_TemplatePalette.begin(); it!=m_TemplatePalette.end(); it++) 
        	if (0==strcmp((*it)->getComment(),M)){ 
            	cls_id = (*it)->getDescription().CLS;
                break;
            }
    }else{
		cls_id 	= parent->getDescription().CLS;
    }
    R_ASSERT2			(cls_id,"Invalid CLASS_ID.");
	// append blender
    CBlender* B 		= CBlender::Create(cls_id);
    // append matrix& constant
    CMemoryWriter M;
    if (parent) parent->Save(M); else B->Save(M);
	// parse data
    IReader data(M.pointer(), M.size());
    data.advance(sizeof(CBlender_DESC));
    DWORD type;
    char key[255];

    while (!data.eof()){
        int sz=0;
        type = data.r_u32();
        data.r_stringZ(key);
        switch(type){
        case xrPID_MARKER:	break;
        case xrPID_MATRIX:
        	sz=sizeof(string64);
            if (strcmp((LPSTR)data.pointer(),"$null")!=0){
	        	if (!parent) strcpy((LPSTR)data.pointer(),AppendMatrix());
    	        else AddMatrixRef((LPSTR)data.pointer());
            }
        break;
        case xrPID_CONSTANT:
        	sz=sizeof(string64);
            if (strcmp((LPSTR)data.pointer(),"$null")!=0){
	            if (!parent) strcpy((LPSTR)data.pointer(),AppendConstant());
    	        else AddConstantRef((LPSTR)data.pointer());
            }
        break;
        case xrPID_TEXTURE:	sz=sizeof(string64); 	break;
        case xrPID_INTEGER:	sz=sizeof(xrP_Integer);	break;
        case xrPID_FLOAT: 	sz=sizeof(xrP_Float); 	break;
        case xrPID_BOOL: 	sz=sizeof(xrP_BOOL); 	break;
        case xrPID_TOKEN: 	sz=sizeof(xrP_TOKEN)+sizeof(xrP_TOKEN::Item)*(((xrP_TOKEN*)data.pointer())->Count);	break;
        default: THROW2("xrPID_????");
        }
        data.advance(sz);
    }
    data.seek(0);
    B->Load(data,B->getDescription().version);
	// set name
    string128 name; name[0]=0;
    B->getDescription().Setup(GenerateItemName(name,folder_name,parent_name));
    // insert blender
	std::pair<BlenderPairIt, bool> I = m_Blenders.insert(mk_pair(xr_strdup(name),B));
	R_ASSERT2 		(I.second,"shader.xr - found duplicate name!!!");
    // insert to TreeView
	ViewAddItem		(name);

	ResetShaders	(true);
    SetCurrentItem	(name);
    Modified		();

    return B->getName();
}

void CSHEngineTools::RenameItem(LPCSTR old_full_name, LPCSTR new_full_name)
{
    ApplyChanges	();
    LPSTR N 		= LPSTR(old_full_name);
	BlenderPairIt I = m_Blenders.find	(N);
    R_ASSERT(I!=m_Blenders.end());
    xr_free			((LPSTR)I->first);
    CBlender* B 	= I->second;
	m_Blenders.erase(I);
	// rename
    B->getDescription().Setup(new_full_name);
	std::pair<BlenderPairIt, bool> RES = m_Blenders.insert(mk_pair(xr_strdup(new_full_name),B));
	R_ASSERT2 		(RES.second,"shader.xr - found duplicate name!!!");

	if (B==m_CurrentBlender) UpdateStreamFromObject();
    ApplyChanges	();
    ResetShaders	();
    if (B==m_CurrentBlender) SetCurrentItem(B->getName());
}

void CSHEngineTools::RenameItem(LPCSTR old_full_name, LPCSTR ren_part, int level)
{
    VERIFY(level<_GetItemCount(old_full_name,'\\'));
    char new_full_name[255];
    _ReplaceItem(old_full_name,level,ren_part,new_full_name,'\\');
	RenameItem(old_full_name,new_full_name);
}

void CSHEngineTools::AddMatrixRef(LPSTR name)
{
	CMatrix* M = FindMatrix(name,false); R_ASSERT(M);
    M->dwReference++;
}

void CSHEngineTools::AddConstantRef(LPSTR name)
{
	CConstant* C = FindConstant(name,false); R_ASSERT(C);
    C->dwReference++;
}

LPCSTR CSHEngineTools::AppendConstant(CConstant* src, CConstant** dest)
{
    CConstant* C = xr_new<CConstant>();
    if (src) *C = *src;
    C->dwReference = 1;
    char name[128];
    std::pair<ConstantPairIt, bool> I = m_Constants.insert(mk_pair(xr_strdup(GenerateConstantName(name)),C));
    VERIFY(I.second);
    if (dest) *dest = C;
    return I.first->first;
}

LPCSTR CSHEngineTools::AppendMatrix(CMatrix* src, CMatrix** dest)
{
    CMatrix* M = xr_new<CMatrix>();
    if (src) *M = *src;
    M->dwReference = 1;
    char name[128];
    std::pair<MatrixPairIt, bool> I = m_Matrices.insert(mk_pair(xr_strdup(GenerateMatrixName(name)),M));
    VERIFY(I.second);
    if (dest) *dest = M;
    return I.first->first;
}

void CSHEngineTools::RemoveItem(LPCSTR name)
{
	R_ASSERT(name && name[0]);
	CBlender* B = FindItem(name);
    R_ASSERT(B);
    // remove refs
	ParseBlender(B,ST_RemoveBlender);
	LPSTR N = LPSTR(name);                               
    BlenderPairIt I = m_Blenders.find	(N);
    xr_free 		((LPSTR)I->first);
    xr_delete		(I->second);
    m_Blenders.erase(I);

	ApplyChanges	();
	ResetShaders	();
}

void CSHEngineTools::RemoveMatrix(LPSTR name){
	if (*name=='$') return;
	R_ASSERT(name && name[0]);
	CMatrix* M = FindMatrix(name,false); VERIFY(M);
    M->dwReference--;
    if (M->dwReference==0){
		LPSTR N = LPSTR(name);
	    MatrixPairIt I = m_Matrices.find	(N);
    	xr_free 	((LPSTR)I->first);
	    xr_delete	(I->second);
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
    	xr_free 	((LPSTR)I->first);
	    xr_delete	(I->second);
	    m_Constants.erase(I);
    }
}

void CSHEngineTools::UpdateStreamFromObject(){
    m_BlenderStream.clear();
    if (m_CurrentBlender) m_CurrentBlender->Save(m_BlenderStream);
	// init properties
    UpdateProperties();
}

void CSHEngineTools::UpdateObjectFromStream()
{
    if (m_CurrentBlender){
        IReader data(m_BlenderStream.pointer(), m_BlenderStream.size());
        m_CurrentBlender->Load(data,m_CurrentBlender->getDescription().version);
    }
}

void CSHEngineTools::SetCurrentItem(LPCSTR name)
{
    if (m_bLockUpdate) return;

	CBlender* B = FindItem(name);
	if (m_CurrentBlender!=B){
        m_CurrentBlender = B;
        UpdateStreamFromObject();
        // apply this shader to non custom object
        UpdateObjectShader();
    }
	ViewSetCurrentItem(name);
}

void CSHEngineTools::ResetCurrentItem()
{
	m_CurrentBlender=0;
    UpdateStreamFromObject();
}

void CSHEngineTools::CollapseMatrix(LPSTR name)
{
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
    CMatrix* N = xr_new<CMatrix>(*M);
    N->dwReference=1;
	m_OptMatrices.insert(mk_pair(xr_strdup(name),N));
}

void CSHEngineTools::CollapseConstant(LPSTR name){
	if (*name=='$') return;
	R_ASSERT(name&&name[0]);
    CConstant* C = FindConstant(name,false); VERIFY(C);
	C->dwReference--;
    for (ConstantPairIt c=m_OptConstants.begin(); c!=m_OptConstants.end(); c++){
        if (c->second->Similar(*C)){
            strcpy(name,c->first);
            c->second->dwReference++;
            return;
        }
    }
    // append opt constant
    CConstant* N = xr_new<CConstant>(*C);
    N->dwReference=1;
	m_OptConstants.insert(mk_pair(xr_strdup(name),N));
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

void CSHEngineTools::ParseBlender(CBlender* B, CParseBlender& P)
{
    CMemoryWriter M;
    B->Save(M);

    IReader data(M.pointer(), M.size());
    data.advance(sizeof(CBlender_DESC));
    DWORD type;
    char key[255];

    while (!data.eof()){
        int sz=0;
        type = data.r_u32();
        data.r_stringZ(key);
        switch(type){
        case xrPID_MARKER:							break;
        case xrPID_MATRIX:	sz=sizeof(string64); 	break;
        case xrPID_CONSTANT:sz=sizeof(string64); 	break;
        case xrPID_TEXTURE: sz=sizeof(string64); 	break;
        case xrPID_INTEGER: sz=sizeof(xrP_Integer);	break;
        case xrPID_FLOAT: 	sz=sizeof(xrP_Float); 	break;
        case xrPID_BOOL: 	sz=sizeof(xrP_BOOL); 	break;
		case xrPID_TOKEN: 	sz=sizeof(xrP_TOKEN)+sizeof(xrP_TOKEN::Item)*(((xrP_TOKEN*)data.pointer())->Count); break;
        default: THROW2("xrPID_????");
        }
        P.Parse(this,type, key, data.pointer());
        data.advance(sz);
    }

    data.seek(0);
    B->Load(data,B->getDescription().version);
}

void CSHEngineTools::CollapseReferences()
{
	for (BlenderPairIt b=m_Blenders.begin(); b!=m_Blenders.end(); b++)
    	ParseBlender(b->second,ST_CollapseBlender);
	for (MatrixPairIt m=m_Matrices.begin(); m!=m_Matrices.end(); m++){
		VERIFY			(m->second->dwReference==0);
		xr_free			((LPSTR)m->first);
		xr_delete		(m->second);
	}
	for (ConstantPairIt c=m_Constants.begin(); c!=m_Constants.end(); c++){
    	VERIFY			(c->second->dwReference==0);
		xr_free			((LPSTR)c->first);
		xr_delete		(c->second);
	}
	m_Matrices.clear	();
	m_Constants.clear	();
	m_Matrices 			= m_OptMatrices;
    m_Constants 		= m_OptConstants;
	m_OptConstants.clear();
    m_OptMatrices.clear	();
}

void CSHEngineTools::UpdateRefCounters()
{
	for (BlenderPairIt b=m_Blenders.begin(); b!=m_Blenders.end(); b++)
    	ParseBlender(b->second,ST_UpdateBlenderRefs);
}

void CSHEngineTools::OnDeviceCreate()
{
	ResetShaders		();
}

void __fastcall CSHEngineTools::PreviewObjClick(TObject *Sender)
{
	int p = dynamic_cast<TMenuItem*>(Sender)->Tag;
    LPCSTR fn;
    m_bCustomEditObject	= false;
    switch(p){
        case 0: fn="editor\\ShaderTest_Plane"; 	break;
        case 1: fn="editor\\ShaderTest_Box"; 	break;
        case 2: fn="editor\\ShaderTest_Sphere"; break;
        case 3: fn="editor\\ShaderTest_Teapot";	break;
        case 4: fn=0;							break;
        case -1: fn=m_EditObject?m_EditObject->GetName():""; if (!TfrmChoseItem::SelectItem(TfrmChoseItem::smObject,fn)) return; m_bCustomEditObject = true; break;
        case -2: fn=0; 							break;
        default: THROW2("Failed select test object.");
    }
    Lib.RemoveEditObject(m_EditObject);
    if (fn){
        m_EditObject = Lib.CreateEditObject(fn);
        if (!m_EditObject)
            ELog.DlgMsg(mtError,"Object '%s.object' can't find in object library. Preview disabled.",fn);
    }
    ZoomObject(false);
    UpdateObjectShader();
    UI.RedrawScene();
}
//---------------------------------------------------------------------------

void CSHEngineTools::UpdateObjectShader()
{
    // apply this shader to non custom object
	if (m_EditObject&&!m_bCustomEditObject){
    	CSurface* surf = *m_EditObject->FirstSurface(); R_ASSERT(surf);
        string512 tex; strcpy(tex,surf->_Texture());
        for (int i=0; i<7; i++){ strcat(tex,","); strcat(tex,surf->_Texture());}
        if (m_CurrentBlender)	surf->SetShader(m_CurrentBlender->getName());
        else					surf->SetShader("editor\\wire");
        UI.RedrawScene();
		m_EditObject->OnDeviceDestroy();
    }
}

void CSHEngineTools::OnShowHint(AStringVec& ss)
{
	if (m_EditObject){
	    Fvector p;
        float dist=UI.ZFar();
        SRayPickInfo pinf;
    	if (m_EditObject->RayPick(dist,UI.m_CurrentRStart,UI.m_CurrentRNorm,Fidentity,&pinf)){
        	R_ASSERT(pinf.e_mesh);
            CSurface* surf=pinf.e_mesh->GetSurfaceByFaceID(pinf.inf.id);
            ss.push_back(AnsiString("Surface: ")+AnsiString(surf->_Name()));
            ss.push_back(AnsiString("Texture: ")+AnsiString(surf->_Texture()));
            ss.push_back(AnsiString("Shader: ")+AnsiString(surf->_ShaderName()));
            ss.push_back(AnsiString("LC Shader: ")+AnsiString(surf->_ShaderXRLCName()));
            ss.push_back(AnsiString("Game Mtl: ")+AnsiString(surf->_GameMtlName()));
            ss.push_back(AnsiString("2 Sided: ")+AnsiString(surf->m_Flags.is(CSurface::sf2Sided)?"on":"off"));
        }
    }
}


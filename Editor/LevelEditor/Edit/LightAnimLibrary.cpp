//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "LightAnimLibrary.h"
//---------------------------------------------------------------------------
#define CHUNK_ITEM_LIST		0x0001
//---------------------------------------------------------------------------
#define CHUNK_ITEM_COMMON	0x0001
#define CHUNK_ITEM_KEYS		0x0002
//---------------------------------------------------------------------------

ELightAnimLibrary LALib;

CLAItem::CLAItem()
{
	cName[0]	= 0;
	fFPS		= 15.f;
	iFrameCount	= 1;
}

void CLAItem::InitDefault(){
	Keys[0]		= 0x00000000;
	Keys[1]		= 0x00000000;
}

void CLAItem::Load(CStream& F)
{
	R_ASSERT(F.FindChunk(CHUNK_ITEM_COMMON));
    F.RstringZ		(cName);
    fFPS			= F.Rfloat();
    iFrameCount		= F.Rdword();

    int key_cnt,key;
	R_ASSERT(F.FindChunk(CHUNK_ITEM_KEYS));
	key_cnt			= F.Rdword();
    for (int i=0; i<key_cnt; i++){
    	key			= F.Rdword	();
        Keys[key]	= F.Rdword();
    }
}

void CLAItem::Save(CFS_Base& F)
{
    F.open_chunk	(CHUNK_ITEM_COMMON);
    F.WstringZ		(cName);
    F.Wfloat		(fFPS);
    F.Wdword		(iFrameCount);
	F.close_chunk	();

    F.open_chunk	(CHUNK_ITEM_KEYS);
    F.Wdword		(Keys.size());
    for (KeyPairIt it=Keys.begin(); it!=Keys.end(); it++){
		F.Wdword	(it->first);
		F.Wdword	(it->second);
    }
	F.close_chunk	();
}

void CLAItem::InsertKey(int frame, DWORD color)
{
	R_ASSERT(frame<=iFrameCount);
	Keys[frame] = color;
}

void CLAItem::DeleteKey(int frame)
{
	R_ASSERT(frame<=iFrameCount);
	KeyPairIt it=Keys.find(frame);
    if (it!=Keys.end()) Keys.erase(it);
}

void CLAItem::MoveKey(int from, int to)
{
	R_ASSERT(from<=iFrameCount);
	R_ASSERT(to<=iFrameCount);
	KeyPairIt it=Keys.find(from);
    if (it!=Keys.end()){
    	Keys[to]=it->second;
		Keys.erase(it);
    }
}

void CLAItem::Resize(int new_len)
{
	VERIFY((new_len>=1));
	if (new_len!=iFrameCount){
    	if (new_len>iFrameCount){
        	int old_len=iFrameCount;
			iFrameCount = new_len;
        	MoveKey(old_len,new_len);
        }else{
        	DWORD clr=Interpolate(new_len-1);
			pair<KeyPairIt, KeyPairIt> I = Keys.equal_range(new_len-1);
            if (I.first!=Keys.end()){
	            if (I.first->first<new_len) I.first++;
    	        Keys.erase(I.first, Keys.end());
				iFrameCount = new_len;
        	    InsertKey(new_len,clr);
            }
        }
    }
}

DWORD CLAItem::Interpolate(int frame)
{
    R_ASSERT(frame<=iFrameCount);
	pair<KeyPairIt, KeyPairIt> I = Keys.equal_range(frame);

    Fcolor c, c0, c1;
    int a0=I.first->first;
    int a1=I.second->first;
	R_ASSERT(I.first!=Keys.end());
    c0.set(I.first->second);
    c1.set(I.second->second);
    float t = float(frame)/float(I.second->first-I.first->first);
    c.lerp(c0,c1,t);

	return c.get();
}

DWORD CLAItem::Calculate(float T)
{
    float	t	= Device.fTimeGlobal;
    float 	l	= float(iFrameCount)/fFPS;
    return Interpolate(fmod(t,l)*fFPS);
}

ELightAnimLibrary::ELightAnimLibrary()
{
}

ELightAnimLibrary::~ELightAnimLibrary()
{
}

void ELightAnimLibrary::OnCreate()
{
	Load();
}

void ELightAnimLibrary::OnDestroy()
{
	for (LAItemIt it=Items.begin(); it!=Items.end(); it++)
    	_DELETE(*it);
}

void ELightAnimLibrary::Load()
{
	AnsiString fn="lanims.xr";
    Engine.FS.m_ServerRoot.Update(fn);
	CStream* fs=Engine.FS.Open(fn.c_str());
    if (fs){
        CStream* OBJ = fs->OpenChunk(CHUNK_ITEM_LIST);
        if (OBJ){
	        CStream* O   = OBJ->OpenChunk(0);
    	    for (int count=1; O; count++) {
        	    CLAItem* I = new CLAItem();
                I->Load(*O);
                Items.push_back(I);
            	O->Close();
	            O = OBJ->OpenChunk(count);
    	    }
	        OBJ->Close();
        }

		Engine.FS.Close(fs);
    }
}

void ELightAnimLibrary::Save()
{
	CFS_Memory F;
    F.open_chunk	(CHUNK_ITEM_LIST);
    int count = 0;
	for (LAItemIt it=Items.begin(); it!=Items.end(); it++){
        F.open_chunk(count++);
		(*it)->Save		(F);
        F.close_chunk();
    }
	F.close_chunk	();

	AnsiString fn="lanims.xr";
    Engine.FS.m_ServerRoot.Update(fn);
    F.SaveTo(fn.c_str(),0);
}

void ELightAnimLibrary::Reload()
{
}

CLAItem* ELightAnimLibrary::FindItem(LPCSTR name)
{
	for (LAItemIt it=Items.begin(); it!=Items.end(); it++)
    	if (0==strcmp((*it)->cName,name)) return *it;
    return 0;
}

LPCSTR ELightAnimLibrary::GenerateName(LPSTR name, LPCSTR source)
{
    int cnt = 0;
	char fld[128]; strcpy(fld,name);
    if (source) strcpy(name,source); else sprintf(name,"%sanim_%02d",fld,cnt++);
	while (FindItem(name))
    	if (source) sprintf(name,"%s_%02d",source,cnt++);
        else sprintf(name,"%sanim_%02d",fld,cnt++);
	return name;
}

CLAItem* ELightAnimLibrary::AppendItem(LPCSTR folder_name, CLAItem* parent)
{
    char old_name[128]; if (parent) strcpy(old_name,parent->cName);
	CLAItem* I=new CLAItem();
    I->InitDefault();
    char new_name[128]; new_name[0]=0;
    if (folder_name) strcpy(new_name,folder_name);
    GenerateName(new_name,parent?old_name:0);
    strcpy(I->cName,new_name);
    Items.push_back(I);
	return I;
}

void ELightAnimLibrary::DeleteItem(LPCSTR name)
{
}


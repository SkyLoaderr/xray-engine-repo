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
	fFPS		= 15.f;
	iFrameCount	= 1;
}

void CLAItem::InitDefault(){
	Keys[0]		= 0x00000000;
}

void CLAItem::Load(IReader& F)
{
	R_ASSERT(F.find_chunk(CHUNK_ITEM_COMMON));
    F.r_stringZ		(cName);
    fFPS			= F.r_float();
    iFrameCount		= F.r_u32();

    int key_cnt,key;
	R_ASSERT(F.find_chunk(CHUNK_ITEM_KEYS));
	key_cnt			= F.r_u32();
    for (int i=0; i<key_cnt; i++){
    	key			= F.r_u32	();
        Keys[key]	= F.r_u32();
    }
}

void CLAItem::Save(IWriter& F)
{
    F.open_chunk	(CHUNK_ITEM_COMMON);
    F.w_stringZ		(cName);
    F.w_float		(fFPS);
    F.w_u32		(iFrameCount);
	F.close_chunk	();

    F.open_chunk	(CHUNK_ITEM_KEYS);
    F.w_u32		(Keys.size());
    for (KeyPairIt it=Keys.begin(); it!=Keys.end(); it++){
		F.w_u32	(it->first);
		F.w_u32	(it->second);
    }
	F.close_chunk	();
}

void CLAItem::InsertKey(int frame, u32 color)
{
	R_ASSERT(frame<=iFrameCount);
	Keys[frame] = color;
}

void CLAItem::DeleteKey(int frame)
{
	R_ASSERT(frame<=iFrameCount);
    if (0==frame) return;
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
			KeyPairIt I = Keys.upper_bound(new_len-1);
            if (I!=Keys.end()) Keys.erase(I, Keys.end());
            iFrameCount = new_len;
        }
    }
}

u32 CLAItem::Interpolate(int frame)
{
    R_ASSERT(frame<=iFrameCount);

    KeyPairIt A=Keys.find(frame);
    KeyPairIt B;
    if (A!=Keys.end()){  			// ���� - ���������� ���� �����
    	return A->second;
    }else{							// �� ����
    	B=Keys.upper_bound(frame);  // ���� ��������� ����
        if (B==Keys.end()){			// ���� ��� ��� ������ ���� ���������� �����
	    	B--;
        	return B->second;
        }
    	A=B;        				// ����� � A ������� ���������� ����
        A--;
    }

    R_ASSERT(Keys.size()>1);
    // ������������� ����
    Fcolor c, c0, c1;
    float a0=(float)A->first;
    float a1=(float)B->first;
    c0.set(A->second);
    c1.set(B->second);
    float t = float(frame-a0)/float(a1-a0);
    c.lerp(c0,c1,t);
    return c.get();
}

u32 CLAItem::Calculate(float T, int& frame)
{
    frame	= iFloor(fmodf(Device.fTimeGlobal,float(iFrameCount)/fFPS)*fFPS);
    return Interpolate(frame);
}

int CLAItem::PrevKeyFrame(int frame)
{
    KeyPairIt A=Keys.lower_bound(frame);
    if (A!=Keys.end()){
    	KeyPairIt B=A; B--;
        if (B!=Keys.end()) return B->first;
        return A->first;
    }else{
    	return Keys.rbegin()->first;
    }
}

int CLAItem::NextKeyFrame(int frame)
{
    KeyPairIt A=Keys.upper_bound(frame);
    if (A!=Keys.end()){
        return A->first;
    }else{
    	return Keys.rbegin()->first;
    }
}

//------------------------------------------------------------------------------
// Library
//------------------------------------------------------------------------------
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
	Unload();
}

void ELightAnimLibrary::Unload()
{
	for (LAItemIt it=Items.begin(); it!=Items.end(); it++)
    	xr_delete(*it);
    Items.clear();
}

void ELightAnimLibrary::Load()
{
	string256 fn;
    FS.update_path(fn,_game_data_,"lanims.xr");
	IReader* fs=FS.r_open(fn);
    if (fs){
        IReader* OBJ = fs->open_chunk(CHUNK_ITEM_LIST);
        if (OBJ){
	        IReader* O   = OBJ->open_chunk(0);
    	    for (int count=1; O; count++) {
        	    CLAItem* I = xr_new<CLAItem>();
                I->Load(*O);
                Items.push_back(I);
            	O->close();
	            O = OBJ->open_chunk(count);
    	    }
	        OBJ->close();
        }

		FS.r_close(fs);
    }
}

void ELightAnimLibrary::Save()
{
	CMemoryWriter F;
    F.open_chunk	(CHUNK_ITEM_LIST);
    int count = 0;
	for (LAItemIt it=Items.begin(); it!=Items.end(); it++){
        F.open_chunk(count++);
		(*it)->Save		(F);
        F.close_chunk();
    }
	F.close_chunk	();

	string256 fn;
    FS.update_path(fn,_game_data_,"lanims.xr");
    F.save_to(fn);
}

void ELightAnimLibrary::Reload()
{
	Unload();
	Load();
}

LAItemIt ELightAnimLibrary::FindItemI(LPCSTR name)
{
	if (name&&name[0])
		for (LAItemIt it=Items.begin(); it!=Items.end(); it++)
			if (0==xr_strcmp((*it)->cName,name)) return it;
	return Items.end();
}

CLAItem* ELightAnimLibrary::FindItem(LPCSTR name)
{
	LAItemIt it=FindItemI(name);
	return (it!=Items.end())?*it:0;
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
    CLAItem* I=xr_new<CLAItem>();
    I->InitDefault();
    string256 new_name; new_name[0]=0;
    if (folder_name) strcpy(new_name,folder_name);
    GenerateName(new_name,parent?*parent->cName:0);
    I->cName		= new_name;
    Items.push_back	(I);
	return I;
}
#ifdef _EDITOR
BOOL ELightAnimLibrary::RemoveObject(LPCSTR _fname, EItemType type)   
{
	if (TYPE_FOLDER==type){
		return TRUE;
    }else if (TYPE_OBJECT==type){
        LAItemIt it=FindItemI(_fname); 
        if (it!=Items.end()){
	        xr_delete(*it);
    	    Items.erase(it);
            return TRUE;
        }
    }else THROW;
    return FALSE;
}
//---------------------------------------------------------------------------

void ELightAnimLibrary::RenameObject(LPCSTR nm0, LPCSTR nm1, EItemType type)
{
	if (TYPE_FOLDER==type){
    }else if (TYPE_OBJECT==type){
		CLAItem* I=FindItem(nm0); R_ASSERT(I);
		strcpy(I->cName,nm1);
	}
}
//---------------------------------------------------------------------------
#endif
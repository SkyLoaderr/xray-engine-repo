// LocatorAPI.cpp: implementation of the CLocatorAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "LocatorAPI_borland.h"

struct fl_mask{
	bool 	b_cmp_nm;
    bool	b_cmp_ext;
	ref_str nm;
    ref_str ext;
    fl_mask():b_cmp_nm(false),b_cmp_ext(false){};
};

DEFINE_VECTOR(fl_mask,FLMaskVec,FLMaskVecIt);
int CLocatorAPI::file_list(FS_QueryMap& dest, LPCSTR path, u32 flags, LPCSTR mask)
{
	R_ASSERT		(path);
	VERIFY			(flags);
	// проверить нужно ли пересканировать пути
    check_pathes	();

	AnsiString		N;
	if (FS.path_exist(path))	update_path(N,path,"");
    else						N=path;

	file			desc;
	desc.name		= N.c_str();
	files_it	I 	= files.find(desc);
	if (I==files.end())	return 0;

    BOOL b_mask 	= FALSE;
	FLMaskVec 		masks;
	if (mask){
		int cnt		= _GetItemCount(mask);
		string64	buf;
		for (int k=0; k<cnt; k++){
        	_GetItem(mask,k,buf);
            fl_mask item;
        	// name
			string64 nm;
        	_GetItem(buf,0,nm,'.'); 
			if (nm&&nm[0]&&(nm[0]!='*'))	{	item.nm=nm; item.b_cmp_nm=true;		}
			// extension
            LPCSTR ext	= strext(buf);
			if (ext&&ext[0]&&(ext[1]!='*'))	{	item.ext=ext; item.b_cmp_ext=true; 	}
            masks.push_back(item);
        }
        b_mask		= !masks.empty();
	}

	size_t base_len	= N.Length();
	for (++I; I!=files.end(); I++)
	{
		const file& entry = *I;
		if (0!=strncmp(entry.name,N.c_str(),base_len))	break;	// end of list
		LPCSTR end_symbol = entry.name+strlen(entry.name)-1;
		if ((*end_symbol) !='\\')	{
			// file
			if ((flags&FS_ListFiles) == 0)	continue;
			LPCSTR entry_begin 		= entry.name+base_len;
			if ((flags&FS_RootOnly)&&strstr(entry_begin,"\\"))	continue;	// folder in folder
			// check extension
			if (b_mask){
                string128 tmp;
                ref_str nm 			= _GetItem(entry_begin,0,tmp,'.');
				ref_str ext 		= strext(entry_begin);
                bool bOK			= false;
                for (FLMaskVecIt it=masks.begin(); it!=masks.end(); it++){
                	bool bNM		= true;
                    bool bEXT		= true;
                    if (it->b_cmp_nm)	if (nm!=it->nm) 	bNM =false;
                    if (it->b_cmp_ext)	if (ext!=it->ext) 	bEXT=false;
                    bOK				= bNM&&bEXT;
                    if (bOK)		break;
                }
                if (!bOK)			continue;
			}
			AnsiString fn			= entry_begin;
			// insert file entry
			if (flags&FS_ClampExt)	fn = ChangeFileExt(fn,"");
			u32 fl = (entry.vfs!=0xffffffff?FS_QueryItem::flVFS:0);
			dest.insert(mk_pair(fn.c_str(),FS_QueryItem(entry.size_real,entry.modif,fl)));
		} else {
			// folder
			if ((flags&FS_ListFolders) == 0)continue;
			LPCSTR entry_begin 		= entry.name+base_len;

			if ((flags&FS_RootOnly)&&(strstr(entry_begin,"\\")!=end_symbol))	continue;	// folder in folder
			u32 fl = FS_QueryItem::flSubDir|(entry.vfs?FS_QueryItem::flVFS:0);
			dest.insert(mk_pair(entry_begin,FS_QueryItem(entry.size_real,entry.modif,fl)));
		}
	}
	return dest.size();
}

bool CLocatorAPI::file_find(FS_QueryItem& dest, LPCSTR path, LPCSTR name, bool clamp_ext)
{
	R_ASSERT			(path);
	// проверить нужно ли пересканировать пути
    check_pathes		();

	AnsiString			N;
	if (FS.path_exist(path))	update_path(N,path,name);
    else						N=AnsiString(path)+name;

	file				desc;
	desc.name			= N.c_str();
	files_it	I 		= files.find(desc);
	if (I==files.end())	return false;

	size_t base_len		= N.Length();
    const file& entry 	= *I;
    AnsiString fn		= (clamp_ext)?ChangeFileExt(entry.name+base_len,""):AnsiString(entry.name+base_len);
    u32 fl 				= (entry.vfs!=0xffffffff?FS_QueryItem::flVFS:0);
    dest.set			(entry.size_real,entry.modif,fl);
    return true;
}

static TRTLCriticalSection CS;
//---------------------------------------------------------------------------
// TShellChangeThread -------------------------------------------------------
__fastcall TShellChangeThread::TShellChangeThread():TThread(true)
{
    FreeOnTerminate 	= false; 
//    FWaitChanged 		= false;
    FMutex 				= CreateMutex(NULL, true /* initial owner - must be Release'd by this thread*/, NULL);
    if (FMutex)
        WaitForSingleObject(FMutex, INFINITE);
}
//---------------------------------------------------------------------------

__fastcall TShellChangeThread::~TShellChangeThread(void)
{
//    FWaitChanged 		= false;

	for (PathIt it=events.begin(); it!=events.end(); it++){
    	Path& P			= *it;
    	P.FChangeEvent	= 0;
        if (P.FWaitHandle != INVALID_HANDLE_VALUE){
            HANDLE hOld 	= P.FWaitHandle;
            P.FWaitHandle= 0;
            FindCloseChangeNotification(hOld);
        }
    }
	events.clear		();
}
//---------------------------------------------------------------------------

void TShellChangeThread::RegisterPath(FS_Path& path)
{                     
	R_ASSERT2(Suspended,"Can't register path. Thread already started.");
	for (PathIt it=events.begin(); it!=events.end(); it++)
    	if ((it->FDirectory==path.m_Path)&&(it->bRecurse==path.m_Flags.is(FS_Path::flRecurse))) return;

    events.push_back	(Path());
    Path& P				= events.back();
    P.FDirectory		= path.m_Path;
    P.bRecurse			= path.m_Flags.is(FS_Path::flRecurse);
    P.FChangeEvent		= path.rescan_path_cb;
    P.FWaitHandle 		= INVALID_HANDLE_VALUE;
}
//---------------------------------------------------------------------------

void __fastcall TShellChangeThread::Execute(void)
{
    EnterCriticalSection(&CS);
	for (PathIt it=events.begin(); it!=events.end(); it++){
    	Path& P			= *it;
        P.FWaitHandle	= FindFirstChangeNotification(P.FDirectory.c_str(), P.bRecurse, FNotifyOptionFlags);
        R_ASSERT3		(P.FWaitHandle != INVALID_HANDLE_VALUE,"Can't create notify handle",P.FDirectory.c_str());
    }
    LeaveCriticalSection(&CS);
//	if (FWaitHandle == INVALID_HANDLE_VALUE)
//		return;

    while(!Terminated)
    {
        HANDLEVec hHandles;
        hHandles.push_back(FMutex);
		for (PathIt it=events.begin(); it!=events.end(); it++)
        	hHandles.push_back(it->FWaitHandle);

        int Obj = WaitForMultipleObjects(hHandles.size(), hHandles.begin(), false, INFINITE);
        if (Obj==WAIT_OBJECT_0){
            ReleaseMutex(FMutex);
            break;
        }else if (Obj>WAIT_OBJECT_0){
        	u32 idx		= Obj-WAIT_OBJECT_0-1;
            if (idx<events.size()){
	            Path& P		= events[idx];
    	        if (P.FChangeEvent)	Synchronize(P.FChangeEvent);
        	    if (P.FWaitHandle) 	FindNextChangeNotification(P.FWaitHandle);
            }
        }else 
        	return;
/*
        if(FWaitChanged)
        {
            EnterCriticalSection(&CS);
            FWaitHandle = FindFirstChangeNotification(FDirectory.c_str(),
                FWatchSubTree, FNotifyOptionFlags);
            FWaitChanged = false;
            LeaveCriticalSection(&CS);
        }
*/
    }
}
//---------------------------------------------------------------------------

void CLocatorAPI::SetEventNotification()
{
	InitializeCriticalSection(&CS);
    FThread 		= new TShellChangeThread();
	FThread->FNotifyOptionFlags	= FILE_NOTIFY_CHANGE_FILE_NAME|FILE_NOTIFY_CHANGE_DIR_NAME|FILE_NOTIFY_CHANGE_LAST_WRITE;
	for(PathPairIt p_it=pathes.begin(); p_it!=pathes.end(); p_it++)
    	if (p_it->second->m_Flags.is(FS_Path::flNotif)) FThread->RegisterPath(*p_it->second);
    FThread->Resume();
}
 
void CLocatorAPI::ClearEventNotification()
{
    if (FThread)
    {
        FThread->Terminate();
        ReleaseMutex(FThread->FMutex); // this current thread must release the mutex
        xr_delete(FThread);
    }
	DeleteCriticalSection(&CS);
}

void CLocatorAPI::lock_rescan()
{
	m_Flags.set(flLockRescan,TRUE);
}

void CLocatorAPI::unlock_rescan()
{
	m_Flags.set(flLockRescan,FALSE);
	if (m_Flags.is(flNeedRescan)) 
    	rescan_pathes();
}


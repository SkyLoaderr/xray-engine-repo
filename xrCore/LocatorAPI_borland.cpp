// LocatorAPI.cpp: implementation of the CLocatorAPI class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#pragma hdrstop

#include "LocatorAPI_borland.h"

int CLocatorAPI::file_list(FS_QueryMap& dest, LPCSTR initial, u32 flags, LPCSTR ext_mask)
{
	VERIFY			(flags);
	// проверить нужно ли пересканировать пути
    check_pathes	();

	string256		N;
	if (initial&&initial[0]) update_path(N,initial,"");

	file			desc;
	desc.name		= N;
	files_it	I 	= files.find(desc);
	if (I==files.end())	return 0;

	LPSTRVec exts;
	if (ext_mask){
		int cnt		= _GetItemCount(ext_mask);
		string32	buf;
		for (int k=0; k<cnt; k++)
			exts.push_back(xr_strdup(_GetItem(ext_mask,k,buf)));
	}

	size_t base_len	= strlen(N);
	for (++I; I!=files.end(); I++)
	{
		const file& entry = *I;
		if (0!=strncmp(entry.name,N,base_len))	break;	// end of list
		const char* end_symbol = entry.name+strlen(entry.name)-1;
		if ((*end_symbol) !='\\')	{
			// file
			if ((flags&FS_ListFiles) == 0)	continue;
			LPCSTR entry_begin 		= entry.name+base_len;
            LPCSTR a = strstr(entry_begin,"\\");
            bool b = flags&FS_RootOnly;
			if ((flags&FS_RootOnly)&&strstr(entry_begin,"\\"))	continue;	// folder in folder
			// check extension
			if (ext_mask){
				LPCSTR ext 			= strext(entry_begin);
				if (ext){
					bool bFound			= false;
					for (LPSTRIt it=exts.begin(); it!=exts.end(); it++)
						if (0==strcmp(ext,*it)) bFound=true;
					if (!bFound)		continue;
				}
			}
			AnsiString fn			= entry_begin;
			// insert file entry
			if (flags&FS_ClampExt)	fn = ChangeFileExt(fn,"");
			u32 fl = (entry.vfs?FS_QueryItem::flVFS:0);
			dest.insert(make_pair(fn,FS_QueryItem(entry.size,entry.modif,fl)));
		} else {
			// folder
			if ((flags&FS_ListFolders) == 0)continue;
			const char* entry_begin = entry.name+base_len;

			if ((flags&FS_RootOnly)&&(strstr(entry_begin,"\\")!=end_symbol))	continue;	// folder in folder
			u32 fl = FS_QueryItem::flSubDir|(entry.vfs?FS_QueryItem::flVFS:0);
			dest.insert(make_pair(entry_begin,FS_QueryItem(entry.size,entry.modif,fl)));
		}
	}
	for (LPSTRIt it=exts.begin(); it!=exts.end(); it++)
		xr_free(*it);
	return dest.size();
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
            R_ASSERT2	(idx<events.size(),"Index out of bounds.");
            Path& P		= events[idx];
            if (P.FChangeEvent)	Synchronize(P.FChangeEvent);
            if (P.FWaitHandle) 	FindNextChangeNotification(P.FWaitHandle);
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


#ifndef LocatorAPI_borlandH
#define LocatorAPI_borlandH
#pragma once

class TShellChangeThread : public Classes::TThread
{
	typedef Classes::TThread inherited;
    friend class TCustomShellChangeNotifier;
private:
    struct Path{
    	AnsiString			FDirectory;
        HANDLE 				FWaitHandle;
        TThreadMethod 		FChangeEvent;
		bool 				bRecurse;
    };
    DEFINE_VECTOR(HANDLE,HANDLEVec,HANDLEIt);
    DEFINE_VECTOR(Path,PathVec,PathIt);
    PathVec					events;
//	bool 					FWaitChanged;
public:
	HANDLE 					FMutex;
	unsigned 				FNotifyOptionFlags;
protected:
	virtual void __fastcall Execute();
public:
	__fastcall virtual 		TShellChangeThread	();
	__fastcall virtual 		~TShellChangeThread	();
    void					RegisterPath		(FS_Path& path);
};

#endif // LocatorAPI_borlandH

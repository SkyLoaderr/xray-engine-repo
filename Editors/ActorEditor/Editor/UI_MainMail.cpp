//---------------------------------------------------------------------------
#include "stdafx.h"
#pragma hdrstop

#include "ui_main.h"
#include "xr_trims.h"
//---------------------------------------------------------------------------

void TUI::CreateMailslot()
{
	AnsiString slot_name = AnsiString("\\\\.\\mailslot\\")+AnsiString(_EDITOR_FILE_NAME_);
    hMailSlot = ::CreateMailslot(slot_name.c_str(),
        0,                             // no maximum message size
        MAILSLOT_WAIT_FOREVER,         // no time-out for operations
        (LPSECURITY_ATTRIBUTES) NULL); // no security attributes
    R_ASSERT2(hMailSlot != INVALID_HANDLE_VALUE,"Can't create mailslot");
}
//---------------------------------------------------------------------------

void TUI::CheckMailslot()
{
    DWORD cbMessage, cMessage, cbRead;
	BOOL fResult;
	LPSTR lpszBuffer; 

    cbMessage = cMessage = cbRead = 0;
    fResult = GetMailslotInfo(hMailSlot,	// mailslot handle
        (LPDWORD) NULL,						// no maximum message size
        &cbMessage,							// size of next message
        &cMessage,							// number of messages
        (LPDWORD) NULL);					// no read time-out
	R_ASSERT2(fResult,"Can't get mailslot info");
	if (cbMessage == MAILSLOT_NO_MESSAGE) return;
    while (cMessage != 0)  // retrieve all messages    
	{ 
		// Allocate memory for the message.  
		lpszBuffer = (LPSTR) GlobalAlloc(GPTR, cbMessage);          
		lpszBuffer[0] = '\0'; 
		fResult = ReadFile(hMailSlot, lpszBuffer, cbMessage, &cbRead, (LPOVERLAPPED) NULL);  
		if (!fResult) {
			GlobalFree((HGLOBAL) lpszBuffer);
			throw Exception("Can't ReadFile");
		}
		OnReceiveMail(lpszBuffer);
		GlobalFree((HGLOBAL) lpszBuffer);
		fResult = GetMailslotInfo(hMailSlot,	// mailslot handle 
			(LPDWORD) NULL,							// no maximum message size 
			&cbMessage,								// size of next message 
			&cMessage,								// number of messages 
			(LPDWORD) NULL);						// no read time-out  
		R_ASSERT2(fResult,"Can't get mailslot info");
    }
}
//---------------------------------------------------------------------------

void TUI::OnReceiveMail(LPCSTR msg)
{
	int cnt = _GetItemCount(msg,' ');
    if (cnt){
        AnsiString M=AnsiString(msg).LowerCase();
        AnsiString p[2];
		_GetItem(msg,0,p[0],' ');
        if (cnt>1) _GetItems(msg,1,cnt,p[1],' ');
        if (p[0]=="exit"){
        	ELog.DlgMsg(mtInformation,"'%s EDITOR': Critical update!",AnsiString(_EDITOR_FILE_NAME_).UpperCase());
            while (1){
            	if (Command(COMMAND_EXIT)){
	                Command(COMMAND_QUIT);
                    break;
                }
            }
        }else if (p[0]=="quit"){ 
        	ELog.Msg(mtInformation,"'%s EDITOR': Super critical update!",AnsiString(_EDITOR_FILE_NAME_).UpperCase());
        	Command(COMMAND_SAVE_BACKUP);
        	Command(COMMAND_QUIT);
        }else if (p[0]=="info"){
        	if (cnt>1) ELog.DlgMsg(mtInformation,"'%s EDITOR': %s",AnsiString(_EDITOR_FILE_NAME_).UpperCase(),p[1].c_str());
        }else if (p[0]=="error"){
        	if (cnt>1) ELog.DlgMsg(mtError,"'%s EDITOR': %s",AnsiString(_EDITOR_FILE_NAME_).UpperCase(),p[1].c_str());
        }
    }
}
//---------------------------------------------------------------------------
 

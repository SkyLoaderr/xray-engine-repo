#pragma once

class CMailSlotMsg {
	char	m_buff [2048];
	DWORD	m_len;
	int     m_pos;
	inline void Read(void* dst, int sz){
				memcpy(dst,(void*)(&m_buff[0]+m_pos),sz); 
				m_pos +=sz;};
	inline void Write(const void* src, int sz){
				memcpy((void*)(&m_buff[0]+m_pos),src,sz); 
				m_pos +=sz; m_len=m_pos; };

public:
	CMailSlotMsg(){Reset();};
	inline void  Reset(){ m_len=0; m_pos=0; memset(m_buff,0,2048);};
	inline void  SetBuffer(const char* b, int sz){Reset(); memcpy(m_buff,b,sz); m_len=sz; m_pos=0;};
	inline void* GetBuffer(){return m_buff;};
	inline void	 SetLen(DWORD l){m_len=l;};
	inline DWORD GetLen()const{return m_len;};
	
	inline BOOL	r_string(char* dst){
		int sz;
		r_int(sz);
		Read(dst,sz+1); 
		return TRUE;
	};

	inline BOOL	w_string(char* dst){
		size_t sz = strlen(dst);
		w_int((int)sz);
		Write(dst,(int)(sz+1)); return TRUE;
	};

	inline BOOL	r_float(float& dst){
		Read(&dst,sizeof(float));
		return TRUE;
	};

	inline BOOL	w_float(float src){
		Write(&src,sizeof(float));
		return TRUE;
	};

	inline BOOL	r_int(int& dst){
		Read(&dst,sizeof(int));
		return TRUE;
	};
	
	inline BOOL	w_int(int src){
		Write(&src,sizeof(int));
		return TRUE;
	};

	inline BOOL	r_buff(void* dst, int sz){
		Read(dst,sz);
		return TRUE;
	};
	
	inline BOOL	w_buff(void* src, int sz){
		Write(src,sz);
		return TRUE;
	};
};

inline HANDLE CreateMailSlotByName(LPSTR slotName)
{
  HANDLE  hSlot = CreateMailslot(slotName, 
        0,                             // no maximum message size 
        MAILSLOT_WAIT_FOREVER,         // no time-out for operations 
        (LPSECURITY_ATTRIBUTES) NULL); // no security attributes 
 
    R_ASSERT (hSlot != INVALID_HANDLE_VALUE);
 
    return hSlot; 
}
inline BOOL CheckExisting(LPSTR slotName)
{
	HANDLE hFile; 
	BOOL res;
hFile = CreateFile(slotName, 
    GENERIC_WRITE, 
    FILE_SHARE_READ,  // required to write to a mailslot 
    (LPSECURITY_ATTRIBUTES) NULL, 
    OPEN_EXISTING, 
    FILE_ATTRIBUTE_NORMAL, 
    (HANDLE) NULL); 

    res = (hFile != INVALID_HANDLE_VALUE);

	if(res)
		CloseHandle(hFile); 
	
	return res;
}
inline BOOL SendMailslotMessage(LPSTR slotName, CMailSlotMsg& msg){
	BOOL fResult; 
	HANDLE hFile; 
	DWORD cbWritten; 
 
hFile = CreateFile(slotName, 
    GENERIC_WRITE, 
    FILE_SHARE_READ,  // required to write to a mailslot 
    (LPSECURITY_ATTRIBUTES) NULL, 
    OPEN_EXISTING, 
    FILE_ATTRIBUTE_NORMAL, 
    (HANDLE) NULL); 
 
    R_ASSERT (hFile != INVALID_HANDLE_VALUE);

	if (hFile == INVALID_HANDLE_VALUE) 
		return false; 


fResult = WriteFile(hFile, 
					msg.GetBuffer(),
					msg.GetLen(),
					&cbWritten, 
					(LPOVERLAPPED) NULL); 
 
	R_ASSERT(fResult);
	fResult = CloseHandle(hFile); 
	R_ASSERT(fResult);
	return fResult;
}

inline BOOL CheckMailslotMessage(HANDLE hSlot, CMailSlotMsg& msg){
    DWORD cbMessage, cMessage, cbRead; 
    BOOL fResult; 
    HANDLE hEvent;
    OVERLAPPED ov;
 
    cbMessage = cMessage = cbRead = 0; 

    hEvent = CreateEvent(NULL, FALSE, FALSE, "__Slot");
    if( NULL == hEvent )
        return FALSE;
    ov.Offset = 0;
    ov.OffsetHigh = 0;
    ov.hEvent = hEvent;
 
 
    fResult = GetMailslotInfo(hSlot, // mailslot handle 
        (LPDWORD) NULL,               // no maximum message size 
        &cbMessage,                   // size of next message 
        &cMessage,                    // number of messages 
        (LPDWORD) NULL);              // no read time-out 
 
	R_ASSERT(fResult);
    if (!fResult) return false; 
 
    if (cbMessage == MAILSLOT_NO_MESSAGE) return false; 
 
 
        fResult = ReadFile(hSlot, 
            msg.GetBuffer(), 
            cbMessage, 
            &cbRead, 
            &ov); 
		msg.SetLen(cbRead);
		R_ASSERT(fResult);

		return fResult;
}
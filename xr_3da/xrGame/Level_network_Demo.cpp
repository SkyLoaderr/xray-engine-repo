#include "stdafx.h"
#include "Level.h"

#define DEMO_DATA_SIZE	65536

void						CLevel::Demo_StoreData			(void* data, u32 size)
{
	if (!IsDemoSave()) return;

	DemoCS.Enter();

	u32 CurTime = timeServer_Async();
	u32 TotalSize = sizeof(CurTime) + sizeof(size) + size;

	R_ASSERT2(size <= DEMO_DATA_SIZE, "Data is too BIG!");
	if ((TotalSize + m_dwStoredDemoDataSize) > DEMO_DATA_SIZE)
	{
		Demo_DumpData();
	};

	CopyMemory(m_pStoredDemoData + m_dwStoredDemoDataSize, &CurTime, 4); m_dwStoredDemoDataSize += 4;
	CopyMemory(m_pStoredDemoData + m_dwStoredDemoDataSize, &size, 4); m_dwStoredDemoDataSize += 4;
	CopyMemory(m_pStoredDemoData + m_dwStoredDemoDataSize, data, size); m_dwStoredDemoDataSize += size;

	DemoCS.Leave();
	/*
	FILE* fTDemo = fopen(m_sDemoName, "ab");
	if (!fTDemo) return;

	static u32 Count = 0;
	static u32 TotalSize = 0;
	u32 CurTime = timeServer_Async();
	fwrite(&(CurTime), sizeof(CurTime), 1, fTDemo); TotalSize += sizeof(CurTime);
	fwrite(&(size), sizeof(size), 1, fTDemo);		TotalSize += sizeof(size);
	if (size) fwrite((data), 1, size, fTDemo);		TotalSize += size;
	fclose(fTDemo);
	Count++;
	*/
}

void						CLevel::Demo_DumpData()
{
	if (!m_sDemoName[0]) return;
	FILE* fTDemo = fopen(m_sDemoName, "ab");
	if (fTDemo)
	{
		fwrite(m_pStoredDemoData, m_dwStoredDemoDataSize, 1, fTDemo);
		fclose(fTDemo);
	}

	m_dwStoredDemoDataSize = 0;
}

void						CLevel_DemoCrash_Handler	()
{
	Level().WriteStoredDemo();
	Level().CallOldCrashHandler();
}

void						CLevel::Demo_PrepareToStore			()
{
	m_pOldCrashHandler = Debug.get_crashhandler();
	Debug.set_crashhandler(CLevel_DemoCrash_Handler);
	//---------------------------------------------------------------
	m_bDemoSaveMode = TRUE;

	string1024 CName = "";
	u32 CNameSize = 1024;
	GetComputerName(CName, (DWORD*)&CNameSize);
	SYSTEMTIME Time;
	GetLocalTime(&Time);
	sprintf(m_sDemoName, "xray_%s_%02d-%02d-%02d_%02d-%02d-%02d.tdemo", CName, Time.wMonth, Time.wDay, Time.wYear, Time.wHour, Time.wMinute, Time.wSecond);
	Msg("Tech Demo would be stored in - %s", m_sDemoName);
	
	FS.update_path      (m_sDemoName,"$logs$",m_sDemoName);
	//---------------------------------------------------------------
	m_dwStoredDemoDataSize = 0;
	m_pStoredDemoData = (u8*) xr_malloc(DEMO_DATA_SIZE);

};

void						CLevel::CallOldCrashHandler			()
{
	if (!m_pOldCrashHandler) return;
	m_pOldCrashHandler();
};

void						CLevel::WriteStoredDemo			()
{
	DemoCS.Enter();
	Demo_DumpData();
	DemoCS.Leave();
};

BOOL	g_bLeaveTDemo = FALSE;

void						CLevel::Demo_Clear				()
{
	WriteStoredDemo();
	m_bDemoSaveMode = FALSE;
	xr_free(m_pStoredDemoData);
	m_dwStoredDemoDataSize = 0;	 

	if (!g_bLeaveTDemo)
	{
		DeleteFile(m_sDemoName);
	};
};

void						CLevel::Demo_Load				(LPCSTR DemoName)
{
	
	string1024	DemoFileName;
	FS.update_path      (DemoFileName,"$logs$",DemoName);

	FILE* fTDemo = fopen(DemoFileName, "rb");
	if (fTDemo)
	{
		NET_Packet NewPacket;
		Msg("\n------- Loading Demo... ---------\n");

		while (!feof(fTDemo))
		{
			fread(&(NewPacket.timeReceive), sizeof(NewPacket.timeReceive), 1, fTDemo);
			fread(&(NewPacket.B.count), sizeof(NewPacket.B.count), 1, fTDemo);
			fread((NewPacket.B.data), 1, NewPacket.B.count, fTDemo);

			m_aDemoData.push_back(NewPacket);
		}
		fclose(fTDemo);
		if (!m_aDemoData.empty()) 
		{
			m_bDemoPlayMode = TRUE;
			m_bDemoSaveMode = FALSE;
		};
	}
}

void						CLevel::Demo_Update				()
{
	if (!IsDemoPlay() || m_aDemoData.empty() || !m_bDemoStarted) return;
	static u32 Pos = 0;
	if (Pos >= m_aDemoData.size()) return;

	for (Pos; Pos < m_aDemoData.size(); Pos++)
	{
		u32 CurTime = timeServer_Async();
		NET_Packet* P = &(m_aDemoData[Pos]);
		//		Msg("tS_A - %d; P->tR - %d", CurTime, P->timeReceive);
		if (P->timeReceive <= CurTime) IPureClient::OnMessage(P->B.data, P->B.count);
		else 
		{			
			break;
		};
	}

	if (Pos >= m_aDemoData.size())
	{
		Msg("! ------------- Demo Ended ------------");
	};	
};


#include "stdafx.h"
#include "Level.h"

#define DEMO_DATA_SIZE	65536

void						CLevel::Demo_StoreData			(void* data, u32 size, DEMO_CHUNK DataType)
{
	if (!IsDemoSave()) return;

//	DemoCS.Enter();

	u32 CurTime = timeServer_Async();
	u32 TotalSize = sizeof(CurTime) + sizeof(size) + size;

	R_ASSERT2(size <= DEMO_DATA_SIZE, "Data is too BIG!");
	if ((TotalSize + m_dwStoredDemoDataSize) > DEMO_DATA_SIZE)
	{
		Demo_DumpData();
	};

	DEMO_CHUNK	Chunk = DataType;
	CopyMemory(m_pStoredDemoData + m_dwStoredDemoDataSize, &Chunk, 4); m_dwStoredDemoDataSize += 4;
	CopyMemory(m_pStoredDemoData + m_dwStoredDemoDataSize, &m_dwCurDemoFrame, 4); m_dwStoredDemoDataSize += 4;
	CopyMemory(m_pStoredDemoData + m_dwStoredDemoDataSize, &CurTime, 4); m_dwStoredDemoDataSize += 4;	
	switch (DataType)
	{
	case DATA_FRAME:
		{
			CopyMemory(m_pStoredDemoData + m_dwStoredDemoDataSize, data, size); m_dwStoredDemoDataSize += size;
		}break;
	case DATA_PACKET:
		{
			CopyMemory(m_pStoredDemoData + m_dwStoredDemoDataSize, &size, 4); m_dwStoredDemoDataSize += 4;
			CopyMemory(m_pStoredDemoData + m_dwStoredDemoDataSize, data, size); m_dwStoredDemoDataSize += size;
		}break;
	}
	
	
//	DemoCS.Leave();

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
	m_pStoredDemoData = xr_alloc<u8>(DEMO_DATA_SIZE/sizeof(u8));
	//---------------------------------------------------------------
	m_dwCurDemoFrame = 0;
};

void						CLevel::CallOldCrashHandler			()
{
	if (!m_pOldCrashHandler) return;
	m_pOldCrashHandler();
};

void						CLevel::WriteStoredDemo			()
{	
	if (!DemoCS.TryEnter()) 
	{
		Msg("! Unable to Write Stored Demo!");
		return;
	};
	
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
//		NET_Packet NewPacket;
		DemoDataStruct NewData;
		Msg("\n------- Loading Demo... ---------\n");

		while (!feof(fTDemo))
		{
			fread(&(NewData.m_dwDataType), sizeof(NewData.m_dwDataType), 1, fTDemo);
			fread(&(NewData.m_dwFrame), sizeof(NewData.m_dwFrame), 1, fTDemo);
			fread(&(NewData.m_dwTimeReceive), sizeof(NewData.m_dwTimeReceive), 1, fTDemo);
			switch (NewData.m_dwDataType)
			{
			case DATA_FRAME:
				{
					fread(&(NewData.FrameTime), 1, sizeof(NewData.FrameTime), fTDemo);
				}break;
			case DATA_PACKET:
				{
					fread(&(NewData.Packet.B.count), sizeof(NewData.Packet.B.count), 1, fTDemo);
					fread((NewData.Packet.B.data), 1, NewData.Packet.B.count, fTDemo);
				}break;
			};			

			m_aDemoData.push_back(NewData);
		}
		fclose(fTDemo);
		if (!m_aDemoData.empty()) 
		{
			m_bDemoPlayMode = TRUE;
			m_bDemoSaveMode = FALSE;
		};
		m_dwCurDemoFrame = 0;
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
		DemoDataStruct* P = &(m_aDemoData[Pos]);
		if (!m_bDemoPlayByFrame)
		{
			if (P->m_dwDataType != DATA_PACKET) continue;
			if (P->m_dwTimeReceive <= CurTime) IPureClient::OnMessage(P->Packet.B.data, P->Packet.B.count);
			else 
			{			
				break;
			};
		}
		else
		{
			if (P->m_dwFrame != m_dwCurDemoFrame) break;
			switch (P->m_dwDataType)
			{
			case DATA_FRAME:
				{
					Device.dwTimeDelta		= P->FrameTime.dwTimeDelta;
					Device.dwTimeGlobal		= P->FrameTime.dwTimeGlobal;
//					CurFrameTime.dwTimeServer		= Level().timeServer();
//					CurFrameTime.dwTimeServer_Delta = Level().timeServer_Delta();
					Device.fTimeDelta		= P->FrameTime.fTimeDelta;
					Device.fTimeGlobal		= P->FrameTime.fTimeGlobal;

				}break;
			case DATA_PACKET:
				{
					IPureClient::OnMessage(P->Packet.B.data, P->Packet.B.count);
				}break;
			}
		}
	}

//	m_dwCurDemoFrame++;

	if (Pos >= m_aDemoData.size())
	{
		Msg("! ------------- Demo Ended ------------");
	};	
};

void						CLevel::Demo_StartFrame			()
{
	if (!IsDemoSave() || !net_IsSyncronised()) return;

	DemoCS.Enter();

	DemoFrameTime CurFrameTime;
	CurFrameTime.dwTimeDelta = Device.dwTimeDelta;
	CurFrameTime.dwTimeGlobal = Device.dwTimeGlobal;
	CurFrameTime.dwTimeServer = Level().timeServer();
	CurFrameTime.dwTimeServer_Delta = Level().timeServer_Delta();
	CurFrameTime.fTimeDelta = Device.fTimeDelta;
	CurFrameTime.fTimeGlobal= Device.fTimeGlobal;

	Demo_StoreData(&CurFrameTime, sizeof(CurFrameTime), DATA_FRAME);

	DemoCS.Leave();
};

void						CLevel::Demo_EndFrame			()
{
	m_dwCurDemoFrame++;	
};


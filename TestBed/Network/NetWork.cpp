// NetWork.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#ifndef SERVER
#include "net_client.h"
#include "net_messages.h"

int main(int argc, char* argv[])
{
    // Init COM so we can use CoCreateInstance
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// Retreive user name
	char nick [ 128 ];
	printf	("* Enter nick: ");
	scanf	("%s",nick);

	// Retreive server name
	char server [ 128 ];
	printf	("* Enter server name: ");
	scanf	("%s",server);

	// Connect
	IPureClient*	client = new IPureClient;
	client->Connect	(server,nick);
	printf("* Hardware connection succesful. Waiting for logical completition...");

	// Waiting
	while (!client->Connect_Complete()) Sleep(1);
	printf("\n* Ready.\n\n");

	// Main cycle
	while (TRUE) {
		for (NET_Packet* P = client->Retreive(); P; P=client->Retreive())
		{
			u16			m_type;
			P->r_begin	(m_type);
			switch (m_type)
			{
			case M_CHAT:
				{
					char	buffer[256];
					P->r_string(buffer);
					printf	("RECEIVE: %s\n",buffer);
				}
				break;
			}
		}
		if (_kbhit()) 
		{
			char c = getch();
			if (c==27) break;

			char			buffer[256];
			buffer[0]		= c;
			printf			("%c",c);
			scanf			("%s",&buffer[1]);

			NET_Packet		P;
			P.w_begin		(M_CHAT);
			P.w_string		(buffer);
			client->Send	(P);
		}
	}

	// Disconnect
	client->Disconnect();
	delete			client;

	return 0;
}

#endif

#include "stdafx.h"

#ifdef SERVER
#include "net_server.h"
#include "net_messages.h"

int main(int argc, char* argv[])
{
    // Init COM so we can use CoCreateInstance
    CoInitializeEx(NULL, COINIT_MULTITHREADED);

	// Retreive session name
	char session [ 128 ];
	printf	("* Enter session name: ");
	scanf	("%s",session);

	// Connect
	IPureServer*	server = new IPureServer;
	server->Connect	(session);
	printf			("* Connection succesful.\n\n");
	printf			("ESC - quit\ns/S - stats\nr/R - reparse\n\n");

	// Main cycle
	BOOL bQuit		= FALSE;
	while (!bQuit)	{
		while		(!_kbhit())	Sleep(1);

		switch (getch())
		{
		case 27:	
			bQuit	= TRUE;
			break;
		case 's':
		case 'S':
			{
				const IServerStatistic*	S = server->GetStatistic();
				printf("\n");
				printf("IN:  %4d/%4d (%2.1f%%)\n",S->bytes_in_real,S->bytes_in,100.f*float(S->bytes_in_real)/float(S->bytes_in));
				printf("OUT: %4d/%4d (%2.1f%%)\n",S->bytes_out_real,S->bytes_out,100.f*float(S->bytes_out_real)/float(S->bytes_out));
			}
			break;
		case 'r':
		case 'R':
			server->Reparse	();
		}
	}

	// Disconnect
	server->Disconnect();
	delete			server;

	return 0;
}

#endif

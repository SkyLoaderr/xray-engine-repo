// xrCompress.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

typedef set<char*,pred_str>	set_cstr;
typedef set_cstr::iterator	set_cstr_it;
	
set_cstr			files;

extern void Recurse	(const char* path);

void ProcessOne		(_finddata_t& F, const char* path)
{
	string256		N;
	strcpy			(N,path);
	strcat			(N,F.name);
	
	if (F.attrib&_A_SUBDIR) {
		if (0==strcmp(F.name,"."))	return;
		if (0==strcmp(F.name,"..")) return;
		strcat(N,"\\");
		files.insert(strlwr(strdup(N)));
		Recurse		(N);
	} else {
		char*	ins	= strlwr(strdup(N));
		printf		("%s\n",ins);
		files.insert(ins); 
	}
}

void Recurse		(const char* path)
{
    _finddata_t		sFile;
    int				hFile;
	
	string256		N;
	strcpy			(N,path);
	strcat			(N,"*.*");
	
    hFile=_findfirst(N, &sFile);
	ProcessOne		(sFile,path);
	
    while			( _findnext( hFile, &sFile ) == 0 )
		ProcessOne	(sFile,path);
	
    _findclose		( hFile );
}

int main			(int argc, char* argv[])
{
	InitMath		();
	printf			("\n\n");

	if (argc!=2)	{
		printf("ERROR: u must pass folder name as parameter.\n");
		return 3;
	}
	printf			("Scanning files...");
	if (0==chdir(argv[1]))
	{
		Recurse			("");
	} else {
		printf("ERROR: folder not found.\n");
		return 3;
	}

	return 0;
}


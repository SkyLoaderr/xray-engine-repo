// xrCompress.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

struct pred		: public std::binary_function<char*, char*, bool> 
{	
	IC bool operator()(const char* x, const char* y) const
	{	return strcmp(x,y)<0;	}
};
	
typedef set<char*,pred_str>	set_cstr;
typedef set_cstr::iterator	set_cstr_it;
	
set_cstr			files;
	
void ProcessOne(_finddata_t& F, const char* path)
{
	string256		N;
	strcpy			(N,path);
	strcat			(N,F.name);
	
	if (F.attrib&_A_SUBDIR) {
		if (0==strcmp(F.name,"."))	return;
		if (0==strcmp(F.name,"..")) return;
		strcat(N,"\\");
		files.insert(strlwr(strdup(N)));
		Recurse(N);
	} else {
		files.insert(strlwr(strdup(N))); 
	}
}

void Recurse(const char* path)
{
    _finddata_t		sFile;
    int				hFile;
	
	string256		N;
	strcpy			(N,path);
	strcat			(N,"*.*");
	
    R_ASSERT		((hFile=_findfirst(N, &sFile)) != -1);
	ProcessOne		(sFile,path);
	
    while			( _findnext( hFile, &sFile ) == 0 )
		ProcessOne	(sFile,path);
	
    _findclose		( hFile );
}

int main(int argc, char* argv[])
{
	InitMath		();
	printf			("\n");
	
	return 0;
}


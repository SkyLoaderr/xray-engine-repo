#ifndef FS_internalH
#define FS_internalH
#pragma once

class CFileWriter : public IWriter
{
private:
	FILE*			hf;
public:
	CFileWriter		(const char *name)
	{
		fName		= xr_strdup(name);
		R_ASSERT	(name);
		R_ASSERT	(name[0]);
		VerifyPath	(fName);
		hf			= fopen(fName, "wb");
		R_ASSERT	(hf);
		setvbuf		(hf, 0, _IOFBF, 24*1024 );
	}

	virtual ~CFileWriter()
	{
		fclose		(hf);
	}

	// kernel
	virtual void	w			(const void* ptr, u32 count) { fwrite(ptr,count,1,hf);};
	virtual void	seek		(u32 pos)	{	fseek(hf,pos,SEEK_SET);		};
	virtual u32		tell		()			{	return ftell(hf);			};
};

#endif
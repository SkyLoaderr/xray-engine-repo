#ifndef _FDemoOps_
#define _FDemoOps_

extern ENGINE_API void *	DownloadFile(char *fn, DWORD *pdwSize=NULL);
extern ENGINE_API bool		FileExists(char *fn);

__forceinline bool isTerm(char a) {
	return (a==13)||(a==10);
};

class ENGINE_API CStream {
	char *	data;
	int		Pos;
	int		Size;
public:
	CStream(char *name)
	{
		data	= (char *)DownloadFile(name,(DWORD *)&Size);
		Pos		= 0;
	};
	~CStream()
	{
		_FREE(data);
	};
	int		Elapsed(void)			// сколько байт осталось
	{	return Size-Pos;		};
	bool	Eof(void)				// есть ли еще данные для чтения
	{	return Elapsed()<=0;	};
	int		Tell(void)				// позиция указателя чтения
	{	return Pos;				};
	int		Length(void)			// размер файла
	{	return Size;			};
	void*	Pointer(void)			// указатель на память
	{	return &(data[Pos]);	};
	void	Seek(int ptr)			// перемещение указателя
	{
		VERIFY(ptr<Size);
		Pos=ptr;
	};
	void	Read(void *p,int cnt)	// главная ф-я чтения
	{
		VERIFY(Pos+cnt<=Size);
		CopyMemory(p,Pointer(),cnt);
		Pos+=cnt;
	};
	void	ReadString(char *dest)	// читает строку до CR/LF
	{
		char *src = (char *) data;
		while (!Eof()) {
			if (isTerm(src[Pos])) {
				*dest = 0;
				Pos++;
				if (!Eof() && isTerm(src[Pos])) Pos++;
				return;
			}
			*dest++ = src[Pos++];
		}
		*dest	=	0;
	};
	void	ReadStringZ(char *dest)	// читает C-Style строку (ANSIZ)
	{
		char *src = (char *) data;
		while ((src[Pos]!=0) && (!Eof())) *dest++ = src[Pos++];
		*dest	=	0;
		Pos		++;
	};
	DWORD	ReadDWORD(void)
	{	DWORD tmp;	Read(&tmp,sizeof(tmp)); return tmp;	};
	BYTE	ReadBYTE(void)
	{	BYTE tmp;	Read(&tmp,sizeof(tmp));	return tmp; };
	char	ReadCHAR(void)
	{	char tmp;	Read(&tmp,sizeof(tmp));	return tmp; };
	int		ReadINT(void)
	{	int tmp;	Read(&tmp,sizeof(tmp));	return tmp; };
	WORD	ReadWORD(void)
	{	WORD tmp;	Read(&tmp,sizeof(tmp));	return tmp; };

	DWORD	FindChunk(DWORD ID)	// поиск XR Chunk'ов - возврат - размер или 0
	{
		DWORD	dwSize,dwType;

		Seek(0);
		while (!Eof()) {
			dwType = ReadDWORD();
			dwSize = ReadDWORD();
			if (dwType == ID)	return dwSize;
			else				Pos+=dwSize;
		}
		return 0;
	};
	bool	ReadChunk(DWORD ID, void *dest)	// чтение XR Chunk'ов (4b-ID,4b-size,??b-data)
	{
		DWORD	dwSize;
		if ((dwSize = FindChunk(ID))!=0) {
			Read(dest,dwSize);
			return true;
		} else return false;
	};
};

#endif

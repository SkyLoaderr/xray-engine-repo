#ifndef _FDemoOps_
#define _FDemoOps_

extern void *	DownloadFile(char *fn, DWORD *pdwSize=NULL);
extern BOOL		FileExists(char *fn);

__forceinline BOOL isTerm(char a) {
	return (a==13)||(a==10);
};

class CStream {
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
		free(data);
	};
	int		Elapsed(void)			// ������� ���� ��������
	{	return Size-Pos;		};
	BOOL	Eof(void)				// ���� �� ��� ������ ��� ������
	{	return Elapsed()<=0;	};
	int		Tell(void)				// ������� ��������� ������
	{	return Pos;				};
	int		Length(void)			// ������ �����
	{	return Size;			};
	void*	Pointer(void)			// ��������� �� ������
	{	return &(data[Pos]);	};
	void	Seek(int ptr)			// ����������� ���������
	{	Pos=ptr;	VERIFY((Pos<=Size) && (Pos>=0));	};
	void	Advance(int cnt)		// ����������� ���������
	{	Pos+=cnt;	VERIFY((Pos<=Size) && (Pos>=0));	};
	void	Read(void *p,int cnt)	// ������� �-� ������
	{	VERIFY(Pos+cnt<=Size);
		CopyMemory(p,Pointer(),cnt);
		Advance(cnt);
	};
	void	ReadString(char *dest)	// ������ ������ �� CR/LF
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
	void	ReadStringZ(char *dest)	// ������ C-Style ������ (ANSIZ)
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

	DWORD	FindChunk(DWORD ID)	// ����� XR Chunk'�� - ������� - ������ ��� 0
	{
		DWORD	dwSize,dwType;

		Seek(0);
		while (!Eof()) {
			dwType = ReadDWORD();
			dwSize = ReadDWORD();
			if (dwType == ID)	return dwSize;
			else				Advance(dwSize);
		}
		return 0;
	};
	BOOL	ReadChunk(DWORD ID, void *dest)	// ������ XR Chunk'�� (4b-ID,4b-size,??b-data)
	{
		DWORD	dwSize;
		if ((dwSize = FindChunk(ID))!=0) {
			Read(dest,dwSize);
			return true;
		} else return false;
	};
};

#endif

// Image.h: interface for the CImage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGE_H__4281EEAB_9522_40E5_A90D_831A40E1A344__INCLUDED_)
#define AFX_IMAGE_H__4281EEAB_9522_40E5_A90D_831A40E1A344__INCLUDED_
#pragma once

class ENGINE_API CImage
{
public:
	DWORD   dwWidth;
	DWORD   dwHeight;
	BOOL	bAlpha;
	DWORD*	pData;

	CImage	()		{ ZeroMemory(this,sizeof(*this)); }
	~CImage	()		{ _FREE(pData); }

	void	LoadT	(char *name);
	void	LoadTGA	(char *name);

	void	Vflip		(void);
	void	Hflip		(void);
	void	Contrast	(float Q);
	void	Grayscale	();

	__forceinline DWORD	GetPixel(int x, int y)			{ return pData[y*dwWidth+x];}
	__forceinline void	PutPixel(int x, int y, DWORD p)	{ pData[y*dwWidth+x] = p;	}
};

#endif // !defined(AFX_IMAGE_H__4281EEAB_9522_40E5_A90D_831A40E1A344__INCLUDED_)

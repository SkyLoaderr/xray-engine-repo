//---------------------------------------------------------------------------
#ifndef EImageManagerH
#define EImageManagerH

struct FIBITMAP;
struct STextureParams;
class EImageThumbnail;

//DEVINE_MAP(LPSTR,EImageThumbnail*,)

class CImageManager{
    void		MakeThumbnail		(EImageThumbnail* THM, FIBITMAP* bm);
    void		MakeGameTexture		(EImageThumbnail* THM, LPCSTR game_name, FIBITMAP* bm);
public:
				CImageManager		(){;}
				~CImageManager		(){;}
	int			GetModifiedFiles	(AStringVec& files);
	void		SynchronizeTexture	(const AnsiString& src_name);
	void		SynchronizeTextures	(LPSTRVec* files=0);
    void		CreateThumbnail		(EImageThumbnail* THM, const AnsiString& src_name);
    void		CreateGameTexture	(const AnsiString& src_name, EImageThumbnail* thumb=0);
    int			GetFiles			(AStringVec& files);
    bool		LoadTextureData		(const AnsiString& src_name, DWORDVec& data, int& w, int& h);
};

extern CImageManager ImageManager;
//---------------------------------------------------------------------------
#endif

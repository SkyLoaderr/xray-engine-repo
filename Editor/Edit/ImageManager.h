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
    BOOL		IfChanged			(const AnsiString& name);
	void		Synchronize			(const AnsiString& name);  // return some as IfUpdated
    void		CreateThumbnail		(EImageThumbnail* THM, const AnsiString& src_name);
    void		CreateGameTexture	(const AnsiString& src_name, EImageThumbnail* thumb=0);
//	void		SynchronizePath		();
    int			GetModifiedFiles	(AStringVec& files);
    int			GetFiles			(AStringVec& files);
    bool		LoadTextureData		(const AnsiString& src_name, DWORDVec& data);
};

extern CImageManager ImageManager;
//---------------------------------------------------------------------------
#endif

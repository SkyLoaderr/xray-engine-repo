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
    BOOL		IfChanged			(LPCSTR name);
	void		Synchronize			(LPCSTR name);  // return some as IfUpdated
    void		CreateThumbnail		(EImageThumbnail* THM, LPCSTR src_name);
	void		SynchronizePath		();
    int			GetModifiedFiles	(AStringVec& files);
    int			GetFiles			(AStringVec& files);
};

extern CImageManager ImageManager;
//---------------------------------------------------------------------------
#endif

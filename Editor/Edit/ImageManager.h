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
    void		CheckUniqueName		();	
    void		CreateThumbnail		(EImageThumbnail* THM, LPCSTR src_name);
};

extern CImageManager ImageManager;
//    void		Synchronize			();				// удаляет текстуры в game которых нет в редакторе
//---------------------------------------------------------------------------
#endif

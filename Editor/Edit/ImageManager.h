//---------------------------------------------------------------------------
#ifndef EImageManagerH
#define EImageManagerH

struct FIBITMAP;
struct STextureParams;
class EImageThumbnail;

//DEVINE_MAP(LPSTR,EImageThumbnail*,)

class CImageManager{
    void		MakeThumbnail		(EImageThumbnail* THM, FIBITMAP* bm, int age);
    void		MakeGameTexture		(STextureParams* P, LPCSTR game_name, FIBITMAP* bm, int age);
public:
				CImageManager		(){;}
				~CImageManager		(){;}
    BOOL		IfChanged			(LPCSTR name);
	void		Synchronize			(LPCSTR name);  // return some as IfUpdated
    void		CheckUniqueName		();	
};

extern CImageManager ImageManager;
//    void		Synchronize			();				// удаляет текстуры в game которых нет в редакторе
//---------------------------------------------------------------------------
#endif

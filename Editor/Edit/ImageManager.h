//---------------------------------------------------------------------------
#ifndef ETextureManagerH
#define ETextureManagerH

class CImageManager{
public:
				CTextureManager		();
				~CTextureManager	();
    void		CreateThumbnail		(LPCSTR name);
    BOOL		IfUpdated			(LPCSTR name);

};

//    void		Synchronize			();				// ������� �������� � game ������� ��� � ���������
//---------------------------------------------------------------------------
#endif

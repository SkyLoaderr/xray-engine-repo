//---------------------------------------------------------------------------

#ifndef LightAnimLibraryH
#define LightAnimLibraryH
//---------------------------------------------------------------------------

class CLAItem{
public:
    string64    cName;
    float   	fFPS;
    DEFINE_MAP(int,DWORD,KeyMap,KeyPairIt);
    KeyMap		Keys;
    int			iFrameCount;
public:
                CLAItem				();
	void		InitDefault			();
    void		Load				(CStream& F);
    void		Save				(CFS_Base& F);
    DWORD		Interpolate			(int frame);
    DWORD		Calculate			(float T);
    void		Resize				(int new_len);
    void		InsertKey			(int frame, DWORD color);
    void		DeleteKey			(int frame);
    void		MoveKey				(int from, int to);
};

class ELightAnimLibrary{
    LPCSTR			GenerateName		(LPSTR name, LPCSTR source);
public:
    DEFINE_VECTOR(CLAItem*,LAItemVec,LAItemIt);
    LAItemVec       Items;
    CLAItem*		FindItem			(LPCSTR name);
public:
					ELightAnimLibrary	();
					~ELightAnimLibrary	();
    void			OnCreate			();
    void			OnDestroy			();
    void			Load				();
    void			Save				();
    void			Reload				();
    CLAItem*		AppendItem			(LPCSTR folder_name, CLAItem* parent);
    void			DeleteItem			(LPCSTR name);
};

extern ELightAnimLibrary LALib;

#endif

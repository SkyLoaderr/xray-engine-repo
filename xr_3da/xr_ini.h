#ifndef __XR_INI_H__
#define __XR_INI_H__

// refs
class	CInifile;
struct	xr_token;


//-----------------------------------------------------------------------------------------------------------
//Описание Inifile
//-----------------------------------------------------------------------------------------------------------

class ENGINE_API CInifile
{
public:
	struct	Item
	{
		LPSTR	first;
		LPSTR	second;
		LPSTR	comment;

		Item() : first(0), second(0), comment(0) {};
	};
	typedef vector<Item>		Items;
	typedef Items::iterator		SectIt;
	struct	Sect {
		LPSTR			Name;
		Items			Data;

		IC SectIt		begin()	{ return Data.begin();	}
		IC SectIt		end()	{ return Data.end();	}
		IC int			size()	{ return Data.size();	}
		IC void			clear()	{ Data.clear();			}
	};
	typedef	vector<Sect>		Root;
	typedef Root::iterator		RootIt;

	struct sect_pred : public binary_function<Sect&, Sect&, bool> 
	{	
		IC bool operator() (const Sect& x, const Sect& y) const
		{	return strcmp(x.Name,y.Name)<0;	}
	};
	struct item_pred : public binary_function<Item&, Item&, bool> 
	{	
		IC bool operator() (const Item& x, const Item& y) const
		{	
			if ((0==x.first) || (0==y.first))	return x.first<y.first;
			else								return strcmp(x.first,y.first)<0;
		}
	};

	// factorisation
	static CInifile*	Create	( LPCSTR szFileName, BOOL ReadOnly=TRUE);
	static void			Destroy	(CInifile*);
private:
	LPSTR						fName;
	Root						DATA;
	BOOL 						bReadOnly;
public:
	CInifile					( LPCSTR szFileName, BOOL ReadOnly=TRUE);
	~CInifile					( );

	LPCSTR		cName			( ) { return fName; };

	Sect&		ReadSection		( LPCSTR S );
	BOOL		LineExists		( LPCSTR S, LPCSTR L );
	DWORD		LineCount		( LPCSTR S );
	BOOL		SectionExists	( LPCSTR S );

	CLASS_ID	ReadCLSID		( LPCSTR S, LPCSTR L );
	LPCSTR 		ReadSTRING		( LPCSTR S, LPCSTR L );
	int 		ReadINT			( LPCSTR S, LPCSTR L );
	float		ReadFLOAT		( LPCSTR S, LPCSTR L );
	DWORD		ReadCOLOR		( LPCSTR S, LPCSTR L );
	Fvector		ReadVECTOR		( LPCSTR S, LPCSTR L );
	Fvector2	ReadVECTOR2		( LPCSTR S, LPCSTR L );
	BOOL		ReadBOOL		( LPCSTR S, LPCSTR L );
	int			ReadTOKEN		( LPCSTR S, LPCSTR L,	const xr_token *token_list);
	BOOL		ReadLINE		( LPCSTR S, int L,		LPCSTR* N, LPCSTR* V );

    void		WriteString		( LPCSTR S, LPCSTR L, LPCSTR			V, LPCSTR comment=0 );
    void		WriteFloat		( LPCSTR S, LPCSTR L, float				V, LPCSTR comment=0 );
    void		WriteInteger	( LPCSTR S, LPCSTR L, int				V, LPCSTR comment=0 );
    void		WriteColor		( LPCSTR S, LPCSTR L, const Fcolor&		V, LPCSTR comment=0 );
    void		WriteColor		( LPCSTR S, LPCSTR L, DWORD				V, LPCSTR comment=0 );
    void		WriteVector		( LPCSTR S, LPCSTR L, const Fvector&	V, LPCSTR comment=0 );
};

// Main configuration file
extern ENGINE_API CInifile *pSettings;


#endif //__XR_INI_H__

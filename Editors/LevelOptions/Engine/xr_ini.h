#ifndef xr_iniH
#define xr_iniH

// refs
class	CInifile;
struct	xr_token;


//-----------------------------------------------------------------------------------------------------------
//Описание Inifile
//-----------------------------------------------------------------------------------------------------------

class XRCORE_API CInifile
{
public:
	struct XRCORE_API Item
	{
		LPSTR	first;
		LPSTR	second;
		LPSTR	comment;

		Item() : first(0), second(0), comment(0) {};
	};
	typedef std::xr_vector<Item>	Items;
	typedef Items::iterator			SectIt;
    struct XRCORE_API Sect {
		LPSTR			Name;
		Items			Data;

		IC SectIt		begin()		{ return Data.begin();	}
		IC SectIt		end()		{ return Data.end();	}
		IC size_t		size()		{ return Data.size();	}
		IC void			clear()		{ Data.clear();			}
	    BOOL			line_exist	(LPCSTR L, LPCSTR* val=0);
	};
	typedef	std::xr_vector<Sect>	Root;
	typedef Root::iterator		RootIt;

	struct sect_pred : public std::binary_function<Sect&, Sect&, bool>
	{
		IC bool operator() (const Sect& x, const Sect& y) const
		{	return strcmp(x.Name,y.Name)<0;	}
	};
	struct item_pred : public std::binary_function<Item&, Item&, bool>
	{
		IC bool operator() (const Item& x, const Item& y) const
		{
			if ((0==x.first) || (0==y.first))	return x.first<y.first;
			else								return strcmp(x.first,y.first)<0;
		}
	};

	// factorisation
	static CInifile*	Create	( LPCSTR szFileName, BOOL ReadOnly=TRUE);
	static void			Destroy	( CInifile*);
    static IC BOOL		IsBOOL	( LPCSTR B)	{ return (strcmp(B,"on")==0 || strcmp(B,"yes")==0 || strcmp(B,"true")==0 || strcmp(B,"1")==0);}
private:
	LPSTR		fName;
	Root		DATA;
	BOOL		bReadOnly;
public:
    BOOL		bSaveAtEnd;
public:
				CInifile		( LPCSTR szFileName, BOOL ReadOnly=TRUE, BOOL bLoadAtStart=TRUE, BOOL SaveAtEnd=TRUE);
	virtual 	~CInifile		( );
    void		save_as         ( LPCSTR new_fname=0 );

	LPCSTR		fname			( ) { return fName; };

	Sect&		r_section		( LPCSTR S );
	BOOL		line_exist		( LPCSTR S, LPCSTR L );
	u32			line_count		( LPCSTR S );
	BOOL		section_exist	( LPCSTR S );
	Root&		sections		( ){return DATA;}

	CLASS_ID	r_clsid			( LPCSTR S, LPCSTR L );
	LPCSTR 		r_string		( LPCSTR S, LPCSTR L );
	u8	 		r_u8			( LPCSTR S, LPCSTR L );
	u16	 		r_u16			( LPCSTR S, LPCSTR L );
	u32	 		r_u32			( LPCSTR S, LPCSTR L );
	s8	 		r_s8			( LPCSTR S, LPCSTR L );
	s16	 		r_s16			( LPCSTR S, LPCSTR L );
	s32	 		r_s32			( LPCSTR S, LPCSTR L );
	float		r_float			( LPCSTR S, LPCSTR L );
	Fcolor		r_fcolor		( LPCSTR S, LPCSTR L );
	u32			r_color			( LPCSTR S, LPCSTR L );
	Ivector2	r_ivector2		( LPCSTR S, LPCSTR L );
	Ivector3	r_ivector3		( LPCSTR S, LPCSTR L );
	Ivector4	r_ivector4		( LPCSTR S, LPCSTR L );
	Fvector2	r_fvector2		( LPCSTR S, LPCSTR L );
	Fvector3	r_fvector3		( LPCSTR S, LPCSTR L );
	Fvector4	r_fvector4		( LPCSTR S, LPCSTR L );
	BOOL		r_bool			( LPCSTR S, LPCSTR L );
	int			r_token			( LPCSTR S, LPCSTR L,	const xr_token *token_list);
	BOOL		r_line			( LPCSTR S, int L,		LPCSTR* N, LPCSTR* V );

    void		w_string		( LPCSTR S, LPCSTR L, LPCSTR			V, LPCSTR comment=0 );
	void		w_u8			( LPCSTR S, LPCSTR L, u8				V, LPCSTR comment=0 );
	void		w_u16			( LPCSTR S, LPCSTR L, u16				V, LPCSTR comment=0 );
	void		w_u32			( LPCSTR S, LPCSTR L, u32				V, LPCSTR comment=0 );
    void		w_s8			( LPCSTR S, LPCSTR L, s8				V, LPCSTR comment=0 );
	void		w_s16			( LPCSTR S, LPCSTR L, s16				V, LPCSTR comment=0 );
	void		w_s32			( LPCSTR S, LPCSTR L, s32				V, LPCSTR comment=0 );
	void		w_float			( LPCSTR S, LPCSTR L, float				V, LPCSTR comment=0 );
    void		w_fcolor		( LPCSTR S, LPCSTR L, const Fcolor&		V, LPCSTR comment=0 );
    void		w_color			( LPCSTR S, LPCSTR L, u32				V, LPCSTR comment=0 );
    void		w_ivector2		( LPCSTR S, LPCSTR L, const Ivector2&	V, LPCSTR comment=0 );
	void		w_ivector3		( LPCSTR S, LPCSTR L, const Ivector3&	V, LPCSTR comment=0 );
	void		w_ivector4		( LPCSTR S, LPCSTR L, const Ivector4&	V, LPCSTR comment=0 );
	void		w_fvector2		( LPCSTR S, LPCSTR L, const Fvector2&	V, LPCSTR comment=0 );
	void		w_fvector3		( LPCSTR S, LPCSTR L, const Fvector3&	V, LPCSTR comment=0 );
	void		w_fvector4		( LPCSTR S, LPCSTR L, const Fvector4&	V, LPCSTR comment=0 );
	void		w_bool			( LPCSTR S, LPCSTR L, BOOL				V, LPCSTR comment=0 );

    void		remove_line		( LPCSTR S, LPCSTR L );
};

// Main configuration file
extern XRCORE_API CInifile *pSettings;


#endif //__XR_INI_H__

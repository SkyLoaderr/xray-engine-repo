#pragma once

#define CMD0(cls)					{ static cls x##cls();				Console->AddCommand(&x##cls);}
#define CMD1(cls,p1)				{ static cls x##cls(p1);			Console->AddCommand(&x##cls);}
#define CMD2(cls,p1,p2)				{ static cls x##cls(p1,p2);			Console->AddCommand(&x##cls);}
#define CMD3(cls,p1,p2,p3)			{ static cls x##cls(p1,p2,p3);		Console->AddCommand(&x##cls);}
#define CMD4(cls,p1,p2,p3,p4)		{ static cls x##cls(p1,p2,p3,p4);	Console->AddCommand(&x##cls);}

class ENGINE_API CConsoleCommand
{
public:
	friend class	CConsole;
	typedef char	TInfo	[256];
	typedef char	TStatus	[256];
protected:
	LPCSTR			cName;
	bool			bEnabled;
	bool			bLowerCaseArgs;
	bool			bEmptyArgsHandled;

	IC	bool		EQ(LPCSTR S1, LPCSTR S2) { return strcmp(S1,S2)==0; }
public:
	CConsoleCommand		(LPCSTR N) : 
	  cName				(N),
	  bEnabled			(TRUE),
	  bLowerCaseArgs	(TRUE),
	  bEmptyArgsHandled	(FALSE)
	{};
	virtual ~CConsoleCommand()
	{};

	LPCSTR			Name()			{ return cName;	}
	void			InvalidSyntax() {
		TInfo I; Info(I);
		Msg("! Invalid syntax in call to '%s'",cName);
		Msg("! Valid arguments: %s", I);
	}
	virtual void	Execute	(LPCSTR args)	= 0;
	virtual void	Status	(TStatus& S)	{ S[0]=0; }
	virtual void	Info	(TInfo& I)		{ strcpy(I,"no arguments"); }
	virtual void	Save	(IWriter *F)	{
		TStatus		S;	Status(S);
		if (S[0])	F->w_printf("%s %s\r\n",cName,S); 
	}
};

class ENGINE_API CCC_Mask : public CConsoleCommand
{
	Flags32*	value;
	u32			mask;
public:
	CCC_Mask(LPCSTR N, Flags32* V, u32 M) :
	  CConsoleCommand(N),
	  value(V),
	  mask(M)
	{};

	virtual void	Execute	(LPCSTR args)
	{
		if (EQ(args,"on"))			value->set(mask,TRUE);
		else if (EQ(args,"off"))	value->set(mask,FALSE);
		else if (EQ(args,"1"))		value->set(mask,TRUE);
		else if (EQ(args,"0"))		value->set(mask,FALSE);
		else InvalidSyntax();
	}
	virtual void	Status	(TStatus& S)
	{	strcpy(S,value->test(mask)?"on":"off"); }
	virtual void	Info	(TInfo& I)
	{	strcpy(I,"'on/off' or '1/0'"); }
};

class ENGINE_API CCC_Token : public CConsoleCommand
{
	u32*			value;
	xr_token*		tokens;
public:
	CCC_Token(LPCSTR N, u32* V, xr_token* T) :
	  CConsoleCommand(N),
	  value(V),
	  tokens(T)
	{};

	virtual void	Execute	(LPCSTR args)
	{
		xr_token *tok = tokens;
		while (tok->name) {
			if (stricmp(tok->name,args)==0) {
				*value=tok->id;
				break;
			}
			tok++;
		}
		if (!tok->name) InvalidSyntax();
	}
	virtual void	Status	(TStatus& S)
	{
		xr_token *tok = tokens;
		while (tok->name) {
			if (tok->id==(int)(*value)) {
				strcpy(S,tok->name);
				return;
			}
			tok++;
		}
		strcpy(S,"?");
		return;
	}
	virtual void	Info	(TInfo& I)
	{	
		I[0]=0;
		xr_token *tok = tokens;
		while (tok->name) {
			if (I[0]) strcat(I,"/");
			strcat(I,tok->name);
			tok++;
		}
	}
};

class ENGINE_API CCC_Float : public CConsoleCommand
{
	float*			value;
	float			min,max;
public:
	CCC_Float(LPCSTR N, float* V, float _min=0, float _max=1) :
	  CConsoleCommand(N),
	  value(V),
	  min(_min),
	  max(_max)
	{};

	virtual void	Execute	(LPCSTR args)
	{
		float v = float(atof(args));
		if (v<min || v>max) InvalidSyntax();
		else	*value = v;
	}
	virtual void	Status	(TStatus& S)
	{	
		sprintf	(S,"%3.5f",*value);
		while	(xr_strlen(S) && ('0'==S[xr_strlen(S)-1]))	S[xr_strlen(S)-1] = 0;
	}
	virtual void	Info	(TInfo& I)
	{	
		sprintf(I,"float value in range [%3.3f,%3.3f]",min,max);
	}
};

class ENGINE_API CCC_Vector3 : public CConsoleCommand
{
	Fvector*		value;
	Fvector			min,max;
public:
	CCC_Vector3(LPCSTR N, Fvector* V, const Fvector _min, const Fvector _max) :
	  CConsoleCommand(N),
	  value(V)
	{
		min.set(_min);
		max.set(_max);
	};

	virtual void	Execute	(LPCSTR args)
	{
		Fvector v;
		if (3!=sscanf(args,"%f,%f,%f",&v.x,&v.y,&v.z))	{ InvalidSyntax(); return; }
		if (v.x<min.x || v.y<min.y || v.z<min.z)		{ InvalidSyntax(); return; }
		if (v.x>max.x || v.y>max.y || v.z>max.z)		{ InvalidSyntax(); return; }
		value->set(v);
	}
	virtual void	Status	(TStatus& S)
	{	
		sprintf	(S,"%f,%f,%f",value->x,value->y,value->z);
	}
	virtual void	Info	(TInfo& I)
	{	
		sprintf(I,"vector3 in range [%f,%f,%f]-[%f,%f,%f]",min.x,min.y,min.z,max.x,max.y,max.z);
	}
};

class ENGINE_API CCC_Integer : public CConsoleCommand
{
	int*			value;
	int				min,max;
public:
	CCC_Integer(LPCSTR N, int* V, int _min=0, int _max=999) :
	  CConsoleCommand(N),
	  value(V),
	  min(_min),
	  max(_max)
	{};

	virtual void	Execute	(LPCSTR args)
	{
		int v = atoi(args);
		if (v<min || v>max) InvalidSyntax();
		else	*value = v;
	}
	virtual void	Status	(TStatus& S)
	{	
		itoa(*value,S,10);
	}
	virtual void	Info	(TInfo& I)
	{	
		sprintf(I,"integer value in range [%d,%d]",min,max);
	}
};

class ENGINE_API CCC_String : public CConsoleCommand
{
	LPSTR			value;
	int				size;
public:
	CCC_String(LPCSTR N, LPSTR V, int _size=2) :
	  CConsoleCommand(N),
	  value	(V),
	  size	(_size)
	{
		R_ASSERT(V);
		R_ASSERT(size>1);
	};

	virtual void	Execute	(LPCSTR args)
	{
		strncpy	(value,args,size-1);
	}
	virtual void	Status	(TStatus& S)
	{	
		strcpy	(S,value);
	}
	virtual void	Info	(TInfo& I)
	{	
		sprintf(I,"string with up to %d characters",size);
	}
};

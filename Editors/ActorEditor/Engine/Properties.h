#ifndef xrPROPERTIES_H
#define xrPROPERTIES_H

#pragma once
#pragma pack(push,4)

// Parameter/Property specifications

// *** FORMAT ***
// u32	type
// stringZ	name
// []		data

enum	xrProperties
{
	xrPID_MARKER	= 0,
	xrPID_MATRIX,		// really only name(stringZ) is written into stream
	xrPID_CONSTANT,		// really only name(stringZ) is written into stream
	xrPID_TEXTURE,		// really only name(stringZ) is written into stream
	xrPID_INTEGER,
	xrPID_FLOAT,
	xrPID_BOOL,
	xrPID_TOKEN,
	xrPID_CLSID,
	xrPID_OBJECT,		// really only name(stringZ) is written into stream
	xrPID_STRING,		// really only name(stringZ) is written into stream
	xrPID_MARKER_TEMPLATE,
	xrPID_FORCEDWORD=u32(-1)
};

struct	xrP_Integer
{
	int					value;
	int					min;
	int					max;

	xrP_Integer() : value(0), min(0), max(255)	{}
};

struct	xrP_Float
{
	float				value;
	float				min;
	float				max;

	xrP_Float()	: value(0), min(0), max(1)		{}
};

struct	xrP_BOOL
{
	BOOL				value;
	xrP_BOOL() : value(FALSE)					{}
};

struct	xrP_TOKEN
{
	struct Item {
		u32		ID;
		string64	str;
	};

	u32				IDselected;
	u32				Count;
	
	//--- elements:		(ID,string64)

	xrP_TOKEN()	: IDselected(0), Count(0)		{}
};

struct	xrP_CLSID
{
	CLASS_ID			Selected;
	u32				Count;
	//--- elements:		(...)

	xrP_CLSID()	: Selected(0), Count(0)			{}
};

struct	xrP_Template
{
	u32				Type;
	u32				Limit;
};

// Base class
class ENGINE_API	CPropertyBase
{
protected:
	
public:
	virtual 	LPCSTR		getName			()								= 0;
	virtual		LPCSTR		getComment		()								= 0;
	
	virtual		void		Save			(CFS_Base&  FS)					= 0;
	virtual		void		Load			(CStream&	FS, WORD version)	= 0;
};

// Writers
IC void		xrPWRITE		(CFS_Base& FS, u32 ID, LPCSTR name, LPCVOID data, u32 size )
{
	FS.Wdword	(ID);
	FS.WstringZ	(name);
	if (data && size)	FS.write	(data,size);
}
IC void		xrPWRITE_MARKER	(CFS_Base& FS, LPCSTR name)
{
	xrPWRITE	(FS,xrPID_MARKER,name,0,0);
}

#define xrPWRITE_PROP(FS,name,ID,data)\
{\
	xrPWRITE	(FS,ID,name,&data,sizeof(data));\
}

// Readers
IC u32	xrPREAD			(CStream& FS)
{
	u32 T		= FS.Rdword();
	FS.SkipStringZ	();
	return		T;
}
IC void		xrPREAD_MARKER	(CStream& FS)
{
	R_ASSERT(xrPID_MARKER==xrPREAD(FS));
}

#define xrPREAD_PROP(FS,ID,data) \
{ \
	R_ASSERT(ID==xrPREAD(FS)); FS.Read(&data,sizeof(data)); \
	switch (ID) \
	{ \
	case xrPID_TOKEN:	FS.Advance(((xrP_TOKEN*)&data)->Count * sizeof(xrP_TOKEN::Item));	break; \
	case xrPID_CLSID:	FS.Advance(((xrP_CLSID*)&data)->Count * sizeof(CLASS_ID));			break; \
	}; \
}

//template <class T>
//IC void		xrPWRITE_PROP	(CFS_Base& FS, LPCSTR name, u32 ID, T& data)
//{
//	xrPWRITE	(FS,ID,name,&data,sizeof(data));
//}

//template <class T>
//IC void		xrPREAD_PROP	(CStream& FS, u32 ID, T& data)
//{
//	R_ASSERT(ID==xrPREAD(FS)); FS.Read(&data,sizeof(data));
//	switch (ID)
//	{
//	case xrPID_TOKEN:	FS.Advance(((xrP_TOKEN*)&data)->Count * sizeof(xrP_TOKEN::Item));	break;
//	case xrPID_CLSID:	FS.Advance(((xrP_CLSID*)&data)->Count * sizeof(CLASS_ID));			break;
//	};
//}
#pragma pack(pop)
#endif // xrPROPERTIES_H
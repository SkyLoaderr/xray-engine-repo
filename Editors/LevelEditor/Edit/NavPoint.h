//----------------------------------------------------
// file: navpoint.h
//----------------------------------------------------

#ifndef _INCDEF_NavPoint_H_
#define _INCDEF_NavPoint_H_

#include "CustomObject.h"

class CFrustum;

class CNavPoint : public CCustomObject {
    friend class    SceneBuilder;
    friend class    TfrmPropertiesNavPoint;
    friend class    CNavNetwork;

    DWORD           m_Type;
    Fvector         m_Position;
    void            DrawNavPoint(Fcolor& c);
public:
	                CNavPoint   ();
	                CNavPoint   ( char *name );
    void            Construct   ();
	virtual         ~CNavPoint  ();

	virtual void    Render      ( Fmatrix& parent, ERenderPriority flag );
	virtual bool    RTL_Pick    ( float& distance,	Fvector& start,	Fvector& direction,
		                          Fmatrix& parent, SPickInfo* pinf = NULL );
    virtual bool 	FrustumPick	( const CFrustum& frustum, const Fmatrix& parent );
	virtual void    Move        ( Fvector& amount );
  	virtual bool 	Load		(IReader&);
	virtual void 	Save		(IWriter&);
	virtual bool    GetBox      ( Fbox& box );
};
#endif /*_INCDEF_NavPoint_H_*/


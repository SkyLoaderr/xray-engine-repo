// FCached.h: interface for the FCached class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FCACHED_H__A095957D_9524_4616_BB39_F029F617477E__INCLUDED_)
#define AFX_FCACHED_H__A095957D_9524_4616_BB39_F029F617477E__INCLUDED_
#pragma once

#include "fbasicvisual.h"

class ENGINE_API FCached  : public FBasicVisual
{
public:
	CVertexStream*	VS;
	LPVOID			pVertices;
	DWORD			vCount;
	LPWORD			pIndices;
	DWORD			iCount;

	virtual void	Load	(const char* N, CStream *data, DWORD dwFlags);
	virtual void	Release	();						// Shared memory release
	virtual void	Copy	(FBasicVisual* from);

	FCached();
	virtual ~FCached();
};

void TransferGeometry	(LPVOID vDest, LPVOID vSrc, DWORD vCount, DWORD vStride,
						 LPWORD iDest, LPWORD iSrc, DWORD iCount, DWORD iOffset,
						 Fmatrix* xform = NULL);

#endif // !defined(AFX_FCACHED_H__A095957D_9524_4616_BB39_F029F617477E__INCLUDED_)

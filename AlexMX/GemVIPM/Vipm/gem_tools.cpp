/* Copyright (C) Tom Forsyth, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Tom Forsyth, 2001"
 */

#include "gemvipm.h"

//---------------------------------------------------------------------------------------------------------------
//
//---------------------------------------------------------------------------------------------------------------
void CMyD3DApplication::CollapseAll()
{
	Object *object	= m_pObject;

//.	m_pObject->collect_quadrics();

	while (true){
		// Find the best collapse you can.
		// (how expensive is this? Ohhhh yes).
		float		fBestError			= 1.0e10f;
		MeshEdge	*pedgeBestError		= NULL;
		MeshPt		*pptBestError		= NULL;
		// NL = NewLevel - would force a new level.
		float		fBestErrorNL		= 1.0e10f;
		MeshEdge	*pedgeBestErrorNL	= NULL;
		MeshPt		*pptBestErrorNL		= NULL;
		MeshPt		*ppt;
		MeshEdge	*pedge;

		float		fAverage			= 0.0f;
		int			iAvCount			= 0;

		// Flush the cache, just in case.
		object->FindCollapseError		( NULL, NULL, FALSE );

		for ( ppt = object->CurPtRoot.ListNext(); ppt != NULL; ppt = ppt->ListNext() ){
			// Disallow any pts that are on an edge - shouldn't be collapsing them.
			BOOL bAllowed = TRUE;
			for ( pedge = ppt->FirstEdge(); pedge != NULL; pedge = ppt->NextEdge() ){
				if ( ( pedge->pTri12 == NULL ) || ( pedge->pTri21 == NULL ) ){
					// This edge does not have two tris on it - disallow it.
					bAllowed = FALSE;
					break;
				}
			}
			if ( !bAllowed ) continue;

			BOOL bRequiresNewLevel = FALSE;
			if ( !object->CollapseAllowedForLevel ( ppt, object->iCurSlidingWindowLevel ) ){
				// This collapse would force a new level.
				bRequiresNewLevel = TRUE;
			}

			// collect error
			for ( pedge = ppt->FirstEdge(); pedge != NULL; pedge = ppt->NextEdge() ){
				float fErrorBin = object->FindCollapseError ( ppt, pedge, TRUE );
				iAvCount++;
				fAverage += fErrorBin;
				if ( bRequiresNewLevel ){
					if ( fBestErrorNL > fErrorBin ){
						fBestErrorNL = fErrorBin;
						pedgeBestErrorNL = pedge;
						pptBestErrorNL = ppt;
					}
				}else{
					if ( fBestError > fErrorBin ){
						fBestError = fErrorBin;
						pedgeBestError = pedge;
						pptBestError = ppt;
					}
				}
			}
		}
		fAverage /= (float)iAvCount;

		// Tweak up the NewLevel errors by a factor.
		if ( fBestError > ( fBestErrorNL + fAverage * m_fSlidingWindowErrorTolerance ) ){
			// Despite the boost, it's still the best,
			// so bite the bullet and do the collapse.
			fBestError = fBestErrorNL;
			pedgeBestError = pedgeBestErrorNL;
			pptBestError = pptBestErrorNL;
		}

		//-----------------------------------------------------------------------------------------------------------
		// Do we need to do any collapses?
		// Collapse auto-found edge.
		if ( ( pedgeBestError != NULL ) && ( pptBestError != NULL ) ){
			MeshPt *pKeptPt = pedgeBestError->OtherPt ( pptBestError ); 
			ASSERT ( pKeptPt != NULL );
			object->CreateEdgeCollapse ( pptBestError, pKeptPt );
		}else{
			break;
		}
	}
}


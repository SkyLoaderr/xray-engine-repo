// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000, All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// FREE SOURCE CODE
// http://www.magic-software.com/License/free.pdf

#include <assert.h>
#include "psolid2.h"
#include "segedge.h"

//---------------------------------------------------------------------------
SegmentEdge::SegmentEdge ()
{
    m_pFirst = 0;
}
//---------------------------------------------------------------------------
SegmentEdge::~SegmentEdge ()
{
    while ( m_pFirst )
    {
        TaggedEdge* pSave = m_pFirst->next;
        delete m_pFirst;
        m_pFirst = pSave;
    }
}
//---------------------------------------------------------------------------
void SegmentEdge::AddEdge (const RPoint2& r0, const RPoint2& r1)
{
    assert( r0 != r1 );

    // check if edge is already in list
    TaggedEdge* pEdge;
    for (pEdge = m_pFirst; pEdge; pEdge = pEdge->next)
    {
        if ( r0 == pEdge->r0 && r1 == pEdge->r1 )
            return;
    }

    pEdge = new TaggedEdge;
    pEdge->r0 = r0;
    pEdge->r1 = r1;
    pEdge->next = m_pFirst;
    m_pFirst = pEdge;
}
//---------------------------------------------------------------------------
void SegmentEdge::MergeAppend (SegmentEdge& TEdge)
{
    if ( TEdge.m_pFirst == 0 )
        return;

    // search for end of list
    TaggedEdge* pPrev = 0;
    for (TaggedEdge* pEdge = m_pFirst; pEdge; pEdge = pEdge->next)
    {
        pPrev = pEdge;
    }

    // append TEdge list to 'this' list
    if ( pPrev )
    {
        pPrev->next = TEdge.m_pFirst;
    }
    else
    {
        // 'this' list was empty, appended list is that of TEdge
        m_pFirst = TEdge.m_pFirst;
    }

    TEdge.m_pFirst = 0;
}
//---------------------------------------------------------------------------
void SegmentEdge::MergeUnique (SegmentEdge& TEdge)
{
    TaggedEdge* pUnique = 0;

    for (TaggedEdge* pTEL = TEdge.m_pFirst; pTEL; pTEL = pTEL->next)
    {
        // check if edge is already in 'this' list
        TaggedEdge* pTER;
        for (pTER = m_pFirst; pTER; pTER = pTER->next)
        {
            if ( pTER->r0 == pTEL->r0 && pTER->r1 == pTEL->r1 )
                break;
        }

        if ( pTER == 0 )
        {
            // pTEL is not in 'this' list, add to unique list
            TaggedEdge* pEdge = new TaggedEdge;
            pEdge->r0 = pTEL->r0;
            pEdge->r1 = pTEL->r1;
            pEdge->next = pUnique;
            pUnique = pEdge;
        }
    }

    if ( pUnique )
    {
        // search for end of list
        TaggedEdge* pPrev = 0;
        for (TaggedEdge* pEdge = m_pFirst; pEdge; pEdge = pEdge->next)
        {
            pPrev = pEdge;
        }
        
        // append TEdge list to 'this' list
        pPrev = pUnique;
    }
}
//---------------------------------------------------------------------------
void SegmentEdge::MergeEqual (SegmentEdge& TEdge)
{
    TaggedEdge* pEqual = 0;

    for (TaggedEdge* pTER = m_pFirst; pTER; pTER = pTER->next)
    {
        for (TaggedEdge* pTEL = TEdge.m_pFirst; pTEL; pTEL = pTEL->next)
        {
            if ( pTER->r0 == pTEL->r0 && pTER->r1 == pTEL->r1 )
            {
                // pTER and pTEL are equal edges, add to equal list
                TaggedEdge* pEdge = new TaggedEdge;
                pEdge->r0 = pTEL->r0;
                pEdge->r1 = pTEL->r1;
                pEdge->next = pEqual;
                pEqual = pEdge;
                break;
            }
        }
    }

    // replace 'this' list by equal list
    while ( m_pFirst )
    {
        TaggedEdge* pSave = m_pFirst->next;
        delete m_pFirst;
        m_pFirst = pSave;
    }

    m_pFirst = pEqual;
}
//---------------------------------------------------------------------------
void SegmentEdge::MergeAppendReverse (SegmentEdge& TEdge)
{
    if ( TEdge.m_pFirst == 0 )
        return;

    // reverse order of edge endpoints
    TaggedEdge* pEdge;
    for (pEdge = TEdge.m_pFirst; pEdge; pEdge = pEdge->next)
    {
        RPoint2 save = pEdge->r0;
        pEdge->r0 = pEdge->r1;
        pEdge->r1 = save;
    }

    // search for end of list
    TaggedEdge* pPrev = 0;
    for (pEdge = m_pFirst; pEdge; pEdge = pEdge->next)
    {
        pPrev = pEdge;
    }

    // append TEdge list to 'this' list
    pPrev->next = TEdge.m_pFirst;
    TEdge.m_pFirst = 0;
}
//---------------------------------------------------------------------------
void SegmentEdge::MergeUniqueReverse (SegmentEdge& TEdge)
{
    TaggedEdge* pUnique = 0;
    TaggedEdge* pEdge;

    for (TaggedEdge* pTEL = TEdge.m_pFirst; pTEL; pTEL = pTEL->next)
    {
        // check if edge is already in 'this' list
        TaggedEdge* pTER;
        for (pTER = m_pFirst; pTER; pTER = pTER->next)
        {
            if ( pTER->r0 == pTEL->r0 && pTER->r1 == pTEL->r1 )
                break;
        }

        if ( pTER == 0 )
        {
            // pTEL is not in 'this' list, add to unique list (reversed)
            pEdge = new TaggedEdge;
            pEdge->r0 = pTEL->r1;
            pEdge->r1 = pTEL->r0;
            pEdge->next = pUnique;
            pUnique = pEdge;
        }
    }

    if ( pUnique )
    {
        // search for end of list
        TaggedEdge* pPrev = 0;
        for (pEdge = m_pFirst; pEdge; pEdge = pEdge->next)
        {
            pPrev = pEdge;
        }
        
        // append TEdge list to 'this' list
        pPrev = pUnique;
    }
}
//---------------------------------------------------------------------------
void SegmentEdge::ConvertToPolySolid (PolySolid2& poly)
{
    for (TaggedEdge* pEdge = m_pFirst; pEdge; pEdge = pEdge->next)
    {
        poly.AddEdge(poly.AddVertex(pEdge->r0),poly.AddVertex(pEdge->r1));
    }
}
//---------------------------------------------------------------------------

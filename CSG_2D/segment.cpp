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

#include "line.h"
#include "psolid2.h"
#include "rpoint2.h"
#include "segment.h"
#include "segedge.h"

int Segmentation::m_klein4[4][4] =
{
    { 0, 1, 2, 3 },
    { 1, 0, 3, 2 },
    { 2, 3, 0, 1 },
    { 3, 2, 1, 0 }
};

//---------------------------------------------------------------------------
Segmentation::Segmentation (const Line& line) : m_rLine(line)
{
    m_pFirst = 0;
    m_pLast = 0;
}
//---------------------------------------------------------------------------
Segmentation::~Segmentation ()
{
    while ( m_pFirst )
    {
        TaggedPoint* pSave = m_pFirst->next;
        delete m_pFirst;
        m_pFirst = pSave;
    }
}
//---------------------------------------------------------------------------
void Segmentation::AddPoint (const RPoint2& r, int tag)
{
    Rational order = m_rLine.normal.x*r.y-m_rLine.normal.y*r.x;

    for (TaggedPoint* pPoint = m_pFirst; pPoint; pPoint = pPoint->next)
    {
        if ( order == pPoint->order )
        {
            // point already in segmentation, adjust its tag
            pPoint->tag = m_klein4[tag][pPoint->tag];
            return;
        }

        if ( order < pPoint->order )
            break;
    }


    // point not in segmentation, add it
    TaggedPoint* pNewPoint = new TaggedPoint;
    pNewPoint->order = order;
    pNewPoint->r = r;
    pNewPoint->tag = tag;

    if ( m_pFirst == 0 )
    {
        // add first item to list
        pNewPoint->next = 0;
        pNewPoint->prev = 0;
        m_pFirst = pNewPoint;
        m_pLast = pNewPoint;
    }
    else if ( pPoint == 0 )
    {
        // add item to end of list
        pNewPoint->next = 0;
        pNewPoint->prev = m_pLast;
        m_pLast->next = pNewPoint;
        m_pLast = pNewPoint;
    }
    else if ( pPoint == m_pFirst )
    {
        // add item to front of list
        pNewPoint->next = m_pFirst;
        pNewPoint->prev = 0;
        m_pFirst->prev = pNewPoint;
        m_pFirst = pNewPoint;
    }
    else
    {
        // add item to interior of list
        pNewPoint->next = pPoint;
        pNewPoint->prev = pPoint->prev;
        pPoint->prev->next = pNewPoint;
        pPoint->prev = pNewPoint;
    }
}
//---------------------------------------------------------------------------
void Segmentation::SegmentBy (const PolySolid2& poly)
{
    for (Edge* pEdge = poly.GetEdges(); pEdge; pEdge = pEdge->next)
    {
        const RPoint2& r0 = pEdge->v0->r;
        const RPoint2& r1 = pEdge->v1->r;
        Rational dot0 = m_rLine.normal.x*r0.x+m_rLine.normal.y*r0.y;
        Rational dot1 = m_rLine.normal.x*r1.x+m_rLine.normal.y*r1.y;

        Rational t;
        RPoint2 intr;

        if ( dot0 > m_rLine.constant )
        {
            if ( dot1 > m_rLine.constant )
            {
                // edge does not intersect line
            }
            else if ( dot1 < m_rLine.constant )
            {
                // edge crosses line
                t = (m_rLine.constant-dot0)/(dot1-dot0);
                intr.x = r0.x+t*(r1.x-r0.x);
                intr.y = r0.y+t*(r1.y-r0.y);
                AddPoint(intr,ITAG);
            }
            else  // dot1 == line.constant
            {
                // end point of edge is on line
                AddPoint(r1,PTAG);
            }
        }
        else if ( dot0 < m_rLine.constant )
        {
            if ( dot1 > m_rLine.constant )
            {
                // edge crosses line
                t = (m_rLine.constant-dot0)/(dot1-dot0);
                intr.x = r0.x+t*(r1.x-r0.x);
                intr.y = r0.y+t*(r1.y-r0.y);
                AddPoint(intr,ITAG);
            }
            else if ( dot1 < m_rLine.constant )
            {
                // edge does not intersect line
            }
            else  // dot1 == line.constant
            {
                // end point of edge is on line
                AddPoint(r1,MTAG);
            }
        }
        else  // dot0 == line.constant
        {
            if ( dot1 > m_rLine.constant )
            {
                // end point of edge is on line
                AddPoint(r0,PTAG);
            }
            else if ( dot1 < m_rLine.constant )
            {
                // end point of edge is on line
                AddPoint(r0,MTAG);
            }
            else  // dot1 == line.constant
            {
                // edge is on the line
            }
        }
    }
}
//---------------------------------------------------------------------------
void Segmentation::IntersectWith (const RPoint2& r0, const RPoint2& r1)
{
    Rational order0 = m_rLine.normal.x*r0.y-m_rLine.normal.y*r0.x;
    int tag = OTAG;
    while ( 1 )
    {
        if ( m_pFirst == 0 )
            return;
        if ( order0.Ordering(m_pFirst->order) < 0 )
        {
            TaggedPoint* pItem = new TaggedPoint;
            pItem->order = order0;
            pItem->r = r0;
            pItem->tag = tag;

            pItem->next = m_pFirst;
            pItem->prev = 0;
            m_pFirst->prev = pItem;
            m_pFirst = pItem;
            break;
        }

        tag = m_klein4[tag][m_pFirst->tag];
        TaggedPoint* pSave = m_pFirst;
        m_pFirst = m_pFirst->next;
        delete pSave;
        if ( m_pFirst )
            m_pFirst->prev = 0;
    }

    Rational order1 = m_rLine.normal.x*r1.y-m_rLine.normal.y*r1.x;
    tag = OTAG;
    while ( 1 )
    {
        if ( m_pLast == 0 )
            return;
        if ( order1.Ordering(m_pLast->order) > 0 )
        {
            TaggedPoint* pItem = new TaggedPoint;
            pItem->order = order1;
            pItem->r = r1;
            pItem->tag = tag;

            pItem->prev = m_pLast;
            pItem->next = 0;
            m_pLast->next = pItem;
            m_pLast = pItem;
            break;
        }

        tag = m_klein4[tag][m_pLast->tag];
        TaggedPoint* pSave = m_pLast;
        m_pLast = m_pLast->prev;
        delete pSave;
        if ( m_pLast )
            m_pLast->next = 0;
    }
}
//---------------------------------------------------------------------------
void Segmentation::ConvertToEdges (SegmentEdge TEdge[4])
{
    TaggedPoint* pCurr = m_pFirst;
    TaggedPoint* pNext = m_pFirst->next;

    for (int tag = OTAG; pNext; pCurr = pNext, pNext = pNext->next)
    {
        tag = m_klein4[tag][pCurr->tag];
        TEdge[tag].AddEdge(pCurr->r,pNext->r);
    }
}
//---------------------------------------------------------------------------

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
#include "segedge.h"
#include "segment.h"

//---------------------------------------------------------------------------
PolySolid2::PolySolid2 ()
{
    m_pVertex = 0;
    m_pEdge = 0;
}
//---------------------------------------------------------------------------
PolySolid2::PolySolid2 (const PolySolid2& poly)
{
    m_pVertex = 0;
    m_pEdge = 0;
    operator=(poly);
}
//---------------------------------------------------------------------------
PolySolid2::~PolySolid2 ()
{
    RemoveAllVertices();
    RemoveAllEdges();
}
//---------------------------------------------------------------------------
PolySolid2& PolySolid2::operator= (const PolySolid2& poly)
{
    RemoveAllVertices();
    RemoveAllEdges();

    for (Edge* pEdge = poly.m_pEdge; pEdge; pEdge = pEdge->next)
    {
        AddEdge(AddVertex(pEdge->v0->r),AddVertex(pEdge->v1->r));
    }

    return *this;
}
//---------------------------------------------------------------------------
Vertex* PolySolid2::AddVertex (const RPoint2& r)
{
    Vertex* pVertex;
    for (pVertex = m_pVertex; pVertex; pVertex = pVertex->next)
    {
        if ( pVertex->r == r )
            return pVertex;
    }

    pVertex = new Vertex;
    pVertex->r.x = r.x;
    pVertex->r.y = r.y;
    pVertex->next = m_pVertex;
    m_pVertex = pVertex;
    return m_pVertex;
}
//---------------------------------------------------------------------------
Edge* PolySolid2::AddEdge (const Vertex* v0, const Vertex* v1)
{
    Edge* pEdge;
    for (pEdge = m_pEdge; pEdge; pEdge = pEdge->next)
    {
        if ( (pEdge->v0 == v0 && pEdge->v1 == v1)
        ||   (pEdge->v0 == v1 && pEdge->v1 == v0) )
        {
            return pEdge;
        }
    }

    pEdge = new Edge;
    pEdge->v0 = v0;
    pEdge->v1 = v1;
    pEdge->next = m_pEdge;
    m_pEdge = pEdge;
    return m_pEdge;
}
//---------------------------------------------------------------------------
void PolySolid2::RemoveAllVertices ()
{
    while ( m_pVertex )
    {
        Vertex* pSave = m_pVertex->next;
        delete m_pVertex;
        m_pVertex = pSave;
    }
}
//---------------------------------------------------------------------------
void PolySolid2::RemoveAllEdges ()
{
    while ( m_pEdge )
    {
        Edge* pSave = m_pEdge->next;
        delete m_pEdge;
        m_pEdge = pSave;
    }
}
//---------------------------------------------------------------------------
void PolySolid2::SegmentAll (PolySolid2& poly, SegmentEdge TEdge[4])
{
    for (Edge* pEdge = poly.m_pEdge; pEdge; pEdge = pEdge->next)
    {
        // construct line containing edge
        const RPoint2& r0 = pEdge->v0->r;
        const RPoint2& r1 = pEdge->v1->r;
        Line line;
        line.normal.x = r1.y-r0.y;
        line.normal.y = r0.x-r1.x;
        line.constant = line.normal.x*r0.x+line.normal.y*r0.y;
        
        // segment line against 'this' polygon
        Segmentation seg(line);
        seg.SegmentBy(*this);

        if ( seg.HasPoints() )
        {
            // compute intersection of edge and segmentation
            assert( r0 != r1 );
            seg.IntersectWith(r0,r1);

            if ( seg.HasPoints() )
            {
                // compute tagged edge lists
                seg.ConvertToEdges(TEdge);
            }
        }

        if ( !seg.HasPoints() )
        {
            // line of edge did not intersect 'this', so must be outside
            TEdge[OTAG].AddEdge(r0,r1);
        }
    }
}
//---------------------------------------------------------------------------
PolySolid2 PolySolid2::operator& (PolySolid2& poly)
{
    // intersection, P AND Q

    // segment the polysolids
    SegmentEdge TEdgeL[4], TEdgeR[4];
    SegmentAll(poly,TEdgeR);
    poly.SegmentAll(*this,TEdgeL);

    // all ITAG edges are in the intersection
    TEdgeL[ITAG].MergeAppend(TEdgeR[ITAG]);

    // all PTAG edges are in the intersection, avoid duplicates
    TEdgeL[PTAG].MergeUnique(TEdgeR[PTAG]);

    // final merge
    TEdgeL[ITAG].MergeAppend(TEdgeL[PTAG]);

    // convert segment edges to a polysolid
    PolySolid2 result;
    TEdgeL[ITAG].ConvertToPolySolid(result);

    return result;
}
//---------------------------------------------------------------------------
PolySolid2 PolySolid2::operator| (PolySolid2& poly)
{
    // union, P OR Q

    // segment the polysolids
    SegmentEdge TEdgeL[4], TEdgeR[4];
    SegmentAll(poly,TEdgeR);
    poly.SegmentAll(*this,TEdgeL);

    // all OTAG edges are in the union
    TEdgeL[OTAG].MergeAppend(TEdgeR[OTAG]);

    // all MTAG-MTAG edges are in the union
    TEdgeL[MTAG].MergeEqual(TEdgeR[MTAG]);

    // all PTAG-PTAG edges are in the union
    TEdgeL[PTAG].MergeEqual(TEdgeR[PTAG]);

    // final merge
    TEdgeL[OTAG].MergeAppend(TEdgeL[MTAG]);
    TEdgeL[OTAG].MergeAppend(TEdgeL[PTAG]);

    // convert segment edges to a polysolid
    PolySolid2 result;
    TEdgeL[OTAG].ConvertToPolySolid(result);

    return result;
}
//---------------------------------------------------------------------------
PolySolid2 PolySolid2::operator- (PolySolid2& poly)
{
    // difference, P-Q = P AND -Q

    // segment the polysolids
    SegmentEdge TEdgeL[4], TEdgeR[4];
    SegmentAll(poly,TEdgeR);
    poly.SegmentAll(*this,TEdgeL);

    // all OTAG edges of P and ITAG edges of Q are in the difference
    TEdgeL[OTAG].MergeAppendReverse(TEdgeR[ITAG]);

    // all PTAG edges are in the difference, avoid duplicates
    TEdgeL[MTAG].MergeUniqueReverse(TEdgeR[PTAG]);

    // final merge
    TEdgeL[OTAG].MergeAppend(TEdgeL[MTAG]);

    // convert segment edges to a polysolid
    PolySolid2 result;
    TEdgeL[OTAG].ConvertToPolySolid(result);

    return result;
}
//---------------------------------------------------------------------------
PolySolid2 PolySolid2::operator^ (PolySolid2& poly)
{
    // exclusive or, P XOR Q = (P-Q) OR (Q-P)
    return (*this - poly) | (poly - *this);
}
//---------------------------------------------------------------------------

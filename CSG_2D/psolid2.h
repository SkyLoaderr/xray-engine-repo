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

#ifndef PSOLID2_H
#define PSOLID2_H

#include "rpoint2.h"

class Vertex
{
public:
    RPoint2 r;
    Vertex* next;
};

class Edge
{
public:
    const Vertex* v0;
    const Vertex* v1;
    Edge* next;
};

class SegmentEdge;

// NOTE:  This code assumes that the bounded polysolid region is
// always to the right of each directed edge <v0,v1> in 'poly'.
// The edge direction is v1-v0 = (a,b) and the normal pointing inwards
// is (b,-a).

class PolySolid2
{
public:
    // construction and destruction
    PolySolid2 ();
    PolySolid2 (const PolySolid2& poly);
    ~PolySolid2 ();

    PolySolid2& operator= (const PolySolid2& poly);

    // vertices
    Vertex* AddVertex (const RPoint2& r);
    Vertex* GetVertices () const { return m_pVertex; }
    void RemoveAllVertices ();

    // edges
    Edge* AddEdge (const Vertex* v0, const Vertex* v1);
    Edge* GetEdges () const { return m_pEdge; }
    void RemoveAllEdges ();

    // boolean operations

    // intersection
    PolySolid2 operator& (PolySolid2& poly);

    // union
    PolySolid2 operator| (PolySolid2& poly);

    // difference
    PolySolid2 operator- (PolySolid2& poly);

    // exclusive or
    PolySolid2 operator^ (PolySolid2& poly);

protected:
    Vertex* m_pVertex;
    Edge* m_pEdge;

    void SegmentAll (PolySolid2& poly, SegmentEdge TEdge[4]);
};

#endif

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

#ifndef SEGEDGE_H
#define SEGEDGE_H

#include "rpoint2.h"

class PolySolid2;

class SegmentEdge
{
public:
    SegmentEdge ();
    ~SegmentEdge ();

    bool HasEdges () { return m_pFirst != 0; }
    void AddEdge (const RPoint2& r0, const RPoint2& r1);

    // for union, intersection
    void MergeAppend (SegmentEdge& TEdge);
    void MergeUnique (SegmentEdge& TEdge);
    void MergeEqual  (SegmentEdge& TEdge);

    // for difference, exclusive or
    void MergeAppendReverse (SegmentEdge& TEdge);
    void MergeUniqueReverse (SegmentEdge& TEdge);

    void ConvertToPolySolid (PolySolid2& poly);

protected:
    typedef struct TaggedEdge
    {
        RPoint2 r0, r1;
        TaggedEdge* next;
    }
    TaggedEdge;

    TaggedEdge* m_pFirst;
};

#endif

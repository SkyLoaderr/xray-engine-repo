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

#ifndef SEGMENT_H
#define SEGMENT_H

class Line;
class RPoint2;
class PolySolid2;
class SegmentEdge;

enum { OTAG, ITAG, MTAG, PTAG };

class Segmentation
{
public:
    Segmentation (const Line& line);
    ~Segmentation ();

    bool HasPoints () { return m_pFirst != 0; }

    void AddPoint (const RPoint2& r, int tag);

    void SegmentBy (const PolySolid2& poly);
    void IntersectWith (const RPoint2& r0, const RPoint2& r1);
    void ConvertToEdges (SegmentEdge TEdge[4]);

protected:
    typedef struct TaggedPoint
    {
        Rational order;
        RPoint2 r;
        int tag;
        TaggedPoint* next;
        TaggedPoint* prev;
    }
    TaggedPoint;

    const Line& m_rLine;
    TaggedPoint* m_pFirst;
    TaggedPoint* m_pLast;
    static int m_klein4[4][4];
};

#endif

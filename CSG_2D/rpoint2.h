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

#ifndef RPOINT2_H
#define RPOINT2_H

#include "rational.h"

class RPoint2
{
public:
    Rational x, y;

    bool operator== (const RPoint2& pt) const
    {
        return ( pt.x == x && pt.y == y );
    }

    bool operator!= (const RPoint2& pt) const
    {
        return !(*this == pt);
    }
};

#endif

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

#include "rectdiff.h"

//---------------------------------------------------------------------------
IRectangleList* CopyList (const IRectangleList* A)
{
    IRectangleList* list = 0;

    for (const IRectangleList* alist = A; alist; alist = alist->next)
    {
        IRectangleList* tmp = new IRectangleList;
        tmp->R = alist->R;
        tmp->next = list;
        list = tmp;
    }

    return list;
}
//---------------------------------------------------------------------------
void DeleteList (IRectangleList*& list)
{
    while ( list )
    {
        IRectangleList* tmp = list->next;
        delete list;
        list = tmp;
    }
}
//---------------------------------------------------------------------------
IRectangleList* Difference (const IRectangle& R0, const IRectangle& R1)
{
    IRectangleList* list = 0;

    if ( R0.xmax <= R1.xmin || R1.xmax <= R0.xmin
    ||   R0.ymax <= R1.ymin || R1.ymax <= R0.ymin )
    {
        // no positive-area intersection
        list = new IRectangleList;
        list->R = R0;
        list->next = 0;
        return list;
    }

    IRectangle current = R0;
    IRectangleList* tmp;

    if ( current.xmin < R1.xmin )
    {
        tmp = new IRectangleList;
        tmp->R.xmin = current.xmin;
        tmp->R.xmax = R1.xmin;
        tmp->R.ymin = current.ymin;
        tmp->R.ymax = current.ymax;
        tmp->next = list;
        list = tmp;
        current.xmin = R1.xmin;
    }

    if ( current.xmax > R1.xmax )
    {
        tmp = new IRectangleList;
        tmp->R.xmin = R1.xmax;
        tmp->R.xmax = current.xmax;
        tmp->R.ymin = current.ymin;
        tmp->R.ymax = current.ymax;
        tmp->next = list;
        list = tmp;
        current.xmax = R1.xmax;
    }

    if ( current.ymin < R1.ymin )
    {
        tmp = new IRectangleList;
        tmp->R.xmin = current.xmin;
        tmp->R.xmax = current.xmax;
        tmp->R.ymin = current.ymin;
        tmp->R.ymax = R1.ymin;
        tmp->next = list;
        list = tmp;
        current.ymin = R1.ymin;
    }

    if ( current.ymax > R1.ymax )
    {
        tmp = new IRectangleList;
        tmp->R.xmin = current.xmin;
        tmp->R.xmax = current.xmax;
        tmp->R.ymin = R1.ymax;
        tmp->R.ymax = current.ymax;
        tmp->next = list;
        list = tmp;
        current.ymax = R1.ymax;
    }

    return list;
}
//---------------------------------------------------------------------------
IRectangleList* Difference (const IRectangleList* A, const IRectangle& R)
{
    IRectangleList* list = 0;

    for (const IRectangleList* alist = A; alist; alist = alist->next)
    {
        IRectangleList* diff = Difference(alist->R,R);
        if ( diff )
        {
            // attach end of diff to beginning of list
            IRectangleList* last = diff;
            while ( last->next )
                last = last->next;
            last->next = list;
            list = diff;
        }
    }

    return list;
}
//---------------------------------------------------------------------------
IRectangleList* Difference (const IRectangleList* A, const IRectangleList* B)
{
    // copy A rectangles
    IRectangleList* list = CopyList(A);

    // subtract out B rectangles
    for (const IRectangleList* blist = B; blist; blist = blist->next)
    {
        IRectangleList* diff = Difference(list,blist->R);
        DeleteList(list);
        if ( diff )
            list = diff;
        else
            break;
    }

    return list;
}
//---------------------------------------------------------------------------

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

#include <windows.h>
#include <stdlib.h>
#include <math.h>
#include "rectdiff.h"
 
#define WSIZE 256
#define RAND (rand() % WSIZE)

//---------------------------------------------------------------------------
IRectangleList* GenerateRectangles (int n)
{
    IRectangleList* list = 0;

    for (int i = 0; i < n; i++)
    {
        IRectangleList* tmp = new IRectangleList;
        tmp->R.xmin = RAND;
        tmp->R.ymin = RAND;
        tmp->R.xmax = tmp->R.xmin + (RAND % 32) + 1;
        tmp->R.ymax = tmp->R.ymin + (RAND % 32) + 1; 
        tmp->next = list;
        list = tmp;
    }

    return list;
}
//---------------------------------------------------------------------------
void DrawSets (HDC hDC, const IRectangleList* A, const IRectangleList* B,
    const IRectangleList* D)
{
    HBRUSH hABrush = CreateSolidBrush(RGB(255,0,0));
    HBRUSH hBBrush = CreateSolidBrush(RGB(0,0,255));
    HBRUSH hDBrush = CreateSolidBrush(RGB(255,0,255));
    RECT rect;
    const IRectangleList* list;

    for (list = A; list; list = list->next)
    {
        rect.left = list->R.xmin;
        rect.right = list->R.xmax;
        rect.bottom = list->R.ymin;
        rect.top = list->R.ymax;
        FillRect(hDC,&rect,hABrush);
    }

    for (list = B; list; list = list->next)
    {
        rect.left = list->R.xmin;
        rect.right = list->R.xmax;
        rect.bottom = list->R.ymin;
        rect.top = list->R.ymax;
        FillRect(hDC,&rect,hBBrush);
    }

    for (list = D; list; list = list->next)
    {
        rect.left = list->R.xmin;
        rect.right = list->R.xmax;
        rect.bottom = list->R.ymin;
        rect.top = list->R.ymax;
        MoveToEx(hDC,rect.left,rect.top,0);
        LineTo(hDC,rect.right,rect.top);
        LineTo(hDC,rect.right,rect.bottom);
        LineTo(hDC,rect.left,rect.bottom);
        LineTo(hDC,rect.left,rect.top);
    }
}
//---------------------------------------------------------------------------
long FAR PASCAL 
WndProc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static int numA = 16;
    static int numB = 8;
    static IRectangleList* A = 0;
    static IRectangleList* B = 0;
    static IRectangleList* D = 0;

    switch ( message ) 
    {
    case WM_CREATE:
    {
        A = GenerateRectangles(numA);
        B = GenerateRectangles(numB);
        D = Difference(A,B);
        return 0;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hDC = BeginPaint(hWnd,&ps);
        DrawSets(hDC,A,B,D);
        EndPaint(hWnd,&ps);
        return 0;
    }
    case WM_CHAR:
    {
        switch ( wParam )
        {
        case 'n':  // next sets
            DeleteList(A);
            DeleteList(B);
            DeleteList(D);
            A = GenerateRectangles(numA);
            B = GenerateRectangles(numB);
            D = Difference(A,B);
            InvalidateRect(hWnd,NULL,TRUE);
            break;
        case 'q':
        case 'Q':
        case VK_ESCAPE:
            PostMessage(hWnd,WM_DESTROY,0,0);
        }
        return 0;
    }
    case WM_DESTROY:
    {
        DeleteList(A);
        DeleteList(B);
        DeleteList(D);
        PostQuitMessage(0);
        return 0;
    }
    }
    return DefWindowProc(hWnd,message,wParam,lParam);
}
//---------------------------------------------------------------------------
int PASCAL WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
{
    static char szAppName[] = "Difference of Unions of Rectangles";

    WNDCLASS wc;
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance;
    wc.hIcon         = LoadIcon(NULL,IDI_APPLICATION);
    wc.hCursor       = LoadCursor(NULL,IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = szAppName;
    RegisterClass(&wc);

    RECT rect = { 0, 0, WSIZE-1, WSIZE-1 };
    AdjustWindowRect(&rect,WS_OVERLAPPEDWINDOW,FALSE);

    HWND hWnd = CreateWindow (
        szAppName,
        szAppName,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        rect.right-rect.left+1,
        rect.bottom-rect.top+1,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    ShowWindow(hWnd,nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while ( TRUE )
    {
        if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
        {
            if ( msg.message == WM_QUIT )
                break;

            HACCEL hAccel = NULL;
            if ( !TranslateAccelerator( hWnd, hAccel, &msg ) )
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
    }
    return msg.wParam;
}
//---------------------------------------------------------------------------

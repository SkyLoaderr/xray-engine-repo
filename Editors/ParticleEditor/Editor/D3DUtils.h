//----------------------------------------------------
// file: D3DUtils.h
//----------------------------------------------------

#ifndef _INCDEF_D3DUtils_H_
#define _INCDEF_D3DUtils_H_
//----------------------------------------------------

namespace DU{
    void OnDeviceCreate	();
    void OnDeviceDestroy();

    void OnRender		();

	void UpdateGrid(int number_of_cell, float square_size, int subdiv=10);

    //----------------------------------------------------
    void DrawCross(const Fvector& p, float szx1, float szy1, float szz1, float szx2, float szy2, float szz2, DWORD clr, bool bRot45=false);
    void DrawEntity(DWORD clr, Shader* s);
    void DrawFlag(const Fvector& p, float heading, float height, float sz, float sz_fl, DWORD clr, bool bDrawEntity);
    void DrawRomboid(const Fvector& p, float radius, DWORD clr);

    void DrawSpotLight(const Fvector& p, const Fvector& d, float range, float phi, DWORD clr);
    void DrawDirectionalLight(const Fvector& p, const Fvector& d, float radius, float range, DWORD clr);
    void DrawPointLight(const Fvector& p, float radius, DWORD clr);

    void DrawSound(const Fvector& p, float radius, DWORD clr);
    void DrawLineSphere(const Fvector& p, float radius, DWORD clr, bool bCross);

	void dbgDrawPlacement(const Fvector& p, int sz, DWORD clr, LPCSTR caption=0, DWORD clr_font=0xffffffff);
    void dbgDrawVert(const Fvector& p0, DWORD clr, LPCSTR caption=0);
    void dbgDrawEdge(const Fvector& p0,	const Fvector& p1, DWORD clr, LPCSTR caption=0);
    void dbgDrawFace(const Fvector& p0,	const Fvector& p1, const Fvector& p2, DWORD clr, LPCSTR caption=0);

    void DrawLine(const Fvector& p0,	const Fvector& p1, DWORD clr);
    IC void DrawLine(const Fvector* p, DWORD clr){DrawLine(p[0],p[1],clr);}
    void DrawLink(const Fvector& p0, const Fvector& p1, float sz, DWORD clr);
    IC void DrawFaceNormal(const Fvector& p0, const Fvector& p1, const Fvector& p2, float size, DWORD clr){
        Fvector N,C,P; N.mknormal(p0,p1,p2); C.set(p0);C.add(p1);C.add(p2);C.div(3);
        P.mad(C,N,size);
        DrawLine(C,P,clr);}
    IC void DrawFaceNormal(const Fvector* p, float size, DWORD clr){DrawFaceNormal(p[0],p[1],p[2],size,clr);}
    IC void DrawFaceNormal(const Fvector& C, const Fvector& N, float size, DWORD clr){
        Fvector P; P.mad(C,N,size);DrawLine(C,P,clr);}
    void DrawSelectionBox(const Fvector& center, const Fvector& size, DWORD* c=0);
    IC void DrawSelectionBox(const Fbox& box, DWORD* c=0){
        Fvector S,C;
        box.getsize(S);
        box.getcenter(C);
        DrawSelectionBox(C,S,c);
    }
    void DrawIdentBox(bool bSolid, bool bWire, DWORD* c=0);
    void DrawBox    (const Fvector& offs, const Fvector& Size, bool bWire, DWORD c);
    void DrawPlane  (const Fvector& center, const Fvector2& scale, const Fvector& rotate, DWORD c, bool bCull, bool bBorder, DWORD cb);

    void DrawGrid	();
    void DrawPivot	(const Fvector& pos, float sz=5.f);
	void DrawAxis	(const Fmatrix& T);
	void DrawObjectAxis(const Fmatrix& T);
	void DrawSelectionRect(const Ipoint& m_SelStart, const Ipoint& m_SelEnd);

    void DrawPrimitiveL(D3DPRIMITIVETYPE pt, DWORD pc, Fvector* vertices, int vc, DWORD color, bool bCull, bool bCycle);
    void DrawPrimitiveTL(D3DPRIMITIVETYPE pt, DWORD pc, FVF::TL* vertices, int vc, bool bCull, bool bCycle);
    void DrawPrimitiveLIT(D3DPRIMITIVETYPE pt, DWORD pc, FVF::LIT* vertices, int vc, bool bCull, bool bCycle);
};
//----------------------------------------------------
#endif /*_INCDEF_D3DUtils_H_*/


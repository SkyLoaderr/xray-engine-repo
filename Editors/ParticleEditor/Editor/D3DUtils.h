//----------------------------------------------------
// file: D3DUtils.h
//----------------------------------------------------

#ifndef D3DUtilsH
#define D3DUtilsH
//----------------------------------------------------

namespace DU{
    void OnDeviceCreate	();
    void OnDeviceDestroy();

    void OnRender		();

	void UpdateGrid(int number_of_cell, float square_size, int subdiv=10);

    //----------------------------------------------------
    void DrawCross(const Fvector& p, float szx1, float szy1, float szz1, float szx2, float szy2, float szz2, u32 clr, bool bRot45=false);
    void DrawEntity(u32 clr, Shader* s);
    void DrawFlag(const Fvector& p, float heading, float height, float sz, float sz_fl, u32 clr, bool bDrawEntity);
    void DrawRomboid(const Fvector& p, float radius, u32 clr);

    void DrawSpotLight(const Fvector& p, const Fvector& d, float range, float phi, u32 clr);
    void DrawDirectionalLight(const Fvector& p, const Fvector& d, float radius, float range, u32 clr);
    void DrawPointLight(const Fvector& p, float radius, u32 clr);

    void DrawSound(const Fvector& p, float radius, u32 clr);
    void DrawLineSphere(const Fvector& p, float radius, u32 clr, bool bCross);
    void DrawSphere(const Fvector& p, float radius, u32 clr);

	void dbgDrawPlacement(const Fvector& p, int sz, u32 clr, LPCSTR caption=0, u32 clr_font=0xffffffff);
    void dbgDrawVert(const Fvector& p0, u32 clr, LPCSTR caption=0);
    void dbgDrawEdge(const Fvector& p0,	const Fvector& p1, u32 clr, LPCSTR caption=0);
    void dbgDrawFace(const Fvector& p0,	const Fvector& p1, const Fvector& p2, u32 clr, LPCSTR caption=0);

    void DrawLine(const Fvector& p0,	const Fvector& p1, u32 clr);
    IC void DrawLine(const Fvector* p, u32 clr){DrawLine(p[0],p[1],clr);}
    void DrawLink(const Fvector& p0, const Fvector& p1, float sz, u32 clr);
    IC void DrawFaceNormal(const Fvector& p0, const Fvector& p1, const Fvector& p2, float size, u32 clr){
        Fvector N,C,P; N.mknormal(p0,p1,p2); C.set(p0);C.add(p1);C.add(p2);C.div(3);
        P.mad(C,N,size);
        DrawLine(C,P,clr);}
    IC void DrawFaceNormal(const Fvector* p, float size, u32 clr){DrawFaceNormal(p[0],p[1],p[2],size,clr);}
    IC void DrawFaceNormal(const Fvector& C, const Fvector& N, float size, u32 clr){
        Fvector P; P.mad(C,N,size);DrawLine(C,P,clr);}
    void DrawSelectionBox(const Fvector& center, const Fvector& size, u32* c=0);
    IC void DrawSelectionBox(const Fbox& box, u32* c=0){
        Fvector S,C;
        box.getsize(S);
        box.getcenter(C);
        DrawSelectionBox(C,S,c);
    }
    void DrawIdentBox(bool bSolid, bool bWire, u32 clr);
    void DrawBox    (const Fvector& offs, const Fvector& Size, bool bWire, u32 c);
    void DrawPlane  (const Fvector& center, const Fvector2& scale, const Fvector& rotate, u32 c, bool bCull, bool bBorder, u32 cb);

    void DrawSafeRect();
    void DrawGrid	();
    void DrawPivot	(const Fvector& pos, float sz=5.f);
	void DrawAxis	(const Fmatrix& T);
	void DrawObjectAxis(const Fmatrix& T);
	void DrawSelectionRect(const Ivector2& m_SelStart, const Ivector2& m_SelEnd);

    void DrawPrimitiveL(D3DPRIMITIVETYPE pt, u32 pc, Fvector* vertices, int vc, u32 color, bool bCull, bool bCycle);
    void DrawPrimitiveTL(D3DPRIMITIVETYPE pt, u32 pc, FVF::TL* vertices, int vc, bool bCull, bool bCycle);
    void DrawPrimitiveLIT(D3DPRIMITIVETYPE pt, u32 pc, FVF::LIT* vertices, int vc, bool bCull, bool bCycle);
};
//----------------------------------------------------
#endif /*_INCDEF_D3DUtils_H_*/


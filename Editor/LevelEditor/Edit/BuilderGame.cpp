//----------------------------------------------------
// file: BuilderGame.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "Scene.h"
#include "RPoint.h"
#include "Event.h"
#include "net_utils.h"
#include "xrMessages.h"

bool SceneBuilder::BuildGame()
{
    CFS_Memory F;
	int chunk = 0;
    // add event
	if (Scene.ObjCount(OBJCLASS_EVENT)) {
		ObjectIt i = Scene.FirstObj(OBJCLASS_EVENT);
        ObjectIt last  = Scene.LastObj(OBJCLASS_EVENT);
        AnsiString temp;
        for(;i!=last;i++){
            CEvent *E = (CEvent*)(*i);
            Fbox B;				E->GetBox(B);
			Fvector center;		B.getcenter(center);
            Fmatrix e_matrix; 	e_matrix.translate(center); e_matrix.invert();

            NET_Packet Packet;
            Packet.w_begin		(M_SPAWN);
            Packet.w_string		("g_event");
            Packet.w_string		(E->GetName());
            Packet.w_u8 		(0xFE);
            Packet.w_vec3		(center);		// возможно нужно будет юзать локальную СК
            Fvector a; a.set	(0,0,0);
            Packet.w_vec3		(a);
            Packet.w_u16		(0);
            WORD fl 			= M_SPAWN_OBJECT_ACTIVE;
            Packet.w_u16		(fl);

            u32	position		= Packet.w_tell	();
            Packet.w_u16		(0);
            // spawn info
            // cform's
            // -count
            Packet.w_u8 		(u8(E->m_Forms.size()));
            // --elements
            for (CEvent::FormIt f_it=E->m_Forms.begin(); f_it!=E->m_Forms.end(); f_it++){
	            Packet.w_u8 	(u8(f_it->m_eType));
            	switch (f_it->m_eType){
                case CEvent::efSphere:	{ Fsphere S; S.set(f_it->vPosition,f_it->vSize.magnitude());Packet.w_vec3(S.P);Packet.w_float(S.R); }break;
                case CEvent::efBox:		{ Fmatrix T; f_it->GetTransform(T); T.mul2(e_matrix); Packet.w_matrix(T); }break;
                }
            }
            // action's
            // -count
            Packet.w_u8 		(u8(E->m_Actions.size()));
            // --elements
            for (CEvent::ActionIt a_it=E->m_Actions.begin(); a_it!=E->m_Actions.end(); a_it++){
	            Packet.w_u8 	(u8(a_it->type));
				Packet.w_u16 	(a_it->count);
				Packet.w_u64 	(a_it->clsid);
				Packet.w_string	(a_it->event.c_str());
            }
            // data size
            u16 size			= u16(Packet.w_tell()-position);
            Packet.w_seek		(position,&size,sizeof(u16));

            F.open_chunk		(chunk);
            F.write				(Packet.B.data,Packet.B.count);
            F.close_chunk		();

            chunk++;
		}
	}
	// -- add spawn entity--
	if (Scene.ObjCount(OBJCLASS_RPOINT)) {
		ObjectIt i = Scene.FirstObj(OBJCLASS_RPOINT);
        ObjectIt last  = Scene.LastObj(OBJCLASS_RPOINT);
        AnsiString temp;
        for(;i!=last;i++){
            CRPoint *rpt = (CRPoint *)(*i);
            if (CRPoint::etEntity==rpt->m_Type){
			    NET_Packet Packet;
                Packet.w_begin		(M_SPAWN);
				Packet.w_string		(rpt->m_EntityRefs);
				Packet.w_string		(rpt->GetName());
   				Packet.w_u8 		(0xFE);
                Packet.w_vec3		(rpt->m_Position);
                Fvector a; a.set	(0,rpt->m_fHeading,0);
                Packet.w_vec3		(a);
                Packet.w_u16		(0);
                WORD fl 			= (rpt->m_Flags.bActive)?M_SPAWN_OBJECT_ACTIVE:0;
                Packet.w_u16		(fl);

                u32	position		= Packet.w_tell	();
				Packet.w_u16		(0);
                // spawn info
                Packet.w_u8 		(u8(rpt->m_dwTeamID));
                Packet.w_u8 		(u8(rpt->m_dwSquadID));
                Packet.w_u8 		(u8(rpt->m_dwGroupID));
                // data size
				u16 size			= u16(Packet.w_tell()-position);
                Packet.w_seek		(position,&size,sizeof(u16));

                F.open_chunk		(chunk);
                F.write				(Packet.B.data,Packet.B.count);
                F.close_chunk		();

            	chunk++;
            }
		}
	}
    if (chunk){
	    AnsiString lev_spawn="level.spawn";
    	m_LevelPath.Update(lev_spawn);
	    F.SaveTo(lev_spawn.c_str(),0);
    }
}


//----------------------------------------------------
// file: BuilderGame.cpp
//----------------------------------------------------

#include "stdafx.h"
#pragma hdrstop

#include "Builder.h"
#include "Scene.h"
#include "RPoint.h"
#include "net_utils.h"
#include "xrMessages.h"

bool SceneBuilder::BuildGame()
{
    CFS_Memory F;
	int chunk = 0;
	// -- add spawn entity--
	if (Scene.ObjCount(OBJCLASS_RPOINT)) {
		ObjectIt i = Scene.FirstObj(OBJCLASS_RPOINT);
        ObjectIt _E  = Scene.LastObj(OBJCLASS_RPOINT);
        AnsiString temp;
        for(;i!=_E;i++){
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

				Packet.w_u16		(0);
                u32	position		= Packet.w_tell	();
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

 
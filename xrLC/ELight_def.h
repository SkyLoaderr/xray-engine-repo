//----------------------------------------------------
// file: ELightDef.h
//----------------------------------------------------

#ifndef ELightDefH
#define ELightDefH

#define LCONTROL_HEMI			"$hemi"				// hemisphere
#define LCONTROL_SUN			"$sun"				// sun
#define LCONTROL_STATIC 		"$static"			// all other static lights

#define LIGHT_EXPORT_VERSION  	0x0000				// export current version

#define LIGHT_EXPORT_FILE_NAME 	"level.lights"		// export file name

namespace ELight{
	enum EFlags{
		flAffectStatic	= (1<<0),
		flAffectDynamic	= (1<<1),
		flProcedural	= (1<<2),
        flBreaking 		= (1<<3),
        flPointFuzzy	= (1<<4),
        flCastShadow	= (1<<5),
    };

    enum EType{
    	ltPointR1	= 1,
    	ltSpotR1  	= 2,
        ltDirectR1	= 3,
    	ltPointR2	= 4,
    	ltSpotR2  	= 5,
        ltMaxCount,
    	lt_max_type = u32(-1),
    };

    // export params
    enum{
        LIGHT_EXPORT_CHUNK_VERSION			= 0x0000,
        LIGHT_EXPORT_CHUNK_LIGHTS			= 0x0001,
    };
};

// light export : format
/*
    chunk 	LIGHT_EXPORT_CHUNK_VERSION
    {
	    u16		(LIGHT_EXPORT_VERSION);
    }
        
    chunk 	LIGHT_EXPORT_CHUNK_LIGHTS
    {
    	for all types
        {
        	chunk TYPE
            {
            	chunk k++
                {
                    stringZ	    (L->Name);										    // light name
                    fvector3    (L->FTransform.c);								    // position
                    fvector3    (L->FTransform.k); 								    // direction
                    fvector3    (L->FTransform.i);                                  // right
                    float	    (L->m_Cone);                                        // cone angle
                    float	    (L->m_Range);                                       // range
                    float	    (L->m_VirtualSize);                                 // virtual size
                    fcolor	    (L->m_Color);                                       // color
                    float	    (L->m_Brightness);                                  // brightness
                    stringZ	    (L->m_pAnimRef?L->m_pAnimRef->cName:"");            // color animator name (if exist)
                    stringZ	    (L->m_FalloffTex.c_str());                          // falloff texture
                    stringZ	    (FindLightControl(L->m_LControl)->name.c_str());    // light control name (LCONTROL_)
                    u32		    (L->m_Flags.get());								    // flags
                }
            }
        }
    }
*/

#endif


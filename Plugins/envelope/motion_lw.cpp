#include "stdafx.h"
#include "motion.h"
#include "envelope.h"

#ifdef LWO_EXPORTS
extern "C"	LWItemInfo		*g_iteminfo;
extern "C"	LWChannelInfo	*g_chinfo;
extern "C"	LWBoneInfo		*g_boneinfo;
extern "C"	LWEnvelopeFuncs	*g_envf;


EChannelType GetChannelType(LWChannelID chan){
	const char* chname = g_chinfo->channelName(chan);
	if (strcmp(chname,"Position.X")==0)	return ctPositionX;
	if (strcmp(chname,"Position.Y")==0)	return ctPositionY;
	if (strcmp(chname,"Position.Z")==0)	return ctPositionZ;
	if (strcmp(chname,"Rotation.H")==0)	return ctRotationH;
	if (strcmp(chname,"Rotation.P")==0)	return ctRotationP;
	if (strcmp(chname,"Rotation.B")==0)	return ctRotationB;
	return ctUnsupported;
}

void CSMotion::ParseBoneMotion(LWItemID bone){
	LWChanGroupID	group, group_goal;
	LWChannelID		chan, chan_goal;
	LWItemID		goal;

	bone_mots.push_back(st_BoneMotion());
	st_BoneMotion&	bm=bone_mots.back();
	strcpy			(bm.name,g_iteminfo->name(bone));

	group			= g_iteminfo->chanGroup(bone);
	chan			= g_chinfo->nextChannel( group, NULL );
	
	goal			= g_iteminfo->goal(bone);
	bool bGoalOrient= !!(g_iteminfo->flags(bone)&LWITEMF_GOAL_ORIENT);
	if (bGoalOrient){
		group_goal	= g_iteminfo->chanGroup(goal);
		chan_goal	= g_chinfo->nextChannel( group_goal, NULL );
		bm.flag		|=WORLD_ORIENTATION;
	}

	while (chan){
		EChannelType t = GetChannelType(chan);
		if (t!=ctUnsupported){
			if (!bGoalOrient||(bGoalOrient&&((t==ctPositionX)||(t==ctPositionY)||(t==ctPositionZ)))){
				CEnvelope* env = CreateEnvelope(chan);
				bm.envs[t]=env;
			}
		}
		chan = g_chinfo->nextChannel(group, chan);
	}
	// goal orientation
	if (bGoalOrient){
		while (chan_goal){
			EChannelType t = GetChannelType(chan_goal);
			if (t!=ctUnsupported){
				if ((t==ctRotationH)||(t==ctRotationP)||(t==ctRotationB)){
					CEnvelope* env = CreateEnvelope(chan_goal);
					bm.envs[t]=env;
				}
			}
			chan_goal = g_chinfo->nextChannel(group_goal, chan_goal);
		}
	}
}

void COMotion::ParseObjectMotion(LWItemID object){
	LWChanGroupID	group;
	LWChannelID		chan;

	group			= g_iteminfo->chanGroup(object);
	chan			= g_chinfo->nextChannel(group, NULL);

	while (chan){
		EChannelType t = GetChannelType(chan);
		if (t!=ctUnsupported){
			CEnvelope* env = CreateEnvelope(chan);
			envs[t]=env;
		}
		chan = g_chinfo->nextChannel(group, chan);
	}
} 

//======================================================================
//Use the Animation Envelopes global to get the keys of an LWEnvelope
//and create our own version.
//======================================================================
CEnvelope* CCustomMotion::CreateEnvelope(LWChannelID chan){
	LWChanGroupID group;
	LWEnvelopeID lwenv;
	LWEnvKeyframeID lwkey;
	CEnvelope *env;
	st_Key *key, *tail = NULL;
	double val;
	
	env = new CEnvelope();
	
	group = g_chinfo->channelParent( chan );
	lwenv = g_chinfo->channelEnvelope( chan );
	lwkey = NULL;
	
	g_envf->egGet( lwenv, group, LWENVTAG_PREBEHAVE,  &env->behavior[0] );
	g_envf->egGet( lwenv, group, LWENVTAG_POSTBEHAVE, &env->behavior[1] );
	
	while ( lwkey = g_envf->nextKey( lwenv, lwkey )) {
		key = new st_Key();
		
		env->keys.push_back(key);
		
		g_envf->keyGet( lwenv, lwkey, LWKEY_SHAPE, &key->shape );
		g_envf->keyGet( lwenv, lwkey, LWKEY_VALUE, &val );		key->value		= ( float ) val;
		g_envf->keyGet( lwenv, lwkey, LWKEY_TIME, &val );		key->time		= ( float ) val;
		g_envf->keyGet( lwenv, lwkey, LWKEY_TENSION, &val );	key->tension	= ( float ) val;
		g_envf->keyGet( lwenv, lwkey, LWKEY_CONTINUITY, &val );	key->continuity = ( float ) val;
		g_envf->keyGet( lwenv, lwkey, LWKEY_BIAS, &val );		key->bias		= ( float ) val;
		g_envf->keyGet( lwenv, lwkey, LWKEY_PARAM_0, &val );	key->param[ 0 ] = ( float ) val;
		g_envf->keyGet( lwenv, lwkey, LWKEY_PARAM_1, &val );	key->param[ 1 ] = ( float ) val;
		g_envf->keyGet( lwenv, lwkey, LWKEY_PARAM_2, &val );	key->param[ 2 ] = ( float ) val;
		g_envf->keyGet( lwenv, lwkey, LWKEY_PARAM_3, &val );	key->param[ 3 ] = ( float ) val;
	}
	
	return env;
}
#endif

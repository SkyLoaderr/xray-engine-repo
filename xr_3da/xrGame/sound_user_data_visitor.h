////////////////////////////////////////////////////////////////////////////
//	Module 		: sound_user_data_visitor.h
//	Created 	: 27.01.2005
//  Modified 	: 27.01.2005
//	Author		: Dmitriy Iassenev
//	Description : Sound user data visitor
////////////////////////////////////////////////////////////////////////////

#pragma once

class CSoundUserDataVisitor {
public:
	virtual			~CSoundUserDataVisitor	();
	virtual void	visit					(CSoundUserData *data);
};
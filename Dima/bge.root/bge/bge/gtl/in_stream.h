////////////////////////////////////////////////////////////////////////////
//	Module 		: in_stream.h
//	Created 	: 22.10.2004
//  Modified 	: 22.10.2004
//	Author		: Dmitriy Iassenev
//	Description : Simple reader class
////////////////////////////////////////////////////////////////////////////

#pragma once

#include "ui.h"

class IReader {
private:
	FILE	*m_handle;

public:
	IC				IReader		(LPCSTR file_name)
	{
		m_handle	= fopen(file_name,"rb");
		VERIFY		(m_handle);
	}

	virtual			~IReader	()
	{
		fclose		(m_handle);
	}

	IC		void	r			(void *dest, const size_t &size)
	{
		fread		(dest,size,1,m_handle);
	}

	IC		u32		r_u32		()
	{
		u32			temp;
		r			(&temp,sizeof(temp));
		return		(temp);
	}
};

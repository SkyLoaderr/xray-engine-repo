#pragma once

class	NET_Event
{
public:
	u32					timestamp;
	u16					type;
	u16					destination;
	vector<u8>			data;
public:
	void				import		(NET_Packet& P)
	{
		data.clear		();
		u16				ID;	
		P.r_begin		(ID			);	VERIFY(M_EVENT==ID);
		P.r_u32			(timestamp	);
		P.r_u16			(type		);
		P.r_u16			(destination);

		DWORD size		= P.r_elapsed();
		data.resize		(size);
		P.r				(data.begin(),size);
	}
	void				export		(NET_Packet& P)
	{
		u16	ID			=	M_EVENT;
		P.w_begin		(ID			);
		P.w_u32			(timestamp	);
		P.w_u16			(type		);
		P.w_u16			(destination);
		P.w				(data.begin(),data.size());
	}
	void				implication	(NET_Packet& P)
	{
		PSGP.memCopy	(P.B.data,data.begin(),data.size());
		P.B.count		= data.size();
		P.r_pos			= 0;
	}

	IC bool operator < (NET_Event& A)	{ return timestamp<A.timestamp; }
};

class	NET_Queue_Event
{
	multiset<NET_Event>	queue;	
public:
	IC BOOL				insert		(NET_Packet& P)
	{
		NET_Event		E;
		E.import		(P);
	}
	IC BOOL				available	(DWORD T)
	{
		if (queue.empty() || (T<queue.front().timestamp))	return FALSE;
		else												return TRUE;
	}
	IC u16				get			(NET_Packet& P)
	{
		NET_Event& E	= *queue.begin();
		u16 type		= E.type;
		E.implication	(P);
		queue.erase		(queue.begin());
		return			type;
	}
};

class	NET_Queue_Update
{
};

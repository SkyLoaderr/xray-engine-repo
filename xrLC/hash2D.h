#pragma once

template <class T, DWORD s_X, DWORD s_Y>
class hash2D 
{
	vector<T>*	table[s_Y][s_X];
	Fbox2		bounds;
	Fvector2	size;
public:
	hash2D()
	{
		ZeroMemory	(this,sizeof(*this));
	}
	~hash2D()
	{
		clear		();
	}

	void		initialize	(Fbox2& R, DWORD faces)
	{
		bounds.set	(R);
		size.set	(R.max.x-R.min.x,R.max.y-R.min.y);
		clear		();

		DWORD		size	= s_Y*s_X;
		DWORD		apx		= faces/size;

		for (DWORD y=0; y<s_Y; y++)
			for (DWORD x=0; x<s_Y; x++)
			{
				table[y][x] = new vector<T> ();
				table[y][x]->reserve		(apx);
			}
	};
	void		add			(Fbox2& R, T& value)
	{
		int x1 = iFloor(float(s_X)*(R.min.x-bounds.min.x)/size.x); clamp(x1,0,int(s_X-1));
		int x2 = iCeil (float(s_X)*(R.max.x-bounds.min.x)/size.x); clamp(x2,0,int(s_X-1));
		int y1 = iFloor(float(s_Y)*(R.min.y-bounds.min.y)/size.y); clamp(y1,0,int(s_Y-1));
		int y2 = iCeil (float(s_Y)*(R.max.y-bounds.min.y)/size.y); clamp(y2,0,int(s_Y-1));
		
		for (int y=y1; y<=y2; y++)
			for (int x=x1; x<=x2; x++)
				table[y][x]->push_back(value);
	};
	vector<T>&	query		(float x, float y)
	{
		int _x	= iFloor(float(s_X)*(x-bounds.min.x)/size.x); clamp(_x,0,int(s_X-1));
		int _y	= iFloor(float(s_Y)*(y-bounds.min.y)/size.y); clamp(_y,0,int(s_Y-1));
		return *table[_y][_x];
	};
	void		clear		()
	{
		for (DWORD y=0; y<s_Y; y++)
			for (DWORD x=0; x<s_Y; x++)
				_DELETE(table[y][x]);
	}
};

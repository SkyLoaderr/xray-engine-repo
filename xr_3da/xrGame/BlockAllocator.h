#ifndef BLOCK_ALLOCATOR_H
#define BLOCK_ALLOCATOR_H
template<class T,u32 block_size>
class BlockAllocator
{
	u32 block_count;
	u32	block_position;
	T*	current_block;
	xr_vector<T*> blocks;
public:
	IC T* add()
	{
		if(block_position==block_size)next_block();
		++block_position;
		return &current_block[block_position-1];
	}
	IC void empty()
	{
		block_count=0;
		if(blocks.size())
		{
			block_position=0;
			current_block=blocks[0];
		}
		else
		{
			block_position=block_size;
		}
	}
	BlockAllocator()
	{
		block_position=block_size;
		block_count=0;
	}

	~BlockAllocator()
	{
		xr_vector<T*>::iterator i=blocks.begin(),e=blocks.end();
		for(;i!=e;++i) xr_free(*i);
		blocks.clear();
	}
private:
	IC	void add_block()
	{
		blocks.push_back((T*)xr_malloc(block_size*sizeof(T)));
	};
	IC void next_block()
	{

		if(block_count==blocks.size()) add_block();
		current_block=blocks[block_count];
		++block_count;
		block_position=0;
	}
};
#endif
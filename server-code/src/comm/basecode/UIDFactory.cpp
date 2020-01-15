#include "UIDFactory.h"

const int32_t MAX_SEQ = (2 << 12) - 1;

void CUIDFactory::Init(uint64_t world_id, uint64_t func_id)
{
	world_id_  = world_id;
	func_id_   = func_id;
	self_time_ = time(NULL);
	seq_	   = 0;
}

uint64_t CUIDFactory::CreateID()
{
	UID_TYPE uid;
	uid._uin64_data = 0;
	uid.world_id	= world_id_;
	uid.func_id		= func_id_;

	time_t cur_time = time(NULL);
	if(self_time_ < cur_time)
	{
		self_time_ = cur_time;
	}

	if(seq_ >= MAX_SEQ)
	{
		self_time_++;
		seq_ = 0;
	}
	else
	{
		seq_++;
	}

	uid.time_data = (self_time_ & 0xffffffff);
	uid.seq_data  = seq_;

	return uid._uin64_data;
}
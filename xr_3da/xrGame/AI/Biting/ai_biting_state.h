#pragma once 

class CAI_Biting;

class IState {
	
protected:	
	u32 priority;
	u32 stage;
	CAI_Biting *pData;

public:
	IState(u32 p, CAI_Biting *pc) : priority(p),stage(0),pData(pc) {}

	void Execute();

private:
	virtual void Init() = 0;
	virtual void Run() = 0;
	virtual void Done() = 0;

	virtual void CheckStartCondition() = 0;

	virtual bool IsWorkDone() = 0;
};

class stateRunAwayInPanic : public IState {

public:

	stateRunAwayInPanic(u32 P,CAI_Biting *pc) : IState(P,pc) {}

	virtual void Init();
	virtual void Run();
	virtual void Done();
	virtual void CheckStartCondition();

	bool IsWorkDone();
	
};

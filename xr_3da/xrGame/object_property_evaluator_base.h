#pragma once

class CObjectPropertyEvaluatorBase {
public:
					CObjectPropertyEvaluatorBase	() {};
	virtual 		~CObjectPropertyEvaluatorBase	() {};
			void	init							() {};
	virtual void	reinit							(CAI_Stalker *object) {};
	virtual void	reload							(LPCSTR section) {};
	virtual void	Load							(LPCSTR section) {};
	IC		bool	evaluate						() {return(false);};
};
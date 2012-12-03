#pragma once
#include "CommonVar.h"
#include "Originator.h"


class Memento
{
public:
	
	RecordObj* GetState()const;
	
	Memento(RecordObj *recordObj); 
	
	~Memento(void);
private:
	RecordObj * _recordObj;
};


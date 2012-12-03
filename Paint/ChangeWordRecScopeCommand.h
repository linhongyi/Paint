#pragma once
#include "command.h"
class ChangeWordRecScopeCommand :
	public Command
{
public:
	ChangeWordRecScopeCommand();
	~ChangeWordRecScopeCommand(void);
	void Execute(RecordObj *recordObj);
	void UnExecute(RecordObj *recordObj);
};


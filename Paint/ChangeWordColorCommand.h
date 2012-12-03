#pragma once
#include "command.h"
class ChangeWordColorCommand :
	public Command
{
public:
	ChangeWordColorCommand();
	~ChangeWordColorCommand(void);
	void Execute(RecordObj *recordObj);
	void UnExecute(RecordObj *recordObj);
};


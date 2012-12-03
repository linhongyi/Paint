#pragma once
#include "command.h"
class WarningCmd :
	public Command
{
public:
	WarningCmd(void);
	~WarningCmd(void);
	void Execute(RecordObj *recordObj);
	void UnExecute(RecordObj *recordObj);
};


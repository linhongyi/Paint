#pragma once
#include "command.h"
class ChangeWordBgColorCommand :
	public Command
{
public:
	ChangeWordBgColorCommand();
	~ChangeWordBgColorCommand(void);
	void Execute(RecordObj *recordObj);
	void UnExecute(RecordObj *recordObj);
};


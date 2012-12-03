#pragma once
#include "command.h"
class ModifyEditWordCmd :
	public Command
{
public:
	ModifyEditWordCmd(void);
	~ModifyEditWordCmd(void);
	void Execute(RecordObj *recordObj);
	void UnExecute(RecordObj *recordObj);
};


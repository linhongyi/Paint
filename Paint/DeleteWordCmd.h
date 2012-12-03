#pragma once
#include "command.h"
class DeleteWordCmd :
	public Command
{
public:
	DeleteWordCmd(void);
	~DeleteWordCmd(void);
	void Execute(RecordObj *recordObj);
	void UnExecute(RecordObj *recordObj);
};


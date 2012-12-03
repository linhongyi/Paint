#pragma once
#include "command.h"
class DeleteGraphCmd :
	public Command
{
public:
	DeleteGraphCmd(void);
	~DeleteGraphCmd(void);
	void Execute(RecordObj *recordObj);
	void UnExecute(RecordObj *recordObj);
};


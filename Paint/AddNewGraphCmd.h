#pragma once
#include "command.h"
class AddNewGraphCmd :
	public Command
{
public:
	AddNewGraphCmd(void);
	~AddNewGraphCmd(void);
	void Execute(RecordObj *recordObj);
	void UnExecute(RecordObj *recordObj);
};


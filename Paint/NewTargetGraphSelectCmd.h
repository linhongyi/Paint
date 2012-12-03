#pragma once
#include "command.h"
class NewTarGetGraphSelectCmd :
	public Command
{
public:
	NewTarGetGraphSelectCmd(void);
	~NewTarGetGraphSelectCmd(void);
	void Execute(RecordObj *recordObj);
	void UnExecute(RecordObj *recordObj);
};


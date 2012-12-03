#pragma once
#include "command.h"
class AddWordCmd :
	public Command
{
public:
	AddWordCmd(void);
	~AddWordCmd(void);
	void Execute(RecordObj *recordObj);
	void UnExecute(RecordObj *recordObj);
};


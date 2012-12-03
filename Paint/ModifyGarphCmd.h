#pragma once
#include "command.h"
class ModifyGarphCmd :
	public Command
{
public:
	ModifyGarphCmd(void);
	~ModifyGarphCmd(void);
	void Execute(RecordObj *recordObj);
	void UnExecute(RecordObj *recordObj);

};


#pragma once
#include "PaintInfo.h"
#include "SgWordDataModelControler.h"
#include "CommonVar.h"
#include "ToolKits.h"
#include "MyListController.h"

extern PaintInfo        gPaintInfo;

extern MyListController gMyList;
class Command
{
public:
	Command(void);
	~Command(void);

	virtual void Execute(RecordObj *recordObj) = 0;
	virtual void UnExecute(RecordObj *recordObj) = 0;
};


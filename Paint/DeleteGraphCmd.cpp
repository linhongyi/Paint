#include "StdAfx.h"
#include "DeleteGraphCmd.h"
#include "Invoker.h"
extern CInvoker gCInvoer;

DeleteGraphCmd::DeleteGraphCmd(void)
{
}


DeleteGraphCmd::~DeleteGraphCmd(void)
{
}

void DeleteGraphCmd::Execute(RecordObj* recordObj)
{
	gCInvoer.GetRedoUndoCmd( COMMAND_DRAW_NEW_GRAPH).UnExecute(recordObj);
}

void DeleteGraphCmd::UnExecute(RecordObj* recordObj)
{
	gCInvoer.GetRedoUndoCmd( COMMAND_DRAW_NEW_GRAPH).Execute(recordObj);
}

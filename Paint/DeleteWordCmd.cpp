#include "StdAfx.h"
#include "DeleteWordCmd.h"
#include <assert.h>
#include "Invoker.h"
extern CInvoker gCInvoer;

DeleteWordCmd::DeleteWordCmd(void)
{
}


DeleteWordCmd::~DeleteWordCmd(void)
{
}


void DeleteWordCmd::Execute(RecordObj *recordObj)
{
    gCInvoer.GetRedoUndoCmd( COMMAND_ADD_WORD ).UnExecute( recordObj );
}

void DeleteWordCmd::UnExecute(RecordObj *recordObj)
{
	gCInvoer.GetRedoUndoCmd( COMMAND_ADD_WORD ).Execute( recordObj );
}
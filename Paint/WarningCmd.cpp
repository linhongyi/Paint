#include "StdAfx.h"
#include "WarningCmd.h"


WarningCmd::WarningCmd(void)
{
}


WarningCmd::~WarningCmd(void)
{
}


void WarningCmd::Execute(RecordObj *recordObj)
{
	::MessageBox( gPaintInfo.GetMainHwnd(), _T(" �L���R�O����  Execute \n"),NULL,NULL);
}

void WarningCmd::UnExecute(RecordObj *recordObj)
{
	::MessageBox( gPaintInfo.GetMainHwnd(), _T(" �L���R�O����  UnExecute \n"),NULL,NULL);
}
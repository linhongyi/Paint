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
	::MessageBox( gPaintInfo.GetMainHwnd(), _T(" 無此命令物件  Execute \n"),NULL,NULL);
}

void WarningCmd::UnExecute(RecordObj *recordObj)
{
	::MessageBox( gPaintInfo.GetMainHwnd(), _T(" 無此命令物件  UnExecute \n"),NULL,NULL);
}
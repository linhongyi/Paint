#include "StdAfx.h"
#include "NewTarGetGraphSelectCmd.h"


NewTarGetGraphSelectCmd::NewTarGetGraphSelectCmd(void)
{
}


NewTarGetGraphSelectCmd::~NewTarGetGraphSelectCmd(void)
{
}

void NewTarGetGraphSelectCmd::Execute(RecordObj * recordObj)
{
	gPaintInfo.GetCSelectRecInfo().SetIsShow(TRUE);
	gPaintInfo.GetCSelectRecInfo().SetPosStart(recordObj->ptNewBeg);
	gPaintInfo.GetCSelectRecInfo().SetPosEnd(recordObj->ptNewEnd);
	gPaintInfo.GetCSelectRecInfo().SetType(recordObj->nObjType);
	recordObj = NULL;
}
	
void NewTarGetGraphSelectCmd::UnExecute(RecordObj * recordObj)
{
	gPaintInfo.GetCSelectRecInfo().SetIsShow(TRUE);
	gPaintInfo.GetCSelectRecInfo().SetPosStart(recordObj->ptBeg);
	gPaintInfo.GetCSelectRecInfo().SetPosEnd(recordObj->ptEnd);
	gPaintInfo.GetCSelectRecInfo().SetType(recordObj->nObjType);
	recordObj = NULL;
}
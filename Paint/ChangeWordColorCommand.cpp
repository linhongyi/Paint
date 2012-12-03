#include "StdAfx.h"
#include "ChangeWordColorCommand.h"


ChangeWordColorCommand::ChangeWordColorCommand(void)
{
}


ChangeWordColorCommand::~ChangeWordColorCommand(void)
{
}

void ChangeWordColorCommand::Execute(RecordObj *recordObj)
{
	gMyList.visitID_Node( recordObj->ID );
	gMyList.ModifyCurrNodeInfo( recordObj->nObjType, recordObj->ptNewBeg, recordObj->ptNewEnd, recordObj->iNewPenColor, recordObj->iNewFigBg , recordObj->iNewPenWidth );
	
	gPaintInfo.SetHitID(recordObj->ID);
	gPaintInfo.ColorMenuClickEvent(gPaintInfo.GetMainHwnd(), recordObj->iNewPenColor, 0);

	gPaintInfo.GetDrawTool()->RePaintWithClipRgn( recordObj->ptNewBeg, recordObj->ptNewEnd );
	gPaintInfo.GetDrawTool()->MapDCToMonitor();
	recordObj = NULL;
}

void ChangeWordColorCommand::UnExecute(RecordObj * recordObj)
{
	gMyList.visitID_Node( recordObj->ID );
	gMyList.ModifyCurrNodeInfo( recordObj->nObjType, recordObj->ptBeg, recordObj->ptEnd, recordObj->iPenColor, recordObj->iFigBg , recordObj->iPenWidth );
	
	gPaintInfo.SetHitID(recordObj->ID);
	gPaintInfo.ColorMenuClickEvent(gPaintInfo.GetMainHwnd(), recordObj->iPenColor, 0);
	
	gPaintInfo.GetDrawTool()->RePaintWithClipRgn( recordObj->ptBeg, recordObj->ptEnd );
	gPaintInfo.GetDrawTool()->MapDCToMonitor();
	recordObj = NULL;
}
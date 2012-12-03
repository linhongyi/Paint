#include "StdAfx.h"
#include "ChangeWordBgColorCommand.h"


ChangeWordBgColorCommand::ChangeWordBgColorCommand(void)
{
}


ChangeWordBgColorCommand::~ChangeWordBgColorCommand(void)
{
}

void ChangeWordBgColorCommand::Execute(RecordObj *recordObj)
{
	gMyList.visitID_Node( recordObj->ID );
	gMyList.ModifyCurrNodeInfo( recordObj->nObjType, recordObj->ptNewBeg, recordObj->ptNewEnd, recordObj->iNewPenColor, recordObj->iNewFigBg , recordObj->iNewPenWidth );
	gPaintInfo.SetHitID(recordObj->ID);

	gPaintInfo.FigBgMenuClickEvent(gPaintInfo.GetMainHwnd(), recordObj->iNewFigBg, 0);
	gPaintInfo.GetDrawTool()->FillWhileRec( recordObj->ptBeg, recordObj->ptEnd );
	gPaintInfo.GetDrawTool()->RePaintWithClipRgn( recordObj->ptBeg, recordObj->ptEnd );
	gPaintInfo.GetDrawTool()->MapDCToMonitor();
	recordObj = NULL;
}

void ChangeWordBgColorCommand::UnExecute(RecordObj * recordObj)
{

	
	gMyList.visitID_Node( recordObj->ID );
	gMyList.ModifyCurrNodeInfo( recordObj->nObjType, recordObj->ptBeg, recordObj->ptEnd, recordObj->iPenColor, recordObj->iFigBg , recordObj->iPenWidth );

	gPaintInfo.SetHitID(recordObj->ID);
	gPaintInfo.FigBgMenuClickEvent(gPaintInfo.GetMainHwnd(), recordObj->iFigBg, 0);
	gPaintInfo.GetDrawTool()->FillWhileRec( recordObj->ptNewBeg, recordObj->ptNewEnd );
	gPaintInfo.GetDrawTool()->RePaintWithClipRgn( recordObj->ptNewBeg, recordObj->ptNewEnd );
	gPaintInfo.GetDrawTool()->MapDCToMonitor();
	recordObj = NULL;
}
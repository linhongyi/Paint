#include "StdAfx.h"
#include "ChangeWordRecScopeCommand.h"



ChangeWordRecScopeCommand::ChangeWordRecScopeCommand(void)
{
}


ChangeWordRecScopeCommand::~ChangeWordRecScopeCommand(void)
{
}

void ChangeWordRecScopeCommand::Execute(RecordObj *recordObj)
{
	MyListNode *wordPtr = gMyList.visitID_Node( recordObj->ID );
		// set selectInfo
	gPaintInfo.GetCSelectRecInfo().SetPosStart(recordObj->ptNewBeg);
	gPaintInfo.GetCSelectRecInfo().SetPosEnd(recordObj->ptNewEnd);
	
	gPaintInfo.SetHitID( recordObj->ID);
	
	gMyList.ModifyCurrNodeInfo(recordObj->nObjType,recordObj->ptNewBeg, recordObj->ptNewEnd, recordObj->iPenColor, recordObj->iFigBg,1 );
	
	gPaintInfo.GetCTextOut()->WordReputationForInsertingWord(recordObj->ID);

	gPaintInfo.GetScrollBarHandle()->TuneWindowView();
	
	//若前後位置有交集
	if( gPaintInfo.GetCHitTest()->RegionHitRegion( recordObj->ptBeg, recordObj->ptEnd, recordObj->ptNewBeg, recordObj->ptNewEnd ) == TRUE )
	{
		POINT recPt1 = recordObj->ptBeg;
		POINT recPt2 = recordObj->ptEnd;
		POINT recPt3 = recordObj->ptNewBeg;
		POINT recPt4 = recordObj->ptNewEnd;

		ToolKits::FindMaxScopeWithFourPoint( recPt1, recPt2, recPt3, recPt4);

		gPaintInfo.GetDrawTool()->FillWhileRec( recPt1, recPt4);
		gPaintInfo.GetDrawTool()->RePaintWithClipRgn( recPt1, recPt4);
	}
	else
	{
		gPaintInfo.GetDrawTool()->FillWhileRec( recordObj->ptBeg, recordObj->ptEnd);
		gPaintInfo.GetDrawTool()->RePaintWithClipRgn( recordObj->ptBeg, recordObj->ptEnd );
		gPaintInfo.GetDrawTool()->FillWhileRec( recordObj->ptNewBeg, recordObj->ptNewEnd);
		gPaintInfo.GetDrawTool()->RePaintWithClipRgn( recordObj->ptNewBeg, recordObj->ptNewEnd );
	}


	 // set pos
	POINT caretPt;
	caretPt = wordPtr->FindLastWordPosInWordRec(caretPt);

    gPaintInfo.GetDrawTool()->ScreenToWindowCoordinate( caretPt.x, caretPt.y);
	gPaintInfo.GetCTextOut()->CaretSetPos( caretPt);

	gPaintInfo.GetDrawTool()->MapDCToMonitor();

	//delete recordObj;
	recordObj = NULL;
	wordPtr   = NULL;
}

void ChangeWordRecScopeCommand::UnExecute(RecordObj *recordObj)
{

	MyListNode *wordPtr = gMyList.visitID_Node( recordObj->ID );

	// set selectInfo
	gPaintInfo.GetCSelectRecInfo().SetPosStart(recordObj->ptBeg);
	gPaintInfo.GetCSelectRecInfo().SetPosEnd(recordObj->ptEnd);

	gPaintInfo.SetHitID( recordObj->ID);
	
	
	gMyList.ModifyCurrNodeInfo(recordObj->nObjType,recordObj->ptBeg, recordObj->ptEnd, recordObj->iPenColor, recordObj->iFigBg,1 );
	
	gPaintInfo.GetCTextOut()->WordReputationForInsertingWord(recordObj->ID);

	gPaintInfo.GetScrollBarHandle()->TuneWindowView();

	//若前後位置有交集
	if( gPaintInfo.GetCHitTest()->RegionHitRegion( recordObj->ptBeg, recordObj->ptEnd, recordObj->ptNewBeg, recordObj->ptNewEnd ) == TRUE )
	{
		POINT recPt1 = recordObj->ptBeg;
		POINT recPt2 = recordObj->ptEnd;
		POINT recPt3 = recordObj->ptNewBeg;
		POINT recPt4 = recordObj->ptNewEnd;

		ToolKits::FindMaxScopeWithFourPoint( recPt1, recPt2, recPt3, recPt4);

		gPaintInfo.GetDrawTool()->FillWhileRec( recPt1, recPt4);
		gPaintInfo.GetDrawTool()->RePaintWithClipRgn( recPt1, recPt4);
	}
	else
	{
		gPaintInfo.GetDrawTool()->FillWhileRec( recordObj->ptBeg, recordObj->ptEnd );
		gPaintInfo.GetDrawTool()->RePaintWithClipRgn( recordObj->ptBeg, recordObj->ptEnd );
		gPaintInfo.GetDrawTool()->FillWhileRec( recordObj->ptNewBeg, recordObj->ptNewEnd );
		gPaintInfo.GetDrawTool()->RePaintWithClipRgn( recordObj->ptNewBeg, recordObj->ptNewEnd );
	}

	
    // set pos
	POINT caretPt;
	caretPt = wordPtr->FindLastWordPosInWordRec(caretPt);
    gPaintInfo.GetDrawTool()->ScreenToWindowCoordinate( caretPt.x, caretPt.y);
	gPaintInfo.GetCTextOut()->CaretHide();
	gPaintInfo.GetCTextOut()->CaretSetPos( caretPt);
	gPaintInfo.GetCTextOut()->CaretShow();

	gPaintInfo.GetDrawTool()->MapDCToMonitor();

	//delete recordObj;
	recordObj = NULL;
	wordPtr   = NULL;
}
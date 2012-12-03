#include "StdAfx.h"
#include "AddWordCmd.h"
#include <assert.h>
#include "SgWordDataModelControler.h"

AddWordCmd::AddWordCmd(void)
{
}


AddWordCmd::~AddWordCmd(void)
{

}

void AddWordCmd::Execute(RecordObj *recordObj)
{

	assert( gMyList.IsID_NodeExit(recordObj->ID) == FALSE );
	
	gMyList.InsertGraphIDNode( 
				recordObj->ID
				,recordObj->nObjType
				,recordObj->ptNewBeg
				,recordObj->ptNewEnd
				,recordObj->iPenColor
				,recordObj->iFigBg
				,recordObj->iPenWidth);
	

	gPaintInfo.SetHitID( recordObj->ID );

	POINT pt1;
	pt1.x = recordObj->ptNewBeg.x + CommonVar::WORD_REC_MARGIN;
	pt1.y = recordObj->ptNewBeg.y + CommonVar::WORD_REC_MARGIN;

	POINT pt2;
	pt2.x = recordObj->ptNewEnd.x + CommonVar::WORD_REC_MARGIN;
	pt2.y = recordObj->ptNewEnd.y + CommonVar::WORD_REC_MARGIN;


	int nlen =  recordObj->vecCharacter.size();
	

	MyListNode *wordPtr = gMyList.visitID_Node(recordObj->ID);

	for( int i = 0 ; i < nlen ; i ++)
	{
		wordPtr->AppendWordNode(pt1, recordObj->vecCharacter[i] );
	}

	gPaintInfo.GetCTextOut()->WordReputationForInsertingWord(recordObj->ID);
	
	gPaintInfo.GetScrollBarHandle()->TuneWindowView();
	gPaintInfo.GetDrawTool()->FillWhileRec(pt1,pt2);
	gPaintInfo.GetDrawTool()->RePaintWithClipRgn( pt1, pt2);
	
	// set selectInfo
	gPaintInfo.GetCSelectRecInfo().SetPosStart(recordObj->ptNewBeg);
	gPaintInfo.GetCSelectRecInfo().SetPosEnd(recordObj->ptNewEnd);

	// set pos
	gPaintInfo.GetCTextOut()->CaretHide();
	

	gPaintInfo.GetDrawTool()->MapDCToMonitor();
	
	recordObj = NULL;
	
}

void AddWordCmd::UnExecute(RecordObj *recordObj)
{
	
	gPaintInfo.SetHitID( recordObj->ID );
	
	gPaintInfo.GetClipboardControl()->GetCWordClipboardControl()->CutWordRec();
	
	recordObj = NULL;

	gPaintInfo.GetDrawTool()->MapDCToMonitor();

	//刪除文字框後，移到新的文字，此時要紀錄這個文字框的資訊
	if( gPaintInfo.GetHitID() >= 0 )
		SgWordDataModelControler::Instance()->SaveAllWordIniInThisWordRec(  gPaintInfo.GetHitID() );
	
	
	gPaintInfo.GetCTextOut()->CaretHide();
}
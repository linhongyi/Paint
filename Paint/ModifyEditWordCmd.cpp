#include "StdAfx.h"
#include "ModifyEditWordCmd.h"
#include "Invoker.h"
#include "RedoUndoController.h"
extern CInvoker gCInvoer;

ModifyEditWordCmd::ModifyEditWordCmd(void)
{
}


ModifyEditWordCmd::~ModifyEditWordCmd(void)
{
}



void ModifyEditWordCmd::Execute(RecordObj *recordObj)
{
   if( gMyList.IsID_NodeExit( recordObj->ID ) == FALSE )
   {
        gCInvoer.GetRedoUndoCmd( COMMAND_ADD_WORD ).Execute( recordObj );
		////通知進行文字編輯前，記錄此時文字狀態
		gPaintInfo.SetAction(TRUE);
   }
   else
   {
   	    gPaintInfo.SetHitID( recordObj->ID );

	    POINT pt1;
	    pt1.x = recordObj->ptNewBeg.x + CommonVar::WORD_REC_MARGIN;
	    pt1.y = recordObj->ptNewBeg.y + CommonVar::WORD_REC_MARGIN;

	    POINT pt2;
	    pt2.x = recordObj->ptNewEnd.x + CommonVar::WORD_REC_MARGIN;
	    pt2.y = recordObj->ptNewEnd.y + CommonVar::WORD_REC_MARGIN;


	    int nlen =  recordObj->vecCharacter.size();
    	

	    MyListNode *wordPtr = gMyList.visitID_Node(recordObj->ID);

	    wordPtr->ClearAllNode();

	    for( int i = 0 ; i < nlen ; i ++)
	    {
		    wordPtr->AppendWordNode(pt1, recordObj->vecCharacter[i] );
	    }

	    gPaintInfo.GetCTextOut()->WordReputationForInsertingWord(recordObj->ID);
		gPaintInfo.GetCTextOut()->IsAddWordRecForWordContent(recordObj->ID);

	    gPaintInfo.GetScrollBarHandle()->TuneWindowView();
	    gPaintInfo.GetDrawTool()->FillWhileRec(pt1,pt2);
	    gPaintInfo.GetDrawTool()->RePaintWithClipRgn( pt1, pt2);
    	
	    // set selectInfo
	    gPaintInfo.GetCSelectRecInfo().SetPosStart(recordObj->ptNewBeg);
	    gPaintInfo.GetCSelectRecInfo().SetPosEnd(recordObj->ptNewEnd);

	    // set pos
	    gPaintInfo.GetCTextOut()->CaretHide();
	 

	    gPaintInfo.GetDrawTool()->MapDCToMonitor();
		
		////通知進行文字編輯前，記錄此時文字狀態
		gPaintInfo.SetAction(TRUE);
	    recordObj = NULL;
   }

	
}

void ModifyEditWordCmd::UnExecute(RecordObj *recordObj)
{
	int nlen =  recordObj->vecOriCharacater.size();

	if( nlen > 0 )
	{
		if( gMyList.IsID_NodeExit( recordObj->ID ) == FALSE )
		{
			gCInvoer.GetRedoUndoCmd( COMMAND_ADD_WORD ).Execute( recordObj );
			////通知進行文字編輯前，記錄此時文字狀態
			gPaintInfo.SetAction(TRUE);
		}
		else
		{
			gPaintInfo.SetHitID( recordObj->ID );
			MyListNode *wordPtr = gMyList.visitID_Node(recordObj->ID);

			POINT pt1;
			pt1.x = wordPtr->GetPtBeg().x + CommonVar::WORD_REC_MARGIN;
			pt1.y = wordPtr->GetPtBeg().y + CommonVar::WORD_REC_MARGIN;

			wordPtr->ClearAllNode();

			for( int i = 0 ; i < nlen ; i ++)
			{
				wordPtr->AppendWordNode(pt1, recordObj->vecOriCharacater[i] );
			}

			gPaintInfo.GetCTextOut()->WordReputationForInsertingWord(recordObj->ID);
			gPaintInfo.GetCTextOut()->IsAddWordRecForWordContent(recordObj->ID);

			gPaintInfo.GetScrollBarHandle()->TuneWindowView();

			POINT pt2;
			pt2.x = wordPtr->GetPtEnd().x + CommonVar::WORD_REC_MARGIN;
			pt2.y = wordPtr->GetPtEnd().y + CommonVar::WORD_REC_MARGIN;

			ToolKits::Normalization( pt1, pt2);
			gPaintInfo.GetDrawTool()->FillWhileRec(pt1,pt2);
			gPaintInfo.GetDrawTool()->RePaintWithClipRgn( pt1, pt2);

			// set selectInfo
			gPaintInfo.GetCSelectRecInfo().SetPosStart(recordObj->ptBeg);
			gPaintInfo.GetCSelectRecInfo().SetPosEnd(recordObj->ptEnd);

			//// set pos
			gPaintInfo.GetCTextOut()->CaretHide();

			gPaintInfo.GetDrawTool()->MapDCToMonitor();

			////通知進行文字編輯前，記錄此時文字狀態
			gPaintInfo.SetAction(TRUE);

			recordObj = NULL;

		}

	}
	else
	{
		// 因為有空文字框的出現，造成Delete前要判斷此物件是否存在於畫布：
		if( gMyList.IsID_NodeExit( recordObj->ID ) == TRUE )
		{
			gCInvoer.GetRedoUndoCmd( COMMAND_ADD_WORD ).UnExecute( recordObj );
		}

	}

}
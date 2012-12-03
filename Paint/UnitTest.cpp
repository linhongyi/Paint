#include "StdAfx.h"
#include "UnitTest.h"
#include "Resource.h"
#include "MyListController.h"
#include "PaintInfo.h"
extern PaintInfo        gPaintInfo;
extern MyListController gMyList;

UnitTest::UnitTest(void)
{
}


UnitTest::~UnitTest(void)
{
}

/*****************************
* 自動產生圖形
******************************/
void UnitTest::AutoGenerateObj()
{
	int nObjNum = 5000;
	int iType      = 0;
	int penColor   = 0;
	int bgColor    = 0;
	int penWidth   = 0;
	POINT leftUpPt, rightDownPt;

	int key;
	MyListNode *wordPtr;
	TCHAR cha;
	int nCharacter;

	for( int nCounter = 0 ; nCounter < nObjNum ; nCounter++ )
	{
		key = (rand() % 2) ;
	    key = 1;
		switch( key )
		{
		case 0:
			iType = (rand() % 3 ) + 220;
			penColor = (rand() % 9 ) + 401;
			bgColor  = (rand() % 9 ) + 420;
			penWidth = (rand() % 5 ) + 410;
			
			leftUpPt.x = (rand() % 4300 )+1;
			leftUpPt.y = (rand() % 4300 )+1;
		
			rightDownPt.x = leftUpPt.x + (rand() % 500 )+1;
			rightDownPt.y = leftUpPt.y + (rand() % 500 )+1;
			
			gMyList.AddGraphNode( iType, leftUpPt, rightDownPt, penColor, bgColor, penWidth);
			break;
		default:
			iType = IDM_WORD_REC;
			penColor = (rand() % 9 ) + 401;
			bgColor  = (rand() % 9 ) + 420;
			penWidth = 1;
			leftUpPt.x = (rand() % 4650 )+1;
			leftUpPt.y = (rand() % 4700 )+1;
			rightDownPt.x = leftUpPt.x + (rand() % 200 )+100;
			rightDownPt.y = leftUpPt.y + (rand() % 200 )+1;

			gMyList.AddGraphNode( iType, leftUpPt, rightDownPt, penColor, bgColor, penWidth);
			wordPtr = gMyList.visitID_Node( gMyList.GetCurrentNo() );
			
			leftUpPt.x += CommonVar::WORD_REC_MARGIN;
			leftUpPt.y += CommonVar::WORD_REC_MARGIN;

			nCharacter = (rand() % 300 ) + 1;
			for( int i = 0 ; i < nCharacter ; i ++ )
			{
				cha = (rand() % 26 )+65;

				wordPtr->AppendWordNode( leftUpPt, cha);
			}
			gPaintInfo.GetCTextOut()->WordReputationForInsertingWord( gMyList.GetCurrentNo() );
			gPaintInfo.GetCTextOut()->IsAddWordRecForWordContent( gMyList.GetCurrentNo() );
			break;
		}
	}

	//draw Obj
	leftUpPt.x = 0;
	leftUpPt.y = 0;
	rightDownPt.x = CommonVar::DRAW_MAP_WIDTH;
	rightDownPt.y = CommonVar::DRAW_MAP_HEIGHT;
	gPaintInfo.SetHitID( gMyList.GetCurrentNo() );
	//重建畫布
	gPaintInfo.GetDrawTool()->RePaintWithClipRgn(leftUpPt,rightDownPt);
	//把畫布貼至螢幕上
	gPaintInfo.GetDrawTool()->MapDCToMonitor();
}
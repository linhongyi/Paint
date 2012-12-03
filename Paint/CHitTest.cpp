#include "StdAfx.h"
#include "CHitTest.h"
#include "PaintInfo.h"
#include "PenSetting.h"
#include "ToolKits.h"
#include "DebugMsgOutputHandle.h"
#include "MyListController.h"

extern MyListController gMyList;



CHitTest::CHitTest(void)
{
	for( int i = 0 ; i < 8 ; i ++ )
	{
	  _pt[i].x = 0;
	  _pt[i].y = 0;
	}

}


CHitTest::~CHitTest(void)
{
}

void CHitTest::SetParent(PaintInfo *ptr)
{
	_parent = ptr;
}

/*****************************************
* 判斷滑鼠點擊點，是否碰到物件的頂點
******************************************/
BOOL CHitTest::PtHitClimax(POINT tarGetPt, int ID)
{
	BOOL bAnswer = FALSE;

	// 目前沒有已選取點
	if( ID < 0 )
		return bAnswer;

	//獲得目前視窗座標原點在畫布上的座標
	POINT scrollpos = _parent->GetScrollBarHandle()->GetNowScrollBarPos();
	//取得在佇列中資料
	MyListNode *ptr = gMyList.visitID_Node(ID);

	//畫布座標轉成視窗座標
	int posStartX = ptr->GetPtBeg().x - scrollpos.x;
	int posStartY = ptr->GetPtBeg().y - scrollpos.y;
	int posEndX   = ptr->GetPtEnd().x - scrollpos.x;
	int posEndY   = ptr->GetPtEnd().y - scrollpos.y;
	int iType = ptr->GetnObjType();

	ptr = NULL;

	//產生八個項點
	this->GenerateEightPt(iType,posStartX,posStartY,posEndX,posEndY);
	//是否命中八個頂點
	bAnswer = this->HitEightPoint(tarGetPt,ID);

	return bAnswer;
}

/*****************************
* 滑鼠的點擊點是否碰到物件內部
******************************/
BOOL CHitTest::PtHitTestRegion(int ID,POINT tarGetPt, POINT posStart , POINT posEnd)
{
	BOOL bAnswer = FALSE;

	// 目前沒有已選取點
	if( ID < 0 )
		return bAnswer;

	//獲得目前視窗座標原點在畫布上的座標
	POINT scrollpos = _parent->GetScrollBarHandle()->GetNowScrollBarPos();

	//取得在佇列中資料
	//MyListNode *ptr = gMyList.visitID_Node(ID);
	
	////畫布座標轉成視窗座標
	int posStartX = posStart.x - scrollpos.x;
	int posStartY = posStart.y - scrollpos.y;
	int posEndX   = posEnd.x   - scrollpos.x;
	int posEndY   = posEnd.y   - scrollpos.y;


	MyListNode *ptr = gMyList.visitID_Node(ID);
	COLORREF bgColor   = PenSetting::TransferIDToBg ( ptr->GetFigBg() );



	//是否背景透明
	BOOL isBg;
	if( bgColor == CommonVar::BG_TRANSPARENT )
		isBg = TRUE;
	else
		isBg = FALSE;
	int iType = ptr->GetnObjType();
	ptr = NULL;


	//產生八個項點
	this->GenerateEightPt(iType,posStartX,posStartY,posEndX,posEndY);
	//是否命中八個頂點
	bAnswer = this->HitEightPoint(tarGetPt,ID);


	RECT rect;
	//八個小矩形沒命中，判斷有無命中大矩形
	if( bAnswer == FALSE )
	{
		rect.bottom = _pt[CursorDirection_SE].y;
     	rect.right  = _pt[CursorDirection_SE].x;
        rect.left   = _pt[CursorDirection_NW].x;
	    rect.top    = _pt[CursorDirection_NW].y;
	    
	    switch(iType)
	    {
	    case IDM_LINE:
			//不是目標選取的圖形，所以只判斷圖形，不判斷方框。
			if( ID != _parent->GetHitID() )
			{
				   if( this->HitLine(tarGetPt,posStartX,posStartY,posEndX,posEndY) == TRUE)
			       {
			          _parent->SetDirectionMouse(CursorDirection_SIZEALL);
			          SetCursor( LoadCursor(NULL,IDC_SIZEALL)); 
			          bAnswer = TRUE;
			       }
			       else
			       {
				      _parent->SetDirectionMouse(CursorDirection_ARROW);
			           SetCursor( LoadCursor(NULL,IDC_ARROW)); 
			       }
			}
			else
			{
				//產生空心方框
			    this->GenerateRecRegion(TRUE);
				//目中命標區域
		       if( PtInRegion(_tarGetRecHrgn,tarGetPt.x,tarGetPt.y) == TRUE)
		       {
			        _parent->SetDirectionMouse(CursorDirection_SIZEALL);
			        SetCursor( LoadCursor(NULL,IDC_SIZEALL)); 
			        bAnswer = TRUE;
				    ::DeleteObject(_tarGetRecHrgn);
		       }
			   else
			   {
				    ::DeleteObject(_tarGetRecHrgn);
					//在實心方框內，且命中直線
			       if( this->HitLine(tarGetPt,posStartX,posStartY,posEndX,posEndY) == TRUE)
			        {
			           _parent->SetDirectionMouse(CursorDirection_SIZEALL);
			           SetCursor( LoadCursor(NULL,IDC_SIZEALL)); 
			           bAnswer = TRUE;
			        }
			        else
			        {
				       _parent->SetDirectionMouse(CursorDirection_ARROW);
			            SetCursor( LoadCursor(NULL,IDC_ARROW)); 
			        }
			    }
			}
		break;
	    case IDM_CIR:
		
			//產生圓形
			this->GenerateCirRegion(isBg);
			
			//不是目標選取的圖形，所以只判斷圖形，不判斷方框。
			if( ID != _parent->GetHitID() )
			{
			    //OutputDebugString(_T("PtHitTestRegion 不是目標選取的圖形，所以只判斷圓形，不判斷方框。\n"));
				//目中命標區域
		        if( PtInRegion(_tarGetCirHrgn,tarGetPt.x,tarGetPt.y) == TRUE)
		        {
			        _parent->SetDirectionMouse(CursorDirection_SIZEALL);
			        SetCursor( LoadCursor(NULL,IDC_SIZEALL)); 
			        bAnswer = TRUE;
		        }
				else
				{
			        _parent->SetDirectionMouse(CursorDirection_ARROW);
			         SetCursor( LoadCursor(NULL,IDC_ARROW)); 
				}
			}
			else
			{
				//OutputDebugString(_T("PtHitTestRegion 是目標選取的圖形，所以只判斷圓形和空心方形。\n"));
				// 產生空心方形
				this->GenerateRecRegion(TRUE);
				// 空心方框 + 圓的組合
		        ::CombineRgn(_tarGetCirHrgn,_tarGetCirHrgn,_tarGetRecHrgn,RGN_OR);

				//目中命標區域
		        if( PtInRegion(_tarGetCirHrgn,tarGetPt.x,tarGetPt.y) == TRUE)
		        {
			        _parent->SetDirectionMouse(CursorDirection_SIZEALL);
			        SetCursor( LoadCursor(NULL,IDC_SIZEALL)); 
			        bAnswer = TRUE;
		        }
				else
				{
			        _parent->SetDirectionMouse(CursorDirection_ARROW);
			         SetCursor( LoadCursor(NULL,IDC_ARROW)); 
				}

				::DeleteObject( _tarGetRecHrgn );
			}
			::DeleteObject( _tarGetCirHrgn );
		break;
	    case IDM_WORD_REC:
			//文字選取框為實心方框
	        this->GenerateRecRegion(FALSE);

			//目中命標區域
		    if( PtInRegion(_tarGetRecHrgn,tarGetPt.x,tarGetPt.y) == TRUE)
		    {
				//刪除實心方框
				::DeleteObject( this->_tarGetRecHrgn );
				//產生透明方框
				this->GenerateRecRegion(TRUE);

				if( PtInRegion(_tarGetRecHrgn,tarGetPt.x,tarGetPt.y) == TRUE)
				{
					_parent->SetDirectionMouse(CursorDirection_SIZEALL);
			         SetCursor( LoadCursor(NULL,IDC_SIZEALL)); 
			         bAnswer = TRUE;
				    
				}
				else
				{
					  _parent->SetDirectionMouse(CursorDirection_IBEAM );
			         SetCursor( LoadCursor(NULL,IDC_IBEAM)); 
			         bAnswer = TRUE;
				}
		    }
		    else
			{
			    _parent->SetDirectionMouse(CursorDirection_ARROW);
			     SetCursor( LoadCursor(NULL,IDC_ARROW)); 
			}
			::DeleteObject( _tarGetRecHrgn );
		break;
		case IDM_REC:
			this->GenerateRecRegion(isBg);

			//目中命標區域
		    if( PtInRegion(_tarGetRecHrgn,tarGetPt.x,tarGetPt.y) == TRUE)
		    {
			    _parent->SetDirectionMouse(CursorDirection_SIZEALL);
			    SetCursor( LoadCursor(NULL,IDC_SIZEALL)); 
			    bAnswer = TRUE;
		    }
		    else
			{
			    _parent->SetDirectionMouse(CursorDirection_ARROW);
			     SetCursor( LoadCursor(NULL,IDC_ARROW)); 
			}
			::DeleteObject( _tarGetRecHrgn );
		break;
	    }
	}

	return bAnswer;
}



/** 兩個圖形是否相交*/
BOOL CHitTest::RegionHitRegion(POINT ptSrc1, POINT ptSrc2, POINT ptDes1, POINT ptDes2)
{
	
	ToolKits::Normalization(ptSrc1, ptSrc2);
	ToolKits::Normalization(ptDes1, ptDes2);
	HRGN hrgn = ::CreateRectRgn(
				ptDes1.x
				,ptDes1.y
				,ptDes2.x
				,ptDes2.y);
	RECT rect;
	rect.left   = ptSrc1.x;
	rect.right  = ptSrc2.x;
	rect.top    = ptSrc1.y;
	rect.bottom = ptSrc2.y;

	BOOL answer = ::RectInRegion(hrgn,&rect);
	
	::DeleteObject( hrgn );
	return answer;
}

/******************
* private function
******************/
//命中八個項點
BOOL CHitTest::HitEightPoint(POINT tarGetPt,int listIndex)
{
	//獲得目前視窗座標原點在畫布上的座標
	POINT scrollpos = _parent->GetScrollBarHandle()->GetNowScrollBarPos();
	//取得在佇列中資料
	MyListNode *ptr = gMyList.visitID_Node(listIndex);
	//畫布座標轉成視窗座標
	int posStartX = ptr->GetPtBeg().x - scrollpos.x;
	int posStartY = ptr->GetPtBeg().y - scrollpos.y;
	int posEndX   = ptr->GetPtEnd().x - scrollpos.x;
	int posEndY   = ptr->GetPtEnd().y - scrollpos.y;

	/***************
	* 偵測是否在矩型內
	****************/
	int iType = ptr->GetnObjType();
	ptr = NULL;

	RECT rect;

	int startIndex;
	//若偵測的點，不是目前的選取點，不計對角點。
	if ( listIndex == _parent->GetHitID() ) 
		startIndex = 0;
    else
	    startIndex = 4;

	

	//判斷是否命中8個小矩形
	for( int smallRectNum = startIndex ; smallRectNum < 8 ; smallRectNum++ )
	{
		rect.bottom  = _pt[smallRectNum].y + CommonVar::SMALL_RECT_BUFFER;
	    rect.right   = _pt[smallRectNum].x + CommonVar::SMALL_RECT_BUFFER;
	    rect.left    = _pt[smallRectNum].x - CommonVar::SMALL_RECT_BUFFER;
	    rect.top     = _pt[smallRectNum].y - CommonVar::SMALL_RECT_BUFFER;

		//ptInRgn
	    if( PtInRect( &rect, tarGetPt ) == TRUE )
	    {
			// 紀錄目標滑鼠方向，並設定滑鼠游標圖示
			switch(iType)
			{
			case IDM_LINE:
			   // OutputDebugString(_T("直線命中\n"));
			    
			    //斜方向
				if( smallRectNum >= 0 && smallRectNum <=3 )
				{  
				    //判斷是否命中直線點，否則只能拖移
				    if( _pt[smallRectNum].x == posStartX && _pt[smallRectNum].y == posStartY)
				    {
				    }
				    else if ( _pt[smallRectNum].x == posEndX && _pt[smallRectNum].y == posEndY)
				    {
				    }
				    else
				    {
				       smallRectNum = CursorDirection_SIZEALL;
				    }
				}
				_parent->SetDirectionMouse(smallRectNum);
				_parent->GetDrawTool()->GetDrawLine()->DecideCursorShape(smallRectNum);
				break;
			case IDM_CIR:
				_parent->SetDirectionMouse(smallRectNum);
				_parent->GetDrawTool()->GetDrawCircular()->DecideCursorShape(smallRectNum);
				break;
			case IDM_REC:
				_parent->SetDirectionMouse(smallRectNum);
				_parent->GetDrawTool()->GetDrawRec()->DecideCursorShape(smallRectNum);
				break;
			case IDM_WORD_REC:
				_parent->SetDirectionMouse(smallRectNum);
				_parent->GetDrawTool()->GetDrawRec()->DecideCursorShape(smallRectNum);
				break;
			default:
				return FALSE;
				break;
			}
		   return TRUE;
		   break;
	    }
	}
	return FALSE;
}

/*********************************
* 產生偵測的八個點
**********************************/
void CHitTest::GenerateEightPt(int iType,int posStartX,int posStartY, int posEndX, int posEndY )
{
		//產生偵測個八個點
	switch(iType)
	{
	case IDM_LINE:
		_parent->GetDrawTool()->GetDrawLine()->DetectPointOutput( _pt, posStartX, posStartY, posEndX, posEndY);
		break;
	case IDM_CIR:
		_parent->GetDrawTool()->GetDrawCircular()->DetectPointOutput( _pt, posStartX, posStartY, posEndX, posEndY);
		break;
	case IDM_REC:
		_parent->GetDrawTool()->GetDrawRec()->DetectPointOutput( _pt, posStartX, posStartY, posEndX, posEndY);
		break;
    case IDM_WORD_REC:
		_parent->GetDrawTool()->GetDrawRec()->DetectPointOutput( _pt, posStartX, posStartY, posEndX, posEndY);
		break;
	default:
		break;
	}
}

/***************************
* 建立圓形的clipRgn
****************************/
void CHitTest::GenerateCirRegion(BOOL isTransparent)
{
	// 空心圓
	if( isTransparent == TRUE )
	{
		//建立空心圓
		HRGN hrgn1 = ::CreateEllipticRgn(
				_pt[CursorDirection_NW].x
				,_pt[CursorDirection_NW].y
				,_pt[CursorDirection_SE].x
				,_pt[CursorDirection_SE].y);

		_tarGetCirHrgn = ::CreateEllipticRgn(
			_pt[CursorDirection_NW].x+CommonVar::SMALL_RECT_BUFFER
			,_pt[CursorDirection_NW].y+CommonVar::SMALL_RECT_BUFFER
			,_pt[CursorDirection_SE].x-CommonVar::SMALL_RECT_BUFFER
			,_pt[CursorDirection_SE].y-CommonVar::SMALL_RECT_BUFFER);


		// _tarGetCirHrgn 為空心圓
		::CombineRgn(_tarGetCirHrgn,_tarGetCirHrgn,hrgn1,RGN_XOR);
		::DeleteObject(hrgn1);
	}
	else
	{
		_tarGetCirHrgn = ::CreateEllipticRgn(
				_pt[CursorDirection_NW].x
				,_pt[CursorDirection_NW].y
				,_pt[CursorDirection_SE].x
				,_pt[CursorDirection_SE].y);
	}
	
}

/**************************
* 建立方形的clipRgn
***************************/
void CHitTest::GenerateRecRegion(BOOL isTransparent)
{
	//空心方形
	if( isTransparent == TRUE )
	{
		//OutputDebugString(_T("空心方\n"));
		HRGN hrgn1 = ::CreateRectRgn(
				_pt[CursorDirection_NW].x
				,_pt[CursorDirection_NW].y
				,_pt[CursorDirection_SE].x
				,_pt[CursorDirection_SE].y);

		_tarGetRecHrgn = ::CreateRectRgn(
			_pt[CursorDirection_NW].x+CommonVar::SMALL_RECT_BUFFER
			,_pt[CursorDirection_NW].y+CommonVar::SMALL_RECT_BUFFER
			,_pt[CursorDirection_SE].x-CommonVar::SMALL_RECT_BUFFER
			,_pt[CursorDirection_SE].y-CommonVar::SMALL_RECT_BUFFER);

		::CombineRgn(_tarGetRecHrgn,_tarGetRecHrgn,hrgn1,RGN_XOR);

		::DeleteObject(hrgn1);

	}
	else
	{
		//OutputDebugString(_T("實心方\n"));
		_tarGetRecHrgn = ::CreateRectRgn(
				_pt[CursorDirection_NW].x
				,_pt[CursorDirection_NW].y
				,_pt[CursorDirection_SE].x
				,_pt[CursorDirection_SE].y);
	}	
}

/**************************************
* 是否hit直線
***************************************/
BOOL CHitTest::HitLine(POINT tarGetPt,int posStartX,int posStartY, int posEndX, int posEndY)
{
	::DeleteObject( _tarGetRecHrgn );

	// 產生實心方框
	this->GenerateRecRegion( false );

	//不在實心方框內，所以不可能碰到直線
	if(::PtInRegion(_tarGetRecHrgn,tarGetPt.x,tarGetPt.y) == FALSE )
	{
	    ::DeleteObject( _tarGetRecHrgn );
		return FALSE;
	}
	::DeleteObject( _tarGetRecHrgn );

	// y = ax + b;
	double m;  //斜率
	double diff_x = posStartX - posEndX;
	double diff_y = posStartY - posEndY;

	if( diff_x == 0) // y = constant
	{
		if ( tarGetPt.y == posEndY && tarGetPt.y == posStartY )
			return TRUE;
		else
			return FALSE;
	}
	else if ( diff_y == 0 )
	{
		if ( tarGetPt.x == posEndX && tarGetPt.x == posStartX )
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		m = diff_y / diff_x;
		double c = ( (posEndY+ posStartY) - m*( posEndX + posStartX ) )/2;
		double y = m * tarGetPt.x + c;

		//double new_m = (tarGetPt.y - posEndY) / (tarGetPt.x - posEndX);
 		if(  abs(tarGetPt.y - y) <= 5 )
			return TRUE;
		else
			return FALSE;
	}
}
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
* �P�_�ƹ��I���I�A�O�_�I�쪫�󪺳��I
******************************************/
BOOL CHitTest::PtHitClimax(POINT tarGetPt, int ID)
{
	BOOL bAnswer = FALSE;

	// �ثe�S���w����I
	if( ID < 0 )
		return bAnswer;

	//��o�ثe�����y�Э��I�b�e���W���y��
	POINT scrollpos = _parent->GetScrollBarHandle()->GetNowScrollBarPos();
	//���o�b��C�����
	MyListNode *ptr = gMyList.visitID_Node(ID);

	//�e���y���ন�����y��
	int posStartX = ptr->GetPtBeg().x - scrollpos.x;
	int posStartY = ptr->GetPtBeg().y - scrollpos.y;
	int posEndX   = ptr->GetPtEnd().x - scrollpos.x;
	int posEndY   = ptr->GetPtEnd().y - scrollpos.y;
	int iType = ptr->GetnObjType();

	ptr = NULL;

	//���ͤK�Ӷ��I
	this->GenerateEightPt(iType,posStartX,posStartY,posEndX,posEndY);
	//�O�_�R���K�ӳ��I
	bAnswer = this->HitEightPoint(tarGetPt,ID);

	return bAnswer;
}

/*****************************
* �ƹ����I���I�O�_�I�쪫�󤺳�
******************************/
BOOL CHitTest::PtHitTestRegion(int ID,POINT tarGetPt, POINT posStart , POINT posEnd)
{
	BOOL bAnswer = FALSE;

	// �ثe�S���w����I
	if( ID < 0 )
		return bAnswer;

	//��o�ثe�����y�Э��I�b�e���W���y��
	POINT scrollpos = _parent->GetScrollBarHandle()->GetNowScrollBarPos();

	//���o�b��C�����
	//MyListNode *ptr = gMyList.visitID_Node(ID);
	
	////�e���y���ন�����y��
	int posStartX = posStart.x - scrollpos.x;
	int posStartY = posStart.y - scrollpos.y;
	int posEndX   = posEnd.x   - scrollpos.x;
	int posEndY   = posEnd.y   - scrollpos.y;


	MyListNode *ptr = gMyList.visitID_Node(ID);
	COLORREF bgColor   = PenSetting::TransferIDToBg ( ptr->GetFigBg() );



	//�O�_�I���z��
	BOOL isBg;
	if( bgColor == CommonVar::BG_TRANSPARENT )
		isBg = TRUE;
	else
		isBg = FALSE;
	int iType = ptr->GetnObjType();
	ptr = NULL;


	//���ͤK�Ӷ��I
	this->GenerateEightPt(iType,posStartX,posStartY,posEndX,posEndY);
	//�O�_�R���K�ӳ��I
	bAnswer = this->HitEightPoint(tarGetPt,ID);


	RECT rect;
	//�K�Ӥp�x�ΨS�R���A�P�_���L�R���j�x��
	if( bAnswer == FALSE )
	{
		rect.bottom = _pt[CursorDirection_SE].y;
     	rect.right  = _pt[CursorDirection_SE].x;
        rect.left   = _pt[CursorDirection_NW].x;
	    rect.top    = _pt[CursorDirection_NW].y;
	    
	    switch(iType)
	    {
	    case IDM_LINE:
			//���O�ؼп�����ϧΡA�ҥH�u�P�_�ϧΡA���P�_��ءC
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
				//���ͪŤߤ��
			    this->GenerateRecRegion(TRUE);
				//�ؤ��R�аϰ�
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
					//�b��ߤ�ؤ��A�B�R�����u
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
		
			//���Ͷ��
			this->GenerateCirRegion(isBg);
			
			//���O�ؼп�����ϧΡA�ҥH�u�P�_�ϧΡA���P�_��ءC
			if( ID != _parent->GetHitID() )
			{
			    //OutputDebugString(_T("PtHitTestRegion ���O�ؼп�����ϧΡA�ҥH�u�P�_��ΡA���P�_��ءC\n"));
				//�ؤ��R�аϰ�
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
				//OutputDebugString(_T("PtHitTestRegion �O�ؼп�����ϧΡA�ҥH�u�P�_��ΩM�Ťߤ�ΡC\n"));
				// ���ͪŤߤ��
				this->GenerateRecRegion(TRUE);
				// �Ťߤ�� + �ꪺ�զX
		        ::CombineRgn(_tarGetCirHrgn,_tarGetCirHrgn,_tarGetRecHrgn,RGN_OR);

				//�ؤ��R�аϰ�
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
			//��r����ج���ߤ��
	        this->GenerateRecRegion(FALSE);

			//�ؤ��R�аϰ�
		    if( PtInRegion(_tarGetRecHrgn,tarGetPt.x,tarGetPt.y) == TRUE)
		    {
				//�R����ߤ��
				::DeleteObject( this->_tarGetRecHrgn );
				//���ͳz�����
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

			//�ؤ��R�аϰ�
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



/** ��ӹϧάO�_�ۥ�*/
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
//�R���K�Ӷ��I
BOOL CHitTest::HitEightPoint(POINT tarGetPt,int listIndex)
{
	//��o�ثe�����y�Э��I�b�e���W���y��
	POINT scrollpos = _parent->GetScrollBarHandle()->GetNowScrollBarPos();
	//���o�b��C�����
	MyListNode *ptr = gMyList.visitID_Node(listIndex);
	//�e���y���ন�����y��
	int posStartX = ptr->GetPtBeg().x - scrollpos.x;
	int posStartY = ptr->GetPtBeg().y - scrollpos.y;
	int posEndX   = ptr->GetPtEnd().x - scrollpos.x;
	int posEndY   = ptr->GetPtEnd().y - scrollpos.y;

	/***************
	* �����O�_�b�x����
	****************/
	int iType = ptr->GetnObjType();
	ptr = NULL;

	RECT rect;

	int startIndex;
	//�Y�������I�A���O�ثe������I�A���p�﨤�I�C
	if ( listIndex == _parent->GetHitID() ) 
		startIndex = 0;
    else
	    startIndex = 4;

	

	//�P�_�O�_�R��8�Ӥp�x��
	for( int smallRectNum = startIndex ; smallRectNum < 8 ; smallRectNum++ )
	{
		rect.bottom  = _pt[smallRectNum].y + CommonVar::SMALL_RECT_BUFFER;
	    rect.right   = _pt[smallRectNum].x + CommonVar::SMALL_RECT_BUFFER;
	    rect.left    = _pt[smallRectNum].x - CommonVar::SMALL_RECT_BUFFER;
	    rect.top     = _pt[smallRectNum].y - CommonVar::SMALL_RECT_BUFFER;

		//ptInRgn
	    if( PtInRect( &rect, tarGetPt ) == TRUE )
	    {
			// �����ؼзƹ���V�A�ó]�w�ƹ���йϥ�
			switch(iType)
			{
			case IDM_LINE:
			   // OutputDebugString(_T("���u�R��\n"));
			    
			    //�פ�V
				if( smallRectNum >= 0 && smallRectNum <=3 )
				{  
				    //�P�_�O�_�R�����u�I�A�_�h�u��첾
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
* ���Ͱ������K���I
**********************************/
void CHitTest::GenerateEightPt(int iType,int posStartX,int posStartY, int posEndX, int posEndY )
{
		//���Ͱ����ӤK���I
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
* �إ߶�Ϊ�clipRgn
****************************/
void CHitTest::GenerateCirRegion(BOOL isTransparent)
{
	// �Ť߶�
	if( isTransparent == TRUE )
	{
		//�إߪŤ߶�
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


		// _tarGetCirHrgn ���Ť߶�
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
* �إߤ�Ϊ�clipRgn
***************************/
void CHitTest::GenerateRecRegion(BOOL isTransparent)
{
	//�Ťߤ��
	if( isTransparent == TRUE )
	{
		//OutputDebugString(_T("�Ťߤ�\n"));
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
		//OutputDebugString(_T("��ߤ�\n"));
		_tarGetRecHrgn = ::CreateRectRgn(
				_pt[CursorDirection_NW].x
				,_pt[CursorDirection_NW].y
				,_pt[CursorDirection_SE].x
				,_pt[CursorDirection_SE].y);
	}	
}

/**************************************
* �O�_hit���u
***************************************/
BOOL CHitTest::HitLine(POINT tarGetPt,int posStartX,int posStartY, int posEndX, int posEndY)
{
	::DeleteObject( _tarGetRecHrgn );

	// ���͹�ߤ��
	this->GenerateRecRegion( false );

	//���b��ߤ�ؤ��A�ҥH���i��I�쪽�u
	if(::PtInRegion(_tarGetRecHrgn,tarGetPt.x,tarGetPt.y) == FALSE )
	{
	    ::DeleteObject( _tarGetRecHrgn );
		return FALSE;
	}
	::DeleteObject( _tarGetRecHrgn );

	// y = ax + b;
	double m;  //�ײv
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
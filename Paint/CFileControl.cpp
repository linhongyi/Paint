#include "StdAfx.h"
#include "CFileControl.h"
#include "Resource.h"

#include "CommonVar.h"
#include "PaintInfo.h"
#include "MyListController.h"


extern RedoUndoController *gRedoUndoController;
extern MyListController gMyList;

CFileControl::CFileControl(void)
{
	::SecureZeroMemory(&szAppName, sizeof(szAppName));
	::SecureZeroMemory(&szTitleName, sizeof(szTitleName));
	::SecureZeroMemory(&szFileName, sizeof(szFileName));

	//::wsprintf(szAppName, _T("%s"),_T("Painter"));


	_bNeedSave = FALSE;

}

CFileControl::~CFileControl(void)
{
}

//���}
void CFileControl::CloseProgram(HWND hWnd)
{
	if( _bNeedSave )
	{
		AskAboutSave(hWnd,szTitleName);
	}

	if ( IDNO == AskAboutExit(hWnd))
		return;
	else
		::DestroyWindow(hWnd);
}
//�}�s�ɮ�
void CFileControl::FileCreate(HWND hWnd)
{
	if( _bNeedSave && IDCANCEL == AskAboutSave(hWnd,szTitleName))
	{
		return;
	}

	//���]�ɮ׸�T
	this->ResetFileNameInfo(hWnd);
	_bNeedSave = FALSE;
	
	_parent->SetHitID(-1);
	_parent->GetCSelectRecInfo().SetIsShow(false);
	//�e�X�M���e�����T��
	_parent->ClickClearMenuItem();
	_parent->GetDrawTool()->ClearMonitor();
	gRedoUndoController->Reset();
}

//�s��
void CFileControl::SaveFile(HWND hWnd)
{
	if( szFileName[0] )
	{
		if( this->PopFileWrite(hWnd,szFileName))
		{
			_bNeedSave = FALSE;
			this->DoCaption(_parent->GetMainHwnd(),szTitleName,false);
			
			TCHAR szBuffer[64 + MAX_PATH];
			::wsprintf(szBuffer, _T("�ɮ׼g�J���\ %s"), szTitleName[0] ? szTitleName: UNTITLED );
			MessageBox(hWnd, szBuffer, szAppName, MB_OK );
		}
		else
		{
			this->OkMessage(hWnd, _T("Could not write file %s"), szTitleName);
		}
	}
	//�|���إ߹L�ɮ�
	else
		this->SaveFileAs(hWnd);
}

//�t�s�s��
void CFileControl::SaveFileAs(HWND hWnd)
{
	if( this->PopFileSaveDlg(hWnd, szFileName, szTitleName) )
	{
		this->DoCaption( hWnd, szTitleName,false );

		if( this->PopFileWrite(hWnd,szFileName))
		{
			_bNeedSave = FALSE;
		}
		else
		{
			this->OkMessage(hWnd, _T("Could not write file %s"), szTitleName);
		}
	}

}

//�}������
void CFileControl::OpenFile(HWND hWnd)
{
	
	if( _bNeedSave)
	{
		short AskAboutSaveResult = AskAboutSave( hWnd, szTitleName);

		if( AskAboutSaveResult == IDCANCEL )
			return;
	}

	if( this->PopFileOpenDlg(hWnd, szFileName, szTitleName))
	{
		if( !PopFileRead( hWnd , szFileName)) 
		{
			this->OkMessage(hWnd, TEXT("����Ū���D�k�ɮ� %s!"), szTitleName);
			szFileName[0]  = '\0';
	        szTitleName[0] = '\0';
		}
		else
		{
			//�M���Ҧ����{����
			gRedoUndoController->Reset();
			this->DoCaption( hWnd, szTitleName,false );
			_bNeedSave = FALSE;
		}
	}
}

void CFileControl::SetParent(PaintInfo *ptr)
{
	_parent = ptr;
}

//�ɦW��ܬP��
void CFileControl::newChangeWithFile(BOOL foo)
{
	this->_bNeedSave = foo;
	this->DoCaption(_parent->GetMainHwnd(),szTitleName,true);
}

//���U�M���ﶵ
void CFileControl::ClickClearMenu(HWND hWnd)
{
	if( _bNeedSave)
	{
		 AskAboutSave(hWnd,szTitleName);
	}

	//���]�ɮצW��
	this->ResetFileNameInfo(hWnd);
	_parent->SetHitID(-1);
	_parent->GetCSelectRecInfo().SetIsShow(false);
	//�e�X�M���e�����T��
	_parent->ClickClearMenuItem();
	_parent->GetDrawTool()->ClearMonitor();
	_parent->SetDirectionMouse( CursorDirection_ARROW );
	gRedoUndoController->Reset();
}

//���]���W�����D
void CFileControl::ResetFileNameInfo(HWND hWnd)
{
	::SetWindowText(hWnd, TEXT("\0"));
	szFileName[0]  = '\0';
	szTitleName[0] = '\0';
	this->DoCaption(hWnd,szTitleName,false);
}
/*************************
* Private Function
**************************/

short CFileControl::AskAboutSave(HWND hWnd, TCHAR* szTitleName)
{
	TCHAR szBuffer[ 64 + MAX_PATH];
	int iReturn;

	::wsprintf(szBuffer, _T("%s"),_T("�O�_�x�s�ثe���e��"));

	iReturn = ::MessageBox(hWnd, szBuffer, szAppName, 
		MB_YESNOCANCEL| MB_ICONQUESTION);

	if( iReturn == IDYES )
		SendMessage(hWnd, WM_COMMAND, IDM_SAVE,0);
		/*if( !SendMessage(hWnd, WM_COMMAND, IDM_SAVE,0))
			iReturn = IDCANCEL;*/
	return iReturn;
}

short CFileControl::AskAboutExit(HWND hWnd)
{
	int iReturn;
	iReturn = ::MessageBox(hWnd, _T("�O�_���}�{��"), szAppName, 
		MB_YESNO| MB_ICONQUESTION);
	return iReturn;
}

//�]�w���W���D�W��
void CFileControl::DoCaption(HWND hWnd, TCHAR* szTitleName, bool bShowStar)
{
	TCHAR szCaption[64 + MAX_PATH];

	::wsprintf(szCaption, _T("%s%s - %s")
		, szTitleName[0] ? szTitleName : UNTITLED
		, bShowStar ? "*" : ""
		,szAppName);

	::SetWindowText(hWnd, szCaption);
}


void CFileControl::OkMessage(HWND hWnd, TCHAR* szMessage, TCHAR* sTitleName )
{
	TCHAR szBuffer[64 + MAX_PATH];
	::wsprintf(szBuffer, szMessage, szTitleName[0] ? szTitleName: UNTITLED );
	MessageBox(hWnd, szBuffer, szAppName, MB_OK | MB_ICONEXCLAMATION );

}

void CFileControl::PopFileInitialize(HWND hWnd)
{
	static TCHAR szFilter[] = TEXT("ker Files (*.ker)\0*.ker\0")\
		/*TEXT("ASCII Files(*.ASC)\0*.asc\0")\*/
		TEXT("All Files (*.*)\0*.*\0\0");

	ofn.lStructSize       = sizeof(OPENFILENAME);
	ofn.hwndOwner         = hWnd;
	ofn.hInstance         = NULL;
	ofn.lpstrFilter       = szFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter    = 0;
	ofn.nFilterIndex      = 0;
	ofn.lpstrFile         = NULL;
	ofn.nMaxFile          = MAX_PATH;
	ofn.lpstrFileTitle    = NULL;
	ofn.nMaxFileTitle     = MAX_PATH;
	ofn.lpstrInitialDir   = NULL;
	ofn.lpstrTitle        = NULL;
	ofn.Flags             = 0;
	ofn.nFileOffset       = 0;
	ofn.nFileExtension    = 0;
	ofn.lpstrDefExt       = TEXT("ker");
	ofn.lCustData         = 0L;
	ofn.lpfnHook          = NULL;
	ofn.lpTemplateName    = NULL;
}

BOOL CFileControl::PopFileSaveDlg(HWND hWnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{	
	
	ofn.hwndOwner      = hWnd;
	ofn.lpstrFile      = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.Flags          = OFN_OVERWRITEPROMPT;

	return ::GetSaveFileName(&ofn);
}
 
BOOL CFileControl::PopFileOpenDlg(HWND hWnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
	ofn.hwndOwner      = hWnd;
	ofn.lpstrFile      = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.Flags          = OFN_HIDEREADONLY | OFN_CREATEPROMPT;

	return ::GetOpenFileName(&ofn);
}

//Ū��
BOOL CFileControl::PopFileRead(HWND hWndEdit, PTSTR pstrFileName)
{

	DWORD  dwBytesRead = 0;
	HANDLE hFile;

	// open the file
	if( INVALID_HANDLE_VALUE == 
		(hFile = CreateFile( pstrFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL)))
		return FALSE;

	BOOL bRead = this->ReadFileToQueueNode(hFile, dwBytesRead);
	
	
	::CloseHandle(hFile);
	return bRead;

}

//�g��
BOOL CFileControl::PopFileWrite(HWND hWndEdit, PTSTR pstrFileName)
{
	
	HANDLE hFile;
	WORD wByteOrderMark = 0XFEFF;
	DWORD dwBytesWritten = 0;

	//open the file, creating it if necessary
	if( INVALID_HANDLE_VALUE == 
		( hFile = CreateFile( pstrFileName, GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL)))
		return FALSE;
	

	//�s�ɫe�M���Ŧr����
	if ( gMyList.CheckIsRemoveNullWordNode(_parent->GetHitID()) == TRUE )
	{
		this->_parent->GetCSelectRecInfo().SetIsShow(false);
		this->_parent->SetHitID(-1);
		//this->GetDrawTool()->MapDCToMonitor();
	}

	this->WriteQueueNodeToFile( hFile,dwBytesWritten );
	
	/*if( sizeof( FileObj ) != (int) dwBytesWritten) 
	{
		::CloseHandle(hFile);
		
		return FALSE;
	}*/

	::CloseHandle(hFile);
	return TRUE;
}


//�Nqueue����T�g�J�ɮ�
void CFileControl::WriteQueueNodeToFile(HANDLE &hFile,DWORD &dwBytesWritten)
{

	FileObj data;
	::SecureZeroMemory(&data, sizeof(FileObj));

	// �[��
	data.nObjType = IDM_ENCRYPT;
	::WriteFile(hFile, &data,  sizeof(data), &dwBytesWritten,NULL );
	
	MyListNode *ptr = gMyList.GetHead();
	WordNodeInfo *wordPtr;

	for( ptr ; ptr != NULL ; ptr = ptr->_rlink )
	{
		::SecureZeroMemory(&data, sizeof(FileObj));
		data.nObjType  = ptr->GetnObjType();
		data.ptBeg     = ptr->GetPtBeg();
	    data.ptEnd     = ptr->GetPtEnd();
		data.iPenColor = ptr->GetPenColor();
		data.iFigBg    = ptr->GetFigBg();

		if( data.nObjType == IDM_WORD_REC )
		{
		
			//�ɢ���l��  dont care in IDM_WORD_REC case
			data.iPenWidth = 1;
			data.iCharacterNum = ptr->GetWordNum();

			wordPtr	= ptr->GetHead();
			
			::WriteFile(hFile, &data,  sizeof(data), &dwBytesWritten,NULL );

			TCHAR cha;
			for( wordPtr ; wordPtr != NULL ; wordPtr = wordPtr->_rlink)
			{
				cha = wordPtr->_cha;
				::WriteFile(hFile, &cha,  sizeof(TCHAR), &dwBytesWritten,NULL );
			}
        	
		}
		else
		{
			
			data.iPenWidth = ptr->GetPenWidth();
			::WriteFile(hFile, &data,  sizeof(data), &dwBytesWritten,NULL );
		
		}
		
	}
}

//Ū�ɦ�QueueNode
BOOL CFileControl::ReadFileToQueueNode(HANDLE &hFile, DWORD &dwBytesRead)
{
	if( !hFile )
		return FALSE;

	FileObj data;
	::SecureZeroMemory(&data, sizeof(FileObj));
	DWORD dwBytesReaded = 0;

	int iFileLength = ::GetFileSize(hFile, NULL );
	int iLastID = -1;

	//�h��
	::ReadFile(hFile, &data,  sizeof(data), &dwBytesReaded,NULL );
	iFileLength -= dwBytesReaded;

	if( data.nObjType != IDM_ENCRYPT)
		return FALSE;

	//�M���e�� = ���U�M���\���
	_parent->ClickClearMenuItem();

	MyListNode* ptr;

	while ( iFileLength > 0 )
	{
	
		::ReadFile(hFile, &data,  sizeof(data), &dwBytesReaded,NULL );
		iFileLength -= dwBytesReaded;
		/** �]�w������*/
		_parent->GetCSelectRecInfo().SetType(data.nObjType);
	    _parent->GetCSelectRecInfo().SetIsShow(TRUE);
	    _parent->GetCSelectRecInfo().SetPosStart(data.ptBeg);
	    _parent->GetCSelectRecInfo().SetPosEnd(data.ptEnd);

		gMyList.AddGraphNode( data.nObjType, data.ptBeg, data.ptEnd, data.iPenColor, data.iFigBg, data.iPenWidth );
		iLastID = gMyList.GetCurrentNo();
	    
		
		if( data.nObjType == IDM_WORD_REC )
		{	
			ptr = gMyList.visitID_Node(iLastID);
			data.ptBeg.x += CommonVar::WORD_REC_MARGIN;
			data.ptBeg.y += CommonVar::WORD_REC_MARGIN;

			
			TCHAR cha;
			for( int i = 0 ; i < data.iCharacterNum ; i ++ )
			{
				::ReadFile(hFile, &cha,  sizeof(TCHAR), &dwBytesReaded,NULL );
				iFileLength -= dwBytesReaded;
				
				ptr->AppendWordNode(data.ptBeg, cha);
				
			}
		
			_parent->GetCTextOut()->WordReputationForInsertingWord( ptr->GetNo() );
		}
		
	
	}

	//�إߥثe���ID
	_parent->SetHitID( iLastID );

	ptr = NULL;

	// �����
	if( iLastID >= 0 )
	{
		POINT leftUpPt, rightDownPt;
		leftUpPt.x = 0;
		leftUpPt.y = 0;
		rightDownPt.x = CommonVar::DRAW_MAP_WIDTH;
		rightDownPt.y = CommonVar::DRAW_MAP_HEIGHT;
		//���صe��
		_parent->GetDrawTool()->RePaintWithClipRgn(leftUpPt,rightDownPt);
	}
	//�S���
	else
	{
		_parent->GetCSelectRecInfo().SetIsShow( false );
	}

	//��e���K�ܿù��W
	_parent->GetDrawTool()->MapDCToMonitor();

	if( iFileLength < 0 )
	{
		return FALSE;
	}

	return TRUE;
}
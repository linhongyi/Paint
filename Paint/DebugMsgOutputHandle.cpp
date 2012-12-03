#include "StdAfx.h"
#include "DebugMsgOutputHandle.h"
#include "PaintInfo.h"

DebugMsgOutputHandle* DebugMsgOutputHandle::_instance = NULL;
DebugMsgOutputHandle::DebugMsgOutputHandle(void)
{
}


DebugMsgOutputHandle::~DebugMsgOutputHandle(void)
{
}

DebugMsgOutputHandle* DebugMsgOutputHandle::Instance()
{
    if( !_instance)
    {
        _instance = new DebugMsgOutputHandle();
    }

    return _instance;
    
}

void DebugMsgOutputHandle::SetRedoUndoFormat(int Cmd, int No, POINT ptBeg, int wordNum1, int wordNum2 )
{
	   wsprintf(szDebugBuffer, _T("(Cmd:%d, No:%d, ptBeg.x: %d , ptBeg.y: %d, ptNewBeg.x: %d, ptNewBeg.y: %d)\n"), Cmd , No,ptBeg.x,ptBeg.y,wordNum1,wordNum2);
}

void DebugMsgOutputHandle::SetSzDebugBuffer(int No,int type,int mouseX, int mouseY, int BeforeScrollX, int BeforeScrollY)
{
	//#ifdef _NDEBUG
	     wsprintf(szDebugBuffer, _T("(No:%d,  type:%d, pt1.x %d, pt1.y:%d, pt2.x: %d, pt2.y: %d )\n"), No, type, mouseX, mouseY, BeforeScrollX, BeforeScrollY);
   // #else 
	   
   // #endif
}

void DebugMsgOutputHandle::MyDebugMsgShow(HWND hwnd)
{
    #ifdef _NDEBUG
	HDC hdc = GetDC(hwnd);
    TextOut(hdc ,250,250,szDebugBuffer,lstrlen(szDebugBuffer));
    ReleaseDC(hwnd,hdc);
    #endif
}
TCHAR* DebugMsgOutputHandle::GetSzDebugBuffer()
{	
    return szDebugBuffer;
}

void DebugMsgOutputHandle::ErrMsg()
{
	
	LPVOID lpMsgBuf;

  FormatMessage(
  FORMAT_MESSAGE_ALLOCATE_BUFFER |
  FORMAT_MESSAGE_FROM_SYSTEM |
  FORMAT_MESSAGE_IGNORE_INSERTS,
  NULL,
  GetLastError(), // �o�̤]�i�H�令�A�Q�ݪ��N�X�ȡA�Ҧp������8L�i�o"�Ŷ�����"
  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
  (LPTSTR) &lpMsgBuf,
  0,
  NULL);
  
   #ifdef _NDEBUG
         TCHAR strMsg[100];
	     wsprintf(strMsg,_T("�T���G%s  �N�X�G0x%x \n"), lpMsgBuf, GetLastError());
		 OutputDebugString(strMsg);
         OutputDebugString(_T("----------------------\n"));
    #else 
	    
    #endif

 
  LocalFree(lpMsgBuf);
}



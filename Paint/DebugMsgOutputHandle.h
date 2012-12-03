#pragma once
class DebugMsgOutputHandle
{
public:
	

    static DebugMsgOutputHandle* Instance();
	void   MyDebugMsgShow(HWND hwnd);
	void   SetSzDebugBuffer(int No,int type,int mouseX, int mouseY, int BeforeScrollX, int BeforeScrollY);
	void   SetRedoUndoFormat(int Cmd, int No, POINT ptBeg, int wordNum1, int wordNum2);
    TCHAR* GetSzDebugBuffer();
	void ErrMsg();
 
private:
	DebugMsgOutputHandle(void);
	~DebugMsgOutputHandle(void);
    static DebugMsgOutputHandle* _instance;
	TCHAR szDebugBuffer[100];
};


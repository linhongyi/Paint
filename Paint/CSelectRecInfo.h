#pragma once

/**************
* ����ظ�T�A�O�_���
***************/
class CSelectRecInfo
{
public:
	CSelectRecInfo(void);
	~CSelectRecInfo(void);

	void SetIsShow(bool foo);
	void SetPosStart(POINT pt);
	void SetPosEnd(POINT pt);
	void SetType(int val);

	POINT GetPosStart();
	POINT GetPosEnd();
	BOOL  GetIsShow();
	int   GetType();

	POINT _posStart, _posEnd;
	BOOL  _isShow;
	int   _iType;
};


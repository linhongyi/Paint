#pragma once
class WordNodeInfo
{
public:
	WordNodeInfo(void);
	~WordNodeInfo(void);
	TCHAR _cha;
	POINT _pt;
	WordNodeInfo* _rlink;
	WordNodeInfo* _llink;
	void FreeObj();
};


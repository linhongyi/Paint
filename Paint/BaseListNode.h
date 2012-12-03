#pragma once
#include <deque>
using namespace std;
class BaseListNode
{

public:
	
	void SetnObjType(int type);
    void SetPtBeg(int x,int y);
	void SetPtEnd(int x,int y);
    void SetPenColor(int val);
	void SetFigBg(int val);
	void SetNo(int val);


	void SetPenWidth(int val)
	{
		_iPenWidth = val;
	};
	int GetPenWidth(){return _iPenWidth;};
	

	int GetnObjType();
	POINT GetPtBeg();
	POINT GetPtEnd();
    int GetPenColor();
	int GetFigBg();
	int GetNo();

  
	
protected:
	BaseListNode(void);
	~BaseListNode(void);

private:
	POINT _ptBeg;
	POINT _ptEnd;
	int _nObjType;
    int _iPenColor;
	int _iPenWidth;
    int _iFigBg;
	int _iNo;

};


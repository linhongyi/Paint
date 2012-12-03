#include "StdAfx.h"
#include "BaseListNode.h"


BaseListNode::BaseListNode(void)
{
	 _ptBeg.x = _ptBeg.y = 0;
	 _ptEnd.x = _ptBeg.y = 0;
	 _nObjType = 0;
     _iPenColor = 0;
    
     _iFigBg = 0;
	 _iNo = 0;
}


BaseListNode::~BaseListNode(void)
{
}

/** 設定圖案形式*/
void BaseListNode::SetnObjType(int type)
{
	_nObjType = type;
}

/** 獲得圖案形式*/
int BaseListNode::GetnObjType()
{
	return _nObjType;
}

/** 設定起始點*/
void BaseListNode::SetPtBeg(int x, int y)
{
	_ptBeg.x = x;
	_ptBeg.y = y;
}

/** 獲得起始點*/
POINT BaseListNode::GetPtBeg()
{
	return _ptBeg;
}

/** 設定終點*/
void BaseListNode::SetPtEnd(int x, int y)
{
	_ptEnd.x = x;
	_ptEnd.y = y;
}

/** 獲得終點*/
POINT BaseListNode::GetPtEnd()
{
	return _ptEnd;
}



void BaseListNode::SetPenColor(int val)
{
    _iPenColor = val;
}

int BaseListNode::GetPenColor()
{
    return _iPenColor;
}

void BaseListNode::SetFigBg(int val)
{
	this->_iFigBg = val;
}

int BaseListNode::GetFigBg()
{
    return this->_iFigBg;
}



void BaseListNode::SetNo(int val)
{
	this->_iNo = val;
}

int BaseListNode::GetNo()
{
	return this->_iNo;
}

#include "StdAfx.h"
#include "CSelectRecInfo.h"
#include <assert.h>

CSelectRecInfo::CSelectRecInfo(void)
{
    _posStart.x = _posStart.y = 0;
	_posEnd.x = _posEnd.y = 0;

	_isShow = FALSE;
	_iType = 0;
}


CSelectRecInfo::~CSelectRecInfo(void)
{
}

void CSelectRecInfo::SetIsShow(bool foo)
{
	this->_isShow = foo;
}

BOOL CSelectRecInfo::GetIsShow()
{
	return this->_isShow;
}
void CSelectRecInfo::SetPosStart(POINT pt)
{
	assert( pt.y >= 0 );
	this->_posStart.x = pt.x;
	this->_posStart.y = pt.y;
}

POINT CSelectRecInfo::GetPosStart()
{
	return this->_posStart;
}

void CSelectRecInfo::SetPosEnd(POINT pt)
{
	this->_posEnd.x = pt.x;
	this->_posEnd.y = pt.y;
}

POINT CSelectRecInfo::GetPosEnd()
{
	return this->_posEnd;
}

void CSelectRecInfo::SetType(int val)
{
	assert( _iType >= 0 );
	this->_iType = val;
}

BOOL CSelectRecInfo::GetType()
{
	return this->_iType;
}

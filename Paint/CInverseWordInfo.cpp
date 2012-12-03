#include "StdAfx.h"
#include "CInverseWordInfo.h"
#include "DebugMsgOutputHandle.h"

CInverseWordInfo::CInverseWordInfo(void)
{
		_caretPt1.x = _caretPt1.y = 0;
	_caretPt2.x = _caretPt2.y = 0;
}


CInverseWordInfo::~CInverseWordInfo(void)
{
}


void CInverseWordInfo::SetCaretPt1(POINT pt)
{
	this->_caretPt1 = pt;
}

POINT CInverseWordInfo::GetCaretPt1()
{
	return this->_caretPt1;
}

void CInverseWordInfo::SetCaretPt2(POINT pt)
{

	this->_caretPt2 = pt;
}

POINT CInverseWordInfo::GetCaretPt2()
{
	return this->_caretPt2;
}

void CInverseWordInfo::ResetInverseWordScope()
{
	this->_caretPt2.x = this->_caretPt1.x;
	this->_caretPt2.y = this->_caretPt1.y;
	
}

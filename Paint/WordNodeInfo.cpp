#include "StdAfx.h"
#include "WordNodeInfo.h"


WordNodeInfo::WordNodeInfo(void)
{
	this->_cha = 0;
	this->_pt.x = 0; 
	this->_pt.y = 0;
	this->FreeObj();
}


WordNodeInfo::~WordNodeInfo(void)
{
	this->FreeObj();
}

void WordNodeInfo::FreeObj()
{
	this->_llink = NULL;
	this->_rlink = NULL;
}
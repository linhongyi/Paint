#include "StdAfx.h"
#include "ToolKits.h"


static HINSTANCE _hLibrary;

ToolKits::ToolKits(void)
{
	_hLibrary = NULL;
}


ToolKits::~ToolKits(void)
{
	this->ReleaseDLL();
}

void ToolKits::IniDLL()
{
	if( _hLibrary == NULL )
	{
		_hLibrary = ::LoadLibrary( TEXT("dllmy.dll"));

		if( !_hLibrary) 
			::MessageBox(NULL,_T(" not find the dllmy.dll\n"),NULL,NULL);
		else 
			WelcomeUseThisDLL();
	}
}

void ToolKits::ReleaseDLL()
{
	::FreeLibrary( _hLibrary );
}


void ToolKits::WelcomeUseThisDLL()
{
	try
	{
		if( IsIniDLL() == FALSE )
			throw _T("-----------not LoadLibrary, plz call iniDLL-----\n");
	}
	catch( TCHAR errStr[] )
	{
		::MessageBox(NULL,errStr,NULL,NULL);
		return;
	}

	typedef void* (WINAPI *PTR) ();
	PTR  ptr = ( PTR )::GetProcAddress(_hLibrary,(LPCSTR)1);
	ptr();
	ptr = NULL;
}

void ToolKits::Swap(long &x,long &y )
{
	try
	{
		if( IsIniDLL() == FALSE )
			throw _T("-----------not LoadLibrary, plz call iniDLL-----\n");
	}
	catch( TCHAR errStr[] )
	{
		::MessageBox(NULL,errStr,NULL,NULL);
		return;
	}

	typedef void* (WINAPI *PTR) ( long &, long &);
	PTR  ptr = ( PTR )::GetProcAddress(_hLibrary,"LongSwap");
	ptr(x,y);
	ptr = NULL;

}

void ToolKits::Swap(int &x,int &y )
{
	try
	{
		if( IsIniDLL() == FALSE )
			throw _T("-----------not LoadLibrary, plz call iniDLL-----\n");
	}
	catch( TCHAR errStr[] )
	{
		::MessageBox(NULL,errStr,NULL,NULL);
		return;
	}

	typedef void* (WINAPI *PTR) ( int &, int &);
	PTR  ptr = ( PTR )::GetProcAddress(_hLibrary,"IntSwap");
	ptr(x,y);
	ptr = NULL;
}

/**���W�ƨ��I�Ҧ������*/
void ToolKits::Normalization(POINT &iniPt, POINT &endPos)
{
	try
	{
		if( IsIniDLL() == FALSE )
			throw _T("-----------not LoadLibrary, plz call iniDLL-----\n");
	}
	catch( TCHAR errStr[] )
	{
		::MessageBox(NULL,errStr,NULL,NULL);
		return;
	}

	typedef void* (WINAPI *PTR) ( POINT &, POINT &);
	PTR  ptr = ( PTR )::GetProcAddress(_hLibrary,"Normalization");
	ptr(iniPt,endPos);
	ptr = NULL;

}

/**�|���I�A�D�Ҧ��̤j���*/
void ToolKits::FindMaxScopeWithFourPoint( POINT &recPt1, POINT &recPt2, POINT &recPt3, POINT &recPt4)
{
	try
	{
		if( IsIniDLL() == FALSE )
			throw _T("-----------not LoadLibrary, plz call iniDLL-----\n");
	}
	catch( TCHAR errStr[] )
	{
		::MessageBox(NULL,errStr,NULL,NULL);
		return;
	}

	typedef void* (WINAPI *PTR) ( POINT &, POINT &, POINT &, POINT &);

	PTR  ptr = ( PTR )::GetProcAddress(_hLibrary,"FindMaxScopeWithFourPoint");
	ptr(recPt1,recPt2,recPt3,recPt4);
	ptr = NULL;

}


/**���I�ھ�Y��V�ȡA�ƦC*/
void ToolKits::TunePtsWithPosY(POINT *ptrPt1, POINT *ptrPt2)
{
	try
	{
		if( IsIniDLL() == FALSE )
			throw _T("-----------not LoadLibrary, plz call iniDLL-----\n");
	}
	catch( TCHAR errStr[] )
	{
		::MessageBox(NULL,errStr,NULL,NULL);
		return;
	}

	typedef void* (WINAPI *PTR) ( POINT *, POINT *);

	PTR  ptr = ( PTR )::GetProcAddress(_hLibrary,"TunePtsWithPosY");
	ptr(ptrPt1,ptrPt2);
	ptr = NULL;

}

/** �ϥզr���I�Ҧ��d��A���W��*/
void ToolKits::InverseWordNormalization( POINT &iniPt, POINT &endPt)
{
	try
	{
		if( IsIniDLL() == FALSE )
			throw _T("-----------not LoadLibrary, plz call iniDLL-----\n");
	}
	catch( TCHAR errStr[] )
	{
		::MessageBox(NULL,errStr,NULL,NULL);
		return;
	}

	typedef void* (WINAPI *PTR) ( POINT &, POINT &);

	PTR  ptr = ( PTR )::GetProcAddress(_hLibrary,"InverseWordNormalization");
	ptr(iniPt,endPt);
	ptr = NULL;

}

BOOL ToolKits::IsIniDLL()
{
	if( _hLibrary == NULL )
		return FALSE;
	else
		return TRUE;
}
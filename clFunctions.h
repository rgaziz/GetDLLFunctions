//----- CLASS CLFUNCTIONS_H ----------------------------------------------------
#ifndef clFunctionsH
#define clFunctionsH
//----- INCLUDES ---------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <iostream>
#include <vector>
//----- CLFUNCTIONS ------------------------------------------------------------
class clFunctions
{
	public:
		clFunctions();
		~clFunctions();
		std::vector<AnsiString> &GetFunctions(LPCTSTR _lpszPathToDll);
	private:
		IMAGE_NT_HEADERS* GetHeader(LPBYTE _pBase);
		IMAGE_SECTION_HEADER* GetSection(IMAGE_NT_HEADERS* _pHeader, DWORD _dwRVA);
		LPBYTE GetFilePointer(LPBYTE _pBase, DWORD _dwRVA);
		LPBYTE OpenPEFile(LPCTSTR _lpszFileName);
		void ClosePEFile(LPBYTE _pBase);
	private:
    std::vector<AnsiString> vecResult;
};
//------------------------------------------------------------------------------
#endif
//------------------------------------------------------------------------------

//----- CLASS FUNCTIONS_CPP ----------------------------------------------------
#pragma hdrstop
//----- INCLUDES ---------------------------------------------------------------
#include "clFunctions.h"
//------------------------------------------------------------------------------
#pragma package(smart_init)
//----- CONSTRUCTOR ------------------------------------------------------------
clFunctions::clFunctions()
{
}

//----- GET HEADER -------------------------------------------------------------
IMAGE_NT_HEADERS* clFunctions::GetHeader(LPBYTE _pBase)
{
	if (_pBase == NULL) return NULL;

	IMAGE_DOS_HEADER* pDosHeader = (IMAGE_DOS_HEADER*)_pBase;

	if (IsBadReadPtr(pDosHeader, sizeof(IMAGE_DOS_HEADER)))
		return NULL;

	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		return NULL;

	IMAGE_NT_HEADERS* pHeader = (IMAGE_NT_HEADERS*)(_pBase + pDosHeader->e_lfanew);

	if (IsBadReadPtr(pHeader, sizeof(IMAGE_NT_HEADERS)))
		return NULL;

	if (pHeader->Signature != IMAGE_NT_SIGNATURE)
		return NULL;

	return pHeader;
}

//----- GET SECTION ------------------------------------------------------------
IMAGE_SECTION_HEADER* clFunctions::GetSection(IMAGE_NT_HEADERS* _pHeader,
                                              DWORD _dwRVA)
{
	if (_pHeader == NULL) return NULL;

	IMAGE_SECTION_HEADER* pSectHeader = IMAGE_FIRST_SECTION(_pHeader);

	for (UINT i = 0; i < _pHeader->FileHeader.NumberOfSections; i++, pSectHeader++)
	{
		if (_dwRVA >= pSectHeader->VirtualAddress
				&& _dwRVA < pSectHeader->VirtualAddress + pSectHeader->Misc.VirtualSize)
			return pSectHeader;
	}

	return NULL;
}

//----- GET FILE POINTER -------------------------------------------------------
LPBYTE clFunctions::GetFilePointer(LPBYTE _pBase, DWORD _dwRVA)
{
	if (_pBase == NULL) return NULL;

	IMAGE_SECTION_HEADER *pSectHeader = GetSection(GetHeader(_pBase), _dwRVA);

	if (pSectHeader == NULL) return _pBase + _dwRVA;

	return _pBase + pSectHeader->PointerToRawData + (_dwRVA - pSectHeader->VirtualAddress);
}

//----- OPEN FILE --------------------------------------------------------------
LPBYTE clFunctions::OpenPEFile(LPCTSTR _lpszFileName)
{
	HANDLE hFile = CreateFile(_lpszFileName, GENERIC_READ, FILE_SHARE_READ, NULL,
														OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (hFile == INVALID_HANDLE_VALUE) return NULL;

	HANDLE hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	CloseHandle(hFile);

	LPBYTE pBase = NULL;

	if (hMapping != NULL)
	{
		pBase = (LPBYTE)MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
		CloseHandle(hMapping);
	}

	return pBase;
}

//----- CLOSE FILE -------------------------------------------------------------
void clFunctions:: ClosePEFile(LPBYTE _pBase)
{
	if (_pBase != NULL) UnmapViewOfFile(_pBase);
}

//----- GET FUNCTIONS FROM DLL -------------------------------------------------
std::vector<AnsiString> &clFunctions::GetFunctions(LPCTSTR _lpszPathToDll)
{
	LPBYTE pBase = OpenPEFile(_lpszPathToDll);
	if (pBase == NULL)
	{
		std::cout << "File not found! \n";
		return vecResult;
	}

	IMAGE_NT_HEADERS* pHeader = GetHeader(pBase);
	if (pHeader == NULL)
	{
		std::cout << "It is not a PE file!\n";
		return vecResult;
	}

	IMAGE_DATA_DIRECTORY ExportDataDir;
	ExportDataDir = pHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
	DWORD dwExportDirStart = ExportDataDir.VirtualAddress;

	IMAGE_EXPORT_DIRECTORY *pExportDir;
	pExportDir= (IMAGE_EXPORT_DIRECTORY*)GetFilePointer(pBase, dwExportDirStart);

	LPDWORD pNameTable = (LPDWORD)GetFilePointer(pBase, pExportDir->AddressOfNames);
	LPWORD pOrdTable = (LPWORD)GetFilePointer(pBase, pExportDir->AddressOfNameOrdinals);

	for (UINT i = 0; i < pExportDir->NumberOfFunctions; i++)
		for (UINT j = 0; j < pExportDir->NumberOfNames; j++)
			if (pOrdTable[j] == i)
			{
				AnsiString strFuncName = (char*)GetFilePointer(pBase, pNameTable[j]);
        vecResult.push_back(strFuncName);
				break;
			}

	ClosePEFile(pBase);

  return vecResult;
}

//----- DESTRUCTOR -------------------------------------------------------------
clFunctions::~clFunctions()
{
  vecResult.clear();
}
//------------------------------------------------------------------------------



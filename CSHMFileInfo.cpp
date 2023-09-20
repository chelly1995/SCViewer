#pragma once
#include "CSHMFileInfo.h"
#include <SC_Error.h>

int CSHMFileInfo::CreateSHMSCView(wchar_t *pTempFileName)
{
	if (nullptr == pTempFileName)
		return SC_ERROR_INVALID_PARAMETER;

	HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 1024, &pTempFileName[1]);
	wcscpy_s(m_szKey, &pTempFileName[1]);

	return SC_ERROR_SUCCESS;
}

int CSHMFileInfo::WriteSHMSCView(wchar_t* pSrcFileName)
{
	if (nullptr == pSrcFileName)
		return SC_ERROR_INVALID_PARAMETER;

	HANDLE hMapFile = OpenFileMappingW(FILE_MAP_ALL_ACCESS,	FALSE, m_szKey);

	if (hMapFile == NULL) 
		return SC_ERROR_NOT_FOUND;

	wchar_t* pData = static_cast<wchar_t*>(MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 1024));    // Number of bytes to map
	
	if (nullptr == pData)
	{
		CloseHandle(hMapFile);
		return SC_ERROR_NOT_FOUND;
	}

	wcscpy_s(pData, 512, pSrcFileName);

	UnmapViewOfFile(pData);
	CloseHandle(hMapFile);

	return SC_ERROR_SUCCESS;
}

int CSHMFileInfo::ReadSHMSCView(wchar_t* pKey, wchar_t* pValue)
{
	if (nullptr == pKey || nullptr == pValue)
		return SC_ERROR_INVALID_PARAMETER;

	HANDLE hMapFile = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, pKey);

	if (hMapFile == NULL)
		return SC_ERROR_NOT_FOUND;

	wchar_t* pData = static_cast<wchar_t*>(MapViewOfFile(hMapFile, FILE_MAP_WRITE, 0, 0, 1024));    // Number of bytes to map

	if (nullptr == pData)
	{
		CloseHandle(hMapFile);
		return SC_ERROR_NOT_FOUND;
	}

	wcscpy_s(pValue, 512, pData);

	UnmapViewOfFile(pData);
	CloseHandle(hMapFile);

	return SC_ERROR_SUCCESS;
}
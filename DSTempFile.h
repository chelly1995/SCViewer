#pragma once

#include <set>
#include <vector>
#include <string>
using namespace std;
#include "SCStrUtil.h"
#include "IDSEDCommon.h"

class DSTempFile  : public IDSTempFile
{
public:
	DSTempFile( LPCSTR lpFileName );
	DSTempFile( LPCWSTR lpFileName );
	virtual	~DSTempFile();

	HANDLE	OpenTempFile( DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile );
	HANDLE	CreateTempFile( DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile );
	LPCWSTR	GetSrcFileName() { return m_strFilename.c_str(); }
	LPCWSTR	GetTempFileName() { return m_strTempFilename.c_str(); }
	HANDLE	GetTempFileHandle() { return m_hTempFile; }
	DWORD	GetDesiredAccess() { return m_dwDesiredAccess; }
	void	SetFileHandle( HANDLE handle ) { m_hTempFile = handle; }

private:
	wstring		m_strFilename;					// 원본 파일명
	wstring		m_strTempFilename;				// 임시 파일명
	wstring		m_strTempDelFilename;			// 삭제될 임시 파일명
	HANDLE		m_hTempFile;
	DWORD		m_dwDesiredAccess;
};

class DSTempFileMgr  : public IDSTempFileMgr
{
public:
	DSTempFileMgr();
	~DSTempFileMgr();

	static DSTempFileMgr* GetInst();


	/**
	* 해당 파일 명 및 옵션을 입력 받아 자동으로 임시파일 생성 리스트 추가 오픈으로 핸들을 돌려준다. 
	* 만약 해당 핸들이 존재 할 경우 자동으로 해당 템프파일을 오픈해 준다.
	* @param[in]  LPCWSTR lpFileName 임시파일 오픈전의 원본 파일명
	* @return HANDLE 오픈된 임시파일의 핸들 
	*/
	HANDLE		OpenOrCreateTempFile( LPCWSTR lpFileName,  DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile );
	HANDLE		CreateTempFile( LPCWSTR lpFileName,  DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile );

	DSTempFile*	NewTempFile( LPCWSTR lpFileName );
	/**
	* 파일 핸들을 사용하여 임시파일 클레스를 리스트에서 찾아서 반환 한다.
	* @param[In]  HANDLE hFile 리스트에서 찾기 위한 핸들	
	* @return DSTempFile 클래스 포인트 반환
	*/
	DSTempFile* GetTempFilePtr( HANDLE hFile );

	/**
	* 파일 경로를 사용하여 임시파일 클레스를 리스트에서 찾아서 반환 한다.
	* @param[In]  HANDLE hFile 리스트에서 찾기 위한 핸들	
	* @return DSTempFile 클래스 포인트 반환
	*/	
	DSTempFile* GetTempFilePtr( LPCWSTR lpFileName );
	DSTempFile* GetTempFilePtr( LPCSTR lpFileName ) { return GetTempFilePtr(STR_TO_WSTR(lpFileName)); }

	
	DSTempFile* GetAlternationFilePtr( LPCWSTR lpFileName );
	DSTempFile* GetAlternationFilePtr( LPCSTR lpFileName ) { return GetAlternationFilePtr(STR_TO_WSTR(lpFileName)); }


	void RemoveTempFile( HANDLE hFile );
private:
	/**
	* 템프파일 클레스를 리스트에 추가한다.
	* @param[In]  DSTempFile* pFile 템프 파일 리스트
	* @return 없음
	*/
	VOID AddTempFile( DSTempFile* pFile ) { m_TempFilePtrList.push_back(pFile); }
	


private:
	vector<DSTempFile*>			m_TempFilePtrList;				// temp 파일 리스트

public:
	vector<wstring>			m_DelTempFilePtrList;
};
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
	wstring		m_strFilename;					// ���� ���ϸ�
	wstring		m_strTempFilename;				// �ӽ� ���ϸ�
	wstring		m_strTempDelFilename;			// ������ �ӽ� ���ϸ�
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
	* �ش� ���� �� �� �ɼ��� �Է� �޾� �ڵ����� �ӽ����� ���� ����Ʈ �߰� �������� �ڵ��� �����ش�. 
	* ���� �ش� �ڵ��� ���� �� ��� �ڵ����� �ش� ���������� ������ �ش�.
	* @param[in]  LPCWSTR lpFileName �ӽ����� �������� ���� ���ϸ�
	* @return HANDLE ���µ� �ӽ������� �ڵ� 
	*/
	HANDLE		OpenOrCreateTempFile( LPCWSTR lpFileName,  DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile );
	HANDLE		CreateTempFile( LPCWSTR lpFileName,  DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile );

	DSTempFile*	NewTempFile( LPCWSTR lpFileName );
	/**
	* ���� �ڵ��� ����Ͽ� �ӽ����� Ŭ������ ����Ʈ���� ã�Ƽ� ��ȯ �Ѵ�.
	* @param[In]  HANDLE hFile ����Ʈ���� ã�� ���� �ڵ�	
	* @return DSTempFile Ŭ���� ����Ʈ ��ȯ
	*/
	DSTempFile* GetTempFilePtr( HANDLE hFile );

	/**
	* ���� ��θ� ����Ͽ� �ӽ����� Ŭ������ ����Ʈ���� ã�Ƽ� ��ȯ �Ѵ�.
	* @param[In]  HANDLE hFile ����Ʈ���� ã�� ���� �ڵ�	
	* @return DSTempFile Ŭ���� ����Ʈ ��ȯ
	*/	
	DSTempFile* GetTempFilePtr( LPCWSTR lpFileName );
	DSTempFile* GetTempFilePtr( LPCSTR lpFileName ) { return GetTempFilePtr(STR_TO_WSTR(lpFileName)); }

	
	DSTempFile* GetAlternationFilePtr( LPCWSTR lpFileName );
	DSTempFile* GetAlternationFilePtr( LPCSTR lpFileName ) { return GetAlternationFilePtr(STR_TO_WSTR(lpFileName)); }


	void RemoveTempFile( HANDLE hFile );
private:
	/**
	* �������� Ŭ������ ����Ʈ�� �߰��Ѵ�.
	* @param[In]  DSTempFile* pFile ���� ���� ����Ʈ
	* @return ����
	*/
	VOID AddTempFile( DSTempFile* pFile ) { m_TempFilePtrList.push_back(pFile); }
	


private:
	vector<DSTempFile*>			m_TempFilePtrList;				// temp ���� ����Ʈ

public:
	vector<wstring>			m_DelTempFilePtrList;
};
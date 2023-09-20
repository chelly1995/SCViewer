// NTempFile.cpp: implementation of the DSTempFile class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "DSTempFile.h"
#include "CODECMgr.h"
#include "DSFileMgr.h"
#include "SCStrUtil.h"
#include "DSStatus.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DSTempFile::DSTempFile( LPCSTR lpFileName )
: m_dwDesiredAccess(NULL)
, m_strFilename(CStrTowstring( lpFileName ))
{
	m_hTempFile		= INVALID_HANDLE_VALUE;
}

DSTempFile::DSTempFile( LPCWSTR lpFileName )
: m_dwDesiredAccess(NULL)
, m_strFilename(lpFileName)
{
	m_hTempFile		= INVALID_HANDLE_VALUE;
}

DSTempFile::~DSTempFile()
{
	if( !m_strTempFilename.empty() )
	{
		WipeOutFileEX( (LPWSTR)m_strTempFilename.c_str() );
	}

	if( !m_strTempDelFilename.empty() )
	{
		WipeOutFileEX( (LPWSTR)m_strTempDelFilename.c_str() );
	}
}

HANDLE DSTempFile::CreateTempFile( DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile )
{	
	WCHAR szTempPath[1024] = {0,};
	WCHAR szTempFilePath[1024] = {0,};

	// 임시 파일명 구함
	::GetTempPathW( 1024, szTempPath );
	::GetTempFileNameW( szTempPath, L"~$" , 0, szTempFilePath );
	
	DeleteFileW(szTempFilePath);

	// 임시 파일명 멤버에 저장
	m_strTempFilename = szTempFilePath;
	LPWSTR	szExt = PathFindExtensionW(szTempFilePath);

	// [2011.12.15] ghlee [TFS:30283 ]:[HKMC]속도 개선: SDSMan에서 파일 직접 암/복호화 하도록 로직수정.
	if( DT_SOM_SECURITY == SCAPI_IDSFileMgr()->IsEncryptedFile( m_strFilename.c_str() ) )
	{
		WipeOutFileEX( szTempFilePath );
		// 복호화 생성
		::CopyFileW( m_strFilename.c_str(), szTempFilePath, FALSE );
	}
	// [2011.12.15] End.

	m_dwDesiredAccess = dwDesiredAccess;

	// [TFS:34720] modified by jhkwon - start
	// : MS-Office 2010 속도 개선 작업 .. 
	// : 기존에 decrypt 를 2회 시도하고 있었음 ..	
	int nError = g_codecMgr.MakeDecryptedFile( m_strFilename.c_str(), szTempFilePath );

	if ( nError != STAT_SUCCESS)
	{
		//TFS 49793 워드인 경우 비활성화가 안되고 그 상태에서 저장시 보안 문서를 손상 시킴.
// 		if ( STAT_CANNOT_AUTHENTICATE == nError )
// 		{
// 			::SetLastError( ERROR_ACCESS_DENIED );
// 			return INVALID_HANDLE_VALUE;
// 		}

		m_hTempFile = ::CreateFileW( m_strTempFilename.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile );
	
		if( INVALID_HANDLE_VALUE != m_hTempFile )
		{
			if( 0 == ::GetFileSize(m_hTempFile, NULL) )
			{
				::CloseHandle(m_hTempFile);
				WipeOutFileEX( (LPWSTR) m_strTempFilename.c_str() );
				m_hTempFile = INVALID_HANDLE_VALUE;
			}
		}
		
		SXTraceErr(MODULE, L"ERROR! MakeDecryptedFile Fail and CreateFile(%08x)(%s)", m_hTempFile, m_strTempFilename.c_str() );

		return m_hTempFile;
	}

	if ( TRUE == SCAPI_IDSIPCPolicy()->IsUseFileCache() )
	{
		CTempFileCache* pCache = SCAPI_IDSFileMgr()->GetTempFileCache( (LPTSTR)m_strFilename.c_str() );

		if ( NULL == pCache )
		{
			DWORD dwFileSize = 0;
			BOOL bGetWriteTime = FALSE;
			FILETIME ftCreate, ftAccess, ftWrite;

			memset( &ftCreate, 0x00, sizeof(FILETIME) );
			memset( &ftAccess, 0x00, sizeof(FILETIME) );
			memset( &ftWrite, 0x00, sizeof(FILETIME) );

			HANDLE hFile = ::CreateFile( (LPTSTR)m_strFilename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

			if ( hFile != INVALID_HANDLE_VALUE )
			{
				dwFileSize = ::GetFileSize( hFile, NULL );

				if ( TRUE == ::GetFileTime( hFile, &ftCreate, &ftAccess, &ftWrite ) )
				{
					bGetWriteTime = TRUE;
				}					

				::CloseHandle( hFile );
			}				

			if ( dwFileSize != 0 && TRUE == bGetWriteTime )
			{
				SCAPI_IDSFileMgr()->InsertTempFileCache( (LPTSTR)m_strFilename.c_str(), dwFileSize, ftWrite );
				SXTraceInfo(MODULE, L"Insert Cache File : Name [%s], Size [%d], Time H/L [%d]/[%d]", m_strFilename.c_str(), dwFileSize, ftWrite.dwHighDateTime, ftWrite.dwLowDateTime );
			}
		}
	}
	
	DSFileMgr::GetInst()->InsertHasBeenOpendBackupFiles( m_strTempFilename.c_str() );

	// 파일 핸들 돌려줌
	m_hTempFile = ::CreateFileW( m_strTempFilename.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile );	

	SXTraceInfo(MODULE, L"SetLastOpenedDocumentFile [%s]", m_strFilename.c_str() );

	if ( lstrcmpi( _T(".tmp"), szExt ) != 0 )
		DSFileMgr::GetInst()->SetLastOpenedDocumentFile( m_strFilename.c_str() );

	return m_hTempFile;
}

HANDLE DSTempFile::OpenTempFile( DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile )
{
	// [TFS:34720] added by jhkwon - start
	// : MS-Office 2010 속도 개선 작업 .. 
	CTempFileCache* pCache = NULL;

	if ( TRUE == SCAPI_IDSIPCPolicy()->IsUseFileCache() )
	{
		if ( DT_SDS_SECURITY == SCAPI_IDSFileMgr()->IsEncryptedFile( m_strFilename.c_str() ) )
		{
			DWORD dwFileSize = 0;
			BOOL bGetWriteTime = FALSE;
			FILETIME ftCreate, ftAccess, ftWrite;

			memset( &ftCreate, 0x00, sizeof(FILETIME) );
			memset( &ftAccess, 0x00, sizeof(FILETIME) );
			memset( &ftWrite, 0x00, sizeof(FILETIME) );

			HANDLE hFile = ::CreateFile( (LPTSTR)m_strFilename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

			if ( hFile != INVALID_HANDLE_VALUE )
			{
				dwFileSize = ::GetFileSize( hFile, NULL );

				if ( TRUE == ::GetFileTime( hFile, &ftCreate, &ftAccess, &ftWrite ) )
				{
					bGetWriteTime = TRUE;
				}					

				::CloseHandle( hFile );
			}

			if ( dwFileSize != 0 && TRUE == bGetWriteTime )
			{
				pCache = SCAPI_IDSFileMgr()->GetTempFileCache( (LPTSTR)m_strFilename.c_str() );

				if ( NULL != pCache )
				{
					FILETIME ftCacheWrite = pCache->GetLastWriteTime();

					if ( dwFileSize == pCache->GetFileSize() 
						 && 0 == CompareFileTime( &ftWrite, &ftCacheWrite )
						 && FALSE == pCache->GetAfterEncrypted() )
					{
						SXTraceInfo(MODULE, L"Use Cache File : Name [%s], Size [%d], Time H/L [%d]/[%d]", m_strFilename.c_str(), dwFileSize, ftWrite.dwHighDateTime, ftWrite.dwLowDateTime );

						m_hTempFile = ::CreateFile( m_strTempFilename.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile );						

						//!< TFS 89145 일부 사용자 환경에서 MSOffice2013 pptx 임시파일 핸들을 닫아주지 않아 공유 위반 발생되어 복구모드로 열람되는 장애대응
						if( NULL == m_hTempFile || INVALID_HANDLE_VALUE == m_hTempFile )
						{
							int nErr = GetLastError();
							if( ERROR_ACCESS_DENIED == nErr || ERROR_SHARING_VIOLATION == nErr )
							{
								WCHAR szTempPath[1024] = {0,};
								WCHAR szTempFilePath[1024] = {0,};

								::GetTempPathW( 1024, szTempPath );
								if( 0 != ::GetTempFileNameW( szTempPath, L"~$" , 0, szTempFilePath ) )
								{
									DeleteFileW( szTempFilePath );

									m_strTempDelFilename = m_strTempFilename;

									DSTempFileMgr::GetInst()->m_DelTempFilePtrList.push_back(m_strTempDelFilename);
									m_strTempFilename = szTempFilePath;

									if( DT_SDS_SECURITY == SCAPI_IDSFileMgr()->IsEncryptedFile( m_strFilename.c_str() ) )
									{
										// 복호화 생성
										if( TRUE == ::CopyFileW( m_strTempDelFilename.c_str(), m_strTempFilename.c_str(), FALSE ) )
											m_hTempFile = ::CreateFile( m_strTempFilename.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile );
									}
								}
							}
						}

						return m_hTempFile;
					}
					else
					{
						SXTraceInfo(MODULE, L"Reset Cache File : Name [%s], Size [%d], Time H/L [%d]/[%d]", m_strFilename.c_str(), dwFileSize, ftWrite.dwHighDateTime, ftWrite.dwLowDateTime );

						pCache->SetFileSize( dwFileSize );
						pCache->SetLastWriteTime( ftWrite );
					}
				}
				else
				{
					SCAPI_IDSFileMgr()->InsertTempFileCache( (LPTSTR)m_strFilename.c_str(), dwFileSize, ftWrite );
					SXTraceInfo(MODULE, L"Insert Cache File : Name [%s], Size [%d], Time H/L [%d]/[%d]", m_strFilename.c_str(), dwFileSize, ftWrite.dwHighDateTime, ftWrite.dwLowDateTime );
				}
			}
		}
	}

	// [2011.12.15] ghlee [TFS:30283 ]:[HKMC]속도 개선: SDSMan에서 파일 직접 암/복호화 하도록 로직수정.
	if( DT_SOM_SECURITY == SCAPI_IDSFileMgr()->IsEncryptedFile( m_strFilename.c_str() ) )
	{
		// 복호화 생성
		::CopyFileW( m_strFilename.c_str(), m_strTempFilename.c_str(), FALSE );
	}
	// [2011.12.15] End.

	if( !m_strTempDelFilename.empty() )
	{
		if( FALSE != WipeOutFileEX( (LPWSTR)m_strTempDelFilename.c_str() ) )
		{
			m_strTempDelFilename = _T("");
		}
	}

	// [TFS:34720] modified by jhkwon - start
	// : MS-Office 2010 속도 개선 작업 .. 
	// 대용량 암호화 파일에 대한 공유 메모리 복호화 시 공유 위반 메시지 발생, SDSMAN.exe에서 복호화 하도록 원복 (MakeDecryptedFileEx -> MakeDecryptedFile)
	int nError = g_codecMgr.MakeDecryptedFile( (LPTSTR)m_strFilename.c_str(), (LPTSTR)m_strTempFilename.c_str() );

	m_dwDesiredAccess = dwDesiredAccess;

	m_hTempFile = ::CreateFileW( m_strTempFilename.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile );

	SXTraceInfo(MODULE, L"CreateFileW(%s), Handle[%x]", m_strTempFilename.c_str(), m_hTempFile );

	if (nError != STAT_SUCCESS || INVALID_HANDLE_VALUE == m_hTempFile)
	{
		if( INVALID_HANDLE_VALUE != m_hTempFile )
		{
			CloseHandle(m_hTempFile);
		}
		//TFS: 62486 - 파워포인트 2013 사용도중 네트워크 드라이브 베타제어가 해제 되는 증상
		//간헐적으로 ~$ 템프파일의 핸들이 풀리지 않은 상태로 복호화 함수가 호출되어 실패하는 현상이 발생함.(오피스에서 핸들을 물고 있는 상태)
		//기존 템프파일을 m_strTempDelFilename 에 백업하고 새로 템프파일을 생성하여 복호화 로직을 수행하도록 로직 수정.
		LPWSTR szExt = PathFindExtensionW( m_strFilename.c_str() );

		if( TRUE == PathFileExistsW(m_strTempFilename.c_str())
			&& (szExt != NULL && lstrcmpi( L".mpp", szExt ) != 0 )
			)
		{
			HANDLE hTemp = ::CreateFileW( m_strTempFilename.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE , NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );
			if( INVALID_HANDLE_VALUE == hTemp )
			{
				int nErr = GetLastError();
				if( ERROR_ACCESS_DENIED == nErr || ERROR_SHARING_VIOLATION == nErr )
				{
					WCHAR szTempPath[1024] = {0,};
					WCHAR szTempFilePath[1024] = {0,};

					// 임시 파일명 구함
					::GetTempPathW( 1024, szTempPath );
					if( 0 != ::GetTempFileNameW( szTempPath, L"~$" , 0, szTempFilePath ) )
					{
						DeleteFileW( szTempFilePath );

						m_strTempDelFilename = m_strTempFilename;
						m_strTempFilename = szTempFilePath;
					}
				}
			}
			else
			{
				::CloseHandle(hTemp);
			}
		}

		nError = g_codecMgr.MakeDecryptedFile((LPTSTR)m_strFilename.c_str(), (LPTSTR)m_strTempFilename.c_str() );
		m_hTempFile = ::CreateFileW( m_strTempFilename.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile );
	}

	if ( nError != STAT_SUCCESS )
	{
		if ( STAT_CANNOT_AUTHENTICATE == nError )
		{
			::SetLastError( ERROR_ACCESS_DENIED );
			return INVALID_HANDLE_VALUE;
		}

		m_hTempFile = ::CreateFileW( m_strTempFilename.c_str(), dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile );

		if( INVALID_HANDLE_VALUE != m_hTempFile )
		{
			if( 0 == ::GetFileSize(m_hTempFile, NULL) )
			{
				::CloseHandle(m_hTempFile);
				WipeOutFileEX( (LPWSTR)m_strTempFilename.c_str() );
				m_hTempFile = INVALID_HANDLE_VALUE;
			}
		}

		return m_hTempFile;
		
	}
	// [TFS:34720] modified by jhkwon - end

	// [TFS:34720] added by jhkwon - start
	// : MS-Office 2010 속도 개선 작업 .. 
	if ( TRUE == SCAPI_IDSIPCPolicy()->IsUseFileCache() )
	{
		if ( NULL != pCache )
		{
			if ( DT_SDS_SECURITY == SCAPI_IDSFileMgr()->IsEncryptedFile( m_strFilename.c_str() ) )			
			{
				pCache->SetAfterEncrypted( FALSE );
			}
		}
	}
	// [TFS:34720] added by jhkwon - end

	return m_hTempFile;
}

DSTempFileMgr::DSTempFileMgr()
{

}

DSTempFileMgr::~DSTempFileMgr()
{
	// temp 파일 버퍼 해제
	DSTempFile* pTempFile = NULL;

	for ( vector<DSTempFile*>::iterator j = m_TempFilePtrList.begin(); j != m_TempFilePtrList.end(); j++ )
	{
		pTempFile = static_cast<DSTempFile*>(*j);

		if ( pTempFile != NULL )
		{
			WipeOutFileEX( (LPWSTR)pTempFile->GetTempFileName() );
			delete pTempFile;
			pTempFile = NULL;
		}
	}

	m_TempFilePtrList.clear();

	for( vector<wstring>::iterator it = m_DelTempFilePtrList.begin(); it != m_DelTempFilePtrList.end(); it++)
	{
		WipeOutFileEX( (LPWSTR)(it->c_str()) );
	}

	m_DelTempFilePtrList.clear();
	
}

DSTempFileMgr* DSTempFileMgr::GetInst()
{
	static DSTempFileMgr Inst;
	return &Inst;
}

DSTempFile* DSTempFileMgr::GetTempFilePtr( HANDLE hFile )
{
	vector<DSTempFile*>::iterator i;

	if( m_TempFilePtrList.empty() )
		return NULL;

	for ( i = m_TempFilePtrList.begin(); i != m_TempFilePtrList.end(); i++ )
	{
		if( static_cast<DSTempFile*>(*i)->GetTempFileHandle() == hFile )
		{
			return *i;
		}
	}

	return NULL;
}

DSTempFile* DSTempFileMgr::GetTempFilePtr( LPCWSTR lpFileName )
{
	vector<DSTempFile*>::iterator i;

	for ( i = m_TempFilePtrList.begin(); i != m_TempFilePtrList.end(); i++ )
	{
		if ( lstrcmpiW( static_cast<DSTempFile*>(*i)->GetSrcFileName(), lpFileName ) == 0 )
		{
			return *i;
		}
	}

	return NULL;
}

DSTempFile* DSTempFileMgr::GetAlternationFilePtr( LPCWSTR lpFileName )
{
	vector<DSTempFile*>::iterator i;

	for ( i = m_TempFilePtrList.begin(); i != m_TempFilePtrList.end(); i++ )
	{
		if ( lstrcmpiW( static_cast<DSTempFile*>(*i)->GetTempFileName(), lpFileName ) == 0 )
		{
			return *i;
		}
	}

	return NULL;
}

void DSTempFileMgr::RemoveTempFile( HANDLE hFile )
{
	DSTempFile*	pTempFile = NULL;
	vector<DSTempFile*>::iterator i;

	for ( i = m_TempFilePtrList.begin(); i != m_TempFilePtrList.end(); i++ )
	{
		if ( static_cast<DSTempFile*>(*i)->GetTempFileHandle() == hFile )
		{
			pTempFile = *i;
			delete pTempFile;
			pTempFile = NULL;

			m_TempFilePtrList.erase( i );
			return;
		}
	}
}

HANDLE DSTempFileMgr::OpenOrCreateTempFile( LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile )
{
	DSTempFile* pTempFile = GetTempFilePtr( lpFileName );

	if( pTempFile == NULL )
	{
		return CreateTempFile( lpFileName , dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile );
	}

	return pTempFile->OpenTempFile( dwDesiredAccess, dwShareMode , lpSecurityAttributes, dwCreationDisposition ,dwFlagsAndAttributes, hTemplateFile );
}

HANDLE DSTempFileMgr::CreateTempFile( LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile )
{
	DSTempFile* pTempFile = new DSTempFile( lpFileName );
	AddTempFile(pTempFile);
	return pTempFile->CreateTempFile( dwDesiredAccess, dwShareMode , lpSecurityAttributes, dwCreationDisposition ,dwFlagsAndAttributes, hTemplateFile );
}

DSTempFile* DSTempFileMgr::NewTempFile( LPCWSTR lpFileName )
{
	DSTempFile* pTempFile = new DSTempFile( lpFileName );
	AddTempFile(pTempFile);
	return pTempFile;
}
// #include "BaseUtil.h"
#include "DSServiceMgr.h"
#include "DSDirLib.h"

DSServiceMgr::DSServiceMgr(void)
{
	m_bInitialize = FALSE;
	m_hMod = NULL;
	m_fpIsEncrytpedDSFile = NULL;
	m_fpRequestSecuDocDecription = NULL;
	m_fpOpenNormalPDFDocument = NULL;
	m_fpOnStartDoc = NULL;
	m_fpOnStartPage = NULL;
	m_fpOnEndPage = NULL;
	m_fpOnEndDoc = NULL;
	m_fpCheckCnpBlock = NULL;
}

DSServiceMgr::~DSServiceMgr(void)
{
}

void DSServiceMgr::Initialize()
{
	WCHAR szDllPath[MAX_PATH] = {0};
 	WCHAR szDSPath[MAX_PATH] = {0};
 	GetSDSDirectory( szDSPath, MAX_PATH );
	
	wsprintf(szDllPath,L"%s\\%s",szDSPath,DSMGR_MODULE_NAME);
	
	// load Service module
	m_hMod = LoadLibrary(szDllPath);
	if( NULL == m_hMod )
		return;
	
	// Set Service fuction pointer
	// SecuDoc
	m_fpIsEncrytpedDSFile = (FP_IsEncrytpedDSFile)GetProcAddress(m_hMod, "IsEncryptedDSFile");
	m_fpRequestSecuDocDecription = (FP_RequestSecuDocDecription)GetProcAddress(m_hMod, "RequestSecuDocDecription");
	m_fpOpenNormalPDFDocument = (FP_OpenNormalPDFDocument)GetProcAddress(m_hMod, "OpenNormalPDFDocument");

	// PrintMarker
	m_fpInitPrintMarkerModule = (FP_InitPrintMarkerModule)GetProcAddress(m_hMod, "InitPrintMarkerModule");
	m_fpOnStartDoc = (FP_OnStartDoc)GetProcAddress(m_hMod, "OnStartDoc");
	m_fpOnStartPage = (FP_OnStartPage)GetProcAddress(m_hMod, "OnStartPage");
	m_fpOnEndPage = (FP_OnEndPage)GetProcAddress(m_hMod, "OnEndPage");
	m_fpOnEndDoc = (FP_OnEndDoc)GetProcAddress(m_hMod, "OnEndDoc");

	// Cnp Ctrl
	m_fpCheckCnpBlock = (FP_CheckCnpBlock)GetProcAddress(m_hMod, "CheckCnpBlock");

	// PrinterDriver
	m_fpCheckPrintDirverW = (FP_CheckPrintDriverW)GetProcAddress(m_hMod, "CheckPrintDriverW");

	if( NULL == m_fpIsEncrytpedDSFile || NULL == m_fpRequestSecuDocDecription || NULL == m_fpOpenNormalPDFDocument ||
		NULL == m_fpOnStartDoc || NULL == m_fpOnStartPage || NULL == m_fpOnEndPage || NULL == m_fpOnEndDoc ||
		NULL == m_fpCheckCnpBlock || NULL == m_fpCheckPrintDirverW )
	{
		OutputDebugStringW(_T("DSServiceMgr::Initialize() Error!!"));
		return;
	}

	m_bInitialize = TRUE;
}

/*! \brief 암호화 문서를 복호화 요청하는 함수
 * \details 현재 보안문서를 DSServiceModule(PDFViewerDSMgr.dll)에게 복호화 요청하는 함수
  			복호화가 성공하면 복호화된 tmp file path가 lpTempFileName으로 리턴된다.

 * \param LPCWSTR lpFileName : 암호화된 문서 ( IN )
 * \param LPWSTR lpTempFileName : 복호화된 tmp 문서 ( OUT )
 * \param DWORD nTempFileNameSize : lpTempFileName 버퍼 사이즈
 * \return BOOL TRUE: 복호화 성공, FALSE: 복호화 실패
*/
BOOL DSServiceMgr::RequsetDcryption(LPCWSTR lpFileName, LPWSTR lpTempFileName, DWORD nTempFileNameSize)
{
	if( NULL == m_fpRequestSecuDocDecription )
		return FALSE;

	return m_fpRequestSecuDocDecription(lpFileName, lpTempFileName, nTempFileNameSize);
}

/*! \brief 일반 PDF 문서 오픈 함수
* \details	PDF Viewer 종료시에 일반 문서의 암호화 정책 적용을 위해 
			일반 문서가 오픈되었다고 통보하기 위해 호출하는 함수.

 * \param LPCWSTR lpFileName : 일반 문서 파일 명 ( IN )
 * \return BOOL TRUE: 성공, FALSE: 실패
*/
BOOL DSServiceMgr::OpenNormalPDFDocument(LPCWSTR lpFileName)
{
	if( NULL == m_fpOpenNormalPDFDocument )
		return FALSE;

	return m_fpOpenNormalPDFDocument(lpFileName);
}


/*! \brief 암호화된 파일인지 여부를 체크하는 함수
 * \details DSServiceModule(PDFViewerDSMgr.dll)에게 암호화된 문서인지 체크 요청하는 함수

 * \param LPCWSTR lpFileName : 체크할 문서 
 * \return BOOL TRUE: 암호화 문서, FALSE: 일반 문서
*/
BOOL DSServiceMgr::IsEncryptedDSFile(LPCWSTR lpFileName)
{
	if( NULL == m_fpIsEncrytpedDSFile )
		return FALSE;

	return m_fpIsEncrytpedDSFile(lpFileName);
}

void DSServiceMgr::InitPrintMarkerModule()
{
	if( NULL == m_fpInitPrintMarkerModule)
		return;

	m_fpInitPrintMarkerModule();
}

int DSServiceMgr::OnStartDoc(HDC hDC, DOCINFOW *docinfo, LPCWSTR lpFileName)
{
	if( NULL == m_fpOnStartDoc )
		return FALSE;

	return m_fpOnStartDoc(hDC,docinfo,lpFileName);
}

int DSServiceMgr::OnStartPage( HDC hDC )
{
	if( NULL == m_fpOnStartPage )
		return FALSE;

	return m_fpOnStartPage(hDC);
}

int DSServiceMgr::OnEndPage( HDC hDC )
{
	if( NULL == m_fpOnEndPage )
		return FALSE;

	return m_fpOnEndPage(hDC);
}

int DSServiceMgr::OnEndDoc(HDC hDC)
{
	if( NULL == m_fpOnEndDoc )
		return FALSE;

	return m_fpOnEndDoc(hDC);
}

/*! \brief 복사 & 붙여넣기 방지 기능 체크 함수
 * \details lpFilePath가 보안문서이면 복사 붙여넣기 방지 정책에 
			Add[ processID, threadID, handle을 등록]하여 복사방지기능이 동작하도록 하고 
			일반 문서일 경우 정책에서 remove하여 복사 붙여넣기 기능을 허용한다.

 * \param LPCWSTR lpFileName : 체크할 문서 
 * \param DWORD processID : 복사 붙여넣기 정책에 등록할 프로세서 ID
 * \param DWORD threadID : 복사 붙여넣기 정책에 등록할 스레드 ID
 * \param HWND handle : 복사 붙여넣기 정책에 등록할 핸들
 * \return none
*/
void DSServiceMgr::CheckCnpBlock(LPCWSTR lpFilePath, DWORD processID, DWORD threadID , HWND handle)
{
	if( NULL == m_fpCheckCnpBlock )
		return;

	m_fpCheckCnpBlock(lpFilePath, processID, threadID, handle);
}

BOOL DSServiceMgr::IsServiceAvailable()
{
	return m_bInitialize;
}

DSServiceMgr* DSServiceMgr::GetInst()
{
	static DSServiceMgr s_DSSvrMgrkInst;
	return &s_DSSvrMgrkInst;
}

BOOL DSServiceMgr::CheckPrintDriverW(LPCWSTR lpszDriverName)
{
	if( NULL == m_fpCheckPrintDirverW ||
		NULL == lpszDriverName ||
		0 == wcslen(lpszDriverName) )
	{
		return FALSE;
	}

	return m_fpCheckPrintDirverW(lpszDriverName);
}


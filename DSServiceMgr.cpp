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

/*! \brief ��ȣȭ ������ ��ȣȭ ��û�ϴ� �Լ�
 * \details ���� ���ȹ����� DSServiceModule(PDFViewerDSMgr.dll)���� ��ȣȭ ��û�ϴ� �Լ�
  			��ȣȭ�� �����ϸ� ��ȣȭ�� tmp file path�� lpTempFileName���� ���ϵȴ�.

 * \param LPCWSTR lpFileName : ��ȣȭ�� ���� ( IN )
 * \param LPWSTR lpTempFileName : ��ȣȭ�� tmp ���� ( OUT )
 * \param DWORD nTempFileNameSize : lpTempFileName ���� ������
 * \return BOOL TRUE: ��ȣȭ ����, FALSE: ��ȣȭ ����
*/
BOOL DSServiceMgr::RequsetDcryption(LPCWSTR lpFileName, LPWSTR lpTempFileName, DWORD nTempFileNameSize)
{
	if( NULL == m_fpRequestSecuDocDecription )
		return FALSE;

	return m_fpRequestSecuDocDecription(lpFileName, lpTempFileName, nTempFileNameSize);
}

/*! \brief �Ϲ� PDF ���� ���� �Լ�
* \details	PDF Viewer ����ÿ� �Ϲ� ������ ��ȣȭ ��å ������ ���� 
			�Ϲ� ������ ���µǾ��ٰ� �뺸�ϱ� ���� ȣ���ϴ� �Լ�.

 * \param LPCWSTR lpFileName : �Ϲ� ���� ���� �� ( IN )
 * \return BOOL TRUE: ����, FALSE: ����
*/
BOOL DSServiceMgr::OpenNormalPDFDocument(LPCWSTR lpFileName)
{
	if( NULL == m_fpOpenNormalPDFDocument )
		return FALSE;

	return m_fpOpenNormalPDFDocument(lpFileName);
}


/*! \brief ��ȣȭ�� �������� ���θ� üũ�ϴ� �Լ�
 * \details DSServiceModule(PDFViewerDSMgr.dll)���� ��ȣȭ�� �������� üũ ��û�ϴ� �Լ�

 * \param LPCWSTR lpFileName : üũ�� ���� 
 * \return BOOL TRUE: ��ȣȭ ����, FALSE: �Ϲ� ����
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

/*! \brief ���� & �ٿ��ֱ� ���� ��� üũ �Լ�
 * \details lpFilePath�� ���ȹ����̸� ���� �ٿ��ֱ� ���� ��å�� 
			Add[ processID, threadID, handle�� ���]�Ͽ� ������������ �����ϵ��� �ϰ� 
			�Ϲ� ������ ��� ��å���� remove�Ͽ� ���� �ٿ��ֱ� ����� ����Ѵ�.

 * \param LPCWSTR lpFileName : üũ�� ���� 
 * \param DWORD processID : ���� �ٿ��ֱ� ��å�� ����� ���μ��� ID
 * \param DWORD threadID : ���� �ٿ��ֱ� ��å�� ����� ������ ID
 * \param HWND handle : ���� �ٿ��ֱ� ��å�� ����� �ڵ�
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


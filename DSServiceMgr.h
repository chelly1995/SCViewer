#pragma once

#include <windows.h>
#define DSMGR_MODULE_NAME L"PDFViewerDSMgr.dll"

typedef bool (*FP_IsEncrytpedDSFile)(LPCWSTR);
typedef bool (*FP_RequestSecuDocDecription)(LPCWSTR, LPWSTR, DWORD);
typedef bool (*FP_OpenNormalPDFDocument)(LPCWSTR);
typedef void (*FP_InitPrintMarkerModule)();
typedef int (*FP_OnStartDoc)(HDC hDC, DOCINFOW *docinfo, LPCWSTR lpFileName);
typedef int (*FP_OnStartPage)( HDC hDC );
typedef int (*FP_OnEndPage)( HDC hDC );
typedef int (*FP_OnEndDoc)(HDC hDC);
typedef void (*FP_CheckCnpBlock)(LPCWSTR lpSecuFilePath, DWORD processID, DWORD threadID , HWND handle);
typedef BOOL (*FP_CheckPrintDriverW)(LPCWSTR lpszDriverName);

class DSServiceMgr
{
public:
	static DSServiceMgr*	GetInst();
	void Initialize();
	BOOL IsServiceAvailable();
	// Secu Doc
	BOOL RequsetDcryption(LPCWSTR lpFileName, LPWSTR lpTempFileName, DWORD);
	BOOL OpenNormalPDFDocument(LPCWSTR lpFileName);
	BOOL IsEncryptedDSFile(LPCWSTR lpFileName);	
	// Printer Marker
	void InitPrintMarkerModule();
	int OnStartDoc(HDC hDC, DOCINFOW *docinfo, LPCWSTR lpFileName);
	int OnStartPage( HDC hDC );
	int OnEndPage( HDC hDC );
	int OnEndDoc(HDC hDC);
	// Cnp Ctrl
	void CheckCnpBlock(LPCWSTR lpFilePath, DWORD processID, DWORD threadID , HWND handle);

	BOOL CheckPrintDriverW(LPCWSTR lpszDriverName);

private:
	DSServiceMgr(void);
	~DSServiceMgr(void);

private:
	BOOL m_bInitialize;
	HMODULE	m_hMod;
	FP_IsEncrytpedDSFile		m_fpIsEncrytpedDSFile;
	FP_RequestSecuDocDecription m_fpRequestSecuDocDecription;
	FP_OpenNormalPDFDocument	m_fpOpenNormalPDFDocument;
	FP_InitPrintMarkerModule	m_fpInitPrintMarkerModule;
	FP_OnStartDoc	m_fpOnStartDoc;
	FP_OnStartPage	m_fpOnStartPage;
	FP_OnEndPage	m_fpOnEndPage;
	FP_OnEndDoc		m_fpOnEndDoc;
	FP_CheckCnpBlock m_fpCheckCnpBlock;
	FP_CheckPrintDriverW		m_fpCheckPrintDirverW;
};

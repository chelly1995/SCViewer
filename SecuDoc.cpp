#include "framework.h"
#include "stdafx.h"
#include "SC_Error.h"
#include "SecuDoc.h"
#include "DSEDDefine.h"
#include "IDSEDCommon.h"
#include "DSClntDefine.h"
#include "CSHMFileInfo.h"



int CSecuDocUtil::DecryptTempFile(LPCWSTR lpSrcFileName, LPWSTR lpDstFileName, DWORD nDstFileName)
{
	if (NULL == lpSrcFileName || NULL == lpDstFileName)
		return SC_ERROR_INVALID_ARGUMENT;
	
	HWND hActiveWindow = ::GetActiveWindow();
	if (NULL != hActiveWindow)
	{
		WCHAR szFilePath[MAX_PATH] = { 0, };
		wcscpy_s(szFilePath, MAX_PATH, lpSrcFileName);

		SCAPI_IDSFileMgr()->SetDocumentFile(hActiveWindow, szFilePath, DT_SDS_SECURITY);
		SCAPI_IDSFileMgr()->SetActivatedDocumentFile(szFilePath, hActiveWindow);
	}

	// create tmp file & decrypt
	HANDLE hResult = SCAPI_IDSTempFileMgr()->OpenOrCreateTempFile(
		lpSrcFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == hResult)
	{
		SXTraceErr(MODULE, L"ERROR! OpenOrCreateTempFile return INVALID_HANDLE_VALUE");
		return SC_ERROR_FAIL;
	}

	// return tmp file path
	IDSTempFile* pTempFile = SCAPI_IDSTempFileMgr()->GetTempFilePtr(lpSrcFileName);
	if (NULL == pTempFile)
	{
		SXTraceErr(MODULE, L"ERROR! GetTempFilePtr return NULL");
		return SC_ERROR_FAIL;
	}

	wcscpy_s(lpDstFileName, nDstFileName, pTempFile->GetDecryptedTmpFileName());

	CSHMFileInfo SHMFileInfo;
	wchar_t* pFileName = PathFindFileName(lpDstFileName);
	SHMFileInfo.CreateSHMSCView(pFileName);
	SHMFileInfo.WriteSHMSCView(const_cast<wchar_t*>(lpSrcFileName));

	return SC_ERROR_SUCCESS;
}

int CSecuDocUtil::SetNormalDocFile(LPCWSTR lpSrcFilePath, int nSrcFilePath)
{
	if (NULL == lpSrcFilePath)
		return SC_ERROR_INVALID_ARGUMENT;


	SCAPI_IDSIPCPolicy()->NormalDocOpen(lpSrcFilePath);

	HWND hActiveWindow = ::GetActiveWindow();
	if (NULL != hActiveWindow)
	{
		SCAPI_IDSFileMgr()->SetDocumentFile(hActiveWindow, const_cast<wchar_t*>(lpSrcFilePath), DT_NORMAL);
		SCAPI_IDSFileMgr()->SetActivatedDocumentFile(lpSrcFilePath, hActiveWindow);
		
		return SC_ERROR_SUCCESS;
	}

	return SC_ERROR_FAIL;
}

int CSecuDocUtil::GetDocType(LPCWSTR lpFilePath, int nFilePath)
{
	if (nullptr == lpFilePath)
		return SC_ERROR_INVALID_PARAMETER;

	return SCAPI_IDSFileMgr()->IsEncryptedFile(lpFilePath);
}

int CSecuDocUtil::GetSecuDocAcl(LPCWSTR szSecuFile, DSCSSDK_ACL eCheckRight, OUT bool* bAuth)
{
	if (NULL == szSecuFile)
		return SC_ERROR_INPUT_PARAM_NULL;

	// zPipe 통신을 위해 SDSMan agent 체크
	HWND hWnd = SCAPI_ISCCommon()->GetSDSManWindowHandle();

	if (NULL == hWnd)
		return SC_ERROR_AGENT_LOAD_FAIL;

	if (SDS_LOGIN_STATUS_LOGOUT == SCAPI_ISCCommon()->CheckDSAgentLogin())
		return SC_ERROR_LOGOUT;

	WCHAR szFilePath[MAX_PATH] = { 0, };
	wcscpy_s(szFilePath, MAX_PATH, szSecuFile);

	SCPipeMsg msg(SDSMAN_REQUEST_SECUFILE_AUTH);

	// 요청하는 권한값을 sdsman에서 사용하는 define값으로 변경
	int nCheckRight = ConvertToSDSManACL(eCheckRight);

	msg << nCheckRight;
	msg << (int)(sizeof(szFilePath) / sizeof(WCHAR));
	msg.WriteRowData((BYTE*)szFilePath, sizeof(szFilePath));

	SCPipeClient scPipe("PIPE_SDSMAN_MSG");

	BOOL bSendMsgRet = scPipe.Send(msg);
	int nMsgRet = 0;
	if (bSendMsgRet == TRUE)
	{
		BOOL bHaveAuth = FALSE;
		msg >> bHaveAuth;
		msg >> nMsgRet;

		if (SC_ERROR_SUCCESS == nMsgRet)
		{
			if (TRUE == bHaveAuth)
			{
				*bAuth = true;
				return SC_ERROR_SUCCESS;
			}
			else
			{
				*bAuth = false;
				return SC_ERROR_CANNOT_AUTHENTICATE;
			}
		}
	}
	
	return SC_ERROR_SCPIPE_DATA_SEND_FAIL;
}


int CSecuDocUtil::ConvertToSDSManACL(DSCSSDK_ACL eRight)
{
	int nRet = 0;
	switch (eRight)
	{
	case DSCSSDK_ACL::ACL_FILE_READ_RIGHT:				nRet = _Q_AUTH_READ_FILE;			break;
	case DSCSSDK_ACL::ACL_DECRYPT_FILE_RIGHT:			nRet = _Q_AUTH_DEC_FILE;			break;
	case DSCSSDK_ACL::ACL_FILE_EDIT_RIGHT:				nRet = _Q_AUTH_EDIT_FILE;			break;
	case DSCSSDK_ACL::ACL_FILE_CHANGE_ACCESS_RIGHT:		nRet = _Q_AUTH_CHANGE_ACCESS_MAN;	break;
	case DSCSSDK_ACL::ACL_FILE_CARRYOUT_RIGHT:			nRet = _Q_AHTU_MAKE_SOM_FILE;		break;
	case DSCSSDK_ACL::ACL_FILE_PRINT_RIGHT:				nRet = _Q_AUTH_PRINT;				break;
	case DSCSSDK_ACL::ACL_FILE_PRINT_MARK_RIGHT:		nRet = _Q_AUTH_PRINT_MARK;			break;
	default:
		break;
	}

	return nRet;
}

// IDSEDCommon.h -> CheckDSAgentLogin() 로 변경

/*
* 
int CSecuDocUtil::GetDSLoginStatus()
{
	HWND hWnd = GetSDSManWindowHandle();

	if (!hWnd)
		return SC_ERROR_NOT_EXIST;

	int nLoginStatus = (int)::SendMessage(hWnd, MSG_SFH_GETLOGINMODE, NULL, NULL);

	return nLoginStatus;
}
*/


// IDSEDCommon.h -> GetSDSManWindowHandle() 로 변경

/*
HWND CSecuDocUtil::GetSDSManWindowHandle()
{
	HWND hwndSDSMan = NULL;
	HANDLE hmapFileName = OpenFileMapping(FILE_MAP_READ, FALSE, _T("_SHM_SDSWND_HANDLE02"));

	if (hmapFileName != NULL)
	{
		LPVOID  pData = MapViewOfFile(hmapFileName, FILE_MAP_READ, 0, 0, 0);
		if (pData != NULL)
		{
			memcpy(&hwndSDSMan, pData, 4);
			UnmapViewOfFile(pData);
			pData = NULL;
		}

		CloseHandle(hmapFileName);
	}

	if (!hwndSDSMan)
		hwndSDSMan = ::FindWindow(NULL, _T("SDSMan"));

	return hwndSDSMan;
}
*/
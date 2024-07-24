#include "framework.h"
#include "stdafx.h"
#include "SC_Error.h"
#include "SecuDoc.h"
#include "DSEDDefine.h"
#include "IDSEDCommon.h"
#include "DSClntDefine.h"
#include "CSHMFileInfo.h"
#include "DSInfo.h"
#include "SCUser.h"
#include "SCDSHdrInfo.h"


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

	LPCWSTR pTempFileName = pTempFile->GetTempFileName();
	if (nullptr == pTempFileName)
		return SC_ERROR_FILE_NOT_EXIST;

	wcscpy_s(lpDstFileName, nDstFileName, pTempFile->GetTempFileName());

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

	SCAPI::HdrInfo::CDAC info;
	if (SC_ERROR_SUCCESS != DS_GetCurrUserAuthInfoToSecuFile(const_cast<wchar_t*>(szSecuFile), &info))
		return SC_ERROR_NOT_FOUND;

	char szAcl[5] = { 0, };
	switch (eCheckRight)
	{
	case DSCSSDK_ACL::ACL_NOTHING:
		break;
	case DSCSSDK_ACL::ACL_DECRYPT_FILE_RIGHT:
	{
		info.GetACL_Decrypt(szAcl);
		if (szAcl[0] == '1')
			return SC_ERROR_SUCCESS;
	}
	break;
	case DSCSSDK_ACL::ACL_FILE_READ_RIGHT:
	{
		info.GetACL_Read(szAcl);
		if (szAcl[0] == '1')
			return SC_ERROR_SUCCESS;
	}
	break;
	case DSCSSDK_ACL::ACL_FILE_EDIT_RIGHT:
	{
		info.GetACL_Edit(szAcl);
		if (szAcl[0] == '1')
			return SC_ERROR_SUCCESS;
	}
	break;
	case DSCSSDK_ACL::ACL_FILE_CHANGE_ACCESS_RIGHT:
	{
		info.GetACL_ChangeAuth(szAcl);
		if (szAcl[0] == '1')
			return SC_ERROR_SUCCESS;
	}
	break;
	case DSCSSDK_ACL::ACL_FILE_CARRYOUT_RIGHT:
	{
		info.GetACL_SOM(szAcl);
		if (szAcl[0] == '1')
			return SC_ERROR_SUCCESS;
	}
	break;
	case DSCSSDK_ACL::ACL_FILE_PRINT_RIGHT:
	{
		info.GetACL_Print(szAcl);
		if (szAcl[0] == '1')
			return SC_ERROR_SUCCESS;
	}
	break;
	case DSCSSDK_ACL::ACL_FILE_PRINT_MARK_RIGHT:
	{
		info.GetACL_PrintMarking(szAcl);
		if (szAcl[0] == '1')
			return SC_ERROR_SUCCESS;
	}
	break;
	default:
		break;
	}
	
	return SC_ERROR_NOT_FOUND;
}
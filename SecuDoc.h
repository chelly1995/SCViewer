#pragma once
#define SDS_LOGIN_STATUS_LOGOUT				0
#define SDS_LOGIN_STATUS_ONLINE				1
#define SDS_LOGIN_STATUS_OFFLINELOGIN		2

class CSecuDocUtil
{
public:
	CSecuDocUtil() = default;
	~CSecuDocUtil() = default;
	
	int DecryptTempFile(LPCWSTR lpSrcFileName, LPWSTR lpDstFileName, DWORD nDstFileName);
	int SetNormalDocFile(LPCWSTR lpFilePath, int nFilePath);
	int GetDocType(LPCWSTR lpFilePath, int nFilePath);
	int GetSecuDocAcl(LPCWSTR lpFilePath, IN DSCSSDK_ACL eCheckRight, OUT bool *bAuth);

private:
	int ConvertToSDSManACL(DSCSSDK_ACL eRight);
	//EDCommon* edcommon = nullptr;
//	int GetDSLoginStatus();
//	HWND GetSDSManWindowHandle();
};

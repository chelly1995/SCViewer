#pragma once
#define SDS_LOGIN_STATUS_LOGOUT				0
#define SDS_LOGIN_STATUS_ONLINE				1
#define SDS_LOGIN_STATUS_OFFLINELOGIN		2

class CSecuDocUtil
{
public:
	CSecuDocUtil() = default;
	~CSecuDocUtil() = default;
	
	/*!
	* \brief 
	* \details 
	* \param[in,out] lpSrcFileName
	* \param[in,out] lpDstFileName
	* \param[in,out] nDstFileName
	* \return int 
	*/
	int DecryptTempFile(LPCWSTR lpSrcFileName, LPWSTR lpDstFileName, DWORD nDstFileName);

	/*!
	* \brief 
	* \details 
	* \param[in,out] lpFilePath
	* \param[in,out] nFilePath
	* \return int 
	*/
	int SetNormalDocFile(LPCWSTR lpFilePath, int nFilePath);

	/*!
	* \brief 
	* \details 
	* \param[in,out] lpFilePath
	* \param[in,out] nFilePath
	* \return int 
	*/
	int GetDocType(LPCWSTR lpFilePath, int nFilePath);

	/*!
	* \brief 
	* \details 
	* \param[in,out] lpFilePath
	* \param[in,out] eCheckRight
	* \param[in,out] bAuth
	* \return int 
	*/
	int GetSecuDocAcl(LPCWSTR lpFilePath, IN DSCSSDK_ACL eCheckRight, OUT bool *bAuth);
};

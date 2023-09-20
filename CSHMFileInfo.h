#pragma once
#include "framework.h"
/*!
 * \class	CSHMFileInfo SCViewer.exe 와 Hook 모듈간 원본명을 교환하기 위한 공유 메모리
 * \brief	Hook모듈은 SCViewer.exe 하위 또는 동일 depth에 실행되는 msedgewebview2.exe 에 인젝션됨
 *			Hook모듈은 임시파일명으로 원본 파일명을 조회할 수 있도록 공유 메모리 정보 사용
*/
class CSHMFileInfo
{
public:
	CSHMFileInfo() = default;
	~CSHMFileInfo() = default;

public:
	/*!
	* \brief 
	* \details 
	* \param[in,out] pTempFileName
	* \return int 
	*/
	int CreateSHMSCView(wchar_t* pTempFileName);

	/*!
	* \brief 
	* \details 
	* \param[in,out] pOriFileName
	* \return int 
	*/
	int WriteSHMSCView(wchar_t* pOriFileName);
	
	/*!
	* \brief 
	* \details 
	* \param[in,out] pKey
	* \param[in,out] pValue
	* \return int 
	*/
	int ReadSHMSCView(wchar_t* pKey, wchar_t* pValue);

private:
	wchar_t m_szKey[MAX_PATH] = { 0, };
};
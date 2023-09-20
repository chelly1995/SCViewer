#pragma once
#include "framework.h"
/*!
 * \class	CSHMFileInfo SCViewer.exe �� Hook ��Ⱓ �������� ��ȯ�ϱ� ���� ���� �޸�
 * \brief	Hook����� SCViewer.exe ���� �Ǵ� ���� depth�� ����Ǵ� msedgewebview2.exe �� �����ǵ�
 *			Hook����� �ӽ����ϸ����� ���� ���ϸ��� ��ȸ�� �� �ֵ��� ���� �޸� ���� ���
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
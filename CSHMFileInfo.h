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
	int CreateSHMSCView(wchar_t* pTempFileName);
	int WriteSHMSCView(wchar_t* pOriFileName);
	int ReadSHMSCView(wchar_t* pKey, wchar_t* pValue);

private:
	wchar_t m_szKey[MAX_PATH] = { 0, };
};
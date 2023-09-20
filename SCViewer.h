#pragma once
#include "framework.h"
#include "resource.h"
#include "SecuDoc.h"

class CSCViewer
{
public:
	CSCViewer() = default;
	~CSCViewer() = default;

	/*!
	* \brief Microsoft Edge WebView2 ��Ÿ�� ��ġ üũ
	* \details ���� ��� ���� �ش� ������Ʈ�� ��ġ���� ���� ��� ���� ����
	* \return int 
	*/
	int CheckWebView2Installed();

	/*!
	* \brief ������ �ڵ� ����
	* \details �ش� �ڵ�� ������ Ÿ��Ʋ�� ����, �Է�Ű ���ܿ� ����
	* \param[in] hwnd
	* \return void 
	*/
	void SetHwnd(HWND hwnd) { m_hMain = hwnd; };
	
	/*!
	* \brief Ÿ��Ʋ�� ����
	* \details ������ ���� ��ü ��θ����� ������ Ÿ��Ʋ ������Ʈ
	* \return int 
	*/
	int UpdateTtitlebar();

	/*!
	* \brief SCViewer OpenFileDialog ����
	* \details IDM_FILEOPEN �޽��� ó�� 
	* \param[in] hwnd
	* \return int 
	*/
	int SCVOpenFileDlg(HWND hwnd);
	
	/*!
	* \brief ���� ���ϸ� ����
	* \details �Ϲ�/���ȹ��� ���� �� ���� ���ϸ� ����. �ش� ���ϸ����� ���ȹ��� ����üũ � ���
	* \param[in] pFilePath ���� ���� ���
	* \param[in] nFilePath ���� ���� ��� ����
	* \return void 
	*/
	void SetOpenFilePath(LPCWSTR pFilePath, int nFilePath);
	
	/*!
	* \brief ���� ���ϸ� ����
	* \details 
	* \return LPWSTR 
	*/
	LPWSTR GetOpenFilePath(){return m_szOpendFilePath; };
	
	/*!
	* \brief ���� ���� ���� �Լ�
	* \details 
	* \param[in] sender drag&drop �̺�Ʈ���� ȣ�� �� ���
	* \return int 
	*/
	int SCVFileOpen(ICoreWebView2* sender = nullptr);

	
private:
	/*!
	* \brief �б� ���� �޼��� ���
	* \details �б� ������ ���� ���ȹ��� ���� �� �������� �б���� �޽��� ��� 
	* \return void 
	*/
	void ShowDeniedReadMsg();
	
	/*!
	* \brief ���� �������� ������ ���� ��ȸ
	* \details SetOpenFilePath()�Լ��� ���� ��ϵ� ������ ���ȹ��� ����üũ
	*		   SDSMan �� Pipe������� �Է¹��� DSCSSDK_ACL �׸��� ������ ��ȸ��
	* \param[in] nRight
	* \return BOOL TRUE: ��������, FALSE : ���� ����
	*/
	BOOL GetSecudocRight(DSCSSDK_ACL nRight);
	
	/*!
	* \brief �Ϲݹ��� ����
	* \details �Ϲݹ��� ������ �ش� �Լ��� ������ -> ����/�ٿ��ֱ� ����, �Ŵ������ SDSMan ���� �����ϴ� �Ϲݹ��� ��Ͽ� �߰�
	* \param[in] pFilePath ���� ���� ���
	* \param[in] nFilePath ���� ���� ��� ����
	* \return int SC_ERROR_SUCCESS ����, �׹ۿ� SC_Error.h ����
	*/
	int OpenNormalDoc(LPCWSTR pFilePath, int nFilePath);
	
	/*!
	* \brief ���ȹ��� ����
	* \details 
	* \param[in] pFilePath ���� ���� ���
	* \param[in] nFilePath ���� ���� ��� ����
	* \return int SC_ERROR_SUCCESS ����, �׹ۿ� SC_Error.h ����
	*/
	int OpenEncDoc(LPCWSTR pFilePath, int nFilePath);
	
	/*!
	* \brief ���� Ÿ��(�Ϲ�/����) ��ȸ
	* \details 
	* \param[in] pFilePath ��ȸ�� ���� ���
	* \param[in] nFilePath ��ȸ�� ���� ��� ����
	* \return int 0:�Ϲݹ��� ,1:���ȹ��� (cf. enum DOCUMENT_TYPE ����)
	*/
	int GetDocType(LPCWSTR pFilePath, int nFilePath);
	
	/*!
	* \brief ����Ȯ���� üũ
	* \details 
	* \param[in] pFilePath ��ȸ�� ���� ���
	* \param[in] nFilePath ��ȸ�� ���� ��� ����
	* \return int SC_ERROR_SUCCESS : ��� ����, �׹ۿ� SC_Error.h ����
	*/
	int IsSupportExtension(LPCWSTR pFilePath, int nFilePath);
	
	/*!
	* \brief SCViewer �Ŵ�/������ Show/Hide
	* \details �Ϲ�/���ȹ��� �� ���ȹ��� ���ѿ� ���� ����/��� ������ Show/Hide
	* \return int 
	*/
	int ModifyToolbar();

private:
	
	HWND			m_hMain = nullptr;
	wchar_t			m_szOpendFilePath[MAX_PATH] = { 0, };		//���� �Ǵ� ���� ���ϸ� ����
	wchar_t			m_szDecryptedFilePath[MAX_PATH] = { 0, };	//���� �Ǵ� ���ȹ����� ��ȣȭ �� �ӽ����ϸ� ����
	CSecuDocUtil	secudocUtil;								//�Ϻ�ȣȭ ���� Ŭ����
};
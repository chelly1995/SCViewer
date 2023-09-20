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
	* \brief Microsoft Edge WebView2 런타임 설치 체크
	* \details 웹뷰 기반 뷰어로 해당 업데이트가 설치되지 않은 경우 실행 종료
	* \return int 
	*/
	int CheckWebView2Installed();

	/*!
	* \brief 윈도우 핸들 저장
	* \details 해당 핸들로 윈도우 타이틀명 변경, 입력키 차단에 쓰임
	* \param[in] hwnd
	* \return void 
	*/
	void SetHwnd(HWND hwnd) { m_hMain = hwnd; };
	
	/*!
	* \brief 타이틀바 갱신
	* \details 열람된 파일 전체 경로명으로 윈도우 타이틀 업데이트
	* \return int 
	*/
	int UpdateTtitlebar();

	/*!
	* \brief SCViewer OpenFileDialog 제어
	* \details IDM_FILEOPEN 메시지 처리 
	* \param[in] hwnd
	* \return int 
	*/
	int SCVOpenFileDlg(HWND hwnd);
	
	/*!
	* \brief 열람 파일명 저장
	* \details 일반/보안문서 열람 시 원본 파일명 저장. 해당 파일명으로 보안문서 권한체크 등에 사용
	* \param[in] pFilePath 원본 파일 경로
	* \param[in] nFilePath 원본 파일 경로 길이
	* \return void 
	*/
	void SetOpenFilePath(LPCWSTR pFilePath, int nFilePath);
	
	/*!
	* \brief 열람 파일명 리턴
	* \details 
	* \return LPWSTR 
	*/
	LPWSTR GetOpenFilePath(){return m_szOpendFilePath; };
	
	/*!
	* \brief 파일 열람 메인 함수
	* \details 
	* \param[in] sender drag&drop 이벤트에서 호출 시 사용
	* \return int 
	*/
	int SCVFileOpen(ICoreWebView2* sender = nullptr);

	
private:
	/*!
	* \brief 읽기 실패 메세지 출력
	* \details 읽기 권한이 없는 보안문서 열람 시 문서보안 읽기실패 메시지 출력 
	* \return void 
	*/
	void ShowDeniedReadMsg();
	
	/*!
	* \brief 현재 열람중인 문서의 권한 조회
	* \details SetOpenFilePath()함수를 통해 등록된 문서의 보안문서 권한체크
	*		   SDSMan 에 Pipe통신으로 입력받은 DSCSSDK_ACL 항목의 권한을 조회함
	* \param[in] nRight
	* \return BOOL TRUE: 권한있음, FALSE : 권한 없음
	*/
	BOOL GetSecudocRight(DSCSSDK_ACL nRight);
	
	/*!
	* \brief 일반문서 열람
	* \details 일반문서 열람은 해당 함수로 열람함 -> 복사/붙여넣기 제어, 매뉴제어와 SDSMan 에서 관리하는 일반문서 목록에 추가
	* \param[in] pFilePath 열람 문서 경로
	* \param[in] nFilePath 열람 문서 경로 길이
	* \return int SC_ERROR_SUCCESS 성공, 그밖에 SC_Error.h 참고
	*/
	int OpenNormalDoc(LPCWSTR pFilePath, int nFilePath);
	
	/*!
	* \brief 보안문서 열람
	* \details 
	* \param[in] pFilePath 열람 문서 경로
	* \param[in] nFilePath 열람 문서 경로 길이
	* \return int SC_ERROR_SUCCESS 성공, 그밖에 SC_Error.h 참고
	*/
	int OpenEncDoc(LPCWSTR pFilePath, int nFilePath);
	
	/*!
	* \brief 문서 타입(일반/보안) 조회
	* \details 
	* \param[in] pFilePath 조회할 문서 경로
	* \param[in] nFilePath 조회할 문서 경로 길이
	* \return int 0:일반문서 ,1:보안문서 (cf. enum DOCUMENT_TYPE 참고)
	*/
	int GetDocType(LPCWSTR pFilePath, int nFilePath);
	
	/*!
	* \brief 지원확장자 체크
	* \details 
	* \param[in] pFilePath 조회할 문서 경로
	* \param[in] nFilePath 조회할 문서 경로 길이
	* \return int SC_ERROR_SUCCESS : 대상 문서, 그밖에 SC_Error.h 참고
	*/
	int IsSupportExtension(LPCWSTR pFilePath, int nFilePath);
	
	/*!
	* \brief SCViewer 매뉴/아이콘 Show/Hide
	* \details 일반/보안문서 및 보안문서 권한에 따라 저장/출력 아이콘 Show/Hide
	* \return int 
	*/
	int ModifyToolbar();

private:
	
	HWND			m_hMain = nullptr;
	wchar_t			m_szOpendFilePath[MAX_PATH] = { 0, };		//열람 되는 원본 파일명 저장
	wchar_t			m_szDecryptedFilePath[MAX_PATH] = { 0, };	//열람 되는 보안문서의 복호화 된 임시파일명 저장
	CSecuDocUtil	secudocUtil;								//암복호화 지원 클래스
};
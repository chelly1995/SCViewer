// SCViewer.cpp : 애플리케이션에 대한 진입점을 정의합니다.

#include <string>
#include <wrl.h>        
#include <wil/com.h>   
#include "WebView2.h"  
#include "WebView2EnvironmentOptions.h"
#include "framework.h"
#include "Resource.h"
#include "SCViewer.h"
#include "SecuDoc.h"
#include "SC_Error.h"
#include "SCMsgBoxMgr.h"
#include "dsLang.h"
#include "DSResStrDefine.h"
#include "ResourceCtrl.h"
#include "DSCSLinkerMgr.h"
#include "CSHMFileInfo.h"
#include "DragandDrop.h"
#include <Shlwapi.h>
#include "SCMsgBoxMgr.h"
#include "DSEDDefine.h"

static wil::com_ptr<ICoreWebView2Controller> webviewController; // WebView2 컨트롤을 제어하는데 사용되는 포인터
static wil::com_ptr<ICoreWebView2> webview; // 주로 WebView2 컨트롤이 로드한 웹 페이지와 상호작용 하는데 사용되는 포인터
static wil::com_ptr<ICoreWebView2Environment> webViewEnvironment;

// 전역 변수:
WCHAR g_szTitle[MAX_PATH] = { 0, };
WCHAR g_szWindowClass[MAX_PATH] = { 0, };
CSCViewer scviewer;

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, g_szTitle, MAX_PATH);
	LoadStringW(hInstance, IDC_SCVIEWER, g_szWindowClass, MAX_PATH);
	MyRegisterClass(hInstance);

	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	//실행 파라미터인 경로 앞뒤에 붙은 쌍따옴표 제거
	size_t length = wcslen(lpCmdLine);
	if (length >= 2 && lpCmdLine[0] == L'"' && lpCmdLine[length - 1] == L'"') 
	{
		for (size_t i = 0; i < length - 2; ++i) 
		{
			lpCmdLine[i] = lpCmdLine[i + 1];
		}

		lpCmdLine[length - 2] = L'\0';
	}

	scviewer.SetOpenFilePath(lpCmdLine, wcslen(lpCmdLine));

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SCVIEWER));

	MSG msg;
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SCPDFVIEWER));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_SCVIEWER);
	wcex.lpszClassName = g_szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SCPDFVIEWER));

	return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd = CreateWindowW(g_szWindowClass, g_szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// WEBVIEW 설치 체크
	if (SC_ERROR_SUCCESS != scviewer.CheckWebView2Installed())
	{
		CSCMsgBoxMgr::GetInst()->SCMessageBoxModaless(GetActiveWindow(), MDSC(STR_CHECK_WEBVIEW_INSTALL),
			MDSC(STR_BOXCAPTION_TITLE), MB_OK | MB_ICONINFORMATION, 5, STR_CHECK_WEBVIEW_INSTALL);

		ExitProcess(0);
		return FALSE;
	}

	PWSTR pszPath = nullptr;
	HRESULT hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &pszPath);

	std::wstring strAppPath;
	if (SUCCEEDED(hr))
	{
		strAppPath = pszPath;
		strAppPath.append(L"\\softcamp\\SCViewer");
		CoTaskMemFree(pszPath);
	}
	else
	{
		strAppPath = L"C:\\windows\\Temp";
	}

	CreateCoreWebView2EnvironmentWithOptions(nullptr, strAppPath.c_str(), nullptr,
		Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2EnvironmentCompletedHandler>(
			[hWnd](HRESULT result, ICoreWebView2Environment* env) -> HRESULT {

				env->CreateCoreWebView2Controller(hWnd, Microsoft::WRL::Callback<ICoreWebView2CreateCoreWebView2ControllerCompletedHandler>(
					[hWnd](HRESULT result, ICoreWebView2Controller* controller) -> HRESULT
					{
						if (controller != nullptr)
						{
							webviewController = controller;
							webviewController->get_CoreWebView2(&webview);   //ICoreWebView2Controller::get_CoreWebView2 메서드를 실행하여 연결된 WebView2 컨트롤을 가져옴
						}

						// 상위 창의 경계에 맞게 WebView 크기 조정 => 파일정보 보기버튼 가리기위해 +50px추가(파일정보 버튼으로 복호화된 원본 파일경로 보임)
						RECT bounds;
						GetClientRect(hWnd, &bounds);
						bounds.right += 50;
						webviewController->put_Bounds(bounds);

						wil::com_ptr<ICoreWebView2Settings> settings;
						webview->get_Settings(&settings);
						settings->put_IsScriptEnabled(TRUE);                 //WebView의 모든 향후 탐색에서 JavaScript 실행이 활성화되는지 여부를 제어
						settings->put_AreDefaultScriptDialogsEnabled(TRUE);  // 새 HTML 문서를 로드할 때 사용
						settings->put_IsWebMessageEnabled(TRUE);             // 새 HTML 문서를 로드할 때 사용
						settings->put_AreDevToolsEnabled(FALSE);             // 개발자 도구 비활성화
						settings->put_AreDefaultContextMenusEnabled(FALSE);  // 콘텍스트 메뉴 비활성화

						NewWindowRequestedEventHandler* eventHandler2 = new NewWindowRequestedEventHandler();
						webview->add_NewWindowRequested(eventHandler2, nullptr);

						scviewer.SetHwnd(hWnd);
						scviewer.SCVFileOpen();
						scviewer.UpdateTtitlebar();

						return S_OK;
					}).Get());

				return S_OK;
			}).Get());

	return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		HMENU hMenu = CreateMenu();
		HMENU mFile = CreatePopupMenu();

		AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)mFile, MDSC(STR_FILE));
		AppendMenu(mFile, MF_STRING, IDM_FILEOPEN, MDSC(STR_OPEN));
		AppendMenu(mFile, MF_STRING, IDM_EXIT, MDSC(STR_CLOSE));

		SetMenu(hWnd, hMenu);
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);

		switch (wmId)
		{

		case IDM_FILEOPEN:
		{
			scviewer.SCVOpenFileDlg(hWnd);
		}
		break;
		case IDM_EXIT:
		{
			DestroyWindow(hWnd);
		}
		break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case WM_SIZE:
	{
		if (webviewController)
		{
			RECT bounds;

			//문서정보 버튼 숨기기 위해서 50px이동
			GetClientRect(hWnd, &bounds);
			bounds.right += 50;
			webviewController->put_Bounds(bounds);
		}
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_HOTKEY:
	{
		return 0;
	}
	break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

int CSCViewer::CheckWebView2Installed()
{
	wil::unique_cotaskmem_string version_info;
	HRESULT hr = GetAvailableCoreWebView2BrowserVersionString(nullptr, &version_info);

	if (hr == S_OK && version_info != nullptr)
		return SC_ERROR_SUCCESS;

	return SC_ERROR_NOT_EXIST;

}
int CSCViewer::SCVFileOpen(ICoreWebView2* sender /*= nullptr*/)
{
	int nType = GetDocType(m_szOpendFilePath, wcslen(m_szOpendFilePath));
	
	switch (nType)
	{
	case DT_NORMAL:
	{
		if (SC_ERROR_SUCCESS == OpenNormalDoc(m_szOpendFilePath, wcslen(m_szOpendFilePath)) )
		{
			if (nullptr != sender)
				sender->Navigate(m_szOpendFilePath);
			else
				webview->Navigate(m_szOpendFilePath);
		}
	}
	break;
	case DT_SDS_SECURITY:
	{
		int nDecRet = OpenEncDoc(m_szOpendFilePath, wcslen(m_szOpendFilePath));
		ModifyToolbar();
		
		if (SC_ERROR_SUCCESS == nDecRet)
		{
			if (nullptr != sender)
				sender->Navigate(m_szDecryptedFilePath);
			else
				webview->Navigate(m_szDecryptedFilePath);
		}
		else
		{
			if (nullptr != sender)
				sender->Navigate(m_szOpendFilePath);
			else
				webview->Navigate(m_szOpendFilePath);
		}
	}
	break;
	}

	return SC_ERROR_SUCCESS;
}

int CSCViewer::SCVOpenFileDlg(HWND hwnd)
{
	if (nullptr == hwnd)
		return SC_ERROR_FAIL;

	m_hMain = hwnd;

	OPENFILENAME OFN = { 0, };
	WCHAR filePathName[MAX_PATH] = { 0, };
	WCHAR lpstrFile[MAX_PATH] = { 0, };
	static WCHAR filter[] = L"모든 파일\0*.*\0텍스트 파일\0*.txt\0fbx 파일\0*.fbx";

	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = m_hMain;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = _countof(filePathName);
	OFN.lpstrInitialDir = L".";

	if (TRUE == GetOpenFileNameW(&OFN))
	{
		int nCheckExtRet = 0;
		if (SC_ERROR_SUCCESS != (nCheckExtRet = IsSupportExtension(lpstrFile, wcslen(lpstrFile))) )
			return nCheckExtRet;

		int nType = GetDocType(lpstrFile, wcslen(lpstrFile));
		switch (nType)
		{
		case DT_NORMAL:
		{
			if (SC_ERROR_SUCCESS == OpenNormalDoc(lpstrFile, wcslen(lpstrFile)))
				webview->Navigate(lpstrFile);
		}
		break;
		case DT_SDS_SECURITY:
		{
			int nDecRet = OpenEncDoc(lpstrFile, wcslen(lpstrFile));

			if (SC_ERROR_SUCCESS == nDecRet)
				webview->Navigate(m_szDecryptedFilePath);
			else
				webview->Navigate(lpstrFile);
		}
		break;
		default:
			break;
		}
	}
	
	ModifyToolbar();
	UpdateTtitlebar();

	return SC_ERROR_SUCCESS;
}

int CSCViewer::UpdateTtitlebar()
{
	if (NULL == m_szOpendFilePath || NULL == m_hMain)
		return SC_ERROR_FAIL;

	SetWindowText(m_hMain, m_szOpendFilePath);

	return SC_ERROR_SUCCESS;
}

void CSCViewer::ShowDeniedReadMsg()
{
	CSCMsgBoxMgr::GetInst()->SCMessageBoxModaless(GetActiveWindow(), MDSC(STR_DONT_HAVE_READ_RIGHT),
		MDSC(STR_BOXCAPTION_TITLE), MB_OK | MB_ICONINFORMATION, 5, STR_DONT_HAVE_READ_RIGHT);
}

BOOL CSCViewer::GetSecudocRight(DSCSSDK_ACL nRight)
{
	bool bAuth = false;
	if (SC_ERROR_SUCCESS == secudocUtil.GetSecuDocAcl(m_szOpendFilePath, nRight, &bAuth))
	{
		if (true == bAuth)
			return TRUE;
	}
	
	return FALSE;
}

int CSCViewer::GetDocType(LPCWSTR pFilePath, int nFilePath)
{
	return secudocUtil.GetDocType(pFilePath, nFilePath);
}

int CSCViewer::IsSupportExtension(LPCWSTR pFilePath, int nFilePath)
{
	if (nullptr == pFilePath)
		return SC_ERROR_INVALID_ARGUMENT;

	LPTSTR szExt = PathFindExtension(pFilePath);

	if (nullptr == szExt )
		return SC_ERROR_NOT_FOUND;

	for (int i = 0; lstrcmpi(SUPPORT_EXTENSION[i], _T("END")) != 0; i++)
	{
		if (0 == lstrcmpi(SUPPORT_EXTENSION[i], szExt))
			return SC_ERROR_SUCCESS;
	}

	return SC_ERROR_NOT_SUPPORT;
}

int CSCViewer::ModifyToolbar()
{
	wil::com_ptr<ICoreWebView2Settings> settings;
	HRESULT hRet = webview->get_Settings(&settings);
	
	if (FAILED(hRet))
		return SC_ERROR_NOT_FOUND;

	wil::com_ptr<ICoreWebView2Settings7> m_settings7;
	m_settings7 = settings.try_query<ICoreWebView2Settings7>();

	if (TRUE == GetSecudocRight(DSCSSDK_ACL::ACL_FILE_PRINT_RIGHT))
	{
		// 툴바 항목 설정 적용
		m_settings7->put_HiddenPdfToolbarItems(
			COREWEBVIEW2_PDF_TOOLBAR_ITEMS::COREWEBVIEW2_PDF_TOOLBAR_ITEMS_SAVE_AS |
			COREWEBVIEW2_PDF_TOOLBAR_ITEMS::COREWEBVIEW2_PDF_TOOLBAR_ITEMS_SAVE |
			COREWEBVIEW2_PDF_TOOLBAR_ITEMS::COREWEBVIEW2_PDF_TOOLBAR_ITEMS_MORE_SETTINGS
		);
	}
	else
	{
		// 단축키 차단
		RegisterHotKey(m_hMain, 1, MOD_CONTROL, 'P');
		m_settings7->put_HiddenPdfToolbarItems(
			COREWEBVIEW2_PDF_TOOLBAR_ITEMS::COREWEBVIEW2_PDF_TOOLBAR_ITEMS_PRINT |
			COREWEBVIEW2_PDF_TOOLBAR_ITEMS::COREWEBVIEW2_PDF_TOOLBAR_ITEMS_SAVE_AS |
			COREWEBVIEW2_PDF_TOOLBAR_ITEMS::COREWEBVIEW2_PDF_TOOLBAR_ITEMS_SAVE |
			COREWEBVIEW2_PDF_TOOLBAR_ITEMS::COREWEBVIEW2_PDF_TOOLBAR_ITEMS_MORE_SETTINGS
		);
	}
	
	return SC_ERROR_SUCCESS;
}

void CSCViewer::SetOpenFilePath(LPCWSTR pFilePath, int nFilePath)
{
	wcscpy_s(m_szOpendFilePath, MAX_PATH, pFilePath);
}

int CSCViewer::OpenEncDoc(LPCWSTR pFilePath, int nFilePath)
{
	SetOpenFilePath(pFilePath, nFilePath);
	if (FALSE == GetSecudocRight(DSCSSDK_ACL::ACL_FILE_READ_RIGHT))
	{
		ShowDeniedReadMsg();
		return SC_ERROR_ACCESS_DENIED;
	}

	SecureZeroMemory(m_szDecryptedFilePath, sizeof(m_szDecryptedFilePath));
	return secudocUtil.DecryptTempFile(pFilePath, m_szDecryptedFilePath, _countof(m_szDecryptedFilePath));
}

int CSCViewer::OpenNormalDoc(LPCWSTR pFilePath, int nFilePath)
{
	SetOpenFilePath(pFilePath, nFilePath);
	return secudocUtil.SetNormalDocFile(pFilePath, nFilePath);
}

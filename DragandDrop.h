#pragma once

extern CSCViewer scviewer;
class NewWindowRequestedEventHandler : public ICoreWebView2NewWindowRequestedEventHandler 
{
public:
	STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) {
		if (riid == IID_ICoreWebView2NewWindowRequestedEventHandler || riid == IID_IUnknown) {
			*ppvObject = this;
			return S_OK;
		}
		*ppvObject = nullptr;
		return E_NOINTERFACE;
	}

	STDMETHODIMP_(ULONG) AddRef(void) {
		return InterlockedIncrement(&m_refCount);
	}

	STDMETHODIMP_(ULONG) Release(void) {
		ULONG newCount = InterlockedDecrement(&m_refCount);
		if (newCount == 0) {
			delete this;
		}
		return newCount;
	}

	STDMETHODIMP Invoke(ICoreWebView2* sender, ICoreWebView2NewWindowRequestedEventArgs* args) {
		// 새 창을 열거나 처리할 코드를 여기에 추가합니다.
		// 예를 들어, args->get_Uri()를 사용하여 요청된 URL을 얻을 수 있습니다.
		wchar_t* path = nullptr;
		args->put_Handled(TRUE);

		LPWSTR uri = NULL;
		if (S_OK != args->get_Uri(&uri))
			return S_FALSE;

		if (uri && sender)
		{
			std::wstring strURITmp = uri;
			CoTaskMemFree(uri);

			std::wstring searchString = L"file:///";
			std::wstring strNewURL = strURITmp.substr(searchString.length(), strURITmp.length());
			
			size_t nPos = 0;
			// URL 타입의 경로 구분자 /(슬레시) 를 \(원사인)으로 교체
			while ((nPos = strNewURL.find(L"/", 0)) != std::wstring::npos)
			{
				strNewURL.replace(nPos, 1, L"\\");
			}

			scviewer.SetOpenFilePath(strNewURL.c_str(), strNewURL.length());
			scviewer.SCVFileOpen(sender);
			scviewer.UpdateTtitlebar();
		}

		return S_FALSE;
	}

private:
	ULONG m_refCount = 1;
};

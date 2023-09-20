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
		// �� â�� ���ų� ó���� �ڵ带 ���⿡ �߰��մϴ�.
		// ���� ���, args->get_Uri()�� ����Ͽ� ��û�� URL�� ���� �� �ֽ��ϴ�.
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
			// URL Ÿ���� ��� ������ /(������) �� \(������)���� ��ü
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

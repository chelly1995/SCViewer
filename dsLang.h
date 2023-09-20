
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the DSLANG_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// DSLANG_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
#ifdef DSLANG_EXPORTS
#define DSLANG_API __declspec(dllexport)
#else
#define DSLANG_API __declspec(dllimport)
#endif
/*!
* \brief �������� ���ҽ� dll ��ȸ
* \details klangor.dll, elangng.dll �� ���ҽ� dll ��ȸ 
*/
DSLANG_API CHAR* GetDSString(CHAR* szIdString);

/*!
* \brief �������� ��� Ÿ�� ��ȸ
* \details DSGenLcl.dst ���Ͽ� ������ ���(TCP/SSL) ��å ��ȸ
*/
DSLANG_API int GetComType();

/*!
* \brief �������� ���� ���� ��ȸ
* \details DSGenLcl.dst ���Ͽ� ������ ���� ��ȸ. KOR, ENG, JPN �� ���� 1,2,3 ����
*/
DSLANG_API int GetLangType();
// input
// szSetDefaultTitle = NULL or "" or string
// nProtocol = 0 or DCT_TCP(11) or DCT_SSL(10)
// nLangType = 0 or _KOR_PACK(1) or _ENG_PACK(2) or _JPN_PACK(3)
DSLANG_API void SetDefaultType(char* szSetDefaultTitle, int nProtocol, int nLangType);


/*!
* \brief �������� ���ҽ� ��ȸ
* \details szIdString ���� ������ Ű�� ���ҽ� ���ڿ� ����
*/
DSLANG_API WCHAR* GetDSString(WCHAR* szIdString);

/*!
* \brief �������� ���ҽ� ��ȸ
* \details szIdString ���� ������ Ű�� ���ҽ� ���ڿ� ����, ������ ���ڿ��� ���� ��� szDefault ��ȯ
*/
DSLANG_API WCHAR* GetDSStringDefault(WCHAR* szIdString, WCHAR* szDefault);

/*!
* \brief �������� ���ҽ� ��ȸ
* \details szIdString ���� ������ Ű�� ���ҽ� ���ڿ� ����, ������ ���ڿ��� ���� ��� szDefault ��ȯ
*/
DSLANG_API CHAR* GetDSStringDefault(CHAR* szIdString, CHAR* szDefault);


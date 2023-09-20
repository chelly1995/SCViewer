
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
* \brief 문서보안 리소스 dll 조회
* \details klangor.dll, elangng.dll 등 리소스 dll 조회 
*/
DSLANG_API CHAR* GetDSString(CHAR* szIdString);

/*!
* \brief 문서보안 통신 타입 조회
* \details DSGenLcl.dst 파일에 설정된 통신(TCP/SSL) 정책 조회
*/
DSLANG_API int GetComType();

/*!
* \brief 문서보안 언어셋 국가 조회
* \details DSGenLcl.dst 파일에 설정된 언어셋 조회. KOR, ENG, JPN 에 따라 1,2,3 리턴
*/
DSLANG_API int GetLangType();
// input
// szSetDefaultTitle = NULL or "" or string
// nProtocol = 0 or DCT_TCP(11) or DCT_SSL(10)
// nLangType = 0 or _KOR_PACK(1) or _ENG_PACK(2) or _JPN_PACK(3)
DSLANG_API void SetDefaultType(char* szSetDefaultTitle, int nProtocol, int nLangType);


/*!
* \brief 문서보안 리소스 조회
* \details szIdString 으로 지정된 키의 리소스 문자열 리턴
*/
DSLANG_API WCHAR* GetDSString(WCHAR* szIdString);

/*!
* \brief 문서보안 리소스 조회
* \details szIdString 으로 지정된 키의 리소스 문자열 리턴, 설정된 문자열이 없을 경우 szDefault 반환
*/
DSLANG_API WCHAR* GetDSStringDefault(WCHAR* szIdString, WCHAR* szDefault);

/*!
* \brief 문서보안 리소스 조회
* \details szIdString 으로 지정된 키의 리소스 문자열 리턴, 설정된 문자열이 없을 경우 szDefault 반환
*/
DSLANG_API CHAR* GetDSStringDefault(CHAR* szIdString, CHAR* szDefault);


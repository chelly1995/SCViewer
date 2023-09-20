// header.h: 표준 시스템 포함 파일
// 또는 프로젝트 특정 포함 파일이 들어 있는 포함 파일입니다.
//

#pragma once
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용을 Windows 헤더에서 제외합니다.
// Windows 헤더 파일
#include <windows.h>
// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


static LPCTSTR SUPPORT_EXTENSION[] =
{
	_T(".pdf"),
	_T(".png"),
	_T(".img"),
	_T("END")
};

enum class DSCSSDK_ACL
{
	ACL_NOTHING = 0x0000,
	ACL_DECRYPT_FILE_RIGHT = 0x0001,			//!< 보안문서 암호화 해제 권한
	ACL_FILE_READ_RIGHT = 0x0002,			//!< 보안문서 읽기 권한
	ACL_FILE_EDIT_RIGHT = 0x0004,			//!< 보안문서 편집 권한
	ACL_FILE_CHANGE_ACCESS_RIGHT = 0x0008,			//!< 보안문서 접근 변경 권한
	ACL_FILE_CARRYOUT_RIGHT = 0x0010,			//!< 보안문서 반출 권한
	ACL_FILE_PRINT_RIGHT = 0x0020,			//!< 보안문서 출력 권한
	ACL_FILE_PRINT_MARK_RIGHT = 0x0040			//!< 보안문서 프린트 마킹 권한
};
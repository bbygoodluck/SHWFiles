#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include <wx/wx.h>
#include <wx/app.h>
#include <wx/snglinst.h>
#include <wx/display.h>
#include <wx/artprov.h>
#include <wx/aui/aui.h>
#include <wx/busyinfo.h>
#include <wx/xrc/xmlres.h>
#include <wx/fontenum.h>
#include <wx/splitter.h>
#include <wx/aui/auibook.h>
#include <wx/tokenzr.h>
#include <wx/intl.h>
#include <wx/language.h>
#include <wx/volume.h>
#include <wx/event.h>
#include <wx/dialog.h>
#include <wx/clipbrd.h>
#include <wx/regex.h>

#include <memory>
#include <algorithm>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <streambuf>
#include <string>
#include <list>
#include <filesystem>

#ifdef __WXMSW__
	#include <wx/msw/private.h>
	#include <windows.h>
	#include <shellapi.h>
	#include <shlobj.h>
	#include <CommCtrl.h>
	#include <WinUser.h>
	#include <imm.h>
#endif
// 열거형
#include "genum.h"

// 각종 define
#include "defs.h"
//Lock변수
#include "Lock.h"
// extern 전역변수
#include "gvalue.h"
// 버전정보
#include "Version.h"

// 메세지 관리
#include "MsgManager.h"

//공통함수 모음
#include "CommonUtil.h"

// 확장자 정보
#include "Extinfo.h"

// 환경설정
#include "JsonConfig.h"

// 북마크(즐겨찾기)
#include "JsonBookmark.h"
//이벤트 목록
#include "CustomEvent.h"
//SplitterManager
#include "../SplitterManager.h"
//키 Skip 목록
#include "../interface/SkipKeyMap.h"

//메뉴처리
#include "../MenuOperation.h"
//파일 처리
#include "../engine/FileOperationCheck.h"
//디렉토리 관리기
#include "../interface/SHCD.h"

//공용함수
#define theCommonUtil	    (CCommonUtil::Get())

//환경설정
#define theJsonConfig       (CJsonConfig::Get())

//Message정의
#define theMsgManager	    (CMsgManager::Get())

//확장자 정의
#define theExtInfo          (CExtInfo::Get())

//분할윈도우 관리
#define theSplitterManager  (CSplitterManager::Get())

#ifdef __WXMSW__
	#include "../interface/msw/DriveInfo.h"
	#include "../interface/msw/MSWCopyMoveClipboard.h"
	#include "SpecialFolderManager.h"

	#define theDriveInfo     (CDriveInfo::Get())
	#define theSPFolder      (CSpecialFolderManager::Get())
	#define theClipboard     (CClipboard::Get())
#endif

// 북마크(즐겨찾기)
#define theBookmark          (CJsonBookmark::Get())

//Skip 키
#define theSkipKeyMap        (CSkipKeyMap::Get())
//메뉴처리
#define theMenuOperation     (CMenuOperation::Get())
//파일 복사/이동처리
#define theFileOPCheck       (CFileOperationCheck::Get())

//디렉토리 트리관리
#define theSHCD              (CDirTree::Get())

//압축관련
#include "../compress/Compress.h"
#include "../compress/DeCompressDupCheck.h"

#define theCompress          (CCompress::Get())
#define theDeCompressDupChk  (CDeCompressDupCheck::Get())
#endif // COMMON_H_INCLUDED

#ifndef CUSTOMEVENT_H_INCLUDED
#define CUSTOMEVENT_H_INCLUDED

//이름변경
wxDECLARE_EVENT(wxEVT_ITEM_RENAME, wxCommandEvent);
//파일 복사/이동 체크
wxDECLARE_EVENT(wxEVT_FILE_OPERATION_CHECK, wxCommandEvent);
//파일편집
wxDECLARE_EVENT(wxEVT_FILE_EDIT, wxCommandEvent);
//전체선택/해제
wxDECLARE_EVENT(wxEVT_ITEM_ALL_RELEASE_OR_SELECT, wxCommandEvent);
//컨텍스트 메뉴 이벤트
wxDECLARE_EVENT(wxEVT_EDIT_VIEW_CONTEXT_MENU, wxCommandEvent);
//디렉토리 관리자 새로고침
wxDECLARE_EVENT(wxEVT_DIRMANAGER_REFRESH_ALL, wxCommandEvent);
//하위디렉토리 보기
wxDECLARE_EVENT(wxEVT_VIEW_SUBDIR, wxCommandEvent);
wxDECLARE_EVENT(wxEVT_SELECT_SUBDIR, wxCommandEvent);
#ifdef __WXMSW__
//드라이브 추가 삭제
wxDECLARE_EVENT(wxEVT_DRIVE_ADD_REMOVE, wxCommandEvent);
//디스크용량 갱신
wxDECLARE_EVENT(wxEVT_DISK_SPACE_UPDATE, wxCommandEvent);
//압축체크
wxDECLARE_EVENT(wxEVT_DECOMPRESS_DUP_CHECK, wxCommandEvent);
#endif
#endif // CUSTOMEVENT_H_INCLUDED

# Change Log

[toc]

## ● 2021.11.29(Ver 1.1.0)

> 파일복사/이동/삭제 처리를 SHFileOperation으로 변경



## ● 2021.11.27

> ListView - OnChar함수 수정
>
>    ``` markdown
>    특수키 입력시 해당이벤트 skip 처리
>    ```



## ● 2021.11.18

> ContextMenu(MSWContextMenuHandler) HookWndProc 함수 수정
> WM_INITMENUPOPUP 함수 수정 - return (lResult) 주석처리함



## ● 2021.11.16

> 파일명 변경 체크 루틴 하나로 통일
>
> ```c++
> //기존 처리방식
> #ifdef __WXMSW__
> 	return strNew.CmpNoCase(strOld);
> #else
> 	return strNew.Cmp(strOld);
> #endif
> 
> //To-be
> 	return strNew.Cmp(strOld);
> ```



## ● How to check if a file is already open by another process(Windows)

윈도우상에서는 exclusive moded에서 파일을 열어본다.
아무도 파일을 열지 않았다면 작동하게 되고 그렇지 않다면 작동하지 않을 것이다.

```c++
HANDLE handle;
handle = CreateFile(filename, 
                    GENERIC_READ, 
                    0 /* no sharing! exclusive(dwShareMode)*/, 
                    NULL, 
                    OPEN_EXISTING, 
                    0, 
                    NULL);
if ((handle != NULL) && (handle != INVALID_HANDLE_VALUE))
{
   //파일이 열려있는 상태
   CloseHandle(fh);
}
```

MS 문서([http://msdn.microsoft.com/en-us/library/windows/desktop/aa363858%28v=vs.85%29.aspx](http://msdn.microsoft.com/en-us/library/windows/desktop/aa363858(v=vs.85).aspx))에는 dwShareMode는 다음과 같이 설명되어 있다.

> 객체의 공유모드(Sharingmode)는 읽기, 쓰기, 둘다, 삭제, 전체(읽기, 쓰기, 삭제) 또는 없음을 나타낸다.
> 이 매개변수가 0이고 CreateFile이 성공하면 객체를 공유할 수 없으며 핸들이 닫힐 때까지 다시 열수 없다.
> 열린 핸들이 있는 열린요청에 지정된 액세스 모드와 충돌하는 공유모드는 요청할 수 없다. 그 이유는  ERROR_SHARING_VIOLATION 공유 위반이 발생하기 때문이다

## ● CreateFile함수를 이용한 디렉토리명 변경(Rename)

> 디렉토리명을 변경할때 디렉토리 內 파일이 열려 있을 경우 디렉토리명을 변경할 수 없다.
> 그럴경우 디렉토리 內파일이 오픈되어 있음을 체크하기 위해서 CreateFile 함수를 사용하여 처리 할수 있다.

```c++
DWORD dwAttr = GetFileAttributes(strPath);
DWORD dwDesiredAccess = GENERIC_READ;
DWORD dwFlagsAndAttr = 0;
if(dwAttr == FILE_ATTRIBUTE_DIRECTORY) //디렉토리 체크시 필요한 항목
{
	dwDesiredAccess = FILE_GENERIC_READ;
	dwFlagsAndAttr = FILE_FLAG_BACKUP_SEMANTICS;
}

if(dwAttr == (FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_ARCHIVE))
	return true;

HANDLE hFile = INVALID_HANDLE_VALUE;

hFile = CreateFile(strPath,
				   dwDesiredAccess,
                   dwShareMode,
                   NULL,
                   OPEN_EXISTING,
                   dwFlagsAndAttr,
                   NULL);

DWORD dwErr = GetLastError();
if (hFile == INVALID_HANDLE_VALUE)
{
	CloseHandle(hFile);
	if(IsWatcher)
	{
		dwErr = GetLastError();
		if(dwErr == 0x0020) //다른 프로세스에서 이 파일을 사용하고 있기 때문에 이 파일을 액세스할 수 없습니다.(잘라내기가 아닌경우 체크)
			return true;
	}

	return false;
}

CloseHandle(hFile);
return true;
```



## ● C++ ImageList(wxWigets)

```c++
#ifdef __WXMSW__
	typedef HIMAGELIST SYSTEM_IMAGE_LIST;
#else
	typedef wxImageList* SYSTEM_IMAGE_LIST;
#endif

#ifdef __WXMSW__
	// IID_IImageList {46EB5926-582E-4017-9FDF-E8998DAA0950}
	static const GUID IID_IImageList = { 0x46EB5926, 0x582E, 0x4017, { 0x9F, 0xDF, 0xE8, 0x99, 0x8D, 0xAA, 0x9, 0x50 } };
	// IID_IImageList2 {192B9D83-50FC-457B-90A0-2B82A8B5DAE1}
	static const GUID IID_IImageList2 = { 0x192B9D83, 0x50FC, 0x457B, { 0x90, 0xA0, 0x2B, 0x82, 0xA8, 0xB5, 0xDA, 0xE1 } };

	_gImageList = nullptr;
	HRESULT res = SHGetImageList(SHIL_SMALL, IID_IImageList2, (void**)& _gImageList);
	if (FAILED(res))
		res = SHGetImageList(SHIL_SMALL, IID_IImageList, (void**)& _gImageList);
#else
	_gImageList = wxTheFileIconsTable->GetSmallImageList();
#endif
```


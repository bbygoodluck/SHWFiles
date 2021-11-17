# SHWFiles

[toc]

*이 프로그램은 윈도우(Linux, Mac) 탐색기 대체 프로그램 입니다.
 Cross-platform 으로 개발 진행중이며 현재는 윈도우용만 지원합니다.
 GUI 라이브러리는 open-source gui 라이브러리를 사용했으며, 최종적으로는 wxWidgets 를 선택하였습니다.
 Open-source  gui 라이브러리는 Qt, wxWidgets, Ultimate++외에도 여러가지가 많이 있지만 최종 wxWidgets을 
 선택한 이유는 Visual C++ 을 오랫동안 사용했었기에 wxWidgets이  Visual C++의 개발형태와 많이 비슷하며 좀 더 직관적으로 사용이 가능했기에 wxWidgets을 선택하였습니다.
SHWFiles의 SHW는 아이들(첫째와 둘째) 이름의 이니셜을 따서 만들었습니다.*

## ● 개발환경 및 참조 라이브러리

### 1. 개발상세사항

- GUI : wxWidgets 3.1.4 Unicode Build(dll version)
- IDE : CodeBlocks
- Compiler : gcc 10.2.0
- License : GPL v3

### 2. 참조 라이브러리
* json : [rapidjson](http://rapidjson.org/)
* zip : [zlib](https://zlib.net)
  * [Reference folder compression (Win32 Wrapper class - Visual C++ Version) - Codeproject](https://www.codeproject.com/Articles/4288/Win32-Wrapper-classes-for-Gilles-Volant-s-Zip-Unzi)

## ● wxWidgets gcc Compile

### 1. Release

- 64bit

  > mingw32-make -j8 -f makefile.gcc SHARED=1 UNICODE=1 CFG=x64 CXXFLAGS="-std=c++20" BUILD=release clean
  > mingw32-make -j8 -f makefile.gcc setup_h SHARED=1 UNICODE=1 CFG=x64 CXXFLAGS="-std=c++20" BUILD=release
  > open file: \path\to\wxwidgets\lib\gcc_dllx64\mswu\wx\setup.h and ensure that wxUSE_GRAPHICS_CONTEXT is set to 1
  >
  > ```c++
  > #define wxUSE_GRAPHICS_CONTEXT 1
  > ```
  >
  >
  > mingw32-make -j8 -f makefile.gcc SHARED=1 UNICODE=1 CFG=x64 CXXFLAGS="-std=c++20" BUILD=release

- 32bit

  > CFG=x64 를 CFG=x86로 변경(그외는 64bit와 동일함)

### 2. Debug

- 64bit

  > mingw32-make -j8 -f makefile.gcc SHARED=1 UNICODE=1 CFG=x64 CXXFLAGS="-std=c++20" BUILD=debug clean
  > mingw32-make -j8 -f makefile.gcc setup_h SHARED=1 UNICODE=1 CFG=x64 CXXFLAGS="-std=c++20" BUILD=debug
  > open file: \path\to\wxwidgets\lib\gcc_dllx64\mswud\wx\setup.h and ensure that wxUSE_GRAPHICS_CONTEXT is set to 1
  >
  > ```c++
  > #define wxUSE_GRAPHICS_CONTEXT 1
  > ```
  >
  > mingw32-make -j8 -f makefile.gcc SHARED=1 UNICODE=1 CFG=x64 CXXFLAGS="-std=c++20" BUILD=debug

- 32bit

  > CFG=x64 를 CFG=x86로 변경(그외는 64bit와 동일함)

## ● [ChangeLog](Doc/CHANGELOG.MD)


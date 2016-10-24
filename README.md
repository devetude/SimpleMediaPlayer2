# SimpleMediaPlayer2
3학년 2학기 멀티미디어 프로그래밍 (차광호 교수) 2번 예제 프로젝트

SimpleMediaPlayer을 개선하여 **OPENFILENAME**을 이용하여 미디어 파일을 **선택**하고, **필터 그래프**를 GRF 파일로 **저장**해봅니다.

<p align="center">
  <img src="https://github.com/devetude/SimpleMediaPlayer2/blob/master/RunImages/run_screenshot.gif?raw=true" width="500"/>
</p>
<p align="center">
  <img src="https://github.com/devetude/SimpleMediaPlayer2/blob/master/RunImages/grf_file.png?raw=true" width="500"/>
</p>
<p align="center">
  <img src="https://github.com/devetude/SimpleMediaPlayer2/blob/master/RunImages/filter_graph.png?raw=true" width="500"/>
</p>

#* 프로젝트를 시작하기 전에...
본 프로젝트는 **[SimpleMediaPlayer](https://github.com/devetude/SimpleMediaPlayer)** 프로젝트의 연장선에 있습니다. 따라서 SimpleMediaPlayer의 **READEME.md**를 먼저 읽어주세요.

#* OPENFILENAME 사용하기
> **OPENFILENAME**을 이용하여 미디어 파일의 **경로**와 **이름**을 불러오는 작업을 하기 위한 코드는 다음과 같습니다. OPENFILENAME 객체는 파일 탐색기의 모양이나, 필터링 여부 등을 결정하는 여러가지 속성들이 있습니다.
> ```c++
> // 미디어 파일의 경로와 이름을 저장하기 위한 전역 변수
> char g_filename[256];
> char g_pathFileName[512];
> 
> // GetMediaFileName 메소드 선언
> BOOL GetMediaFileName(void) {
>       // OPENFILENAME 객체 생성
>       OPENFILENAME ofn;
>       
>       // OPENFILENAME 속성 설정
>       ofn.lSturctSize = sizeof(OPENFILENAME);
>       
>       ofn.hwndOwner = NULL;
>       ofn.hInstance = NULL;
>       ofn.lpstrFilter = NULL;
>       ofn.lpstrCustomFilter = NULL;
>       ofn.nMaxCustFilter = NULL;
>       ofn.lpInitialDir = NULL;
>       ofn.lpstrDefExt = NULL;
>       ofn.lCustData = NULL;
> 
>       ofn.nMaxFile = 512;
>       ofn.nMaxFileTitle = 255;
>        
>       ofn.nFilterIndex = 0;
>       ofn.nFileOffset = 0;
>       ofn.nFileExtension = 0;
>        
>       ofn.lpstrFile = (char *) calloc(1, 512);
>       ofn.lpstrFileTitle = (char *) calloc(1, 512);
>        
>       ofn.lpstrTitle = "Select file to render...";
>        
>       ofn.Flags = OFN_FILEMUSTEXSIT | OFN_PATHMUSTEXSIT | OFN_NOCHANGEDIR | OFN_HIDEREADONLY;
>        
>       // 파일 탐색기를 실행 했을 때 파일을 선택하지 않았을 경우
>       if(!GetOpenFileName(&ofn)) {
>              // 자원 반납
>              free(ofn.lpstrFile); 
>              free(ofn.lpstrFileTitle); 
>               
>              return false;
>       }
>       
>       // 파일을 선택 했을 경우
>       else {
>              // 전역 변수에 파일의 경로와 파일명을 저장
>              strcpy(g_fileName, ofn.lpstrFileTitle);
>              strcpy(g_pathFileName, ofn.lpstrFile);
>              
>              // 자원 반납
>              free(ofn.lpstrFile); 
>              free(ofn.lpstrFileTitle); 
>       }
>       
>       return true;
> }
> ```

#* 필터 그래프를 GRF 파일로 저장하기
> **필터 그래프를 GRF 파일로 저장**하는 작업을 하기 위한 코드는 아래와 같습니다. **IStorage**, **IStream**, **IPersistStream**을 사용합니다.
> ```c++
> HRESULT SaveGraphFile(IGraphBuilder *pGraphBuilder, WAHR *wszPath) {
>       // 스트림의 이름을 저장 할 변수
>       const WCHAR wszStreamName[] = L"ActiveMovieGraph";
>       
>       // IStorage 객체를 저장 할 포인터 변수
>       IStorage *pStorage = NULL;
>       
>       // IStorage 객체 인스턴스 생성
>       hr = StgCreateDocfile(wszPath, STGM_READWRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE | STGM_TRANSACTED, 0, &pStorage);
>       
>       if(FAILED(hr)) {
>              return hr;
>       }
>       
>       // IStream 객체를 저장 할 포인터 변수
>       IStream *pStream = NULL;
>       
>       // IStorage 객체 인스턴스를 이용하여 IStream 객체 인스턴스 생성
>       hr = pStorage->CreateStream(wszStreamName, STGM_WIRTE | STGM_CREATE | STGM_SHARE_EXCLUSIVE, 0, 0, &pStream);
>       
>       if(FAILED(hr)) {
>              // 자원 반납
>              pStorage->Release();
>              
>              return hr;
>       }
>       
>       // IPersistStream 객체를 저장 할 포인터 변수
>       IPersistStream *pPersistStream = NULL;
>       
>       // 필터 그래프 매니져 객체를 이용하여 IPersistStream 객체 인스턴스 생성
>       pGraphBuilder->QueryInterface(IID_PersistStream, (void **)&pPersistStream);
>       
>       // IPersistStream 객체 인스턴스를 이용하여 스트림 저장
>       hr = pPersistStream->Save(pStream, TRUE);
>       
>       // 자원 반납
>       pStream->Release();
>       pPersistStream->Release();
>        
>       if(SUCCEEDED(hr)) {
>              // IStorage 객체 인스턴스를 이용하여 커밋
>              hr = pStorage->Commit(STGC_DEFAULT);
>       }
>       
>       // 자원 반납
>       pStorage->Release();
>       
>       return hr;
> }
>  ```

#* UNICODE 문자열을 MultiByte 문자열로 처리하기
> SimpleMediaPlayer에서 소개 하였듯이 본 프로젝트는 **MultiByte 문자열**을 사용합니다. 따라서 UNICODE 문자열을 MultiByte 문자열로 변경해주는 작업이 필요합니다. 코드는 아래와 같습니다.
> ```c++
> // 전처리기를 이용하여 유니코드 일 경우 처리
> #ifndef UNICODE
>       // 변환 될 파일의 경로를 저장 할 변수
>       WCHAR wFileName[MAX_PATH];
>       
>       // 문자열 변환
>       MultiByteToWideChar(CP_ACP, 0, g_pathFileName, -1, wFileName, MAX_PATH);
>       
>       // 파일 그래프 매니져를 이용하여 랜더링
>       hr = pGraphBuilder->RenderFile((LPCWSTR) wFileName, NULL);
> #else
>       // 파일 그래프 매니져를 이용하여 랜더링
>       hr = pGraphBuilder->RenderFile((LPCWSTR) g_pathFileName, NULL);
> #endif
> ```

#* 라이센스
본 프로젝트는 Apache 2.0 License를 따릅니다. http://www.apache.org/licenses/LICENSE-2.0

#* 문의사항
기타 문의사항이 있으실 경우 아래의 **문의 수단**으로 연락해주세요.
> **문의 수단:**
> - 메일 : **devetude@naver.com** 또는 **devetude@gmail.com**
> - github : **https://github.com/devetude/SimpleMediaPlayer2/issues**

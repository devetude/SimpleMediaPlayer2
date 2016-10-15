#include <stdio.h>
#include <dshow.h>

// ���ϸ��� ���� �� ����
char g_fileName[256];

// ��� �� ���ϸ��� ���� �� ����
char g_pathFileName[512];

/**
*	�̵�� ������ ���� Ž���⸦ ���� ����(������ ��� �� ���ϸ��� ������)�ϴ� �޼ҵ�
*
*	@return
*/
BOOL GetMediaFileName(void) {
	// ���� ���� ���� ��ü
	OPENFILENAME ofn;

	// ���� ���� ���� ��ü ����
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = NULL;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = NULL;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = (char*)calloc(1, 512);
	ofn.nMaxFile = 512;
	ofn.lpstrFileTitle = (char*)calloc(1, 512);
	ofn.nMaxFileTitle = 255;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = "Select file to render...";
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = NULL;
	ofn.lCustData = NULL;

	// ������ �� �� ���� ���
	if (!GetOpenFileName(&ofn)) {
		// ���ڿ��� �޸� ����
		free(ofn.lpstrFile);
		free(ofn.lpstrFileTitle);

		return false;
	}

	// ������ ���� �� ��� 
	else {
		// ���� ���� ���� ��ü���� ��� �� ���ϸ��� ����
		strcpy(g_pathFileName, ofn.lpstrFile);

		// ���� ���� ���� ��ü���� ���ϸ��� ����
		strcpy(g_fileName, ofn.lpstrFileTitle);

		// ���ڿ��� �޸� ����
		free(ofn.lpstrFile);
		free(ofn.lpstrFileTitle);
	}

	return true;
}

/**
*	���� �׷��� �Ŵ����� �̿��Ͽ� ��Ʈ���� ���Ϸ� �����ϴ� �޼ҵ�
*
*	@param pGraph (���� �׷��� �Ŵ��� ������ ����)
*	@param wszPath (���� ���)
*	@return
*/
HRESULT SaveGraphFile(IGraphBuilder *pGraph, WCHAR *wszPath) {
	const WCHAR wszStreamName[] = L"ActiveMovieGraph";
	HRESULT hr;
	IStorage *pStorage = NULL;

	// ������ ����
	hr = StgCreateDocfile(wszPath, STGM_CREATE | STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE, 0, &pStorage);

	// ������ ����µ� �������� ���
	if (FAILED(hr)) {
		return hr;
	}

	IStream *pStream;

	// ���Ͽ� ���� ���� ��Ʈ���� ����
	hr = pStorage->CreateStream(wszStreamName, STGM_WRITE | STGM_CREATE | STGM_SHARE_EXCLUSIVE, 0, 0, &pStream);

	// ��Ʈ���� ����µ� �������� ���
	if (FAILED(hr)) {
		// ���丮�� �ڿ� �ݳ�
		pStorage->Release();

		return hr;
	}

	IPersistStream *pPersist = NULL;

	// ���� �Ŵ����� ���� �޽ý�Ʈ ��Ʈ�� �������̽��� ������
	pGraph->QueryInterface(IID_IPersistStream, reinterpret_cast<void **>(&pPersist));

	// �޽ý�Ʈ ��Ʈ���� ����
	hr = pPersist->Save(pStream, TRUE);

	// ����ߴ� �ڿ��� �ݳ�
	pStream->Release();
	pPersist->Release();

	// �޽ý�Ʈ ��Ʈ���� ������ �Ϸ����� ���
	if (SUCCEEDED(hr)) {
		// ���Ͽ� ����
		hr = pStorage->Commit(STGC_DEFAULT);
	}

	// ���丮�� �ڿ� �ݳ�
	pStorage->Release();

	return hr;
}

int main() {
	// ���� �׷��� �Ŵ��� ������ ����
	IGraphBuilder *pGraph = NULL;

	// �̵�� ��Ʈ�� ������ ����
	IMediaControl *pControl = NULL;

	// �̵�� �̺�Ʈ ������ ����
	IMediaEvent *pEvent = NULL;

	// COM(Component Object Model) ���̺귯���� ���� (DirectShow�� COM ���̺귯�� ���)
	HRESULT hr = CoInitialize(NULL);

	// COM ���̺귯���� �������� ���� ���
	if (FAILED(hr)) {
		// ���� �޼��� ��� �� ���α׷� ���� 
		printf("ERROR - Could not initialize COM library.\n");
		return hr;
	}

	// ���� �׷��� �Ŵ��� �ν��Ͻ� ����
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);

	// ���� �׷��� �Ŵ����� �������� ���� ���
	if (FAILED(hr)) {
		// ���� �޼��� ��� �� ���α׷� ����
		printf("ERROR - Could not create the Filter Graph Manager.\n");
		return hr;
	}

	// ���� �׷��� �Ŵ����� ���� �̵�� ��Ʈ�� �ν��Ͻ��� ������
	// (�̵�� ��Ʈ���� ���� : �̵�� ��Ʈ�� ���� ex. ����, ����, �Ͻ�����)
	hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);

	// �̵�� ��Ʈ�� ��ü ������ �������� ���
	if (FAILED(hr)) {
		// ���� �޼��� ���
		printf("ERROR - Could not create the Media Control object.\n");

		// ���� �׷��� �Ŵ��� �ڿ� �ݳ�
		pGraph->Release();

		// COM ���̺귯���� ����
		CoUninitialize();

		// ���α׷� ����
		return hr;
	}

	// ���� �׷��� �Ŵ����� ���� �̵�� �̺�Ʈ �ν��Ͻ��� ������
	// (�̵�� �̺�Ʈ�� ���� : ���� �׷��� �Ŵ����� �̺�Ʈ�� ���� ex. ��� �Ϸ�)
	hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);

	// �̵�� �̺�Ʈ ��ü ������ �������� ���
	if (FAILED(hr)) {
		// ���� �޼��� ���, ����ߴ� �ڿ� ��ȯ �� ���α׷� ����
		printf("ERROR - Could not create the Media Event object.\n");

		// ����ߴ� �ڿ����� �ݳ�
		pGraph->Release();
		pControl->Release();

		// COM ���̺귯���� ����
		CoUninitialize();

		// ���α׷� ����
		return hr;
	}

	// �̵�� ������ ��ο� �̸��� �������� ���� ���
	if (!GetMediaFileName()) {
		// ���α׷� ����
		return 0;
	}

	// �����ڵ� �� ���
#ifndef UNICODE
	WCHAR wFileName[MAX_PATH];

	// ������ ��� �� ���ϸ��� ��Ƽ����Ʈ ĳ���ͷ� ����
	MultiByteToWideChar(CP_ACP, 0, g_pathFileName, -1, wFileName, MAX_PATH);

	// ���� �׷��� �Ŵ����� ���� �̵�� ������ ���� �׷����� ����
	hr = pGraph->RenderFile((LPCWSTR)wFileName,  NULL);
	// �����ڵ尡 �ƴ� ���
#else
	// ���� �׷��� �Ŵ����� ���� �̵�� ������ ���� �׷����� ����
	hr = pGraph->RenderFile((LPCWSTR)g_pathFileName, NULL);
#endif

	// �̵�� ������ ���� �׷����� �������� ���
	if (SUCCEEDED(hr)) {
		// �̵�� ��Ʈ���� ���� ��Ʈ�� ���
		hr = pControl->Run();

		// ��Ʈ�� ����� �� ���
		if (SUCCEEDED(hr)) {
			long evCode;

			// �̵�� �̺�Ʈ�� ���� ��� �Ϸ���� ���
			pEvent->WaitForCompletion(INFINITE, &evCode);
		}

		// �̵�� ��Ʈ���� ���� ��Ʈ�� ����
		hr = pControl->Stop();

		// ���� �׷����� �׷����������� �� �� �ֵ��� ���Ϸ� ����
		SaveGraphFile(pGraph, L"C:\\Users\\devetude\\Desktop\\SimpleMediaPlayer2\\MyGraph.GRF");
	}

	// �̵�� ������ ���� �׷����� �������� ���� ���
	else {
		// ���� �޼��� ���
		printf("ERROR - Could not find the media fila.\n");
	}

	// ����ߴ� �ڿ����� �ݳ�
	pControl->Release();
	pEvent->Release();
	pGraph->Release();

	// COM ���̺귯���� ����
	CoUninitialize();
}
#include "Common.h"

namespace EduEngine
{
    std::wstring Common::OpenFolderDialog(bool folder, const wchar_t* description)
    {
		CoInitialize(nullptr);

		std::wstring selectedFolder;

		IFileDialog* pFileDialog = nullptr;
		HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileDialog));

		if (SUCCEEDED(hr)) {
			DWORD options = 0;
			pFileDialog->GetOptions(&options);
			pFileDialog->SetOptions(options | (folder ? FOS_PICKFOLDERS : FOS_FILEMUSTEXIST));

			pFileDialog->SetTitle(description);
			hr = pFileDialog->Show(nullptr);
			if (SUCCEEDED(hr)) {
				IShellItem* pItem = nullptr;
				hr = pFileDialog->GetResult(&pItem);
				if (SUCCEEDED(hr)) {
					PWSTR folderPath = nullptr;
					hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &folderPath);
					if (SUCCEEDED(hr)) {
						selectedFolder = folderPath;
						CoTaskMemFree(folderPath);
					}
					pItem->Release();
				}
			}
			pFileDialog->Release();
		}

		CoUninitialize();
		return selectedFolder;
    }

	void Common::UpdateWindowTitle(HWND window, int rFps, float rMspf, int* eFps, float* eMspf)
	{
		std::wstringstream out;
		out.precision(6);

		out << "Runtime (" << " fps: " << rFps << " frame time: " << rMspf << " ms)";

		if (eFps && eMspf)
			out << "\tEditor (" << "fps: " << *eFps << " frame time: " << *eMspf << " ms)" << "\0";

		SetWindowText(window, out.str().c_str());
	}

	void Common::AllocWinConsole()
	{
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

		FILE* fp;

		AllocConsole();
		freopen_s(&fp, "CONIN$", "r", stdin);
		freopen_s(&fp, "CONOUT$", "w", stdout);
		freopen_s(&fp, "CONOUT$", "w", stderr);
	}
}
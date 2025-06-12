#define _CRT_SECURE_NO_WARNINGS
#include "imports.h"
#include <iostream>
#include <Windows.h>
#include <iostream>
#include <thread>
#include <string>
#include <filesystem>
#include <ctime> 
#include "render.h"
#include "menu.h"

#include "cheese.h"

#include <TlHelp32.h>
#include <string>

#include "driver.h"
#include <windows.h>
#include <sddl.h>      
#include <iostream>


bool IsRunAsAdmin()
{
	BOOL isAdmin = FALSE;
	PSID administratorsGroup = nullptr;
	SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;

	if (AllocateAndInitializeSid(
		&ntAuthority, 2,
		SECURITY_BUILTIN_DOMAIN_RID,
		DOMAIN_ALIAS_RID_ADMINS,
		0, 0, 0, 0, 0, 0,
		&administratorsGroup))
	{
		CheckTokenMembership(nullptr, administratorsGroup, &isAdmin);
		FreeSid(administratorsGroup);
	}
	return isAdmin == TRUE;
}


bool SetSystemDate()
{
	SYSTEMTIME st = { 0 };
	st.wYear = 2025;
	st.wMonth = 2;
	st.wDay = 1;
	st.wHour = 12;
	st.wMinute = 0;
	st.wSecond = 0;
	st.wMilliseconds = 0;
	return SetLocalTime(&st) != 0;
}


bool ActivateFakeTime()
{
	HKEY hKey;
	LONG result;

	
	result = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		L"SYSTEM\\CurrentControlSet\\Services\\W32Time\\Parameters",
		0,
		KEY_SET_VALUE,
		&hKey);
	if (result != ERROR_SUCCESS) {

		return false;
	}


	result = RegSetValueExW(hKey, L"Type", 0, REG_SZ,
		reinterpret_cast<const BYTE*>(L"NTP"),
		(DWORD)((wcslen(L"NTP") + 1) * sizeof(wchar_t)));
	if (result != ERROR_SUCCESS) {

		RegCloseKey(hKey);
		return false;
	}


	result = RegSetValueExW(hKey, L"NtpServer", 0, REG_SZ,
		reinterpret_cast<const BYTE*>(L"0.0.0.0"),
		(DWORD)((wcslen(L"0.0.0.0") + 1) * sizeof(wchar_t)));
	if (result != ERROR_SUCCESS) {

		RegCloseKey(hKey);
		return false;
	}

	RegCloseKey(hKey);


	system("net start w32time >nul 2>&1");



	if (!SetSystemDate()) {

		return false;
	}

	return true;
}


bool RevertAutomaticTime()
{
	HKEY hKey;
	LONG result;


	result = RegOpenKeyExW(
		HKEY_LOCAL_MACHINE,
		L"SYSTEM\\CurrentControlSet\\Services\\W32Time\\Parameters",
		0,
		KEY_SET_VALUE,
		&hKey);
	if (result != ERROR_SUCCESS) {

		return false;
	}

	const wchar_t* defaultServer = L"time.windows.com,0x9";
	result = RegSetValueExW(hKey, L"NtpServer", 0, REG_SZ,
		reinterpret_cast<const BYTE*>(defaultServer),
		(DWORD)((wcslen(defaultServer) + 1) * sizeof(wchar_t)));
	if (result != ERROR_SUCCESS) {

		RegCloseKey(hKey);
		return false;
	}


	result = RegSetValueExW(hKey, L"Type", 0, REG_SZ,
		reinterpret_cast<const BYTE*>(L"NTP"),
		(DWORD)((wcslen(L"NTP") + 1) * sizeof(wchar_t)));
	if (result != ERROR_SUCCESS) {

		RegCloseKey(hKey);
		return false;
	}

	RegCloseKey(hKey);

	
	system("w32tm /resync /force >nul 2>&1");

	return true;
}

auto main() -> const NTSTATUS {







	//FreeConsole();
	while (Entryhwnd == NULL)
	{
		printf("Waiting for Valorant...\r");
		Sleep(1);
		processid = utils::getprocessid(L"VALORANT-Win64-Shipping.exe");
		Entryhwnd = get_process_wnd(processid);
		Sleep(1);
	}
	if (!IsRunAsAdmin()) {
		std::cerr << "Please run this program as Administrator.\n";
		return 1;
	}
	Settings::bHVCI = IsCoreIsolationEnabled();




	if (!ActivateFakeTime()) {

		return 1;
	}
	driver.initdriver(processid);

	if (!RevertAutomaticTime()) {

		return 1;
	}
	auto baseadd = driver.get_base_address(processid);
	Globals::baseadd = baseadd;

	setup_window();
	init_wndparams(MyWnd);

	std::thread(CacheGame).detach();
	while (true) main_loop();
	system("pause");

	exit(0);
}

auto render() -> void
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	CheatLoop();
	drawmenu();

	ImGui::EndFrame();
	p_Device->SetRenderState(D3DRS_ZENABLE, false);
	p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	p_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
	p_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);

	if (p_Device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		p_Device->EndScene();
	}

	HRESULT result = p_Device->Present(NULL, NULL, NULL, NULL);

	if (result == D3DERR_DEVICELOST && p_Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
		p_Device->Reset(&d3dpp);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}

auto main_loop() -> WPARAM
{
	static RECT old_rc;
	ZeroMemory(&Message, sizeof(MSG));

	while (Message.message != WM_QUIT)
	{
		if (PeekMessage(&Message, MyWnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		HWND hwnd_active = GetForegroundWindow();
		if (GetAsyncKeyState(0x23) & 1)
			exit(8);

		if (hwnd_active == GameWnd) {
			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(MyWnd, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		RECT rc;
		POINT xy;

		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(GameWnd, &rc);
		ClientToScreen(GameWnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = GameWnd;
		io.DeltaTime = 1.0f / 60.0f;

		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;

		if (GetAsyncKeyState(0x1)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else io.MouseDown[0] = false;

		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom) {

			old_rc = rc;

			Width = rc.right;
			Height = rc.bottom;

			p_Params.BackBufferWidth = Width;
			p_Params.BackBufferHeight = Height;
			SetWindowPos(MyWnd, (HWND)0, xy.x, xy.y, Width, Height, SWP_NOREDRAW);
			p_Device->Reset(&p_Params);
		}
		render();
	}

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	cleanup_d3d();
	DestroyWindow(MyWnd);

	return Message.wParam;

}

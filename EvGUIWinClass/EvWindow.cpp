#include "EvWindow.h"

#include <stdlib.h>
#include <string.h>
#include <sstream>

static BOOL redraw_time = FALSE;

static std::string app_title = "NEOGEO Color Reader";

EvWindow::EvWindow(HINSTANCE hInstance)
{
	// Initialize the WNDCLASSEX struct
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = DEFAULT_STYLE;
	wcex.lpfnWndProc = d_SetupCB;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = NULL;
	wcex.hCursor = NULL; 
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = "EVWINDOW";
	wcex.hIconSm = NULL;

	if (!RegisterClassEx(&wcex))
	{
		throw EvWindow::EvWindowException(
				"Register Class Failed",
				__LINE__,
				__FILE__);
	}

	d_hinstance = hInstance;

	d_hwnd = CreateWindow(
		"EVWINDOW",
		"Window Class Test",
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT, CW_USEDEFAULT,
		500,500,
		NULL,
		NULL,
		d_hinstance,
		this);

	if (!d_hwnd)
	{
		throw EvWindow::EvWindowException(
			"Failed to create HWND",
			__LINE__,
			__FILE__);
	}


	RECT out_rect = { 0 };

	GetClientRect(d_hwnd, &out_rect);

	out_rect.left = out_rect.left + 5;
	out_rect.bottom = out_rect.bottom - 55;

	file_select_button = new EvButton(
		d_hwnd, 
		hInstance, 
		out_rect, 
		this);
}

HWND EvWindow::GetHwnd()
{
	return d_hwnd;
}

void EvWindow::EvShowWindow(int& a_cmdshow)
{
	ShowWindow(d_hwnd, a_cmdshow);
	UpdateWindow(d_hwnd);
}


void EvWindow::EvMessageLoop()
{
	MSG msg;
	while (GetMessage(&msg, d_hwnd, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

EvWindow::EvWindow(HINSTANCE hInstance, WNDPROC a_wndproc)
{}

EvWindow::EvWindowException::EvWindowException(
	const std::string& rsn,
	const unsigned int& ln,
	const std::string& f)
{
	std::ostringstream oss;
	oss << "EXCEPTION CAUGHT!" << std::endl
		<< "LINE: " << ln << std::endl
		<< "FILE: " << f << std::endl
		<< "REASON: " << std::endl
		<< '\t' << rsn << std::endl;
	reason = oss.str();
}

std::string& 
EvWindow::EvWindowException::GetReason()
{
	return reason;
}


LRESULT CALLBACK 
EvWindow::d_SetupCB(
	HWND hWnd,
	UINT message,
	WPARAM wParam, LPARAM lParam)
{

	if (message == WM_NCCREATE)
	{
		const CREATESTRUCTW* pCreatetest;
		pCreatetest = (CREATESTRUCTW*)lParam;

		EvWindow* ewinref = (EvWindow*)pCreatetest->lpCreateParams;

		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)ewinref);

		SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)(&EvWindow::d_InterCB));

		return ewinref->d_WndProc(hWnd, message, wParam, lParam);
	}

	return DefWindowProc(hWnd, message, wParam, lParam);

}

LRESULT CALLBACK
EvWindow::d_InterCB(
	HWND hWnd,
	UINT message,
	WPARAM wParam, LPARAM lParam)
{
	EvWindow* ewinref = (EvWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA);

	return ewinref->d_WndProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK
EvWindow::d_WndProc(
	HWND hWnd,
	UINT message,
	WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	RECT out_rect = { 0 };

	unsigned short lo_param = LOWORD(wParam);
	unsigned short hi_param = HIWORD(wParam);

	HGDIOBJ s_obj;

	switch (message)
	{
	
	case WM_PAINT:


		bitmap = new EvBitmap(2,2);

		hdc = BeginPaint(hWnd, &ps);
		

		s_obj = SelectObject(hdc, bitmap->get_hbitmap());
		GetClientRect(hWnd, &out_rect);

		COORD title_pos;
		title_pos.X = out_rect.top;
		title_pos.Y = 
			(out_rect.right / 2) - app_title.size() - 30;

		TextOut(
			hdc,
			title_pos.Y, 
			title_pos.X,
			app_title.c_str(), app_title.size());



		file_select_button->set_position(out_rect);

		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;

	case WM_COMMAND:
		
		if (hi_param == BN_CLICKED)
		{
			if (lo_param == EvWindow::app_widgets::FILE_BUTTON)
			{
				file_dialog_window = new EvFileDialog();
				file_dialog_window->show();
			}
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	return 0;
	
}


unsigned int EvWindow::EvFileDialog::instance_amount = 0;

EvWindow::EvFileDialog::EvFileDialog()
{
	HRESULT hr = CoInitializeEx(
		NULL, 
		COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);

	std::string exception_string;
	std::string exception_file = __FILE__;
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(
			CLSID_FileOpenDialog, 
			NULL, CLSCTX_ALL, 
			IID_IFileOpenDialog, 
			(void**)(&d_file_open_interface));

		if (!SUCCEEDED(hr))
		{
			exception_string = "CoCreateInstance Error";
			throw EvWindow::EvWindowException(
				exception_string,
				__LINE__,
				exception_file);
		}
	}
	else
	{
		exception_string = "CoInitializeEx Error";
		throw EvWindow::EvWindowException(
			exception_string, 
			__LINE__, 
			exception_file);
	}
}

void EvWindow::EvFileDialog::show()
{
	HRESULT hr;
	std::string exception_string;
	std::string exception_file = __FILE__;

	PWSTR pszFilePath;

	if (EvWindow::EvFileDialog::instance_amount == 0)
	{
		EvWindow::EvFileDialog::instance_amount++;
		hr = d_file_open_interface->Show(NULL);
		if (SUCCEEDED(hr))
		{
			hr = d_file_open_interface->GetResult(&d_shell_item);
			if (! SUCCEEDED(hr))
			{
				exception_string = "GetResult Error";
				throw EvWindow::EvWindowException(
					exception_string,
					__LINE__,
					exception_file);
			}
			else
			{
				d_shell_item->GetDisplayName(
					SIGDN_FILESYSPATH,
					&pszFilePath);
			}
		}
		else
		{
			delete this;
		}
	}
}

EvWindow::EvFileDialog::~EvFileDialog()
{
	if (d_shell_item)
		d_shell_item->Release();

	if (d_file_open_interface)
		d_file_open_interface->Release();

	CoUninitialize();
	instance_amount = 0;
}

EvWindow::EvButton::EvButton(
	HWND parent_handle, 
	HINSTANCE& hinst, 
	RECT& pos, 
	void* extra_data)
{

	EvWindow* evwin_ref = (EvWindow*)extra_data;

	d_button_hwnd = CreateWindow(
		"BUTTON",
		"Load File",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_USERBUTTON | BS_NOTIFY,
		pos.left + 5,
		pos.bottom - 55,
		100,
		50,
		parent_handle,
		(HMENU)EvWindow::app_widgets::FILE_BUTTON,
		hinst,
		evwin_ref);

	memcpy(&d_position, &pos, sizeof(RECT));
}

void 
EvWindow::EvButton::set_position(
	RECT& new_position)
{
	memcpy(&d_position, &new_position, sizeof(RECT));
	
	BOOL windowpos_res = FALSE;

	windowpos_res = SetWindowPos(
		d_button_hwnd, 
		HWND_TOP, 
		5, 
		new_position.bottom - 55, 
		0, 0, 
		SWP_ASYNCWINDOWPOS | SWP_NOSIZE);
}


EvWindow::EvBitmap::EvBitmap(
	const std::string bitmap_path)
{

}

EvWindow::EvBitmap::EvBitmap(LONG height, LONG width)
{
	
	WORD bitmap_data[4] = { 0x00FF, 0x00FF, 0x00FF, 0x0F0F };

	d_hbitmap = CreateBitmap(width, height, 2,16, (void*)bitmap_data);
	
	int test = 33;
}

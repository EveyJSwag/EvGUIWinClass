#include "EvWindow.h"
#include "MemoryManager.h"
#include <stdlib.h>
#include <string.h>
#include <sstream>

static BOOL redraw_time = FALSE;

static std::string app_title = "NEOGEO Color Reader";

callback_structure EvWindow::overlord_callback_array[3];

//
// EvWindow
//

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

	RECT file_button_pos = out_rect;
	RECT test_button_pos = out_rect;
	file_button_pos.left = out_rect.left + 5;
	file_button_pos.bottom = out_rect.bottom - 55;

	file_select_button = new EvButton(
		d_hwnd, 
		hInstance, 
		file_button_pos,
		EvWindow::FILE_BUTTON,
		"Select File",
		this);

	file_select_button->add_callback(file_select_callback);

	test_button_pos.left = out_rect.right - 100;
	test_button_pos.bottom = out_rect.bottom - 55;

	EvErrorMsgArea::set_error_message("OH BOY", d_hwnd);

	stupid_test_button = new EvButton(
		d_hwnd,
		hInstance,
		test_button_pos,
		EvWindow::DUMB_BUTTON,
		"Test Button",
		this);

	stupid_test_button->add_callback(stupid_test_callback);

	edit_control = new EvEditControl(
		d_hwnd,
		hInstance,
		out_rect,
		EvWindow::MEMORY_ENTRY,
		this);

	// left
	// top
	// right
	// bottom
	RECT init_pos = {200, 100, 400, 200};
	COLORREF init_color = RGB(200, 0, 0);

	error_message_area = new EvErrorMsgArea(
		init_pos, 
		init_color);
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

void EvWindow::file_select_callback(void* object_reference1, void* object_reference2)
{
	EvWindow::EvButton* button_reference = (EvWindow::EvButton*)object_reference1;
	EvWindow* window_reference = (EvWindow*)object_reference2;
	
	window_reference->file_dialog_window = new EvFileDialog();
	EvErrorMsgArea::set_error_message("Clicked File Button", window_reference->GetHwnd());
	window_reference->file_dialog_window->show();
}

void EvWindow::stupid_test_callback(void* object_reference1, void* object_reference2)
{
	EvWindow::EvButton* button_reference = (EvWindow::EvButton*)object_reference1;
	EvWindow* window_reference = (EvWindow*)object_reference2;
	window_reference->edit_control->get_entry_text();
	EvErrorMsgArea::set_error_message("LOL", window_reference->GetHwnd());
}

void EvWindow::call_overlord_function(app_widgets id_index)
{
	callback_structure callback_entry = overlord_callback_array[id_index-15];
	if (callback_entry.callback)
	{
		void* argument1 = callback_entry.argument1;
		void* argument2 = callback_entry.argument2;

		callback_entry.callback(argument1, argument2);
	}
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
	HBITMAP hbmp;

	unsigned short lo_param = LOWORD(wParam);
	unsigned short hi_param = HIWORD(wParam);

	HGDIOBJ s_obj;

	LPSTR lpMsgBuf;

	switch (message)
	{
	
	case WM_PAINT:

		hdc = BeginPaint(hWnd, &ps);

		HFONT old_font;

		old_font = {0};

		GetClientRect(
			hWnd, 
			&out_rect);

		error_message_area->display(hdc);

		LPSTR test_msg;
		test_msg = (char*)"test";

		error_message_area->display_message();

		COORD title_pos;
		title_pos.X = out_rect.top;
		title_pos.Y = 
			(out_rect.right / 2) - app_title.size() - 30;

		TextOut(
			hdc,
			title_pos.Y, 
			title_pos.X,
			app_title.c_str(), 
			app_title.size());

		file_select_button->set_position(out_rect);
		
		 //edit_control->set_position(out_rect);
		
		EndPaint(hWnd, &ps);

		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		break;

	case WM_COMMAND:
		
		if (hi_param == BN_CLICKED)
		{
			EvWindow::call_overlord_function((app_widgets)lo_param);
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

// ****************
// * BUTTON CLASS *
// ****************
EvWindow::EvButton::EvButton(
	HWND parent_handle, 
	HINSTANCE& hinst, 
	RECT& pos, 
	app_widgets id,
	const char* button_label,
	void* extra_data)
{

	EvWindow* evwin_ref = (EvWindow*)extra_data;

	d_button_hwnd = CreateWindow(
		"BUTTON",
		button_label,
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHLIKE | BS_NOTIFY,
		pos.left + 5,
		pos.bottom - 55,
		100,
		50,
		parent_handle,
		(HMENU)id,
		hinst,
		evwin_ref);

	d_id = id;
	d_extra_data = extra_data;
	memcpy(&d_position, &pos, sizeof(RECT));
}


void EvWindow::EvButton::add_callback(
	callback_procedure* callback)
{
	callback_structure callback_entry;
	callback_entry.callback = callback;
	callback_entry.argument1 = this;
	callback_entry.argument2 = d_extra_data;
	overlord_callback_array[d_id - 15] = callback_entry;
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
// ********************
// ********************
// ********************


// ****************
// * EDIT CONTROL *
// ****************
EvWindow::EvEditControl::EvEditControl(
	HWND parent_handle,
	HINSTANCE& hinst,
	RECT& pos,
	app_widgets id,
	void* extra_data)
{
	EvWindow* evwin_ref = (EvWindow*)extra_data;
	
	d_econtrol_hwnd = CreateWindow(
		"EDIT",
		NULL,
		WS_BORDER | WS_CHILD | WS_VISIBLE| ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL,
		pos.left + 5,
		pos.bottom - 55,
		100,
		50,
		parent_handle,
		(HMENU)EvWindow::app_widgets::MEMORY_ENTRY,
		hinst,
		evwin_ref);

	memcpy(&d_position, &pos, sizeof(RECT));

	d_extra_data = extra_data;
	d_id = id;

}

void
EvWindow::EvEditControl::set_position(
	RECT& new_position)
{
	memcpy(&d_position, &new_position, sizeof(RECT));

	BOOL windowpos_res = FALSE;

	windowpos_res = SetWindowPos(
		d_econtrol_hwnd,
		HWND_TOP,
		5,
		new_position.bottom - 20,
		20, 20,
		SWP_ASYNCWINDOWPOS | SWP_NOSIZE);
}

void EvWindow::EvEditControl::add_callback(
	callback_procedure* callback)
{
	callback_structure callback_entry;
	callback_entry.callback = callback;
	callback_entry.argument1 = this;
	callback_entry.argument2 = d_extra_data;
	overlord_callback_array[d_id - 15] = callback_entry;
}

std::string EvWindow::EvEditControl::get_entry_text()
{
	char current_text[100];
	TCHAR test[] = "IM DUMB";
	SendMessage(d_econtrol_hwnd, WM_GETTEXT, 100, (LPARAM)current_text);
	d_entry_text = std::string((char*)current_text);
	return d_entry_text;
}

// ****************
// ****************
// ****************

// **********************
// * ERROR MESSAGE AREA *
// **********************

LPSTR EvWindow::EvErrorMsgArea::ds_error_message = (char *)"";

void EvWindow::EvErrorMsgArea::set_error_message(
	const char* message_txt, 
	HWND hWnd)
{
	RECT rect_to_update;
	GetClientRect(hWnd, &rect_to_update);
	InvalidateRect(hWnd, &rect_to_update, FALSE);
	ds_error_message = (char *)message_txt;
	UpdateWindow(hWnd);
	ValidateRect(hWnd, &rect_to_update);
}

EvWindow::EvErrorMsgArea::EvErrorMsgArea(
	RECT& pos, 
	COLORREF& init_color)
{
	memcpy(&d_sizepos, &pos, sizeof(RECT));
	
	d_region = CreateRectRgnIndirect(&d_sizepos);
	
	d_bkg_color = init_color;

	d_fg_color = RGB(0, 0, 0);

	d_brush = CreateSolidBrush(d_bkg_color);

	d_message = (LPSTR)"";
}

void EvWindow::EvErrorMsgArea::display( 
	HDC& hdc)
{
	d_hdc_ref = hdc;

	repaint();

	SetBkColor(d_hdc_ref, RGB(200, 0, 0));

	LPRECT rect_to_use;
	rect_to_use = (RECT*)malloc(sizeof(RECT));

	DrawText(
		d_hdc_ref,
		"ERROR MESSAGES: ",
		16,
		&d_sizepos,
		DT_BOTTOM);

	SetBkColor(d_hdc_ref, RGB(255, 255, 255));
}

void EvWindow::EvErrorMsgArea::set_size(RECT& pos)
{
	memcpy(&d_sizepos, &pos, sizeof(RECT));

	d_region = CreateRectRgnIndirect(&d_sizepos);

	d_brush = CreateSolidBrush(d_bkg_color);

	repaint();
}

void EvWindow::EvErrorMsgArea::color_bkg(COLORREF& new_color)
{
	d_bkg_color = new_color;
	d_brush = CreateSolidBrush(d_bkg_color);

	repaint();
}

void EvWindow::EvErrorMsgArea::display_message()
{
	RECT message_pos = d_sizepos;

	message_pos.top += 15;
	message_pos.bottom += 15;
	SetBkColor(d_hdc_ref, RGB(200, 0, 0));
	DrawText(
		d_hdc_ref,
		ds_error_message,
		lstrlen(ds_error_message),
		&message_pos,
		DT_BOTTOM);

	SetBkColor(d_hdc_ref, RGB(255, 255, 255));
}

void EvWindow::EvErrorMsgArea::repaint()
{
	FillRgn(d_hdc_ref, d_region, d_brush);
}

// ****************
// ****************
// ****************
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

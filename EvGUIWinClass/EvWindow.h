#pragma once
#include <windows.h>
#include <exception>
#include <string>
#include <ShObjIdl.h>

#define DEFAULT_STYLE	(CS_HREDRAW | CS_VREDRAW);

class EvWindow
{

public:

	// Default Constructor:
	// Uses a 'default' version of WndProc
	EvWindow(HINSTANCE hInstance);

	// Constructor with custom version of WndProc
	EvWindow(HINSTANCE hInstance, WNDPROC a_wndproc);

	// Wrapper for show windows function
	void EvShowWindow(int& a_cmdshow);

	void EvMessageLoop();

	HWND GetHwnd();

	enum app_events
	{
		FILE_OPEN = 1,
	};

	enum app_widgets
	{
		FILE_BUTTON = 15,
	};

	class EvWindowException : std::exception
	{

	public:
		EvWindowException(
			const std::string& rsn, 
			const unsigned int& ln, 
			const std::string& f);

		std::string& GetReason();

	private:
		std::string reason;

	};

	class EvFileDialog
	{
	public:
		EvFileDialog();

		~EvFileDialog();

		void show();

	private:
		IFileOpenDialog* d_file_open_interface;

		IShellItem* d_shell_item;

		static unsigned int instance_amount;
	};

	class EvButton 
	{
	public:
		EvButton(
			HWND parent_handle, 
			HINSTANCE& hinst,
			RECT& pos,
			void* extra_data);

		HWND get_handle(){return d_button_hwnd;}

		RECT get_position(){return d_position;}

		void set_position(RECT& new_position);

	private:
		HWND d_button_hwnd;
		RECT d_position;
	};

	class EvBitmap
	{

	public:
		EvBitmap(const std::string bitmap_path);
		EvBitmap(LONG height, LONG width);

		BITMAP* get_bitmap() { return d_bitmap; }
		HBITMAP get_hbitmap() { return d_hbitmap; }

	private:
		BITMAP* d_bitmap;
		HBITMAP d_hbitmap;
	};

private:

	WNDCLASSEX wcex;

	HINSTANCE d_hinstance;

	HWND d_hwnd;

	EvButton* file_select_button;
	EvFileDialog* file_dialog_window;
	EvBitmap* bitmap;

	const char* name = "HAHAHAHA";

	static LRESULT CALLBACK d_SetupCB(
		HWND hWnd,
		UINT message,
		WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK d_InterCB(
		HWND hWnd,
		UINT message,
		WPARAM wParam, LPARAM lParam);

	LRESULT CALLBACK d_WndProc(
		HWND hWnd, 
		UINT message, 
		WPARAM wParam, LPARAM lParam);


};
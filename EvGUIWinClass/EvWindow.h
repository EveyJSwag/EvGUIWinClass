#pragma once
#include <windows.h>
#include <exception>
#include <string>
#include <ShObjIdl.h>
#include "EvGUITypes.h"

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
		MEMORY_ENTRY,
		DUMB_BUTTON,
		WRITE_MEMORY_BUTTON
	};

	static callback_structure overlord_callback_array[4];

	static void call_overlord_function(app_widgets id_index);

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
			app_widgets id,
			const char* button_label,
			void* extra_data);

		HWND get_handle(){return d_button_hwnd;}

		RECT get_position(){return d_position;}

		void set_position(RECT& new_position);

		void add_callback(callback_procedure* callback);

	private:
		HWND d_button_hwnd;
		RECT d_position;
		app_widgets d_id;
		void* d_extra_data;
	};

	class EvEditControl
	{

	public:
		EvEditControl(
			HWND parent_handle,
			HINSTANCE& hinst,
			RECT& pos,
			app_widgets id,
			void* extra_data);

		HWND get_handle() { return d_econtrol_hwnd; }

		RECT get_position() { return d_position; }

		std::string get_entry_text();

		void add_callback(callback_procedure* callback);

		void set_position(RECT& new_position);

	private:
		HWND d_econtrol_hwnd;
		RECT d_position;
		void* d_extra_data;
		app_widgets d_id;
		std::string d_entry_text;

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

	class EvErrorMsgArea
	{
	public:
		EvErrorMsgArea(RECT& pos, COLORREF& init_color);

		void set_size(RECT& pos);

		void display_message();

		void color_bkg(COLORREF& new_color);

		void display(HDC& hdc);

		RECT get_size() { return d_sizepos; }

		COLORREF get_bkg_color() { return d_bkg_color; }
		COLORREF get_fg_color() { return d_fg_color; }

		static void set_error_message(const char* message_txt, HWND hWnd);

	private:
		
		RECT     d_sizepos;
		HBRUSH   d_brush;
		HRGN     d_region;
		LPSTR    d_message;
		COLORREF d_bkg_color;
		COLORREF d_fg_color;
		HDC      d_hdc_ref;

		//
		// Make it static so it can be set from anywhere...
		//
		static LPSTR ds_error_message;

		void repaint();
	};


private:

	WNDCLASSEX wcex;

	HINSTANCE d_hinstance;

	HWND d_hwnd;

	EvButton* file_select_button;
	EvButton* stupid_test_button;

	static void file_select_callback(void* object_reference1, void* object_reference2);

	static void stupid_test_callback(void* object_reference1, void* object_reference2);

	static void write_memory_callback(void* object_reference1, void* object_reference2);

	EvButton* write_memory_button;

	EvEditControl* edit_control;
	EvFileDialog* file_dialog_window;
	EvBitmap* bitmap;
	EvErrorMsgArea* error_message_area;

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

	void paint_widgets(HWND hWnd);
};

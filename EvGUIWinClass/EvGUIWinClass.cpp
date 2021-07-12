#include <windows.h>
#include <string.h>
#include "EvWindow.h"

static HINSTANCE my_hinstance;

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	try
	{
		my_hinstance = hInstance;
		EvWindow* main_window = new EvWindow(hInstance);

		main_window->EvShowWindow(nCmdShow);

		main_window->EvMessageLoop();

	}
	catch (EvWindow::EvWindowException& excp)
	{
		MessageBox(
			NULL,
			excp.GetReason().c_str(),
			"FAILED",
			NULL);
	}
}
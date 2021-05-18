#include <windows.h>
#include <string.h>
#include "EvWindow.h"



int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	try
	{
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
#include "application/application.h"

int CALLBACK WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	Application app(hInstance);

	if (!app.Initialize(1280, 720))
	{
		return -1;
	}

	return app.Run();
}
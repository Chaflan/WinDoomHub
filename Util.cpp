#include "Util.h"
#include <windows.h>

// TODO: Remove
#include <iostream>

void Util::CreateProcessWrap(const std::string& program, const std::string& args)
{
    // Arguments need to be char* (not const char*) and with a preceeding space
//    char argsCStr[512];
//    if (args.empty()) {
//        strcpy_s(argsCStr, 512, args.c_str());
//    } else {
//        argsCStr[0] = ' ';
//        strcpy_s(argsCStr + 1, 512, args.c_str());
//    }

    const std::string dickHoleSurprise = program + (args.empty() || program.empty() ? "" : " ") + args;
    char argsCStr[512];
    strcpy_s(argsCStr, 512, dickHoleSurprise.c_str());
    std::cout << "argsCStr=" << argsCStr << std::endl << std::endl;

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    // set the size of the structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // start the program up
    CreateProcessA
    (
        //program.c_str(),    // the path to executable
        NULL,
        argsCStr,           // arguments, or full command
        NULL,               // Process handle not inheritable
        NULL,               // Thread handle not inheritable
        FALSE,              // Set handle inheritance to FALSE
        CREATE_NEW_CONSOLE, // Opens file in a separate console
        NULL,               // Use parent's environment block
        NULL,               // Use parent's starting directory
        &si,                // Pointer to STARTUPINFO structure
        &pi                 // Pointer to PROCESS_INFORMATION structure
    );

    // Close process and thread handles.
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
 
using namespace std;
 
char fileToInject[250] = "C:\\Slrgrbrgr.dll";
char processName[] = "csgo.exe";
 
typedef HINSTANCE (*fpLoadLibrary)(char*);
 
// Inject DLL
bool injectDll(DWORD processId) {
    HINSTANCE hDll = LoadLibrary("KERNEL32");
 
    fpLoadLibrary LoadLibraryAddr = (fpLoadLibrary)GetProcAddress(hDll, "LoadLibraryA");
    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, false, processId);
 
    LPVOID paramAddr = VirtualAllocEx(hProc, 0, strlen(fileToInject) + 1, MEM_COMMIT, PAGE_READWRITE);
    int memoryWritten = WriteProcessMemory(hProc, paramAddr, fileToInject, strlen(fileToInject) + 1, NULL);
 
    // -----------------------
    FILE * pFile;
    fopen_s(&pFile, fileToInject, "r");
    
    cout << errno << endl;
    // -----------------------
    
    HANDLE hRemoteThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryAddr, paramAddr, 0, 0);
    CloseHandle(hProc);
 
    return memoryWritten != 0;
}
 
// Entry function
int main()
{
    DWORD processId = NULL;
 
    PROCESSENTRY32 pe32 = { sizeof(PROCESSENTRY32) };
    HANDLE hProcSnap;
 
    // Look for right process
    while (!processId) {
        system("CLS");
        cout << "Looking for " << processName << "..." << endl;
 
        hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
 
        // Search in all processes for ProcessName[] and get id
        if (Process32First(hProcSnap, &pe32)) {
            while (Process32Next(hProcSnap, &pe32)) {
                if (!strcmp(pe32.szExeFile, processName)) {
                    processId = pe32.th32ProcessID;
                }
            }
        }
 
        Sleep(1000);
    }
 
    while (!injectDll(processId)) {
        system("CLS");
        cout << "DLL failed to inject" << endl;
        Sleep(1000);
    }
 
    cout << "DLL injected sucessfuly!" << endl << endl;
    cout << "Closing in 5s...";
 
    CloseHandle(hProcSnap);
    Sleep(5000);
 
    return 1;
}

#include <Windows.h>
#include <QtWidgets/QApplication>
#include "ClientCore.h"

int main(int argc, char *argv[]) {
    // »¥³âËø
    HANDLE m_hMutex = CreateMutex(NULL, FALSE, L"RGBLightClient");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        CloseHandle(m_hMutex);
        m_hMutex = NULL;
        return 0;
    }

    QApplication a(argc, argv);
    a.setQuitOnLastWindowClosed(false);
    ClientCore core;

    return a.exec();
}

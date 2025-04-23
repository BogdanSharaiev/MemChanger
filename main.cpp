#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QDebug>
#include <fstream>
#include <windows.h>
#include <tlhelp32.h>
#include <QComboBox>
#include <string>

std::vector<uintptr_t> addrs;
std::vector<uintptr_t> result;
std::vector<uintptr_t> foundAddresses;

void FillComboBox(QComboBox* combobox) {
    HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 proc;
    proc.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hsnap, &proc)) return;

    do {
        DWORD pid = proc.th32ProcessID;
        HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
        DWORD gui = GetGuiResources(process, GR_GDIOBJECTS);
        CloseHandle(process);

        QString processName = QString::fromWCharArray(proc.szExeFile);
        if (gui >= 10) {
            combobox->addItem(processName + " pid: " + QString::number(pid));
        }
    } while (Process32Next(hsnap, &proc));
    CloseHandle(hsnap);
}


DWORD getPid(QComboBox* combo) {
    QString text = combo->currentText();
    QString res = "";
    for (int i = 0; i < text.length(); i++) {
        if (text[i] == ":") {
            i++;
            for(int j = i;j<text.length();j++){
                res+=text[j];
            }
        }
    }
    return res.toUInt();
}

void PlaySound(const std::string& path) {
    PlaySoundA(path.c_str(), NULL, SND_FILENAME | SND_ASYNC);
}

void scanMemory(DWORD pid, int val) {
    addrs.clear();
    HANDLE proc = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid);


    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    MEMORY_BASIC_INFORMATION mbi;
    uintptr_t baseAddr = (uintptr_t)sysInfo.lpMinimumApplicationAddress;
    uintptr_t maxAddr = (uintptr_t)sysInfo.lpMaximumApplicationAddress;

    while (baseAddr < maxAddr) {
        if (VirtualQueryEx(proc, (LPCVOID)baseAddr, &mbi, sizeof(mbi))) {
            if (mbi.State == MEM_COMMIT &&
                !(mbi.Protect & PAGE_GUARD) &&
                mbi.Protect != PAGE_NOACCESS) {

                SIZE_T numInts = mbi.RegionSize / sizeof(int);
                std::vector<int> buffer(numInts);
                SIZE_T bytesRead = 0;

                if (ReadProcessMemory(proc, (LPCVOID)baseAddr, buffer.data(), numInts * sizeof(int), &bytesRead) && bytesRead > 0) {
                    SIZE_T actualInts = bytesRead / sizeof(int);
                    for (SIZE_T i = 0; i < actualInts; i++) {
                        if (buffer[i] == val) {
                            addrs.push_back(baseAddr + i * sizeof(int)); // Сохраняем найденные адреса
                        }
                    }
                }
            }
        }
        baseAddr += mbi.RegionSize;
    }

    CloseHandle(proc);
}

void filterAddresses(DWORD pid, int newVal) {
    result.clear(); // Очищаем старые результаты

    HANDLE proc = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid); // Используем только чтение
    if (!proc) return;

    for (const auto& addr : addrs) {
        int value = 0;
        SIZE_T size = 0;
        if (ReadProcessMemory(proc, (LPCVOID)addr, &value, sizeof(value), &size) && size > 0) {
            if (value == newVal) { // Если значение на этом адресе совпадает с новым
                result.push_back(addr); // Добавляем адрес в результаты
            }
        }
    }

    CloseHandle(proc);
}

void writeToAddress(DWORD pid, int newVal, uintptr_t addr) {
    HANDLE proc = OpenProcess(PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pid);
    if (!proc) return;

    SIZE_T written = 0;
    BOOL success = WriteProcessMemory(proc, (LPVOID)addr, &newVal, sizeof(newVal), &written);

    if (success && written == sizeof(newVal)) {
        qDebug() << "Memory written successfully!";
    } else {
        qDebug() << "Failed to write to memory.";
    }

    CloseHandle(proc);
}


int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    MainWindow w;

    FillComboBox(w.getComboBox());

    w.show();

    QDir::setCurrent(QCoreApplication::applicationDirPath());
    qDebug() << "Current Path after setting:" << QDir::currentPath();

    std::string path = "Media/pudge.wav";
    if (!std::ifstream(path)) {
        qDebug() << "File not found!";
    } else {
        PlaySound(path);
    }

    return a.exec();
}

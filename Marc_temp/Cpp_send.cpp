#include <windows.h>
#include <iostream>
#include <cstring>
using namespace std;

#define BUF_SIZE 4096

int main()
{
    // 定义共享数据
    char szBuffer[8];
    for (char & i : szBuffer) {
        i='0';
    }
    char* a;


    // 创建共享文件句柄
    HANDLE hMapFile = CreateFileMapping(
            INVALID_HANDLE_VALUE,   // 物理文件句柄
            NULL,   // 默认安全级别
            PAGE_READWRITE,   // 可读可写
            0,   // 高位文件大小
            BUF_SIZE,   // 低位文件大小
            "S_ME"   // 共享内存名称
    );

    // 映射缓存区视图 , 得到指向共享内存的指针
    LPVOID lpBase = MapViewOfFile(
            hMapFile,            // 共享内存的句柄
            FILE_MAP_ALL_ACCESS, // 可读写许可
            0,
            0,
            BUF_SIZE
    );
    a = (char*)lpBase;
    // 将数据拷贝到共享内存
    //strcpy((UINT32*)lpBase, szBuffer);
    *a = '0';
    while(true){
        if(*a == '0'){
            cin.getline(&szBuffer[1], sizeof(szBuffer)- sizeof(char));
            szBuffer[0]='1';
            memcpy((char *)lpBase, szBuffer,sizeof(szBuffer));
            if(strcmp(&szBuffer[1], "end") == 0){
                Sleep(1000);
                break;
                }
            for (char & i : szBuffer) {
                i='0';
            }
            Sleep(100);
        }

    }

    // 解除文件映射
    UnmapViewOfFile(lpBase);
    // 关闭内存映射文件对象句柄
    CloseHandle(hMapFile);
    return 0;
}

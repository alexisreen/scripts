#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>
#include <winbase.h>
int lines = 0;
int lineCounter(const char* path)
{
    OFSTRUCT ofs = {0};
    ofs.cBytes = sizeof(ofs);
    HFILE file_open = OpenFile(path, &ofs, OF_READ); 
    char data[1024] = {0};
    DWORD size = 0;
    if(file_open != HFILE_ERROR)
    {
        while(ReadFile((HANDLE)file_open, data, sizeof(data), &size, NULL))
        {
            for(DWORD i = 0; i <= size; ++i)
            {
                if(data[i] == '\n')
                    ++lines;
            }
            if(size < sizeof(data))
                break;
        }
        return lines+1;
    }  
    else
        return 0;  
}

#define ROT13_KEY 13

void EncodeDecodeText(char cipher[]) 
{
    for(; *cipher != '\0'; ++cipher)
    {
        if(islower(*cipher)) *cipher = 'a' + (*cipher - 'a' + ROT13_KEY) % 26;
        else if(isupper(*cipher)) *cipher = 'A' + (*cipher - 'A' + ROT13_KEY) % 26;
    }
}

int EncodeFile(const char* path, const char* file_name, char data[])
{
    DWORD size = 0, _size = 0;
    int i = 0, key = lineCounter(path);
    HANDLE file_open = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE file_create = CreateFile(file_name, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if((file_open != INVALID_HANDLE_VALUE) && (file_create != INVALID_HANDLE_VALUE))
    {
        while(ReadFile(file_open, data, 1024, &size, NULL))
        {
            EncodeDecodeText(data);
            if(WriteFile(file_create, data, 1024, &_size, NULL) != TRUE)
                break;
            if(size < 1024)
                break;
        }
        return 0;
    }
    return GetLastError();
}

int DecodeFile(const char* path, const char* file_name, char data[])
{
    DWORD size = 0, _size = 0, error = 0;
    int i = 0, key = lineCounter(path);
    HANDLE file_open = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    HANDLE file_create = CreateFile(file_name, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if((file_open != INVALID_HANDLE_VALUE) && (file_create != INVALID_HANDLE_VALUE))
    {
        while(ReadFile(file_open, data, 1024, &size, NULL))
        {
            EncodeDecodeText(data);
            if(WriteFile(file_create, data, 1024, &_size, NULL) != TRUE)
                return GetLastError();
            if(size < 1024)
                break;
        }
    }
    return GetLastError();
}

int main(int argc, char** argv)
{
    int choice = 0;
    char str[1024] = {0};
    printf("Available Operation:\n1) Count File Lines\n2) Encode/Decode File\n3) Show Usage\n");
    printf("Enter an operation: ");
    scanf("%i", &choice);
    if((choice == 1) && (argc == 2))
        printf("Lines: %d\n", lineCounter(argv[1]));
    else if((choice == 2) && (argc == 3))
        return DecodeFile(argv[1], argv[2], str);
    else if(choice == 3)
    {
        fprintf(stderr, "Please open this file in command line (cmd)!\n");
        fprintf(stderr, "Usage: %s <input> <output>\n", argv[0]);
    }
    else
        fprintf(stderr, "Unavailable Operation ID\n");
    
    return 1;
}
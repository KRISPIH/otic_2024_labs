#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstring>
#include <vector>
using namespace std;

const char signatur[] = {0X11, 0X22, 0X33, 0X44, 0X55, 0X66};
const char version[2][2] = {{0X00, 0X00},{0X00,0X01}};
const char codeCompres[1][3] = {{0X00, 0X00, 0X00}};

void codec(char** filesOpen, char *fileWrite,unsigned short countFiles)
{
    FILE *oticFile = fopen(fileWrite, "w+b");
    fwrite(signatur, 1, 6, oticFile);
    fwrite(version, 1, 2, oticFile);
    fwrite(codeCompres[0], 1, 3, oticFile); 
    fwrite(&countFiles, 1, sizeof countFiles, oticFile);  
    for (int i = 0;i<countFiles;i++){
        FILE *ptrFile = fopen(filesOpen[i],"rb");
        if (!ptrFile){
            fputs("File not exist", stderr);
            exit(1);
        }

        fseek(ptrFile, 0, SEEK_END);
        long lSize = ftell(ptrFile);
        rewind(ptrFile);

        char *buffer = (char *)malloc(sizeof(char) * lSize);
        if (buffer == NULL){
            fputs("Memory fall", stderr);
            exit(2);
        }

        size_t FileLength = fread(buffer, 1, lSize, ptrFile);
        fclose(ptrFile);
        fwrite(&FileLength, sizeof FileLength, 1, oticFile);
        char PathLen = strlen(filesOpen[i]);
        fwrite(&PathLen, 1, 1, oticFile);
        fwrite(filesOpen[i],1,PathLen,oticFile);
        fwrite(buffer, 1, FileLength, oticFile);
        free(buffer);
    }
    fclose(oticFile);
}

void decodec0(FILE* fileOpen, long lSize){
    
    char d_count[8];
    char *data = (char *)malloc(sizeof(char) * (lSize));
    fread(d_count, 1, 8, fileOpen);
    fread(data, 1, lSize - 16, fileOpen);

    FILE *res = fopen("OTIC.bin", "w+b");
    fwrite(data, sizeof(char), lSize - 16, res);
    fclose(res);
    free(data);
}
void decodec1(FILE* OpenFile){
    char d_codes[3];
    unsigned short countFiles;
    unsigned long long lenFile;
    char lenPath;
    char* data;
    char* path;

    fread(d_codes,1,3,OpenFile);
    for (int i=0;i<3;i++){
        if (d_codes[i]!= codeCompres[0][i]){
            printf("The code not exist!");
            exit(5);
        }
    }

    fread(&countFiles,1,2,OpenFile);
    for (int i=0;i<countFiles;i++){
        fread(&lenFile,1,8,OpenFile);
        fread(&lenPath,1,1,OpenFile);
        path = new char[lenPath+1];
        path[lenPath] = '\0';
        data = new char[lenFile];
        fread(path,1,lenPath,OpenFile);
        fread(data,1,lenFile,OpenFile);
        FILE* ptrFile = fopen(path,"wb");
        fwrite(data,1,lenFile,ptrFile);
        fclose(ptrFile);
        delete[] path;
        delete[] data;

    }
}

void GetSettings(char *fileOpen)
{   
    char* ergegrer;
    char d_signature[6];
    char d_version[2];
    FILE *OpenFile = fopen(fileOpen, "r+b");
    if (!OpenFile)
    {
        fputs("File not exist", stderr);
        exit(1);
    }


    fseek(OpenFile, 0, SEEK_END);
    long lSize = ftell(OpenFile);
    rewind(OpenFile);

    fread(d_signature, 1, 6, OpenFile);
    fread(d_version, 1, 2, OpenFile);


    for (int i = 0; i < 6; i++)
    {
        if (d_signature[i] != signatur[i])
        {
            printf("Matching signaturs error!");
            exit(3);
        }
    }

    if ((d_version[0] == version[0][0]) && (d_version[1] == version[0][1])) decodec0(OpenFile,lSize);
    else if ((d_version[0] == version[1][0]) && (d_version[1] == version[1][1])) decodec1(OpenFile);
    else {
            printf("Matching versions error!");
            exit(4);
    }

    fclose(OpenFile);
}
int main()
{
    int n,countfile;
    char** filesOpen;
    char archive[0XFF];
    while (true)
    {
        printf("Codec programm\n"
               "1 - coder\n"
               "2 - decodec\n"
               "3 - info\n"
               "4 - exit\n");
        scanf("%d", &n);

        switch (n)
        {
        case (1):
            printf("Сколько файлав записать в архив?\n");
            scanf("%d",&countfile);
            filesOpen = new char*[countfile];
            for (int i =0;i<countfile;i++){
                filesOpen[i] = new char[0XFF];
                printf("Open file: ");
                scanf("%s", filesOpen[i]);
            }

            printf("Write file: ");
            scanf("%s", archive);

            codec(filesOpen, archive,countfile);
            for (int i = 0; i < countfile; i++) delete [] filesOpen[i];
                delete [] filesOpen;
            break;
        case (2):

            printf("Open archive: ");
            scanf("%s", archive);
            GetSettings(archive);
            break;
        case (3):
            printf("Путь к файлу записывается относительно папки с кодом программы\n"
            "Если файл находится на другом диске допускается запись полного пути к файлу\n");
            break;
        case (4):
            return 0;

        default:
            printf("Error\n\n");
            break;
        }
    }
}
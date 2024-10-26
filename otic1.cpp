#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
using namespace std;

const char signatur[] = {0X11, 0X22, 0X33, 0X44, 0X55, 0X66};
const char version[] = {0X00, 0X00};

void codec(char *fileOpen, char *fileWrite)
{

    FILE *ptrFile = fopen(fileOpen, "rb");
    if (!ptrFile)
    {
        fputs("File not exist", stderr);
        exit(1);
    }
    fseek(ptrFile, 0, SEEK_END);
    long lSize = ftell(ptrFile);
    rewind(ptrFile);

    char *buffer = (char *)malloc(sizeof(char) * lSize);
    if (buffer == NULL)
    {
        fputs("Memory fall", stderr);
        exit(2);
    }
    size_t FileLength = fread(buffer, 1, lSize, ptrFile);
    fclose(ptrFile);

    FILE *ptrFile2 = fopen(fileWrite, "w+b");
    fwrite(signatur, 1, 6, ptrFile);
    fwrite(version, 1, 2, ptrFile);
    fwrite(&FileLength, sizeof FileLength, 1, ptrFile);
    fwrite(buffer, 1, FileLength, ptrFile);
    fclose(ptrFile);
    free(buffer);
}

void decodec(char *fileOpen, char *fileWrite)
{
    FILE *ptrFile = fopen(fileOpen, "r+b");
    if (!ptrFile)
    {
        fputs("File not exist", stderr);
        exit(1);
    }
    char d_signature[6];
    char d_version[2];
    char d_count[8];

    fseek(ptrFile, 0, SEEK_END);
    long lSize = ftell(ptrFile);
    rewind(ptrFile);

    char *data = (char *)malloc(sizeof(char) * (lSize));
    fread(d_signature, 1, 6, ptrFile);
    fread(d_version, 1, 2, ptrFile);
    fread(d_count, 1, 8, ptrFile);
    fread(data, 1, lSize - 16, ptrFile);
    fclose(ptrFile);

    for (int i = 0; i < 6; i++)
    {
        if (d_signature[i] != signatur[i])
        {
            printf("Matching signaturs error!");
            exit(3);
        }
    }

    for (int i = 0; i < 2; i++)
    {
        if (d_version[i] != version[i])
        {
            printf("Matching versions error!");
            exit(4);
        }
    }

    FILE *res = fopen(fileWrite, "w+b");
    fwrite(data, sizeof(char), lSize - 16, res);
    fclose(res);
    free(data);
}

int main()
{
    int c;
    char fileOpen[100];
    char fileWrite[100];
    while (true)
    {
        printf("Codec programm\n"
               "1 - codec\n"
               "2 - decodec\n"
               "3 - exit\n");
        scanf("%d", &c);


        switch (c)
        {
            case (3):
                return 0;

        case (1):

            printf("Open file: ");
            scanf("%s", fileOpen);
            printf("Write file: ");
            scanf("%s", fileWrite);

            codec(fileOpen, fileWrite);
            break;
        case (2):

            printf("Open file: ");
            scanf("%s", fileOpen);
            printf("Write file: ");
            scanf("%s", fileWrite);

            decodec(fileOpen, fileWrite);
            break;

        default:
            printf("Error\n\n");
            break;
        }
    }
}
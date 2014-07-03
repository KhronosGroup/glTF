#include "KMZ2Collada.h"
#include "unzip.h"
#include "ioapi.h"

#include <direct.h>

using namespace std;

namespace GLTF{
    std::string Kmz2Collada(std::string strInputFile) {
        int iFileLength;
        unzFile zFile;
        unsigned int num = 512;
        unz_file_info64 zFileInfo;
        unz_global_info64 zGlobalInfo;
        char *pFileName = new char[num];
        std::string strDaePath;
        std::string strKmzPath;
        zFile = unzOpen64(strInputFile.c_str());

        if (NULL == zFile) {
            printf("Open Kmz file error\n");
            return "";
        }

        char *pTemporaryContent = new char[strInputFile.size()+1];
        std::string strFilePath;

        for (unsigned int ipos = 0; ipos < strInputFile.size(); ipos ++) {
            if (strInputFile[ipos] == '\\' || strInputFile[ipos] == '/') {
                pTemporaryContent[ipos] = '\0';
                    strFilePath = pTemporaryContent;
                    _mkdir(strFilePath.c_str());
            }
            pTemporaryContent[ipos] = strInputFile[ipos];
        }

        if (strFilePath != "")
            strFilePath += '\\';
        strInputFile = strFilePath;
        strKmzPath = strInputFile;

        delete[] pTemporaryContent;

        if (UNZ_OK != unzGetGlobalInfo64(zFile, &zGlobalInfo)) {
            printf("Parsing file error\n");
            return "";
        }

        for (int i = 0; i < zGlobalInfo.number_entry; i++) {
            if (UNZ_OK != unzGetCurrentFileInfo64(zFile, &zFileInfo, 
                pFileName, num, NULL, 0, NULL, 0)) {
                    printf("Read file error\n");
                    return "";
            }
            std::string strFilePath = pFileName;

            std::string strFolderPath = "";
            char *pFolderName = new char[strFilePath.size()];

            for (unsigned int i = 0; i < strFilePath.size(); i++) {
                if (strFilePath[i] == '\\' || strFilePath[i] == '/') {
                    pFolderName[i] = '\0';
                    strFolderPath = strKmzPath + pFolderName;
                    _mkdir(strFolderPath.c_str());
                }
                pFolderName[i] = strFilePath[i];
            }
            strFilePath = strKmzPath +strFilePath;
            delete[] pFolderName;
            if (strstr(strFilePath.c_str(), ".dae"))
                strDaePath = strFilePath;

            FILE *fp = fopen(strFilePath.c_str(), "wb");
            if (UNZ_OK != unzOpenCurrentFile(zFile)) {
                printf("Open current file error");
                return "";
            }

            iFileLength = (int)zFileInfo.uncompressed_size;
            char *pFileData = new char[iFileLength];
            int len = 1;
            while (len) {
                len = unzReadCurrentFile(zFile, (voidp)pFileData,
                    iFileLength - 1);
                pFileData[len] = '\0';

                if ( len <= 0) {
                    break;
                }
                fwrite(pFileData, 1, len, fp);
            }
            fclose(fp);
            delete[] pFileData;
            pFileData = NULL;

            unzGoToNextFile(zFile);
        }
        unzCloseCurrentFile(zFile);

        delete[] pFileName;
        pFileName = NULL;

        return strDaePath;
    }
}
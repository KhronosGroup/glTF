#ifndef __KMZ2COLLADA_H__
#define __KMZ2COLLADA_H__

#include <string>

namespace GLTF{
    class Kmz2Collada
    {
    public:
        Kmz2Collada();
        ~Kmz2Collada();

        /** get the kmz file path and set folder.*/
        void setFileFolder(std::string& strSrc);

        /** get kmz file path.*/
        void getFilePath(std::string& strSrc);

        /** copy function.*/
        std::string operator()(std::string strInputFile);

    private:
        std::string strDaePath;
        std::string strKmzPath;
    };
    
}
#endif
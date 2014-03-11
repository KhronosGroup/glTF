#ifndef __GLTF_ACCESSOR_CACHE__
#define __GLTF_ACCESSOR_CACHE__

#include "../GLTFOpenCOLLADA.h"

namespace GLTF
{
    class GLTFAccessorCache
    {
    public:
        GLTFAccessorCache(void *pData, size_t length);
        GLTFAccessorCache(const GLTFAccessorCache& rhs);
        virtual ~GLTFAccessorCache();
        bool operator<(const GLTFAccessorCache& rhs) const;
        size_t length() const;
    private:
        const GLTFAccessorCache& operator=(const GLTFAccessorCache& rhs);
        unsigned char *m_pData;
        size_t m_length;
    };
}
#endif
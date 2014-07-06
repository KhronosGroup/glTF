#include "GLTF.h"
#include "../GLTFOpenCOLLADA.h"
#include "GLTFAsset.h"
#include "GLTFAccessorCache.h"

namespace GLTF
{
    GLTFAccessorCache::GLTFAccessorCache(void *pData, size_t length) :
    m_length(length) {
        m_pData = new unsigned char[this->m_length];
        memcpy(m_pData, pData, m_length);
    }
    
    GLTFAccessorCache::GLTFAccessorCache(const GLTFAccessorCache& rhs) :
    m_length(rhs.length()) {
        m_pData = new unsigned char[m_length];
        memcpy(m_pData, rhs.m_pData, m_length);
    }

    size_t GLTFAccessorCache::length() const {
        return this->m_length;
    }

    GLTFAccessorCache::~GLTFAccessorCache() {
        delete[] this->m_pData;
    }
    
    bool GLTFAccessorCache::operator<(const GLTFAccessorCache& rhs) const {
        if (this->m_length != rhs.m_length) {
            return m_length < rhs.m_length;
        }
        
        if (this->m_pData == rhs.m_pData) {
            return false;
        }
        
        return memcmp(m_pData, rhs.m_pData, this->m_length) < 0;
    }
}

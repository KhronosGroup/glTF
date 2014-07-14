// Copyright (c) 2012, Motorola Mobility, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of the Motorola Mobility, Inc. nor the names of its
//    contributors may be used to endorse or promote products derived from this
//    software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef __GLTF_ACCESSOR_H__
#define __GLTF_ACCESSOR_H__

/* 
    A GLTFAccessor contains all the properties required to describe a buffer to be handled by WebGL (http://www.khronos.org/registry/webgl/specs/latest/ )
     
    Its design is very inspired by COLLADA (http://www.khronos.org/files/collada_spec_1_4.pdf )but was adapted for consistent naming and requirements (like byte sizes) to be consistent with typed arrays.
 
 */
namespace GLTF 
{
    /* this has not been updated to use GL Types */
    typedef void (*GLTFAccessorApplierFunc)(void* /* value */,
        const std::string& componentType /* componentType */,
        const std::string& type,
    
        size_t /* elementsPerValue */,
        size_t /* index */,
        size_t /* vertexAttributeByteSize*/,
        void* /* context */
    );
    
    class COLLADA2GLTF_EXPORT GLTFAccessor : public JSONObject {
    private:
        void _generateID();

    public:
        
        /* distinction between type and component type
         Note: in the API componentType is passed as BYTE until it is resolved as an int using the destination profile
            componentType :  [BYTE, UNSIGNED_BYTE, SHORT, UNSIGNED_SHORT, FLOAT], // integer (enum)
            type : ['SCALAR', 'VEC_2', 'VEC_3', 'VEC_4', 'MAT4x2', and son on....] // string
        */
        
        GLTFAccessor(std::shared_ptr<GLTFProfile>, const std::string& type, const std::string& componentType);
        GLTFAccessor(GLTFAccessor *);
        
        virtual ~GLTFAccessor();
        
        void setBufferView(std::shared_ptr <GLTFBufferView> buffer);
        std::shared_ptr <GLTFBufferView> getBufferView();
                
        void setByteStride(size_t stride);
        size_t getByteStride();
        
        size_t componentsPerElement();
        
        //as high level string
        std::string componentType();
        //as GL enum that comes from profile
        unsigned int GLComponentType();

        std::string type();
        
        void setByteOffset(size_t offset);
        size_t getByteOffset();
        
        void setCount(size_t length);
        size_t getCount();
        
        virtual void applyOnAccessor(GLTFAccessorApplierFunc applierFunc, void* context);
        
        const std::string& getID();
        
        size_t elementByteLength();
        
        std::shared_ptr<JSONArray> min();
        std::shared_ptr<JSONArray> max();
        
        bool matchesLayout(GLTFAccessor* meshAttribute);
        void exposeMinMax();
        
        virtual std::string valueType();

    private:
        void _computeMinMaxIfNeeded();
        
    private:
        std::shared_ptr <GLTFBufferView> _bufferView;
        size_t      _componentsPerElement;
        std::string _componentType;
        size_t      _elementByteLength;
        std::string _ID;
        bool        _minMaxDirty;
    };
}

#endif

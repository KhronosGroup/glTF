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

#ifndef __JSON_WRITER_H__
#define __JSON_WRITER_H__

namespace GLTF
{
    shared_ptr <JSONValue> serializeVec3(double x,double y, double z);
    shared_ptr <JSONValue> serializeVec4(double x,double y, double z, double w);
    shared_ptr <JSONObject> serializeBuffer(GLTFBuffer* buffer, void *context);
    shared_ptr <JSONObject> serializeBufferView(GLTFBufferView* bufferView, void *context);
    shared_ptr <JSONObject> serializeEffect(GLTFEffect* effect, void *context);
    shared_ptr <JSONObject> serializeMesh(GLTFMesh* mesh, void *context);
    shared_ptr <JSONObject> serializeMeshAttribute(GLTFMeshAttribute* meshAttribute, void *context);
    shared_ptr <JSONObject> serializeIndices(GLTFIndices* indices, void *context);
    shared_ptr <JSONObject> serializePrimitive(GLTFPrimitive* primitive, void *context);
    shared_ptr <JSONObject> serializeAnimation(GLTFAnimation* animation, void *context);
    shared_ptr <JSONObject> serializeSkin(GLTFSkin* skin);

    class GLTFWriter {
    private:
        
    public:        
        
        GLTFWriter(rapidjson::PrettyWriter <rapidjson::FileStream> *writer);
        GLTFWriter();
        virtual ~GLTFWriter();
        
        void setWriter(rapidjson::PrettyWriter <rapidjson::FileStream> *writer);
        rapidjson::PrettyWriter <rapidjson::FileStream>* getWriter();

        //base
        void writeArray(JSONArray* array, void *context);
        void writeObject(JSONObject* object, void *context);
        void writeNumber(JSONNumber* number, void *context);
        void writeString(JSONString* str, void *context);        
        void write(JSONValue* value, void *context);

    private:

        rapidjson::PrettyWriter <rapidjson::FileStream> *_writer;
    };

}


#endif
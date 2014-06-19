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

#ifndef __JSON_VALUE_H__
#define __JSON_VALUE_H__

namespace GLTF 
{    
    class JSONValue;
    typedef void (*JSONValueApplierFunc)(JSONValue*  , void* /*context*/);
    
    class GLTFWriter;
    class COLLADA2GLTF_EXPORT JSONValue {
    private:

    public:        
        JSONValue();
        virtual ~JSONValue();
        
        virtual void write(GLTFWriter* , void* context = 0);
        
        std::shared_ptr<JSONValue> valueForKeyPath(std::string keyPath);
        
        virtual void evaluate(void*);
        
        virtual JSONType getJSONType() = 0;
        
        virtual void apply(JSONValueApplierFunc, void* context);
        
        //consider overloading == later, but for now we are transitioning, so relying on isEqualTo implicitly provides more control over what/when comparaison are done
        virtual bool isEqualTo(JSONValue* value);
    private:
    };

}


#endif
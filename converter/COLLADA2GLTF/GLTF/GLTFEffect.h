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

#ifndef __JSON_EFFECT_H__
#define __JSON_EFFECT_H__

namespace GLTF 
{
    typedef std::vector <std::string > SemanticArray;
    typedef shared_ptr<SemanticArray> SemanticArrayPtr;
    typedef std::map<std::string /* texcoord */, SemanticArrayPtr > TexCoordToSemanticsArrayPtr;
    
    class GLTFEffect : public JSONObject {
        
    public:
        GLTFEffect(const std::string &ID);
        virtual ~GLTFEffect();
        
        const std::string& getID();        

        void setTechniqueGenerator(shared_ptr <JSONObject> techniqueGenerator);
        shared_ptr <JSONObject> getTechniqueGenerator();

        void setName(const std::string& name);
        std::string getName();
        
        void setLightingModel(const std::string& lightingModel);
        const std::string& getLightingModel();

        void setValues(shared_ptr <JSONObject>);
        shared_ptr <JSONObject> getValues();
        
        void addSemanticForTexcoordName(const std::string &texcoord, const std::string &semantic);
        SemanticArrayPtr getSemanticsForTexcoordName(const std::string &texcoord);
        
        void evaluate(void *context);
        
    private:
        std::string _ID;
        std::string _lightingModel;
        shared_ptr <JSONObject> _techniqueGenerator;
        shared_ptr <JSONObject> _values;
        TexCoordToSemanticsArrayPtr _texcoordToSemantics ;
    };
}

#endif

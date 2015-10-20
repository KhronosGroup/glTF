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

#include "GLTF.h"
#include "GLTFAsset.h"
#include "../shaders/commonProfileShaders.h"
#include <algorithm>

using namespace rapidjson;
#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;

namespace GLTF
{
    GLTFEffect::GLTFEffect(const std::string& ID): JSONObject(),
    _ID(ID), _techniqueGenerator(nullptr) {
    }
    
    GLTFEffect::GLTFEffect(const GLTFEffect &effect) : JSONObject() {
        GLTFEffect* source = const_cast<GLTFEffect*>(&effect);
        this->_ID = source->getID();
        this->_techniqueGenerator = source->getTechniqueGenerator();
        this->setName(source->getName());
        this->setValues(source->getValues());
        this->setLightingModel(source->getLightingModel());
        this->_texcoordToSemantics = effect._texcoordToSemantics;
    }

    GLTFEffect::~GLTFEffect() {
    }
                        
    const std::string& GLTFEffect::getID() {
        return this->_ID;
    }
    
    void GLTFEffect::setID(const std::string& id) {
        this->_ID = id;
    }
    
    void GLTFEffect::setTechniqueGenerator(shared_ptr <JSONObject> techniqueGenerator) {
        this->_techniqueGenerator = techniqueGenerator;
    }
    
    shared_ptr <JSONObject> GLTFEffect::getTechniqueGenerator() {
        return this->_techniqueGenerator;
    }
        
    void GLTFEffect::setName(const std::string& name) {
        this->setString(kName, name);
    }
    
    std::string GLTFEffect::getName() {
        return this->getString(kName);
    }
    
    void GLTFEffect::setValues(shared_ptr <JSONObject> values) {
        this->_values = values;
    }
    
    shared_ptr <JSONObject> GLTFEffect::getValues() {
        return this->_values;
    }

    void GLTFEffect::setKhrMaterialsCommonValues(std::shared_ptr<JSONObject> values)
    {
        this->_khrMaterialsCommonValues = values;
    }

    std::shared_ptr<JSONObject> GLTFEffect::getKhrMaterialsCommonValues()
    {
        return this->_khrMaterialsCommonValues;
    }

    void GLTFEffect::setLightingModel(const std::string& lightingModel) {
        this->_lightingModel = lightingModel;
    }
    
    const std::string& GLTFEffect::getLightingModel() {
        return this->_lightingModel;
    }
    
    void GLTFEffect::addSemanticForTexcoordName(const std::string &texcoord, const std::string &semantic) {
        shared_ptr <SemanticArray> semanticArrayPtr;
        if (this->_texcoordToSemantics.count(texcoord) == 0) {
            semanticArrayPtr =  shared_ptr<SemanticArray> (new SemanticArray());
            this->_texcoordToSemantics[texcoord] = semanticArrayPtr;
        } else {
            semanticArrayPtr = this->_texcoordToSemantics[texcoord];
        }
        
        semanticArrayPtr->push_back(semantic);
    }
    
    SemanticArrayPtr GLTFEffect::getSemanticsForTexcoordName(const std::string &texcoord) {
        return this->_texcoordToSemantics[texcoord];
    }

    inline void AddMaterialsCommonValue(const shared_ptr<JSONObject>& materialsCommonValues,
        const shared_ptr<JSONObject>& values, const std::string& name)
    {
        if (values->contains(name))
        {
            materialsCommonValues->setValue(name,
                static_pointer_cast<JSONObject>(values->getValue(name))->getValue(kValue));
        }
    }
    
    void GLTFEffect::evaluate(void *context) {
        GLTFAsset* asset = (GLTFAsset*)context;
        bool bUseKhrMaterialsCommon = CONFIG_BOOL(asset, "useKhrMaterialsCommon");
        shared_ptr <JSONObject> techniqueGenerator = this->getTechniqueGenerator();

        if (!bUseKhrMaterialsCommon)
        {
            std::string techniqueID = GLTF::getReferenceTechniqueID(techniqueGenerator, asset);

            this->setString(kTechnique, techniqueID);
            shared_ptr<JSONObject> outputs(new JSONObject());
            shared_ptr <JSONObject> values = this->getValues();
            std::vector <std::string> keys = values->getAllKeys();
            for (size_t i = 0; i < keys.size(); i++) {
                shared_ptr <JSONObject> parameter = static_pointer_cast <JSONObject> (values->getValue(keys[i]));
                shared_ptr <JSONObject> parameterValue = static_pointer_cast <JSONObject> (parameter->getValue(kValue));
                shared_ptr<JSONObject> output(new JSONObject());
                if (parameterValue) {
                    outputs->setValue(keys[i], parameterValue);
                }
            }
            this->setValue(kValues, outputs);
        }
        else
        {
            //
            // Export KHR_materials_common extension
            //
            shared_ptr <GLTF::JSONObject> extensions = this->createObjectIfNeeded("extensions");
            shared_ptr <GLTF::JSONObject> khrMaterialsCommon(new GLTF::JSONObject());

            shared_ptr<JSONObject> techniqueExtras = techniqueGenerator->getObject("techniqueExtras");
            bool doubleSided = techniqueExtras->getBool(kDoubleSided);

            std::string lightingModel = techniqueGenerator->getString("lightingModel");
            std::transform(lightingModel.begin(), lightingModel.end(), lightingModel.begin(), ::toupper);
            khrMaterialsCommon->setString(kTechnique, lightingModel);
            shared_ptr<JSONObject> materialsCommonValues(new JSONObject());

            if (doubleSided)
            {
                materialsCommonValues->setBool("doubleSided", doubleSided);
            }
            AddMaterialsCommonValue(materialsCommonValues, _khrMaterialsCommonValues, "emission");
            AddMaterialsCommonValue(materialsCommonValues, _khrMaterialsCommonValues, "transparency");

            if (lightingModel != "CONSTANT")
            {
                AddMaterialsCommonValue(materialsCommonValues, _khrMaterialsCommonValues, "ambient");
                AddMaterialsCommonValue(materialsCommonValues, _khrMaterialsCommonValues, "diffuse");

                if (lightingModel != "LAMBERT")
                {
                    AddMaterialsCommonValue(materialsCommonValues, _khrMaterialsCommonValues, "specular");
                    AddMaterialsCommonValue(materialsCommonValues, _khrMaterialsCommonValues, "shininess");
                }
            }

            khrMaterialsCommon->setValue(kValues, materialsCommonValues);

            extensions->setValue("KHR_materials_common", khrMaterialsCommon);
        }
    }
    
    std::string GLTFEffect::valueType() {
        return "effect";
    }

}

// Copyright (c) 2013, Fabrice Robinet.
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

using namespace rapidjson;
using namespace std::tr1;
using namespace std;

namespace GLTF 
{    
    GLTFConfig::GLTFConfig() {
        this->_configObject = shared_ptr<JSONObject> (new JSONObject());
        this->_setupDefaultConfigOptions(this->_configObject);
    }
    GLTFConfig::~GLTFConfig() {}
    
    void GLTFConfig::_setupDefaultConfigOptions(shared_ptr<JSONObject> optionsRoot) {
        optionsRoot->setBool("invertTransparency", false);
        optionsRoot->setBool("exportAnimations", true);
        optionsRoot->setBool("outputProgress", false);
        optionsRoot->setBool("exportPassDetails", true);
        
        optionsRoot->setBool("useDefaultLight", true);
        optionsRoot->setBool("optimizeParameters", false);

        optionsRoot->setBool("alwaysExportTRS", false);
        optionsRoot->setBool("alwaysExportFilterColor", false);
        optionsRoot->setBool("alwaysExportTransparency", false);
        
        optionsRoot->setBool("exportDefaultValues", false);
        
        optionsRoot->setString("compressionType", "none");
        optionsRoot->setString("compressionMode", "ascii");
    }
    
    bool GLTFConfig::initWithPath(const std::string& path) {
        char *error = NULL;
        if (this->_configObject->initWithContentsOfFile(path.c_str(), &error) == false) {
            if (error != NULL) {
                printf("[error] config file has parsing error: %s\n", error);
                return false;
            }
        }
        return true;
    }
    
    shared_ptr <JSONObject> GLTFConfig::config() {
        return this->_configObject;
    }

    
    unsigned int GLTFConfig::unsignedInt32ForKeyPath(const std::string &keyPath, unsigned int defaultValue) {
        unsigned int resultValue = defaultValue;
        shared_ptr<JSONValue> value = this->_configObject->valueForKeyPath(keyPath);
        if (value) {
            if (value->getType() == NUMBER) {
                shared_ptr<JSONNumber> numberValue = static_pointer_cast<JSONNumber>(value);
                resultValue = numberValue->getUnsignedInt32();
            }
        }
        
        return resultValue;
    }
    
    int GLTFConfig::int32ForKeyPath(const std::string &keyPath, int defaultValue) {
        int resultValue = defaultValue;
        shared_ptr<JSONValue> value = this->_configObject->valueForKeyPath(keyPath);
        if (value) {
            if (value->getType() == NUMBER) {
                shared_ptr<JSONNumber> numberValue = static_pointer_cast<JSONNumber>(value);
                resultValue = numberValue->getInt32();
            }
        }
        
        return resultValue;

    }
    
    double GLTFConfig::doubleForKeyPath(const std::string &keyPath, double defaultValue) {
        double resultValue = defaultValue;
        shared_ptr<JSONValue> value = this->_configObject->valueForKeyPath(keyPath);
        if (value) {
            if (value->getType() == NUMBER) {
                shared_ptr<JSONNumber> numberValue = static_pointer_cast<JSONNumber>(value);
                resultValue = numberValue->getDouble();
            }
        }
        
        return resultValue;

    }
    
    bool GLTFConfig::boolForKeyPath(const std::string &keyPath, bool defaultValue) {
        bool resultValue = defaultValue;
        shared_ptr<JSONValue> value = this->_configObject->valueForKeyPath(keyPath);
        if (value) {
            if (value->getType() == NUMBER) {
                shared_ptr<JSONNumber> numberValue = static_pointer_cast<JSONNumber>(value);
                resultValue = numberValue->getBool();
            }
        }
        
        return resultValue;
    }
    
    std::string GLTFConfig::stringForKeyPath(const std::string &keyPath, std::string defaultValue) {
        std::string resultValue = defaultValue;
        shared_ptr<JSONValue> value = this->_configObject->valueForKeyPath(keyPath);
        if (value) {
            if (value->getType() == STRING) {
                shared_ptr<JSONString> numberValue = static_pointer_cast<JSONString>(value);
                resultValue = numberValue->getString();
            }
        }
        
        return resultValue;
    }

}

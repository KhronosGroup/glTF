// Copyright (c) Fabrice Robinet
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
#include "../GLTF-OpenCOLLADA.h"

#include "../GLTFConverterContext.h"
#include "commonProfileShaders.h"
#ifndef WIN32
#include "png.h"
#include "stdarg.h"
#endif
using namespace std;

namespace GLTF
{
#define PNGSIGSIZE 8
    #ifndef WIN32
    void userReadData(png_structp pngPtr, png_bytep data, png_size_t length) {
        ((std::istream*)png_get_io_ptr(pngPtr))->read((char*)data, length);
    }
#endif
    //thanks to piko3d.com libpng tutorial here
    static bool imageHasAlpha(const char *path)
    {
#ifndef WIN32
        bool hasAlpha = false;
        std::ifstream source;
        
        source.open(path, ios::in | ios::binary);
        
        png_byte pngsig[PNGSIGSIZE];
        int isPNG = 0;
        source.read((char*)pngsig, PNGSIGSIZE);
        if (!source.good())
            return false;
        isPNG = png_sig_cmp(pngsig, 0, PNGSIGSIZE) == 0;
        if (isPNG) {
            png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
            if (pngPtr) {
                png_infop infoPtr = png_create_info_struct(pngPtr);
                if (infoPtr) {
                    png_set_read_fn(pngPtr,(png_voidp)&source, userReadData);
                    png_set_sig_bytes(pngPtr, PNGSIGSIZE);
                    png_read_info(pngPtr, infoPtr);
                    png_uint_32 color_type = png_get_color_type(pngPtr, infoPtr);
                    switch (color_type) {
                        case PNG_COLOR_TYPE_RGB_ALPHA:
                            hasAlpha =  true;
                            break;
                        case PNG_COLOR_TYPE_GRAY_ALPHA:
                            hasAlpha = true;
                            break;
                        default:
                            break;
                    }
                    
                    png_destroy_read_struct(&pngPtr, (png_infopp)0, (png_infopp)0);
                }
            }
        }
        
        source.close();
        
        return hasAlpha;
#else
		return false;
#endif
    }
    
    
    //Not yet implemented for everything
    static bool slotIsContributingToLighting(const std::string &slot, shared_ptr <JSONObject> inputParameters) {
        if (inputParameters->contains(slot)) {
            shared_ptr <JSONObject> param = inputParameters->getObject(slot);
            
            if (param->getString("type") == "SAMPLER_2D" )
                return true; //it is a texture, so presumably yes, this slot is not neutral
            
            if (param->contains("value")) {
                if (slot == "reflective")
                    return false;
                
                shared_ptr <JSONArray> color = static_pointer_cast<JSONArray>(param->getValue("value"));
                vector <shared_ptr <JSONValue> >  values = color->values();
                size_t count = values.size();
                if (count == 3) {
                    //FIXME: handling post processing of JSON Array with numbers is just overkill
                    shared_ptr <JSONNumber> r = static_pointer_cast<JSONNumber>(values[0]);
                    shared_ptr <JSONNumber> g = static_pointer_cast<JSONNumber>(values[1]);
                    shared_ptr <JSONNumber> b = static_pointer_cast<JSONNumber>(values[2]);
                    return (r->getDouble() > 0 || r->getDouble() > 0 || b->getDouble());
                }
            }
        }
        
        return false;
    }
    
    static double getTransparency(shared_ptr<JSONObject> parameters, const GLTFConverterContext& context) {
        //super naive for now, also need to check sketchup work-around
        //if (effectCommon->getOpacity().isTexture()) {
        //    return 1;
        //}
        
        double transparency = parameters->contains("transparency") ? parameters->getDouble("transparency") : 1;
        
        return context.invertTransparency ? 1 - transparency : transparency;
    }
    
    static bool hasTransparency(shared_ptr<JSONObject> parameters,
                         GLTFConverterContext& context) {
        return getTransparency(parameters, context)  < 1;
    }
    
    static bool isOpaque(shared_ptr <JSONObject> parameters, GLTFConverterContext& context) {

        if (parameters->contains("diffuse")) {
            shared_ptr <JSONObject> diffuse = parameters->getObject("diffuse");
            
            if (diffuse->getString("type") == "SAMPLER_2D") {
                std::string imagePath = context._imageIdToImagePath[diffuse->getString("image")];
                
                COLLADABU::URI inputURI(context.inputFilePath.c_str());
                std::string imageFullPath = inputURI.getPathDir() + imagePath;
                if (imageHasAlpha(imageFullPath.c_str()))
                    return false;
            }
        }
        return !hasTransparency(parameters, context);
    }
    
    //support this style for semantics
    //http://www.nvidia.com/object/using_sas.html
    /*
     WORLD
     VIEW
     PROJECTION
     WORLDVIEW
     VIEWPROJECTION
     WORLDVIEWPROJECTION
     WORLDINVERSE
     VIEWINVERSE
     PROJECTIONINVERSE
     WORLDVIEWINVERSE
     VIEWPROJECTIONINVERSE
     WORLDVIEWPROJECTIONINVERSE
     WORLDTRANSPOSE
     VIEWTRANSPOSE
     PROJECTIONTRANSPOSE
     WORLDVIEWTRANSPOSE
     VIEWPROJECTIONTRANSPOSE
     WORLDVIEWPROJECTIONTRANSPOSE
     WORLDINVERSETRANSPOSE
     VIEWINVERSETRANSPOSE
     PROJECTIONINVERSETRANSPOSE
     WORLDVIEWINVERSETRANSPOSE
     VIEWPROJECTIONINVERSETRANSPOSE
     WORLDVIEWPROJECTIONINVERSETRANSPOSE
     */
    
    static std::string WORLDVIEW = "WORLDVIEW";
    static std::string WORLDVIEWINVERSETRANSPOSE = "WORLDVIEWINVERSETRANSPOSE";
    static std::string PROJECTION = "PROJECTION";
    
    /* uniform types, derived from
     GL_INT
     GL_INT_VEC2
     GL_INT_VEC3
     GL_INT_VEC4
     GL_BOOL
     GL_BOOL_VEC2
     GL_BOOL_VEC3
     GL_BOOL_VEC4
     GL_FLOAT_MAT2
     GL_FLOAT_MAT3
     GL_FLOAT_MAT4
     GL_SAMPLER_2D
     GL_SAMPLER_CUBE
     */
    
    
    static std::string typeForSemanticAttribute(const std::string& semantic) {
        static std::map<std::string , std::string> typeForSemanticAttribute;
        
        if (semantic.find("TEXCOORD") != string::npos) {
            return "FLOAT_VEC2";
        }
        
        if (typeForSemanticAttribute.empty()) {
            typeForSemanticAttribute["POSITION"] = "FLOAT_VEC3";
            typeForSemanticAttribute["NORMAL"] = "FLOAT_VEC3";
            typeForSemanticAttribute["REFLECTIVE"] = "FLOAT_VEC2";
        }
        return typeForSemanticAttribute[semantic];
    }

    static std::string typeForSemanticUniform(const std::string& semantic) {
        static std::map<std::string , std::string> typeForSemanticUniform;
        
        if (typeForSemanticUniform.empty()) {
            typeForSemanticUniform["WORLDVIEWINVERSETRANSPOSE"] = "FLOAT_MAT3"; //typically the normal matrix
            typeForSemanticUniform["WORLDVIEW"] = "FLOAT_MAT4"; 
            typeForSemanticUniform["PROJECTION"] = "FLOAT_MAT4"; 
        }
        return typeForSemanticUniform[semantic];
    }

    static std::string buildSlotHash(shared_ptr<JSONObject> &parameters, std::string slot) {
        std::string hash = slot + ":";

        if (slotIsContributingToLighting(slot, parameters)) {
        //if (parameters->contains(slot)) {
            shared_ptr<JSONObject> parameter = parameters->getObject(slot);
            
            if (parameter->contains("type")) {
                hash += parameter->getString("type");
                return hash;
            }
        } else if (parameters->contains(slot) && slot != "diffuse") {
            parameters->removeValue(slot);
        }
        return hash + "none";
    }
    
    static std::string buildTechniqueHash(shared_ptr<JSONObject> parameters, shared_ptr<JSONObject> techniqueExtras, GLTFConverterContext& context) {
        std::string techniqueHash = "";
        
        //FIXME:now assume we always have diffuse specified
        shared_ptr<JSONObject> parameter = parameters->getObject("diffuse");
        
        techniqueHash += buildSlotHash(parameters, "diffuse");
        techniqueHash += buildSlotHash(parameters, "ambient");
        techniqueHash += buildSlotHash(parameters, "emission");
        techniqueHash += buildSlotHash(parameters, "specular");
        techniqueHash += buildSlotHash(parameters, "reflective");
        
        techniqueHash += "double_sided:" + GLTFUtils::toString(techniqueExtras->getBool("double_sided"));
        techniqueHash += "opaque:"+ GLTFUtils::toString(isOpaque(parameters, context));
        techniqueHash += "hasTransparency:"+ GLTFUtils::toString(hasTransparency(parameters, context));
                
        return techniqueHash;
    }
    
    bool writeShaderIfNeeded(const std::string& shaderId,  GLTFConverterContext& context)
    {
        shared_ptr <JSONObject> shadersObject = context.root->createObjectIfNeeded("shaders");
        
        shared_ptr <JSONObject> shaderObject  = shadersObject->getObject(shaderId);
        
        if (!shaderObject) {
            shaderObject = shared_ptr <GLTF::JSONObject> (new GLTF::JSONObject());
            
            std::string path = shaderId+".glsl";
            shadersObject->setValue(shaderId, shaderObject);
            shaderObject->setString("path", path);
            
            //also write the file on disk
            std::string shaderString = context.shaderIdToShaderString[shaderId];
            if (shaderString.size() > 0) {
                COLLADABU::URI outputURI(context.outputFilePath);
                std::string shaderPath =  outputURI.getPathDir() + path;
                GLTF::GLTFUtils::writeData(shaderPath, "w",(unsigned char*)shaderString.c_str(), shaderString.size());
                if (!context.outputProgress) {
                    printf("[shader]: %s\n", shaderPath.c_str());
                }
            }
        }
        
        return true;
    }
    
    static shared_ptr <JSONObject> createStatesForTechnique(shared_ptr<JSONObject> parameters, shared_ptr<JSONObject> techniqueExtras, GLTFConverterContext& context)
    {
        shared_ptr <JSONObject> states(new GLTF::JSONObject());

        states->setBool("cullFaceEnable", !techniqueExtras->getBool("double_sided"));
        
        if (isOpaque(parameters, context)) {
            states->setBool("depthTestEnable", true);
            states->setBool("depthMask", true);
            states->setBool("blendEnable", false);            
        } else {            
            states->setBool("blendEnable", true);
            states->setBool("depthTestEnable", true);
            states->setBool("depthMask", false);         //should be true for images, and false for plain color
            states->setString("blendEquation", "FUNC_ADD");
            shared_ptr <JSONObject> blendFunc(new GLTF::JSONObject());
            blendFunc->setString("sfactor", "SRC_ALPHA");
            blendFunc->setString("dfactor", "ONE_MINUS_SRC_ALPHA");
            states->setValue("blendFunc", blendFunc) ;
        }
        
        return states;
    }
    
    typedef std::map<std::string , std::string > TechniqueHashToTechniqueID;
    
    /*
    static size_t __GetSetIndex(const std::string &semantic) {
        size_t index = semantic.find("_");
        if (index !=  string::npos) {
            std::string setStr = semantic.substr(index + 1);
            index = atoi(setStr.c_str());
            return index;
        }
        
        return 0;
    }
    */
    
    class GLSLShader {
    public:
        
        GLSLShader() {
            _declarations = "precision highp float;\n";;
            _body = "void main(void) {\n";
        }
        
        void setName(std::string name) {
            _name = name;
        }
        
        std::string getName() {
            return _name;
        }
        
        static std::string GLSLTypeForGLType(const std::string &glType) {
            static std::map<std::string , std::string> GLSLTypeForGLType;
            
            if (GLSLTypeForGLType.empty()) {
                GLSLTypeForGLType["FLOAT"] = "float";
                GLSLTypeForGLType["FLOAT_VEC2"] = "vec2";
                GLSLTypeForGLType["FLOAT_VEC3"] = "vec3";
                GLSLTypeForGLType["FLOAT_VEC4"] = "vec4";
                
                GLSLTypeForGLType["FLOAT_MAT2"] = "mat2";
                GLSLTypeForGLType["FLOAT_MAT3"] = "mat3";
                GLSLTypeForGLType["FLOAT_MAT4"] = "mat4";
                
                GLSLTypeForGLType["INT"] = "int";
                GLSLTypeForGLType["INT_VEC2"] = "ivec";
                GLSLTypeForGLType["INT_VEC3"] = "ivec3";
                GLSLTypeForGLType["INT_VEC4"] = "ivec4";
                
                GLSLTypeForGLType["BOOL"] = "bool";
                GLSLTypeForGLType["BOOL_VEC2"] = "bvec2";
                GLSLTypeForGLType["BOOL_VEC3"] = "bvec3";
                GLSLTypeForGLType["BOOL_VEC4"] = "bvec4";
                
                GLSLTypeForGLType["SAMPLER_2D"] = "sampler2D";
                GLSLTypeForGLType["SAMPLER_CUBE"] = "samplerCube";
            }
            return GLSLTypeForGLType[glType];
        }
        
        void _addDeclaration(std::string qualifier, std::string symbol, std::string type) {
            std::string declaration = qualifier + " ";
            declaration += GLSLTypeForGLType(type);
            declaration += " " + symbol +";\n";
            _declarations += declaration;
        }
        
        void addAttribute(std::string symbol, std::string type) {
            _addDeclaration("attribute", symbol, type);
        }
        
        void addUniform(std::string symbol, std::string type) {
            _addDeclaration("uniform", symbol, type);
        }
        
        void addVarying(std::string symbol, std::string type) {
            _addDeclaration("varying", symbol, type);
        }
        
        void appendCode(const char * format, ... ) {
            char buffer[1000];
            va_list args;
            va_start (args, format);
            vsprintf (buffer,format, args);
            _body += buffer;
            va_end (args);
        }
        
        std::string source() {
            return _declarations + _body;
        }
        
    private:
        std::string _name;
        std::string _declarations;
        std::string _body;
    };
    
    class GLSLProgram {
    public:
        GLSLProgram() {
            _uniforms = shared_ptr <GLTF::JSONObject>(new GLTF::JSONObject());
            _attributes = shared_ptr <GLTF::JSONObject>(new GLTF::JSONObject());
        }

        void _nameDidChange() {
            _vertexShader.setName(_name + "VS");
            _fragmentShader.setName(_name + "FS");
        }
        
        void setName(std::string name) {
            _name = name;
            _nameDidChange();
        }
        
        std::string getName() {
            return _name;
        }
        
        shared_ptr <JSONObject> attributes() {
            return _attributes;
        }
        
        shared_ptr <JSONObject> uniforms() {
            return _uniforms;
        }
        
        GLSLShader* vertexShader() {
            return &_vertexShader;
        }
        
        GLSLShader* fragmentShader() {
            return &_fragmentShader;
        }
        
        void addVarying(std::string symbol, std::string type) {
            _vertexShader.addVarying(symbol, type);
            _fragmentShader.addVarying(symbol, type);
        }
        
    private:
        GLSLShader _vertexShader;
        GLSLShader _fragmentShader;
        std::string _name;
        
        shared_ptr <GLTF::JSONObject> _attributes;
        shared_ptr <GLTF::JSONObject> _uniforms;
    };
    
    class Pass {
    public:
        Pass() {}
        
        GLSLProgram* instanceProgram() {
            return &_instanceProgram;
        }
        
        shared_ptr <JSONObject> getDetails(const std::string &lightingModel,
                                           shared_ptr<JSONObject> values,
                                           shared_ptr<JSONObject> techniqueExtras,
                                           std::map<std::string , std::string > &texcoordBindings,
                                           GLTFConverterContext& context) {
            shared_ptr <JSONObject> details(new JSONObject());
            
            shared_ptr <JSONObject> commonProfile = details->createObjectIfNeeded("commonProfile");
            shared_ptr <JSONObject> extras = commonProfile->createObjectIfNeeded("extras");
            
            details->setString("type", "COLLADA-1.4.1/commonProfile");
            
            shared_ptr<JSONArray> parameters(new JSONArray());
            
            shared_ptr <JSONObject> uniforms = _instanceProgram.uniforms();
            vector <std::string> keys = uniforms->getAllKeys();
            for (size_t i = 0 ; i < keys.size() ; i++) {
                std::string parameter = uniforms->getString(keys[i]);
                parameters->appendValue(shared_ptr <JSONValue> (new JSONString( parameter)));

                /*if ((parameter == "diffuse") ||
                    (parameter == "ambient") ||
                    (parameter == "specular") ||
                    (parameter == "normal") ||
                    (parameter == "reflective") ||
                    (parameter == "transparency")) {
                    parameters->appendValue(shared_ptr <JSONValue> (new JSONString( parameter)));
                }*/
            }
            commonProfile->setValue("parameters", parameters);
            
            commonProfile->setString("lightingModel", lightingModel);
            
            extras->setBool("doubleSided", techniqueExtras->getBool("double_sided"));
            
            if (texcoordBindings.size() > 0) {
                std::map<std::string , std::string >::const_iterator texcoordIterator;
                shared_ptr <JSONObject> texcoordBindingsObject = commonProfile->createObjectIfNeeded("texcoordBindings");
                
                for (texcoordIterator = texcoordBindings.begin() ; texcoordIterator != texcoordBindings.end() ; texcoordIterator++) {
                    std::string key = (*texcoordIterator).first;
                    std::string texcoord = texcoordBindings[key];
                    texcoordBindingsObject->setString(key, texcoord);
                }
            }
            
            return details;
        }
        
    private:
        GLSLProgram _instanceProgram;
        shared_ptr <JSONObject> states;
    };
    
    class Technique {
    public:
        
        //FIXME: pass id when we support multipass
        Pass* getPass() {
            return &_pass;
        }
        
        //FIXME: it's a bad API since we won't have at the same time a varying an uniform...
        void addSemantic(std::string vertexOrFragment, std::string uniformOrAttribute,
                         std::string semantic,
                         std::string parameterID,
                         bool includesVarying) {
            
            std::string symbol = (uniformOrAttribute == "attribute") ? "a_" + parameterID : "u_" + parameterID;

            std::string type = (uniformOrAttribute == "uniform") ?
                                    typeForSemanticUniform(semantic) :
                                    typeForSemanticAttribute(semantic);
            
            shared_ptr <JSONObject> parameter(new GLTF::JSONObject());
            parameter->setString("semantic", semantic);
            parameter->setString("type",  type);
            _parameters->setValue(parameterID, parameter);

            //FIXME: should not assume default pass / default program
            GLSLProgram* program = _pass.instanceProgram();
            GLSLShader* shader = (vertexOrFragment == "vs") ? program->vertexShader() : program->fragmentShader();
            if (uniformOrAttribute == "attribute") {
                program->attributes()->setString(symbol, parameterID);
            } else if (uniformOrAttribute == "uniform") {
                program->uniforms()->setString(symbol, parameterID);
            } else {
                printf("cannot add semantic of unknown kind %s\n", uniformOrAttribute.c_str());
                return;
            }
            
            if (uniformOrAttribute == "attribute") {
                shader->addAttribute(symbol, type);
                
                if (includesVarying) {
                    program->addVarying("v_" + parameterID, type);
                }
            } else {
                shader->addUniform(symbol, type);
            }            
        }
        
        //FIXME: refactor with addSemantic
        void addValue(std::string vertexOrFragment, std::string uniformOrAttribute,
                      std::string type,
                      std::string parameterID) {
            
                        
            std::string symbol = (uniformOrAttribute == "attribute") ? "a_" + parameterID : "u_" + parameterID;
            
            shared_ptr <JSONObject> parameter(new GLTF::JSONObject());
            parameter->setString("type",  type);
            _parameters->setValue(parameterID, parameter);
            
            //FIXME: should not assume default pass / default program
            GLSLProgram* program = _pass.instanceProgram();
            GLSLShader* shader = (vertexOrFragment == "vs") ? program->vertexShader() : program->fragmentShader();
            if (uniformOrAttribute == "attribute") {
                program->attributes()->setString(symbol, parameterID);
            } else if (uniformOrAttribute == "uniform") {
                program->uniforms()->setString(symbol, parameterID);
            } else {
                printf("cannot add semantic of unknown kind %s\n", uniformOrAttribute.c_str());
                return;
            }
            
            if (uniformOrAttribute == "attribute") {
                shader->addAttribute(symbol, type);
            } else {
                shader->addUniform(symbol, type);
            }
        }
        
        Technique(const std::string &lightingModel,
                  std::string techniqueID,
                  shared_ptr<JSONObject> values,
                  shared_ptr<JSONObject> techniqueExtras,
                  std::map<std::string , std::string > &texcoordBindings,
                  GLTFConverterContext& context) {
            
            _parameters = shared_ptr<GLTF::JSONObject>(new GLTF::JSONObject());
            
            shared_ptr <JSONObject> inputParameters = values;

            bool useSimpleLambert = !(slotIsContributingToLighting("specular", inputParameters) &&
                                      inputParameters->contains("shininess"));
            
            std::vector <std::string> allAttributes;
            std::vector <std::string> allUniforms;
            std::string shaderName = techniqueID;
            
            GLSLProgram* program = _pass.instanceProgram();
            GLSLShader* vertexShader = program->vertexShader();
            GLSLShader* fragmentShader = program->fragmentShader();
            program->setName(shaderName);

            //position attribute
            addSemantic("vs", "attribute",
                        "POSITION", "position" , false);
            
            //normal attribute
            addSemantic("vs", "attribute",
                        "NORMAL", "normal", true);
            
            //normal matrix
            addSemantic("vs", "uniform",
                        "WORLDVIEWINVERSETRANSPOSE", "normalMatrix" , false);

            //worldview matrix
            addSemantic("vs", "uniform",
                        "WORLDVIEW", "worldViewMatrix" , false);
            
            //projection matrix
            addSemantic("vs", "uniform",
                        "PROJECTION", "projectionMatrix" , false);
            
            vertexShader->appendCode("%s = normalize(%s * %s);\n",
                                        "v_normal", "u_normalMatrix", "a_normal");
            
            
            vertexShader->appendCode("%s pos = %s * vec4(%s,1.0);\n",
                    GLSLShader::GLSLTypeForGLType("FLOAT_VEC4").c_str(),
                    "u_worldViewMatrix",
                    "a_position");
            
            
            fragmentShader->appendCode("vec3 normal = normalize(%s);\n",
                                        "v_normal");
            if (techniqueExtras->getBool("double_sided")) {
                fragmentShader->appendCode("if (gl_FrontFacing == false) normal = -normal;\n");
            }
            
            if (useSimpleLambert) {
                fragmentShader->appendCode("float lambert = max(dot(normal,vec3(0.,0.,1.)), 0.);\n");
            }
            
            //color to cumulate all components and light contribution
            fragmentShader->appendCode("vec4 color = vec4(0., 0., 0., 0.);\n");
            fragmentShader->appendCode("vec4 diffuse = vec4(0., 0., 0., 1.);\n");
            if (slotIsContributingToLighting("emission", inputParameters)) {
                fragmentShader->appendCode("vec4 emission;\n");             }
            if (slotIsContributingToLighting("reflective", inputParameters)) {
                fragmentShader->appendCode("vec4 reflective;\n");
            }
            if (slotIsContributingToLighting("specular", inputParameters)) {
                fragmentShader->appendCode("vec4 specular;\n");
            }
            
            if (!useSimpleLambert) {
                shared_ptr <JSONObject> shininessObject = inputParameters->getObject("shininess");
                addValue("fs", "uniform",   shininessObject->getString("type"), "shininess");
                
                //save light direction
                program->addVarying("v_lightDirection", "FLOAT_VEC3");
                
                vertexShader->appendCode("v_lightDirection = vec3(%s * (vec4((vec3(0.,0.,-1.) - %s.xyz) ,1.0)));\n",
                        "u_worldviewMatrixSymbol",
                        "a_position");
                
                //save camera-vertex
                program->addVarying("v_mPos", "FLOAT_VEC3");
                vertexShader->appendCode("v_mPos = pos.xyz;\n");
                
                fragmentShader->appendCode("vec3 l = normalize(v_lightDirection);\n\
                        vec3 v = normalize(v_mPos);\n\
                        vec3 h = normalize(l+v);\n\
                        float lambert = max(-dot(normal,l), 0.);\n\
                        float specLight = pow(max(0.0,-dot(normal,h)),u_shininess);\n");
            }
            
            //texcoords
            std::string texcoordAttributeSymbol = "a_texcoord";
            std::string texcoordVaryingSymbol = "v_texcoord";
            std::map<std::string , std::string> declaredTexcoordAttributes;
            std::map<std::string , std::string> declaredTexcoordVaryings;
            
            const int slotsCount = 4;
            std::string slots[slotsCount] = { "diffuse", "emission", "reflective", "specular" };
            for (size_t slotIndex = 0 ; slotIndex < slotsCount ; slotIndex++) {
                std::string slot = slots[slotIndex];
                
                if (!slotIsContributingToLighting(slot, inputParameters))
                    continue;
                
                shared_ptr <JSONObject> param = inputParameters->getObject(slot);
                
                //FIXME:currently colors are known to be FLOAT_VEC3, this should cleaned up by using the type.
                std::string slotType = param->getString("type");
                if (slotType == "FLOAT_VEC3" ) {
                    std::string slotColorSymbol = "u_"+slot;
                    fragmentShader->appendCode("%s.xyz = %s;\n", slot.c_str(), slotColorSymbol.c_str());
                    
                    addValue("fs", "uniform",   slotType, slot);
                } else if (slotType == "SAMPLER_2D") {
                    std::string semantic = texcoordBindings[slot];
                    std::string texSymbol;
                    std::string texVSymbol;
                    
                    if (slot == "reflective") {
                        texVSymbol = texcoordVaryingSymbol + GLTFUtils::toString(declaredTexcoordVaryings.size());
                        std::string reflectiveType = typeForSemanticAttribute("REFLECTIVE");
                        program->addVarying(texVSymbol, reflectiveType);
                        
                        //Update Vertex shader for reflection
                        std::string normalType = GLSLShader::GLSLTypeForGLType("FLOAT_VEC3");
                        vertexShader->appendCode("%s normalizedVert = normalize(%s(pos));\n",
                                normalType.c_str(),
                                normalType.c_str());
                        vertexShader->appendCode("%s r = reflect(normalizedVert, %s);\n",
                                normalType.c_str(),
                                "v_normal");
                        vertexShader->appendCode("r.z += 1.0;\n");
                        vertexShader->appendCode("float m = 2.0 * sqrt(dot(r,r));\n");
                        vertexShader->appendCode("%s = (r.xy / m) + 0.5;\n", texVSymbol.c_str());
                        //sprintf(stringBuffer, "%s = %s;\n", texVSymbol.c_str(), texSymbol.c_str()); vsBody += stringBuffer;
                        declaredTexcoordVaryings[semantic] = texVSymbol;
                    } else {
                        if  (declaredTexcoordAttributes.count(semantic) == 0) {
                            texSymbol = texcoordAttributeSymbol + GLTFUtils::toString(declaredTexcoordAttributes.size());
                            texVSymbol = texcoordVaryingSymbol + GLTFUtils::toString(declaredTexcoordVaryings.size());
                            
                            addSemantic("vs", "attribute",
                                        semantic, "texcoord" + GLTFUtils::toString(declaredTexcoordAttributes.size()), false);
                            program->addVarying(texVSymbol, typeForSemanticAttribute(semantic));
                            
                            vertexShader->appendCode("%s = %s;\n", texVSymbol.c_str(), texSymbol.c_str());
                            declaredTexcoordAttributes[semantic] = texSymbol;
                            declaredTexcoordVaryings[semantic] = texVSymbol;
                        }
                    }
                    
                    
                    std::string textureSymbol = "u_"+ slot;
                    
                    //get the texture
                    shared_ptr <JSONObject> textureParameter = inputParameters->getObject(slot);
                    //FIXME:this should eventually not come from the inputParameter
                    addValue("fs", "uniform", textureParameter->getString("type"), slot);

                    fragmentShader->appendCode("%s = texture2D(%s, %s);\n", slot.c_str(), textureSymbol.c_str(), texVSymbol.c_str());
                }
            }
            
            if (slotIsContributingToLighting("reflective", inputParameters)) {
                fragmentShader->appendCode("diffuse.xyz += reflective.xyz;\n");
            }
            
            fragmentShader->appendCode("diffuse.xyz *= lambert;\n");
            fragmentShader->appendCode("color += diffuse;\n");
            
            if (slotIsContributingToLighting("emission", inputParameters)) {
                fragmentShader->appendCode("color.xyz += emission.xyz;\n");
            }
            
            if (!useSimpleLambert) {
                fragmentShader->appendCode("color.xyz += specular.xyz * specLight;\n");
            }
            
            bool hasTransparency = inputParameters->contains("transparency");
            if (hasTransparency) {
                std::string slot = "transparency";
                shared_ptr <JSONObject> transparencyParam = inputParameters->getObject(slot);
                
                addValue("fs", "uniform",   transparencyParam->getString("type"), slot);
                
                fragmentShader->appendCode("gl_FragColor = vec4(color.rgb * color.a, color.a * %s);\n", "u_transparency");
            } else {
                fragmentShader->appendCode("gl_FragColor = vec4(color.rgb * color.a, color.a);\n");
            }
            
            vertexShader->appendCode("gl_Position = %s * pos;\n",
                                     "u_projectionMatrix");
            
            vertexShader->appendCode("}\n");
            fragmentShader->appendCode("}\n");
            
        }
        
        shared_ptr <GLTF::JSONObject> parameters() {
            return _parameters;
        }
        
    private:
        Pass _pass;
        shared_ptr <GLTF::JSONObject> _parameters;
        
    };
    
    std::string getReferenceTechniqueID(const std::string &lightingModel,
                                        shared_ptr<JSONObject> values,
                                        shared_ptr<JSONObject> techniqueExtras,
                                        std::map<std::string , std::string > &texcoordBindings,
                                        GLTFConverterContext& context) {
        
        shared_ptr <JSONObject> inputParameters = values;
        shared_ptr <JSONObject> techniquesObject = context.root->createObjectIfNeeded("techniques");
        std::string techniqueHash = buildTechniqueHash(values, techniqueExtras, context);

        static TechniqueHashToTechniqueID techniqueHashToTechniqueID;
        if (techniqueHashToTechniqueID.count(techniqueHash) == 0) {
            techniqueHashToTechniqueID[techniqueHash] = "technique" + GLTFUtils::toString(techniqueHashToTechniqueID.size());
        }
        
        std::string techniqueID = techniqueHashToTechniqueID[techniqueHash];
        if (techniquesObject->contains(techniqueID))
            return techniqueID;
        
        GLTF::Technique glTFTechnique(lightingModel, techniqueID, values, techniqueExtras, texcoordBindings, context);
        GLTF::Pass *glTFPass = glTFTechnique.getPass();
        
        std::string passName("defaultPass");
        //if the technique has not been serialized, first thing create the default pass for this technique
        shared_ptr <GLTF::JSONObject> pass(new GLTF::JSONObject());
        
        shared_ptr <GLTF::JSONObject> states = createStatesForTechnique(values, techniqueExtras, context);
        pass->setValue("states", states);
        
        GLSLProgram* glTFProgram = glTFPass->instanceProgram();
        GLSLShader* vs = glTFProgram->vertexShader();
        GLSLShader* fs = glTFProgram->fragmentShader();
        
        
        //create shader name made of the input file name to avoid file name conflicts
        COLLADABU::URI outputFileURI(context.outputFilePath.c_str());
        std::string shaderBaseId = outputFileURI.getPathFileBase()+GLTFUtils::toString(context.shaderIdToShaderString.size());
        std::string shaderFS = shaderBaseId + "FS";
        std::string shaderVS = shaderBaseId + "VS";
        
        context.shaderIdToShaderString[shaderVS] = vs->source();
        context.shaderIdToShaderString[shaderFS] = fs->source();
        
        writeShaderIfNeeded(shaderVS, context);
        writeShaderIfNeeded(shaderFS, context);
        
        shared_ptr <JSONObject> programsObject = context.root->createObjectIfNeeded("programs");
        std::string programID = "program_" + GLTFUtils::toString(programsObject->getKeysCount());
        shared_ptr <GLTF::JSONObject> program(new GLTF::JSONObject());
        shared_ptr <GLTF::JSONObject> instanceProgram(new GLTF::JSONObject());
        
        instanceProgram->setValue("uniforms", glTFProgram->uniforms());
        instanceProgram->setValue("attributes", glTFProgram->attributes());
        pass->setValue("instanceProgram", instanceProgram);
        instanceProgram->setString("program", programID);
        programsObject->setValue(programID, program);
        
        program->setString("vertexShader", shaderVS);
        program->setString("fragmentShader", shaderFS);
        
        shared_ptr<JSONObject> referenceTechnique(new JSONObject());
        
        referenceTechnique->setValue("parameters", glTFTechnique.parameters());
        referenceTechnique->setString("pass", passName);
        
        shared_ptr <GLTF::JSONObject> passes = referenceTechnique->createObjectIfNeeded("passes");
        
        passes->setValue(passName, pass);
        techniquesObject->setValue(techniqueID, referenceTechnique);
        
        if (context.exportPassDetails) {
            shared_ptr <JSONObject> details = glTFPass->getDetails(lightingModel, values, techniqueExtras, texcoordBindings, context);
            pass->setValue("details", details);
        }
        
        return techniqueID;
    }    
}

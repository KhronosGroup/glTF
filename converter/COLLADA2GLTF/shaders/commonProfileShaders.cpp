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
#endif

#include "stdarg.h"

#define _GL_STR(X) #X
#define _GL(X) (this->_profile->getGLenumForString(_GL_STR(X)));

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
//        printf("path:%s\n",path);
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
    static bool slotIsContributingToLighting(const std::string &slot, shared_ptr <JSONObject> inputParameters, const GLTFConverterContext& context) {
        if (inputParameters->contains(slot)) {
            if (context.optimizeParameters == false)
                return true;
            
            //FIXME: we need an explicit option to allow this, and make sure we get then consistent instanceTechnique and technique parameters
            shared_ptr <JSONObject> param = inputParameters->getObject(slot);
            
            if (param->getUnsignedInt32("type") == context.profile->getGLenumForString("SAMPLER_2D"))
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
        
        if (!parameters->contains("transparency")) {
            return 1;
        }
        
        shared_ptr<JSONObject> tr = parameters->getObject("transparency");
        
        double transparency = tr->getDouble("value");
        
        return context.invertTransparency ? 1 - transparency : transparency;
    }
    
    static bool hasTransparency(shared_ptr<JSONObject> parameters,
                         GLTFConverterContext& context) {
        return getTransparency(parameters, context)  < 1;
    }
    
    static bool isOpaque(shared_ptr <JSONObject> parameters, GLTFConverterContext& context) {

        if (parameters->contains("diffuse")) {
            shared_ptr <JSONObject> diffuse = parameters->getObject("diffuse");
            
            if (diffuse->getUnsignedInt32("type") == context.profile->getGLenumForString("SAMPLER_2D")) {
                shared_ptr<JSONObject> textures = context.root->createObjectIfNeeded("textures");
                if (textures->getKeysCount() == 0) {
                    return false;
                }
                shared_ptr<JSONObject> texture = textures->getObject(diffuse->getString("value"));
                std::string sourceUID = texture->getString("source");
                shared_ptr<JSONObject> images = context.root->createObjectIfNeeded("images");
                
                if (images->contains(sourceUID)) {
                    shared_ptr<JSONObject> image = images->getObject(sourceUID);
                    std::string imagePath = image->getString("path");
                    COLLADABU::URI inputURI(context.inputFilePath.c_str());
                    std::string imageFullPath = inputURI.getPathDir() + imagePath;
                    if (imageHasAlpha(imageFullPath.c_str()))
                        return false;
                } else {
                    static bool printedOnce = false;
                    if (!printedOnce) {
                        printedOnce = true;
                        printf("Inconsistency error: this asset probably refers to invalid image Ids within <surface>\n");
                    }
                    return false;
                }
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
    
    static std::string buildSlotHash(shared_ptr<JSONObject> &parameters, std::string slot, const GLTFConverterContext& converterContext) {
        std::string hash = slot + ":";

        if (slotIsContributingToLighting(slot, parameters, converterContext)) {
            shared_ptr<JSONObject> parameter = parameters->getObject(slot);
            
            if (parameter->contains("type")) {
                hash += GLTFUtils::toString(parameter->getUnsignedInt32("type"));
                return hash;
            }
        } else if (parameters->contains(slot) && slot != "diffuse") {
            parameters->removeValue(slot);
        }
        return hash + "none";
    }
    
    /*
    static std::string buildLightsHash(shared_ptr<JSONObject> parameters, shared_ptr<JSONObject> techniqueExtras, GLTFConverterContext& context) {
        std::string lightsHash = "";
        
        if (context.root->contains("lightsIds")) {
            shared_ptr<JSONArray> lightsIds = context.root->createArrayIfNeeded("lightsIds");
            std::vector <shared_ptr <JSONValue> > ids = lightsIds->values();
            
            for (size_t i = 0 ; i < ids.size() ; i++) {
                shared_ptr<JSONString> lightUID = static_pointer_cast<JSONString>(ids[i]);
                
                shared_ptr<JSONArray> lightsNodesIds = static_pointer_cast<JSONArray>(context._uniqueIDOfLightToNodes[lightUID->getString()]);
                
                std::vector <shared_ptr <JSONValue> > nodesIds = lightsNodesIds->values();
                for (size_t i = 0 ; i < ids.size() ; i++) {
                }
           }
        }
        return lightsHash;
    }
    */
    static std::string buildTechniqueHash(shared_ptr<JSONObject> parameters, shared_ptr<JSONObject> techniqueExtras, GLTFConverterContext& context) {
        std::string techniqueHash = "";
        
        //FIXME:now assume we always have diffuse specified
        shared_ptr<JSONObject> parameter = parameters->getObject("diffuse");
        
        techniqueHash += buildSlotHash(parameters, "diffuse", context);
        techniqueHash += buildSlotHash(parameters, "ambient", context);
        techniqueHash += buildSlotHash(parameters, "emission", context);
        techniqueHash += buildSlotHash(parameters, "specular", context);
        techniqueHash += buildSlotHash(parameters, "reflective", context);
        //techniqueHash += buildLightsHash(parameters, techniqueExtras, context);
        
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
    
    static shared_ptr <JSONObject> createStatesForTechnique(shared_ptr<JSONObject> parameters, shared_ptr<JSONObject> techniqueExtras, GLTFConverterContext& converterContext)
    {
        shared_ptr <JSONObject> states(new GLTF::JSONObject());
        shared_ptr <GLTFProfile> profile = converterContext.profile;
        unsigned int GLZero = 0;
        unsigned int GLOne = 1;
        
        states->setUnsignedInt32("cullFaceEnable", techniqueExtras->getBool("double_sided") ? GLZero : GLOne );
        
        if (isOpaque(parameters, converterContext)) {
            states->setUnsignedInt32("depthTestEnable", GLOne);
            states->setUnsignedInt32("depthMask", GLOne);
            states->setUnsignedInt32("blendEnable", GLZero);
        } else {            
            states->setUnsignedInt32("blendEnable", GLOne);
            states->setUnsignedInt32("depthTestEnable", GLOne);
            states->setUnsignedInt32("depthMask", GLZero); //should be true for images, and false for plain color
            states->setUnsignedInt32("blendEquation", profile->getGLenumForString("FUNC_ADD"));
            shared_ptr <JSONObject> blendFunc(new GLTF::JSONObject());
            blendFunc->setUnsignedInt32("sfactor", profile->getGLenumForString("SRC_ALPHA"));
            blendFunc->setUnsignedInt32("dfactor", profile->getGLenumForString("ONE_MINUS_SRC_ALPHA"));
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
        
        GLSLShader(shared_ptr <GLTFProfile> profile) {
            this->_declarations = "precision highp float;\n";;
            this->_body = "void main(void) {\n";
            this->_profile = profile;
        }
        
        void setName(std::string name) {
            _name = name;
        }
        
        std::string getName() {
            return _name;
        }
        
        std::string GLSLTypeForGLType(unsigned int glType) {
            return this->_profile->getGLSLTypeForGLType(glType);
        }
        
        void _addDeclaration(std::string qualifier, std::string symbol, unsigned int type, size_t count) {
            std::string declaration = qualifier + " ";
            declaration += GLSLTypeForGLType(type);
            declaration += " " + symbol;
            if (count > 1) {
                declaration += "["+GLTFUtils::toString(count)+"]";
            }
            declaration += ";\n";
            _declarations += declaration;
        }
        
        void addAttribute(std::string symbol, unsigned int type) {
            _addDeclaration("attribute", symbol, type, 1);
        }
        
        void addUniform(std::string symbol, unsigned int type, size_t count) {
            _addDeclaration("uniform", symbol, type, count);
        }
        
        void addVarying(std::string symbol, unsigned int type) {
            _addDeclaration("varying", symbol, type, 1);
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
        
        shared_ptr <GLTFProfile> _profile;
    };
    
    class GLSLProgram {
    public:
        GLSLProgram(shared_ptr<GLTFProfile> profile) {
            this->_profile = profile;
            this->_uniforms = shared_ptr <GLTF::JSONObject>(new GLTF::JSONObject());
            this->_attributes = shared_ptr <GLTF::JSONObject>(new GLTF::JSONObject());
            this->_vertexShader = new GLSLShader(profile);
            this->_fragmentShader = new GLSLShader(profile);
        }
        
        virtual ~GLSLProgram() {
            delete this->_vertexShader;
            delete this->_fragmentShader;
        }

        void _nameDidChange() {
            this->_vertexShader->setName(_name + "VS");
            this->_fragmentShader->setName(_name + "FS");
        }
        
        void setName(std::string name) {
            this->_name = name;
            this->_nameDidChange();
        }
        
        std::string getName() {
            return this->_name;
        }
        
        shared_ptr <JSONObject> attributes() {
            return this->_attributes;
        }
        
        shared_ptr <JSONObject> uniforms() {
            return this->_uniforms;
        }
        
        GLSLShader* vertexShader() {
            return this->_vertexShader;
        }
        
        GLSLShader* fragmentShader() {
            return this->_fragmentShader;
        }
        
        void addVarying(std::string symbol, unsigned int type) {
            this->_vertexShader->addVarying(symbol, type);
            this->_fragmentShader->addVarying(symbol, type);
        }
        
    private:
        GLSLShader *_vertexShader;
        GLSLShader *_fragmentShader;
        std::string _name;
        
        shared_ptr <GLTF::JSONObject> _attributes;
        shared_ptr <GLTF::JSONObject> _uniforms;
        
        shared_ptr <GLTFProfile> _profile;
    };
    
    class Pass {
    public:
        Pass(shared_ptr <GLTFProfile> profile) {
            this->_profile = profile;
            this->_instanceProgram = new GLSLProgram(profile);
        }
        
        GLSLProgram* instanceProgram() {
            return this->_instanceProgram;
        }
        
        shared_ptr <JSONObject> getDetails(const std::string &lightingModel,
                                           shared_ptr<JSONObject> values,
                                           shared_ptr<JSONObject> techniqueExtras,
                                           shared_ptr<JSONObject> texcoordBindings,
                                           GLTFConverterContext& context) {
            shared_ptr <JSONObject> details(new JSONObject());
            
            shared_ptr <JSONObject> commonProfile = details->createObjectIfNeeded("commonProfile");
            shared_ptr <JSONObject> extras = commonProfile->createObjectIfNeeded("extras");
            
            details->setString("type", "COLLADA-1.4.1/commonProfile");
            
            shared_ptr<JSONArray> parameters(new JSONArray());
            
            shared_ptr <JSONObject> uniforms = _instanceProgram->uniforms();
            vector <std::string> keys = uniforms->getAllKeys();
            for (size_t i = 0 ; i < keys.size() ; i++) {
                std::string parameter = uniforms->getString(keys[i]);
                parameters->appendValue(shared_ptr <JSONValue> (new JSONString( parameter)));
            }
            commonProfile->setValue("parameters", parameters);
            
            commonProfile->setString("lightingModel", lightingModel);
            
            extras->setBool("doubleSided", techniqueExtras->getBool("double_sided"));
            
            if (texcoordBindings->getKeysCount() > 0) {
                commonProfile->setValue("texcoordBindings", texcoordBindings);
            }
            
            return details;
        }
        
    private:
        GLSLProgram* _instanceProgram;
        shared_ptr <GLTFProfile> _profile;
        shared_ptr <JSONObject> states;
    };
    
    class Technique {
    public:
        
        //FIXME: pass id when we support multipass
        Pass* getPass() {
            return this->_pass;
        }
        
        unsigned int typeForSemanticAttribute(const std::string& semantic) {
            static std::map<std::string , unsigned int> typeForSemanticAttribute;
            
            if (semantic.find("TEXCOORD") != string::npos) {
                return _GL(FLOAT_VEC2);
            }
            
            if (typeForSemanticAttribute.empty()) {
                typeForSemanticAttribute["POSITION"] = _GL(FLOAT_VEC3);
                typeForSemanticAttribute["NORMAL"] = _GL(FLOAT_VEC3);
                typeForSemanticAttribute["REFLECTIVE"] = _GL(FLOAT_VEC2);
                typeForSemanticAttribute["WEIGHT"] = _GL(FLOAT_VEC4);
                typeForSemanticAttribute["JOINT"] = _GL(FLOAT_VEC4);
                
            }
            
            return typeForSemanticAttribute[semantic];
        }
        
        unsigned int typeForSemanticUniform(const std::string& semantic) {
            static std::map<std::string , unsigned int> typeForSemanticUniform;
            if (typeForSemanticUniform.empty()) {
                typeForSemanticUniform["WORLDVIEWINVERSETRANSPOSE"] = _GL(FLOAT_MAT3);; //typically the normal matrix
                typeForSemanticUniform["WORLDVIEW"] = _GL(FLOAT_MAT4);
                typeForSemanticUniform["PROJECTION"] = _GL(FLOAT_MAT4);
                typeForSemanticUniform["JOINT_MATRIX"] = _GL(FLOAT_MAT4);
            }
            return typeForSemanticUniform[semantic];
        }
        
        //FIXME: it's a bad API since we won't have at the same time a varying an uniform...
        void addSemantic(std::string vertexOrFragment, std::string uniformOrAttribute,
                         std::string semantic,
                         std::string parameterID,
                         size_t count,
                         bool includesVarying) {
            
            std::string symbol = (uniformOrAttribute == "attribute") ? "a_" + parameterID : "u_" + parameterID;

            unsigned int type = (uniformOrAttribute == "uniform") ?
                                    typeForSemanticUniform(semantic) :
                                    typeForSemanticAttribute(semantic);
            
            shared_ptr <JSONObject> parameter(new GLTF::JSONObject());
            parameter->setString("semantic", semantic);
            parameter->setUnsignedInt32("type",  type);
            _parameters->setValue(parameterID, parameter);

            //FIXME: should not assume default pass / default program
            GLSLProgram* program = _pass->instanceProgram();
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
                shader->addUniform(symbol, type, count);
            }            
        }
        
        //FIXME: refactor with addSemantic
        shared_ptr <JSONObject> addParameter(std::string parameterID, unsigned int type) {
            shared_ptr <JSONObject> parameter(new GLTF::JSONObject());
            parameter->setUnsignedInt32("type",  type);
            _parameters->setValue(parameterID, parameter);
            
            return parameter;
        }
        
        shared_ptr <JSONObject> addValue(std::string vertexOrFragment,
                      std::string uniformOrAttribute,
                      unsigned int type,
                      size_t count,
                      std::string parameterID) {
                        
            std::string symbol = (uniformOrAttribute == "attribute") ? "a_" + parameterID : "u_" + parameterID;
                        
            //FIXME: should not assume default pass / default program
            GLSLProgram* program = _pass->instanceProgram();
            GLSLShader* shader = (vertexOrFragment == "vs") ? program->vertexShader() : program->fragmentShader();
            if (uniformOrAttribute == "attribute") {
                program->attributes()->setString(symbol, parameterID);
            } else if (uniformOrAttribute == "uniform") {
                program->uniforms()->setString(symbol, parameterID);
            } else {
                printf("cannot add semantic of unknown kind %s\n", uniformOrAttribute.c_str());
            }
            
            if (uniformOrAttribute == "attribute") {
                shader->addAttribute(symbol, type);
            } else {
                shader->addUniform(symbol, type, count);
            }
            
            return this->addParameter(parameterID, type);
        }
        
        Technique(const std::string &lightingModel,
                  shared_ptr<JSONObject> attributeSemantics,
                  std::string techniqueID,
                  shared_ptr<JSONObject> values,
                  shared_ptr<JSONObject> techniqueExtras,
                  shared_ptr<JSONObject> texcoordBindings,
                  GLTFConverterContext& converterContext) {
            
            this->_profile = converterContext.profile;
            
            unsigned int vec3Type =  _GL(FLOAT_VEC3);
            unsigned int vec4Type = _GL(FLOAT_VEC4);
            unsigned int mat4Type = _GL(FLOAT_MAT4);
            unsigned int sampler2DType = _GL(SAMPLER_2D);
            
            this->_pass = new Pass(this->_profile);
            this->_parameters = shared_ptr<GLTF::JSONObject>(new GLTF::JSONObject());
            
            shared_ptr <JSONObject> inputParameters = values;
            
            bool useSimpleLambert = !(inputParameters->contains("specular") &&
                                     inputParameters->contains("shininess"));
            
            bool hasSkinning = attributeSemantics->contains("WEIGHT") && attributeSemantics->contains("JOINT");
            
            std::vector <std::string> allAttributes;
            std::vector <std::string> allUniforms;
            std::string shaderName = techniqueID;
            
            GLSLProgram* program = this->_pass->instanceProgram();
            GLSLShader* vertexShader = program->vertexShader();
            GLSLShader* fragmentShader = program->fragmentShader();
            program->setName(shaderName);

            //position attribute
            addSemantic("vs", "attribute",
                        "POSITION", "position" , 1, false);
            
            //normal attribute
            addSemantic("vs", "attribute",
                        "NORMAL", "normal", 1, true);

            if (hasSkinning) {
                addSemantic("vs", "attribute",
                            "JOINT", "joint", 1, true);
                addSemantic("vs", "attribute",
                            "WEIGHT", "weight", 1, true);
                //addValue("vs", "uniform",   "FLOAT_MAT4", 60, "jointMat");
                addSemantic("vs", "uniform",
                            "JOINT_MATRIX", "jointMat", 60, false);

            }
            
            //normal matrix
            addSemantic("vs", "uniform",
                        "WORLDVIEWINVERSETRANSPOSE", "normalMatrix" , 1, false);

            //worldview matrix
            addSemantic("vs", "uniform",
                        "WORLDVIEW", "worldViewMatrix" , 1, false);
            
            //projection matrix
            addSemantic("vs", "uniform",
                        "PROJECTION", "projectionMatrix" , 1, false);
            
           
            /* 
                Handle hardware skinning, for now with a fixed limit of 4 influences
             */
            
            if (hasSkinning) {
                vertexShader->appendCode("mat4 skinMat = a_weight.x * u_jointMat[int(a_joint.x)];\n");
                vertexShader->appendCode("skinMat += a_weight.y * u_jointMat[int(a_joint.y)];\n");
                vertexShader->appendCode("skinMat += a_weight.z * u_jointMat[int(a_joint.z)];\n");
                vertexShader->appendCode("skinMat += a_weight.w * u_jointMat[int(a_joint.w)];\n");
                vertexShader->appendCode("%s pos = %s * skinMat * vec4(%s,1.0);\n",
                                         vertexShader->GLSLTypeForGLType(vec4Type).c_str(),
                                         "u_worldViewMatrix",
                                         "a_position");
                vertexShader->appendCode("%s = normalize(%s * mat3(skinMat)* %s);\n",
                                         "v_normal", "u_normalMatrix", "a_normal");
                
            } else {
                vertexShader->appendCode("%s pos = %s * vec4(%s,1.0);\n",
                                         vertexShader->GLSLTypeForGLType(vec4Type).c_str(),
                                         "u_worldViewMatrix",
                                         "a_position");
                vertexShader->appendCode("%s = normalize(%s * %s);\n",
                                         "v_normal", "u_normalMatrix", "a_normal");
                
            }
            
            bool modelContainsLights = false;
            if (converterContext.root->contains("lightsIds")) {
                shared_ptr<JSONArray> lightsIds = converterContext.root->createArrayIfNeeded("lightsIds");
                std::vector <shared_ptr <JSONValue> > ids = lightsIds->values();
                if (ids.size() > 0) {
                    if (ids.size() == 1) {
                        shared_ptr<JSONString> lightUID = static_pointer_cast<JSONString>(ids[0]);
                        shared_ptr<JSONArray> lightsNodesIds = static_pointer_cast<JSONArray>(converterContext._uniqueIDOfLightToNodes[lightUID->getString()]);
                        
                        shared_ptr<JSONObject> lights = converterContext.root->createObjectIfNeeded("lights");
                        shared_ptr<JSONObject> light = lights->getObject(lightUID->getString());
                        std::string lightType = light->getString("type");
                        
                        //we ignore lighting if the only light we have is ambient
                        modelContainsLights = lightType != "ambient";
                    } else {
                        modelContainsLights = true;
                    }
                }
            }
            
            
            bool lightingIsEnabled = modelContainsLights || converterContext.useDefaultLight;
            
            if (lightingIsEnabled) {
                fragmentShader->appendCode("vec3 normal = normalize(%s);\n", "v_normal");
                if (techniqueExtras->getBool("double_sided")) {
                    fragmentShader->appendCode("if (gl_FrontFacing == false) normal = -normal;\n");
                }
            } else {
                //https://github.com/KhronosGroup/glTF/issues/121
                //we want to keep consistent the parameter in the instanceTechnique and the ones actually in use in the technique.
                //Given the context, some parameters from instanceTechnique will be removed because they aren't used in the resulting shader. As an example, we may have no light declared and the default lighting disabled == no lighting at all, but still a specular color and a shininess. in this case specular and shininess won't be used 
                
            }
            
            //color to cumulate all components and light contribution
            fragmentShader->appendCode("vec4 color = vec4(0., 0., 0., 0.);\n");
            fragmentShader->appendCode("vec4 diffuse = vec4(0., 0., 0., 1.);\n");
            if (modelContainsLights)
                fragmentShader->appendCode("vec3 diffuseLight = vec3(0., 0., 0.);\n");
            
            if (slotIsContributingToLighting("emission", inputParameters, converterContext)) {
                fragmentShader->appendCode("vec4 emission;\n");
            }
            
            if (slotIsContributingToLighting("reflective", inputParameters, converterContext)) {
                fragmentShader->appendCode("vec4 reflective;\n");
            }
            if (lightingIsEnabled && slotIsContributingToLighting("ambient", inputParameters, converterContext)) {
                fragmentShader->appendCode("vec4 ambient;\n");
                fragmentShader->appendCode("vec3 ambientLight = vec3(0., 0., 0.);\n");
            }
            if (lightingIsEnabled && slotIsContributingToLighting("specular", inputParameters, converterContext)) {
                fragmentShader->appendCode("vec4 specular;\n");
                fragmentShader->appendCode("vec3 specularLight = vec3(0., 0., 0.);\n");
            }
                                    
            /* 
             Handle lighting
             */
            
            shared_ptr <JSONObject> shininessObject;
            
            size_t lightIndex = 0;
            if (modelContainsLights && converterContext.root->contains("lightsIds")) {
                shared_ptr<JSONArray> lightsIds = converterContext.root->createArrayIfNeeded("lightsIds");
                std::vector <shared_ptr <JSONValue> > ids = lightsIds->values();
                
                for (size_t i = 0 ; i < ids.size() ; i++) {
                    shared_ptr<JSONString> lightUID = static_pointer_cast<JSONString>(ids[i]);
                    shared_ptr<JSONArray> lightsNodesIds = static_pointer_cast<JSONArray>(converterContext._uniqueIDOfLightToNodes[lightUID->getString()]);
                    
                    shared_ptr<JSONObject> lights = converterContext.root->createObjectIfNeeded("lights");
                    shared_ptr<JSONObject> light = lights->getObject(lightUID->getString());                    
                    std::string lightType = light->getString("type");
                    
                    //we ignore lighting if the only light we have is ambient
                    if ((lightType == "ambient") && ids.size() == 1)
                        continue;
                    
                    
                    shared_ptr<JSONObject> description = light->getObject(lightType);
                    std::vector <shared_ptr <JSONValue> > nodesIds = lightsNodesIds->values();
                    for (size_t j = 0 ; j < nodesIds.size() ; j++, lightIndex++) {
                        
                        //each light needs to be re-processed for each node
                        char lightIndexCStr[100];
                        char lightColor[100];
                        char lightTransform[100];
                        sprintf(lightIndexCStr, "light%d", (int)lightIndex);
                        sprintf(lightColor, "%sColor", lightIndexCStr);
                        sprintf(lightTransform, "%sTransform", lightIndexCStr);
                        fragmentShader->appendCode("{\n");
                        
                        shared_ptr <JSONObject> lightColorParameter = addValue("fs", "uniform", vec3Type, 1, lightColor);
                        lightColorParameter->setValue("value", description->getValue("color"));
                        
                        fragmentShader->appendCode("float diffuseIntensity;\n");
                        fragmentShader->appendCode("float specularIntensity;\n");
                        
                        if (lightType == "ambient") {
                            //FIXME: what happens if multiple ambient light ?
                            fragmentShader->appendCode("ambientLight += u_%s;\n", lightColor);
                            
                        } else if (1 /*lightType == "directional"*/) {
                            char varyingLightDirection[100];
                            sprintf(varyingLightDirection, "v_%sDirection", lightIndexCStr);
                            
                            shared_ptr <JSONObject> lightTransformParameter = addValue("vs", "uniform", mat4Type, 1, lightTransform);
                            
                            vertexShader->appendCode("%s = normalize(mat3(u_%s) * vec3(0.,0.,1.));\n", varyingLightDirection, lightTransform);

                            program->addVarying(varyingLightDirection, vec3Type);
                            lightTransformParameter->setValue("source", nodesIds[j]);

                            if (!useSimpleLambert) {
                                if (inputParameters->contains("shininess") && (!shininessObject)) {
                                    shininessObject = inputParameters->getObject("shininess");
                                    addValue("fs", "uniform", shininessObject->getUnsignedInt32("type") , 1, "shininess");
                                }
                                
                                fragmentShader->appendCode("vec3 l = normalize(%s);\n", varyingLightDirection);
                                fragmentShader->appendCode("vec3 h = normalize(l+vec3(0.,0.,1.));\n");
                                fragmentShader->appendCode("diffuseIntensity = max(dot(normal,l), 0.);\n");
                                fragmentShader->appendCode("specularIntensity = pow(max(0.0,dot(normal,h)),u_shininess);\n");
                                fragmentShader->appendCode("specularLight += u_%s * specularIntensity;\n", lightColor);
                                
                            } else {
                                fragmentShader->appendCode("diffuseIntensity = max(dot(normal,normalize(%s)), 0.);\n",varyingLightDirection);
                            }
                            
                            fragmentShader->appendCode("diffuseLight += u_%s * diffuseIntensity;\n", lightColor);


                        } else if (lightType == "spot") {
                            
                        } else if (lightType == "point") {
                            
                        } else {
                            //FIXME: report error
                            
                        }
                        fragmentShader->appendCode("}\n");
                    }
                }
            }
            
            //Currently the declaration of the shininess is dependent of the presence of light.
            //if we want details, we want also all parameters.
            if (converterContext.exportPassDetails) {
                if (inputParameters->contains("shininess") && (!shininessObject)) {
                    shininessObject = inputParameters->getObject("shininess");
                    addValue("fs", "uniform",   shininessObject->getUnsignedInt32("type"), 1, "shininess");
                }
            }
            
            //texcoords
            std::string texcoordAttributeSymbol = "a_texcoord";
            std::string texcoordVaryingSymbol = "v_texcoord";
            std::map<std::string , std::string> declaredTexcoordAttributes;
            std::map<std::string , std::string> declaredTexcoordVaryings;
            
            const int slotsCount = 5;
            std::string slots[slotsCount] = { "ambient", "diffuse", "emission", "reflective", "specular" };
            for (size_t slotIndex = 0 ; slotIndex < slotsCount ; slotIndex++) {
                std::string slot = slots[slotIndex];
                
                if (inputParameters->contains(slot) == false)
                    continue;
                
                if (!slotIsContributingToLighting(slot, inputParameters, converterContext))
                    continue;
                
                shared_ptr <JSONObject> param = inputParameters->getObject(slot);
                unsigned int slotType = param->getUnsignedInt32("type");

                if ((!lightingIsEnabled) && ((slot == "ambient") || (slot == "specular"))) {
                    //as explained in https://github.com/KhronosGroup/glTF/issues/121 export all parameters when details is specified
                    if (converterContext.exportPassDetails)
                        addParameter(slot, slotType);
                    continue;
                }
                
                if (slotType == vec4Type) {
                    std::string slotColorSymbol = "u_"+slot;
                    fragmentShader->appendCode("%s = %s;\n", slot.c_str(), slotColorSymbol.c_str());
                    
                    addValue("fs", "uniform",   slotType, 1, slot);
                } else if (slotType == sampler2DType) {
                    std::string semantic = texcoordBindings->getString(slot);
                    if (semantic.length() == 0) {
                        semantic = "TEXCOORD_0";
                    }
                    
                    std::string texSymbol;
                    std::string texVSymbol;
                    
                    if (slot == "reflective") {
                        texVSymbol = texcoordVaryingSymbol + GLTFUtils::toString(declaredTexcoordVaryings.size());
                        unsigned int reflectiveType = typeForSemanticAttribute("REFLECTIVE");
                        program->addVarying(texVSymbol, reflectiveType);
                        
                        //Update Vertex shader for reflection
                        std::string normalType = vertexShader->GLSLTypeForGLType(vec3Type);
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
                                        semantic, "texcoord" + GLTFUtils::toString(declaredTexcoordAttributes.size()), 1, false);
                            program->addVarying(texVSymbol, typeForSemanticAttribute(semantic));
                            
                            vertexShader->appendCode("%s = %s;\n", texVSymbol.c_str(), texSymbol.c_str());
                            declaredTexcoordAttributes[semantic] = texSymbol;
                            declaredTexcoordVaryings[semantic] = texVSymbol;
                        } else {
                            texSymbol =  declaredTexcoordAttributes[semantic];
                            texVSymbol = declaredTexcoordVaryings[semantic];
                        }
                    }
                    
                    std::string textureSymbol = "u_"+ slot;
                    
                    //get the texture
                    shared_ptr <JSONObject> textureParameter = inputParameters->getObject(slot);
                    //FIXME:this should eventually not come from the inputParameter
                    addValue("fs", "uniform", textureParameter->getUnsignedInt32("type"), 1, slot);

                    fragmentShader->appendCode("%s = texture2D(%s, %s);\n", slot.c_str(), textureSymbol.c_str(), texVSymbol.c_str());
                }
            }
            
            if (slotIsContributingToLighting("reflective", inputParameters, converterContext)) {
                fragmentShader->appendCode("diffuse.xyz += reflective.xyz;\n");
            }
            if (lightingIsEnabled && slotIsContributingToLighting("ambient", inputParameters, converterContext)) {
                fragmentShader->appendCode("ambient.xyz *= ambientLight;\n");
                fragmentShader->appendCode("color.xyz += ambient.xyz;\n");
            }
            if (lightingIsEnabled && slotIsContributingToLighting("specular", inputParameters, converterContext)) {
                fragmentShader->appendCode("specular.xyz *= specularLight;\n");
                fragmentShader->appendCode("color.xyz += specular.xyz;\n");
            }
            if (modelContainsLights) {
                fragmentShader->appendCode("diffuse.xyz *= diffuseLight;\n");
            } else if (converterContext.useDefaultLight) {
                fragmentShader->appendCode("diffuse.xyz *= max(dot(normal,vec3(0.,0.,1.)), 0.);\n");
            }
            fragmentShader->appendCode("color.xyz += diffuse.xyz;\n");
            
            if (slotIsContributingToLighting("emission", inputParameters, converterContext)) {
                fragmentShader->appendCode("color.xyz += emission.xyz;\n");
            }
                        
            bool hasTransparency = inputParameters->contains("transparency");
            if (hasTransparency) {
                std::string slot = "transparency";
                shared_ptr <JSONObject> transparencyParam = inputParameters->getObject(slot);
                
                addValue("fs", "uniform",   transparencyParam->getUnsignedInt32("type"), 1, slot);
                
                fragmentShader->appendCode("color = vec4(color.rgb * diffuse.a, diffuse.a * %s);\n", "u_transparency");
            } else {
                fragmentShader->appendCode("color = vec4(color.rgb * diffuse.a, diffuse.a);\n");
            }
            
            if (converterContext.alwaysExportFilterColor) {
                if (inputParameters->contains("filterColor")) {
                    shared_ptr<JSONObject> filterColor = inputParameters->getObject("filterColor");
                    shared_ptr <JSONObject> filterColorParameter = addValue("fs", "uniform", vec4Type, 1, "filterColor");
                    filterColorParameter->setValue("value", filterColor->getValue("value"));
                    fragmentShader->appendCode("color *= u_filterColor;\n");
                }
            }

            fragmentShader->appendCode("gl_FragColor = color;\n");

            vertexShader->appendCode("gl_Position = %s * pos;\n",
                                     "u_projectionMatrix");
            
            vertexShader->appendCode("}\n");
            fragmentShader->appendCode("}\n");
            
        }
        
        shared_ptr <GLTF::JSONObject> parameters() {
            return _parameters;
        }
        
    private:
        Pass *_pass;
        shared_ptr <GLTF::JSONObject> _parameters;
        shared_ptr <GLTFProfile> _profile;
        
    };
    
    std::string getReferenceTechniqueID(shared_ptr<JSONObject> techniqueGenerator, GLTFConverterContext& context) {
        
        std::string lightingModel = techniqueGenerator->getString("lightingModel");
        shared_ptr<JSONObject> attributeSemantics = techniqueGenerator->getObject("attributeSemantics");
        shared_ptr<JSONObject> values = techniqueGenerator->getObject("values");
        shared_ptr<JSONObject> techniqueExtras = techniqueGenerator->getObject("techniqueExtras");
        shared_ptr<JSONObject> texcoordBindings = techniqueGenerator->getObject("texcoordBindings");
        
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
        
        GLTF::Technique glTFTechnique(lightingModel, attributeSemantics, techniqueID, values, techniqueExtras, texcoordBindings, context);
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

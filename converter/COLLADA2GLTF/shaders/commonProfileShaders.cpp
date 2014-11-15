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
#include "../GLTFOpenCOLLADA.h"

#include "GLTFAsset.h"
#include "commonProfileShaders.h"
#include "helpers/encodingHelpers.h"
#ifdef USE_LIBPNG
#include "png.h"
#endif

#include "stdarg.h"

#define _GL_STR(X) #X
#define _GL(X) (this->_profile->getGLenumForString(_GL_STR(X)));


typedef enum {
    phong = 0,
    blinn,
    constant,
    lambert
} GLTFLightingModel;

using namespace std;

namespace GLTF
{
#define PNGSIGSIZE 8
#ifdef USE_LIBPNG
    void userReadData(png_structp pngPtr, png_bytep data, png_size_t length) {
        ((std::istream*)png_get_io_ptr(pngPtr))->read((char*)data, length);
    }
#endif
    //thanks to piko3d.com libpng tutorial here
    static bool imageHasAlpha(const char *path)
    {
#ifdef USE_LIBPNG
        bool hasAlpha = false;
        std::shared_ptr<std::istream> source;
        if (is_dataUri(path))
        {
            std::stringstream* sin = new std::stringstream();
            sin->str(decode_dataUri(path));
            source.reset(sin);
        }
        else
        {
            std::ifstream* fin = new std::ifstream();
            fin->open(path, ios::in | ios::binary);
            source.reset(fin);
        }
        
        //        printf("path:%s\n",path);
        png_byte pngsig[PNGSIGSIZE];
        int isPNG = 0;
        source->read((char*)pngsig, PNGSIGSIZE);
        if (!source->good())
            return false;
        isPNG = png_sig_cmp(pngsig, 0, PNGSIGSIZE) == 0;
        if (isPNG) {
            png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
            if (pngPtr) {
                png_infop infoPtr = png_create_info_struct(pngPtr);
                if (infoPtr) {
                    png_set_read_fn(pngPtr,(png_voidp)source.get(), userReadData);
                    png_set_sig_bytes(pngPtr, PNGSIGSIZE);
                    png_read_info(pngPtr, infoPtr);
                    png_uint_32 color_type = png_get_color_type(pngPtr, infoPtr);
                    if (color_type == PNG_COLOR_TYPE_RGB_ALPHA || color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
                        int width = png_get_image_width(pngPtr, infoPtr);
                        int height = png_get_image_height(pngPtr, infoPtr);
                        png_byte bit_depth = png_get_bit_depth(pngPtr, infoPtr);

                        if (bit_depth == 16)
                            png_set_strip_16(pngPtr);

                        if (png_get_valid(pngPtr, infoPtr, PNG_INFO_tRNS))
                            png_set_tRNS_to_alpha(pngPtr);

                        if (color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
                            png_set_gray_to_rgb(pngPtr);

                        png_read_update_info(pngPtr, infoPtr);

                        // Allocate row pointers
                        png_bytep *row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
                        png_size_t rowBytes = png_get_rowbytes(pngPtr, infoPtr);
                        for (int y = 0; y < height; y++) {
                            row_pointers[y] = (png_byte*)malloc(rowBytes);
                        }

                        png_read_image(pngPtr, row_pointers);

                        // Check for alpha less than 255
                        for (int y = 0; (y < height) && !hasAlpha; y++) {
                            png_bytep row = row_pointers[y];
                            for (int x = 0; (x < width) && !hasAlpha; x++) {
                                png_bytep px = &(row[x * 4]);
                                if (px[3] != 255) {
                                    hasAlpha = true;
                                }
                            }
                        }

                        // Free row pointers
                        for (int y = 0; y < height; y++) {
                            free(row_pointers[y]);
                        }
                        free(row_pointers);
                    }
                    
                    png_destroy_read_struct(&pngPtr, (png_infopp)0, (png_infopp)0);
                }
            }
        }
        
        return hasAlpha;
#else
		return false;
#endif
    }
    
    
    //Not yet implemented for everything
    static bool slotIsContributingToLighting(const std::string &slot, shared_ptr <JSONObject> inputParameters,  GLTFAsset* asset) {
        if (inputParameters->contains(slot)) {
            if (CONFIG_BOOL(asset, "optimizeParameters") == false)
                return true;
            
            //FIXME: we need an explicit option to allow this, and make sure we get then consistent instanceTechnique and technique parameters
            shared_ptr <JSONObject> param = inputParameters->getObject(slot);
            
            if (param->getUnsignedInt32(kType) == asset->profile()->getGLenumForString("SAMPLER_2D"))
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
    
    static double getTransparency(shared_ptr<JSONObject> parameters, GLTFAsset* asset) {
        //super naive for now, also need to check sketchup work-around
        //if (effectCommon->getOpacity().isTexture()) {
        //    return 1;
        //}
        
        if (!parameters->contains("transparency")) {
            return 1;
        }
        
        shared_ptr<JSONObject> tr = parameters->getObject("transparency");
        
        double transparency = tr->getDouble("value");
        
        return CONFIG_BOOL(asset, "invertTransparency") ? 1 - transparency : transparency;
    }
    
    static bool hasTransparency(shared_ptr<JSONObject> parameters,
                                GLTFAsset* asset) {
        return getTransparency(parameters, asset)  < 1;
    }
    
    static bool isOpaque(shared_ptr <JSONObject> parameters, GLTFAsset* asset) {
        
        if (parameters->contains("diffuse")) {
            shared_ptr <JSONObject> diffuse = parameters->getObject("diffuse");
            
            if (diffuse->getUnsignedInt32(kType) == asset->profile()->getGLenumForString("SAMPLER_2D")) {
                shared_ptr<JSONObject> textures = asset->root()->createObjectIfNeeded("textures");
                if (textures->getKeysCount() == 0) {
                    return false;
                }
                shared_ptr<JSONObject> texture = textures->getObject(diffuse->getString("value"));
                std::string sourceUID = texture->getString(kSource);
                shared_ptr<JSONObject> images = asset->root()->createObjectIfNeeded(kImages);
                
                if (images->contains(sourceUID)) {
                    shared_ptr<JSONObject> image = images->getObject(sourceUID);
                    std::string imagePath = image->getString(kURI);
                    COLLADABU::URI inputURI(asset->getInputFilePath().c_str());
                    std::string imageFullPath = inputURI.getPathDir() + imagePath;
                    if (imageHasAlpha(imageFullPath.c_str()))
                        return false;
                } else {
                    static bool printedOnce = false;
                    if (!printedOnce) {
                        printedOnce = true;
                        asset->log("Inconsistency error: this asset probably refers to invalid image Ids within <surface>\n");
                    }
                    return false;
                }
            }
        }
        return !hasTransparency(parameters, asset);
    }
    /*
     From https://github.com/KhronosGroup/glTF/issues/83#issuecomment-24095883
     
     MODEL - mat4 or mat3 - Transforms from model to world coordinates using the transform's node and all of its parents.
     VIEW - mat4 or mat3 - Transforms from world to view coordinates using the active camera node.
     PROJECTION - mat4 or mat3 - Transforms from view to clip coordinates using the active camera node.
     MODELVIEW - mat4 or mat3 - Combined MODEL and VIEW.
     MODELVIEWPROJECTION - mat4 or mat3 - Combined MODEL, VIEW, and PROJECTION.
     Inverses
     
     MODELINVERSE - mat4 or mat3 - Inverse of MODEL.
     VIEWINVERSE - mat4 or mat3 - Inverse of VIEW.
     PROJECTIONINVERSE - mat4 or mat3 - Inverse of PROJECTION.
     MODELVIEWINVERSE - mat4 or mat3 - Inverse of MODELVIEW.
     MODELVIEWPROJECTIONINVERSE - mat4 or mat3 - Inverse of MODELVIEWPROJECTION.
     Inverse transposes
     
     MODELINVERSETRANSPOSE - mat3 or mat2 - The inverse-transpose of MODEL without the translation. This translates normals in model coordinates to world coordinates.
     MODELVIEWINVERSETRANSPOSE - mat3 or mat2 - The inverse-transpose of MODELVIEW without the translation. This translates normals in model coordinates to eye coordinates.
     */
    
    
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
    
    static std::string buildSlotHash(shared_ptr<JSONObject> &parameters, std::string slot, GLTFAsset* asset) {
        std::string hash = slot + ":";
        
        if (slotIsContributingToLighting(slot, parameters, asset)) {
            if (parameters->contains(slot)) {
                shared_ptr<JSONObject> parameter = parameters->getObject(slot);
                if (parameter->contains(kType)) {
                    hash += GLTFUtils::toString(parameter->getUnsignedInt32(kType));
                    return hash;
                }
            }
        } else if (parameters->contains(slot) && slot != "diffuse") {
            parameters->removeValue(slot);
        }
        return hash + "none";
    }
    
    /*
     static std::string buildLightsHash(shared_ptr<JSONObject> parameters, shared_ptr<JSONObject> techniqueExtras, GLTFAsset& context) {
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
    static std::string buildTechniqueHash(shared_ptr<JSONObject> parameters, shared_ptr<JSONObject> techniqueExtras, GLTFAsset* asset) {
        std::string techniqueHash = "";
        bool doubleSided = false;
        unsigned int jointsCount = 0;
        
        techniqueHash += buildSlotHash(parameters, "diffuse", asset);
        techniqueHash += buildSlotHash(parameters, "ambient", asset);
        techniqueHash += buildSlotHash(parameters, "emission", asset);
        techniqueHash += buildSlotHash(parameters, "specular", asset);
        techniqueHash += buildSlotHash(parameters, "reflective", asset);
        techniqueHash += buildSlotHash(parameters, "bump", asset);
        
        //techniqueHash += buildLightsHash(parameters, techniqueExtras, context);
        
        if (techniqueExtras) {
            jointsCount = techniqueExtras->getUnsignedInt32("jointsCount");
            doubleSided = techniqueExtras->getBool(kDoubleSided);
        }
        
        techniqueHash += "double_sided:" + GLTFUtils::toString(doubleSided);
        techniqueHash += "jointsCount:" + GLTFUtils::toString(jointsCount);
        techniqueHash += "opaque:"+ GLTFUtils::toString(isOpaque(parameters, asset));
        techniqueHash += "hasTransparency:"+ GLTFUtils::toString(hasTransparency(parameters, asset));
        
        return techniqueHash;
    }
    
    bool writeShaderIfNeeded(const std::string& shaderId,  const std::string& shaderString, GLTFAsset *asset, unsigned int type)
    {
        shared_ptr <JSONObject> shadersObject = asset->root()->createObjectIfNeeded("shaders");
        
        if (shadersObject->contains(shaderId) == false) {
            shared_ptr <JSONObject> shaderObject  = shadersObject->getObject(shaderId);
            shaderObject = shared_ptr <GLTF::JSONObject> (new GLTF::JSONObject());
            
            shadersObject->setValue(shaderId, shaderObject);
			shaderObject->setUnsignedInt32(kType, type);
            if (CONFIG_BOOL(asset, "embedResources"))
			{
				shaderObject->setString(kURI, create_dataUri(shaderString, "text/plain"));
			}
			else
			{
				std::string path = shaderId + ".glsl";
                shaderObject->setString(kURI, COLLADABU::URI::uriEncode(asset->resourceOuputPathForPath(path)));
				//also write the file on disk
				if (shaderString.size() > 0) {
					COLLADABU::URI outputURI(asset->getOutputFilePath());
					std::string shaderPath = outputURI.getPathDir() + path;
					GLTF::GLTFUtils::writeData(shaderPath, "w", (unsigned char*)shaderString.c_str(), shaderString.size());
					if (!CONFIG_BOOL(asset, "outputProgress") && asset->converterConfig()->boolForKeyPath("verboseLogging")) {
						asset->log("[shader]: %s\n", shaderPath.c_str());
					}
				}
			}
        }
        
        return true;
    }
    
    static shared_ptr <JSONObject> createStatesForTechnique(shared_ptr<JSONObject> parameters, shared_ptr<JSONObject> techniqueExtras, GLTFAsset* asset)
    {
        shared_ptr <JSONObject> functions(new GLTF::JSONObject());
        shared_ptr <JSONObject> states(new GLTF::JSONObject());
        shared_ptr <JSONArray> enableArray(new GLTF::JSONArray());
        shared_ptr <GLTFProfile> profile = asset->profile();
        
        if (techniqueExtras->getBool(kDoubleSided) == false) {
            enableArray->appendValue(shared_ptr<JSONNumber> (new JSONNumber(profile->getGLenumForString("CULL_FACE"))));
        }
        
        enableArray->appendValue(shared_ptr<JSONNumber> (new JSONNumber(profile->getGLenumForString("DEPTH_TEST"))));

        if (isOpaque(parameters, asset) == false) {
            enableArray->appendValue(shared_ptr<JSONNumber> (new JSONNumber(profile->getGLenumForString("BLEND"))));
            
            unsigned func_add = profile->getGLenumForString("FUNC_ADD");
            shared_ptr <JSONArray> blendEquationSeparate(new GLTF::JSONArray());
            blendEquationSeparate->appendValue(shared_ptr<JSONNumber> (new JSONNumber(func_add)));
            blendEquationSeparate->appendValue(shared_ptr<JSONNumber> (new JSONNumber(func_add)));
            functions->setValue("blendEquationSeparate", blendEquationSeparate);
            
            shared_ptr <JSONArray> blendFuncSeparate(new GLTF::JSONArray());
            if (CONFIG_BOOL(asset, "premultipliedAlpha")) {
                blendFuncSeparate->appendValue(shared_ptr<JSONNumber> (new JSONNumber(profile->getGLenumForString("ONE"))));
                blendFuncSeparate->appendValue(shared_ptr<JSONNumber> (new JSONNumber(profile->getGLenumForString("ONE_MINUS_SRC_ALPHA"))));
                blendFuncSeparate->appendValue(shared_ptr<JSONNumber> (new JSONNumber(profile->getGLenumForString("ONE"))));
                blendFuncSeparate->appendValue(shared_ptr<JSONNumber> (new JSONNumber(profile->getGLenumForString("ONE_MINUS_SRC_ALPHA"))));
            } else {
                blendFuncSeparate->appendValue(shared_ptr<JSONNumber> (new JSONNumber(profile->getGLenumForString("SRC_ALPHA"))));
                blendFuncSeparate->appendValue(shared_ptr<JSONNumber> (new JSONNumber(profile->getGLenumForString("ONE_MINUS_SRC_ALPHA"))));
                blendFuncSeparate->appendValue(shared_ptr<JSONNumber> (new JSONNumber(profile->getGLenumForString("SRC_ALPHA"))));
                blendFuncSeparate->appendValue(shared_ptr<JSONNumber> (new JSONNumber(profile->getGLenumForString("ONE_MINUS_SRC_ALPHA"))));
            }
            functions->setValue("blendFuncSeparate", blendFuncSeparate) ;
        }
        
        if (enableArray->getCount() > 0) {
            states->setValue("enable", enableArray);
        }
        if (functions->getKeysCount() > 0) {
            states->setValue("functions", functions);
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
    private:
        void _commonInit(shared_ptr <GLTFProfile> profile) {
            this->_declarations += "precision highp float;\n";;
            this->_body = "void main(void) {\n";
            this->_profile = profile;
        }
    public:

        GLSLShader(GLTFAsset *asset) {
            this->_declarations = "";
            if (asset->converterConfig()->config()->contains("glslVersion")) {
                this->_declarations += "#version " + GLTFUtils::toString(asset->converterConfig()->config()->getInt32("glslVersion")) + "\n";
            }
            
            _commonInit(asset->profile());
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
        
        void _addDeclaration(std::string qualifier, std::string symbol, unsigned int type, size_t count, bool forcesAsAnArray = false) {
            if (this->hasSymbol(symbol) == false) {
                std::string declaration = qualifier + " ";
                declaration += GLSLTypeForGLType(type);
                declaration += " " + symbol;
                if ((count > 1) || forcesAsAnArray) {
                    declaration += "["+GLTFUtils::toString(count)+"]";
                }
                declaration += ";\n";
                _declarations += declaration;
                
                _allSymbols[symbol] = symbol;
            }
        }
        
        void addAttribute(std::string symbol, unsigned int type) {
            _addDeclaration("attribute", symbol, type, 1);
        }
        
        void addUniform(std::string symbol, unsigned int type, size_t count, bool forcesAsAnArray = false) {
            _addDeclaration("uniform", symbol, type, count, forcesAsAnArray);
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
        
        bool hasSymbol(const std::string &symbol) {
            return _allSymbols.count(symbol) > 0;
        }
        
    private:
        std::string _name;
        std::string _declarations;
        std::string _body;
        
        std::map<std::string, std::string> _allSymbols;
        
        shared_ptr <GLTFProfile> _profile;
    };
    
    class GLSLProgram {
    public:
        GLSLProgram(GLTFAsset *asset) {
            this->_profile = asset->profile();
            this->_uniforms = shared_ptr <GLTF::JSONObject>(new GLTF::JSONObject());
            this->_attributes = shared_ptr <GLTF::JSONObject>(new GLTF::JSONObject());
            this->_vertexShader = new GLSLShader(asset);
            this->_fragmentShader = new GLSLShader(asset);
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
        Pass(GLTFAsset * asset) {
            this->_profile = asset->profile();
            this->_instanceProgram = new GLSLProgram(asset);
        }

        ~Pass() {
            delete _instanceProgram;
        }
        
        GLSLProgram* instanceProgram() {
            return this->_instanceProgram;
        }
        
        shared_ptr <JSONObject> getDetails(const std::string &lightingModel,
                                           shared_ptr<JSONObject> values,
                                           shared_ptr<JSONObject> techniqueExtras,
                                           shared_ptr<JSONObject> texcoordBindings,
                                           GLTFAsset *asset) {
            shared_ptr <JSONObject> details(new JSONObject());
            
            shared_ptr <JSONObject> commonProfile = details->createObjectIfNeeded("commonProfile");
            shared_ptr <JSONObject> extras = commonProfile->createObjectIfNeeded("extras");
            
            details->setString(kType, "COLLADA-1.4.1/commonProfile");
            
            shared_ptr<JSONArray> parameters(new JSONArray());
            
            shared_ptr <JSONObject> uniforms = _instanceProgram->uniforms();
            vector <std::string> keys = uniforms->getAllKeys();
            for (size_t i = 0 ; i < keys.size() ; i++) {
                std::string parameter = uniforms->getString(keys[i]);
                parameters->appendValue(shared_ptr <JSONValue> (new JSONString( parameter)));
            }
            commonProfile->setValue("parameters", parameters);
            
            commonProfile->setString("lightingModel", lightingModel);
            
            extras->setBool("doubleSided", techniqueExtras->getBool(kDoubleSided));
            
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
                typeForSemanticAttribute["TEXTANGENT"] = _GL(FLOAT_VEC3);
                typeForSemanticAttribute["TEXBINORMAL"] = _GL(FLOAT_VEC3);
            }
            
            return typeForSemanticAttribute[semantic];
        }
        
        unsigned int typeForSemanticUniform(const std::string& semantic) {
            static std::map<std::string , unsigned int> typeForSemanticUniform;
            if (typeForSemanticUniform.empty()) {
                typeForSemanticUniform[MODELVIEWINVERSETRANSPOSE] = _GL(FLOAT_MAT3);; //typically the normal matrix
                typeForSemanticUniform[MODELVIEWINVERSE] = _GL(FLOAT_MAT4);
                typeForSemanticUniform[MODELVIEW] = _GL(FLOAT_MAT4);
                typeForSemanticUniform[PROJECTION] = _GL(FLOAT_MAT4);
                typeForSemanticUniform[JOINTMATRIX] = _GL(FLOAT_MAT4);
            }
            return typeForSemanticUniform[semantic];
        }
        
        //FIXME: it's a bad API since we won't have at the same time a varying an uniform...
        bool addSemantic(std::string vertexOrFragment, std::string uniformOrAttribute,
                         std::string semantic,
                         std::string parameterID,
                         size_t count,
                         bool includesVarying,
                         bool forcesAsAnArray = false) {
            
            std::string symbol = (uniformOrAttribute == "attribute") ? "a_" + parameterID : "u_" + parameterID;
            
            unsigned int type = (uniformOrAttribute == "uniform") ?
            typeForSemanticUniform(semantic) :
            typeForSemanticAttribute(semantic);
            
            shared_ptr <JSONObject> parameter(new GLTF::JSONObject());
            parameter->setString(kSemantic, semantic);
            parameter->setUnsignedInt32(kType,  type);
            _parameters->setValue(parameterID, parameter);
            
            //FIXME: should not assume default pass / default program
            GLSLProgram* program = _pass->instanceProgram();
            GLSLShader* shader = (vertexOrFragment == "vs") ? program->vertexShader() : program->fragmentShader();
            if (uniformOrAttribute == "attribute") {
                program->attributes()->setString(symbol, parameterID);
            } else if (uniformOrAttribute == "uniform") {
                program->uniforms()->setString(symbol, parameterID);
            } else {
                return false;
            }
            
            if (uniformOrAttribute == "attribute") {
                shader->addAttribute(symbol, type);
                
                if (includesVarying) {
                    program->addVarying("v_" + parameterID, type);
                }
            } else {
                shader->addUniform(symbol, type, count, forcesAsAnArray);
                if ((count > 1) || forcesAsAnArray) {
                    parameter->setUnsignedInt32(kCount, count);
                }
            }
            
            return true;
        }
        
        //FIXME: refactor with addSemantic
        shared_ptr <JSONObject> addParameter(std::string parameterID, unsigned int type) {
            shared_ptr <JSONObject> parameter(new GLTF::JSONObject());
            parameter->setUnsignedInt32(kType,  type);
            _parameters->setValue(parameterID, parameter);
            
            return parameter;
        }
        
        shared_ptr <JSONObject> addValue(std::string vertexOrFragment,
                                         std::string uniformOrAttribute,
                                         unsigned int type,
                                         size_t count,
                                         std::string parameterID,
                                         GLTFAsset* asset) {
            
            std::string symbol = (uniformOrAttribute == "attribute") ? "a_" + parameterID : "u_" + parameterID;
            
            //FIXME: should not assume default pass / default program
            GLSLProgram* program = _pass->instanceProgram();
            GLSLShader* shader = (vertexOrFragment == "vs") ? program->vertexShader() : program->fragmentShader();
            if (uniformOrAttribute == "attribute") {
                program->attributes()->setString(symbol, parameterID);
            } else if (uniformOrAttribute == "uniform") {
                program->uniforms()->setString(symbol, parameterID);
            } else {
                asset->log("cannot add semantic of unknown kind %s\n", uniformOrAttribute.c_str());
            }
            
            if (uniformOrAttribute == "attribute") {
                shader->addAttribute(symbol, type);
            } else {
                shader->addUniform(symbol, type, count);
            }
            
            return this->addParameter(parameterID, type);
        }
        
        static GLTFLightingModel _lightingModelFromString(const std::string &lightingModel) {
            if (lightingModel == "Phong") {
                return GLTFLightingModel::phong;
            } else if (lightingModel == "Blinn") {
                return GLTFLightingModel::blinn;
            } else if (lightingModel == "Constant") {
                return GLTFLightingModel::constant;
            } else {
                return GLTFLightingModel::lambert;
            }
        }
        
        Technique(const std::string &lm,
                  shared_ptr<JSONObject> attributeSemantics,
                  std::string techniqueID,
                  shared_ptr<JSONObject> values,
                  shared_ptr<JSONObject> techniqueExtras,
                  shared_ptr<JSONObject> texcoordBindings,
                  GLTFAsset *asset) {
            
            GLTFLightingModel lightingModel = _lightingModelFromString(lm);
            
            this->_profile = asset->profile();
            
            unsigned int vec3Type = _GL(FLOAT_VEC3);
            unsigned int vec4Type = _GL(FLOAT_VEC4);
            unsigned int mat4Type = _GL(FLOAT_MAT4);
            unsigned int sampler2DType = _GL(SAMPLER_2D);
            unsigned int floatType = _GL(FLOAT);
            
            this->_pass = new Pass(asset);
            this->_parameters = shared_ptr<GLTF::JSONObject>(new GLTF::JSONObject());
            
            shared_ptr <JSONObject> inputParameters = values;
            
            bool hasSpecular =  inputParameters->contains("specular") &&
            inputParameters->contains("shininess") &&
            ((lightingModel == phong) || (lightingModel == blinn));
            
            unsigned int jointsCount = 0;
            bool hasSkinning = false;
            bool hasNormalMap = false;
            
            if (techniqueExtras != nullptr) {
                jointsCount = techniqueExtras->getUnsignedInt32("jointsCount");
                hasSkinning =   attributeSemantics->contains("WEIGHT") &&
                attributeSemantics->contains("JOINT") &&
                (jointsCount > 0);
            }
            
            hasNormalMap = false;
            
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
            
            bool hasNormals = attributeSemantics->contains("NORMAL");
            if (hasNormals) {
                //normal attribute
                addSemantic("vs", "attribute",
                            "NORMAL", "normal", 1, true);
            }
            
            if (hasSkinning) {
                addSemantic("vs", "attribute",
                            "JOINT", "joint", 1, false);
                addSemantic("vs", "attribute",
                            "WEIGHT", "weight", 1, false);
                
                assert(techniqueExtras != nullptr);
                addSemantic("vs", "uniform",
                            JOINTMATRIX, "jointMat", jointsCount, false, true /* force as an array */);
            }
            
            if (hasNormals) {
                //normal matrix
                addSemantic("vs", "uniform",
                            MODELVIEWINVERSETRANSPOSE, "normalMatrix" , 1, false);
            }
            
            //modeliew matrix
            addSemantic("vs", "uniform",
                        MODELVIEW, "modelViewMatrix" , 1, false);
            
            //projection matrix
            addSemantic("vs", "uniform",
                        PROJECTION, "projectionMatrix" , 1, false);
            
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
                                         "u_modelViewMatrix",
                                         "a_position");
                if (hasNormals) {
                    vertexShader->appendCode("%s = %s * mat3(skinMat)* %s;\n",
                                             "v_normal", "u_normalMatrix", "a_normal");
                }
                
            } else {
                vertexShader->appendCode("%s pos = %s * vec4(%s,1.0);\n",
                                         vertexShader->GLSLTypeForGLType(vec4Type).c_str(),
                                         "u_modelViewMatrix",
                                         "a_position");
                if (hasNormals) {
                    vertexShader->appendCode("%s = %s * %s;\n",
                                             "v_normal", "u_normalMatrix", "a_normal");
                }
                
            }
            
            bool modelContainsLights = false;
            if (asset->root()->contains("lightsIds")) {
                shared_ptr<JSONArray> lightsIds = asset->root()->createArrayIfNeeded("lightsIds");
                std::vector <shared_ptr <JSONValue> > ids = lightsIds->values();
                if (ids.size() > 0) {
                    for (size_t i = 0 ; i < ids.size() ; i++) {
                        shared_ptr<JSONString> lightUID = static_pointer_cast<JSONString>(ids[i]);
                        shared_ptr<JSONArray> lightsNodesIds = static_pointer_cast<JSONArray>(asset->_uniqueIDOfLightToNodes[lightUID->getString()]);
                        
                        shared_ptr<JSONObject> lights = asset->root()->createObjectIfNeeded("lights");
                        shared_ptr<JSONObject> light = lights->getObject(lightUID->getString());
                        
                        if (light != nullptr) {
                            std::string lightType = light->getString(kType);
                            
                            //we ignore lighting if the only light we have is ambient
                            modelContainsLights |= lightType != "ambient";
                        }
                    }
                }
            }
            
            bool lightingIsEnabled = hasNormals && (modelContainsLights || CONFIG_BOOL(asset, "useDefaultLight"));
            if (lightingIsEnabled) {
                fragmentShader->appendCode("vec3 normal = normalize(%s);\n", "v_normal");
                if (techniqueExtras) {
                    if (techniqueExtras->getBool(kDoubleSided)) {
                        fragmentShader->appendCode("if (gl_FrontFacing == false) normal = -normal;\n");
                    }
                }
            } else {
                //https://github.com/KhronosGroup/glTF/issues/121
                //we want to keep consistent the parameter in the instanceTechnique and the ones actually in use in the technique.
                //Given the context, some parameters from instanceTechnique will be removed because they aren't used in the resulting shader. As an example, we may have no light declared and the default lighting disabled == no lighting at all, but still a specular color and a shininess. in this case specular and shininess won't be used
                
            }
            
            bool hasAmbientLight = false, hasSpecularLight = false;
            
            //color to cumulate all components and light contribution
            fragmentShader->appendCode("vec4 color = vec4(0., 0., 0., 0.);\n");
            fragmentShader->appendCode("vec4 diffuse = vec4(0., 0., 0., 1.);\n");
            if (modelContainsLights) {
                fragmentShader->appendCode("vec3 diffuseLight = vec3(0., 0., 0.);\n");
            }
            
            if (slotIsContributingToLighting("emission", inputParameters, asset)) {
                fragmentShader->appendCode("vec4 emission;\n");
            }
            
            if (slotIsContributingToLighting("reflective", inputParameters, asset)) {
                fragmentShader->appendCode("vec4 reflective;\n");
            }
            if (lightingIsEnabled && slotIsContributingToLighting("ambient", inputParameters, asset)) {
                fragmentShader->appendCode("vec4 ambient;\n");
            }
            if (lightingIsEnabled && slotIsContributingToLighting("specular", inputParameters, asset)) {
                fragmentShader->appendCode("vec4 specular;\n");
            }
            
            //texcoords
            std::string texcoordAttributeSymbol = "a_texcoord";
            std::string texcoordVaryingSymbol = "v_texcoord";
            std::map<std::string , std::string> declaredTexcoordAttributes;
            std::map<std::string , std::string> declaredTexcoordVaryings;
            
            const int slotsCount = 6;
            std::string slots[slotsCount] = { "ambient", "diffuse", "emission", "reflective", "specular", "bump" };
            for (size_t slotIndex = 0 ; slotIndex < slotsCount ; slotIndex++) {
                std::string slot = slots[slotIndex];
                
                if (inputParameters->contains(slot) == false)
                    continue;
                
                if (!slotIsContributingToLighting(slot, inputParameters, asset))
                    continue;
                
                shared_ptr <JSONObject> param = inputParameters->getObject(slot);
                unsigned int slotType = param->getUnsignedInt32(kType);
                
                if ((!lightingIsEnabled) && ((slot == "ambient") || (slot == "specular"))) {
                    //as explained in https://github.com/KhronosGroup/glTF/issues/121 export all parameters when details is specified
                    if (CONFIG_BOOL(asset, "exportPassDetails"))
                        addParameter(slot, slotType);
                    continue;
                }
                
                if (lightingIsEnabled && (slot == "bump")) {
                    hasNormalMap = slotType == sampler2DType && attributeSemantics->contains("TEXTANGENT") && attributeSemantics->contains("TEXBINORMAL") ;
                    if (hasNormalMap == true) {
                        addSemantic("vs", "attribute",
                                    "TEXTANGENT", "textangent", 1, true);
                        addSemantic("vs", "attribute",
                                    "TEXBINORMAL", "texbinormal", 1, true);
                        
                        vertexShader->appendCode("v_texbinormal = u_normalMatrix * a_texbinormal;\n");
                        vertexShader->appendCode("v_textangent = u_normalMatrix * a_textangent;\n");
                        fragmentShader->appendCode("vec4 bump;\n");
                    }
                }
                
                if (slotType == vec4Type) {
                    std::string slotColorSymbol = "u_"+slot;
                    fragmentShader->appendCode("%s = %s;\n", slot.c_str(), slotColorSymbol.c_str());
                    
                    addValue("fs", "uniform",   slotType, 1, slot, asset);
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
                    addValue("fs", "uniform", textureParameter->getUnsignedInt32(kType), 1, slot, asset);
                    
                    if ((hasNormalMap == false) && (slot == "bump"))
                        continue;
                    fragmentShader->appendCode("%s = texture2D(%s, %s);\n", slot.c_str(), textureSymbol.c_str(), texVSymbol.c_str());
                }
            }
            
            if (hasNormalMap) {
                //fragmentShader->appendCode("vec3 binormal = normalize( cross(normal,v_textangent));\n");
                fragmentShader->appendCode("mat3 bumpMatrix = mat3(normalize(v_textangent), normalize(v_texbinormal), normal);\n");
                fragmentShader->appendCode("normal = normalize(-1.0 + bump.xyz * 2.0);\n");
            }
            
            /*
             Handle lighting
             */
            
            shared_ptr <JSONObject> shininessObject;
            
            size_t lightIndex = 0;
            if (lightingIsEnabled && asset->root()->contains("lightsIds")) {
                shared_ptr<JSONArray> lightsIds = asset->root()->createArrayIfNeeded("lightsIds");
                std::vector <shared_ptr <JSONValue> > ids = lightsIds->values();
                
                if (inputParameters->contains("shininess") && (!shininessObject)) {
                    shininessObject = inputParameters->getObject("shininess");
                    addValue("fs", "uniform", shininessObject->getUnsignedInt32(kType) , 1, "shininess", asset);
                }
                
                for (size_t i = 0 ; i < ids.size() ; i++) {
                    shared_ptr<JSONString> lightUID = static_pointer_cast<JSONString>(ids[i]);
                    shared_ptr<JSONArray> lightsNodesIds = static_pointer_cast<JSONArray>(asset->_uniqueIDOfLightToNodes[lightUID->getString()]);
                    
                    shared_ptr<JSONObject> lights = asset->root()->createObjectIfNeeded("lights");
                    shared_ptr<JSONObject> light = lights->getObject(lightUID->getString());
                    if (light == nullptr)
                        continue;
                    std::string lightType = light->getString(kType);
                    
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
                        char lightInverseTransform[100];
                        char lightConstantAttenuation[100];
                        char lightLinearAttenuation[100];
                        char lightQuadraticAttenuation[100];
                        char lightFallOffAngle[100];
                        char lightFallOffExponent[100];
                        sprintf(lightIndexCStr, "light%d", (int)lightIndex);
                        sprintf(lightColor, "%sColor", lightIndexCStr);
                        sprintf(lightTransform, "%sTransform", lightIndexCStr);
                        sprintf(lightInverseTransform, "%sInverseTransform", lightIndexCStr);
                        sprintf(lightConstantAttenuation, "%sConstantAttenuation", lightIndexCStr);
                        sprintf(lightLinearAttenuation, "%sLinearAttenuation", lightIndexCStr);
                        sprintf(lightQuadraticAttenuation, "%sQuadraticAttenuation", lightIndexCStr);
                        sprintf(lightFallOffAngle, "%sFallOffAngle", lightIndexCStr);
                        sprintf(lightFallOffExponent, "%sFallOffExponent", lightIndexCStr);
                        
                        if (hasSpecular && (hasSpecularLight == false)) {
                            fragmentShader->appendCode("vec3 specularLight = vec3(0., 0., 0.);\n");
                            hasSpecularLight = true;
                        }
                        
                        if (lightType == "ambient") {
                            if (hasAmbientLight == false) {
                                fragmentShader->appendCode("vec3 ambientLight = vec3(0., 0., 0.);\n");
                                hasAmbientLight = true;
                            }
                            
                            fragmentShader->appendCode("{\n");
                            
                            shared_ptr <JSONObject> lightColorParameter = addValue("fs", "uniform", vec3Type, 1, lightColor, asset);
                            lightColorParameter->setValue("value", description->getValue("color"));
                            
                            //FIXME: what happens if multiple ambient light ?
                            fragmentShader->appendCode("ambientLight += u_%s;\n", lightColor);
                            fragmentShader->appendCode("}\n");
                            
                        } else {
                            char varyingLightDirection[100];
                            sprintf(varyingLightDirection, "v_%sDirection", lightIndexCStr);
                            program->addVarying(varyingLightDirection, vec3Type);
                            if ((vertexShader->hasSymbol("v_position") == false) && ((lightingModel == phong) || (lightType == "spot"))) {
                                program->addVarying("v_position", vec3Type);
                                vertexShader->appendCode("v_position = pos.xyz;\n");
                            }
                            fragmentShader->appendCode("{\n");
                            
                            fragmentShader->appendCode("float specularIntensity = 0.;\n");
                            
                            if (lightType != "directional") {
                                shared_ptr <JSONObject> lightConstantAttenuationParameter = addValue("fs", "uniform", floatType, 1, lightConstantAttenuation, asset);
                                lightConstantAttenuationParameter->setValue("value", description->getValue("constantAttenuation"));
                                shared_ptr <JSONObject> lightLinearAttenuationParameter = addValue("fs", "uniform", floatType, 1, lightLinearAttenuation, asset);
                                lightLinearAttenuationParameter->setValue("value", description->getValue("linearAttenuation"));
                                shared_ptr <JSONObject> lightQuadraticAttenuationParameter = addValue("fs", "uniform", floatType, 1, lightQuadraticAttenuation, asset);
                                lightQuadraticAttenuationParameter->setValue("value", description->getValue("quadraticAttenuation"));
                            }
                            shared_ptr <JSONObject> lightColorParameter = addValue("fs", "uniform", vec3Type, 1, lightColor, asset);
                            lightColorParameter->setValue("value", description->getValue("color"));
                            shared_ptr <JSONObject> lightTransformParameter = addValue("vs", "uniform", mat4Type, 1, lightTransform, asset);
                            lightTransformParameter->setValue(kSource, nodesIds[j]);
                            lightTransformParameter->setString(kSemantic, MODELVIEW);
                            
                            if (lightType == "directional") {
                                vertexShader->appendCode("%s = mat3(u_%s) * vec3(0.,0.,1.);\n", varyingLightDirection, lightTransform);
                            } else {
                                vertexShader->appendCode("%s = u_%s[3].xyz - pos.xyz;\n", varyingLightDirection, lightTransform) ;
                            }
                            
                            fragmentShader->appendCode("float attenuation = 1.0;\n");
                            
                            if (lightType != "directional") {
                                //compute light attenuation from non-normalized light direction
                                fragmentShader->appendCode("float range = length(%s);\n", varyingLightDirection);
                                fragmentShader->appendCode("attenuation = 1.0 / ( u_%s + (u_%s * range) + (u_%s * range * range) ) ;\n",
                                                           lightConstantAttenuation, lightLinearAttenuation, lightQuadraticAttenuation);
                            }
                            //compute lighting from normalized light direction
                            fragmentShader->appendCode("vec3 l = normalize(%s);\n", varyingLightDirection);
                            
                            if (lightType == "spot") {
                                shared_ptr <JSONObject> lightInverseTransformParameter = addValue("fs", "uniform", mat4Type, 1, lightInverseTransform, asset);
                                lightInverseTransformParameter->setValue(kSource, nodesIds[j]);
                                lightInverseTransformParameter->setString(kSemantic, MODELVIEWINVERSE);
                                
                                shared_ptr <JSONObject> lightFallOffExponentParameter = addValue("fs", "uniform", floatType, 1, lightFallOffExponent, asset);
                                lightFallOffExponentParameter->setValue("value", description->getValue("fallOffExponent"));
                                shared_ptr <JSONObject> lightFallOffAngleParameter = addValue("fs", "uniform", floatType, 1, lightFallOffAngle, asset);
                                lightFallOffAngleParameter->setValue("value", description->getValue("fallOffAngle"));
                                
                                //As in OpenGL ES 2.0 programming guide
                                //Raise spec issue about the angle
                                //we can test this in the shader generation
                                fragmentShader->appendCode("vec4 spotPosition = u_%s * vec4(v_position, 1.);\n", lightInverseTransform);
                                fragmentShader->appendCode("float cosAngle = dot(vec3(0.0,0.0,-1.0), normalize(spotPosition.xyz));\n");
                                //doing this cos each pixel is just wrong (for performance)
                                //need to find a way to specify that we pass the cos of a value
                                fragmentShader->appendCode("if (cosAngle > cos(radians(u_%s * 0.5)))\n", lightFallOffAngle);
                                fragmentShader->appendCode("{\n");
                                fragmentShader->appendCode("attenuation *= max(0.0, pow(cosAngle, u_%s));\n", lightFallOffExponent);
                            }
                            
                            //we handle phong, blinn, constant and lambert
                            if (hasSpecular) {
                                if (lightingModel == phong) {
                                    fragmentShader->appendCode("vec3 position = v_position;\n");
                                    if (hasNormalMap) {
                                        fragmentShader->appendCode("l *= bumpMatrix;\n");
                                        fragmentShader->appendCode("position *= bumpMatrix;\n");
                                    }
                                    fragmentShader->appendCode("float phongTerm = max(0.0, dot(reflect(-l,normal), normalize(-position)));\n");
                                    fragmentShader->appendCode("specularIntensity = max(0., pow(phongTerm , u_shininess)) * attenuation;\n");
                                } else if (lightingModel == blinn) {
                                    fragmentShader->appendCode("vec3 h = normalize(l+vec3(0.,0.,1.));\n");
                                    if (hasNormalMap) {
                                        fragmentShader->appendCode("h *= bumpMatrix;\n");
                                        fragmentShader->appendCode("l *= bumpMatrix;\n");
                                    }
                                    fragmentShader->appendCode("specularIntensity = max(0., pow(max(dot(normal,h), 0.) , u_shininess)) * attenuation;\n");
                                }
                                fragmentShader->appendCode("specularLight += u_%s * specularIntensity;\n", lightColor);
                            }
                            
                            //write diffuse
                            fragmentShader->appendCode("diffuseLight += u_%s * max(dot(normal,l), 0.) * attenuation;\n", lightColor);
                            if (lightType == "spot") {
                                //close previous scope beginning with "if (cosAngle > " ...
                                fragmentShader->appendCode("}\n");
                            }
                            fragmentShader->appendCode("}\n");
                        }
                    }
                }
            }
            
            //Currently the declaration of the shininess is dependent of the presence of light.
            //if we want details, we want also all parameters.
            if (CONFIG_BOOL(asset, "exportPassDetails")) {
                if (inputParameters->contains("shininess") && (!shininessObject)) {
                    shininessObject = inputParameters->getObject("shininess");
                    addValue("fs", "uniform",   shininessObject->getUnsignedInt32(kType), 1, "shininess", asset);
                }
            }
            
            if (slotIsContributingToLighting("reflective", inputParameters, asset)) {
                fragmentShader->appendCode("diffuse.xyz += reflective.xyz;\n");
            }
            if (hasAmbientLight && lightingIsEnabled && slotIsContributingToLighting("ambient", inputParameters, asset)) {
                fragmentShader->appendCode("ambient.xyz *= ambientLight;\n");
                fragmentShader->appendCode("color.xyz += ambient.xyz;\n");
            }
            if (hasSpecularLight && lightingIsEnabled && slotIsContributingToLighting("specular", inputParameters, asset)) {
                fragmentShader->appendCode("specular.xyz *= specularLight;\n");
                fragmentShader->appendCode("color.xyz += specular.xyz;\n");
            }
            if (modelContainsLights) {
                fragmentShader->appendCode("diffuse.xyz *= diffuseLight;\n");
            } else if (CONFIG_BOOL(asset, "useDefaultLight") && hasNormals) {
                fragmentShader->appendCode("diffuse.xyz *= max(dot(normal,vec3(0.,0.,1.)), 0.);\n");
            }
            fragmentShader->appendCode("color.xyz += diffuse.xyz;\n");
            
            if (slotIsContributingToLighting("emission", inputParameters, asset)) {
                fragmentShader->appendCode("color.xyz += emission.xyz;\n");
            }
            
            bool hasTransparency = inputParameters->contains("transparency");
            if (hasTransparency) {
                std::string slot = "transparency";
                shared_ptr <JSONObject> transparencyParam = inputParameters->getObject(slot);
                
                addValue("fs", "uniform",   transparencyParam->getUnsignedInt32(kType), 1, slot, asset);
                
                fragmentShader->appendCode("color = vec4(color.rgb * diffuse.a, diffuse.a * %s);\n", "u_transparency");
            } else {
                fragmentShader->appendCode("color = vec4(color.rgb * diffuse.a, diffuse.a);\n");
            }
            
            if (CONFIG_BOOL(asset, "alwaysExportFilterColor")) {
                if (inputParameters->contains("filterColor")) {
                    shared_ptr<JSONObject> filterColor = inputParameters->getObject("filterColor");
                    shared_ptr <JSONObject> filterColorParameter = addValue("fs", "uniform", vec4Type, 1, "filterColor", asset);
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

        ~Technique() {
            delete _pass;
        }
        
        shared_ptr <GLTF::JSONObject> parameters() {
            return _parameters;
        }
        
    private:
        Pass *_pass;
        shared_ptr <GLTF::JSONObject> _parameters;
        shared_ptr <GLTFProfile> _profile;
        
    };

    static TechniqueHashToTechniqueID techniqueHashToTechniqueID;
    void clearCommonProfileTechniqueCache()
    {
        techniqueHashToTechniqueID.clear();
    }
    
    std::string getTechniqueKey(shared_ptr<JSONObject> techniqueGenerator, GLTFAsset* asset)  {
        shared_ptr<JSONObject> values = techniqueGenerator->getObject("values");
        shared_ptr<JSONObject> techniqueExtras = techniqueGenerator->getObject("techniqueExtras");
        
        return buildTechniqueHash(values, techniqueExtras, asset);
    }

    
    std::string getReferenceTechniqueID(shared_ptr<JSONObject> techniqueGenerator, GLTFAsset* asset) {
        
        std::string techniqueHash = getTechniqueKey(techniqueGenerator, asset);
        shared_ptr<JSONObject> values = techniqueGenerator->getObject("values");
        shared_ptr<JSONObject> techniqueExtras = techniqueGenerator->getObject("techniqueExtras");
        std::string lightingModel = techniqueGenerator->getString("lightingModel");
        shared_ptr<JSONObject> attributeSemantics = techniqueGenerator->getObject("attributeSemantics");
        shared_ptr<JSONObject> texcoordBindings = techniqueGenerator->getObject("texcoordBindings");
        
        shared_ptr <JSONObject> techniquesObject = asset->root()->createObjectIfNeeded("techniques");
        
        if (techniqueHashToTechniqueID.count(techniqueHash) == 0) {
            techniqueHashToTechniqueID[techniqueHash] = "technique" + GLTFUtils::toString(techniqueHashToTechniqueID.size());
        }
        
        std::string techniqueID = techniqueHashToTechniqueID[techniqueHash];
        if (techniquesObject->contains(techniqueID))
            return techniqueID;
        
        GLTF::Technique glTFTechnique(lightingModel, attributeSemantics, techniqueID, values, techniqueExtras, texcoordBindings, asset);
        GLTF::Pass *glTFPass = glTFTechnique.getPass();
        
        std::string passName("defaultPass");
        //if the technique has not been serialized, first thing create the default pass for this technique
        shared_ptr <GLTF::JSONObject> pass(new GLTF::JSONObject());
        
        shared_ptr <GLTF::JSONObject> states = createStatesForTechnique(values, techniqueExtras, asset);
        pass->setValue("states", states);
        
        GLSLProgram* glTFProgram = glTFPass->instanceProgram();
        GLSLShader* vs = glTFProgram->vertexShader();
        GLSLShader* fs = glTFProgram->fragmentShader();
        
        if (asset->converterConfig()->config()->contains("glslVersion")) {
        }
        
        //create shader name made of the input file name to avoid file name conflicts
        COLLADABU::URI outputFileURI(asset->getOutputFilePath().c_str());
        shared_ptr <JSONObject> shaders = asset->root()->createObjectIfNeeded("shaders");
        
        std::string shaderBaseId = outputFileURI.getPathFileBase() + GLTFUtils::toString(shaders->getKeysCount() / 2);
        std::string shaderFS = shaderBaseId + "FS";
        std::string shaderVS = shaderBaseId + "VS";
        
        writeShaderIfNeeded(shaderVS, vs->source(), asset, asset->profile()->getGLenumForString("VERTEX_SHADER"));
        writeShaderIfNeeded(shaderFS, fs->source(), asset, asset->profile()->getGLenumForString("FRAGMENT_SHADER"));
        
        shared_ptr <JSONObject> programsObject = asset->root()->createObjectIfNeeded("programs");
        std::string programID = "program_" + GLTFUtils::toString(programsObject->getKeysCount());
        shared_ptr <GLTF::JSONObject> program(new GLTF::JSONObject());
        shared_ptr <GLTF::JSONObject> instanceProgram(new GLTF::JSONObject());
        
        instanceProgram->setValue("uniforms", glTFProgram->uniforms());
        instanceProgram->setValue("attributes", glTFProgram->attributes());
        pass->setValue("instanceProgram", instanceProgram);
        instanceProgram->setString("program", programID);
        programsObject->setValue(programID, program);
        
        program->setValue("attributes", glTFProgram->attributes()->keys());
        program->setString("vertexShader", shaderVS);
        program->setString("fragmentShader", shaderFS);
        
        shared_ptr<JSONObject> referenceTechnique(new JSONObject());
        
        referenceTechnique->setValue("parameters", glTFTechnique.parameters());
        referenceTechnique->setString("pass", passName);
        
        shared_ptr <GLTF::JSONObject> passes = referenceTechnique->createObjectIfNeeded("passes");
        
        passes->setValue(passName, pass);
        techniquesObject->setValue(techniqueID, referenceTechnique);
        
        if (CONFIG_BOOL(asset, "exportPassDetails")) {
            shared_ptr <JSONObject> details = glTFPass->getDetails(lightingModel, values, techniqueExtras, texcoordBindings, asset);
            pass->setValue("details", details);
        }
        
        return techniqueID;
    }    
}

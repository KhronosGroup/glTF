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

#include <string>
#include <algorithm>

#include "COLLADASaxFWLLoader.h"

#include "COLLADABUStableHeaders.h"
#include "COLLADABUNativeString.h"
#include "COLLADABUStringUtils.h"

#include "COLLADAFWRoot.h"
#include "COLLADAFWGeometry.h"
#include "COLLADAFWCamera.h"
#include "COLLADAFWMesh.h"
#include "COLLADAFWMeshVertexData.h"
#include "COLLADAFWNode.h"
#include "COLLADAFWVisualScene.h"
#include "COLLADAFWInstanceGeometry.h"
#include "COLLADAFWInstanceCamera.h"
#include "COLLADAFWMaterial.h"
#include "COLLADAFWEffect.h"
#include "COLLADAFWImage.h"
#include "COLLADABUURI.h"
#include "Math/COLLADABUMathMatrix4.h"

#include "GLTF.h"

#include "../GLTFConverterContext.h"
#include "commonProfileShaders.h"
#ifndef WIN32
#include "png.h"
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
    
    static std::string GLSLDeclarationForAttribute(shared_ptr<JSONObject> attribute)
    {
        std::string attributeDec = "attribute ";
        attributeDec += GLSLTypeForGLType(attribute->getString("type"));
        attributeDec += " " + attribute->getString("symbol")+";\n";
        
        return attributeDec;
    }
    
    static std::string GLSLDeclarationForUniform(shared_ptr<JSONObject> uniform)
    {
        std::string uniformDec = "uniform ";
        uniformDec += GLSLTypeForGLType(uniform->getString("type"));
        uniformDec += " " + uniform->getString("symbol")+";\n";
        
        return uniformDec;
    }
    
    static std::string GLSLDeclarationForVarying(std::string symbol, std::string type)
    {
        std::string uniformDec = "varying ";
        uniformDec += GLSLTypeForGLType(type);
        uniformDec += " " + symbol+";\n";
        
        return uniformDec;
    }

    static std::string typeForSemanticAttribute(const std::string& semantic) {
        static std::map<std::string , std::string> typeForSemanticAttribute;
        
        if (typeForSemanticAttribute.empty()) {
            typeForSemanticAttribute["POSITION"] = "FLOAT_VEC3";
            typeForSemanticAttribute["NORMAL"] = "FLOAT_VEC3";
            
            //FIXME: be smarter and parse
            typeForSemanticAttribute["TEXCOORD_0"] = "FLOAT_VEC2";
            typeForSemanticAttribute["TEXCOORD_1"] = "FLOAT_VEC2";
            typeForSemanticAttribute["TEXCOORD_2"] = "FLOAT_VEC2";
            typeForSemanticAttribute["TEXCOORD_3"] = "FLOAT_VEC2";
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
        if (parameters->contains(slot)) {
            shared_ptr<JSONObject> parameter = parameters->getObject(slot);
            
            if (parameter->contains("type")) {
                hash += parameter->getString("type");
                return hash;
            }
        }
        return hash + "none";
    }
    
    static std::string buildTechniqueHash(shared_ptr<JSONObject> technique, GLTFConverterContext& context) {
        std::string techniqueHash = "";
        
        shared_ptr<JSONObject> parameters = technique->getObject("parameters");
        
        //FIXME:now assume we always have diffuse specified
        shared_ptr<JSONObject> parameter = parameters->getObject("diffuse");
        
        techniqueHash += buildSlotHash(parameters, "diffuse");
        techniqueHash += buildSlotHash(parameters, "ambient");
        techniqueHash += buildSlotHash(parameters, "emission");
        techniqueHash += buildSlotHash(parameters, "specular");
        
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
                
                printf("[shader]: %s\n", shaderPath.c_str());
            }
        }
        
        return true;
    }
    
    static shared_ptr <JSONObject> createStatesForTechnique(shared_ptr<JSONObject> technique, GLTFConverterContext& context)
    {
        shared_ptr <JSONObject> states(new GLTF::JSONObject());
        
        shared_ptr <GLTF::JSONObject> parameters = technique->createObjectIfNeeded("parameters");

        if (isOpaque(parameters, context)) {
            states->setBool("depthTestEnable", true);
            states->setBool("depthMask", true);
            states->setBool("blendEnable", false);            
        } else {
            states->setBool("blendEnable", true);
            states->setBool("depthTestEnable", true);
            states->setBool("depthMask", false);
            states->setString("blendEquation", "FUNC_ADD");
            shared_ptr <JSONObject> blendFunc(new GLTF::JSONObject());
            blendFunc->setString("sfactor", "SRC_ALPHA");
            blendFunc->setString("dfactor", "ONE_MINUS_SRC_ALPHA");
            states->setValue("blendFunc", blendFunc) ;
        }
        
        return states;
    }

    shared_ptr <JSONObject> createAttribute(std::string semantic, std::string symbol) {
        shared_ptr <JSONObject> attribute(new GLTF::JSONObject());
                
        attribute->setString("semantic", semantic);
        attribute->setString("symbol", symbol);
        attribute->setString("type", typeForSemanticAttribute(semantic));
        
        return attribute;
    }
    
    //need this for parameters
    void appendUniform(std::string semantic, std::string symbol, shared_ptr <JSONArray> uniforms, std::string &declaration) {
        shared_ptr <JSONObject> uniform(new GLTF::JSONObject());
        
        uniform->setString("semantic", semantic);
        uniform->setString("symbol", symbol);
        uniform->setString("type", typeForSemanticUniform(semantic));
        
        uniforms->appendValue(static_cast<shared_ptr<JSONValue> >(uniform));
        
        declaration += GLSLDeclarationForUniform(uniform);
    }

    void appendUniformParameter(std::string slot, shared_ptr <JSONObject> inputParameter , std::string symbol, shared_ptr <JSONArray> uniforms, std::string &declaration) {
        shared_ptr <JSONObject> uniformParameter(new GLTF::JSONObject());
        
        uniformParameter->setString("parameter", slot);
        uniformParameter->setString("symbol", symbol);
        uniformParameter->setString("type", inputParameter->getString("type"));
        
        uniforms->appendValue(static_cast<shared_ptr<JSONValue> >(uniformParameter));
        
        declaration += GLSLDeclarationForUniform(uniformParameter);
    }
    
    typedef std::map<std::string , std::string > TechniqueHashToTechniqueID;
    
    std::string getReferenceTechniqueID(shared_ptr<JSONObject> technique, std::vector<shared_ptr<JSONObject> > &texcoordBindings, GLTFConverterContext& context) {
        
        shared_ptr <JSONObject> inputParameters = technique->getObject("parameters");
        
        shared_ptr <JSONObject> techniquesObject = context.root->createObjectIfNeeded("techniques");
        std::string techniqueHash = buildTechniqueHash(technique, context);

        static TechniqueHashToTechniqueID techniqueHashToTechniqueID;
        if (techniqueHashToTechniqueID.count(techniqueHash) == 0) {
            techniqueHashToTechniqueID[techniqueHash] = "technique" + GLTFUtils::toString(techniqueHashToTechniqueID.size());
        }
        
        std::string techniqueID = techniqueHashToTechniqueID[techniqueHash];
        
        if (techniquesObject->contains(techniqueID))
            return techniqueID;
        
        shared_ptr<JSONObject> referenceTechnique(new JSONObject());
        std::vector <std::string> allAttributes;
        std::vector <std::string> allUniforms;
        std::string shaderName = techniqueID; //simplification
        std::string vs =  shaderName + "Vs";
        std::string fs =  shaderName + "Fs";
        
        std::string vsDeclarations = "precision highp float;\n";
        std::string fsDeclarations = "precision highp float;\n";
        
        std::string vsBody = "void main(void) {\n";
        std::string fsBody = "void main(void) {\n";
        
        //we will build the attribute list and the shader at the same time
        shared_ptr <GLTF::JSONArray> attributes(new GLTF::JSONArray());
        shared_ptr <GLTF::JSONArray> uniforms(new GLTF::JSONArray());

        std::string positionAttributeSymbol = "a_position";
        //NORMAL
        std::string normalAttributeSymbol = "a_normal";
        std::string normalMatrixSymbol = "u_normalMatrix";
        std::string normalVaryingSymbol = "v_normal";
        shared_ptr <JSONObject> normalAttributeObject = createAttribute("NORMAL", normalAttributeSymbol);
        
        appendUniform("WORLDVIEWINVERSETRANSPOSE", normalMatrixSymbol, uniforms, vsDeclarations);

        attributes->appendValue(static_cast<shared_ptr<JSONValue> >(normalAttributeObject));
        
        /*
            attribute vec3 normal;\n
            varying vec3 v_normal;\n
            uniform mat3 u_normalMatrix;\n
         */

        vsDeclarations += GLSLDeclarationForAttribute(normalAttributeObject);
        vsDeclarations += GLSLDeclarationForVarying(normalVaryingSymbol, normalAttributeObject->getString("type"));
        fsDeclarations += GLSLDeclarationForVarying(normalVaryingSymbol, normalAttributeObject->getString("type"));
    
        char stringBuffer[1000];
        
        //VS
        sprintf(stringBuffer, "%s = normalize(%s * %s);\n", normalVaryingSymbol.c_str(),
                normalMatrixSymbol.c_str(),
                normalAttributeSymbol.c_str());
        vsBody += stringBuffer;
        
        //FS -> FIXME do not hard code type
        sprintf(stringBuffer, "vec3 normal = normalize(%s);\n", normalVaryingSymbol.c_str()); fsBody += stringBuffer;
        sprintf(stringBuffer, "float lambert = max(dot(normal,vec3(0.,0.,1.)), 0.);\n"); fsBody += stringBuffer;
        
        //naive implementation for now, accumulate lighting model component in color
        sprintf(stringBuffer, "vec4 color = vec4(0., 0., 0., 0.);\n"); fsBody += stringBuffer;
        
        //texcoords
        std::string texcoordAttributeSymbol = "a_texcoord";
        std::string texcoordVaryingSymbol = "v_texcoord";
        for (size_t i = 0 ; i < texcoordBindings.size() ; i++) {
            shared_ptr<JSONObject> texcoordBinding = texcoordBindings[i];
            //slot : diffuse, ambient...
            //semantic: TEXCOORD_0 ...
            std::string slot = texcoordBinding->getString("slot");
            std::string semantic = texcoordBinding->getString("semantic");
            std::string texSymbol = texcoordAttributeSymbol + GLTFUtils::toString(i);
            std::string texVSymbol = texcoordVaryingSymbol + GLTFUtils::toString(i);
            
            shared_ptr <JSONObject> texcoordAttributeObject = createAttribute(semantic, texSymbol);
            attributes->appendValue(static_cast<shared_ptr<JSONValue> >(texcoordAttributeObject));
            
            //VS
            sprintf(stringBuffer, "%s = %s;\n", texVSymbol.c_str(), texSymbol.c_str()); vsBody += stringBuffer;
            
            vsDeclarations += GLSLDeclarationForAttribute(texcoordAttributeObject);
            vsDeclarations += GLSLDeclarationForVarying(texVSymbol, texcoordAttributeObject->getString("type"));
            
            fsDeclarations += GLSLDeclarationForVarying(texVSymbol, texcoordAttributeObject->getString("type"));
            
            std::string textureSymbol = "u_"+ slot + "Texture";
                        
            //get the texture
            shared_ptr <JSONObject> textureParameter = inputParameters->getObject(slot);
            //FIXME:this should eventually not come from the inputParameter
            appendUniformParameter(slot, textureParameter, textureSymbol, uniforms, fsDeclarations);
            
            //FS
            //FIXME: much more slots to consider, and be more generic...
            if (slot == "diffuse") {
                sprintf(stringBuffer, "color = color + texture2D(%s, %s) * vec4(lambert,lambert,lambert,1.);\n", textureSymbol.c_str(), texVSymbol.c_str()); fsBody += stringBuffer;
            } else if (slot == "emission") {
                sprintf(stringBuffer, "color = color + texture2D(%s, %s);\n", textureSymbol.c_str(), texVSymbol.c_str()); fsBody += stringBuffer;
            }
        }

        if (inputParameters->contains("diffuse")) {
            std::string slot = "diffuse";
            shared_ptr <JSONObject> diffuseParam = inputParameters->getObject(slot);
            if (diffuseParam->getString("type") != "SAMPLER_2D" ) {
                std::string diffuseSymbol = "u_" + slot;
                sprintf(stringBuffer, "color = color + vec4(%s,1.) * vec4(lambert,lambert,lambert,1.);\n", diffuseSymbol.c_str()); fsBody += stringBuffer;
                appendUniformParameter(slot, diffuseParam, diffuseSymbol, uniforms, fsDeclarations);
            }
        }
        
        bool hasTransparency = inputParameters->contains("transparency");
        if (hasTransparency) {
            std::string slot = "transparency";

            shared_ptr <JSONObject> transparencyParam = inputParameters->getObject(slot);
            std::string transparencySymbol = "u_" + slot;
            
            appendUniformParameter(slot, transparencyParam, transparencySymbol, uniforms, fsDeclarations);
            
            sprintf(stringBuffer, "gl_FragColor = vec4(color.rgb * color.a, color.a * %s);\n", transparencySymbol.c_str()); fsBody += stringBuffer;
        } else {
            sprintf(stringBuffer, "gl_FragColor = vec4(color.rgb * color.a, color.a);\n"); fsBody += stringBuffer;
        }
        
        //attribute vec3 vert;\n
        shared_ptr <JSONObject> positionAttributeObject = createAttribute("POSITION", positionAttributeSymbol);
        vsDeclarations += GLSLDeclarationForAttribute(positionAttributeObject);
        attributes->appendValue(static_cast<shared_ptr<JSONValue> >(positionAttributeObject));

        std::string worldviewMatrixSymbol = "u_worldviewMatrix";
        std::string projectionMatrixSymbol = "u_projectionMatrix";
        appendUniform("WORLDVIEW", worldviewMatrixSymbol, uniforms, vsDeclarations);
        appendUniform("PROJECTION", projectionMatrixSymbol, uniforms, vsDeclarations);
        
        sprintf(stringBuffer, "gl_Position = %s * %s * vec4(%s,1.0);\n",
                projectionMatrixSymbol.c_str(),
                worldviewMatrixSymbol.c_str(),
                positionAttributeSymbol.c_str());
                vsBody += stringBuffer;

        vsBody += "}\n";
        fsBody += "}\n";

        std::string passName("defaultPass");
        //if the technique has not been serialized, first thing create the default pass for this technique
        shared_ptr <GLTF::JSONObject> pass(new GLTF::JSONObject());
        
        shared_ptr <GLTF::JSONObject> states = createStatesForTechnique(technique, context);
        pass->setValue("states", states);
        
        context.shaderIdToShaderString[vs] = vsDeclarations + vsBody;
        context.shaderIdToShaderString[fs] = fsDeclarations + fsBody;
        
        writeShaderIfNeeded(vs, context);
        writeShaderIfNeeded(fs, context);
        
        shared_ptr <GLTF::JSONObject> program(new GLTF::JSONObject());
        
        pass->setValue("program", program);
        
        program->setString("VERTEX_SHADER", vs);
        program->setString("FRAGMENT_SHADER", fs);
        
        referenceTechnique->setString("pass", passName);
        
        shared_ptr <GLTF::JSONObject> parameters = referenceTechnique->createObjectIfNeeded("parameters");
        program->setValue("uniforms", uniforms);
        program->setValue("attributes", attributes);
        
        shared_ptr <GLTF::JSONObject> passes = referenceTechnique->createObjectIfNeeded("passes");
        
        passes->setValue(passName, pass);
        techniquesObject->setValue(techniqueID, referenceTechnique);
        return techniqueID;
    }

}

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

#include "JSONExport.h"

#include "commonProfileShaders.h"

namespace JSONExport
{
    //lambert0 -> just a diffuse color
    const char* lambert0Vs = SHADER(
                                    precision highp float;\n
                                    attribute vec3 vert;\n
                                    attribute vec3 normal;\n
                                    varying vec3 v_normal;\n
                                    uniform mat4 u_mvMatrix;\n
                                    uniform mat3 u_normalMatrix;\n
                                    uniform mat4 u_projMatrix;\n
                                    \n
                                    void main(void) {\n
                                        v_normal = normalize(u_normalMatrix * normal);\n
                                        gl_Position = u_projMatrix * u_mvMatrix * vec4(vert,1.0);\n
                                    });
    
    const char* lambert0Fs = SHADER(
                                    precision highp float;\n
                                    uniform vec3 u_diffuseColor;\n
                                    varying vec3 v_normal;\n
                                    \n
                                    void main(void) { \n
                                        vec3 normal = normalize(v_normal);\n
                                        float lambert = max(dot(normal,vec3(0.,0.,1.)), 0.);\n
                                        gl_FragColor = vec4(u_diffuseColor.xyz *lambert, 1.); \n
                                    });
    
    const char* lambert1Vs = SHADER(
                                    precision highp float;\n
                                    attribute vec3 vert;\n
                                    attribute vec3 normal;\n
                                    attribute vec2 texcoord;\n
                                    varying vec3 v_normal;\n
                                    varying vec2 v_texcoord;\n
                                    uniform mat4 u_mvMatrix;\n
                                    uniform mat3 u_normalMatrix;\n
                                    uniform mat4 u_projMatrix;\n
                                    \n
                                    void main(void) {\n
                                        v_normal = normalize(u_normalMatrix * normal);\n
                                        v_texcoord = vec2(texcoord.x, texcoord.y);\n
                                        gl_Position = u_projMatrix * u_mvMatrix * vec4(vert,1.0);\n
                                    });
    
    const char* lambert1Fs = SHADER(
                                    precision highp float;\n
                                    varying vec3 v_normal;\n
                                    varying vec2 v_texcoord;\n
                                    uniform sampler2D u_diffuseTexture;\n
                                    \n
                                    void main(void) { \n
                                        vec3 normal = normalize(v_normal);\n
                                        float lambert = max(dot(normal,vec3(0.,0.,1.)), 0.);\n
                                        vec4 color = texture2D(u_diffuseTexture, v_texcoord);\n
                                        gl_FragColor = vec4(color.rgb * lambert, 1.); \n
                                    });
    
    
    const char* lambert2Vs = SHADER(
                                    precision highp float;\n
                                    attribute vec3 vert;\n
                                    attribute vec3 normal;\n
                                    varying vec3 v_normal;\n
                                    uniform mat4 u_mvMatrix;\n
                                    uniform mat3 u_normalMatrix;\n
                                    uniform mat4 u_projMatrix;\n
                                    \n
                                    void main(void) {\n
                                        v_normal = normalize(u_normalMatrix * normal);\n
                                        gl_Position = u_projMatrix * u_mvMatrix * vec4(vert,1.0);\n
                                    });
    
    const char* lambert2Fs = SHADER(
                                    precision highp float;\n
                                    uniform vec3 u_diffuseColor;\n
                                    varying vec3 v_normal;\n
                                    uniform float u_transparency;\n
                                    \n
                                    void main(void) { \n
                                        vec3 normal = normalize(v_normal);\n
                                        float lambert = max(dot(normal,vec3(0.,0.,1.)), 0.);\n
                                        gl_FragColor = vec4(u_diffuseColor.xyz *lambert, u_transparency); \n
                                    });
    
    const char* lambert3Vs = SHADER(
                                    precision highp float;\n
                                    attribute vec3 vert;\n
                                    attribute vec3 normal;\n
                                    attribute vec2 texcoord;\n
                                    varying vec3 v_normal;\n
                                    varying vec2 v_texcoord;\n
                                    uniform mat4 u_mvMatrix;\n
                                    uniform mat3 u_normalMatrix;\n
                                    uniform mat4 u_projMatrix;\n
                                    \n
                                    void main(void) {\n
                                        v_normal = normalize(u_normalMatrix * normal);\n
                                        v_texcoord = vec2(texcoord.x, texcoord.y);\n
                                        gl_Position = u_projMatrix * u_mvMatrix * vec4(vert,1.0);\n
                                    });
    
    const char* lambert3Fs = SHADER(
                                    precision highp float;\n
                                    varying vec3 v_normal;\n
                                    varying vec2 v_texcoord;\n
                                    uniform sampler2D u_diffuseTexture;\n
                                    uniform float u_transparency;\n
                                    \n
                                    void main(void) { \n
                                        vec3 normal = normalize(v_normal);\n
                                        float lambert = max(dot(normal,vec3(0.,0.,1.)), 0.);\n
                                        vec4 color = texture2D(u_diffuseTexture, v_texcoord);\n
                                        gl_FragColor = vec4(color.rgb * lambert, color.a * u_transparency); \n
                                    });
    
    static float getTransparency(const COLLADAFW::EffectCommon* effectCommon, const JSONExport::COLLADA2JSONContext& context) {
        //super naive for now, also need to check sketchup work-around
        if (effectCommon->getOpacity().isTexture()) {
            return 1;
        }
        float transparency = static_cast<float>(effectCommon->getOpacity().getColor().getAlpha());
        
        return context.invertTransparency ? 1 - transparency : transparency;
    }
    
    static float isOpaque(const COLLADAFW::EffectCommon* effectCommon, const JSONExport::COLLADA2JSONContext& context) {
        return getTransparency(effectCommon, context)  >= 1;
    }
    
    static std::string kVertexAttribute = "vert";
    static std::string kNormalAttribute = "normal";
    static std::string kTexcoordAttribute = "texcoord";
    static std::string kModelViewMatrixUniform = "u_mvMatrix";
    static std::string kNormalMatrixUniform = "u_normalMatrix";
    static std::string kProjectionMatrixUniform = "u_projMatrix";
    static std::string kDiffuseColorUniform = "u_diffuseColor";
    static std::string kDiffuseTextureUniform = "u_diffuseTexture";
    static std::string kTransparencyUniform = "u_transparency";
    
    
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
     GL_FLOAT,
     GL_FLOAT_VEC2
     GL_FLOAT_VEC3
     GL_FLOAT_VEC4
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
    
    static std::string typeForUniform(const std::string& symbol) {
        static std::map<std::string , std::string> typeForUniform;
        if (typeForUniform.empty()) {
            typeForUniform[kModelViewMatrixUniform] = "FLOAT_MAT4";
            typeForUniform[kNormalMatrixUniform] = "FLOAT_MAT3";
            typeForUniform[kProjectionMatrixUniform] = "FLOAT_MAT4";
            typeForUniform[kDiffuseColorUniform] = "FLOAT_VEC4";
            typeForUniform[kDiffuseTextureUniform] = "SAMPLER_2D";
            typeForUniform[kTransparencyUniform] = "FLOAT";
        }
        return typeForUniform[symbol];
    }
    
    /* attribute types derived from
     GL_FLOAT
     GL_FLOAT_VEC2
     GL_FLOAT_VEC3
     GL_FLOAT_VEC4
     GL_FLOAT_MAT2
     GL_FLOAT_MAT3
     GL_FLOAT_MAT4
     GL_FLOAT_MAT2x3
     GL_FLOAT_MAT2x4
     GL_FLOAT_MAT3x2
     GL_FLOAT_MAT3x4
     GL_FLOAT_MAT4x2
     GL_FLOAT_MAT4x3
     */
    
    static std::string typeForAttribute(const std::string& symbol) {
        static std::map<std::string , std::string> typeForAttribute;
        if (typeForAttribute.empty()) {
            typeForAttribute[kVertexAttribute] = "FLOAT_VEC3";
            typeForAttribute[kNormalAttribute] = "FLOAT_VEC3";
            typeForAttribute[kTexcoordAttribute] = "FLOAT_VEC2";
        }
        return typeForAttribute[symbol];
    }
    
    static std::string parameterForUniform(const std::string& symbol) {
        static std::map<std::string , std::string> symbolToParameter;
        if (symbolToParameter.empty()) {
            symbolToParameter[kDiffuseColorUniform] = "diffuseColor";
            symbolToParameter[kDiffuseTextureUniform] = "diffuseTexture";
            symbolToParameter[kTransparencyUniform] = "transparency";
        }
        return symbolToParameter[symbol];
    }
    
    static std::string semanticForUniform(const std::string& symbol) {
        static std::map<std::string , std::string> symbolToSemantic;
        if (symbolToSemantic.empty()) {
            symbolToSemantic[kModelViewMatrixUniform] = WORLDVIEW;
            symbolToSemantic[kNormalMatrixUniform] = WORLDVIEWINVERSETRANSPOSE;
            symbolToSemantic[kProjectionMatrixUniform] = PROJECTION;
        }
        return symbolToSemantic[symbol];
    }
    
    static bool symbolIsAUniformParameter(const std::string& symbol)
    {
        return ((symbol != kModelViewMatrixUniform) &&
                (symbol != kNormalMatrixUniform) &&
                (symbol != kProjectionMatrixUniform));
    }
    
    //FIXME: this won't be hardcoded anymore when we will rely on a "real" shader generation.
    static void appendAttributeSymbolsForShaderID(const std::string& shaderID,
                                                  std::vector <std::string> &symbols)
    {
        if ((shaderID == "lambert0Vs") || (shaderID == "lambert2Vs"))  {
            symbols.push_back(kVertexAttribute);
            symbols.push_back(kNormalAttribute);
        } else if ((shaderID == "lambert1Vs") || (shaderID == "lambert3Vs")) {
            symbols.push_back(kVertexAttribute);
            symbols.push_back(kNormalAttribute);
            symbols.push_back(kTexcoordAttribute);
        } else {
            printf("WARNING: unexpected shaderID: %s\n", shaderID.c_str());
        }
    }
    
    static void appendUniformSymbolsForShaderID(const std::string& shaderID,
                                                std::vector <std::string> &symbols)
    {
        if ((shaderID == "lambert0Vs") ||
            (shaderID == "lambert2Vs") ||
            (shaderID == "lambert1Vs") ||
            (shaderID == "lambert3Vs"))  {
            symbols.push_back(kModelViewMatrixUniform);
            symbols.push_back(kNormalMatrixUniform);
            symbols.push_back(kProjectionMatrixUniform);
        } else if (shaderID == "lambert0Fs") {
            symbols.push_back(kDiffuseColorUniform);
        } else if (shaderID == "lambert1Fs") {
            symbols.push_back(kDiffuseTextureUniform);
        } else if (shaderID == "lambert2Fs") {
            symbols.push_back(kDiffuseColorUniform);
            symbols.push_back(kTransparencyUniform);
        } else if (shaderID == "lambert3Fs") {
            symbols.push_back(kDiffuseTextureUniform);
            symbols.push_back(kTransparencyUniform);
        } else {
            printf("WARNING: unexpected shaderID: %s\n", shaderID.c_str());
        }
    }
    
    std::string getTechniqueNameForProfile(const COLLADAFW::EffectCommon* effectCommon, JSONExport::COLLADA2JSONContext& context) {
        std::string techniqueName;
        if (effectCommon->getDiffuse().isTexture()) {
            if (!isOpaque(effectCommon, context)) {
                techniqueName = "lambert3";
            } else {
                techniqueName = "lambert1";
            }
        } else {
            if (!isOpaque(effectCommon, context)) {
                techniqueName = "lambert2";
            } else {
                techniqueName = "lambert0";
            }
        }
        
        return techniqueName;
    }
    
    bool writeShaderIfNeeded(const std::string& shaderId,  JSONExport::COLLADA2JSONContext& context)
    {
        shared_ptr <JSONExport::JSONObject> shadersObject = context.root->createObjectIfNeeded("shaders");
        
        shared_ptr <JSONExport::JSONObject> shaderObject  = static_pointer_cast <JSONExport::JSONObject> (shadersObject->getValue(shaderId));
        
        if (!shaderObject) {
            shaderObject = shared_ptr <JSONExport::JSONObject> (new JSONExport::JSONObject());
            
            std::string path = shaderId+".glsl";
            shadersObject->setValue(shaderId, shaderObject);
            shaderObject->setString("path", path);
            
            //also write the file on disk
            std::string shaderString = context.shaderIdToShaderString[shaderId];
            if (shaderString.size() > 0) {
                COLLADABU::URI outputURI(context.outputFilePath);
                std::string shaderPath =  outputURI.getPathDir() + path;
                JSONExport::JSONUtils::writeData(shaderPath, "w",(unsigned char*)shaderString.c_str(), shaderString.size());
                
                printf("[shader]: %s\n", shaderPath.c_str());
            }
        }
        
        return true;
    }
    
    shared_ptr <JSONExport::JSONObject> createTechniqueForProfile(const COLLADAFW::EffectCommon* effectCommon, JSONExport::COLLADA2JSONContext& context) {
        
        std::vector <std::string> allAttributes;
        std::vector <std::string> allUniforms;
        std::string techniqueName = getTechniqueNameForProfile(effectCommon, context);
        std::string shaderName = techniqueName; //simplification
        std::string vs =  shaderName + "Vs";
        std::string fs =  shaderName + "Fs";
        shared_ptr <JSONExport::JSONObject> techniqueObject(new JSONExport::JSONObject());
        
        if (shaderName == "lambert0") {
            context.shaderIdToShaderString[vs] = lambert0Vs;
            context.shaderIdToShaderString[fs] = lambert0Fs;
        }
        
        if (shaderName == "lambert1") {
            context.shaderIdToShaderString[vs] = lambert1Vs;
            context.shaderIdToShaderString[fs] = lambert1Fs;
        }
        
        if (shaderName == "lambert2") {
            context.shaderIdToShaderString[vs] = lambert2Vs;
            context.shaderIdToShaderString[fs] = lambert2Fs;
        }
        
        if (shaderName == "lambert3") {
            context.shaderIdToShaderString[vs] = lambert3Vs;
            context.shaderIdToShaderString[fs] = lambert3Fs;
        }
        
        appendAttributeSymbolsForShaderID(vs, allAttributes);
        appendUniformSymbolsForShaderID(vs, allUniforms);
        appendUniformSymbolsForShaderID(fs, allUniforms);
        
        std::string passName("defaultPass");
        //if the technique has not been serialized, first thing create the default pass for this technique
        shared_ptr <JSONExport::JSONObject> pass(new JSONExport::JSONObject());
        
        shared_ptr <JSONExport::JSONObject> states(new JSONExport::JSONObject());
        pass->setValue("states", states);
        states->setValue("BLEND", shared_ptr <JSONExport::JSONNumber> (new JSONExport::JSONNumber((bool)!isOpaque(effectCommon, context))));
        
        writeShaderIfNeeded(vs, context);
        writeShaderIfNeeded(fs, context);
        
        shared_ptr <JSONExport::JSONObject> program(new JSONExport::JSONObject());
        
        pass->setString("type", "program");
        pass->setValue("program", program);
        
        program->setString("x-shader/x-vertex", vs);
        program->setString("x-shader/x-fragment", fs);
        
        techniqueObject->setString("pass", passName);
        
        shared_ptr <JSONExport::JSONObject> parameters = techniqueObject->createObjectIfNeeded("parameters");
        
        shared_ptr <JSONExport::JSONArray> uniforms(new JSONExport::JSONArray());
        program->setValue("uniforms", uniforms);
        for (unsigned int i = 0 ; i < allUniforms.size() ; i++) {
            shared_ptr <JSONExport::JSONObject> uniform(new JSONExport::JSONObject());
            std::string symbol = allUniforms[i];
            
            uniform->setString("symbol", symbol);
            uniforms->appendValue(uniform);
            
            if (symbolIsAUniformParameter(symbol)) {
                std::string parameterName = parameterForUniform(symbol);
                uniform->setString("parameter",parameterName);                   //if we want to set default values...
                if (!parameters->contains("parameter")) {
                    //just set white for diffuseColor
                    if (parameterName == "diffuseColor")
                        parameters->setValue(parameterName, serializeVec3(1,1,1));
                }
            } else {
                uniform->setString("semantic", semanticForUniform(symbol));
            }
            
            uniform->setString("type", typeForUniform(symbol));
        }
        
        shared_ptr <JSONExport::JSONArray> attributes(new JSONExport::JSONArray());
        program->setValue("attributes", attributes);
        for (unsigned int i = 0 ; i < allAttributes.size() ; i++) {
            shared_ptr <JSONExport::JSONObject> attribute(new JSONExport::JSONObject());
            std::string semantic;
            
            std::string symbol = allAttributes[i];
            if (symbol == kVertexAttribute) {
                semantic = "VERTEX";
            } else if (symbol == kNormalAttribute) {
                semantic = "NORMAL";
            } else if (symbol == kTexcoordAttribute) {
                semantic = "TEXCOORD";
            } else {
                printf("WARNING:symbol not handled %s\n", symbol.c_str());
                continue;
            }
            
            attribute->setString("semantic", semantic);
            attribute->setString("symbol", symbol);
            attribute->setString("type", typeForAttribute(symbol));
            
            //TODO: handle multiple sets => that would be
            //vertexAttribute->setValue("set", set);
            
            attributes->appendValue(attribute);
        }
        
        /*
         shared_ptr <JSONExport::JSONObject> diffuseColorObject = shared_ptr <JSONExport::JSONObject> (new JSONExport::JSONObject());
         diffuseColorObject->setString("symbol", "u_diffuseColor");
         inputs->setValue("diffuseColor", diffuseColorObject);
         */
        
        shared_ptr <JSONExport::JSONObject> passes = techniqueObject->createObjectIfNeeded("passes");
        
        passes->setValue(passName, pass);
        return techniqueObject;
    }


}
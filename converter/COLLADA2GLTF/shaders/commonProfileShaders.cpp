
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

#include "../GLTFConverterContext.h"
#include "commonProfileShaders.h"
#ifndef WIN32
#include "png.h"
#endif
using namespace std;

namespace JSONExport
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
    
    //lambert0:
    //lighting model: lambert
    //light0: hardcoded
    //diffuse: color
    //transparency: no
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
    
    //lambert1:
    //lighting model: lambert
    //light0: hardcoded
    //diffuse: texture
    //transparency: no
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
                                        gl_FragColor = vec4(color.rgb * color.a * lambert, color.a); \n
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
                                        gl_FragColor = vec4(color.rgb * color.a *lambert, color.a * u_transparency); \n
                                    });
    
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
            typeForUniform[kDiffuseColorUniform] = "FLOAT_VEC3";
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
            symbolToParameter[kDiffuseColorUniform] = "diffuse";
            symbolToParameter[kDiffuseTextureUniform] = "diffuse";
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
        } else if ((shaderID == "lambert1BlendOnVs") || (shaderID == "lambert1Vs") || (shaderID == "lambert3Vs")) {
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
            (shaderID == "lambert1BlendOnVs") ||
            (shaderID == "lambert3Vs"))  {
            symbols.push_back(kModelViewMatrixUniform);
            symbols.push_back(kNormalMatrixUniform);
            symbols.push_back(kProjectionMatrixUniform);
        } else if (shaderID == "lambert0Fs") {
            symbols.push_back(kDiffuseColorUniform);
        } else if ((shaderID == "lambert1Fs") || (shaderID == "lambert1BlendOnFs")) {
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
    
    std::string inferTechniqueName(shared_ptr<JSONObject> technique, GLTFConverterContext& context) {
        std::string techniqueName;
        
        shared_ptr<JSONObject> parameters = technique->getObject("parameters");
        
        //FIXME:now assume we always have diffuse specified
        shared_ptr<JSONObject> parameter = parameters->getObject("diffuse");
        
        bool hasDiffuseTexture = parameter->getString("type") == "SAMPLER_2D";
        //bool hasDiffuseColor = parameter->getString("type") == "FLOAT_VEC3";
        
        if (hasDiffuseTexture) {
            if (hasTransparency(parameters, context)) {
                techniqueName = "lambert3";
            } else {
                if (isOpaque(parameters, context))
                    techniqueName = "lambert1";
                else
                    techniqueName = "lambert1BlendOn";
            }
        } else {
            if (hasTransparency(parameters, context)) {
                techniqueName = "lambert2";
            } else {
                techniqueName = "lambert0";
            }
        }
        
        return techniqueName;
    }
    
    bool writeShaderIfNeeded(const std::string& shaderId,  GLTFConverterContext& context)
    {
        shared_ptr <JSONObject> shadersObject = context.root->createObjectIfNeeded("shaders");
        
        shared_ptr <JSONObject> shaderObject  = shadersObject->getObject(shaderId);
        
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
    
    static shared_ptr <JSONObject> createStatesForTechnique(shared_ptr<JSONObject> technique, GLTFConverterContext& context)
    {
        shared_ptr <JSONObject> states(new JSONExport::JSONObject());
        
        shared_ptr <JSONExport::JSONObject> parameters = technique->createObjectIfNeeded("parameters");

        if (isOpaque(parameters, context)) {
            states->setBool("depthTestEnable", true);
            states->setBool("depthMask", true);
            states->setBool("blendEnable", false);            
        } else {
            states->setBool("blendEnable", true);
            states->setBool("depthTestEnable", true);
            states->setBool("depthMask", false);
            states->setString("blendEquation", "FUNC_ADD");
            shared_ptr <JSONObject> blendFunc(new JSONExport::JSONObject());
            blendFunc->setString("sfactor", "SRC_ALPHA");
            blendFunc->setString("dfactor", "ONE_MINUS_SRC_ALPHA");
            states->setValue("blendFunc", blendFunc) ;
        }
        
        return states;
    }

    shared_ptr<JSONObject> createReferenceTechniqueBasedOnTechnique(shared_ptr<JSONObject> technique, GLTFConverterContext& context) {
        
        shared_ptr<JSONObject> referenceTechnique(new JSONObject());
        std::vector <std::string> allAttributes;
        std::vector <std::string> allUniforms;
        std::string techniqueName = inferTechniqueName(technique, context);
        std::string shaderName = techniqueName; //simplification
        std::string vs =  shaderName + "Vs";
        std::string fs =  shaderName + "Fs";
        
        if (shaderName == "lambert0") {
            context.shaderIdToShaderString[vs] = lambert0Vs;
            context.shaderIdToShaderString[fs] = lambert0Fs;
        }
        
        if (shaderName == "lambert1") {
            context.shaderIdToShaderString[vs] = lambert1Vs;
            context.shaderIdToShaderString[fs] = lambert1Fs;
        }

        if (shaderName == "lambert1BlendOn") {
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
        
        shared_ptr <JSONExport::JSONObject> states = createStatesForTechnique(technique, context);
        pass->setValue("states", states);
        
        writeShaderIfNeeded(vs, context);
        writeShaderIfNeeded(fs, context);
        
        shared_ptr <JSONExport::JSONObject> program(new JSONExport::JSONObject());
        
        pass->setString("type", "program");
        pass->setValue("program", program);
        
        program->setString("VERTEX_SHADER", vs);
        program->setString("FRAGMENT_SHADER", fs);
        
        referenceTechnique->setString("pass", passName);
        
        shared_ptr <JSONExport::JSONObject> parameters = referenceTechnique->createObjectIfNeeded("parameters");
        
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
                semantic = "POSITION";
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
        
        shared_ptr <JSONExport::JSONObject> passes = referenceTechnique->createObjectIfNeeded("passes");
        
        passes->setValue(passName, pass);
        
        return referenceTechnique;
    }


}
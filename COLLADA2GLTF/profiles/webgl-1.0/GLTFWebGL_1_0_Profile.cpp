// Copyright (c) 2013, Fabrice Robinet
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
#include "GLTFWebGL_1_0_Profile.h"

namespace GL {
    //FR:typedef is used in the header below (i wanted the header to be kept exactly as extracted from the WebGL specification).
    typedef int GLenum;
    #include "webgl-idl.h"
}

const std::string webgl_1_0_defaults = "{\"ACTIVE_TEXTURE\":33984,\"BLEND\":false,\"BLEND_COLOR\":[0,0,0,0],\"COLOR_CLEAR_VALUE\":[0,0,0,0],\"COLOR_WRITEMASK\":[true,true,true,true],\"CULL_FACE\":false,\"CULL_FACE_MODE\":1029,\"DEPTH_CLEAR_VALUE\":1,\"DEPTH_FUNC\":513,\"DEPTH_RANGE\":[0,1],\"DEPTH_TEST\":false,\"DEPTH_WRITEMASK\":true,\"FRONT_FACE\":2305,\"LINE_WIDTH\":1,\"SAMPLE_BUFFERS\":1,\"STENCIL_CLEAR_VALUE\":0,\"STENCIL_FUNC\":519,\"STENCIL_PASS_DEPTH_FAIL\":7680,\"STENCIL_PASS_DEPTH_PASS\":7680,\"STENCIL_REF\":0,\"STENCIL_TEST\":false,\"POLYGON_OFFSET_FILL\":false,\"SCISSOR_TEST\":false, \"BLEND_EQUATION\":32774}";

namespace GLTF
{    
#define Enum_STR(Src) #Src
#define registerGLEnum(X) (setGLenumForString(Enum_STR(X), GL::X))
    
    GLTFWebGL_1_0_Profile::GLTFWebGL_1_0_Profile()
    {
        this->_defaultValues = std::shared_ptr<JSONObject>(new JSONObject());
        this->_defaultValues->initWithCString(webgl_1_0_defaults.c_str(), nullptr);
        
        registerGLEnum(DEPTH_BUFFER_BIT);
        registerGLEnum(STENCIL_BUFFER_BIT);
        registerGLEnum(COLOR_BUFFER_BIT);

        registerGLEnum(POINTS);
        registerGLEnum(LINES);
        registerGLEnum(LINE_LOOP);
        registerGLEnum(LINE_STRIP);
        registerGLEnum(TRIANGLES);
        registerGLEnum(TRIANGLE_STRIP);
        registerGLEnum(TRIANGLE_FAN);
        
        registerGLEnum(ZERO);
        registerGLEnum(ONE);
        registerGLEnum(SRC_COLOR);
        registerGLEnum(ONE_MINUS_SRC_COLOR);
        registerGLEnum(SRC_ALPHA);
        registerGLEnum(ONE_MINUS_SRC_ALPHA);
        registerGLEnum(DST_ALPHA);
        registerGLEnum(ONE_MINUS_DST_ALPHA);

        registerGLEnum(DST_COLOR);
        registerGLEnum(ONE_MINUS_DST_COLOR);
        registerGLEnum(SRC_ALPHA_SATURATE);

        registerGLEnum(FUNC_ADD);

        registerGLEnum(BLEND_EQUATION);
        registerGLEnum(BLEND_EQUATION_RGB);
        registerGLEnum(BLEND_EQUATION_ALPHA);
        
        registerGLEnum(FUNC_SUBTRACT);
        registerGLEnum(FUNC_REVERSE_SUBTRACT);
        
        registerGLEnum(BLEND_DST_RGB);
        registerGLEnum(BLEND_SRC_RGB);
        registerGLEnum(BLEND_DST_ALPHA);
        registerGLEnum(BLEND_SRC_ALPHA);
        
        registerGLEnum(CONSTANT_COLOR);
        registerGLEnum(ONE_MINUS_CONSTANT_COLOR);
        registerGLEnum(CONSTANT_ALPHA);
        registerGLEnum(ONE_MINUS_CONSTANT_ALPHA);
        registerGLEnum(BLEND_COLOR);
        
        registerGLEnum(ARRAY_BUFFER);
        registerGLEnum(ELEMENT_ARRAY_BUFFER);
        registerGLEnum(ARRAY_BUFFER_BINDING);
        registerGLEnum(ELEMENT_ARRAY_BUFFER_BINDING);
        
        registerGLEnum(STREAM_DRAW);
        registerGLEnum(STATIC_DRAW);
        registerGLEnum(DYNAMIC_DRAW);

        registerGLEnum(BUFFER_SIZE);
        registerGLEnum(BUFFER_USAGE);
        
        registerGLEnum(CURRENT_VERTEX_ATTRIB);

        registerGLEnum(FRONT);
        registerGLEnum(BACK);
        registerGLEnum(FRONT_AND_BACK);
        
        registerGLEnum(CULL_FACE);
        registerGLEnum(BLEND);
        registerGLEnum(DITHER);
        registerGLEnum(STENCIL_TEST);
        registerGLEnum(DEPTH_TEST);
        registerGLEnum(SCISSOR_TEST);
        registerGLEnum(POLYGON_OFFSET_FILL);
        registerGLEnum(SAMPLE_ALPHA_TO_COVERAGE);
        registerGLEnum(SAMPLE_COVERAGE);
        
        registerGLEnum(NO_ERROR);
        registerGLEnum(INVALID_ENUM);
        registerGLEnum(INVALID_VALUE);
        registerGLEnum(INVALID_OPERATION);
        registerGLEnum(OUT_OF_MEMORY);
        
        registerGLEnum(CW);
        registerGLEnum(CCW);
        
        registerGLEnum(LINE_WIDTH);
        registerGLEnum(ALIASED_POINT_SIZE_RANGE);
        registerGLEnum(ALIASED_LINE_WIDTH_RANGE);
        registerGLEnum(CULL_FACE_MODE);
        registerGLEnum(FRONT_FACE);
        registerGLEnum(DEPTH_RANGE);
        registerGLEnum(DEPTH_WRITEMASK);
        
        registerGLEnum(DEPTH_CLEAR_VALUE);
        registerGLEnum(DEPTH_FUNC);
        registerGLEnum(STENCIL_CLEAR_VALUE);
        registerGLEnum(STENCIL_FUNC);
        registerGLEnum(STENCIL_FAIL);
        registerGLEnum(STENCIL_PASS_DEPTH_FAIL);
        registerGLEnum(STENCIL_PASS_DEPTH_PASS);
        registerGLEnum(STENCIL_REF);
        registerGLEnum(STENCIL_VALUE_MASK);
        registerGLEnum(STENCIL_WRITEMASK);
        registerGLEnum(STENCIL_BACK_FUNC);
        registerGLEnum(STENCIL_BACK_FAIL);
        registerGLEnum(STENCIL_BACK_PASS_DEPTH_FAIL);
        registerGLEnum(STENCIL_BACK_PASS_DEPTH_PASS);
        registerGLEnum(STENCIL_BACK_REF);
        registerGLEnum(STENCIL_BACK_VALUE_MASK);
        registerGLEnum(STENCIL_BACK_WRITEMASK);
        registerGLEnum(VIEWPORT);
        registerGLEnum(SCISSOR_BOX);

        registerGLEnum(COLOR_CLEAR_VALUE);
        registerGLEnum(COLOR_WRITEMASK);
        registerGLEnum(UNPACK_ALIGNMENT);
        registerGLEnum(PACK_ALIGNMENT);
        registerGLEnum(MAX_TEXTURE_SIZE);
        registerGLEnum(MAX_VIEWPORT_DIMS);
        registerGLEnum(SUBPIXEL_BITS);
        registerGLEnum(RED_BITS);
        registerGLEnum(GREEN_BITS);
        registerGLEnum(BLUE_BITS);
        registerGLEnum(ALPHA_BITS);
        registerGLEnum(DEPTH_BITS);
        registerGLEnum(STENCIL_BITS);
        registerGLEnum(POLYGON_OFFSET_UNITS);
        
        registerGLEnum(POLYGON_OFFSET_FACTOR);
        registerGLEnum(TEXTURE_BINDING_2D);
        registerGLEnum(SAMPLE_BUFFERS);
        registerGLEnum(SAMPLES);
        registerGLEnum(SAMPLE_COVERAGE_VALUE);
        registerGLEnum(SAMPLE_COVERAGE_INVERT);

        registerGLEnum(COMPRESSED_TEXTURE_FORMATS);

        registerGLEnum(DONT_CARE);
        registerGLEnum(FASTEST);
        registerGLEnum(NICEST);
        
        registerGLEnum(GENERATE_MIPMAP_HINT);
        
        /* DataType */
        registerGLEnum(BYTE);
        registerGLEnum(UNSIGNED_BYTE);
        registerGLEnum(SHORT);
        registerGLEnum(UNSIGNED_SHORT);
        registerGLEnum(INT);
        registerGLEnum(UNSIGNED_INT);
        registerGLEnum(FLOAT);
        
        /* PixelFormat */
        registerGLEnum(DEPTH_COMPONENT);
        registerGLEnum(ALPHA);
        registerGLEnum(RGB);
        registerGLEnum(RGBA);
        registerGLEnum(LUMINANCE);
        registerGLEnum(LUMINANCE_ALPHA);
        
        
        /* PixelType */
        registerGLEnum(UNSIGNED_SHORT_4_4_4_4);
        registerGLEnum(UNSIGNED_SHORT_5_5_5_1);
        registerGLEnum(UNSIGNED_SHORT_5_6_5);
        
        registerGLEnum(FRAGMENT_SHADER);
        registerGLEnum(VERTEX_SHADER);
        registerGLEnum(MAX_VERTEX_ATTRIBS);
        registerGLEnum(MAX_VERTEX_UNIFORM_VECTORS);
        registerGLEnum(MAX_VARYING_VECTORS);
        registerGLEnum(MAX_COMBINED_TEXTURE_IMAGE_UNITS);
        registerGLEnum(MAX_VERTEX_TEXTURE_IMAGE_UNITS);
        registerGLEnum(MAX_TEXTURE_IMAGE_UNITS);
        registerGLEnum(MAX_FRAGMENT_UNIFORM_VECTORS);
        registerGLEnum(SHADER_TYPE);
        registerGLEnum(DELETE_STATUS);
        registerGLEnum(LINK_STATUS);
        registerGLEnum(VALIDATE_STATUS);
        registerGLEnum(ATTACHED_SHADERS);
        registerGLEnum(ACTIVE_UNIFORMS);
        registerGLEnum(ACTIVE_ATTRIBUTES);
        registerGLEnum(SHADING_LANGUAGE_VERSION);
        registerGLEnum(CURRENT_PROGRAM);
        
        /* StencilFunction */
        registerGLEnum(NEVER);
        registerGLEnum(LESS);
        registerGLEnum(EQUAL);
        registerGLEnum(LEQUAL);
        registerGLEnum(GREATER);
        registerGLEnum(NOTEQUAL);
        registerGLEnum(GEQUAL);
        registerGLEnum(ALWAYS);
        
        /* StencilOp */
        /*      ZERO */
        registerGLEnum(KEEP);
        registerGLEnum(REPLACE);
        registerGLEnum(INCR);
        registerGLEnum(DECR);
        registerGLEnum(INVERT);
        registerGLEnum(INCR_WRAP);
        registerGLEnum(DECR_WRAP);        
        
        /* StringName */
        registerGLEnum(VENDOR);
        registerGLEnum(RENDERER);
        registerGLEnum(VERSION);

        /* TextureMagFilter */
        registerGLEnum(NEAREST);
        registerGLEnum(LINEAR);
        
        /* TextureMinFilter */
        /*      NEAREST */
        /*      LINEAR */
        registerGLEnum(NEAREST_MIPMAP_NEAREST);
        registerGLEnum(LINEAR_MIPMAP_NEAREST);
        registerGLEnum(NEAREST_MIPMAP_LINEAR);
        registerGLEnum(LINEAR_MIPMAP_LINEAR);
        
        /* TextureParameterName */
        registerGLEnum(TEXTURE_MAG_FILTER);
        registerGLEnum(TEXTURE_MIN_FILTER);
        registerGLEnum(TEXTURE_WRAP_S);
        registerGLEnum(TEXTURE_WRAP_T);
                
        /* TextureTarget */
        registerGLEnum(TEXTURE_2D);
        registerGLEnum(TEXTURE);

        registerGLEnum(TEXTURE_CUBE_MAP);
        registerGLEnum(TEXTURE_BINDING_CUBE_MAP);
        registerGLEnum(TEXTURE_CUBE_MAP_POSITIVE_X);
        registerGLEnum(TEXTURE_CUBE_MAP_NEGATIVE_X);
        registerGLEnum(TEXTURE_CUBE_MAP_POSITIVE_Y);
        registerGLEnum(TEXTURE_CUBE_MAP_NEGATIVE_Y);
        registerGLEnum(TEXTURE_CUBE_MAP_POSITIVE_Z);
        registerGLEnum(TEXTURE_CUBE_MAP_NEGATIVE_Z);
        registerGLEnum(MAX_CUBE_MAP_TEXTURE_SIZE);
        
        /* TextureUnit */
        registerGLEnum(TEXTURE0);
        registerGLEnum(TEXTURE1);
        registerGLEnum(TEXTURE2);
        registerGLEnum(TEXTURE3);
        registerGLEnum(TEXTURE4);
        registerGLEnum(TEXTURE5);
        registerGLEnum(TEXTURE6);
        registerGLEnum(TEXTURE7);
        registerGLEnum(TEXTURE8);
        registerGLEnum(TEXTURE9);
        registerGLEnum(TEXTURE10);
        registerGLEnum(TEXTURE11);
        registerGLEnum(TEXTURE12);
        registerGLEnum(TEXTURE13);
        registerGLEnum(TEXTURE14);
        registerGLEnum(TEXTURE15);
        registerGLEnum(TEXTURE16);
        registerGLEnum(TEXTURE17);
        registerGLEnum(TEXTURE18);
        registerGLEnum(TEXTURE19);
        registerGLEnum(TEXTURE20);
        registerGLEnum(TEXTURE21);
        registerGLEnum(TEXTURE22);
        registerGLEnum(TEXTURE23);
        registerGLEnum(TEXTURE24);
        registerGLEnum(TEXTURE25);
        registerGLEnum(TEXTURE26);
        registerGLEnum(TEXTURE27);
        registerGLEnum(TEXTURE28);
        registerGLEnum(TEXTURE29);
        registerGLEnum(TEXTURE30);
        registerGLEnum(TEXTURE31);
        
        registerGLEnum(ACTIVE_TEXTURE);

        /* TextureWrapMode */
        registerGLEnum(REPEAT);
        registerGLEnum(CLAMP_TO_EDGE);
        registerGLEnum(MIRRORED_REPEAT);
                
        /* Uniform Types */
        registerGLEnum(FLOAT_VEC2);
        registerGLEnum(FLOAT_VEC3);
        registerGLEnum(FLOAT_VEC4);
        registerGLEnum(INT_VEC2);
        registerGLEnum(INT_VEC3);
        registerGLEnum(INT_VEC4);
        registerGLEnum(BOOL);
        registerGLEnum(BOOL_VEC2);
        registerGLEnum(BOOL_VEC3);
        registerGLEnum(BOOL_VEC4);
        registerGLEnum(FLOAT_MAT2);
        registerGLEnum(FLOAT_MAT3);
        registerGLEnum(FLOAT_MAT4);
        registerGLEnum(SAMPLER_2D);
        registerGLEnum(SAMPLER_CUBE);
        
        
        /* Vertex Arrays */
        registerGLEnum(VERTEX_ATTRIB_ARRAY_ENABLED);
        registerGLEnum(VERTEX_ATTRIB_ARRAY_SIZE);
        registerGLEnum(VERTEX_ATTRIB_ARRAY_STRIDE);
        registerGLEnum(VERTEX_ATTRIB_ARRAY_TYPE);
        registerGLEnum(VERTEX_ATTRIB_ARRAY_NORMALIZED);
        registerGLEnum(VERTEX_ATTRIB_ARRAY_POINTER);
        registerGLEnum(VERTEX_ATTRIB_ARRAY_BUFFER_BINDING);
        
        /* Shader Source */
        registerGLEnum(COMPILE_STATUS);
        
        /* Shader Precision-Specified Types */
        registerGLEnum(LOW_FLOAT);
        registerGLEnum(MEDIUM_FLOAT);
        registerGLEnum(HIGH_FLOAT);
        registerGLEnum(LOW_INT);
        registerGLEnum(MEDIUM_INT);
        registerGLEnum(HIGH_INT);
        
        /* Framebuffer Object. */
        registerGLEnum(FRAMEBUFFER);
        registerGLEnum(RENDERBUFFER);
        
        registerGLEnum(RGBA4);
        registerGLEnum(RGB5_A1);
        registerGLEnum(RGB565);
        registerGLEnum(DEPTH_COMPONENT16);
        registerGLEnum(STENCIL_INDEX);
        registerGLEnum(STENCIL_INDEX8);
        registerGLEnum(DEPTH_STENCIL);
        
        registerGLEnum(RENDERBUFFER_WIDTH);
        registerGLEnum(RENDERBUFFER_HEIGHT);
        registerGLEnum(RENDERBUFFER_INTERNAL_FORMAT);
        registerGLEnum(RENDERBUFFER_RED_SIZE);
        registerGLEnum(RENDERBUFFER_GREEN_SIZE);
        registerGLEnum(RENDERBUFFER_BLUE_SIZE);
        registerGLEnum(RENDERBUFFER_ALPHA_SIZE);
        registerGLEnum(RENDERBUFFER_DEPTH_SIZE);
        registerGLEnum(RENDERBUFFER_STENCIL_SIZE);
        
        registerGLEnum(FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE);
        registerGLEnum(FRAMEBUFFER_ATTACHMENT_OBJECT_NAME);
        registerGLEnum(FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL);
        registerGLEnum(FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE);

        registerGLEnum(COLOR_ATTACHMENT0);
        registerGLEnum(DEPTH_ATTACHMENT);
        registerGLEnum(STENCIL_ATTACHMENT);
        registerGLEnum(DEPTH_STENCIL_ATTACHMENT);
        
        registerGLEnum(NONE);
        
        registerGLEnum(FRAMEBUFFER_COMPLETE);
        registerGLEnum(FRAMEBUFFER_INCOMPLETE_ATTACHMENT);
        registerGLEnum(FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT);
        registerGLEnum(FRAMEBUFFER_INCOMPLETE_DIMENSIONS);
        registerGLEnum(FRAMEBUFFER_UNSUPPORTED);
        
        registerGLEnum(FRAMEBUFFER_BINDING);
        registerGLEnum(RENDERBUFFER_BINDING);
        registerGLEnum(MAX_RENDERBUFFER_SIZE);
        
        registerGLEnum(INVALID_FRAMEBUFFER_OPERATION);
        
        /* WebGL-specific enums */
        registerGLEnum(UNPACK_FLIP_Y_WEBGL);
        registerGLEnum(UNPACK_PREMULTIPLY_ALPHA_WEBGL);
        registerGLEnum(CONTEXT_LOST_WEBGL);
        registerGLEnum(UNPACK_COLORSPACE_CONVERSION_WEBGL);
        registerGLEnum(BROWSER_DEFAULT_WEBGL);
    }
    
    std::shared_ptr<JSONValue> GLTFWebGL_1_0_Profile::defaultValueForState(const std::string& state) {
        return this->_defaultValues->getValue(state);
    }
    
    bool GLTFWebGL_1_0_Profile::isDefaultValueForState(const std::string& state, std::shared_ptr<JSONValue> value) {
        assert(value);
        std::shared_ptr<JSONValue> defaultValue = defaultValueForState(state);
        assert(defaultValue);
        return value->isEqualTo(defaultValue.get());
    }
    
    GLTFWebGL_1_0_Profile::~GLTFWebGL_1_0_Profile() {
    }
    
    unsigned int GLTFWebGL_1_0_Profile::getGLTypeForComponentTypeAndType(const std::string &componentType, const std::string &type)
    {
        if (componentType == "BYTE") {
            return GL::BYTE;
        } else if (componentType == "UNSIGNED_BYTE") {
            return GL::UNSIGNED_BYTE;
        } else if (componentType == "SHORT") {
            return GL::SHORT;
        } else if (componentType == "UNSIGNED_SHORT") {
            return GL::UNSIGNED_SHORT;
        } else if (componentType == "FLOAT") {
            size_t componentsPerElement = GLTFProfile::getComponentsCountForType(type);
            switch (componentsPerElement) {
                case 1:
                    return GL::FLOAT;
                case 2:
                    return GL::FLOAT_VEC2;
                case 3:
                    return GL::FLOAT_VEC3;
                case 4:
                    return GL::FLOAT_VEC4;
                case 16:
                    return GL::FLOAT_MAT4;
            }
        }
        return 0;
    }
    
    size_t GLTFWebGL_1_0_Profile::sizeOfGLType(unsigned int glType) {
        switch (glType) {
            case GL::FLOAT:
                return sizeof(float);
            case GL::INT:
                return sizeof(int);
            case GL::UNSIGNED_SHORT:
                return sizeof(unsigned short);
            case GL::BOOL:
                return sizeof(bool);                
            case GL::FLOAT_VEC2:
                return sizeof(float) * 2;
            case GL::INT_VEC2:
                return sizeof(int) * 2;
            case GL::BOOL_VEC2:
                return sizeof(bool) * 2;
            case GL::FLOAT_VEC3:
                return sizeof(float) * 3;
            case GL::INT_VEC3:
                return sizeof(int) * 3;
            case GL::BOOL_VEC3:
                return sizeof(bool) * 3;
            case GL::FLOAT_VEC4:
                return sizeof(float) * 4;
            case GL::INT_VEC4:
                return sizeof(int) * 4;
            case GL::BOOL_VEC4:
                return sizeof(bool) * 4;
            case GL::FLOAT_MAT2:
                return sizeof(float) * 4;
            case GL::FLOAT_MAT3:
                return sizeof(float) * 9;
            case GL::FLOAT_MAT4:
                return sizeof(float) * 16;
            default:
                return 0;
        }
    }
    
    std::string GLTFWebGL_1_0_Profile::getGLSLTypeForGLType(unsigned int glType) {
        static std::map<unsigned int , std::string> GLSLTypeForGLType;
        if (GLSLTypeForGLType.empty()) {
            GLSLTypeForGLType[GL::FLOAT] = "float";
            GLSLTypeForGLType[GL::FLOAT_VEC2] = "vec2";
            GLSLTypeForGLType[GL::FLOAT_VEC3] = "vec3";
            GLSLTypeForGLType[GL::FLOAT_VEC4] = "vec4";
            
            GLSLTypeForGLType[GL::FLOAT_MAT2] = "mat2";
            GLSLTypeForGLType[GL::FLOAT_MAT3] = "mat3";
            GLSLTypeForGLType[GL::FLOAT_MAT4] = "mat4";
            
            GLSLTypeForGLType[GL::INT] = "int";
            GLSLTypeForGLType[GL::INT_VEC2] = "ivec";
            GLSLTypeForGLType[GL::INT_VEC3] = "ivec3";
            GLSLTypeForGLType[GL::INT_VEC4] = "ivec4";
            
            GLSLTypeForGLType[GL::BOOL] = "bool";
            GLSLTypeForGLType[GL::BOOL_VEC2] = "bvec2";
            GLSLTypeForGLType[GL::BOOL_VEC3] = "bvec3";
            GLSLTypeForGLType[GL::BOOL_VEC4] = "bvec4";
            
            GLSLTypeForGLType[GL::SAMPLER_2D] = "sampler2D";
            GLSLTypeForGLType[GL::SAMPLER_CUBE] = "samplerCube";
        }
        return GLSLTypeForGLType[glType];
    }
    
    size_t GLTFWebGL_1_0_Profile::getComponentsCountForGLType(unsigned int glType) {
        switch (glType) {
            case GL::FLOAT:
            case GL::INT:
            case GL::BOOL:
            case GL::UNSIGNED_SHORT:
            case GL::UNSIGNED_BYTE:
                return 1;

            case GL::FLOAT_VEC2:
            case GL::INT_VEC2:
            case GL::BOOL_VEC2:
                return 2;
                
            case GL::FLOAT_VEC3:
            case GL::INT_VEC3:
            case GL::BOOL_VEC3:
                return 3;

            case GL::FLOAT_VEC4:
            case GL::INT_VEC4:
            case GL::BOOL_VEC4:
            case GL::FLOAT_MAT2:
                return 4;

            case GL::FLOAT_MAT3:
                return 9;
            case GL::FLOAT_MAT4:
                return 16;
            default:
                return 0;
        }
    }

    std::string GLTFWebGL_1_0_Profile::getTypeForGLType(unsigned int glType) {
        switch (glType) {
            case GL::FLOAT:
            case GL::INT:
            case GL::BOOL:
            case GL::UNSIGNED_SHORT:
            case GL::UNSIGNED_INT:
                return "SCALAR";
        
            case GL::FLOAT_VEC2:
            case GL::INT_VEC2:
            case GL::BOOL_VEC2:
                return "VEC2";
                
            case GL::FLOAT_VEC3:
            case GL::INT_VEC3:
            case GL::BOOL_VEC3:
                return "VEC3";

            case GL::FLOAT_VEC4:
            case GL::INT_VEC4:
            case GL::BOOL_VEC4:
                return "VEC4";

            case GL::FLOAT_MAT2:
                return "MAT2";
                
            case GL::FLOAT_MAT3:
                return "MAT3";
                
            case GL::FLOAT_MAT4:
                return "MAT4";
                
            default:
                return "";
        }
    }

    unsigned int GLTFWebGL_1_0_Profile::getGLComponentTypeForGLType(unsigned int glType) {
        switch (glType) {
            case GL::FLOAT:
            case GL::FLOAT_VEC2:
            case GL::FLOAT_VEC3:
            case GL::FLOAT_VEC4:
            case GL::FLOAT_MAT2:
            case GL::FLOAT_MAT3:
            case GL::FLOAT_MAT4:
                return GL::FLOAT;
            case GL::INT:
            case GL::INT_VEC2:
            case GL::INT_VEC3:
            case GL::INT_VEC4:
                return GL::INT;
            case GL::BOOL:
            case GL::BOOL_VEC2:
            case GL::BOOL_VEC3:
            case GL::BOOL_VEC4:
                return GL::BOOL;
            case GL::UNSIGNED_SHORT:
                return GL::UNSIGNED_SHORT;
            case GL::UNSIGNED_INT:
                return GL::UNSIGNED_INT;
            default:
                return 0;
        }
    }

    std::shared_ptr<GLTF::JSONObject> GLTFWebGL_1_0_Profile::id()
    {
        auto id = std::make_shared<JSONObject>();
        id->setString("api", "WebGL");
        id->setString("version", "1.0.2");

        return id;
    }
}

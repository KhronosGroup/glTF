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

#ifndef __JSON_UTILS_H__
#define __JSON_UTILS_H__

#include <float.h>

namespace GLTF 
{    
    class GLTFUtils 
    {
    public:        

        static bool writeData(FILE* fd, unsigned char* data, size_t length)
        {            
            if (fd) {
                fwrite (data , 1 ,length  , fd );
            }            
            return true;
        }
        
        static bool writeData(std::string filename, const char* mode, unsigned char* data, size_t length)
        {
            // FIXME: check encode / decode 
			FILE* fd = fopen(filename.c_str(), mode);        
            
            if (fd) {
                fwrite (data , 1 ,length  , fd );
                fclose(fd);
            }
            
            return true;
        }
        
        static std::string generateIDForType(const char* typeCStr, const char* suffix = 0)
        {   
            char separator = '_';
            
            std::string type(typeCStr);
            type +=  separator; // FIXME: should probably not generate a "-" for a JSON output
            type += GLTFUtils::toString(s_generatedIDCount++);
            if (suffix) {
                type +=  separator;
                type += suffix;
            }
            
            return type;
        }

        static void resetIDCount()
        {
            s_generatedIDCount = 1;
        }
                
        static std::string getStringForGLType(int componentType)
        {
            switch (componentType) {
                case GLTF::UNSIGNED_BYTE:
                    return "UNSIGNED_BYTE";
                case GLTF::SHORT:
                    return "SHORT";
                case GLTF::UNSIGNED_SHORT:
                    return "UNSIGNED_SHORT";
                case GLTF::UNSIGNED_INT:
                    return "UNSIGNED_INT";
                case GLTF::FIXED:
                    return "UNSIGNED_INT";
                case GLTF::FLOAT: 
                    return "FLOAT";
                default:
                    return "UNKNOWN";
            }
        }
        
        static std::string getStringForSemantic(GLTF::Semantic semantic) 
        {
            switch (semantic) {
                case GLTF::POSITION:
                    return "POSITION";
                case GLTF::NORMAL:
                    return "NORMAL";
                case GLTF::TEXCOORD:
                    return "TEXCOORD";
                case GLTF::COLOR:
                    return "COLOR";
                case GLTF::JOINT:
                    return "JOINT";
                case GLTF::WEIGHT:
                    return "WEIGHT";
                    
                default:
                    return "UNKNOWN";
            }
        }
        
        /** Converts @a value to a string.
         @param T The type of the value to convert.
         @param value The value of type @a T to convert to a string.
         */
        //This tiny code snippet was copied from the OpenCOLLADA fmk in order to keep the GLTF independant
        template<class T>
        static std::string toString(const T & value)
        {
            std::stringstream stream;
            stream << value;
            return stream.str();
        }

        static bool isPathSeparator(char c) {
#if WIN32
            return (c == '\\');
#else
            return (c == '/');
#endif
        }
        
        static bool isAbsolutePath(const std::string& path) {
            if (path.length() == 0)
                return false;
            const char* const name = path.c_str();
#if WIN32
            return (path.length() >= 3) && ((name[0] >= 'a' && name[0] <= 'z') || (name[0] >= 'A' && name[0] <= 'Z')) && name[1] == ':' && isPathSeparator(name[2]);
#else
            return isPathSeparator(name[0]);
#endif
        }


        private:
            static unsigned int s_generatedIDCount;
    };
    
}

#endif
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

namespace JSONExport 
{    
    class JSONUtils 
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
            static unsigned int generatedIDCount = 1;
            char separator = '_';
            
            std::string type(typeCStr);
            type +=  separator; // FIXME: should probably not generate a "-" for a JSON output
            type += JSONUtils::toString(generatedIDCount++);
            if (suffix) {
                type +=  separator;
                type += suffix;
            }
            
            return type;
        }
                
        static std::string getStringForGLType(int componentType)
        {
            switch (componentType) {
                case JSONExport::UNSIGNED_BYTE:
                    return "UNSIGNED_BYTE";
                case JSONExport::SHORT:
                    return "SHORT";
                case JSONExport::UNSIGNED_SHORT:
                    return "UNSIGNED_SHORT";
                case JSONExport::UNSIGNED_INT:
                    return "UNSIGNED_INT";
                case JSONExport::FIXED:
                    return "UNSIGNED_INT";
                case JSONExport::FLOAT: 
                    return "FLOAT";
                default:
                    return "UNKNOWN";
            }
        }
        
        static std::string getStringForSemantic(JSONExport::Semantic semantic) 
        {
            switch (semantic) {
                case JSONExport::POSITION:
                    return "POSITION";
                case JSONExport::NORMAL:
                    return "NORMAL";
                case JSONExport::TEXCOORD:
                    return "TEXCOORD";
                case JSONExport::COLOR:
                    return "COLOR";
                default:
                    return "UNKNOWN";
            }
        }
        
        /** Converts @a value to a string.
         @param T The type of the value to convert.
         @param value The value of type @a T to convert to a string.
         */
        //This tiny code snippet was copied from the OpenCOLLADA fmk in order to keep the JSONExport independant
        template<class T>
        static std::string toString(const T & value)
        {
            std::stringstream stream;
            stream << value;
            return stream.str();
        }
                        
    };
    
}

#endif
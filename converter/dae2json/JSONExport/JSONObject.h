// Copyright (c) 2011-2012, Motorola Mobility, Inc.
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

#ifndef __JSON_OBJECT_H__
#define __JSON_OBJECT_H__

namespace JSONExport 
{    
    typedef std::map<std::string , shared_ptr <JSONValue> > KeyToJSONValue;
    
    class JSONObject : public JSONValue {
    protected:
        JSONObject(JSONValueType type);
        
    public:        
        
        JSONObject();
        virtual ~JSONObject();
                
        void setValue(const std::string &key, shared_ptr <JSONValue> value);
        shared_ptr <JSONValue> getValue(std::string);
        
        void setUnsignedInt32(const std::string &key, unsigned int value);
        unsigned int getUnsignedInt32(const std::string &key);
        
        void setInt32(const std::string &key, int value);
        int getInt32(const std::string &key);
        
        void setDouble(const std::string &key, double value);
        double getDouble(const std::string &key);
        
        void setString(const std::string &key, const std::string &value);
        const std::string& getString(const std::string &key);
        
        std::vector <std::string> getAllKeys();
        
    private:
        KeyToJSONValue _keyToJSONValue;
    };

}


#endif
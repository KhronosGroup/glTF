// Copyright (c) 2013, Fabrice Robinet.
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

#ifndef __GLTFSKIN_H__
#define __GLTFSKIN_H__

namespace GLTF 
{
    class GLTFController {
    public:
        GLTFController();
        virtual ~GLTFController();
        virtual std::string getType() = 0;

        shared_ptr<JSONObject> extras();
    private:
        shared_ptr<JSONObject> _extras;
    };

    class GLTFSkin : public GLTFController
    {
    public:
        GLTFSkin();
        GLTFSkin(std::string id);
        virtual ~GLTFSkin();
        
        shared_ptr<JSONArray> getBindShapeMatrix();
        void setBindShapeMatrix(shared_ptr<JSONArray>  bindShapeMatrix);
        
        shared_ptr<JSONArray> getJointsIds();
        void setJointsIds(shared_ptr<JSONArray> jointIds);
        
        const std::string& getId();
        
        void setJoints(shared_ptr <GLTFMeshAttribute> joints);
        shared_ptr <GLTFMeshAttribute> getJoints();

        void setWeights(shared_ptr <GLTFMeshAttribute> weights);
        shared_ptr <GLTFMeshAttribute> getWeights();

        void setSourceUID(std::string uniqueId);
        std::string getSourceUID();
                
        //controller
        std::string getType();
        
    private:
        shared_ptr<JSONArray> _bindShapeMatrix;
        shared_ptr<JSONArray> _jointsIds;
        
        shared_ptr<GLTFMeshAttribute> _joints;
        shared_ptr<GLTFMeshAttribute> _weights;
                
        std::string _id;
        std::string _sourceUID;
    };
}


#endif
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

#ifndef __GLTF_ANIMATION_H__
#define __GLTF_ANIMATION_H__

#include "GLTF.h"
#include "../GLTFOpenCOLLADA.h"

using namespace std;

namespace GLTF 
{
    class GLTFAsset;
        
    class COLLADA2GLTF_EXPORT GLTFAnimation : public JSONObject {
    public:
        class AnimationType {
        public:
            const static string ROTATE;
            const static string TRANSLATE;
            const static string SCALE;
            const static string TIME;
        };

        class AnimationDataType {
        public:
            const static string VEC4;
            const static string VEC3;
            const static string SCALAR;
        };

        const static map<string, string> ANIMATION_DATA_TYPES;
        const static map<string, int> ANIMATION_DATA_TYPE_SIZE;
        const static vector<string> ANIMATION_TYPES;

        GLTFAnimation();
        GLTFAnimation(const COLLADAFW::Animation* animation, GLTFAsset* asset);
        virtual ~GLTFAnimation();

        int getCount() { return _keyFrames.size(); }
        
        void setID(string animationID);
        string getID();
        
        void setOriginalID(string originalID);
        string getOriginalID();
        
        shared_ptr<JSONObject> getParameterNamed(string parameter);
        void removeParameterNamed(string parameter);

        string getTargetNodeId() { return _nodeId; }
        void setTargetNodeId(string nodeId) { _nodeId = nodeId; }

        void addValuesAtKeyFrame(double frame, vector<double> values);
        void addInterpolatedKeyFrame(double frame);
        void doubleKeyFrames();
        bool hasKeyFrame(double frame) { return _outputValues.count(frame) != 0; }
        double getKeyFrameAtIndex(int index) { return _keyFrames[index]; }
        vector<double> getValuesAtKeyFrame(double frame) { return _outputValues[frame]; }
        
        shared_ptr <JSONObject> samplers();
        shared_ptr <JSONArray> channels();
        shared_ptr <JSONObject> targets();
        shared_ptr <JSONObject> parameters();

        string getSamplerIDForName(std::string name);
                
        void registerBufferView(string parameterName, shared_ptr<GLTFBufferView>);
        void unregisterBufferView(string parameterName);

        shared_ptr<GLTFBufferView> getBufferViewForParameter(string);
        
        void writeAnimation(GLTFAsset* asset);
        
        virtual string valueType();

    private:
        string _id;
        string _originalID;
        shared_ptr <JSONObject> _targets;
        string _nodeId;
        vector<double> _keyFrames;
        map<double, vector<double>> _outputValues;
        map<string, shared_ptr<GLTFBufferView>> _bufferViews;
    };
    
}

#endif
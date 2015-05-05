// Copyright (c) 2015, Analytical Graphics, Inc.
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
#include "GLTFAsset.h"
#include "GLTFCleanupEvaluator.h"
#include "JSONObject.h"
#include <memory>

namespace GLTF
{

    void GLTFCleanupEvaluator::evaluationWillStart(GLTFAsset*)
    {

    };

    void GLTFCleanupEvaluator::evaluate(JSONValue* value, GLTFAsset* asset)
    {

    }

    void GLTFCleanupEvaluator::evaluationDidComplete(GLTFAsset* asset)
    {
        // Remove some extra info we stored in images
        if (asset->root()->contains(kImages))
        {
            std::shared_ptr<JSONObject> images = asset->root()->getObject(kImages);
            std::vector<std::string> imageKeys = images->getAllKeys();
            for (size_t i = 0; i < imageKeys.size(); i++) {
                std::shared_ptr<JSONObject> image = images->getObject(imageKeys[i]);
                if (image->contains("has_alpha"))
                {
                    image->removeValue("has_alpha");
                }
            }
        }
    }
}


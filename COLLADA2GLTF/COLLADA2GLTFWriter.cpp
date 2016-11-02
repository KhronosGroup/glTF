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

// reminder; to run recursively against all dae files: find . -name '*.dae' -exec dae2json {} \;
#include "COLLADA2GLTFWriter.h"
#include "GLTFOpenCOLLADAUtils.h"
#include "GLTFExtraDataHandler.h"
#include "COLLADASaxFWLLoader.h"
#include "COLLADAFWFileInfo.h"
#include "GLTF-Open3DGC.h"
#include "profiles/webgl-1.0/GLTFWebGL_1_0_Profile.h"
#include "GitSHA1.h"
#include <algorithm>
#include "commonProfileShaders.h"
#include "helpers/encodingHelpers.h"
#include "COLLADAFWFileInfo.h"
#include "Math/COLLADABUMathPrerequisites.h"
#include "COLLADAFWAnimationList.h"

#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;
using namespace COLLADAFW;
using namespace COLLADABU;
using namespace COLLADASaxFWL;

namespace
{
    const double radianPerDegree = Math::PI / 180.0;
}

namespace GLTF
{
    /*
    */
    COLLADA2GLTFWriter::COLLADA2GLTFWriter(shared_ptr<GLTFAsset> asset) :
        _asset(asset),
        _visualScene(0){
    }

    /*
    */
    COLLADA2GLTFWriter::~COLLADA2GLTFWriter() {
    }

    /*
    */
    void COLLADA2GLTFWriter::reportError(const std::string& method, const std::string& message) {
        printf("ERROR: method:%s message:%s\n", method.c_str(), message.c_str());
    }

    /*
    */
    bool COLLADA2GLTFWriter::write() {
        this->_extraDataHandler = new ExtraDataHandler();
        //To comply with macro to access config
        GLTFAsset *asset = this->_asset.get();
        asset->setExtras(this->_extraDataHandler->allExtras());
        asset->prepareForProfile(shared_ptr <GLTFWebGL_1_0_Profile>(new GLTFWebGL_1_0_Profile()));

        COLLADAFW::Root root(&this->_loader, this);
        this->_loader.registerExtraDataCallbackHandler(this->_extraDataHandler);
        const std::string& fileData = asset->getInputFileData();
        if (fileData.empty())
        {
            // We don't have any data so read the file
            if (!root.loadDocument(asset->getInputFilePath())) {
                delete _extraDataHandler;
                return false;
            }
        }
        else
        {
            // We have the data in memory so use it
            if (!root.loadDocument(asset->getInputFilePath(), fileData.data(), (int)fileData.size())) {
                delete _extraDataHandler;
                return false;
            }
        }

        asset->write();

        // Cleanup IDs and Technique cache in case we have another conversion
        GLTFUtils::resetIDCount();
        clearCommonProfileTechniqueCache();

        delete _extraDataHandler;
        return true;
    }

    //--------------------------------------------------------------------
    void COLLADA2GLTFWriter::cancel(const std::string& errorMessage) {
        printf("CONVERTION ABORTED: message:%s\n", errorMessage.c_str());
    }

    //--------------------------------------------------------------------
    void COLLADA2GLTFWriter::start() {
    }

    //--------------------------------------------------------------------
    void COLLADA2GLTFWriter::finish() {
    }


    //--------------------------------------------------------------------
    bool COLLADA2GLTFWriter::writeGlobalAsset(const COLLADAFW::FileInfo* globalAsset) {
        GLTFAsset* asset = this->_asset.get();
        shared_ptr<JSONObject> assetObject = asset->root()->createObjectIfNeeded(kAsset);
        std::string version = "collada2gltf@" + std::string(g_GIT_SHA1);

        const COLLADAFW::FileInfo::ValuePairPointerArray& valuePairs = globalAsset->getValuePairArray();
        for (size_t i = 0, count = valuePairs.getCount(); i < count; ++i)
        {
            const COLLADAFW::FileInfo::ValuePair* valuePair = valuePairs[i];
            const COLLADAFW::String& key = valuePair->first;
            const COLLADAFW::String& value = valuePair->second;
            const char *sketchUp = "Google SketchUp";

            if ((key == "authoring_tool") && value.length() > 0) {
                if (value.find(sketchUp) != string::npos) {
                    //isolate x.y (version) that is the latest token from a string that is possibily x.y.z
                    size_t end = value.find_last_of(" ", value.length() - 1);
                    if (end != string::npos) {
                        std::string version = value.substr(end);
                        size_t major = version.find(".", 0);
                        if (major != string::npos) {
                            size_t minor = version.find(".", major + 1);
                            if (minor != string::npos) {
                                version = version.substr(0, minor);
                                double sketchUpVersion = atof(version.c_str());
                                if (sketchUpVersion < 7.1) {
                                    asset->converterConfig()->config()->setBool("invertTransparency", true);
                                    this->_asset->log("WARNING: Fixing transparency - by inverting it - as we convert an asset generated from SketchUp version:%s \n", version.c_str());
                                }
                            }
                        }

                    }

                }
            }
        }

        assetObject->setString("generator", version);
        assetObject->setBool(kPremultipliedAlpha, CONFIG_BOOL(asset, kPremultipliedAlpha));
        assetObject->setValue(kProfile, asset->profile()->id());
        assetObject->setString(kVersion, glTFVersion);

        _metersPerUnit = globalAsset->getUnit().getLinearUnitMeter();
        if (globalAsset->getUpAxisType() == COLLADAFW::FileInfo::X_UP ||
            globalAsset->getUpAxisType() == COLLADAFW::FileInfo::Z_UP)
        {
            COLLADABU::Math::Matrix4 matrix = COLLADABU::Math::Matrix4::IDENTITY;
            if (globalAsset->getUpAxisType() == COLLADAFW::FileInfo::X_UP)
            {
                // Rotate -90 deg around Z
                matrix.setElement(0, 0, 0.0f);
                matrix.setElement(0, 1, -1.0f);
                matrix.setElement(1, 0, 1.0f);
                matrix.setElement(1, 1, 0.0f);
            }
            else // Z_UP
            {
                // Rotate 90 deg around X
                matrix.setElement(1, 1, 0.0f);
                matrix.setElement(1, 2, 1.0f);
                matrix.setElement(2, 1, -1.0f);
                matrix.setElement(2, 2, 0.0f);
            }

            _rootTransform = std::shared_ptr<GLTF::JSONObject>(new GLTF::JSONObject());
            _rootTransform->setString(kName, "Y_UP_Transform");
            _rootTransform->setValue("matrix", serializeOpenCOLLADAMatrix4(matrix));

            shared_ptr <GLTF::JSONArray> childrenArray(new GLTF::JSONArray());
            _rootTransform->setValue(kChildren, childrenArray);
        }
        asset->setDistanceScale(globalAsset->getUnit().getLinearUnitMeter());

        return true;
    }

    //--------------------------------------------------------------------

    float COLLADA2GLTFWriter::getTransparency(const COLLADAFW::EffectCommon* effectCommon) {
        static bool loggedOnce = false;
        GLTFAsset* asset = this->_asset.get();
        //super naive for now, also need to check sketchup work-around
        if (effectCommon->getOpacity().isTexture()) {
            return 1;
        }

        COLLADAFW::EffectCommon::OpaqueMode opaqueMode;
        float transparency = 1.0;

        opaqueMode = effectCommon->getOpaqueMode();

        ColorOrTexture transparent = effectCommon->getTransparent();
        double transparentAlpha = (transparent.getType() == ColorOrTexture::COLOR) ? transparent.getColor().getAlpha() : 1.0;

        switch (opaqueMode) {

        case COLLADAFW::EffectCommon::OpaqueMode::RGB_ZERO:
        case COLLADAFW::EffectCommon::OpaqueMode::A_ZERO:
            transparency = static_cast<float>(1.0 - transparentAlpha * effectCommon->getTransparency().getFloatValue());
            if (!loggedOnce) {
                this->_asset->log("WARNING: unsupported opaque mode:%s fallback to A_ONE\n", opaqueModeToString(opaqueMode).c_str());
                loggedOnce = true;
            }
            break;
        case COLLADAFW::EffectCommon::OpaqueMode::RGB_ONE:
            transparency = static_cast<float>(transparentAlpha * effectCommon->getTransparency().getFloatValue());
            if (!loggedOnce) {
                this->_asset->log("WARNING: unsupported opaque mode:%s fallback to A_ONE\n", opaqueModeToString(opaqueMode).c_str());
                loggedOnce = true;
            }
            break;

        case COLLADAFW::EffectCommon::OpaqueMode::A_ONE:
            transparency = static_cast<float>(transparentAlpha * effectCommon->getTransparency().getFloatValue());
            break;
        case COLLADAFW::EffectCommon::OpaqueMode::UNSPECIFIED_OPAQUE:
        default:
            transparency = static_cast<float>(effectCommon->getOpacity().getColor().getAlpha());
            break;
        }

        return CONFIG_BOOL(asset, "invertTransparency") ? 1 - transparency : transparency;
    }

    float COLLADA2GLTFWriter::isOpaque(const COLLADAFW::EffectCommon* effectCommon) {
        return getTransparency(effectCommon) >= 1;
    }

    void COLLADA2GLTFWriter::registerObjectWithOriginalUID(std::string originalId, shared_ptr <JSONObject> obj, shared_ptr <JSONObject> objLib) {
        if (this->_asset->_originalIDToTrackedObject.count(originalId) == 0) {
            if (!objLib->contains(originalId)) {
                objLib->setValue(originalId, obj);
                this->_asset->_originalIDToTrackedObject[originalId] = obj;
            }
            else {
                this->_asset->log("WARNING:Object with id:%s is already tracked, failed attempt to add object\n", originalId.c_str());
            }
        }
        else {
            this->_asset->log("WARNING:Object with id:%s is already tracked, failed attempt to add object\n", originalId.c_str());
        }
    }

    void COLLADA2GLTFWriter::_storeMaterialBindingArray(const std::string& prefix,
        const std::string& nodeUID,
        const std::string& meshUID,
        MaterialBindingArray &materialBindings) {

        if (this->_asset->containsValueForUniqueId(meshUID) == false) {
            return;
        }

        shared_ptr <GLTFMesh> mesh = static_pointer_cast<GLTFMesh>(this->_asset->getValueForUniqueId(meshUID));
        this->_asset->setOriginalId(meshUID, mesh->getID());

        MaterialBindingsForNodeUID& mb = this->_asset->materialBindingsForNodeUID();
        shared_ptr <MaterialBindingsForMeshUID> materialBindingsMap;
        if (mb.count(nodeUID) == 0) {
            materialBindingsMap = shared_ptr<MaterialBindingsForMeshUID>(new MaterialBindingsForMeshUID());
            mb[nodeUID] = materialBindingsMap;
        }
        else {
            materialBindingsMap = mb[nodeUID];
        }

        //apply prefix on MeshUID
        std::string prefixedMeshUID = prefix + meshUID;
        shared_ptr <MaterialBindingsPrimitiveMap> materialBindingsPrimitiveMap;
        if (materialBindingsMap->count(prefixedMeshUID) == 0) {
            materialBindingsPrimitiveMap = shared_ptr<MaterialBindingsPrimitiveMap>(new MaterialBindingsPrimitiveMap());
            (*materialBindingsMap)[prefixedMeshUID] = materialBindingsPrimitiveMap;
        }
        else {
            materialBindingsPrimitiveMap = (*materialBindingsMap)[prefixedMeshUID];
        }

        GLTF::JSONValueVector primitives = mesh->getPrimitives()->values();
        for (size_t j = 0; j < primitives.size(); j++) {
            shared_ptr <GLTF::GLTFPrimitive> primitive = static_pointer_cast<GLTFPrimitive>(primitives[j]);
            //FIXME: consider optimizing this with a hashtable, would be better if it was coming that way from OpenCOLLADA
            int materialBindingIndex = -1;
            for (size_t k = 0; k < materialBindings.getCount(); k++) {
                if (materialBindings[k].getMaterialId() == primitive->getMaterialObjectID()) {
                    materialBindingIndex = (int)k;
                }
            }
            if (materialBindingIndex != -1) {
                shared_ptr <COLLADAFW::MaterialBinding> materialBinding(new COLLADAFW::MaterialBinding(materialBindings[materialBindingIndex]));
                (*materialBindingsPrimitiveMap)[primitive->getMaterialObjectID()] = materialBinding;
            }
        }
        return;
    }

    bool bufferEquals(float* first, float* second, int size, double epsilon) {
        for (int i = 0; i < size; i++) {
            if (fabs(first[i] - second[i]) > epsilon) {
                return false;
            }
        }
        return true;
    }

    COLLADABU::Math::Matrix4 getTransformationMatrix(const Transformation* transform) {
        switch (transform->getTransformationType()) {
        case Transformation::ROTATE: {
            Rotate* rotate = (Rotate*)transform;
            COLLADABU::Math::Vector3 axis = rotate->getRotationAxis();
            axis.normalise();
            double angle = rotate->getRotationAngle();
            return COLLADABU::Math::Matrix4(COLLADABU::Math::Quaternion(COLLADABU::Math::Utils::degToRad(angle), axis));
        }
        case Transformation::TRANSLATE: {
            Translate* translate = (Translate*)transform;
            const COLLADABU::Math::Vector3& translation = translate->getTranslation();
            COLLADABU::Math::Matrix4 translationMatrix;
            translationMatrix.makeTrans(translation);
            return translationMatrix;
        }
        case Transformation::SCALE: {
            Scale* scale = (Scale*)transform;
            const COLLADABU::Math::Vector3& scaleVector = scale->getScale();
            COLLADABU::Math::Matrix4 scaleMatrix;
            scaleMatrix.makeScale(scaleVector);
            return scaleMatrix;
        }
        case Transformation::MATRIX: {
            return ((Matrix*)transform)->getMatrix();
        }
        }
        return COLLADABU::Math::Matrix4::IDENTITY;
    }

    COLLADABU::Math::Matrix4 getFlattenedTransform(vector<const Transformation*> transforms) {
        COLLADABU::Math::Matrix4 matrix = COLLADABU::Math::Matrix4::IDENTITY;
        for (const Transformation* transform : transforms) {
            switch (transform->getTransformationType())  {
            case Transformation::ROTATE: {
                Rotate* rotate = (Rotate*)transform;
                COLLADABU::Math::Vector3 axis = rotate->getRotationAxis();
                axis.normalise();
                double angle = rotate->getRotationAngle();
                matrix = matrix * COLLADABU::Math::Matrix4(COLLADABU::Math::Quaternion(COLLADABU::Math::Utils::degToRad(angle), axis));
                break;
            }
            case Transformation::TRANSLATE: {
                Translate* translate = (Translate*)transform;
                const COLLADABU::Math::Vector3& translation = translate->getTranslation();
                COLLADABU::Math::Matrix4 translationMatrix;
                translationMatrix.makeTrans(translation);
                matrix = matrix * translationMatrix;
                break;
            }
            case Transformation::SCALE: {
                Scale* scale = (Scale*)transform;
                const COLLADABU::Math::Vector3& scaleVector = scale->getScale();
                COLLADABU::Math::Matrix4 scaleMatrix;
                scaleMatrix.makeScale(scaleVector);
                matrix = matrix * scaleMatrix;
                break;
            }
            case Transformation::MATRIX: {
                Matrix* transformMatrix = (Matrix*)transform;
                matrix = matrix * transformMatrix->getMatrix();
                break;
            }
            case Transformation::LOOKAT: {
                Lookat* lookAt = (Lookat*)transform;
                buildLookAtMatrix(lookAt, matrix);
                break;
            }
            }
        }
        return matrix;
    }

    map<COLLADAFW::Transformation::TransformationType, float*> decomposeTransformationMatrix(COLLADABU::Math::Matrix4 matrix, vector<const Transformation*> transforms) {
        map<COLLADAFW::Transformation::TransformationType, float*> decomposition;
        float* scale = new float[3];
        float* translation = new float[3];
        float* rotation = new float[4];
        GLTF::decomposeMatrix(matrix, translation, rotation, scale);

        if (scale[0] == 0.0 && scale[1] == 0.0 && scale[2] == 0.0)
        {
            // Matrix decompose failed because of a uniform scaling of 0 in the transformations.
            // We've lost the rotation information. We'll have to manually extract the rotations.
            Math::Quaternion rotationQuat = Math::Quaternion::IDENTITY;
            for (const Transformation* transform : transforms)
            {
                if (transform->getTransformationType() == Transformation::ROTATE)
                {
                    Rotate* rotate = (Rotate*)transform;
                    Math::Vector3 axis = rotate->getRotationAxis();
                    axis.normalise();
                    double angle = rotate->getRotationAngle();
                    rotationQuat = rotationQuat * Math::Quaternion(Math::Utils::degToRad(angle), axis);
                }
            }

            Math::Real angle;
            Math::Vector3 axis;
            rotationQuat.toAngleAxis(angle, axis);
            rotation[0] = (float)axis.x;
            rotation[1] = (float)axis.y;
            rotation[2] = (float)axis.z;
            rotation[3] = (float)angle;
        }
        decomposition[COLLADAFW::Transformation::TransformationType::SCALE] = scale;
        decomposition[COLLADAFW::Transformation::TransformationType::TRANSLATE] = translation;
        decomposition[COLLADAFW::Transformation::TransformationType::ROTATE] = rotation;
        return decomposition;
    }

    bool COLLADA2GLTFWriter::writeNode(const COLLADAFW::Node* node, shared_ptr <GLTF::JSONObject> nodeObject, string nodeOriginalId) {
        GLTFAsset *asset = this->_asset.get();
        const NodePointerArray& nodes = node->getChildNodes();

        std::string uniqueUID = node->getUniqueId().toAscii();
        nodeObject->setString(kName, node->getName());

        this->_asset->_uniqueIDToOpenCOLLADAObject[uniqueUID] = shared_ptr <COLLADAFW::Object>(node->clone());
        this->_asset->setOriginalId(uniqueUID, nodeOriginalId);
        this->_asset->setValueForUniqueId(uniqueUID, nodeObject);
        if (node->getType() == COLLADAFW::Node::JOINT) {
            const string& sid = node->getSid();
            nodeObject->setString(kJointName, sid);
        }

        const InstanceCameraPointerArray& instanceCameras = node->getInstanceCameras();
        size_t camerasCount = instanceCameras.getCount();
        if (camerasCount > 0) {
            InstanceCamera* instanceCamera = instanceCameras[0];
            shared_ptr<GLTF::JSONObject> cameraObject(new GLTF::JSONObject());
            std::string id = instanceCamera->getInstanciatedObjectId().toAscii();
            std::string cameraId = this->_asset->getOriginalId(id);
            nodeObject->setString(kCamera, cameraId);
        }

        const InstanceControllerPointerArray& instanceControllers = node->getInstanceControllers();
        unsigned int count = (unsigned int)instanceControllers.getCount();
        if (count > 0) {
            shared_ptr<JSONObject> skins = this->_asset->root()->createObjectIfNeeded(kSkins);
            for (unsigned int i = 0; i < count; i++) {
                InstanceController* instanceController = instanceControllers[i];
                MaterialBindingArray &materialBindings = instanceController->getMaterialBindings();
                COLLADAFW::UniqueId uniqueId = instanceController->getInstanciatedObjectId();
                if (asset->containsValueForUniqueId(uniqueId.toAscii())) {
                    shared_ptr<JSONString> skinControllerDataUID = static_pointer_cast<JSONString>(asset->getValueForUniqueId(uniqueId.toAscii()));
                    shared_ptr<GLTFSkin> skin = static_pointer_cast<GLTFSkin>(skins->getObject(skinControllerDataUID->getString()));

                    UniqueId meshUniqueId(skin->getSourceUID());
                    _storeMaterialBindingArray("skin-meshes-",
                        node->getUniqueId().toAscii(),
                        meshUniqueId.toAscii(),
                        materialBindings);

                    shared_ptr<JSONArray> skeletons(new JSONArray());

                    for (size_t k = 0; k < instanceController->skeletons().size(); k++) {
                        std::string skeleton = instanceController->skeletons()[k].getFragment();
                        skeletons->appendValue(shared_ptr<JSONString>(new JSONString(skeleton)));
                    }

                    nodeObject->setValue("skeletons", skeletons);
                    nodeObject->setString(kSkin, skin->getId());
                }
            }
        }

        // save mesh
        const InstanceGeometryPointerArray& instanceGeometries = node->getInstanceGeometries();

        count = (unsigned int)instanceGeometries.getCount();
        if (count > 0) {
            for (unsigned int i = 0; i < count; i++) {
                InstanceGeometry* instanceGeometry = instanceGeometries[i];
                COLLADAFW::UniqueId uniqueId = instanceGeometry->getInstanciatedObjectId();
                MaterialBindingArray& materialBindings = instanceGeometry->getMaterialBindings();
                if (materialBindings.getCount() > 0) {
                    MaterialBinding materialBinding = materialBindings[0];
                    COLLADAFW::UniqueId materialId = materialBinding.getReferencedMaterial();
                    // Check if this geometry has already been bound to another node with a different material
                    bool alreadyBound = false;
                    MaterialBindingsForNodeUID nodeBindings = this->_asset->materialBindingsForNodeUID();
                    for (auto nodeBinding : nodeBindings) {
                        shared_ptr<MaterialBindingsForMeshUID> meshBindings = nodeBinding.second;
                        for (auto meshBinding : *(meshBindings.get())) {
                            string meshId = meshBinding.first;
                            if ("meshes-" + uniqueId.toAscii() == meshId) {
                                shared_ptr<MaterialBindingsPrimitiveMap> materialBindings = meshBinding.second;
                                bool materialUsed = false;
                                for (auto materialBinding : *(materialBindings.get())) {
                                    shared_ptr<MaterialBinding> primitiveBindings = materialBinding.second;
                                    if (primitiveBindings->getReferencedMaterial() == materialId) {
                                        materialUsed = true;
                                        break;
                                    }
                                }
                                if (!materialUsed) {
                                    alreadyBound = true;
                                    break;
                                }
                            }
                        }
                        if (alreadyBound) {
                            break;
                        }
                    }
                    if (alreadyBound) {
                        // This is an instance of a material
                        shared_ptr<GLTFMesh> mesh = static_pointer_cast<GLTFMesh>(this->_asset->getValueForUniqueId(uniqueId.toAscii()));

                        COLLADAFW::UniqueId meshUID = COLLADAFW::UniqueId(uniqueId.toAscii());
                        // Create a unique instance of the mesh so that it can bind to different materials
                        while (asset->containsValueForUniqueId(meshUID.toAscii())) {
                            meshUID = COLLADAFW::UniqueId(meshUID.getClassId(), meshUID.getObjectId() + 1, meshUID.getFileId());
                        }

                        std::string meshID = mesh->getID() + "-" + std::to_string(meshUID.getObjectId() + 1);

                        shared_ptr<GLTFMesh> meshInstance = mesh->clone();
                        meshInstance->setID(meshID);
                        meshInstance->setName(meshID);

                        asset->root()->createObjectIfNeeded(kMeshes)->setValue(meshID, meshInstance);
                        asset->setValueForUniqueId(meshUID.toAscii(), meshInstance);
                    }
                }
                _storeMaterialBindingArray("meshes-",
                    node->getUniqueId().toAscii(),
                    instanceGeometry->getInstanciatedObjectId().toAscii(),
                    materialBindings);
            }
        }

        shared_ptr <GLTF::JSONArray> childrenArray(new GLTF::JSONArray());
        nodeObject->setValue(kChildren, childrenArray);

        count = (unsigned int)nodes.getCount();
        for (unsigned int i = 0; i < count; i++)  {
            std::string childOriginalID = nodes[i]->getOriginalId();
            if (childOriginalID.length() == 0) {
                childOriginalID = uniqueIdWithType(kNode, nodes[i]->getUniqueId());
            }
            childrenArray->appendValue(shared_ptr <GLTF::JSONString>(new GLTF::JSONString(childOriginalID)));
        }

        const InstanceNodePointerArray& instanceNodes = node->getInstanceNodes();
        count = (unsigned int)instanceNodes.getCount();
        for (unsigned int i = 0; i < count; i++) {
            InstanceNode* instanceNode = instanceNodes[i];
            std::string id = instanceNode->getInstanciatedObjectId().toAscii();
            shared_ptr<JSONArray> parents;
            if (this->_asset->_uniqueIDToParentsOfInstanceNode.count(id) == 0) {
                parents = shared_ptr<JSONArray>(new JSONArray());
                this->_asset->_uniqueIDToParentsOfInstanceNode[id] = parents;
            }
            else {
                parents = this->_asset->_uniqueIDToParentsOfInstanceNode[id];
            }

            parents->appendValue(shared_ptr<JSONString>(new JSONString(node->getUniqueId().toAscii())));

            if (this->_asset->containsValueForUniqueId(id)) {
                std::string instanceNodeOriginalId = this->_asset->getOriginalId(id);
                childrenArray->appendValue(shared_ptr <GLTF::JSONString>(new GLTF::JSONString(instanceNodeOriginalId)));
            }
        }

        shared_ptr <GLTF::JSONArray> lightsInNode(new GLTF::JSONArray());

        const InstanceLightPointerArray& instanceLights = node->getInstanceLights();
        count = (unsigned int)instanceLights.getCount();

        //For a given light, keep track of all the nodes holding it
        if (count) {
            shared_ptr<JSONObject> extension = this->_asset->root()->createObjectIfNeeded(kExtensions);
            shared_ptr<JSONObject> khrMaterialsCommon = extension->createObjectIfNeeded("KHR_materials_common");
            shared_ptr<JSONObject> lights = khrMaterialsCommon->createObjectIfNeeded(kLights);
            for (unsigned int i = 0; i < count; i++) {
                InstanceLight* instanceLight = instanceLights[i];
                std::string id = instanceLight->getInstanciatedObjectId().toAscii();

                shared_ptr<JSONObject> light = static_pointer_cast<JSONObject>(this->_asset->getValueForUniqueId(id));
                if (light) {
                    std::string lightUID = this->_asset->getOriginalId(id);

                    shared_ptr<JSONArray> listOfNodesPerLight;
                    if (this->_asset->_uniqueIDOfLightToNodes.count(id) == 0) {
                        listOfNodesPerLight = shared_ptr<JSONArray>(new JSONArray());
                        this->_asset->_uniqueIDOfLightToNodes[lightUID] = listOfNodesPerLight;
                    }
                    else {
                        listOfNodesPerLight = this->_asset->_uniqueIDOfLightToNodes[lightUID];
                    }

                    listOfNodesPerLight->appendValue(JSONSTRING(nodeOriginalId));
                    lightsInNode->appendValue(shared_ptr <JSONString>(new JSONString(lightUID)));
                    lights->setValue(lightUID, light);
                }
            }
            //We just want a single light per node
            //https://github.com/KhronosGroup/glTF/issues/13
            //lightKhrMaterialsCommon->setValue("lights", lightsInNode);
            if (lightsInNode->values().size() > 0 && CONFIG_BOOL(asset, "useKhrMaterialsCommon")) {
                shared_ptr<JSONObject> lightExtension = nodeObject->createObjectIfNeeded(kExtensions);
                shared_ptr<JSONObject> lightKhrMaterialsCommon = lightExtension->createObjectIfNeeded("KHR_materials_common");
                lightKhrMaterialsCommon->setValue(kLight, lightsInNode->values()[0]);
                if (count > 1) {
                    //FR: AFAIK no authoring tool export multiple light per node, but we'll warn if that's the case
                    //To fix this, dummy sub nodes should be created.
                    static bool printedOnce = false;
                    if (printedOnce) {
                        this->_asset->log("WARNING: some unhandled lights because some nodes carry more than a single light\n");
                        printedOnce = false;
                    }
                }
            }
        }

        return true;
    }

    // Flattening   [UNFINISHED CODE]
    //conditions for flattening
    // -> same material
    // option to merge of not non-opaque geometry
    //  -> check per primitive that sources / semantic layout matches
    // [meshAttributes]
    // -> for all meshes
    //   -> collect all kind of semantic
    // -> for all meshes
    //   -> get all meshAttributes
    //   -> transforms & write vtx attributes
    bool COLLADA2GLTFWriter::processSceneFlatteningInfo(SceneFlatteningInfo* sceneFlatteningInfo) {
        /*
        MeshFlatteningInfoVector allMeshes = sceneFlatteningInfo->allMeshes;
        //First collect all kind of meshAttributes available
        size_t count = allMeshes.size();
        for (size_t i = 0 ; i < count ; i++) {
        shared_ptr <MeshFlatteningInfo> meshInfo = allMeshes[i];
        MeshVectorSharedPtr *meshes = this->_uniqueIDToMeshes[meshInfo->getUID()];
        // shared_ptr <MeshAttributeVector> meshAttributes = mesh->meshAttributes();
        }
        */
        return true;
    }

    bool COLLADA2GLTFWriter::writeVisualScene(const COLLADAFW::VisualScene* visualScene) {
        //FIXME: only one visual scene assumed/handled
        shared_ptr <GLTF::JSONObject> scenesObject(new GLTF::JSONObject());
        shared_ptr <GLTF::JSONObject> sceneObject(new GLTF::JSONObject());
        shared_ptr <GLTF::JSONObject> nodesObject = this->_asset->root()->createObjectIfNeeded(kNodes);

        const NodePointerArray& nodePointerArray = visualScene->getRootNodes();
        size_t nodeCount = nodePointerArray.getCount();

        this->_asset->root()->setValue(kScenes, scenesObject);
        this->_asset->root()->setString(kScene, "defaultScene");

        scenesObject->setValue("defaultScene", sceneObject); //FIXME: should use this id -> visualScene->getOriginalId()

        //first pass to output children name of our root node
        shared_ptr <GLTF::JSONArray> childrenArray;
        if (_rootTransform) {
            // Add the root transform to the nodes
            std::string yUpNodeID = uniqueIdWithType(kNode, this->_loader.getUniqueId(COLLADAFW::COLLADA_TYPE::NODE));
            nodesObject->setValue(yUpNodeID, _rootTransform);

            // Create a children array for the scene and add the root transform to it
            shared_ptr <GLTF::JSONArray> sceneChildrenArray(new GLTF::JSONArray());
            sceneObject->setValue(kNodes, sceneChildrenArray);
            shared_ptr <GLTF::JSONString> rootIDValue(new GLTF::JSONString(yUpNodeID));
            sceneChildrenArray->appendValue(static_pointer_cast <GLTF::JSONValue> (rootIDValue));

            // Set childrenArray to the root transform's children array so all root nodes will become its children
            childrenArray = std::static_pointer_cast<GLTF::JSONArray>(_rootTransform->getValue(kChildren));
        }
        else {
            // No root transform so just add all root nodes to the scene's children array
            childrenArray = std::shared_ptr<GLTF::JSONArray>(new GLTF::JSONArray());
            sceneObject->setValue(kNodes, childrenArray);
        }

        for (size_t i = 0; i < nodeCount; i++) {
            COLLADAFW::Node* childNode = nodePointerArray[i];
            std::string nodeUID = childNode->getOriginalId();
            if (nodeUID.length() == 0) {
                nodeUID = uniqueIdWithType(kNode, nodePointerArray[i]->getUniqueId());
                childNode->setOriginalId(nodeUID);
            }
            shared_ptr <GLTF::JSONString> nodeIDValue(new GLTF::JSONString(nodeUID));
            childrenArray->appendValue(static_pointer_cast <GLTF::JSONValue> (nodeIDValue));
        }

        vector<const COLLADAFW::Node*> nodes;
        for (size_t i = 0; i < nodeCount; i++) {
            nodes.push_back(nodePointerArray[i]);
        }
        return writeNodes(nodes);
    }

    //--------------------------------------------------------------------
    bool COLLADA2GLTFWriter::writeScene(const COLLADAFW::Scene* scene) {
        return true;
    }

    //--------------------------------------------------------------------
    bool COLLADA2GLTFWriter::writeLibraryNodes(const COLLADAFW::LibraryNodes* libraryNodes) {
        vector<const COLLADAFW::Node*> nodes;
        const NodePointerArray& nodeArray = libraryNodes->getNodes();
        for (size_t i = 0; i < nodeArray.getCount(); i++) {
            const COLLADAFW::Node* node = nodeArray[i];
            std::string id = node->getUniqueId().toAscii();
            if (this->_asset->_uniqueIDToParentsOfInstanceNode.count(id) > 0) {
                shared_ptr<JSONArray> parents = this->_asset->_uniqueIDToParentsOfInstanceNode[id];
                std::vector <shared_ptr <JSONValue> > values = parents->values();
                for (size_t k = 0; k < values.size(); k++) {
                    shared_ptr<JSONString> value = static_pointer_cast<JSONString>(values[k]);
                    shared_ptr<JSONObject> parentNode = static_pointer_cast<JSONObject>(this->_asset->getValueForUniqueId(value->getString()));
                    if (parentNode) {
                        shared_ptr <JSONArray> children = parentNode->createArrayIfNeeded(kChildren);
                        children->appendValue(shared_ptr <JSONString>(new JSONString(node->getOriginalId())));
                    }
                }
            }
            nodes.push_back(node);
        }
        return writeNodes(nodes);
    }

    void writeTransform(shared_ptr<GLTF::GLTFAsset> asset, shared_ptr<GLTF::JSONObject> nodeObject, COLLADABU::Math::Matrix4 matrix, vector<const Transformation*> transformations, bool useTRS) {
        if (useTRS) {
            map<COLLADAFW::Transformation::TransformationType, float*> decomposition = decomposeTransformationMatrix(matrix, transformations);
            float* translation = decomposition[COLLADAFW::Transformation::TransformationType::TRANSLATE];
            float* rotation = decomposition[COLLADAFW::Transformation::TransformationType::ROTATE];
            float* scale = decomposition[COLLADAFW::Transformation::TransformationType::SCALE];

            // Scale distance units if we need to
            translation[0] *= (float)asset->getDistanceScale();
            translation[1] *= (float)asset->getDistanceScale();
            translation[2] *= (float)asset->getDistanceScale();

            bool exportDefaultValues = CONFIG_BOOL(asset, "exportDefaultValues");
            bool exportTranslation = !(!exportDefaultValues &&
                ((translation[0] == 0) && (translation[1] == 0) && (translation[2] == 0)));
            if (exportTranslation)
                nodeObject->setValue("translation", serializeVec3(translation[0], translation[1], translation[2]));

            // Rotation is a quaternion [V, s]
            nodeObject->setValue("rotation", serializeVec4(rotation[0], rotation[1], rotation[2], rotation[3]));

            bool exportScale = !(!exportDefaultValues && ((scale[0] == 1) && (scale[1] == 1) && (scale[2] == 1)));
            if (exportScale)
                nodeObject->setValue("scale", serializeVec3(scale[0], scale[1], scale[2]));
        }
        else {
            matrix.scaleTrans(asset->getDistanceScale());
            bool exportMatrix = !((matrix == COLLADABU::Math::Matrix4::IDENTITY && (CONFIG_BOOL(asset, "exportDefaultValues") == false)));
            if (exportMatrix) {
                nodeObject->setValue("matrix", serializeOpenCOLLADAMatrix4(matrix));
            }
        }
    }

    bool COLLADA2GLTFWriter::writeNodes(vector<const COLLADAFW::Node*> nodes) {
        shared_ptr<GLTF::JSONObject> nodesObject = this->_asset->root()->createObjectIfNeeded(kNodes);
        vector<const Transformation*> nodeTransforms;
        COLLADABU::Math::Matrix4 matrix;
        for (const COLLADAFW::Node* node : nodes) {
            std::string baseId = node->getOriginalId();
            std::string id = baseId;

            shared_ptr <GLTF::JSONObject> nodeObject(new GLTF::JSONObject());
            TransformationPointerArray transformations = node->getTransformations();
            bool needsRoot = true;

            for (size_t i = 0; i < transformations.getCount(); i++) {
                const Transformation* transformation = transformations[i];
                UniqueId animationListId = transformation->getAnimationList();
                if (animationListId.isValid()) {
                    this->_asset->_transformationForAnimationListId[animationListId] = transformation->clone();
                    // Split off any pre-existing transforms
                    if (nodeTransforms.size() > 0) {
                        matrix = getFlattenedTransform(nodeTransforms);
                        if (matrix != COLLADABU::Math::Matrix4::IDENTITY) {
                            shared_ptr<JSONArray> children = nodeObject->createArrayIfNeeded(kChildren);
                            nodeObject = shared_ptr<GLTF::JSONObject>(new GLTF::JSONObject());
                            if (!needsRoot) {
                                id = "_" + baseId + "_split_" + to_string((int)animationListId.getObjectId());
                            }
                            children->appendValue(shared_ptr<JSONString>(new JSONString(id)));
                            writeTransform(this->_asset, nodeObject, matrix, nodeTransforms, false);
                            nodesObject->setValue(id, nodeObject);
                            needsRoot = false;
                        }
                    }

                    // Make a new node to target for the animation
                    shared_ptr<JSONArray> children = nodeObject->createArrayIfNeeded(kChildren);
                    nodeObject = shared_ptr<GLTF::JSONObject>(new GLTF::JSONObject());
                    if (!needsRoot) {
                        id = "_" + baseId + "_target_" + to_string((int)animationListId.getObjectId());
                    }
                    else {
                        needsRoot = false;
                    }
                    this->_asset->_animationListIdForNodeId[id] = animationListId;
                    this->_asset->_nodeIdForAnimationListId[animationListId] = id;
                    children->appendValue(shared_ptr<JSONString>(new JSONString(id)));
                    matrix = getTransformationMatrix(transformation);
                    writeTransform(this->_asset, nodeObject, matrix, nodeTransforms, true);
                    nodesObject->setValue(id, nodeObject);

                    nodeTransforms.clear();
                }
                else {
                    // Flatten this transform into the node.
                    nodeTransforms.push_back(transformation->clone());
                }
            }
            // Write out any remaining transforms 
            matrix = COLLADABU::Math::Matrix4::IDENTITY;
            if (nodeTransforms.size() > 0) {
                matrix = getFlattenedTransform(nodeTransforms);
                if (matrix != COLLADABU::Math::Matrix4::IDENTITY) {
                    if (!needsRoot) {
                        shared_ptr<JSONArray> children = nodeObject->createArrayIfNeeded(kChildren);
                        nodeObject = shared_ptr<GLTF::JSONObject>(new GLTF::JSONObject());
                        id = "_" + baseId + "_split";
                        children->appendValue(shared_ptr<JSONString>(new JSONString(id)));
                    }
                    writeTransform(this->_asset, nodeObject, matrix, nodeTransforms, false);
                }
                nodeTransforms.clear();
            }
            nodesObject->setValue(id, nodeObject);
            writeNode(node, nodeObject, id);

            vector<const COLLADAFW::Node*> children;
            NodePointerArray childNodes = node->getChildNodes();
            if (childNodes.getCount() > 0) {
                for (size_t i = 0; i < childNodes.getCount(); i++) {
					COLLADAFW::Node* childNode = childNodes[i];
					std::string nodeUID = childNode->getOriginalId();
					if (nodeUID.length() == 0) {
						nodeUID = uniqueIdWithType(kNode, childNode->getUniqueId());
						childNode->setOriginalId(nodeUID);
					}
                    children.push_back(childNode);
                }
                writeNodes(children);
            }
        }
        return true;
    }

    //--------------------------------------------------------------------
    bool COLLADA2GLTFWriter::writeGeometry(const COLLADAFW::Geometry* geometry) {
        switch (geometry->getType()) {
        case Geometry::GEO_TYPE_MESH:
        {
            const COLLADAFW::Mesh* mesh = (COLLADAFW::Mesh*)geometry;
            std::string meshUID = geometry->getUniqueId().toAscii();
            if (this->_asset->containsValueForUniqueId(meshUID) == false) {
                shared_ptr<GLTFMesh> cvtMesh = convertOpenCOLLADAMesh((COLLADAFW::Mesh*)mesh, this->_asset.get());
                if (cvtMesh != nullptr) {
                    this->_asset->root()->createObjectIfNeeded(kMeshes)->setValue(cvtMesh->getID(), cvtMesh);
                    this->_asset->setValueForUniqueId(meshUID, cvtMesh);
                }
            }
        }
        break;
        case Geometry::GEO_TYPE_SPLINE:
        case Geometry::GEO_TYPE_CONVEX_MESH:
            // FIXME: handle convertion to mesh
        case Geometry::GEO_TYPE_UNKNOWN:
            //FIXME: handle error
        default:
            return false;
        }

        return true;
    }

    //--------------------------------------------------------------------
    bool COLLADA2GLTFWriter::writeMaterial(const COLLADAFW::Material* material) {
        const UniqueId& effectUID = material->getInstantiatedEffect();
        std::string materialID = material->getUniqueId().toAscii();
        this->_asset->_materialUIDToName[materialID] = material->getName();
        this->_asset->_materialUIDToEffectUID[materialID] = effectUID;
        return true;
    }


    //--------------------------------------------------------------------
    unsigned int __GetGLWrapMode(COLLADAFW::Sampler::WrapMode wrapMode, GLTFProfile *profile) {
        switch (wrapMode) {
        case COLLADAFW::Sampler::WRAP_MODE_UNSPECIFIED:
        case COLLADAFW::Sampler::WRAP_MODE_NONE:
        case COLLADAFW::Sampler::WRAP_MODE_WRAP:
            return profile->getGLenumForString("REPEAT");
        case COLLADAFW::Sampler::WRAP_MODE_MIRROR:
            return profile->getGLenumForString("MIRRORED_REPEAT");
        case COLLADAFW::Sampler::WRAP_MODE_CLAMP:
            return profile->getGLenumForString("CLAMP_TO_EDGE");
        default:
            break;
        }
        return profile->getGLenumForString("REPEAT");
    }

    static unsigned int __GetFilterMode(COLLADAFW::Sampler::SamplerFilter wrapMode, GLTFProfile *profile) {
        switch (wrapMode) {
        case COLLADAFW::Sampler::SAMPLER_FILTER_UNSPECIFIED:
        case COLLADAFW::Sampler::SAMPLER_FILTER_NONE:
        case COLLADAFW::Sampler::SAMPLER_FILTER_LINEAR:
            return profile->getGLenumForString("LINEAR");
        case COLLADAFW::Sampler::SAMPLER_FILTER_NEAREST:
            return profile->getGLenumForString("NEAREST");
        case COLLADAFW::Sampler::SAMPLER_FILTER_NEAREST_MIPMAP_NEAREST:
            return profile->getGLenumForString("NEAREST_MIPMAP_NEAREST");
        case COLLADAFW::Sampler::SAMPLER_FILTER_LINEAR_MIPMAP_NEAREST:
            return profile->getGLenumForString("LINEAR_MIPMAP_NEAREST");
        case COLLADAFW::Sampler::SAMPLER_FILTER_NEAREST_MIPMAP_LINEAR:
            return profile->getGLenumForString("NEAREST_MIPMAP_LINEAR");
        case COLLADAFW::Sampler::SAMPLER_FILTER_LINEAR_MIPMAP_LINEAR:
            return profile->getGLenumForString("LINEAR_MIPMAP_LINEAR");
        default:
            break;
        }
        return profile->getGLenumForString("LINEAR");
    }

    std::string COLLADA2GLTFWriter::getSamplerUIDForParameters(unsigned int wrapS,
        unsigned int wrapT,
        unsigned int minFilter,
        unsigned int maxFilter) {
        std::string samplerHash = GLTFUtils::toString(wrapS) + GLTFUtils::toString(wrapT) + GLTFUtils::toString(minFilter) + GLTFUtils::toString(maxFilter);
        bool addSampler = false;
        size_t index = 0;

        if (this->_asset->_samplerHashtoSamplerIndex.count(samplerHash) == 0) {
            index = this->_asset->_samplerHashtoSamplerIndex.size();
            this->_asset->_samplerHashtoSamplerIndex[samplerHash] = (unsigned int)index;
            addSampler = true;
        }
        else {
            index = this->_asset->_samplerHashtoSamplerIndex[samplerHash];
        }

        std::string samplerUID = "sampler_" + GLTFUtils::toString(index);
        if (addSampler) {
            shared_ptr <JSONObject> sampler2D(new JSONObject());

            sampler2D->setUnsignedInt32("wrapS", wrapS);
            sampler2D->setUnsignedInt32("wrapT", wrapT);
            sampler2D->setUnsignedInt32("minFilter", minFilter);
            sampler2D->setUnsignedInt32("magFilter", maxFilter);

            shared_ptr <GLTF::JSONObject> samplers = this->_asset->root()->createObjectIfNeeded("samplers");
            samplers->setValue(samplerUID, sampler2D);
        }

        return samplerUID;
    }


    void COLLADA2GLTFWriter::_installTextureSlot(Sampler* sampler,
        const std::string& slotName,
        const std::string& texcoord,
        shared_ptr <GLTFAsset> asset,
        shared_ptr<GLTFEffect> cvtEffect)
    {
        assert(sampler);
        assert(asset);
        assert(cvtEffect);
        shared_ptr <JSONObject> values = cvtEffect->getValues();
        shared_ptr <JSONObject> khrMaterialsCommonValues = cvtEffect->getKhrMaterialsCommonValues();
        std::string originalImageUID = asset->getOriginalId(sampler->getSourceImage().toAscii());
        GLTFProfile* profile = asset->profile().get();

        cvtEffect->addSemanticForTexcoordName(texcoord, slotName);
        shared_ptr <JSONObject> slotObject(new JSONObject());

        //do we need to export a new texture ? if yes compose a new unique ID
        slotObject->setUnsignedInt32(kType, profile->getGLenumForString("SAMPLER_2D"));

        //do we need a new sampler ?
        std::string samplerUID = this->getSamplerUIDForParameters(__GetGLWrapMode(sampler->getWrapS(), profile),
            __GetGLWrapMode(sampler->getWrapT(), profile),
            __GetFilterMode(sampler->getMinFilter(), profile),
            __GetFilterMode(sampler->getMagFilter(), profile));

        std::string textureUID = "texture_" + originalImageUID;
        shared_ptr <GLTF::JSONObject> textures = asset->root()->createObjectIfNeeded("textures");
        if (textures->contains(textureUID) == false) {
            shared_ptr <JSONObject> textureObject(new JSONObject());
            textureObject->setString(kSource, originalImageUID);
            textureObject->setString("sampler", samplerUID);
            textureObject->setUnsignedInt32("format", profile->getGLenumForString("RGBA"));

            if (CONFIG_BOOL(asset, "exportDefaultValues")) {
                textureObject->setUnsignedInt32("internalFormat", profile->getGLenumForString("RGBA"));
                //UNSIGNED_BYTE is default https://github.com/KhronosGroup/glTF/issues/195
                textureObject->setUnsignedInt32(kType, profile->getGLenumForString("UNSIGNED_BYTE"));
            }
            textureObject->setUnsignedInt32(kTarget, profile->getGLenumForString("TEXTURE_2D"));
            textures->setValue(textureUID, textureObject);
        }        
        shared_ptr<JSONArray> slotArray(new JSONArray());
        slotArray->appendValue(shared_ptr<JSONString>(new JSONString(textureUID)));
        slotObject->setValue("value", slotArray);
        values->setValue(slotName, slotObject);
        khrMaterialsCommonValues->setValue(slotName, slotObject);
    }

    void COLLADA2GLTFWriter::handleEffectSlot(const COLLADAFW::EffectCommon* commonProfile,
        std::string slotName,
        shared_ptr <GLTFEffect> cvtEffect,
        shared_ptr <JSONObject> extras) {
        shared_ptr <JSONObject> values = cvtEffect->getValues();
        shared_ptr <JSONObject> khrMaterialsCommonValues = cvtEffect->getKhrMaterialsCommonValues();
        GLTFAsset *asset = this->_asset.get();
        GLTFProfile* profile = asset->profile().get();

        ColorOrTexture slot;

        if (slotName == "diffuse")
            slot = commonProfile->getDiffuse();
        else if (slotName == "ambient") {
            bool lockAmbientWithDiffuse = false;
            if (extras) {
                lockAmbientWithDiffuse = extras->getBool("ambient_diffuse_lock");
            }
            if (lockAmbientWithDiffuse) {
                slot = commonProfile->getDiffuse();
            }
            else {
                slot = commonProfile->getAmbient();
            }
        }
        else if (slotName == "emission")
            slot = commonProfile->getEmission();
        else if (slotName == "specular")
            slot = commonProfile->getSpecular();
        else if (slotName == "reflective")
            slot = commonProfile->getReflective();
        else if (slotName == "bump") {
            //here we handle an extras slot
            //for other extras, this will need refactoring
            if (extras->contains("textures")) {
                shared_ptr <JSONObject> textures = extras->getObject("textures");
                if (textures->contains("bump")) {
                    shared_ptr <JSONObject> bump = textures->getObject("bump");

                    std::string texture = bump->getString("texture");
                    std::string texcoord = bump->getString("texcoord");
                    const SamplerPointerArray& samplers = commonProfile->getSamplerPointerArray();
                    for (size_t i = 0; i < samplers.getCount(); i++) {
                        if (samplers[i]->getSid() == texture) {
                            Sampler* sampler = (Sampler*)samplers[i];
                            _installTextureSlot(sampler, slotName, texcoord, this->_asset, cvtEffect);
                        }
                    }
                }
            }
        }
        else
            return;

        //retrieve the type, parameterName -> symbol -> type
        double red = 1, green = 1, blue = 1, alpha = 1;
        if (slot.isColor() && (slotName != "reflective")) {
            const Color& color = slot.getColor();
            if (slot.getType() != COLLADAFW::ColorOrTexture::UNSPECIFIED) {
                red = color.getRed();
                green = color.getGreen();
                blue = color.getBlue();
                alpha = color.getAlpha();
            }

            shared_ptr <JSONObject> slotObject(new JSONObject());
            slotObject->setValue("value", serializeVec4(red, green, blue, alpha));
            slotObject->setUnsignedInt32(kType, profile->getGLenumForString("FLOAT_VEC4"));
            khrMaterialsCommonValues->setValue(slotName, slotObject);
            values->setValue(slotName, slotObject);
        }
        else if (slot.isTexture()) {
            const Texture&  texture = slot.getTexture();
            const SamplerPointerArray& samplers = commonProfile->getSamplerPointerArray();
            Sampler* sampler = (Sampler*)samplers[texture.getSamplerId()];
            std::string texcoord = texture.getTexcoord();
            _installTextureSlot(sampler, slotName, texcoord, this->_asset, cvtEffect);
        }
        else {
            // nothing to do, no texture or color
        }
    }

    bool COLLADA2GLTFWriter::writeEffect(const COLLADAFW::Effect* effect) {
        GLTFAsset *asset = this->_asset.get();
        shared_ptr<GLTFProfile> profile = asset->profile();
        const COLLADAFW::CommonEffectPointerArray& commonEffects = effect->getCommonEffects();

        if (commonEffects.getCount() > 0) {
            std::string uniqueId = "";
#if EXPORT_MATERIALS_AS_EFFECTS
            uniqueId += "material.";
#else
            uniqueId += "effect.";
#endif
            uniqueId += GLTF::GLTFUtils::toString(effect->getUniqueId().getObjectId());;

            shared_ptr <GLTFEffect> cvtEffect = make_shared<GLTFEffect>(effect->getOriginalId(), profile);
            shared_ptr <JSONObject> values(new JSONObject());
            shared_ptr <JSONObject> khrMaterialsCommonValues(new JSONObject());

            cvtEffect->setValues(values);
            cvtEffect->setKhrMaterialsCommonValues(khrMaterialsCommonValues);

            const COLLADAFW::EffectCommon* effectCommon = commonEffects[0];

            switch (effectCommon->getShaderType()) {
            case COLLADAFW::EffectCommon::SHADER_BLINN:
                cvtEffect->setLightingModel("Blinn");
                break;
            case COLLADAFW::EffectCommon::SHADER_CONSTANT:
                cvtEffect->setLightingModel("Constant");
                break;
            case COLLADAFW::EffectCommon::SHADER_PHONG:
                cvtEffect->setLightingModel("Phong");
                break;
            case COLLADAFW::EffectCommon::SHADER_LAMBERT:
                cvtEffect->setLightingModel("Lambert");
                break;
            default:
                break;
            }

            shared_ptr<JSONObject> extras = this->_extraDataHandler->getExtras(effect->getUniqueId());

            handleEffectSlot(effectCommon, "diffuse", cvtEffect, extras);
            handleEffectSlot(effectCommon, "ambient", cvtEffect, extras);
            handleEffectSlot(effectCommon, "emission", cvtEffect, extras);
            handleEffectSlot(effectCommon, "specular", cvtEffect, extras);
            handleEffectSlot(effectCommon, "reflective", cvtEffect, extras);
            handleEffectSlot(effectCommon, "bump", cvtEffect, extras);

            if (CONFIG_BOOL(asset, "alwaysExportFilterColor")) {
                shared_ptr <JSONObject> slotObject(new JSONObject());
                slotObject->setValue("value", serializeVec4(1, 1, 1, 1));
                slotObject->setUnsignedInt32(kType, profile->getGLenumForString("FLOAT_VEC4"));
                values->setValue("filterColor", slotObject);
            }

            if (!isOpaque(effectCommon) || CONFIG_BOOL(asset, "alwaysExportTransparency")) {
                shared_ptr <JSONObject> transparencyObject(new JSONObject());
				shared_ptr <JSONArray> transparencyArray(new JSONArray());
				transparencyObject->setUnsignedInt32(kType, profile->getGLenumForString("FLOAT"));
				transparencyArray->appendValue(shared_ptr<JSONNumber>(new JSONNumber(this->getTransparency(effectCommon))));
				transparencyObject->setValue("value", transparencyArray);
                values->setValue("transparency", transparencyObject);
                khrMaterialsCommonValues->setValue("transparency", transparencyObject);
            }

            //should check if has specular first and the lighting model (if not lambert)
            double shininess = effectCommon->getShininess().getFloatValue();
            if (shininess >= 0) {
                if (shininess < 1) {
                    shininess *= 128.0;
                }
                shared_ptr<JSONObject> shininessObject(new JSONObject());
                shared_ptr<JSONArray> shininessArray(new JSONArray());
                shininessObject->setUnsignedInt32(kType, profile->getGLenumForString("FLOAT"));
                shininessArray->appendValue(shared_ptr<JSONNumber>(new JSONNumber(shininess)));
                shininessObject->setValue("value", shininessArray);
                values->setValue("shininess", shininessObject);
                khrMaterialsCommonValues->setValue("shininess", shininessObject);
            }

            shared_ptr<JSONObject> materials = this->_asset->root()->createObjectIfNeeded(kMaterials);
            materials->setValue(cvtEffect->getID(), cvtEffect);
            this->_asset->setValueForUniqueId(effect->getUniqueId().toAscii(), cvtEffect);

        }
        return true;
    }

    //--------------------------------------------------------------------
    bool COLLADA2GLTFWriter::writeCamera(const COLLADAFW::Camera* camera) {
        shared_ptr <GLTF::JSONObject> camerasObject = static_pointer_cast <GLTF::JSONObject> (this->_asset->root()->getValue("cameras"));
        if (!camerasObject) {
            camerasObject = shared_ptr <GLTF::JSONObject>(new GLTF::JSONObject());
            this->_asset->root()->setValue("cameras", camerasObject);
        }

        shared_ptr <GLTF::JSONObject> cameraObject(new GLTF::JSONObject());
        shared_ptr <GLTF::JSONObject> projectionObject(new GLTF::JSONObject());

        std::string cameraUID = camera->getUniqueId().toAscii();
        this->_asset->setValueForUniqueId(cameraUID, cameraObject);
        this->_asset->setOriginalId(cameraUID, camera->getOriginalId());
        camerasObject->setValue(camera->getOriginalId(), cameraObject);

        if (!camera->getName().empty())
        {
            cameraObject->setString(kName, camera->getName());
        }

        switch (camera->getCameraType()) {
        case Camera::UNDEFINED_CAMERATYPE:
            this->_asset->log("WARNING: unknown camera type: using perspective\n");
            break;
        case Camera::ORTHOGRAPHIC:
        {
            cameraObject->setString(kType, "orthographic");
            cameraObject->setValue("orthographic", projectionObject);
            switch (camera->getDescriptionType()) {
            case Camera::UNDEFINED: //!< The orthographic camera object is invalid
                this->_asset->log("WARNING: Invalid orthographic camera. At least 2 of xmag, ymag & aspect ratio must be provided. Defaulting to 1.\n");
                projectionObject->setDouble("xmag", 1.0);
                projectionObject->setDouble("ymag", 1.0);
                break;
            case Camera::SINGLE_X: //!< Only xmag. ymag default to 1.
                this->_asset->log("WARNING: Invalid orthographic camera. At least 2 of xmag, ymag & aspect ratio must be provided. Defaulting ymag to 1.\n");
                projectionObject->setDouble("xmag", camera->getXMag().getValue());
                projectionObject->setDouble("ymag", 1.0);
                break;
            case Camera::SINGLE_Y: //!< Only ymag. xmag defaults to 1.
                this->_asset->log("WARNING: Invalid orthographic camera. At least 2 of xmag, ymag & aspect ratio must be provided. Defaulting xmag to 1.\n");
                projectionObject->setDouble("xmag", 1.0);
                projectionObject->setDouble("ymag", camera->getYMag().getValue());
                break;
            case Camera::X_AND_Y: //!< xmag and ymag.
                projectionObject->setDouble("xmag", camera->getXMag().getValue());
                projectionObject->setDouble("ymag", camera->getYMag().getValue());
                break;
            case Camera::ASPECTRATIO_AND_X: //!< aspect ratio and xmag. Compute ymag.
            {
                double x = camera->getXMag().getValue();
                projectionObject->setDouble("xmag", x);
                projectionObject->setDouble("ymag", x / camera->getAspectRatio().getValue());
            }
            break;
            case Camera::ASPECTRATIO_AND_Y: //!< aspect ratio ymag. Compute xmag.
            {
                double y = camera->getYMag().getValue();
                projectionObject->setDouble("xmag", y * camera->getAspectRatio().getValue());
                projectionObject->setDouble("ymag", y);
            }
            break;
            }

        }
        break;
        case Camera::PERSPECTIVE:
        {
            cameraObject->setString(kType, "perspective");
            cameraObject->setValue("perspective", projectionObject);
            switch (camera->getDescriptionType()) {
            case Camera::UNDEFINED: //!< The perspective camera object is invalid
            case Camera::SINGLE_X: //!< Only xfov. Default yfov to 1.0.
                this->_asset->log("WARNING: Invalid perspective camera. At least yfov must be provided. Defaulting yfov to 1.\n");
                projectionObject->setDouble("yfov", 1.0);
                break;
            case Camera::SINGLE_Y: //!< Only yfov. Use aspect ratio of the canvas.
                projectionObject->setDouble("yfov", camera->getYFov().getValue() * radianPerDegree);
                break;
            case Camera::X_AND_Y: //!< xfov and yfov. Compute aspect ratio.
            {
                double x = camera->getXFov().getValue() * radianPerDegree;
                double y = camera->getYFov().getValue() * radianPerDegree;
                projectionObject->setDouble("yfov", y);
                projectionObject->setDouble(kAspectRatio, x / y);
            }
            break;
            case Camera::ASPECTRATIO_AND_X: //!< aspect ratio and xfov. Compute yfov.
            {
                double x = camera->getXFov().getValue() * radianPerDegree;
                double aspect_ratio = camera->getAspectRatio().getValue();
                projectionObject->setDouble("yfov", x / aspect_ratio);
                projectionObject->setDouble(kAspectRatio, aspect_ratio);
            }
            break;
            case Camera::ASPECTRATIO_AND_Y: //!< aspect ratio and yfov.
                projectionObject->setDouble("yfov", camera->getYFov().getValue() * radianPerDegree);
                projectionObject->setDouble(kAspectRatio, camera->getAspectRatio().getValue());
                break;
            }

        }
        break;
        }

        projectionObject->setDouble("znear", camera->getNearClippingPlane().getValue() * _asset->getDistanceScale());
        projectionObject->setDouble("zfar", camera->getFarClippingPlane().getValue() * _asset->getDistanceScale());

        return true;
    }

    //--------------------------------------------------------------------
    bool COLLADA2GLTFWriter::writeImage(const COLLADAFW::Image* openCOLLADAImage) {
        shared_ptr <GLTF::JSONObject> images = this->_asset->root()->createObjectIfNeeded(kImages);
        shared_ptr <GLTF::JSONObject> image(new GLTF::JSONObject());

        std::string imageUID = openCOLLADAImage->getUniqueId().toAscii();
        this->_asset->setValueForUniqueId(imageUID, image);
        this->_asset->setOriginalId(imageUID, openCOLLADAImage->getOriginalId());
        images->setValue(openCOLLADAImage->getOriginalId(), image);

        if (!openCOLLADAImage->getName().empty())
        {
            image->setString(kName, openCOLLADAImage->getName());
        }

        shared_ptr<JSONObject> extras = this->_extraDataHandler->getExtras(openCOLLADAImage->getUniqueId());
        if (extras->contains("has_alpha"))
        {
            image->setBool("has_alpha", extras->getBool("has_alpha"));
        }

        const COLLADABU::URI &imageURI = openCOLLADAImage->getImageURI();
        if (is_dataUri(imageURI.originalStr()))
        {
            // We already have a data uri so just use it
            image->setString(kURI, imageURI.originalStr());
            return true;
        }

        std::string relPathFile = imageURI.getPathFile();
        std::string pathDir = getPathDir(imageURI);
        if (pathDir.substr(0, 2) != "./") {
            relPathFile = pathDir + imageURI.getPathFile();
        }
        else {
            relPathFile = pathDir.substr(2) + imageURI.getPathFile();
        }

        if (CONFIG_BOOL(_asset, "embedResources"))
        {
            COLLADABU::URI inputURI(_asset->getInputFilePath().c_str());
            std::string imageFullPath = COLLADABU::URI::uriDecode(getPathDir(inputURI) + relPathFile);

            std::ifstream fin(imageFullPath, ios::in | ios::binary);
            if (fin.is_open())
            {
                std::ostringstream ss;
                ss << fin.rdbuf();
                COLLADABU::URI u(imageFullPath);
                std::string ext = u.getPathExtension();
                std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                std::string contentType = "application/octet-stream";
                if (ext == "png")
                {
                    contentType = "image/png";
                }
                else if (ext == "gif")
                {
                    contentType = "image/gif";
                }
                else if (ext == "jpg" || ext == "jpeg")
                {
                    contentType = "image/jpeg";
                }

                image->setString(kURI, create_dataUri(ss.str(), contentType));

                return true;
            }
        }

        image->setString(kURI, COLLADABU::URI::uriEncode(this->_asset->resourceOuputPathForPath(relPathFile)));

        return true;
    }

    //--------------------------------------------------------------------
    bool COLLADA2GLTFWriter::writeLight(const COLLADAFW::Light* light) {
        //FIXME: add projection
        shared_ptr <JSONObject> glTFLight(new JSONObject());
        shared_ptr <JSONObject> description(new JSONObject());

        COLLADAFW::Light::LightType lightType = light->getLightType();
        Color color = light->getColor();

        float constantAttenuation = (float)light->getConstantAttenuation().getValue();
        shared_ptr<JSONArray> constantAttenuationArray = shared_ptr<JSONArray>(new JSONArray());
        constantAttenuationArray->appendValue(shared_ptr<JSONNumber>(new JSONNumber(constantAttenuation)));
        float linearAttenuation = (float)light->getLinearAttenuation().getValue();
        shared_ptr<JSONArray> linearAttenuationArray = shared_ptr<JSONArray>(new JSONArray());
        linearAttenuationArray->appendValue(shared_ptr<JSONNumber>(new JSONNumber(linearAttenuation)));
        float quadraticAttenuation = (float)light->getQuadraticAttenuation().getValue();
        shared_ptr<JSONArray> quadraticAttenuationArray = shared_ptr<JSONArray>(new JSONArray());
        quadraticAttenuationArray->appendValue(shared_ptr<JSONNumber>(new JSONNumber(quadraticAttenuation)));

        shared_ptr <JSONValue> lightColor = serializeVec3(color.getRed(), color.getGreen(), color.getBlue());
        
		switch (lightType) {
		case COLLADAFW::Light::AMBIENT_LIGHT: {
			glTFLight->setString(kType, "ambient");
			break;
		}
		case COLLADAFW::Light::DIRECTIONAL_LIGHT: {
			glTFLight->setString(kType, "directional");
			break;
		}
        case COLLADAFW::Light::POINT_LIGHT: {
            glTFLight->setString(kType, "point");

            description->setValue("constantAttenuation", constantAttenuationArray);             
            description->setValue("linearAttenuation", linearAttenuationArray);
            description->setValue("quadraticAttenuation", quadraticAttenuationArray);
			break;
        }
        case COLLADAFW::Light::SPOT_LIGHT: {
            glTFLight->setString(kType, "spot");

            float fallOffAngle =  (float)(light->getFallOffAngle().getValue() * radianPerDegree);
            shared_ptr<JSONArray> fallOffAngleArray = shared_ptr<JSONArray>(new JSONArray());
            fallOffAngleArray->appendValue(shared_ptr<JSONNumber>(new JSONNumber(fallOffAngle)));

            float fallOffExponent = (float)light->getFallOffExponent().getValue();
            shared_ptr<JSONArray> fallOffExponentArray = shared_ptr<JSONArray>(new JSONArray());
            fallOffExponentArray->appendValue(shared_ptr<JSONNumber>(new JSONNumber(fallOffExponent)));
                
            description->setValue("constantAttenuation", constantAttenuationArray);
            description->setValue("linearAttenuation", linearAttenuationArray);
            description->setValue("quadraticAttenuation", quadraticAttenuationArray);
                
            description->setValue("fallOffAngle", fallOffAngleArray);
            description->setValue("fallOffExponent", fallOffExponentArray);
			break;
        }
        default: {
            return false;
        }}
        
        description->setValue("color", lightColor);
        glTFLight->setValue(glTFLight->getString(kType), description);

        if (!light->getName().empty())
        {
            glTFLight->setString(kName, light->getName());
        }

        const std::string &lightId = light->getUniqueId().toAscii();
        this->_asset->setValueForUniqueId(lightId, glTFLight);
        this->_asset->setOriginalId(lightId, light->getOriginalId());

        shared_ptr<JSONArray> lightsIds = this->_asset->root()->createArrayIfNeeded("lightsIds");
        lightsIds->appendValue(std::make_shared<JSONString>(light->getOriginalId()));

        return true;
    }

    /**
    * This just initializes the animation and makes a placeholder in the glTF tree
    */
    bool COLLADA2GLTFWriter::writeAnimation(const COLLADAFW::Animation* animation) {
        shared_ptr<GLTFAnimation> cvtAnimation = shared_ptr<GLTFAnimation>(new GLTFAnimation(animation, this->_asset.get()));
        cvtAnimation->setOriginalID(animation->getOriginalId());
        shared_ptr<JSONObject> animations = this->_asset->root()->createObjectIfNeeded("animations");
        animations->setValue(animation->getUniqueId().toAscii(), cvtAnimation);
        return true;
    }

    COLLADABU::Math::Quaternion writeMatrixAnimation(COLLADABU::Math::Matrix4 matrix, size_t index, float* translation, float* rotation, float* scale, COLLADABU::Math::Quaternion lastQuat) {
        map<COLLADAFW::Transformation::TransformationType, float*> trs = decomposeTransformationMatrix(matrix, vector<const COLLADAFW::Transformation*>());
        float* trsTranslation = trs[COLLADAFW::Transformation::TRANSLATE];
        translation[index * 3] = (float)trsTranslation[0];
        translation[index * 3 + 1] = (float)trsTranslation[1];
        translation[index * 3 + 2] = (float)trsTranslation[2];
        float* trsRotation = trs[COLLADAFW::Transformation::ROTATE];
        // There are two decompositions for every quaternion, we want the one that is closest to the last one so that interpolation works correctly
        COLLADABU::Math::Quaternion quat = COLLADABU::Math::Quaternion(trsRotation[3], trsRotation[0], trsRotation[1], trsRotation[2]);
        double diff = fabs(quat.w - lastQuat.w) + fabs(quat.x - lastQuat.x) + fabs(quat.y - lastQuat.y) + fabs(quat.z - lastQuat.z);
        COLLADABU::Math::Quaternion negQuat = quat * -1.0;
        double negDiff = fabs(negQuat.w - lastQuat.w) + fabs(negQuat.x - lastQuat.x) + fabs(negQuat.y - lastQuat.y) + fabs(negQuat.z - lastQuat.z);
        if (negDiff < diff) {
            quat = negQuat;
        }
        rotation[index * 4] = (float)quat.x;
        rotation[index * 4 + 1] = (float)quat.y;
        rotation[index * 4 + 2] = (float)quat.z;
        rotation[index * 4 + 3] = (float)quat.w;
        float* trsScale = trs[COLLADAFW::Transformation::SCALE];
        scale[index * 3] = (float)trsScale[0];
        scale[index * 3 + 1] = (float)trsScale[1];
        scale[index * 3 + 2] = (float)trsScale[2];
        return quat;
    }

    /**
    * Combine animations where necessary and write out animation data
    */
    bool COLLADA2GLTFWriter::writeAnimationList(const COLLADAFW::AnimationList* animationList) {
        shared_ptr<JSONObject> animations = this->_asset->root()->createObjectIfNeeded("animations");
        const COLLADAFW::AnimationList::AnimationBindings &animationBindings = animationList->getAnimationBindings();
        string nodeId = this->_asset->_nodeIdForAnimationListId[animationList->getUniqueId()];
        shared_ptr<JSONObject> nodes = this->_asset->root()->getObject("nodes");
        shared_ptr<JSONObject> node = nodes->getObject(nodeId);

        size_t animationBindingsLength = animationBindings.getCount();

        // Normalize the number of keyframes across animations that need to be combined
        shared_ptr<GLTFAnimation> rootAnimation = static_pointer_cast<GLTFAnimation>(animations->getObject(animationBindings[0].animation.toAscii()));
        for (size_t i = 1; i < animationBindingsLength; i++) {
            COLLADAFW::AnimationList::AnimationBinding animationBinding = animationBindings[i];
            shared_ptr<GLTFAnimation> animation = static_pointer_cast<GLTFAnimation>(animations->getObject(animationBinding.animation.toAscii()));
            for (size_t j = 0; j < animation->getCount(); j++) {
                double keyFrame = animation->getKeyFrameAtIndex(j);
                rootAnimation->addInterpolatedKeyFrame(keyFrame);
            }
        }

        for (size_t i = 1; i < animationBindingsLength; i++) {
            COLLADAFW::AnimationList::AnimationBinding animationBinding = animationBindings[i];
            shared_ptr<GLTFAnimation> animation = static_pointer_cast<GLTFAnimation>(animations->getObject(animationBinding.animation.toAscii()));
            for (size_t j = 0; j < rootAnimation->getCount(); j++) {
                double keyFrame = rootAnimation->getKeyFrameAtIndex(j);
                animation->addInterpolatedKeyFrame(keyFrame);
            }
        }

        const int numKeyFrames = rootAnimation->getCount();
        bool hasTranslation = false;
        bool hasRotation = false;
        bool hasScale = false;
        float* keyFrames = new float[numKeyFrames];
        float* rotation = new float[numKeyFrames * 4];
        float* scale = new float[numKeyFrames * 3];
        float* translation = new float[numKeyFrames * 3];
        shared_ptr<JSONArray> translationArray = node->getArray("translation");
        float translateX = static_pointer_cast<JSONNumber>(translationArray->values()[0])->getDouble();
        float translateY = static_pointer_cast<JSONNumber>(translationArray->values()[1])->getDouble();
        float translateZ = static_pointer_cast<JSONNumber>(translationArray->values()[2])->getDouble();
        bool* writeTranslation = new bool[numKeyFrames * 3];
        rootAnimation->setTargetNodeId(nodeId);

        COLLADABU::Math::Quaternion lastQuat = COLLADABU::Math::Quaternion::IDENTITY;
        for (size_t i = 0; i < animationBindingsLength; i++) {
            COLLADAFW::AnimationList::AnimationBinding animationBinding = animationBindings[i];
            shared_ptr<GLTFAnimation> animation = static_pointer_cast<GLTFAnimation>(animations->getObject(animationBinding.animation.toAscii()));
            COLLADAFW::AnimationList::AnimationClass animationClass = animationBinding.animationClass;
            GLTFAnimation::AnimationType animationType;
            const Transformation* transform = this->_asset->_transformationForAnimationListId[animationList->getUniqueId()];

            for (size_t j = 0; j < animation->getCount(); j++) {
                double keyFrame = animation->getKeyFrameAtIndex(j);
                keyFrames[j] = (float)keyFrame;
                vector<double> values = animation->getValuesAtKeyFrame(keyFrame);
                if (i == 0) {
                    writeTranslation[j * 3] = false;
                    writeTranslation[j * 3 + 1] = false;
                    writeTranslation[j * 3 + 2] = false;
                }

                switch (animationClass) {
                case COLLADAFW::AnimationList::MATRIX4X4: {
                    COLLADABU::Math::Matrix4 matrix = COLLADABU::Math::Matrix4(
                        values[0], values[1], values[2], values[3],
                        values[4], values[5], values[6], values[7],
                        values[8], values[9], values[10], values[11],
                        values[12], values[13], values[14], values[15]
                        );
                    lastQuat = writeMatrixAnimation(matrix, j, translation, rotation, scale, lastQuat);
                    hasTranslation = true;
                    hasRotation = true;
                    hasScale = true;
                    break;
                }
                case COLLADAFW::AnimationList::AXISANGLE: {
                    rotation[j * 4] = (float)values[0];
                    rotation[j * 4 + 1] = (float)values[1];
                    rotation[j * 4 + 2] = (float)values[2];
                    rotation[j * 4 + 3] = (float)values[3];
                    hasRotation = true;
                    break;
                }
                case COLLADAFW::AnimationList::ANGLE: {
                    if (transform->getTransformationType() == COLLADAFW::Transformation::TransformationType::ROTATE) {
                        COLLADAFW::Rotate* rotate = (COLLADAFW::Rotate*)transform;
                        COLLADABU::Math::Quaternion quat = COLLADABU::Math::Quaternion(COLLADABU::Math::Utils::degToRad(values[0]), rotate->getRotationAxis());
                        rotation[j * 4] = (float)quat.x;
                        rotation[j * 4 + 1] = (float)quat.y;
                        rotation[j * 4 + 2] = (float)quat.z;
                        rotation[j * 4 + 3] = (float)quat.w;
                        hasRotation = true;
                    }
                    break;
                }
                case COLLADAFW::AnimationList::POSITION_XYZ: {
                    if (!writeTranslation[j * 3]) {
                        translation[j * 3] = 0;
                        writeTranslation[j * 3] = true;
                    }
                    if (!writeTranslation[j * 3 + 1]) {
                        translation[j * 3 + 1] = 0;
                        writeTranslation[j * 3 + 1] = true;
                    }
                    if (!writeTranslation[j * 3 + 2]) {
                        translation[j * 3 + 2] = 0;
                        writeTranslation[j * 3 + 2] = true;
                    }
                    translation[j * 3] += (float)values[0];
                    translation[j * 3 + 1] += (float)values[1];
                    translation[j * 3 + 2] += (float)values[2];
                    hasTranslation = true;
                    break;
                }
                case COLLADAFW::AnimationList::POSITION_X: {
                    if (!writeTranslation[j * 3]) {
                        translation[j * 3] = 0;
                        writeTranslation[j * 3] = true;
                    }
                    translation[j * 3] += (float)values[0];
                    hasTranslation = true;
                    break;
                }
                case COLLADAFW::AnimationList::POSITION_Y: {
                    if (!writeTranslation[j * 3 + 1]) {
                        translation[j * 3 + 1] = 0;
                        writeTranslation[j * 3 + 1] = true;
                    }
                    translation[j * 3 + 1] += (float)values[0];
                    hasTranslation = true;
                    break;
                }
                case COLLADAFW::AnimationList::POSITION_Z: {
                    if (!writeTranslation[j * 3 + 2]) {
                        translation[j * 3 + 2] = 0;
                        writeTranslation[j * 3 + 2] = true;
                    }
                    translation[j * 3 + 2] += (float)values[0];
                    hasTranslation = true;
                    break;
                }
                }
            }
        }
        shared_ptr<GLTF::GLTFBufferView> keyFrameBufferView = createBufferViewWithAllocatedBuffer(keyFrames, 0, rootAnimation->getCount(), true);
        rootAnimation->registerBufferView(GLTFAnimation::AnimationType::TIME, keyFrameBufferView);
        if (hasTranslation) {
            for (size_t i = 0; i < numKeyFrames; i++) {
                if (!writeTranslation[i * 3]) {
                    translation[i * 3] = translateX;
                }
                if (!writeTranslation[i * 3 + 1]) {
                    translation[i * 3 + 1] = translateY;
                }
                if (!writeTranslation[i * 3 + 2]) {
                    translation[i * 3 + 2] = translateZ;
                }
            }
            shared_ptr<GLTF::GLTFBufferView> translateBufferView = createBufferViewWithAllocatedBuffer(translation, 0, rootAnimation->getCount() * 3, true);
            rootAnimation->registerBufferView(GLTFAnimation::AnimationType::TRANSLATE, translateBufferView);
        }
        if (hasRotation) {
            shared_ptr<GLTF::GLTFBufferView> rotateBufferView = createBufferViewWithAllocatedBuffer(rotation, 0, rootAnimation->getCount() * 4, true);
            rootAnimation->registerBufferView(GLTFAnimation::AnimationType::ROTATE, rotateBufferView);
        }
        if (hasScale) {
            shared_ptr<GLTF::GLTFBufferView> scaleBufferView = createBufferViewWithAllocatedBuffer(scale, 0, rootAnimation->getCount() * 3, true);
            rootAnimation->registerBufferView(GLTFAnimation::AnimationType::SCALE, scaleBufferView);
        }
        return true;
    }

    //--------------------------------------------------------------------
    bool COLLADA2GLTFWriter::writeSkinControllerData(const COLLADAFW::SkinControllerData* skinControllerData) {
        shared_ptr <GLTFSkin> glTFSkin(new GLTFSkin(skinControllerData->getOriginalId()));
        shared_ptr <GLTFProfile> profile = this->_asset->profile();

        glTFSkin->setString(kName, skinControllerData->getName());

        glTFSkin->setBindShapeMatrix(serializeOpenCOLLADAMatrix4(skinControllerData->getBindShapeMatrix()));

        const COLLADAFW::UIntValuesArray& jointsPerVertex = skinControllerData->getJointsPerVertex();
        const COLLADAFW::IntValuesArray& jointIndices = skinControllerData->getJointIndices();
        const COLLADAFW::UIntValuesArray& weightIndices = skinControllerData->getWeightIndices();
        const COLLADAFW::FloatOrDoubleArray& weights = skinControllerData->getWeights();

        size_t vertexCount = skinControllerData->getVertexCount();
        size_t maxNumberOfInfluencesPerVertex = 0;
        //Pre-pass to figure out what's the maxium number of influences we are dealing with
        for (size_t i = 0; i < vertexCount; i++) {
            if (jointsPerVertex[i] > maxNumberOfInfluencesPerVertex) {
                maxNumberOfInfluencesPerVertex = jointsPerVertex[i];
            }
        }

        size_t index = 0;
        int bucketSize = 4;
        size_t componentSize = sizeof(float);
        size_t skinAttributeSize = componentSize * vertexCount * bucketSize;
        float *weightsPtr = (float*)malloc(skinAttributeSize);
        float *bonesIndices = (float*)malloc(skinAttributeSize);

        memset(weightsPtr, 0, skinAttributeSize);
        memset(bonesIndices, 0, skinAttributeSize);

        for (size_t i = 0; i < vertexCount; i++) {
            size_t pairCount = jointsPerVertex[i];
            if (pairCount == 0)
                continue;

            if (weights.getType() == COLLADAFW::FloatOrDoubleArray::DATA_TYPE_FLOAT) {
                const COLLADAFW::FloatArray* floatWeights = weights.getFloatValues();


                for (size_t j = 0; j < pairCount; ++j, ++index) {
                    if (j < bucketSize) {
                        bonesIndices[(i * bucketSize) + j] = (float)jointIndices[index];
                        weightsPtr[(i * bucketSize) + j] = (*floatWeights)[weightIndices[index]];
                    }
                }
            }
            else if (weights.getType() == COLLADAFW::FloatOrDoubleArray::DATA_TYPE_DOUBLE) {
                //in this case, cast the double to float
                const COLLADAFW::DoubleArray* doubleWeights = weights.getDoubleValues();
                for (size_t j = 0; j < pairCount; ++j, ++index) {
                    if (j < bucketSize) {
                        bonesIndices[(i * bucketSize) + j] = (float)jointIndices[index];
                        weightsPtr[(i * bucketSize) + j] = (float)(*doubleWeights)[weightIndices[index]];
                    }
                }
            }
        }

        //inverse bind matrice
        const Matrix4Array& matrices = skinControllerData->getInverseBindMatrices();
        size_t matricesSize = sizeof(float) * 16 * skinControllerData->getJointsCount();
        float *matricesPtr = (float*)malloc(matricesSize);
        for (size_t i = 0; i < skinControllerData->getJointsCount(); i++) {
            fillFloatPtrFromOpenCOLLADAMatrix4(matrices[i], matricesPtr + (i * 16));
        }
        shared_ptr <GLTFBufferView> inverseBindMatricesView = createBufferViewWithAllocatedBuffer(matricesPtr, 0, matricesSize, true);
        glTFSkin->setInverseBindMatrices(inverseBindMatricesView);

        shared_ptr<GLTFAccessor> inverseBindMatrices(new GLTFAccessor(profile, "FLOAT", "MAT4"));
        inverseBindMatrices->setByteStride(64);
        inverseBindMatrices->setCount((unsigned int)skinControllerData->getJointsCount());
        inverseBindMatrices->setBufferView(inverseBindMatricesView);
        inverseBindMatrices->exposeMinMax();
        glTFSkin->extras()->setValue(kInverseBindMatrices, inverseBindMatrices);

        shared_ptr<GLTFOutputStream> animationOutputStream = this->_asset->createOutputStreamIfNeeded(this->_asset->getSharedBufferId());
        inverseBindMatrices->setUnsignedInt32(kByteOffset, (unsigned int)animationOutputStream->length());
        shared_ptr<GLTFBuffer> buffer = glTFSkin->getInverseBindMatrices()->getBuffer();
        animationOutputStream->write(buffer);

        //
        shared_ptr <GLTFBufferView> weightsView = createBufferViewWithAllocatedBuffer(weightsPtr, 0, skinAttributeSize, true);
        shared_ptr <GLTFAccessor> weightsAttribute(new GLTFAccessor(profile, "FLOAT", GLTFUtils::getTypeForVectorSize(bucketSize)));

        weightsAttribute->setBufferView(weightsView);
        weightsAttribute->setByteStride(componentSize * bucketSize);
        weightsAttribute->setCount(vertexCount);
        weightsAttribute->exposeMinMax();

        glTFSkin->setWeights(weightsAttribute);

        shared_ptr <GLTFBufferView> jointsView = createBufferViewWithAllocatedBuffer(bonesIndices, 0, skinAttributeSize, true);
        shared_ptr <GLTFAccessor> jointsAttribute(new GLTFAccessor(profile, "FLOAT", GLTFUtils::getTypeForVectorSize(bucketSize)));

        jointsAttribute->setBufferView(jointsView);
        jointsAttribute->setByteStride(componentSize * bucketSize);
        jointsAttribute->setCount(vertexCount);
        jointsAttribute->exposeMinMax();

        glTFSkin->setJoints(jointsAttribute);
        glTFSkin->setJointsCount(skinControllerData->getJointsCount());

        shared_ptr<JSONObject> skins = this->_asset->root()->createObjectIfNeeded(kSkins);

        //Also we work around here what looks to be a bug in OpenCOLLADA with a fileId == 0
        COLLADAFW::UniqueId uniqueId = skinControllerData->getUniqueId();

        std::string skinUID = uniqueId.toAscii();
        skins->setValue(skinUID, glTFSkin);

        return true;
    }

    //--------------------------------------------------------------------

    static shared_ptr<GLTFAccessor> __CreateAttributeByApplyingRemapTable(shared_ptr<GLTFAccessor> meshAttribute, size_t vertexCount, unsigned int* remapTableForPositions, shared_ptr<GLTFProfile> profile) {
        unsigned char* sourcePtr = (unsigned char*)meshAttribute->getBufferView()->getBufferDataByApplyingOffset();
        size_t bufferSize = meshAttribute->elementByteLength() * vertexCount;
        unsigned char* destinationPtr = (unsigned char*)malloc(bufferSize);

        shared_ptr <GLTFAccessor> targetAttribute(new GLTFAccessor(profile, meshAttribute->componentType(), meshAttribute->type()));
        targetAttribute->setByteStride(meshAttribute->getByteStride());
        targetAttribute->setCount(vertexCount);

        for (size_t i = 0; i < vertexCount; i++) {
            unsigned int rindex = remapTableForPositions[i];

            void *ptrSrc = sourcePtr + (rindex * meshAttribute->getByteStride());
            void *ptrDst = destinationPtr + (i * targetAttribute->getByteStride());
            memcpy(ptrDst, ptrSrc, meshAttribute->elementByteLength());
        }

        shared_ptr<GLTFBufferView> targetView = createBufferViewWithAllocatedBuffer(destinationPtr, 0, bufferSize, true);
        targetAttribute->setBufferView(targetView);

        return targetAttribute;
    }

    bool COLLADA2GLTFWriter::writeController(const COLLADAFW::Controller* controller) {
        if (controller->getControllerType() == COLLADAFW::Controller::CONTROLLER_TYPE_SKIN) {
            COLLADAFW::SkinController* skinController = (COLLADAFW::SkinController*)controller;

            //Now we get the skin and the mesh, and
            shared_ptr<JSONObject> skins = this->_asset->root()->createObjectIfNeeded(kSkins);

            COLLADAFW::UniqueId uniqueId = skinController->getSkinControllerData().toAscii();

            shared_ptr <GLTFSkin> glTFSkin = static_pointer_cast<GLTFSkin>(skins->getValue(uniqueId.toAscii()));
            shared_ptr<GLTFMesh> mesh = static_pointer_cast<GLTFMesh>(this->_asset->getValueForUniqueId(skinController->getSource().toAscii()));
            this->_asset->setValueForUniqueId(controller->getUniqueId().toAscii(),
                shared_ptr<JSONString>(new JSONString(uniqueId.toAscii())));
            glTFSkin->setSourceUID(skinController->getSource().toAscii());

            unsigned int *remapTableForPositions = mesh->getRemapTableForPositions();
            size_t vertexCount = mesh->getMeshAttribute(GLTF::POSITION, 0)->getCount();
            //Now we remap the bone indices and weight attribute in respect of deindexing we have

            shared_ptr<GLTFAccessor> weightsAttribute = __CreateAttributeByApplyingRemapTable(glTFSkin->getWeights(), vertexCount, remapTableForPositions, this->_asset->profile());

            mesh->setMeshAttribute(GLTF::WEIGHT, 0, weightsAttribute);

            shared_ptr<GLTFAccessor> jointsAttribute = __CreateAttributeByApplyingRemapTable(glTFSkin->getJoints(), vertexCount, remapTableForPositions, this->_asset->profile());

            mesh->setMeshAttribute(GLTF::JOINT, 0, jointsAttribute);

            GLTF::JSONValueVector primitives = mesh->getPrimitives()->values();
            for (size_t i = 0; i < primitives.size(); i++) {
                shared_ptr<GLTFPrimitive> primitive = static_pointer_cast<GLTFPrimitive>(primitives[i]);
                primitive->appendVertexAttribute(shared_ptr <GLTF::JSONVertexAttribute>(new GLTF::JSONVertexAttribute(GLTF::JOINT, 0)));
                primitive->appendVertexAttribute(shared_ptr <GLTF::JSONVertexAttribute>(new GLTF::JSONVertexAttribute(GLTF::WEIGHT, 0)));
            }

            //save the list of joints, first as uniqueIds, and then we will replace with sid's
            shared_ptr <JSONArray> joints(new JSONArray());
            UniqueIdArray& jointsUID = skinController->getJoints();
            for (size_t i = 0; i < jointsUID.getCount(); i++) {
                shared_ptr<JSONString> jointId(new JSONString(jointsUID[i].toAscii()));
                joints->appendValue(jointId);
            }
            glTFSkin->setJointNames(joints);
        }
        return true;
    }

} // namespace COLLADA2JSON

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

#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;
using namespace COLLADAFW;
using namespace COLLADABU;
using namespace COLLADASaxFWL;
using namespace rapidjson;

namespace GLTF
{
    /*
     */
    COLLADA2GLTFWriter::COLLADA2GLTFWriter(shared_ptr<GLTFAsset> asset):
    _asset(asset),
    _visualScene(0),
    _metersPerUnit(1.0) {
	}
    
    /*
     */
	COLLADA2GLTFWriter::~COLLADA2GLTFWriter() {
	}
    
    /*
     */
	void COLLADA2GLTFWriter::reportError( const std::string& method, const std::string& message) {
        printf("ERROR: method:%s message:%s\n", method.c_str(), message.c_str());
	}

    /*
     */
    bool COLLADA2GLTFWriter::write() {
        this->_extraDataHandler = new ExtraDataHandler();
        //To comply with macro to access config
        GLTFAsset *asset = this->_asset.get();
        asset->setExtras(this->_extraDataHandler->allExtras());
        asset->prepareForProfile(shared_ptr <GLTFWebGL_1_0_Profile> (new GLTFWebGL_1_0_Profile()));
                
		COLLADAFW::Root root(&this->_loader, this);
        this->_loader.registerExtraDataCallbackHandler(this->_extraDataHandler);
		if (!root.loadDocument(asset->getInputFilePath()))
			return false;
        
        asset->write();

        // Cleanup IDs and Technique cache in case we have another conversion
        GLTFUtils::resetIDCount();
        clearCommonProfileTechniqueCache();
                
		return true;
	}
    
	//--------------------------------------------------------------------
	void COLLADA2GLTFWriter::cancel( const std::string& errorMessage ) {
        printf("CONVERTION ABORTED: message:%s\n", errorMessage.c_str());
	}
    
	//--------------------------------------------------------------------
	void COLLADA2GLTFWriter::start() {
	}
    
	//--------------------------------------------------------------------
	void COLLADA2GLTFWriter::finish() {
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeGlobalAsset( const COLLADAFW::FileInfo* globalAsset ) {
        GLTFAsset* asset = this->_asset.get();
        shared_ptr<JSONObject> assetObject = asset->root()->createObjectIfNeeded(kAsset);
        std::string version = "collada2gltf@"+std::string(g_GIT_SHA1);
        
        assetObject->setString("generator",version);
        assetObject->setBool(kPremultipliedAlpha, CONFIG_BOOL(asset, kPremultipliedAlpha));
        assetObject->setString(kProfile, asset->profile()->id());
        assetObject->setDouble(kVersion, glTFVersion);

        _metersPerUnit = globalAsset->getUnit().getLinearUnitMeter();

        return true;
	}
    
	//--------------------------------------------------------------------
            
    float COLLADA2GLTFWriter::getTransparency(const COLLADAFW::EffectCommon* effectCommon) {
        GLTFAsset* asset = this->_asset.get();
        //super naive for now, also need to check sketchup work-around
        if (effectCommon->getOpacity().isTexture()) {
            return 1;
        }
        float transparency = static_cast<float>(effectCommon->getOpacity().getColor().getAlpha());
        
        return CONFIG_BOOL(asset, "invertTransparency") ? 1 - transparency : transparency;
    }
    
    float COLLADA2GLTFWriter::isOpaque(const COLLADAFW::EffectCommon* effectCommon) {
        return getTransparency(effectCommon)  >= 1;
    }
    
    void COLLADA2GLTFWriter::registerObjectWithOriginalUID(std::string originalId, shared_ptr <JSONObject> obj, shared_ptr <JSONObject> objLib) {
        if (this->_asset->_originalIDToTrackedObject.count(originalId) == 0) {
            if (!objLib->contains(originalId)) {
                objLib->setValue(originalId, obj);
                this->_asset->_originalIDToTrackedObject[originalId] = obj;
            } else {
                this->_asset->log("WARNING:Object with id:%s is already tracked, failed attempt to add object\n", originalId.c_str());
            }
        } else {
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
        } else {
            materialBindingsMap = mb[nodeUID];
        }
        
        //apply prefix on MeshUID
        std::string prefixedMeshUID = prefix + meshUID;
        shared_ptr <MaterialBindingsPrimitiveMap> materialBindingsPrimitiveMap;
        if (materialBindingsMap->count(prefixedMeshUID) == 0) {
            materialBindingsPrimitiveMap = shared_ptr<MaterialBindingsPrimitiveMap> (new MaterialBindingsPrimitiveMap());
            (*materialBindingsMap)[prefixedMeshUID] = materialBindingsPrimitiveMap;
        } else {
            materialBindingsPrimitiveMap = (*materialBindingsMap)[prefixedMeshUID];
        }
        
        GLTF::JSONValueVector primitives = mesh->getPrimitives()->values();
        for (size_t j = 0 ; j < primitives.size() ; j++) {
            shared_ptr <GLTF::GLTFPrimitive> primitive = static_pointer_cast<GLTFPrimitive>(primitives[j]);
            //FIXME: consider optimizing this with a hashtable, would be better if it was coming that way from OpenCOLLADA
            int materialBindingIndex = -1;
            for (size_t k = 0; k < materialBindings.getCount() ; k++) {
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

    
    bool COLLADA2GLTFWriter::writeNode( const COLLADAFW::Node* node,
                                       shared_ptr <GLTF::JSONObject> nodesObject,
                                       COLLADABU::Math::Matrix4 parentMatrix,
                                       SceneFlatteningInfo* sceneFlatteningInfo) {
        GLTFAsset *asset = this->_asset.get();
        bool shouldExportTRS = CONFIG_BOOL(asset, "alwaysExportTRS");
        const NodePointerArray& nodes = node->getChildNodes();
        std::string nodeOriginalID = node->getOriginalId();
        if (nodeOriginalID.length() == 0) {
            nodeOriginalID = uniqueIdWithType(kNode, node->getUniqueId());
        }
        
        std::string uniqueUID = node->getUniqueId().toAscii();
        
        COLLADABU::Math::Matrix4 matrix = COLLADABU::Math::Matrix4::IDENTITY;
        
        shared_ptr <GLTF::JSONObject> nodeObject(new GLTF::JSONObject());
        nodeObject->setString(kName,node->getName());
        
        this->_asset->_uniqueIDToOpenCOLLADAObject[uniqueUID] = shared_ptr <COLLADAFW::Object> (node->clone());
        this->_asset->setOriginalId(uniqueUID, nodeOriginalID);
        this->_asset->setValueForUniqueId(uniqueUID, nodeObject);
        if (node->getType() == COLLADAFW::Node::JOINT) {
            const string& sid = node->getSid();
            nodeObject->setString("jointId",sid);
        }
        
        bool nodeContainsLookAtTr = false;
        const InstanceCameraPointerArray& instanceCameras = node->getInstanceCameras();
        size_t camerasCount = instanceCameras.getCount();
        if (camerasCount > 0) {
            InstanceCamera* instanceCamera = instanceCameras[0];
            shared_ptr <GLTF::JSONObject> cameraObject(new GLTF::JSONObject());
            
            std::string cameraId = uniqueIdWithType(kCamera, instanceCamera->getInstanciatedObjectId());
            nodeObject->setString(kCamera, cameraId);
            
            //FIXME: just handle the first camera within a node now
            //for (size_t i = 0 ; i < camerasCount ; i++) {
            //}
            //Checks if we have a "look at" transformm because it is not handled by getTransformationMatrix when baking the matrix. (TODO: file a OpenCOLLADA issue for that).
            const TransformationPointerArray& transformations = node->getTransformations();
            size_t transformationsCount = transformations.getCount();
            for (size_t i = 0 ; i < transformationsCount ; i++) {
                const Transformation* tr = transformations[i];
                if (tr->getTransformationType() == Transformation::LOOKAT) {
                    Lookat* lookAt = (Lookat*)tr;
                    buildLookAtMatrix(lookAt, matrix);
                    nodeContainsLookAtTr = true;
                    break;
                }
            }
            
            if (nodeContainsLookAtTr && (transformationsCount > 1)) {
                //FIXME: handle warning/error
                this->_asset->log("WARNING: node contains a look at transform combined with other transforms\n");
            }
        }
        
        if (!nodeContainsLookAtTr) {
            node->getTransformationMatrix(matrix);
        }
                
        float scale[3];
        float translation[3];
        float rotation[4];

        const TransformationPointerArray& transformations = node->getTransformations();
        size_t transformationsCount = transformations.getCount();
        for (size_t i = 0 ; i < transformationsCount ; i++) {
            const Transformation* tr = transformations[i];
            const UniqueId& animationListID = tr->getAnimationList();
            if (!animationListID.isValid())
                continue;
            
            shared_ptr<AnimatedTargets> animatedTargets(new AnimatedTargets());
            
            this->_asset->_uniqueIDToAnimatedTargets[animationListID.toAscii()] = animatedTargets;
            shared_ptr <JSONObject> animatedTarget(new JSONObject());
            std::string animationID = animationListID.toAscii();
            animatedTarget->setString(kTarget, uniqueUID);
            animatedTarget->setString("transformId", animationID);

            if (tr->getTransformationType() == COLLADAFW::Transformation::MATRIX)  {
                animatedTarget->setString("path", "MATRIX");
                animatedTargets->push_back(animatedTarget);
                shouldExportTRS = true;
            }
            
            if (tr->getTransformationType() == COLLADAFW::Transformation::TRANSLATE)  {
                animatedTarget->setString("path", "translation");
                animatedTargets->push_back(animatedTarget);
                shouldExportTRS = true;
            }
            
            if (tr->getTransformationType() == COLLADAFW::Transformation::ROTATE)  {
                animatedTarget->setString("path", "rotation");
                animatedTargets->push_back(animatedTarget);
                shouldExportTRS = true;
            }
          
            if (tr->getTransformationType() == COLLADAFW::Transformation::SCALE)  {
                animatedTarget->setString("path", "scale");
                animatedTargets->push_back(animatedTarget);
                shouldExportTRS = true;
            }
        }
                    
        const COLLADABU::Math::Matrix4 worldMatrix = parentMatrix * matrix;
                
        if (shouldExportTRS) {
            GLTF::decomposeMatrix(matrix, translation, rotation, scale);

            // Scale distance units if we need to
            translation[0] *= _metersPerUnit;
            translation[1] *= _metersPerUnit;
            translation[2] *= _metersPerUnit;
            
            bool exportDefaultValues = CONFIG_BOOL(asset, "exportDefaultValues");
            bool exportTranslation = !(!exportDefaultValues &&
                                       ((translation[0] == 0) && (translation[1] == 0) && (translation[2] == 0)));
            if (exportTranslation)
                nodeObject->setValue("translation", serializeVec3(translation[0], translation[1], translation[2]));
            
            //hum what should be the identity for axis angle ? https://github.com/KhronosGroup/glTF/issues/197
            nodeObject->setValue("rotation", serializeVec4(rotation[0], rotation[1], rotation[2], rotation[3]));

            bool exportScale = !(!exportDefaultValues && ((scale[0] == 1) && (scale[1] == 1) && (scale[2] == 1)));
            if (exportScale)
                nodeObject->setValue("scale", serializeVec3(scale[0], scale[1], scale[2]));
            
        } else {
            //FIXME: OpenCOLLADA typo
            bool exportMatrix = !((matrix.isIdentiy() && (CONFIG_BOOL(asset, "exportDefaultValues") == false) ));
            if (exportMatrix)
                nodeObject->setValue("matrix", serializeOpenCOLLADAMatrix4(matrix));
        }
        
        const InstanceControllerPointerArray& instanceControllers = node->getInstanceControllers();
		unsigned int count = (unsigned int)instanceControllers.getCount();
        if (count > 0) {
            shared_ptr<JSONObject> skins = this->_asset->root()->createObjectIfNeeded(kSkins);
            for (unsigned int i = 0 ; i < count; i++) {
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
                    
                    shared_ptr<JSONObject> instanceSkin(new JSONObject());
                    shared_ptr<JSONArray> skeletons(new JSONArray());
                    
                    for (size_t k = 0 ; k < instanceController->skeletons().size() ; k++) {
                        std::string skeleton = instanceController->skeletons()[k].getFragment();
                        skeletons->appendValue(shared_ptr<JSONString>(new JSONString(skeleton)));
                    }
                    
                    instanceSkin->setValue("skeletons", skeletons);
                    instanceSkin->setString(kSkin, skin->getId());
                    nodeObject->setValue(kInstanceSkin, instanceSkin);
                }
            }
        }
        
        // save mesh
		const InstanceGeometryPointerArray& instanceGeometries = node->getInstanceGeometries();
        
        count = (unsigned int)instanceGeometries.getCount();
        if (count > 0) {
            for (unsigned int i = 0 ; i < count; i++) {
                InstanceGeometry* instanceGeometry = instanceGeometries[i];
                MaterialBindingArray& materialBindings = instanceGeometry->getMaterialBindings();
                COLLADAFW::UniqueId uniqueId = instanceGeometry->getInstanciatedObjectId();
                _storeMaterialBindingArray("meshes-",
                                           node->getUniqueId().toAscii(),
                                           uniqueId.toAscii(),
                                           materialBindings);
            }
        }
        
        shared_ptr <GLTF::JSONArray> childrenArray(new GLTF::JSONArray());
        nodeObject->setValue(kChildren, childrenArray);
        
        count = (unsigned int)nodes.getCount();
        for (unsigned int i = 0 ; i < count ; i++)  {
            std::string childOriginalID = nodes[i]->getOriginalId();
            if (childOriginalID.length() == 0) {
                childOriginalID = uniqueIdWithType(kNode, nodes[i]->getUniqueId());
            }
            childrenArray->appendValue(shared_ptr <GLTF::JSONString> (new GLTF::JSONString(childOriginalID)));
        }
        
        registerObjectWithOriginalUID(nodeOriginalID, nodeObject, nodesObject);
        
        for (unsigned int i = 0 ; i < count ; i++)  {
            this->writeNode(nodes[i], nodesObject, worldMatrix, sceneFlatteningInfo);
        }
        
        const InstanceNodePointerArray& instanceNodes = node->getInstanceNodes();
        count = (unsigned int)instanceNodes.getCount();
        for (unsigned int i = 0 ; i < count ; i++) {
            InstanceNode* instanceNode  = instanceNodes[i];
            std::string id = instanceNode->getInstanciatedObjectId().toAscii();
            shared_ptr<JSONArray> parents;
            if (this->_asset->_uniqueIDToParentsOfInstanceNode.count(id) == 0) {
                parents =  shared_ptr<JSONArray> (new JSONArray());
                this->_asset->_uniqueIDToParentsOfInstanceNode[id] = parents;
            } else {
                parents = this->_asset->_uniqueIDToParentsOfInstanceNode[id];
            }
            
            parents->appendValue(shared_ptr<JSONString> (new JSONString(node->getUniqueId().toAscii())));
        }
        
        shared_ptr <GLTF::JSONArray> lightsInNode(new GLTF::JSONArray());

        const InstanceLightPointerArray& instanceLights = node->getInstanceLights();
        count = (unsigned int)instanceLights.getCount();
        
        //For a given light, keep track of all the nodes holding it
        if (count) {
            shared_ptr<JSONObject> lights = this->_asset->root()->createObjectIfNeeded(kLights);
            for (unsigned int i = 0 ; i < count ; i++) {
                InstanceLight* instanceLight  = instanceLights[i];
                std::string id = instanceLight->getInstanciatedObjectId().toAscii();
                
                shared_ptr<JSONObject> light = static_pointer_cast<JSONObject>(this->_asset->getValueForUniqueId(id));
                if (light) {
                    std::string lightUID = this->_asset->getOriginalId(id);
                    
                    shared_ptr<JSONArray> listOfNodesPerLight;
                    if (this->_asset->_uniqueIDOfLightToNodes.count(id) == 0) {
                        listOfNodesPerLight =  shared_ptr<JSONArray> (new JSONArray());
                        this->_asset->_uniqueIDOfLightToNodes[lightUID] = listOfNodesPerLight;
                    } else {
                        listOfNodesPerLight = this->_asset->_uniqueIDOfLightToNodes[lightUID];
                    }
                    
                    listOfNodesPerLight->appendValue(JSONSTRING(nodeOriginalID));
                    lightsInNode->appendValue(shared_ptr <JSONString> (new JSONString(lightUID)));
                    lights->setValue(lightUID, light);
                }
            }
            //We just want a single light per node
            //https://github.com/KhronosGroup/glTF/issues/13
            //nodeObject->setValue("lights", lightsInNode);
            if (lightsInNode->values().size() > 0) {
                nodeObject->setValue(kLight, lightsInNode->values()[0]);
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
    
    bool COLLADA2GLTFWriter::writeVisualScene( const COLLADAFW::VisualScene* visualScene ) {
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
        shared_ptr <GLTF::JSONArray> childrenArray(new GLTF::JSONArray());
        sceneObject->setValue(kNodes, childrenArray);
        
        for (size_t i = 0 ; i < nodeCount ; i++) {
            std::string nodeUID = nodePointerArray[i]->getOriginalId();
            if (nodeUID.length() == 0) {
                nodeUID = uniqueIdWithType(kNode, nodePointerArray[i]->getUniqueId());
            }
                        
            shared_ptr <GLTF::JSONString> nodeIDValue(new GLTF::JSONString(nodeUID));
            childrenArray->appendValue(static_pointer_cast <GLTF::JSONValue> (nodeIDValue));
        }
                
        for (size_t i = 0 ; i < nodeCount ; i++) {
            //FIXME: &this->_sceneFlatteningInfo
            this->writeNode(nodePointerArray[i], nodesObject, COLLADABU::Math::Matrix4::IDENTITY, NULL);
        }
        
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeScene( const COLLADAFW::Scene* scene ) {
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeLibraryNodes( const COLLADAFW::LibraryNodes* libraryNodes ) {
        const NodePointerArray& nodes = libraryNodes->getNodes();
        
        shared_ptr <GLTF::JSONObject> nodesObject = this->_asset->root()->createObjectIfNeeded(kNodes);
                
        size_t count = nodes.getCount();
        for (size_t i = 0 ; i < count ; i++) {
            const COLLADAFW::Node *node = nodes[i];
            
            std::string id = node->getUniqueId().toAscii();
            if (this->_asset->_uniqueIDToParentsOfInstanceNode.count(id) > 0) {
                shared_ptr<JSONArray> parents = this->_asset->_uniqueIDToParentsOfInstanceNode[id];
                std::vector <shared_ptr <JSONValue> > values = parents->values();
                for (size_t k = 0 ; k < values.size() ; k++) {
                    shared_ptr<JSONString> value = static_pointer_cast<JSONString>(values[k]);

                    shared_ptr<JSONObject> parentNode = static_pointer_cast<JSONObject>(this->_asset->getValueForUniqueId(value->getString()));
                    if (parentNode) {
                        shared_ptr <JSONArray> children = parentNode->createArrayIfNeeded(kChildren);
                        children->appendValue(shared_ptr <JSONString>(new JSONString(node->getOriginalId())));
                    }
                }
            }
            
            if (!this->writeNode(node,  nodesObject, COLLADABU::Math::Matrix4::IDENTITY, 0))
                return false;
        }
        
        return true;
	}
        
	//--------------------------------------------------------------------
    bool COLLADA2GLTFWriter::writeGeometry( const COLLADAFW::Geometry* geometry ) {
        switch (geometry->getType()) {
            case Geometry::GEO_TYPE_MESH:
            {
                const COLLADAFW::Mesh* mesh = (COLLADAFW::Mesh*)geometry;
                std::string meshUID = geometry->getUniqueId().toAscii();
                if (this->_asset->containsValueForUniqueId(meshUID) == false) {
                    shared_ptr<GLTFMesh> cvtMesh = convertOpenCOLLADAMesh((COLLADAFW::Mesh*)mesh, this->_asset.get(), _metersPerUnit);
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
	bool COLLADA2GLTFWriter::writeMaterial( const COLLADAFW::Material* material ) {
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
        std::string samplerHash = GLTFUtils::toString(wrapS)+GLTFUtils::toString(wrapT)+GLTFUtils::toString(minFilter)+GLTFUtils::toString(maxFilter);
        bool addSampler = false;
        size_t index = 0;
        
        if (this->_asset->_samplerHashtoSamplerIndex.count(samplerHash) == 0) {
            index = this->_asset->_samplerHashtoSamplerIndex.size();
			this->_asset->_samplerHashtoSamplerIndex[samplerHash] = (unsigned int)index;
            addSampler = true;
        } else {
            index = this->_asset->_samplerHashtoSamplerIndex[samplerHash];
        }
        
        std::string samplerUID = "sampler_"+GLTFUtils::toString(index);
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
        
        slotObject->setString("value", textureUID);
        values->setValue(slotName, slotObject);
    }
    
    void COLLADA2GLTFWriter::handleEffectSlot(const COLLADAFW::EffectCommon* commonProfile,
                                              std::string slotName,
                                              shared_ptr <GLTFEffect> cvtEffect,
                                              shared_ptr <JSONObject> extras) {
        shared_ptr <JSONObject> values = cvtEffect->getValues();
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
            } else {
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
                    for (size_t i = 0 ; i < samplers.getCount() ; i++) {
                        if (samplers[i]->getSid() == texture) {
                            Sampler* sampler = (Sampler*)samplers[i];
                            _installTextureSlot(sampler, slotName, texcoord, this->_asset, cvtEffect);
                        }
                    }
                }
            }
        } else
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
            values->setValue(slotName, slotObject);
            
        } else if (slot.isTexture()) {
            const Texture&  texture = slot.getTexture();
            const SamplerPointerArray& samplers = commonProfile->getSamplerPointerArray();
            Sampler* sampler = (Sampler*)samplers[texture.getSamplerId()];
            std::string texcoord = texture.getTexcoord();
            _installTextureSlot(sampler, slotName, texcoord, this->_asset, cvtEffect);
        } else {
            // nothing to do, no texture or color
        }
    }
        
    bool COLLADA2GLTFWriter::writeEffect( const COLLADAFW::Effect* effect ) {
        GLTFAsset *asset = this->_asset.get();
        GLTFProfile* profile = asset->profile().get();
        const COLLADAFW::CommonEffectPointerArray& commonEffects = effect->getCommonEffects();
        
        if (commonEffects.getCount() > 0) {
            std::string uniqueId = "";
#if EXPORT_MATERIALS_AS_EFFECTS
            uniqueId += "material.";
#else
            uniqueId += "effect.";
#endif
            uniqueId += GLTF::GLTFUtils::toString(effect->getUniqueId().getObjectId());;
            
            shared_ptr <GLTFEffect> cvtEffect(new GLTFEffect(effect->getOriginalId()));
            shared_ptr <JSONObject> values(new JSONObject());
            
            cvtEffect->setValues(values);
            
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
            
            handleEffectSlot(effectCommon,"diffuse" , cvtEffect, extras);
            handleEffectSlot(effectCommon,"ambient" , cvtEffect, extras);
            handleEffectSlot(effectCommon,"emission" , cvtEffect, extras);
            handleEffectSlot(effectCommon,"specular" , cvtEffect, extras);
            handleEffectSlot(effectCommon,"reflective" , cvtEffect, extras);
            handleEffectSlot(effectCommon,"bump" , cvtEffect, extras);
            
            if (CONFIG_BOOL(asset, "alwaysExportFilterColor")) {
                shared_ptr <JSONObject> slotObject(new JSONObject());
                slotObject->setValue("value", serializeVec4(1, 1, 1, 1));
                slotObject->setUnsignedInt32(kType, profile->getGLenumForString("FLOAT_VEC4"));
                values->setValue("filterColor", slotObject);
            }
            
            if (!isOpaque(effectCommon) || CONFIG_BOOL(asset, "alwaysExportTransparency")) {
                shared_ptr <JSONObject> transparency(new JSONObject());
                transparency->setDouble("value", this->getTransparency(effectCommon));
                transparency->setUnsignedInt32(kType, profile->getGLenumForString("FLOAT"));
                values->setValue("transparency", transparency);
            }
            
            //should check if has specular first and the lighting model (if not lambert)
            double shininess = effectCommon->getShininess().getFloatValue();
            if (shininess >= 0) {
                if (shininess < 1) {
                    shininess *= 128.0;
                }
                shared_ptr <JSONObject> shininessObject(new JSONObject());
                shininessObject->setUnsignedInt32(kType, profile->getGLenumForString("FLOAT"));
                shininessObject->setDouble("value", shininess);
                values->setValue("shininess", shininessObject);
            }
            
            shared_ptr<JSONObject> materials = this->_asset->root()->createObjectIfNeeded(kMaterials);
            materials->setValue(cvtEffect->getID(), cvtEffect);
            this->_asset->setValueForUniqueId(effect->getUniqueId().toAscii(), cvtEffect);
            
        }
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeCamera( const COLLADAFW::Camera* camera ) {
        shared_ptr <GLTF::JSONObject> camerasObject = static_pointer_cast <GLTF::JSONObject> (this->_asset->root()->getValue("cameras"));
        if (!camerasObject) {
            camerasObject = shared_ptr <GLTF::JSONObject> (new GLTF::JSONObject());
            this->_asset->root()->setValue("cameras", camerasObject);
        }
        
        shared_ptr <GLTF::JSONObject> cameraObject(new GLTF::JSONObject());
        shared_ptr <GLTF::JSONObject> projectionObject(new GLTF::JSONObject());

        std::string id = uniqueIdWithType(kCamera, camera->getUniqueId());
        
        camerasObject->setValue(id, cameraObject);
        
        switch (camera->getCameraType()) {
            case Camera::UNDEFINED_CAMERATYPE:
                this->_asset->log("WARNING: unknown camera type: using perspective\n");
                break;
            case Camera::ORTHOGRAPHIC:
            {
                cameraObject->setString(kType, "orthographic");
                cameraObject->setValue("orthographic", projectionObject);
                switch (camera->getDescriptionType()) {
                    case Camera::UNDEFINED: //!< The perspective camera object is invalid
                        //FIXME: handle error
                        break;
                    case Camera::SINGLE_X: //!< Only xfov or xmag, respectively describes the camera
                        projectionObject->setDouble("xmag", camera->getXMag().getValue());
                        break;
                    case Camera::SINGLE_Y: //!< Only yfov or ymag, respectively describes the camera
                        projectionObject->setDouble("ymag", camera->getYMag().getValue());
                        break;
                    case Camera::X_AND_Y: //!< xfov and yfov or xmag and ymag, respectively describe the camera
                        projectionObject->setDouble("xmag", camera->getXMag().getValue());
                        projectionObject->setDouble("ymag", camera->getYMag().getValue());
                        break;
                    case Camera::ASPECTRATIO_AND_X: //!< aspect ratio and xfov or xmag, respectively describe the camera
                        projectionObject->setDouble("xmag", camera->getXMag().getValue());
                        projectionObject->setDouble("aspect_ratio", camera->getAspectRatio().getValue());
                        break;
                    case Camera::ASPECTRATIO_AND_Y: //!< aspect ratio and yfov or <mag, respectivelydescribe the camera
                        projectionObject->setDouble("ymag", camera->getYMag().getValue());
                        projectionObject->setDouble("aspect_ratio", camera->getAspectRatio().getValue());
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
                        //FIXME: handle error
                        break;
                    case Camera::SINGLE_X: //!< Only xfov or xmag, respectively describes the camera
                        projectionObject->setDouble("xfov", camera->getXFov().getValue());
                        break;
                    case Camera::SINGLE_Y: //!< Only yfov or ymag, respectively describes the camera
                        projectionObject->setDouble("yfov", camera->getYFov().getValue());
                        break;
                    case Camera::X_AND_Y: //!< xfov and yfov or xmag and ymag, respectively describe the camera
                        projectionObject->setDouble("xfov", camera->getXFov().getValue());
                        projectionObject->setDouble("yfov", camera->getYFov().getValue());
                        break;
                    case Camera::ASPECTRATIO_AND_X: //!< aspect ratio and xfov or xmag, respectively describe the camera
                        projectionObject->setDouble("xfov", camera->getXFov().getValue());
                        projectionObject->setDouble("aspect_ratio", camera->getAspectRatio().getValue());
                        break;
                    case Camera::ASPECTRATIO_AND_Y: //!< aspect ratio and yfov or <mag, respectivelydescribe the camera
                        projectionObject->setDouble("yfov", camera->getYFov().getValue());
                        projectionObject->setDouble("aspect_ratio", camera->getAspectRatio().getValue());
                        break;
                }
                
            }
                break;
        }
        
        projectionObject->setDouble("znear", camera->getNearClippingPlane().getValue());
        projectionObject->setDouble("zfar", camera->getFarClippingPlane().getValue());
        
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeImage( const COLLADAFW::Image* openCOLLADAImage ) {
        shared_ptr <GLTF::JSONObject> images = this->_asset->root()->createObjectIfNeeded(kImages);
        shared_ptr <GLTF::JSONObject> image(new GLTF::JSONObject());
        
        std::string imageUID = openCOLLADAImage->getUniqueId().toAscii();
        this->_asset->setValueForUniqueId(imageUID, image);
        this->_asset->setOriginalId(imageUID, openCOLLADAImage->getOriginalId());
        images->setValue(openCOLLADAImage->getOriginalId(), image);
        
        const COLLADABU::URI &imageURI = openCOLLADAImage->getImageURI();
        std::string relPathFile = imageURI.getPathFile();
		if (imageURI.getPathDir().substr(0, 2) != "./") {
			relPathFile = imageURI.getPathDir() + imageURI.getPathFile();
		}
		else {
			relPathFile = imageURI.getPathDir().substr(2) + imageURI.getPathFile();
		}
        
		if (_asset->getEmbedResources())
		{
			COLLADABU::URI inputURI(_asset->getInputFilePath().c_str());
			std::string imageFullPath = inputURI.getPathDir() + relPathFile;

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

				image->setString("uri", create_dataUri(ss.str(), contentType));

				return true;
			}
		}
		
        image->setString("uri", COLLADABU::URI::uriEncode(this->_asset->resourceOuputPathForPath(relPathFile)));
        
        return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeLight( const COLLADAFW::Light* light ) {
        //FIXME: add projection
        shared_ptr <JSONObject> glTFLight(new JSONObject());
        shared_ptr <JSONObject> description(new JSONObject());

        COLLADAFW::Light::LightType lightType = light->getLightType();
		Color color = light->getColor();

        float constantAttenuation = (float)light->getConstantAttenuation().getValue();
        float linearAttenuation = (float)light->getLinearAttenuation().getValue();
        float quadraticAttenuation = (float)light->getQuadraticAttenuation().getValue();

        shared_ptr <JSONValue> lightColor = serializeVec3(color.getRed(), color.getGreen(), color.getBlue());
        
        switch (lightType) {
            case COLLADAFW::Light::AMBIENT_LIGHT:
                glTFLight->setString(kType, "ambient");
                break;
            case COLLADAFW::Light::DIRECTIONAL_LIGHT:
                glTFLight->setString(kType, "directional");
                break;
            case COLLADAFW::Light::POINT_LIGHT: {
                glTFLight->setString(kType, "point");

                description->setValue("constantAttenuation", shared_ptr <JSONNumber> (new JSONNumber(constantAttenuation)));
                description->setValue("linearAttenuation", shared_ptr <JSONNumber> (new JSONNumber(linearAttenuation)));
                description->setValue("quadraticAttenuation", shared_ptr <JSONNumber> (new JSONNumber(quadraticAttenuation)));
            }
                break;
            case COLLADAFW::Light::SPOT_LIGHT: {
                glTFLight->setString(kType, "spot");

                float fallOffAngle =  (float)light->getFallOffAngle().getValue();
                float fallOffExponent = (float)light->getFallOffExponent().getValue();
                
                description->setValue("constantAttenuation", shared_ptr <JSONNumber> (new JSONNumber(constantAttenuation)));
                description->setValue("linearAttenuation", shared_ptr <JSONNumber> (new JSONNumber(linearAttenuation)));
                description->setValue("quadraticAttenuation", shared_ptr <JSONNumber> (new JSONNumber(quadraticAttenuation)));
                
                description->setValue("fallOffAngle", shared_ptr <JSONNumber> (new JSONNumber(fallOffAngle)));
                description->setValue("fallOffExponent", shared_ptr <JSONNumber> (new JSONNumber(fallOffExponent)));
            }
                break;
            default:
                return false;
        }
        
        description->setValue("color", lightColor);
        glTFLight->setValue(glTFLight->getString(kType), description);
        
        const std::string &lightId = light->getUniqueId().toAscii();
        this->_asset->setValueForUniqueId(lightId, glTFLight);
        this->_asset->setOriginalId(lightId, light->getOriginalId());
        
        shared_ptr<JSONArray> lightsIds = this->_asset->root()->createArrayIfNeeded("lightsIds");
        lightsIds->appendValue(shared_ptr<JSONString>(new JSONString(light->getOriginalId())));
        
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeAnimation( const COLLADAFW::Animation* animation) {
        shared_ptr <GLTFAnimation> cvtAnimation = convertOpenCOLLADAAnimationToGLTFAnimation(animation, this->_asset.get(), _metersPerUnit);
        
        cvtAnimation->setOriginalID(animation->getOriginalId());
        
        if (this->_asset->_flattenerMapsForAnimationID.count(animation->getOriginalId()) == 0) {
            this->_asset->_flattenerMapsForAnimationID[animation->getOriginalId()] = shared_ptr <AnimationFlattenerForTargetUID> (new AnimationFlattenerForTargetUID());
        }
        
        shared_ptr<JSONObject> animations = this->_asset->root()->createObjectIfNeeded("animations");
        animations->setValue(animation->getUniqueId().toAscii(), cvtAnimation);
        
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeAnimationList( const COLLADAFW::AnimationList* animationList ) {
        const COLLADAFW::AnimationList::AnimationBindings &animationBindings = animationList->getAnimationBindings();
        
        shared_ptr<JSONObject> animations = this->_asset->root()->createObjectIfNeeded("animations");
        AnimatedTargetsSharedPtr animatedTargets = this->_asset->_uniqueIDToAnimatedTargets[animationList->getUniqueId().toAscii()];
        
        for (size_t i = 0 ; i < animationBindings.getCount() ; i++) {
            const COLLADAFW::AnimationList::AnimationClass animationClass = animationBindings[i].animationClass;

            shared_ptr <GLTFAnimation> cvtAnimation = static_pointer_cast<GLTFAnimation>(animations->getObject(animationBindings[i].animation.toAscii()));

            AnimationFlattenerForTargetUIDSharedPtr animationFlattenerMap = this->_asset->_flattenerMapsForAnimationID[cvtAnimation->getOriginalID()];
            for (size_t j = 0 ; j < animatedTargets->size() ; j++) {
                shared_ptr<JSONObject> animatedTarget = (*animatedTargets)[j];
                shared_ptr<GLTFAnimationFlattener> animationFlattener;
                std::string targetUID = animatedTarget->getString(kTarget);
                if (animationFlattenerMap->count(targetUID) == 0) {
                    //FIXME: assuming node here is wrong
                    COLLADAFW::Node *node = (COLLADAFW::Node*)this->_asset->_uniqueIDToOpenCOLLADAObject[targetUID].get();
                    animationFlattener = shared_ptr<GLTFAnimationFlattener> (new GLTFAnimationFlattener(node));
                    (*animationFlattenerMap)[targetUID] = animationFlattener;
                }
            }

            cvtAnimation->registerAnimationFlatteners(animationFlattenerMap);
            
            if (!GLTF::writeAnimation(cvtAnimation, animationClass, animatedTargets, this->_asset.get())) {
                //if an animation failed to convert, we don't want to keep track of it.
                animations->removeValue(animationBindings[i].animation.toAscii());
            }
        }
        
		return true;
	}
    
	//--------------------------------------------------------------------
	bool COLLADA2GLTFWriter::writeSkinControllerData( const COLLADAFW::SkinControllerData* skinControllerData ) {
        shared_ptr <GLTFSkin> glTFSkin(new GLTFSkin(skinControllerData->getOriginalId()));
        shared_ptr <GLTFProfile> profile = this->_asset->profile();
        
        glTFSkin->setBindShapeMatrix(serializeOpenCOLLADAMatrix4(skinControllerData->getBindShapeMatrix()));
        
		const COLLADAFW::UIntValuesArray& jointsPerVertex = skinControllerData->getJointsPerVertex();
		const COLLADAFW::IntValuesArray& jointIndices = skinControllerData->getJointIndices();
		const COLLADAFW::UIntValuesArray& weightIndices = skinControllerData->getWeightIndices();
		const COLLADAFW::FloatOrDoubleArray& weights = skinControllerData->getWeights();

        size_t vertexCount = skinControllerData->getVertexCount();
        size_t maxNumberOfInfluencesPerVertex = 0;
        //Pre-pass to figure out what's the maxium number of influences we are dealing with
        for (size_t i = 0 ; i < vertexCount ; i++) {
            if (jointsPerVertex[i] > maxNumberOfInfluencesPerVertex) {
                maxNumberOfInfluencesPerVertex = jointsPerVertex[i];
            }
        }
        
        size_t index = 0;
        size_t bucketSize = 4;
        size_t componentSize = sizeof(float);
        size_t skinAttributeSize = componentSize * vertexCount * bucketSize;
        float *weightsPtr = (float*)malloc(skinAttributeSize);
        float *bonesIndices = (float*)malloc(skinAttributeSize);

        memset(weightsPtr, 0, skinAttributeSize);
        memset(bonesIndices, 0, skinAttributeSize);
        
        for (size_t i = 0 ; i < vertexCount ; i++) {
            size_t pairCount = jointsPerVertex[i];
			if ( pairCount == 0 )
				continue;
            
			if ( weights.getType() == COLLADAFW::FloatOrDoubleArray::DATA_TYPE_FLOAT ) {
				const COLLADAFW::FloatArray* floatWeights = weights.getFloatValues();
				
                
                for (size_t j = 0; j < pairCount; ++j, ++index) {
                    if (j < bucketSize) {
                        bonesIndices[(i * bucketSize) + j] = (float)jointIndices[index];
                        weightsPtr[(i * bucketSize) + j] = (*floatWeights)[weightIndices[index]];
                    }
				}
			}
			else if ( weights.getType() == COLLADAFW::FloatOrDoubleArray::DATA_TYPE_DOUBLE ) {
				//in this case, cast the double to float
                const COLLADAFW::DoubleArray* doubleWeights = weights.getDoubleValues();
				for (size_t j = 0; j < pairCount; ++j, ++index) {
                    if (j < bucketSize) {
                        bonesIndices[(i * bucketSize) + j] = (float)jointIndices[index];
                        weightsPtr[(i * bucketSize) + j]  = (float)(*doubleWeights)[weightIndices[index]];
                    }
				}
			}
        }

        //inverse bind matrice
        const Matrix4Array& matrices = skinControllerData->getInverseBindMatrices();
        size_t matricesSize = sizeof(float) * 16 * skinControllerData->getJointsCount();
        float *matricesPtr = (float*)malloc(matricesSize);
        for (size_t i = 0 ; i < skinControllerData->getJointsCount() ; i++) {
            fillFloatPtrFromOpenCOLLADAMatrix4(matrices[i], matricesPtr + (i*16));
        }
        shared_ptr <GLTFBufferView> inverseBindMatricesView = createBufferViewWithAllocatedBuffer(matricesPtr, 0, matricesSize, true);
        glTFSkin->setInverseBindMatrices(inverseBindMatricesView);
        
        shared_ptr<JSONObject> inverseBindMatrices(new JSONObject());
        inverseBindMatrices->setUnsignedInt32(kType, profile->getGLenumForString("FLOAT_MAT4"));
		inverseBindMatrices->setUnsignedInt32(kCount, (unsigned int)skinControllerData->getJointsCount());
        inverseBindMatrices->setUnsignedInt32(kByteOffset, 0);
        glTFSkin->extras()->setValue(kInverseBindMatrices, inverseBindMatrices);
        
        shared_ptr<GLTFOutputStream> animationOutputStream = this->_asset->createOutputStreamIfNeeded(this->_asset->getSharedBufferId());
		inverseBindMatrices->setUnsignedInt32(kByteOffset, (unsigned int)animationOutputStream->length());
        shared_ptr<GLTFBuffer> buffer = glTFSkin->getInverseBindMatrices()->getBuffer();
        animationOutputStream->write(buffer);

        //
        shared_ptr <GLTFBufferView> weightsView = createBufferViewWithAllocatedBuffer(weightsPtr, 0, skinAttributeSize, true);
        shared_ptr <GLTFAccessor> weightsAttribute(new GLTFAccessor(profile, profile->getGLTypeForComponentType(GLTF::FLOAT, bucketSize)));
        
        weightsAttribute->setBufferView(weightsView);
        weightsAttribute->setByteStride(componentSize * bucketSize);
        weightsAttribute->setCount(vertexCount);

        glTFSkin->setWeights(weightsAttribute);
        
        shared_ptr <GLTFBufferView> jointsView = createBufferViewWithAllocatedBuffer(bonesIndices, 0, skinAttributeSize, true);
        shared_ptr <GLTFAccessor> jointsAttribute(new GLTFAccessor(profile, profile->getGLTypeForComponentType(GLTF::FLOAT, bucketSize)));
        
        jointsAttribute->setBufferView(jointsView);
        jointsAttribute->setByteStride(componentSize * bucketSize);
        jointsAttribute->setCount(vertexCount);

        glTFSkin->setJoints(jointsAttribute);
        glTFSkin->setJointsCount(skinControllerData->getJointsCount());
        
        shared_ptr<JSONObject> skins = this->_asset->root()->createObjectIfNeeded(kSkins);
         
        //Also we work around here what looks to be a bug in OpenCOLLADA with a fileId == 0
        COLLADAFW::UniqueId uniqueId = skinControllerData->getUniqueId();
        
        std::string skinUID = uniqueId.toAscii();
        skins->setValue(skinUID , glTFSkin);
        
		return true;
	}
    
	//--------------------------------------------------------------------
    
    static shared_ptr<GLTFAccessor> __CreateAttributeByApplyingRemapTable(shared_ptr<GLTFAccessor> meshAttribute, size_t vertexCount, unsigned int* remapTableForPositions, shared_ptr<GLTFProfile> profile) {
        unsigned char* sourcePtr = (unsigned char*)meshAttribute->getBufferView()->getBufferDataByApplyingOffset();
        size_t bufferSize = meshAttribute->elementByteLength() * vertexCount;
        unsigned char* destinationPtr = (unsigned char*)malloc(bufferSize);

        shared_ptr <GLTFAccessor> targetAttribute(new GLTFAccessor(profile, profile->getGLTypeForComponentType(meshAttribute->componentType(), meshAttribute->componentsPerElement())));
        targetAttribute->setByteStride(meshAttribute->getByteStride());
        targetAttribute->setCount(vertexCount);

        for (size_t i = 0 ; i < vertexCount  ; i++) {
            unsigned int rindex = remapTableForPositions[i];
            
            void *ptrSrc = sourcePtr + (rindex * meshAttribute->getByteStride());
            void *ptrDst = destinationPtr + (i * targetAttribute->getByteStride());
            memcpy(ptrDst, ptrSrc , meshAttribute->elementByteLength());
        }
        
        shared_ptr<GLTFBufferView> targetView = createBufferViewWithAllocatedBuffer(destinationPtr, 0, bufferSize, true);
        targetAttribute->setBufferView(targetView);
        
        return targetAttribute;
    }
    
	bool COLLADA2GLTFWriter::writeController( const COLLADAFW::Controller* controller ) {
        if (controller->getControllerType() == COLLADAFW::Controller::CONTROLLER_TYPE_SKIN) {
            COLLADAFW::SkinController* skinController = (COLLADAFW::SkinController*)controller;
            
            //Now we get the skin and the mesh, and
            shared_ptr<JSONObject> skins = this->_asset->root()->createObjectIfNeeded(kSkins);
            
            COLLADAFW::UniqueId uniqueId = skinController->getSkinControllerData().toAscii();

            shared_ptr <GLTFSkin> glTFSkin = static_pointer_cast<GLTFSkin>(skins->getValue(uniqueId.toAscii()));
            shared_ptr<GLTFMesh> mesh = static_pointer_cast<GLTFMesh>(this->_asset->getValueForUniqueId(skinController->getSource().toAscii()));
            this->_asset->setValueForUniqueId(controller->getUniqueId().toAscii(),
                                              shared_ptr<JSONString> (new JSONString(uniqueId.toAscii())));
            glTFSkin->setSourceUID(skinController->getSource().toAscii());
            
            unsigned int *remapTableForPositions = mesh->getRemapTableForPositions();
            size_t vertexCount = mesh->getMeshAttribute(GLTF::POSITION, 0)->getCount();
            //Now we remap the bone indices and weight attribute in respect of deindexing we have
            
            shared_ptr<GLTFAccessor> weightsAttribute = __CreateAttributeByApplyingRemapTable(glTFSkin->getWeights(), vertexCount, remapTableForPositions, this->_asset->profile());
            
            mesh->setMeshAttribute(GLTF::WEIGHT, 0, weightsAttribute);
            
            shared_ptr<GLTFAccessor> jointsAttribute = __CreateAttributeByApplyingRemapTable(glTFSkin->getJoints(), vertexCount, remapTableForPositions, this->_asset->profile());
            
            mesh->setMeshAttribute(GLTF::JOINT, 0, jointsAttribute);
            
            GLTF::JSONValueVector primitives = mesh->getPrimitives()->values();
            for (size_t i = 0 ; i < primitives.size() ; i++) {
                shared_ptr<GLTFPrimitive> primitive = static_pointer_cast<GLTFPrimitive>(primitives[i]);
                primitive->appendVertexAttribute(shared_ptr <GLTF::JSONVertexAttribute>( new GLTF::JSONVertexAttribute(GLTF::JOINT,0)));
                primitive->appendVertexAttribute(shared_ptr <GLTF::JSONVertexAttribute>( new GLTF::JSONVertexAttribute(GLTF::WEIGHT,0)));
            }
            
            //save the list of joints, first as uniqueIds, and then we will replace with sid's
            shared_ptr <JSONArray> joints(new JSONArray());
            UniqueIdArray& jointsUID = skinController->getJoints();
            for (size_t i = 0 ; i < jointsUID.getCount() ; i++) {
                shared_ptr<JSONString> jointId(new JSONString(jointsUID[i].toAscii()));
                joints->appendValue(jointId);
            }
            glTFSkin->setJointsIds(joints);
        }
		return true;
	}
    
} // namespace COLLADA2JSON

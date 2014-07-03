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

#ifndef __COLLADA2JSONWRITER_H__
#define __COLLADA2JSONWRITER_H__

#include "GLTF.h"
#include "COLLADA2GLTFExport.h"
#include "GLTFOpenCOLLADA.h"
#include "GLTFAsset.h"

#include "helpers/geometryHelpers.h"
#include "helpers/mathHelpers.h"
#include "convert/animationConverter.h"
#include "convert/meshConverter.h"

#ifdef WIN32
#pragma warning(disable: 4275)
#endif

namespace GLTF
{
    class GLTFObject;
    class ExtraDataHandler;
        
    // -- SceneFlattening
    
    class MeshFlatteningInfo
    {
    public:
        MeshFlatteningInfo(unsigned int meshUID, const COLLADABU::Math::Matrix4& worldMatrix) :
        _worldMatrix(worldMatrix),
        _meshUID(meshUID) {}
        
        unsigned int getUID() { return this->_meshUID; }
        const COLLADABU::Math::Matrix4& getWorldMatrix() { return this->_worldMatrix; }
        
    private:
        COLLADABU::Math::Matrix4 _worldMatrix;
        unsigned int _meshUID;
    };
    
    
    typedef std::vector < std::shared_ptr <MeshFlatteningInfo> > MeshFlatteningInfoVector;
    
    typedef struct 
    {
        BBOX sceneBBOX;
        MeshFlatteningInfoVector allMeshes;
    } SceneFlatteningInfo;
    
    //-- OpenCOLLADA -> JSON writer implementation
    
	class COLLADA2GLTF_EXPORT COLLADA2GLTFWriter : public COLLADAFW::IWriter
	{
	public:        
		COLLADA2GLTFWriter(std::shared_ptr<GLTF::GLTFAsset> asset);
		virtual ~COLLADA2GLTFWriter();
    private:
		static void reportError(const std::string& method, const std::string& message);
        bool writeNode(const COLLADAFW::Node* node, std::shared_ptr <GLTF::JSONObject> nodesObject, COLLADABU::Math::Matrix4, SceneFlatteningInfo*);
        bool processSceneFlatteningInfo(SceneFlatteningInfo* sceneFlatteningInfo);
        float getTransparency(const COLLADAFW::EffectCommon* effectCommon);
        float isOpaque(const COLLADAFW::EffectCommon* effectCommon);
        bool writeMeshFromUIDWithMaterialBindings(COLLADAFW::UniqueId uniqueId,
                                                  COLLADAFW::MaterialBindingArray &materialBindings,
                                                  std::shared_ptr <GLTF::JSONArray> &meshesArray);
        
	public:        
        
		bool write();
        
		/** Deletes the entire scene.
         @param errorMessage A message containing informations about the error that occurred.
         */
		void cancel(const std::string& errorMessage);;
        
		/** Prepare to receive data.*/
		void start();;
        
		/** Remove all objects that don't have an object. Deletes unused visual scenes.*/
		void finish();;
        
		/** When this method is called, the writer must write the global document asset->
         @return The writer should return true, if writing succeeded, false otherwise.*/
		virtual bool writeGlobalAsset ( const COLLADAFW::FileInfo* asset );
        
		/** Writes the entire visual scene.
         @return True on succeeded, false otherwise.*/
		virtual bool writeVisualScene ( const COLLADAFW::VisualScene* visualScene );
        
		/** Writes the scene.
         @return True on succeeded, false otherwise.*/
		virtual bool writeScene ( const COLLADAFW::Scene* scene );
        
		/** Handles all nodes in the library nodes.
         @return True on succeeded, false otherwise.*/
		virtual bool writeLibraryNodes( const COLLADAFW::LibraryNodes* libraryNodes );
        
		/** Writes the geometry.
         @return True on succeeded, false otherwise.*/
		virtual bool writeGeometry ( const COLLADAFW::Geometry* geometry );
        
		/** Writes the material.
         @return True on succeeded, false otherwise.*/
		virtual bool writeMaterial( const COLLADAFW::Material* material );
        
		/** Writes the effect.
         @return True on succeeded, false otherwise.*/
		virtual bool writeEffect( const COLLADAFW::Effect* effect );
        
		/** Writes the camera.
         @return True on succeeded, false otherwise.*/
		virtual bool writeCamera( const COLLADAFW::Camera* camera );
        
		/** Writes the image.
         @return True on succeeded, false otherwise.*/
		virtual bool writeImage( const COLLADAFW::Image* image );
        
		/** Writes the light.
         @return True on succeeded, false otherwise.*/
		virtual bool writeLight( const COLLADAFW::Light* light );
        
		/** Writes the animation.
         @return True on succeeded, false otherwise.*/
		virtual bool writeAnimation( const COLLADAFW::Animation* animation );
        
		/** Writes the animation.
         @return True on succeeded, false otherwise.*/
		virtual bool writeAnimationList( const COLLADAFW::AnimationList* animationList );
        
		/** Writes the skin controller data.
         @return True on succeeded, false otherwise.*/
		virtual bool writeSkinControllerData( const COLLADAFW::SkinControllerData* skinControllerData );
        
		/** Writes the controller.
         @return True on succeeded, false otherwise.*/
		virtual bool writeController( const COLLADAFW::Controller* Controller );
        
		/** When this method is called, the writer must write the formulas. All the formulas of the entire
         COLLADA file are contained in @a formulas.
         @return The writer should return true, if writing succeeded, false otherwise.*/
		virtual bool writeFormulas( const COLLADAFW::Formulas* formulas ){return true;}
        
		/** When this method is called, the writer must write the kinematics scene. 
         @return The writer should return true, if writing succeeded, false otherwise.*/
		virtual bool writeKinematicsScene( const COLLADAFW::KinematicsScene* kinematicsScene ){return true;};
        
	private:
        void _installTextureSlot(COLLADAFW::Sampler* sampler,
                                 const std::string& slotName,
                                 const std::string& texcoord,
                                 std::shared_ptr <GLTF::GLTFAsset> asset,
                                 std::shared_ptr<GLTF::GLTFEffect> cvtEffect);
        
        void _storeMaterialBindingArray(const std::string& prefix,
                                        const std::string& nodeUID,
                                        const std::string& meshUID,
                                        COLLADAFW::MaterialBindingArray &materialBindings);
        void registerObjectWithOriginalUID(std::string nodeUID, std::shared_ptr <JSONObject> nodeObject, std::shared_ptr <JSONObject> nodesObject);
        bool writeData(std::string filename, unsigned char* data, size_t length);
        void handleEffectSlot(const COLLADAFW::EffectCommon* commonProfile,
                              std::string slotName,
                              std::shared_ptr <GLTFEffect> cvtEffect,
                              std::shared_ptr<JSONObject> extras);
        
        std::string getSamplerUIDForParameters(unsigned int wrapS, unsigned int wrapT, unsigned int minFilter, unsigned int maxFilter);
        
	private:
        COLLADASaxFWL::Loader _loader;
        std::shared_ptr<GLTF::GLTFAsset> _asset;
        const COLLADAFW::VisualScene *_visualScene;
        SceneFlatteningInfo _sceneFlatteningInfo;
        GLTF::ExtraDataHandler *_extraDataHandler;
        std::ofstream _compressedDataOutputStream;
	};
} 

#endif 

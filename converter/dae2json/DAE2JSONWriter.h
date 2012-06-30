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

#ifndef __DAE2JSONWRITER_H__
#define __DAE2JSONWRITER_H__

#include "JSONExport.h"
#include "COLLADASaxFWLLoader.h"

#include "COLLADABUStableHeaders.h"
#include "COLLADABUNativeString.h"
#include "COLLADABUStringUtils.h"


#include "COLLADAFWRoot.h"
#include "COLLADAFWGeometry.h"
#include "COLLADAFWMesh.h"
#include "COLLADAFWMeshVertexData.h"
#include "COLLADAFWNode.h"
#include "COLLADAFWVisualScene.h"
#include "COLLADAFWInstanceGeometry.h"

#include "COLLADAFWIWriter.h"
#include "COLLADAFWUniqueId.h"
#include "COLLADAFWVisualScene.h"
#include "COLLADAFWLibraryNodes.h"
#include "COLLADAFWMaterial.h"
#include "COLLADAFWEffect.h"
#include "COLLADAFWImage.h"
#include "COLLADABUURI.h"
#include "Math/COLLADABUMathMatrix4.h"

#include "prettywriter.h"	
#include "filestream.h"

#include <stack>
#include <list>
#include <map>
#include <set>
#include <string>

#include <iostream>

#include <vector>
#include <tr1/memory>
#include <tr1/unordered_map>

using namespace COLLADAFW;
using namespace COLLADABU;
using namespace COLLADASaxFWL;
using namespace rapidjson;

using namespace std::tr1;
using namespace std;


namespace DAE2JSON
{
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, shared_ptr<JSONExport::JSONMesh> > UniqueIDToMesh;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, unsigned int /* effectID */ > MaterialUIDToEffectUID;
    typedef std::map<unsigned int /* openCOLLADA uniqueID */, shared_ptr<JSONExport::JSONEffect> > UniqueIDToEffect;

	class DAE2JSONWriter : public COLLADAFW::IWriter
	{
	public:        
		DAE2JSONWriter( const COLLADABU::URI& inputFile, PrettyWriter <FileStream> *jsonWriter );
		virtual ~DAE2JSONWriter();

		static void reportError(const String& method, const String& message);
        bool writeNode(const COLLADAFW::Node* node, shared_ptr <JSONExport::JSONObject> nodesObject);
        shared_ptr <JSONExport::JSONArray> serializeMatrix4Array  (const COLLADABU::Math::Matrix4 &matrix);
        
	public:        
        
		bool write();
        
		/** Deletes the entire scene.
         @param errorMessage A message containing informations about the error that occurred.
         */
		void cancel(const String& errorMessage);;
        
		/** Prepare to receive data.*/
		void start();;
        
		/** Remove all objects that don't have an object. Deletes unused visual scenes.*/
		void finish();;
        
		/** When this method is called, the writer must write the global document asset.
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
        
	protected:
        
        bool writeData(String filename, unsigned char* data, size_t length);
        
	private:
        COLLADABU::URI _inputFile;
        const COLLADAFW::VisualScene* _visualScene;
        UniqueIDToMesh _uniqueIDToMesh;
        UniqueIDToEffect _uniqueIDToEffect;
        MaterialUIDToEffectUID _materialUIDToEffectUID;
        JSONExport::JSONWriter _writer;
        shared_ptr <JSONExport::JSONObject> _rootJSONObject;
        ofstream _fileOutputStream;
	};
} 

#endif 

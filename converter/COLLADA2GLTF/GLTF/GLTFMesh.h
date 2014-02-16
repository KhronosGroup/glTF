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

#ifndef __GLTFMESH_H__
#define __GLTFMESH_H__

namespace GLTF 
{
    class GLTFMesh;
    
    shared_ptr <GLTFMesh> createUnifiedIndexesMeshFromMesh(GLTFMesh *sourceMesh, std::vector< shared_ptr<IndicesVector> > &vectorOfIndicesVector, shared_ptr<GLTFProfile> profile);
    
    typedef std::map<unsigned int /* IndexSet */, shared_ptr<GLTF::GLTFAccessor> > IndexSetToMeshAttributeHashmap;
    typedef std::map<GLTF::Semantic , IndexSetToMeshAttributeHashmap > SemanticToMeshAttributeHashmap;
    
    class GLTFMesh {
    public:
        GLTFMesh();
        GLTFMesh(const GLTFMesh &mesh);
        
        virtual ~GLTFMesh();
        
        shared_ptr <MeshAttributeVector> meshAttributes();
        
        bool appendPrimitive(shared_ptr <GLTF::GLTFPrimitive> primitive);
        
        bool hasSemantic(Semantic semantic);

        void setMeshAttributesForSemantic(GLTF::Semantic semantic, IndexSetToMeshAttributeHashmap& indexSetToMeshAttributeHashmap);
        IndexSetToMeshAttributeHashmap& getMeshAttributesForSemantic(Semantic semantic); 
        
        void setMeshAttribute(Semantic semantic, size_t indexOfSet, shared_ptr<GLTFAccessor> meshAttribute);
        shared_ptr<GLTFAccessor> getMeshAttribute(Semantic semantic, size_t indexOfSet);
        
        size_t getMeshAttributesCountForSemantic(Semantic semantic);
        
        std::vector <GLTF::Semantic> allSemantics();
                
        std::string getID();
        void setID(std::string ID);
        
        std::string getName();
        void setName(std::string name);
        
        shared_ptr<JSONObject> getExtensions();

        void setRemapTableForPositions(unsigned int* remapTableForPositions);
        unsigned int* getRemapTableForPositions();

        PrimitiveVector const getPrimitives();

        bool writeAllBuffers(std::ofstream& verticesOutputStream, std::ofstream& indicesOutputStream, std::ofstream& genericStream);
        
#ifdef USE_WEBGLLOADER
        void setCompressedBuffer(shared_ptr<GLTFBuffer> compressedBuffer);
        shared_ptr<GLTFBuffer> getCompressedBuffer();
#endif
        
    private:
        PrimitiveVector _primitives;
        SemanticToMeshAttributeHashmap _semanticToMeshAttributes;
        std::string _ID, _name;

        shared_ptr<JSONObject> _extensions;
        
        //WEBGLLOADER
        shared_ptr<GLTFBuffer> _compressedBuffer;
        
        //This is unfortunate that we need to keep this information,
        //but since we get skinning weights and bone indices after the mesh and the openCOLLADA mesh is not available anymore, we need to keep
        //the remap table to build the weights and bone indices as mesh attributes.
        unsigned int *_remapTableForPositions;
    };    

}


#endif
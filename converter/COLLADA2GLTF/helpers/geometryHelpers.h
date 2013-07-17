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

#ifndef __GEOMETRY_HELPERS__
#define __GEOMETRY_HELPERS__

namespace GLTF
{
    std::string keyWithSemanticAndSet(GLTF::Semantic semantic, unsigned int indexSet);

    shared_ptr <GLTFMesh> createUnifiedIndexesMeshFromMesh(GLTFMesh *sourceMesh, std::vector< shared_ptr<IndicesVector> > &vectorOfIndicesVector);
    
    bool createMeshesWithMaximumIndicesCountFromMeshIfNeeded(GLTFMesh *sourceMesh, unsigned int maxiumIndicesCount, MeshVector &meshes);
    
    unsigned int* createTrianglesFromPolylist(unsigned int *verticesCount /* array containing the count for each array of indices per face */,
                                              unsigned int *polylist /* array containing the indices of a face */,
                                              unsigned int count /* count of entries within the verticesCount array */,
                                              unsigned int *triangulatedIndicesCount /* number of indices in returned array */);

    //helper only required to facilitate compression as Open3DGC and WebGLLoader do not allow (yet) to share the same vertex buffer with multiple index buffer
    bool createMeshesFromMeshPrimitives(GLTFMesh *sourceMesh, MeshVector &meshes);

}

#endif
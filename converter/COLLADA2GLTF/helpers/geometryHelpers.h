#ifndef __GEOMETRY_HELPERS__
#define __GEOMETRY_HELPERS__

namespace GLTF
{
    std::string _KeyWithSemanticAndSet(GLTF::Semantic semantic, unsigned int indexSet);

    shared_ptr <GLTFMesh> CreateUnifiedIndexesMeshFromMesh(GLTFMesh *sourceMesh, std::vector< shared_ptr<IndicesVector> > &vectorOfIndicesVector);
    
    bool CreateMeshesWithMaximumIndicesCountFromMeshIfNeeded(GLTFMesh *sourceMesh, unsigned int maxiumIndicesCount, MeshVector &meshes);
    
    unsigned int* createTrianglesFromPolylist(unsigned int *verticesCount /* array containing the count for each array of indices per face */,
                                              unsigned int *polylist /* array containing the indices of a face */,
                                              unsigned int count /* count of entries within the verticesCount array */,
                                              unsigned int *triangulatedIndicesCount /* number of indices in returned array */);

}

#endif
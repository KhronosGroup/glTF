/*
*/

#ifndef __GLTFMESHCONVERTER_H__
#define __GLTFMESHCONVERTER_H__

namespace GLTF
{
    shared_ptr <GLTFMesh> convertOpenCOLLADAMesh(COLLADAFW::Mesh* openCOLLADAMesh, GLTFConverterContext& context);
    shared_ptr <GLTFBufferView> convertIntArrayToGLTFBufferView(const COLLADAFW::IntValuesArray &array);
    shared_ptr <GLTFBufferView> convertUnsignedIntArrayToGLTFBufferView(const COLLADAFW::UIntValuesArray &array);
    shared_ptr <GLTFBufferView> convertFloatOrDoubleArrayToGLTFBufferView(const COLLADAFW::FloatOrDoubleArray &floatOrDoubleArray);
    bool writeAllMeshBuffers(shared_ptr <GLTFMesh> mesh, std::ofstream& verticesOutputStream, std::ofstream& indicesOutputStream, std::ofstream& genericStream, const GLTFConverterContext& context);

}


#endif


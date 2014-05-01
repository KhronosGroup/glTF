/*
*/

#ifndef __GLTFMESHCONVERTER_H__
#define __GLTFMESHCONVERTER_H__

namespace GLTF
{
    std::shared_ptr <GLTFMesh> convertOpenCOLLADAMesh(COLLADAFW::Mesh* openCOLLADAMesh, GLTFAsset* asset);
    std::shared_ptr <GLTFBufferView> convertIntArrayToGLTFBufferView(const COLLADAFW::IntValuesArray &array);
    std::shared_ptr <GLTFBufferView> convertUnsignedIntArrayToGLTFBufferView(const COLLADAFW::UIntValuesArray &array);
    std::shared_ptr <GLTFBufferView> convertFloatOrDoubleArrayToGLTFBufferView(const COLLADAFW::FloatOrDoubleArray &floatOrDoubleArray);
}


#endif


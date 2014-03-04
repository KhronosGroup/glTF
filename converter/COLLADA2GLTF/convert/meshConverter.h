/*
*/

#ifndef __GLTFMESHCONVERTER_H__
#define __GLTFMESHCONVERTER_H__

namespace GLTF
{
    shared_ptr <GLTFMesh> convertOpenCOLLADAMesh(COLLADAFW::Mesh* openCOLLADAMesh, GLTFAsset& context);
    shared_ptr <GLTFBufferView> convertIntArrayToGLTFBufferView(const COLLADAFW::IntValuesArray &array);
    shared_ptr <GLTFBufferView> convertUnsignedIntArrayToGLTFBufferView(const COLLADAFW::UIntValuesArray &array);
    shared_ptr <GLTFBufferView> convertFloatOrDoubleArrayToGLTFBufferView(const COLLADAFW::FloatOrDoubleArray &floatOrDoubleArray);
}


#endif


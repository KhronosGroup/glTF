#include "GLTF.h"
#include "../GLTFOpenCOLLADA.h"
#include "GLTFAsset.h"

#include "meshConverter.h"
#include "../helpers/mathHelpers.h"
#include "../helpers/geometryHelpers.h"

//--- X3DGC
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#ifdef USE_OPEN3DGC
#include <math.h>

#include "../extensions/o3dgc-compression/GLTF-Open3DGC.h"

#endif
//--- X3DGC

using namespace rapidjson;
#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;


namespace GLTF
{
    /*
     Convert an OpenCOLLADA's FloatOrDoubleArray type to a GLTFBufferView
     Note: the resulting GLTFBufferView is not typed, it's the call responsability to keep track of the type if needed.
     */
    shared_ptr <GLTFBufferView> convertFloatOrDoubleArrayToGLTFBufferView(const COLLADAFW::FloatOrDoubleArray &floatOrDoubleArray) {
        unsigned char* sourceData = 0;
        size_t sourceSize = 0;
        
        switch (floatOrDoubleArray.getType()) {
            case COLLADAFW::MeshVertexData::DATA_TYPE_FLOAT: {
                const COLLADAFW::FloatArray* array = floatOrDoubleArray.getFloatValues();
                
                sourceData = (unsigned char*)array->getData();
                sourceSize = array->getCount() * sizeof(float);
            }
                break;
            case COLLADAFW::MeshVertexData::DATA_TYPE_DOUBLE: {
                const COLLADAFW::DoubleArray* array = floatOrDoubleArray.getDoubleValues();
                
                sourceData = (unsigned char*)array->getData();
                sourceSize = array->getCount() * sizeof(double);
            }
                break;
            default:
            case COLLADAFW::MeshVertexData::DATA_TYPE_UNKNOWN:
                //FIXME report error
                break;
        }
        unsigned char* copiedData = (unsigned char*)malloc(sourceSize);
        memcpy(copiedData, sourceData, sourceSize);
        
        shared_ptr <GLTF::GLTFBufferView> bufferView = createBufferViewWithAllocatedBuffer(copiedData, 0, sourceSize, true);
        
        return bufferView;
    }
    
    shared_ptr <GLTFBufferView> convertUnsignedIntArrayToGLTFBufferView(const COLLADAFW::UIntValuesArray &array) {
        unsigned char* sourceData = (unsigned char*)array.getData();
        size_t sourceSize = array.getCount() * sizeof(unsigned int);
        unsigned char* copiedData = (unsigned char*)malloc(sourceSize);
        memcpy(copiedData, sourceData, sourceSize);
        shared_ptr <GLTF::GLTFBufferView> bufferView = createBufferViewWithAllocatedBuffer(copiedData, 0, sourceSize, true);
        
        return bufferView;
    }
    
    //FIXME: these 3 functions up there could use some refactoring
    shared_ptr <GLTFBufferView> convertIntArrayToGLTFBufferView(const COLLADAFW::IntValuesArray &array) {
        unsigned char* sourceData = (unsigned char*)array.getData();
        size_t sourceSize = array.getCount() * sizeof(int);
        unsigned char* copiedData = (unsigned char*)malloc(sourceSize);
        memcpy(copiedData, sourceData, sourceSize);
        shared_ptr <GLTF::GLTFBufferView> bufferView = createBufferViewWithAllocatedBuffer(copiedData, 0, sourceSize, true);
        
        return bufferView;
    }
    
    
    static unsigned int __ConvertOpenCOLLADAMeshVertexDataToGLTFAccessors(const COLLADAFW::MeshVertexData &vertexData,
                                                                          GLTFMesh* mesh,
                                                                          GLTF::Semantic semantic,
                                                                          size_t allowedComponentsPerAttribute,
                                                                          shared_ptr<GLTFProfile> profile)
    {
        // The following are OpenCOLLADA fmk issues preventing doing a totally generic processing of sources
        //1. "set"(s) other than texCoord don't have valid input infos
        //2. not the original id in the source
        
        std::string id;
        size_t length, elementsCount;
        size_t stride = 0;
        size_t componentsPerElement = 0;
        size_t byteOffset = 0;
        size_t inputLength = 0;
        
        size_t setCount = vertexData.getNumInputInfos();
        bool unpatchedOpenCOLLADA = (setCount == 0); // reliable heuristic to know if the input have not been set
        
        if (unpatchedOpenCOLLADA)
            setCount = 1;
        
        for (size_t indexOfSet = 0 ; indexOfSet < setCount ; indexOfSet++) {
            bool meshAttributeOwnsBuffer = false;

            if (!unpatchedOpenCOLLADA) {
                id = vertexData.getName(indexOfSet);
                componentsPerElement = vertexData.getStride(indexOfSet);
                inputLength = vertexData.getLength(indexOfSet);
            } else {
                // for unpatched version of OpenCOLLADA we need this work-around.
                id = GLTF::GLTFUtils::generateIDForType("buffer").c_str();
                componentsPerElement = 3; //only normal and positions should reach this code
                inputLength = vertexData.getLength(0);
            }
            
            length = inputLength ? inputLength : vertexData.getValuesCount();
            elementsCount = length / componentsPerElement;
            unsigned char *sourceData = 0;
            size_t sourceSize = 0;
            
            GLTF::ComponentType componentType = GLTF::NOT_AN_ELEMENT_TYPE;
            switch (vertexData.getType()) {
                case COLLADAFW::MeshVertexData::DATA_TYPE_FLOAT: {
                    componentType = GLTF::FLOAT;
                    stride = sizeof(float) * componentsPerElement;
                    const COLLADAFW::FloatArray* array = vertexData.getFloatValues();
                    
                    sourceData = (unsigned char*)array->getData() + byteOffset;
                    sourceSize = length * sizeof(float);

                     byteOffset += sourceSize; //Doh! - OpenCOLLADA store all sets contiguously in the same array
                }
                    break;
                case COLLADAFW::MeshVertexData::DATA_TYPE_DOUBLE: {
                    //FIXME: handle this
                    /*
                     sourceType = DOUBLE;
                     
                     const DoubleArray& array = vertexData.getDoubleValues()[indexOfSet];
                     const size_t count = array.getCount();
                     sourceData = (void*)array.getData();
                     sourceSize = length * sizeof(double);
                     */
                    // Warning if can't make "safe" conversion
                }
                    
                    break;
                default:
                case COLLADAFW::MeshVertexData::DATA_TYPE_UNKNOWN:
                    //FIXME report error
                    break;
            }
            
            //FIXME: this is assuming float
            if (allowedComponentsPerAttribute != componentsPerElement) {
                sourceSize = elementsCount * sizeof(float) * allowedComponentsPerAttribute;
                
                float *adjustedSource = (float*)malloc(sourceSize);
                float *originalSource = (float*)sourceData;
                size_t adjustedStride = sizeof(float) * allowedComponentsPerAttribute;
                
                if (allowedComponentsPerAttribute < componentsPerElement) {
                    for (size_t i = 0 ; i < elementsCount ; i++) {
                        for (size_t j= 0 ; j < allowedComponentsPerAttribute ; j++) {
                            adjustedSource[(i*allowedComponentsPerAttribute) + j] = originalSource[(i*componentsPerElement) + j];
                        }
                    }
                } else {
                    //FIXME: unlikely but should be taken care of
                }

                //Free source before replacing it
                if (meshAttributeOwnsBuffer) {
                    free(sourceData);
                }
                
                componentsPerElement = allowedComponentsPerAttribute;
                meshAttributeOwnsBuffer = true;
                sourceData = (unsigned char*)adjustedSource;
                stride = adjustedStride;
            }
            
            // FIXME: the source could be shared, store / retrieve it here
            shared_ptr <GLTFBufferView> cvtBufferView = createBufferViewWithAllocatedBuffer(id, sourceData, 0, sourceSize, meshAttributeOwnsBuffer);
            shared_ptr <GLTFAccessor> cvtMeshAttribute(new GLTFAccessor(profile, profile->getGLTypeForComponentType(componentType, componentsPerElement)));
            
            cvtMeshAttribute->setBufferView(cvtBufferView);
            cvtMeshAttribute->setByteStride(stride);
            cvtMeshAttribute->setCount(elementsCount);
            
            mesh->setMeshAttribute(semantic, indexOfSet, cvtMeshAttribute);
        }
        
        return (unsigned int)setCount;
    }
    
    static void __AppendIndices(shared_ptr <GLTF::GLTFPrimitive> &primitive, IndicesVector &primitiveIndicesVector, shared_ptr <GLTF::GLTFAccessor> &indices, GLTF::Semantic semantic, unsigned int indexOfSet)
    {
        primitive->appendVertexAttribute(shared_ptr <GLTF::JSONVertexAttribute>( new GLTF::JSONVertexAttribute(semantic,indexOfSet)));
        primitiveIndicesVector.push_back(indices);
    }
    
    static void __HandleIndexList(unsigned int idx,
                                  COLLADAFW::IndexList *indexList,
                                  Semantic semantic,
                                  bool shouldTriangulate,
                                  unsigned int count,
                                  unsigned int vcount,
                                  unsigned int *verticesCountArray,
                                  shared_ptr <GLTF::GLTFPrimitive> cvtPrimitive,
                                  IndicesVector &primitiveIndicesVector,
                                  shared_ptr<GLTFProfile> profile)
    {
        unsigned int triangulatedIndicesCount = 0;
        bool ownData = false;
        unsigned int *indices = indexList->getIndices().getData();
        
        if (shouldTriangulate) {
            indices = createTrianglesFromPolylist(verticesCountArray, indices, vcount, &triangulatedIndicesCount);
            count = triangulatedIndicesCount;
            ownData = true;
        }
        
        //Why is OpenCOLLADA doing this ? why adding an offset the indices ??
        //We need to offset it backward here.
		unsigned int initialIndex = (unsigned int)indexList->getInitialIndex();
        if (initialIndex != 0) {
            unsigned int *bufferDestination = 0;
            if (!ownData) {
                bufferDestination = (unsigned int*)malloc(sizeof(unsigned int) * count);
                ownData = true;
            } else {
                bufferDestination = indices;
            }
            for (size_t idx = 0 ; idx < count ; idx++) {
                bufferDestination[idx] = indices[idx] - initialIndex;
            }
            indices = bufferDestination;
        }
        
        shared_ptr <GLTF::GLTFBufferView> uvBuffer = createBufferViewWithAllocatedBuffer(indices, 0, count * sizeof(unsigned int), ownData);
        shared_ptr <GLTFAccessor> accessor(new GLTFAccessor(profile, profile->getGLenumForString("UNSIGNED_SHORT")));
        
        accessor->setBufferView(uvBuffer);
        accessor->setCount(count);
        
        __AppendIndices(cvtPrimitive, primitiveIndicesVector, accessor, semantic, idx);
    }
    
    static shared_ptr <GLTF::GLTFPrimitive> __ConvertOpenCOLLADAMeshPrimitive(COLLADAFW::MeshPrimitive *openCOLLADAMeshPrimitive,
                                                                              IndicesVector &primitiveIndicesVector,
                                                                              shared_ptr<GLTFProfile> profile)
    {
        shared_ptr <GLTF::GLTFPrimitive> cvtPrimitive(new GLTF::GLTFPrimitive());
        
        // We want to match OpenGL/ES mode , as WebGL spec points to OpenGL/ES spec...
        // "Symbolic constants GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, and GL_TRIANGLES are accepted."
        std::string type;
        bool shouldTriangulate = false;
        
        switch(openCOLLADAMeshPrimitive->getPrimitiveType()) {
                //these 2 requires transforms
            case COLLADAFW::MeshPrimitive::POLYLIST:
            case COLLADAFW::MeshPrimitive::POLYGONS:
                // FIXME: perform conversion, but until not done report error
                //these mode are supported by WebGL
                shouldTriangulate = true;
                //force triangles
                type = "TRIANGLES";
                break;
            case  COLLADAFW::MeshPrimitive::LINES:
                type = "LINES";
                break;
            case  COLLADAFW::MeshPrimitive::LINE_STRIPS:
                type = "LINE_STRIP";
                break;
                
            case  COLLADAFW::MeshPrimitive::TRIANGLES:
                type = "TRIANGLES";
                break;
                
            case  COLLADAFW::MeshPrimitive::TRIANGLE_FANS:
                type = "TRIANGLE_FANS";
                break;
                
            case  COLLADAFW::MeshPrimitive::TRIANGLE_STRIPS:
                type = "TRIANGLE_STRIPS";
                break;
                
            case  COLLADAFW::MeshPrimitive::POINTS:
                type = "POINTS";
                break;
            default:
                break;
        }
        
        cvtPrimitive->setMaterialObjectID((unsigned int)openCOLLADAMeshPrimitive->getMaterialId());
        cvtPrimitive->setPrimitive(profile->getGLenumForString(type));
        
        //count of indices , it must be the same for all kind of indices
        size_t count = openCOLLADAMeshPrimitive->getPositionIndices().getCount();
        
        //vertex
        //IndexList &positionIndexList = openCOLLADAMeshPrimitive->getPositionIndices();
        unsigned int *indices = openCOLLADAMeshPrimitive->getPositionIndices().getData();
        unsigned int *verticesCountArray = 0;
        unsigned int vcount = 0;   //count of elements in the array containing the count of indices per polygon & polylist.
        
        if (shouldTriangulate) {
            unsigned int triangulatedIndicesCount = 0;
            //We have to upcast to polygon to retrieve the array of vertexCount
            //OpenCOLLADA use polylist as polygon.
            COLLADAFW::Polygons *polygon = (COLLADAFW::Polygons*)openCOLLADAMeshPrimitive;
            const COLLADAFW::Polygons::VertexCountArray& vertexCountArray = polygon->getGroupedVerticesVertexCountArray();
            vcount = (unsigned int)vertexCountArray.getCount();
            verticesCountArray = (unsigned int*)malloc(sizeof(unsigned int) * vcount);
            for (size_t i = 0; i < vcount; i++) {
                verticesCountArray[i] = polygon->getGroupedVerticesVertexCount(i);;
            }
            indices = createTrianglesFromPolylist(verticesCountArray, indices, vcount, &triangulatedIndicesCount);
            count = triangulatedIndicesCount;
        }
        
        shared_ptr <GLTFBufferView> positionBuffer = createBufferViewWithAllocatedBuffer(indices, 0, count * sizeof(unsigned int), shouldTriangulate ? true : false);
        
        shared_ptr <GLTF::GLTFAccessor> positionIndices(new GLTF::GLTFAccessor(profile, profile->getGLenumForString("UNSIGNED_SHORT")));
        
        positionIndices->setBufferView(positionBuffer);
        positionIndices->setCount(count);
        
        __AppendIndices(cvtPrimitive, primitiveIndicesVector, positionIndices, POSITION, 0);
        
        if (openCOLLADAMeshPrimitive->hasNormalIndices()) {
            unsigned int triangulatedIndicesCount = 0;
            indices = openCOLLADAMeshPrimitive->getNormalIndices().getData();
            if (shouldTriangulate) {
                indices = createTrianglesFromPolylist(verticesCountArray, indices, vcount, &triangulatedIndicesCount);
                count = triangulatedIndicesCount;
            }
            
            shared_ptr <GLTF::GLTFBufferView> normalBuffer = createBufferViewWithAllocatedBuffer(indices, 0, count * sizeof(unsigned int), shouldTriangulate ? true : false);
            shared_ptr <GLTF::GLTFAccessor> normalIndices(new GLTF::GLTFAccessor(profile, profile->getGLenumForString("UNSIGNED_SHORT")));
            
            normalIndices->setBufferView(normalBuffer);
            normalIndices->setCount(count);
            
            __AppendIndices(cvtPrimitive, primitiveIndicesVector, normalIndices, NORMAL, 0);
        }
        
        if (openCOLLADAMeshPrimitive->hasColorIndices()) {
            COLLADAFW::IndexListArray& colorListArray = openCOLLADAMeshPrimitive->getColorIndicesArray();
            for (size_t i = 0 ; i < colorListArray.getCount() ; i++) {
                COLLADAFW::IndexList* indexList = openCOLLADAMeshPrimitive->getColorIndices(i);
				__HandleIndexList((unsigned int)i,
                                  indexList,
                                  GLTF::COLOR,
                                  shouldTriangulate,
								  (unsigned int)count,
                                  vcount,
                                  verticesCountArray,
                                  cvtPrimitive,
                                  primitiveIndicesVector,
                                  profile);
            }
        }
        
        if (openCOLLADAMeshPrimitive->hasUVCoordIndices()) {
            COLLADAFW::IndexListArray& uvListArray = openCOLLADAMeshPrimitive->getUVCoordIndicesArray();
            for (size_t i = 0 ; i < uvListArray.getCount() ; i++) {
                COLLADAFW::IndexList* indexList = openCOLLADAMeshPrimitive->getUVCoordIndices(i);
				__HandleIndexList((unsigned int)i,
                                  indexList,
                                  GLTF::TEXCOORD,
                                  shouldTriangulate,
								  (unsigned int)count,
                                  vcount,
                                  verticesCountArray,
                                  cvtPrimitive,
                                  primitiveIndicesVector,
                                  profile);
            }
        }
        
        if (verticesCountArray) {
            free(verticesCountArray);
        }
        
        return cvtPrimitive;
    }
    
    shared_ptr<GLTFMesh> convertOpenCOLLADAMesh(COLLADAFW::Mesh* openCOLLADAMesh, GLTFAsset* asset) {
        shared_ptr <GLTF::GLTFMesh> cvtMesh(new GLTF::GLTFMesh());
        
        cvtMesh->setID(openCOLLADAMesh->getOriginalId());
        cvtMesh->setName(openCOLLADAMesh->getName());
        
        const COLLADAFW::MeshPrimitiveArray& primitives =  openCOLLADAMesh->getMeshPrimitives();
        size_t primitiveCount = primitives.getCount();
        
        std::vector< shared_ptr<IndicesVector> > allPrimitiveIndicesVectors;
                
        // get all primitives
        for (size_t i = 0 ; i < primitiveCount ; i++) {
            
            const COLLADAFW::MeshPrimitive::PrimitiveType primitiveType = primitives[i]->getPrimitiveType();
            if ((primitiveType != COLLADAFW::MeshPrimitive::TRIANGLES) &&
                //(primitiveType != COLLADAFW::MeshPrimitive::TRIANGLE_STRIPS) &&
                (primitiveType != COLLADAFW::MeshPrimitive::POLYLIST) &&
                (primitiveType != COLLADAFW::MeshPrimitive::POLYGONS)) {
                
                
                static bool printedOnce = false;
                if (!printedOnce) {
                    if (asset->converterConfig()->boolForKeyPath("verboseLogging")) {
                        asset->log("WARNING: some primitives failed to convert\nCurrently supported are TRIANGLES, POLYLIST and POLYGONS\nMore: https://github.com/KhronosGroup/glTF/issues/129\nand https://github.com/KhronosGroup/glTF/issues/135\n");
                        printedOnce = true;
                    }
                }
                
                continue;
            }
            
            shared_ptr <GLTF::IndicesVector> primitiveIndicesVector(new GLTF::IndicesVector());
            allPrimitiveIndicesVectors.push_back(primitiveIndicesVector);
            
            shared_ptr <GLTF::GLTFPrimitive> primitive = __ConvertOpenCOLLADAMeshPrimitive(primitives[i],*primitiveIndicesVector, asset->profile());
            cvtMesh->appendPrimitive(primitive);
            
            VertexAttributeVector vertexAttributes = primitive->getVertexAttributes();
            primitiveIndicesVector = allPrimitiveIndicesVectors[allPrimitiveIndicesVectors.size()-1];
            
            // once we got a primitive, keep track of its meshAttributes
            std::vector< shared_ptr<GLTF::GLTFAccessor> > allIndices = *primitiveIndicesVector;
            for (size_t k = 0 ; k < allIndices.size() ; k++) {
                shared_ptr<GLTF::GLTFAccessor> indices = allIndices[k];
                GLTF::Semantic semantic = vertexAttributes[k]->getSemantic();
                
                switch (semantic) {
                    case GLTF::POSITION:
                        __ConvertOpenCOLLADAMeshVertexDataToGLTFAccessors(openCOLLADAMesh->getPositions(), cvtMesh.get(), GLTF::POSITION, 3, asset->profile());
                        break;
                        
                    case GLTF::NORMAL:
                        __ConvertOpenCOLLADAMeshVertexDataToGLTFAccessors(openCOLLADAMesh->getNormals(), cvtMesh.get(), GLTF::NORMAL, 3, asset->profile());
                        break;
                        
                    case GLTF::TEXCOORD:
                        __ConvertOpenCOLLADAMeshVertexDataToGLTFAccessors(openCOLLADAMesh->getUVCoords(), cvtMesh.get(), GLTF::TEXCOORD, 2, asset->profile());
                        break;
                        
                    case GLTF::COLOR:
                        __ConvertOpenCOLLADAMeshVertexDataToGLTFAccessors(openCOLLADAMesh->getColors(), cvtMesh.get(), GLTF::COLOR, 4, asset->profile());
                        break;
                        
                    default:
                        break;
                }
            }
        }
                
        if (cvtMesh->getPrimitivesCount() > 0) {
            //After this point cvtMesh should be referenced anymore and will be deallocated
            return createUnifiedIndexesMeshFromMesh(cvtMesh.get(), allPrimitiveIndicesVectors, asset->profile());
        }

        return shared_ptr <GLTF::GLTFMesh> ((GLTFMesh*)0);
    }
}

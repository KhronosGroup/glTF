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

#include "GLTF.h"
#include <stdio.h>
#include <stdlib.h>
#include "geometryHelpers.h"

using namespace rapidjson;
#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;

namespace GLTF
{
    unsigned int* createTrianglesFromPolylist(unsigned int *verticesCount /* array containing the count for each array of indices per face */,
                                              unsigned int *polylist /* array containing the indices of a face */,
                                              unsigned int count /* count of entries within the verticesCount array */,
                                              unsigned int *triangulatedIndicesCount /* number of indices in returned array */) {
        //destination buffer size
        unsigned int indicesCount = 0;
        for (unsigned int i = 0 ; i < count ; i++) {
            indicesCount += (verticesCount[i] - 2) * 3;
        }

        if (triangulatedIndicesCount) {
            *triangulatedIndicesCount = indicesCount;
        }

        unsigned int *triangleIndices = (unsigned int*)malloc(sizeof(unsigned int) * indicesCount);
        unsigned int offsetDestination = 0;
        unsigned int offsetSource = 0;
        
        for (unsigned int i = 0 ; i < count ; i++) {
            unsigned int trianglesCount = verticesCount[i] - 2;
            
            unsigned int firstIndex = polylist[0];
            offsetSource = 1;
            
            for (unsigned k = 0 ; k < trianglesCount ; k++) {
                triangleIndices[offsetDestination] = firstIndex;
                triangleIndices[offsetDestination + 1] = polylist[offsetSource];
                triangleIndices[offsetDestination + 2] = polylist[offsetSource + 1];
                //printf("%d %d %d\n",triangleIndices[offsetDestination], triangleIndices[offsetDestination +1], triangleIndices[offsetDestination +2]);
                offsetSource += 1;
                offsetDestination += 3;
            }
            offsetSource += 1;
            
            polylist += verticesCount[i];
        }
        
        
        return triangleIndices;
    }
    
    std::string keyWithSemanticAndSet(GLTF::Semantic semantic, unsigned int indexSet)
    {
        std::string semanticIndexSetKey = "";
        semanticIndexSetKey += kSemantic;
        semanticIndexSetKey += GLTFUtils::toString(semantic);
        semanticIndexSetKey += ":indexSet";
        semanticIndexSetKey += GLTFUtils::toString(indexSet);
        
        return semanticIndexSetKey;
    }
    
    //---- GLTFPrimitiveRemapInfos -------------------------------------------------------------
    
    // FIXME: put better comment here
    //RemappedMeshIndexes[0] = count ;  RemappedMeshIndexes[1+] original indexes (used for hash code)
    struct RemappedMeshIndexesHash {
        inline size_t operator()(unsigned int* remappedMeshIndexes) const
        {
            size_t hash = 0;
            size_t count = (size_t)remappedMeshIndexes[0];
            
            for (size_t i = 0 ; i < count ; i++) {
                hash += (size_t)remappedMeshIndexes[i + 1 /* skip count */];
            }

            return hash;
        }
    };
    
    struct RemappedMeshIndexesEq {
        inline bool operator()(unsigned int* k1, unsigned int* k2) const {
            
            size_t count = (size_t)k1[0];
            
            if (count != (size_t)k2[0])
                return false;
            
            for (size_t i = 0 ; i < count ; i++) {
                if (k1[i + 1] != k2[i + 1])
                    return false;
            }
            
            return true;
        }
    };
    
    typedef unordered_map<unsigned int* ,unsigned int /* index of existing n-uplet of indices */, RemappedMeshIndexesHash, RemappedMeshIndexesEq> RemappedMeshIndexesHashmap;

    typedef unordered_map<unsigned int ,unsigned int> IndicesMap;
    
    //FIXME: this could be just an intermediate anonymous(no id) GLTFBuffer
    class GLTFPrimitiveRemapInfos
    {
    public:
        GLTFPrimitiveRemapInfos(unsigned int* generatedIndices, unsigned int generatedIndicesCount, unsigned int *originalCountAndIndexes);
        virtual ~GLTFPrimitiveRemapInfos();
        
        unsigned int generatedIndicesCount();
        unsigned int* generatedIndices();
        unsigned int* originalCountAndIndexes();
        
    private:
        unsigned int _generatedIndicesCount;
        unsigned int* _generatedIndices;
        unsigned int* _originalCountAndIndexes;
    };
    
    //---- GLTFPrimitiveRemapInfos -------------------------------------------------------------
    GLTFPrimitiveRemapInfos::GLTFPrimitiveRemapInfos(unsigned int* generatedIndices, unsigned int generatedIndicesCount, unsigned int *originalCountAndIndexes):
    _generatedIndicesCount(generatedIndicesCount),
    _generatedIndices(generatedIndices),
    _originalCountAndIndexes(originalCountAndIndexes)
    {
    }
    
    GLTFPrimitiveRemapInfos::~GLTFPrimitiveRemapInfos()
    {
        if (this->_generatedIndices)
            free(this->_generatedIndices);
        if (this->_originalCountAndIndexes)
            free(this->_originalCountAndIndexes);
    }
    
    unsigned int GLTFPrimitiveRemapInfos::generatedIndicesCount()
    {
        return _generatedIndicesCount;
    }
    
    unsigned int* GLTFPrimitiveRemapInfos::generatedIndices()
    {
        return _generatedIndices;
    }
    
    unsigned int* GLTFPrimitiveRemapInfos::originalCountAndIndexes()
    {
        return _originalCountAndIndexes;
    }
    
    typedef struct {
        unsigned char* remappedBufferData;
        //size_t remappedMeshAttributeByteOffset;
        size_t remappedMeshAttributeByteStride;
        
        unsigned char* originalBufferData;
        //size_t originalMeshAttributeByteOffset;
        size_t originalMeshAttributeByteStride;
        
        size_t elementByteLength;
        
        
    } MeshAttributesBufferInfos;
    
    static MeshAttributesBufferInfos* createMeshAttributesBuffersInfos(MeshAttributeVector allOriginalMeshAttributes ,MeshAttributeVector allRemappedMeshAttributes, unsigned int*indicesInRemapping, unsigned int count)
    {
        MeshAttributesBufferInfos* allBufferInfos = (MeshAttributesBufferInfos*)malloc(count * sizeof(MeshAttributesBufferInfos));
        for (size_t meshAttributeIndex = 0 ; meshAttributeIndex < count; meshAttributeIndex++) {
            MeshAttributesBufferInfos *bufferInfos = &allBufferInfos[meshAttributeIndex];
            
            shared_ptr <GLTF::GLTFAccessor> remappedMeshAttribute = allRemappedMeshAttributes[indicesInRemapping[meshAttributeIndex]];
            shared_ptr <GLTF::GLTFAccessor> originalMeshAttribute = allOriginalMeshAttributes[indicesInRemapping[meshAttributeIndex]];
            
            if (originalMeshAttribute->elementByteLength() != remappedMeshAttribute->elementByteLength()) {
                // FIXME : report error
                free(allBufferInfos);
                return 0;
            }            
            bufferInfos->remappedBufferData = (unsigned char*)remappedMeshAttribute->getBufferView()->getBufferDataByApplyingOffset();
            bufferInfos->remappedMeshAttributeByteStride = remappedMeshAttribute->getByteStride();
            
            bufferInfos->originalBufferData = (unsigned char*)originalMeshAttribute->getBufferView()->getBufferDataByApplyingOffset();;
            bufferInfos->originalMeshAttributeByteStride = originalMeshAttribute->getByteStride();
            
            bufferInfos->elementByteLength = remappedMeshAttribute->elementByteLength();
        }
        
        return allBufferInfos;
    }
    
    bool __RemapPrimitiveVertices(shared_ptr<GLTF::GLTFPrimitive> primitive,
                                  std::vector< shared_ptr<GLTF::GLTFAccessor> > allIndices,
                                  MeshAttributeVector allOriginalMeshAttributes,
                                  MeshAttributeVector allRemappedMeshAttributes,
                                  unsigned int* indicesInRemapping,
                                  shared_ptr<GLTF::GLTFPrimitiveRemapInfos> primitiveRemapInfos,
                                  unsigned int* remapTableForPositions /* we fill/keep this to be able to remap skin weight/bone indices later on*/)
    {
        size_t indicesSize = allIndices.size();
        if (allOriginalMeshAttributes.size() < indicesSize) {
            //TODO: assert & inconsistency check
        }
        
        unsigned int vertexAttributesCount = (unsigned int)indicesSize;
        
        //get the primitive infos to know where we need to "go" for remap
        unsigned int* indices = primitiveRemapInfos->generatedIndices();
        
        MeshAttributesBufferInfos *allBufferInfos = createMeshAttributesBuffersInfos(allOriginalMeshAttributes , allRemappedMeshAttributes, indicesInRemapping, vertexAttributesCount);
        
        unsigned int* uniqueIndicesBuffer = (unsigned int*)primitive->getIndices()->getBufferView()->getBufferDataByApplyingOffset();
        unsigned int *originalCountAndIndexes = primitiveRemapInfos->originalCountAndIndexes();
        
        unsigned int count = primitiveRemapInfos->generatedIndicesCount();
        for (unsigned int k = 0 ; k < count ; k++) {
            unsigned int idx = indices[k];
            unsigned int* remappedIndex = &originalCountAndIndexes[idx * (allOriginalMeshAttributes.size() + 1)];
            
            for (size_t meshAttributeIndex = 0 ; meshAttributeIndex < vertexAttributesCount  ; meshAttributeIndex++) {
                unsigned int indiceInRemap = indicesInRemapping[meshAttributeIndex];
                unsigned int rindex = remappedIndex[1 /* skip count */ + indiceInRemap];
                
                if (meshAttributeIndex == 0) {
                    //HACK: we know that first vertex attribute to be processed is of semantic POSITION
                    remapTableForPositions[uniqueIndicesBuffer[idx]] = rindex;
                }
                
                MeshAttributesBufferInfos *bufferInfos = &allBufferInfos[meshAttributeIndex];
                void *ptrSrc = (unsigned char*)bufferInfos->originalBufferData + (rindex * bufferInfos->originalMeshAttributeByteStride);
                //FIXME: optimize / secure this a bit, too many indirections without testing for invalid pointers
                /* copy the vertex attributes at the right offset and right indice (using the generated uniqueIndexes table */
                void *ptrDst = bufferInfos->remappedBufferData + (uniqueIndicesBuffer[idx] * bufferInfos->remappedMeshAttributeByteStride);

                memcpy(ptrDst, ptrSrc , bufferInfos->elementByteLength);
            }
        }
        
        if (allBufferInfos)
            free(allBufferInfos);
        
        return true;
    }
    
    
    shared_ptr<GLTF::GLTFPrimitiveRemapInfos> __BuildPrimitiveUniqueIndexes(shared_ptr<GLTF::GLTFPrimitive> primitive,
                                                                            std::vector< shared_ptr<GLTF::GLTFAccessor> > allIndices,
                                                                            RemappedMeshIndexesHashmap& remappedMeshIndexesMap,
                                                                            unsigned int* indicesInRemapping,
                                                                            size_t startIndex,
                                                                            unsigned int meshAttributesCount,
                                                                            size_t &endIndex,
                                                                            shared_ptr<GLTFProfile> profile)
    {
        unsigned int generatedIndicesCount = 0;

        size_t allIndicesSize = allIndices.size();
        size_t vertexIndicesCount = allIndices[0]->getCount();
        size_t sizeOfRemappedIndex = (meshAttributesCount + 1) * sizeof(unsigned int);
        
        unsigned int* originalCountAndIndexes = (unsigned int*)calloc( vertexIndicesCount, sizeOfRemappedIndex);
        //this is useful for debugging.
        
        unsigned int *uniqueIndexes = (unsigned int*)calloc( vertexIndicesCount , sizeof(unsigned int));
        unsigned int *generatedIndices = (unsigned int*) calloc (vertexIndicesCount , sizeof(unsigned int)); //owned by PrimitiveRemapInfos
		unsigned int currentIndex = (unsigned int)startIndex;
        
        unsigned int** allIndicesPtr = (unsigned int**) malloc(sizeof(unsigned int*) * allIndicesSize);
        for (unsigned int i = 0 ; i < allIndicesSize ; i++) {
            allIndicesPtr[i] = (unsigned int*)allIndices[i]->getBufferView()->getBufferDataByApplyingOffset();
        }
        
        for (size_t k = 0 ; k < vertexIndicesCount ; k++) {
            unsigned int* remappedIndex = &originalCountAndIndexes[k * (meshAttributesCount + 1)];
                        
            remappedIndex[0] = meshAttributesCount;
            for (unsigned int i = 0 ; i < allIndicesSize ; i++) {
                unsigned int idx = indicesInRemapping[i];
                unsigned int* indicesPtr = allIndicesPtr[i];
                remappedIndex[1 + idx] = indicesPtr[k];
            }
            
            unsigned int index;
            if (remappedMeshIndexesMap.count(remappedIndex) == 0) {
                index = currentIndex++;
                generatedIndices[generatedIndicesCount++] = (unsigned int)k;
                remappedMeshIndexesMap[remappedIndex] = index;
                                
            } else {
                index = remappedMeshIndexesMap[remappedIndex];
            }
            uniqueIndexes[k] = index;
        }
        
        endIndex = currentIndex;
        shared_ptr <GLTF::GLTFPrimitiveRemapInfos> primitiveRemapInfos(new GLTF::GLTFPrimitiveRemapInfos(generatedIndices, generatedIndicesCount, originalCountAndIndexes));
        shared_ptr <GLTF::GLTFBufferView> indicesBufferView = createBufferViewWithAllocatedBuffer(uniqueIndexes, 0, vertexIndicesCount * sizeof(unsigned int), true);
        
        shared_ptr <GLTF::GLTFAccessor> indices = shared_ptr <GLTF::GLTFAccessor> (new GLTFAccessor(profile, profile->getGLenumForString("UNSIGNED_SHORT")));
        
        indices->setBufferView(indicesBufferView);
        indices->setCount(vertexIndicesCount);
        
        primitive->setIndices(indices);
        
        free(allIndicesPtr);
        
        return primitiveRemapInfos;
    }

#define DUMP_UNIFIED_INDEXES_INFO 0
    
    shared_ptr <GLTFMesh> createUnifiedIndexesMeshFromMesh( GLTFMesh *sourceMesh,
                                                            std::vector< shared_ptr<IndicesVector> > &vectorOfIndicesVector,
                                                            shared_ptr<GLTFProfile> profile)
    {
        MeshAttributeVector originalMeshAttributes;
        MeshAttributeVector remappedMeshAttributes;
        shared_ptr <GLTFMesh> targetMesh(new GLTFMesh(*sourceMesh));
        
        GLTF::JSONValueVector sourcePrimitives = sourceMesh->getPrimitives()->values();
        GLTF::JSONValueVector targetPrimitives = targetMesh->getPrimitives()->values();
        
#if DUMP_UNIFIED_INDEXES_INFO
        {
        shared_ptr<GLTFAccessor> vertexSource = sourceMesh->getMeshAttribute(GLTF::POSITION, 0);
        printf("vertex source cout :%d\n",(int)vertexSource->getCount());
        }
#endif
        size_t startIndex = 0;
        size_t endIndex = 0;
        size_t primitiveCount = sourcePrimitives.size();
        unsigned int maxVertexAttributes = 0;
        
        if (primitiveCount == 0) {
            // FIXME: report error
            return nullptr;
        }
        
        //in originalMeshAttributes we'll get the flattened list of all the meshAttributes as a vector.
        //fill semanticAndSetToIndex with key: (semantic, indexSet) value: index in originalMeshAttributes vector.
        vector <GLTF::Semantic> allSemantics = sourceMesh->allSemantics();
        std::map<string, unsigned int> semanticAndSetToIndex;
        
        for (unsigned int i = 0 ; i < allSemantics.size() ; i++) {
            GLTF::Semantic semantic = allSemantics[i];
            size_t attributesCount = sourceMesh->getMeshAttributesCountForSemantic(semantic);
            for (size_t j = 0 ; j < attributesCount ; j ++) {
                shared_ptr <GLTF::GLTFAccessor> selectedMeshAttribute = sourceMesh->getMeshAttribute(semantic, j);
                unsigned int indexSet = j;
                std::string semanticIndexSetKey = keyWithSemanticAndSet(semantic, indexSet);
                unsigned int size = (unsigned int)originalMeshAttributes.size();
                semanticAndSetToIndex[semanticIndexSetKey] = size;
        
                originalMeshAttributes.push_back(selectedMeshAttribute);
            }
        }
        
        maxVertexAttributes = (unsigned int)originalMeshAttributes.size();
        
        vector <shared_ptr<GLTF::GLTFPrimitiveRemapInfos> > allPrimitiveRemapInfos;
        
        //build a array that maps the meshAttributes that the indices points to with the index of the indice.
        GLTF::RemappedMeshIndexesHashmap remappedMeshIndexesMap;
        for (unsigned int i = 0 ; i < primitiveCount ; i++) {
            shared_ptr<IndicesVector>  allIndicesSharedPtr = vectorOfIndicesVector[i];
            IndicesVector *allIndices = allIndicesSharedPtr.get();
            unsigned int* indicesInRemapping = (unsigned int*)malloc(sizeof(unsigned int) * allIndices->size());
            
            shared_ptr<GLTFPrimitive> sourcePrimitive = static_pointer_cast<GLTFPrimitive>(sourcePrimitives[i]);
            VertexAttributeVector vertexAttributes = sourcePrimitive->getVertexAttributes();
            for (unsigned int k = 0 ; k < allIndices->size() ; k++) {
                GLTF::Semantic semantic = vertexAttributes[k]->getSemantic();
				unsigned int indexSet = (unsigned int)vertexAttributes[k]->getIndexOfSet();
                std::string semanticIndexSetKey = keyWithSemanticAndSet(semantic, indexSet);
                unsigned int idx = semanticAndSetToIndex[semanticIndexSetKey];
                indicesInRemapping[k] = idx;
            }
            
            shared_ptr<GLTFPrimitive> targetPrimitive = static_pointer_cast<GLTFPrimitive>(targetPrimitives[i]);
            
            shared_ptr<GLTF::GLTFPrimitiveRemapInfos> primitiveRemapInfos = __BuildPrimitiveUniqueIndexes(targetPrimitive, *allIndices, remappedMeshIndexesMap, indicesInRemapping, startIndex, maxVertexAttributes, endIndex, profile);
            
            free(indicesInRemapping);
            
            if (primitiveRemapInfos.get()) {
                startIndex = endIndex;
                allPrimitiveRemapInfos.push_back(primitiveRemapInfos);
            } else {
                // FIXME: report error
                return nullptr;
            }
        }
        
        // now we got not only the uniqueIndexes but also the number of different indexes, i.e the number of vertex attributes count
        // we can allocate the buffer to hold vertex attributes
		unsigned int vertexCount = (unsigned int)endIndex;
        //just allocate it now, will be filled later
        unsigned int* remapTableForPositions = (unsigned int*)malloc(sizeof(unsigned int) * vertexCount);
        targetMesh->setRemapTableForPositions(remapTableForPositions);
        
        for (unsigned int i = 0 ; i < allSemantics.size() ; i++) {
            GLTF::Semantic semantic = allSemantics[i];
            size_t attributesCount = sourceMesh->getMeshAttributesCountForSemantic(semantic);
            for (size_t j = 0 ; j < attributesCount ; j ++) {
                shared_ptr <GLTF::GLTFAccessor> selectedMeshAttribute = sourceMesh->getMeshAttribute(semantic, j);
                size_t sourceSize = vertexCount * selectedMeshAttribute->elementByteLength();
                void* sourceData = malloc(sourceSize);
                
                shared_ptr <GLTFBufferView> referenceBufferView = selectedMeshAttribute->getBufferView();
                shared_ptr <GLTFBufferView> remappedBufferView = createBufferViewWithAllocatedBuffer(referenceBufferView->getID(), sourceData, 0, sourceSize, true);

                shared_ptr <GLTFAccessor> remappedMeshAttribute(new GLTFAccessor(selectedMeshAttribute.get()));
                remappedMeshAttribute->setBufferView(remappedBufferView);
                remappedMeshAttribute->setCount(vertexCount);
                
                targetMesh->setMeshAttribute(semantic, j, remappedMeshAttribute);
                
                remappedMeshAttributes.push_back(remappedMeshAttribute);
            }
        }
        
        /*
         if (_allOriginalMeshAttributes.size() != allIndices.size()) {
         // FIXME: report error
         return false;
         }
         */
        
        for (unsigned int i = 0 ; i < primitiveCount ; i++) {
            shared_ptr<IndicesVector>  allIndicesSharedPtr = vectorOfIndicesVector[i];
            IndicesVector *allIndices = allIndicesSharedPtr.get();
            unsigned int* indicesInRemapping = (unsigned int*)calloc(sizeof(unsigned int) * (*allIndices).size(), 1);
            shared_ptr<GLTFPrimitive> sourcePrimitive = static_pointer_cast<GLTFPrimitive>(sourcePrimitives[i]);
            VertexAttributeVector vertexAttributes = sourcePrimitive->getVertexAttributes();
            
            for (unsigned int k = 0 ; k < (*allIndices).size() ; k++) {
                GLTF::Semantic semantic = vertexAttributes[k]->getSemantic();
				unsigned int indexSet = (unsigned int)vertexAttributes[k]->getIndexOfSet();
                std::string semanticIndexSetKey = keyWithSemanticAndSet(semantic, indexSet);
                unsigned int idx = semanticAndSetToIndex[semanticIndexSetKey];
                indicesInRemapping[k] = idx;
            }
            
            shared_ptr<GLTFPrimitive> targetPrimitive = static_pointer_cast<GLTFPrimitive>(targetPrimitives[i]);
            bool status = __RemapPrimitiveVertices(targetPrimitive,
                                                   (*allIndices),
                                                   originalMeshAttributes ,
                                                   remappedMeshAttributes,
                                                   indicesInRemapping,
                                                   allPrimitiveRemapInfos[i],
                                                   remapTableForPositions);
            free(indicesInRemapping);
            
            if (!status) {
                // FIXME: report error
                return nullptr;
            }
        }
        
#if DUMP_UNIFIED_INDEXES_INFO
        {
            shared_ptr<GLTFAccessor> vertexSource = targetMesh->getMeshAttribute(GLTF::POSITION, 0);
            printf("vertex dest count :%d\n",(int)vertexSource->getCount());
        }
#endif
        
        return targetMesh;
    }
        
    //------ Mesh splitting ----
    
    class SubMeshContext {
    public:
        shared_ptr <GLTFMesh> targetMesh;
        //For each sub mesh being built, maintain 2 maps,
        //with key:indice value: remapped indice
        IndicesMap indexToRemappedIndex;
    } ;

    SubMeshContext* __CreateSubMeshContext(const std::string& id)
    {
        SubMeshContext *subMesh = new SubMeshContext();
        shared_ptr <GLTFMesh> targetMesh = shared_ptr <GLTFMesh> (new GLTFMesh());
        targetMesh->setID(id);
        subMesh->targetMesh = targetMesh;
        
        return subMesh;
    }
    
    void __PushAndRemapIndicesInSubMesh(SubMeshContext *subMesh, unsigned int* indices, int count)
    {
        for (int i = 0 ; i < count ; i++) {
            unsigned int index = indices[i];
            
            bool shouldRemap = subMesh->indexToRemappedIndex.count(index) == 0;
            if (shouldRemap) {
				unsigned int remappedIndex = (unsigned int)subMesh->indexToRemappedIndex.size();
                
                subMesh->indexToRemappedIndex[index] = remappedIndex;
            } 
        }
    }
    
    static void __RemapMeshAttribute(void *value,
                          GLTF::ComponentType type,
                          size_t componentsPerElement,
                          size_t index,
                          size_t vertexAttributeByteSize,
                          void *context) {
        
        void **remapContext = (void**)context;
        unsigned char *targetBufferPtr = (unsigned char*)remapContext[0];
        SubMeshContext *subMesh = (SubMeshContext*)remapContext[1];
		if (subMesh->indexToRemappedIndex.count((unsigned int)index) > 0) {
			size_t remappedIndex = subMesh->indexToRemappedIndex[(unsigned int)index];
            memcpy(&targetBufferPtr[vertexAttributeByteSize * remappedIndex], value, vertexAttributeByteSize);
        }
    }
    
    //FIXME: add suport for interleaved arrays
    static void __RemapSubMesh(SubMeshContext *subMesh, GLTFMesh *sourceMesh)
    {
        //remap the subMesh using the original mesh
        //we walk through all meshAttributes
        vector <GLTF::Semantic> allSemantics = sourceMesh->allSemantics();
        std::map<string, unsigned int> semanticAndSetToIndex;
        
        for (unsigned int i = 0 ; i < allSemantics.size() ; i++) {
            GLTF::Semantic semantic = allSemantics[i];
            size_t attributesCount = sourceMesh->getMeshAttributesCountForSemantic(semantic);
            for (size_t j = 0 ; j < attributesCount ; j ++) {
                shared_ptr <GLTFAccessor> selectedMeshAttribute = sourceMesh->getMeshAttribute(semantic, j);
                
                shared_ptr <GLTFBufferView> referenceBufferView = selectedMeshAttribute->getBufferView();
                
				unsigned int vertexAttributeCount = (unsigned int)subMesh->indexToRemappedIndex.size();
                
                //FIXME: this won't work with interleaved
                unsigned int *targetBufferPtr = (unsigned int*)malloc(selectedMeshAttribute->elementByteLength() * vertexAttributeCount);
                
                void *context[2];
                context[0] = targetBufferPtr;
                context[1] = subMesh;
                selectedMeshAttribute->applyOnAccessor(__RemapMeshAttribute, (void*)context);
                                        
                shared_ptr <GLTFBufferView> remappedBufferView =
                createBufferViewWithAllocatedBuffer(referenceBufferView->getID(), targetBufferPtr, 0, selectedMeshAttribute->elementByteLength() * vertexAttributeCount, true);
                
                shared_ptr <GLTFAccessor> remappedMeshAttribute(new GLTF::GLTFAccessor(selectedMeshAttribute.get()));
                remappedMeshAttribute->setBufferView(remappedBufferView);
                remappedMeshAttribute->setCount(vertexAttributeCount);
                
                subMesh->targetMesh->setMeshAttribute(semantic, j, remappedMeshAttribute);
            }
        }
    }
    
    shared_ptr <GLTFMesh> createMeshWithMaximumIndicesCountFromMeshIfNeeded(GLTFMesh *sourceMesh, size_t maximumIndicesCount, shared_ptr<GLTFProfile> profile)
    {
        shared_ptr <GLTFMesh> destinationMesh = nullptr;
        bool splitNeeded = sourceMesh->getMeshAttribute(GLTF::POSITION, 0)->getCount()  >= maximumIndicesCount;
        if (!splitNeeded)
            return nullptr;
        
        GLTF::JSONValueVector primitives = sourceMesh->getPrimitives()->values();

        SubMeshContext *subMesh = nullptr;

        bool stillHavePrimitivesElementsToBeProcessed = false;
        bool primitiveCompleted = false;
        
        int *allNextPrimitiveIndices = (int*)calloc(primitives.size(), sizeof(int));
        size_t meshIndex = 0;
        shared_ptr <GLTFMesh> targetMesh = nullptr;
        
        for (size_t i = 0 ; i < primitives.size() ; i++) {
            if (allNextPrimitiveIndices[i] == -1)
                continue;
            
            if (subMesh == nullptr) {
                if (targetMesh == nullptr) {
                    subMesh = __CreateSubMeshContext(sourceMesh->getID());
                    targetMesh = subMesh->targetMesh;
                }
                
                else {
                    std::string id = sourceMesh->getID() + "split_" + GLTFUtils::toString(targetMesh->subMeshes()->values().size());
                    subMesh = __CreateSubMeshContext(id);
                    targetMesh->subMeshes()->appendValue(subMesh->targetMesh);
                }
                std::string meshName = sourceMesh->getName();
                if (meshIndex) {
                    meshName += GLTFUtils::toString(meshIndex);
                }
                subMesh->targetMesh->setName(meshName);
                
                stillHavePrimitivesElementsToBeProcessed = false;
                meshIndex++;
            }
            
            shared_ptr <GLTFPrimitive> targetPrimitive;
            //when we are done with a primitive we mark its nextIndice with a -1
            shared_ptr<GLTFPrimitive> primitive = static_pointer_cast<GLTFPrimitive> (primitives[i]);

            targetPrimitive = shared_ptr <GLTFPrimitive> (new GLTFPrimitive((*primitive)));
            
            unsigned int nextPrimitiveIndex = (unsigned int)allNextPrimitiveIndices[i];
            
            shared_ptr<GLTFAccessor> indices = primitive->getIndices();
            
            unsigned int* indicesPtr = (unsigned int*)indices->getBufferView()->getBufferDataByApplyingOffset();
            unsigned int* targetIndicesPtr = (unsigned int*)malloc(indices->getBufferView()->getBuffer()->getByteLength());
            
            //sub meshes are built this way [ and it is not optimal yet (*)]:
            //each primitive is iterated through all its triangles/lines/...
            //When the indices count in indexToRemappedIndex is >= maxiumIndicesCount then we try the next primitive.
            /* 
                we could continue walking through a primitive even if the of maximum indices has been reached, because, for instance the next say, triangles could be within the already remapped indices. That said, not doing so should produce meshes that have more chances to have adjacent triangles. Need more experimentation about this. Having 2 modes would ideal.
             */
            //Different iterators type will be needed for these types
            /*
             type = "TRIANGLES";
             type = "LINES";
             type = "LINE_STRIP";
             type = "TRIANGLES";
             type = "TRIANGLE_FANS";
             type = "TRIANGLE_STRIPS";
             type = "POINTS";
             */
            size_t j = 0;
            unsigned int primitiveCount = 0;
            unsigned int targetIndicesCount = 0;
            if (primitive->getPrimitive() == profile->getGLenumForString("TRIANGLES")) {
                unsigned int indicesPerElementCount = 3;
				primitiveCount = (unsigned int)indices->getCount() / indicesPerElementCount;
                for (j = nextPrimitiveIndex ; j < primitiveCount ; j++) {
                    unsigned int *indicesPtrAtPrimitiveIndex = indicesPtr + (j * indicesPerElementCount);
                    //will we still have room to store coming indices from this mesh ?
                    //note: this is tied to the policy described above in (*)
                    size_t currentSize = subMesh->indexToRemappedIndex.size();
                    if ((currentSize + indicesPerElementCount) < maximumIndicesCount) {
                        __PushAndRemapIndicesInSubMesh(subMesh, indicesPtrAtPrimitiveIndex, indicesPerElementCount);

                        //build the indices for the primitive to be added to the subMesh
                        targetIndicesPtr[targetIndicesCount] = subMesh->indexToRemappedIndex[indicesPtrAtPrimitiveIndex[0]];
                        targetIndicesPtr[targetIndicesCount + 1] = subMesh->indexToRemappedIndex[indicesPtrAtPrimitiveIndex[1]];
                        targetIndicesPtr[targetIndicesCount + 2] = subMesh->indexToRemappedIndex[indicesPtrAtPrimitiveIndex[2]];
                        
                        targetIndicesCount += indicesPerElementCount;
                    
                        nextPrimitiveIndex++;
                    } else {
                        allNextPrimitiveIndices[i] = -1;
                        primitiveCompleted = true;
                        break;
                    }
                }
            }
            else if (primitive->getPrimitive() == profile->getGLenumForString("LINES")) {
                unsigned int indicesPerElementCount = 2;
                primitiveCount = (unsigned int)indices->getCount() / indicesPerElementCount;
                for (j = nextPrimitiveIndex; j < primitiveCount; j++) {
                    unsigned int *indicesPtrAtPrimitiveIndex = indicesPtr + (j * indicesPerElementCount);
                    //will we still have room to store coming indices from this mesh ?
                    //note: this is tied to the policy described above in (*)
                    size_t currentSize = subMesh->indexToRemappedIndex.size();
                    if ((currentSize + indicesPerElementCount) < maximumIndicesCount) {
                        __PushAndRemapIndicesInSubMesh(subMesh, indicesPtrAtPrimitiveIndex, indicesPerElementCount);

                        //build the indices for the primitive to be added to the subMesh
                        targetIndicesPtr[targetIndicesCount] = subMesh->indexToRemappedIndex[indicesPtrAtPrimitiveIndex[0]];
                        targetIndicesPtr[targetIndicesCount + 1] = subMesh->indexToRemappedIndex[indicesPtrAtPrimitiveIndex[1]];

                        targetIndicesCount += indicesPerElementCount;

                        nextPrimitiveIndex++;
                    }
                    else {
                        allNextPrimitiveIndices[i] = -1;
                        primitiveCompleted = true;
                        break;
                    }
                }
            }
            
            allNextPrimitiveIndices[i] = nextPrimitiveIndex;

            if (targetIndicesCount > 0) {
                shared_ptr <GLTFBufferView> targetBufferView = createBufferViewWithAllocatedBuffer(targetIndicesPtr, 0,targetIndicesCount * sizeof(unsigned int), true);
                
                shared_ptr <GLTFAccessor> indices(new GLTFAccessor(profile, profile->getGLenumForString("UNSIGNED_SHORT")));

                indices->setBufferView(targetBufferView);
                indices->setCount(targetIndicesCount);
                
                targetPrimitive->setIndices(indices);
                
                subMesh->targetMesh->appendPrimitive(targetPrimitive);
            } else {
                if (targetIndicesPtr)
                    free(targetIndicesPtr);
            }
            
            if (j < primitiveCount)
                stillHavePrimitivesElementsToBeProcessed = true;
            
            //did we process the last primitive ?
            if (primitiveCompleted || (((i + 1) == primitives.size()))) {
                __RemapSubMesh(subMesh, sourceMesh);
                if (stillHavePrimitivesElementsToBeProcessed) {
                    //loop again and build new mesh
                    i = -1;
                    delete subMesh;
                    subMesh = 0;
                }
            }
        }
        
        free(allNextPrimitiveIndices);
        
        return targetMesh;
    }

    //Not required anymore since Open3DGC supports now sharing same vertex buffer and WebGL is disabled
    //DO NOT REMOVE - might be useful again someday
    /*
    bool createMeshesFromMeshPrimitives(GLTFMesh *sourceMesh, MeshVector &meshes, shared_ptr<GLTFProfile> profile) {
        GLTF::JSONValueVector primitives = sourceMesh->getPrimitives()->values();
        if (primitives.size() == 1) {
            return false;
        }
        
        size_t primitiveCount = primitives.size();
        
        for (size_t i = 0 ; i < primitiveCount ; i++) {
            IndicesMap remappedIndices;
            shared_ptr <GLTFMesh> targetMesh = shared_ptr <GLTFMesh> (new GLTFMesh());
            shared_ptr <GLTFPrimitive> refPrimitive = static_pointer_cast<GLTFPrimitive>(primitives[i]);
            shared_ptr <GLTFPrimitive> targetPrimitive = shared_ptr <GLTFPrimitive> (new GLTFPrimitive((*refPrimitive)));

            targetMesh->appendPrimitive(targetPrimitive);
            
            unsigned int* originalIndices = (unsigned int*)refPrimitive->getIndices()->getBufferView()->getBufferDataByApplyingOffset();
            size_t indicesCount = refPrimitive->getIndices()->getCount();
            unsigned int* targetIndices = (unsigned int*)malloc(indicesCount * sizeof(unsigned int));
            
            //perform remapping of indices
            for (size_t j = 0 ; j < indicesCount ; j++) {
                if (remappedIndices.count(originalIndices[j]) == 0) {
                    //this index is not yet added
                    targetIndices[j] = remappedIndices.size();
                    remappedIndices[originalIndices[j]] = targetIndices[j];
                } else {
                    targetIndices[j] = remappedIndices[originalIndices[j]];
                }
            }
            
            shared_ptr <GLTFBufferView> targetIndicesView = createBufferViewWithAllocatedBuffer(targetIndices, 0,indicesCount * sizeof(unsigned int), true);
            
            shared_ptr <GLTFAccessor> indices(new GLTFAccessor(profile, profile->getGLenumForString("UNSIGNED_SHORT")));
            
            indices->setBufferView(targetIndicesView);
            indices->setCount(indicesCount);
            
            targetPrimitive->setIndices(indices);

            
            // Now for each mesh attribute in the mesh, create another one just for the primitive
            shared_ptr <GLTFPrimitive> primitive = static_pointer_cast<GLTFPrimitive>(primitives[i]);
            VertexAttributeVector vertexAttributes = primitive->getVertexAttributes();
            for (size_t j = 0 ; j < vertexAttributes.size() ; j++) {
                Semantic semantic = vertexAttributes[j]->getSemantic();
                size_t indexOfSet = vertexAttributes[j]->getIndexOfSet();
                shared_ptr <GLTFAccessor> meshAttribute = sourceMesh->getMeshAttribute(semantic, indexOfSet);
                
                size_t targetVertexCount = remappedIndices.size();
                
                unsigned char *sourcePtr = (unsigned char *)meshAttribute->getBufferView()->getBufferDataByApplyingOffset();
                
                shared_ptr<GLTFAccessor> targetAttribute = shared_ptr<GLTFAccessor> (new GLTFAccessor(meshAttribute.get()));
                size_t targetAttributeSize = targetVertexCount * meshAttribute->elementByteLength();
                unsigned char *targetAttributePtr = (unsigned char*)malloc(targetAttributeSize);
                shared_ptr <GLTFBufferView> targetAttributeBufferView = createBufferViewWithAllocatedBuffer(targetAttributePtr, 0, targetAttributeSize, true);
                targetAttribute->setCount(targetVertexCount);
                targetAttribute->setBufferView(targetAttributeBufferView);

                size_t vertexAttributeByteSize = meshAttribute->elementByteLength();

                IndicesMap::const_iterator indicesIterator;                
                for (indicesIterator = remappedIndices.begin() ; indicesIterator != remappedIndices.end() ; indicesIterator++) {
                    //(*it).first;             // the key value (of type Key)
                    //(*it).second;            // the mapped value (of type T)
                    unsigned int originalIndex = (*indicesIterator).first;
                    unsigned int remappedIndex = (*indicesIterator).second;
                    
                    memcpy(&targetAttributePtr[vertexAttributeByteSize * remappedIndex], sourcePtr + (vertexAttributeByteSize * originalIndex), vertexAttributeByteSize);

                }
                
                targetMesh->setMeshAttribute(semantic, indexOfSet, targetAttribute);
            }
            meshes->appendValue(targetMesh);
            
        }

        return true;
    }
    */
    
}
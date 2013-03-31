#include "GLTF.h"
#include <stdio.h>
#include <stdlib.h>
#include "geometryHelpers.h"

using namespace rapidjson;
using namespace std::tr1;
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
    
    std::string _KeyWithSemanticAndSet(GLTF::Semantic semantic, unsigned int indexSet)
    {
        std::string semanticIndexSetKey = "";
        semanticIndexSetKey += "semantic";
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
            
            shared_ptr <GLTF::GLTFMeshAttribute> remappedMeshAttribute = allRemappedMeshAttributes[indicesInRemapping[meshAttributeIndex]];
            shared_ptr <GLTF::GLTFMeshAttribute> originalMeshAttribute = allOriginalMeshAttributes[indicesInRemapping[meshAttributeIndex]];
            
            if (originalMeshAttribute->getVertexAttributeByteLength() != remappedMeshAttribute->getVertexAttributeByteLength()) {
                // FIXME : report error
                free(allBufferInfos);
                return 0;
            }
            
            bufferInfos->remappedBufferData = (unsigned char*)remappedMeshAttribute->getBufferView()->getBufferDataByApplyingOffset();
            bufferInfos->remappedMeshAttributeByteStride = remappedMeshAttribute->getByteStride();
            
            bufferInfos->originalBufferData = (unsigned char*)originalMeshAttribute->getBufferView()->getBufferDataByApplyingOffset();;
            bufferInfos->originalMeshAttributeByteStride = originalMeshAttribute->getByteStride();
            
            bufferInfos->elementByteLength = remappedMeshAttribute->getVertexAttributeByteLength();
        }
        
        return allBufferInfos;
    }
    
    bool __RemapPrimitiveVertices(shared_ptr<GLTF::GLTFPrimitive> primitive,
                                  std::vector< shared_ptr<GLTF::GLTFIndices> > allIndices,
                                  MeshAttributeVector allOriginalMeshAttributes,
                                  MeshAttributeVector allRemappedMeshAttributes,
                                  unsigned int* indicesInRemapping,
                                  shared_ptr<GLTF::GLTFPrimitiveRemapInfos> primitiveRemapInfos)
    {
        size_t indicesSize = allIndices.size();
        if (allOriginalMeshAttributes.size() < indicesSize) {
            //TODO: assert & inconsistency check
        }
        
        unsigned int vertexAttributesCount = (unsigned int)indicesSize;
        
        //get the primitive infos to know where we need to "go" for remap
        unsigned int count = primitiveRemapInfos->generatedIndicesCount();
        unsigned int* indices = primitiveRemapInfos->generatedIndices();
        
        MeshAttributesBufferInfos *allBufferInfos = createMeshAttributesBuffersInfos(allOriginalMeshAttributes , allRemappedMeshAttributes, indicesInRemapping, vertexAttributesCount);
        
        unsigned int* uniqueIndicesBuffer = (unsigned int*)primitive->getIndices()->getBufferView()->getBufferDataByApplyingOffset();
        
        unsigned int *originalCountAndIndexes = primitiveRemapInfos->originalCountAndIndexes();
        
        for (unsigned int k = 0 ; k < count ; k++) {
            unsigned int idx = indices[k];
            unsigned int* remappedIndex = &originalCountAndIndexes[idx * (allOriginalMeshAttributes.size() + 1)];
            
            for (size_t meshAttributeIndex = 0 ; meshAttributeIndex < vertexAttributesCount  ; meshAttributeIndex++) {
                MeshAttributesBufferInfos *bufferInfos = &allBufferInfos[meshAttributeIndex];
                unsigned int indiceInRemap = indicesInRemapping[meshAttributeIndex];
                unsigned int rindex = remappedIndex[1 /* skip count */ + indiceInRemap];
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
                                                                                  std::vector< shared_ptr<GLTF::GLTFIndices> > allIndices,
                                                                                  RemappedMeshIndexesHashmap& remappedMeshIndexesMap,
                                                                                  unsigned int* indicesInRemapping,
                                                                                  unsigned int startIndex,
                                                                                  unsigned int meshAttributesCount,
                                                                                  unsigned int &endIndex)
    {
        unsigned int generatedIndicesCount = 0;

        size_t allIndicesSize = allIndices.size();
        size_t vertexIndicesCount = allIndices[0]->getCount();
        size_t sizeOfRemappedIndex = (meshAttributesCount + 1) * sizeof(unsigned int);
        
        unsigned int* originalCountAndIndexes = (unsigned int*)calloc( vertexIndicesCount, sizeOfRemappedIndex);
        //this is useful for debugging.
        
        unsigned int *uniqueIndexes = (unsigned int*)calloc( vertexIndicesCount , sizeof(unsigned int));
        unsigned int *generatedIndices = (unsigned int*) calloc (vertexIndicesCount , sizeof(unsigned int)); //owned by PrimitiveRemapInfos
        unsigned int currentIndex = startIndex;
        
        for (size_t k = 0 ; k < vertexIndicesCount ; k++) {
            unsigned int* remappedIndex = &originalCountAndIndexes[k * (meshAttributesCount + 1)];
                        
            remappedIndex[0] = meshAttributesCount;
            for (unsigned int i = 0 ; i < allIndicesSize ; i++) {
                unsigned int idx = indicesInRemapping[i];
                unsigned int* indicesPtr = (unsigned int*)allIndices[i]->getBufferView()->getBufferDataByApplyingOffset();
                remappedIndex[1 + idx] = indicesPtr[k];
            }
            
            unsigned int index = remappedMeshIndexesMap[remappedIndex];
            if (index == 0) {
                index = currentIndex++;
                generatedIndices[generatedIndicesCount++] = (unsigned int)k;
                remappedMeshIndexesMap[remappedIndex] = index;
            }
            uniqueIndexes[k] = index - 1;
        }
        
        endIndex = currentIndex;
        shared_ptr <GLTF::GLTFPrimitiveRemapInfos> primitiveRemapInfos(new GLTF::GLTFPrimitiveRemapInfos(generatedIndices, generatedIndicesCount, originalCountAndIndexes));
        shared_ptr <GLTF::GLTFBufferView> indicesBufferView = createBufferViewWithAllocatedBuffer(uniqueIndexes, 0, vertexIndicesCount * sizeof(unsigned int), true);
        
        shared_ptr <GLTF::GLTFIndices> indices = shared_ptr <GLTF::GLTFIndices> (new GLTF::GLTFIndices(indicesBufferView, vertexIndicesCount));
        
        primitive->setIndices(indices);
        
        return primitiveRemapInfos;
    }
    
    
    shared_ptr <GLTFMesh> CreateUnifiedIndexesMeshFromMesh(GLTFMesh *sourceMesh, std::vector< shared_ptr<IndicesVector> > &vectorOfIndicesVector)
    {
        MeshAttributeVector originalMeshAttributes;
        MeshAttributeVector remappedMeshAttributes;
        shared_ptr <GLTFMesh> targetMesh(new GLTFMesh(*sourceMesh));
        
        PrimitiveVector sourcePrimitives = sourceMesh->getPrimitives();
        PrimitiveVector targetPrimitives = targetMesh->getPrimitives();
        
        size_t startIndex = 1; // begin at 1 because the hashtable will return 0 when the element is not present
        unsigned endIndex = 0;
        size_t primitiveCount = sourcePrimitives.size();
        unsigned int maxVertexAttributes = 0;
        
        if (primitiveCount == 0) {
            // FIXME: report error
            //return 0;
        }
        
        //in originalMeshAttributes we'll get the flattened list of all the meshAttributes as a vector.
        //fill semanticAndSetToIndex with key: (semantic, indexSet) value: index in originalMeshAttributes vector.
        vector <GLTF::Semantic> allSemantics = sourceMesh->allSemantics();
        std::map<string, unsigned int> semanticAndSetToIndex;
        
        for (unsigned int i = 0 ; i < allSemantics.size() ; i++) {
            IndexSetToMeshAttributeHashmap& indexSetToMeshAttribute = sourceMesh->getMeshAttributesForSemantic(allSemantics[i]);
            IndexSetToMeshAttributeHashmap::const_iterator meshAttributeIterator;
            for (meshAttributeIterator = indexSetToMeshAttribute.begin() ; meshAttributeIterator != indexSetToMeshAttribute.end() ; meshAttributeIterator++) {
                //(*it).first;             // the key value (of type Key)
                //(*it).second;            // the mapped value (of type T)
                shared_ptr <GLTF::GLTFMeshAttribute> selectedMeshAttribute = (*meshAttributeIterator).second;
                unsigned int indexSet = (*meshAttributeIterator).first;
                GLTF::Semantic semantic = allSemantics[i];
                std::string semanticIndexSetKey = _KeyWithSemanticAndSet(semantic, indexSet);
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
            
            
            VertexAttributeVector vertexAttributes = sourcePrimitives[i]->getVertexAttributes();
            for (unsigned int k = 0 ; k < allIndices->size() ; k++) {
                GLTF::Semantic semantic = vertexAttributes[k]->getSemantic();
                unsigned int indexSet = vertexAttributes[k]->getIndexOfSet();
                std::string semanticIndexSetKey = _KeyWithSemanticAndSet(semantic, indexSet);
                unsigned int idx = semanticAndSetToIndex[semanticIndexSetKey];
                indicesInRemapping[k] = idx;
            }
            
            shared_ptr<GLTF::GLTFPrimitiveRemapInfos> primitiveRemapInfos = __BuildPrimitiveUniqueIndexes(targetPrimitives[i], *allIndices, remappedMeshIndexesMap, indicesInRemapping, startIndex, maxVertexAttributes, endIndex);
            
            free(indicesInRemapping);
            
            if (primitiveRemapInfos.get()) {
                startIndex = endIndex;
                allPrimitiveRemapInfos.push_back(primitiveRemapInfos);
            } else {
                // FIXME: report error
                //return NULL;
            }
        }
        
        // we are using WebGL for rendering, this involve OpenGL/ES where only float are supported.
        // now we got not only the uniqueIndexes but also the number of different indexes, i.e the number of vertex attributes count
        // we can allocate the buffer to hold vertex attributes
        unsigned int vertexCount = endIndex - 1;
        
        for (unsigned int i = 0 ; i < allSemantics.size() ; i++) {
            IndexSetToMeshAttributeHashmap& indexSetToMeshAttribute = sourceMesh->getMeshAttributesForSemantic(allSemantics[i]);
            IndexSetToMeshAttributeHashmap& destinationIndexSetToMeshAttribute = targetMesh->getMeshAttributesForSemantic(allSemantics[i]);
            IndexSetToMeshAttributeHashmap::const_iterator meshAttributeIterator;
            
            //FIXME: consider turn this search into a method for mesh
            for (meshAttributeIterator = indexSetToMeshAttribute.begin() ; meshAttributeIterator != indexSetToMeshAttribute.end() ; meshAttributeIterator++) {
                //(*it).first;             // the key value (of type Key)
                //(*it).second;            // the mapped value (of type T)
                shared_ptr <GLTF::GLTFMeshAttribute> selectedMeshAttribute = (*meshAttributeIterator).second;
                
                size_t sourceSize = vertexCount * selectedMeshAttribute->getVertexAttributeByteLength();
                void* sourceData = malloc(sourceSize);
                
                shared_ptr <GLTFBufferView> referenceBufferView = selectedMeshAttribute->getBufferView();
                shared_ptr <GLTFBufferView> remappedBufferView = createBufferViewWithAllocatedBuffer(referenceBufferView->getID(), sourceData, 0, sourceSize, true);

                shared_ptr <GLTFMeshAttribute> remappedMeshAttribute(new GLTFMeshAttribute(selectedMeshAttribute.get()));
                remappedMeshAttribute->setBufferView(remappedBufferView);
                remappedMeshAttribute->setCount(vertexCount);
                
                destinationIndexSetToMeshAttribute[(*meshAttributeIterator).first] = remappedMeshAttribute;
                
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
            VertexAttributeVector vertexAttributes = sourcePrimitives[i]->getVertexAttributes();
            
            for (unsigned int k = 0 ; k < (*allIndices).size() ; k++) {
                GLTF::Semantic semantic = vertexAttributes[k]->getSemantic();
                unsigned int indexSet = vertexAttributes[k]->getIndexOfSet();
                std::string semanticIndexSetKey = _KeyWithSemanticAndSet(semantic, indexSet);
                unsigned int idx = semanticAndSetToIndex[semanticIndexSetKey];
                indicesInRemapping[k] = idx;
            }
            
            bool status = __RemapPrimitiveVertices(targetPrimitives[i],
                                                   (*allIndices),
                                                   originalMeshAttributes ,
                                                   remappedMeshAttributes,
                                                   indicesInRemapping,
                                                   allPrimitiveRemapInfos[i]);
            free(indicesInRemapping);
            
            if (!status) {
                // FIXME: report error
                //return NULL;
            }
        }
        
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

    SubMeshContext* __CreateSubMeshContext()
    {
        SubMeshContext *subMesh = new SubMeshContext();
        shared_ptr <GLTFMesh> targetMesh = shared_ptr <GLTFMesh> (new GLTFMesh());
        subMesh->targetMesh = targetMesh;
        
        return subMesh;
    }
    
    void __PushAndRemapIndicesInSubMesh(SubMeshContext *subMesh, unsigned int* indices, int count)
    {
        for (int i = 0 ; i < count ; i++) {
            unsigned int index = indices[i];
            
            bool shouldRemap = subMesh->indexToRemappedIndex.count(index) == 0;
            if (shouldRemap) {
                unsigned int remappedIndex = subMesh->indexToRemappedIndex.size();
                
                subMesh->indexToRemappedIndex[index] = remappedIndex;
            } 
        }
    }
    
    static void __RemapMeshAttribute(void *value,
                          GLTF::ComponentType type,
                          size_t elementsPerVertexAttribute,
                          size_t index,
                          size_t vertexAttributeByteSize,
                          void *context) {
        
        void **remapContext = (void**)context;
        unsigned char *targetBufferPtr = (unsigned char*)remapContext[0];
        SubMeshContext *subMesh = (SubMeshContext*)remapContext[1];
        if (subMesh->indexToRemappedIndex.count(index) > 0) {
            size_t remappedIndex = subMesh->indexToRemappedIndex[index];
            memcpy(&targetBufferPtr[vertexAttributeByteSize * remappedIndex], value, vertexAttributeByteSize);
        }
    }
    
    //FIXME: add suport for interleaved arrays
    void __RemapSubMesh(SubMeshContext *subMesh, GLTFMesh *sourceMesh)
    {
        //remap the subMesh using the original mesh
        //we walk through all meshAttributes
        vector <GLTF::Semantic> allSemantics = sourceMesh->allSemantics();
        std::map<string, unsigned int> semanticAndSetToIndex;
        
        for (unsigned int i = 0 ; i < allSemantics.size() ; i++) {
            IndexSetToMeshAttributeHashmap& indexSetToMeshAttribute = sourceMesh->getMeshAttributesForSemantic(allSemantics[i]);
            IndexSetToMeshAttributeHashmap& targetIndexSetToMeshAttribute = subMesh->targetMesh->getMeshAttributesForSemantic(allSemantics[i]);
            IndexSetToMeshAttributeHashmap::const_iterator meshAttributeIterator;
            for (meshAttributeIterator = indexSetToMeshAttribute.begin() ; meshAttributeIterator != indexSetToMeshAttribute.end() ; meshAttributeIterator++) {
                //(*it).first;             // the key value (of type Key)
                //(*it).second;            // the mapped value (of type T)
                shared_ptr <GLTFMeshAttribute> selectedMeshAttribute = (*meshAttributeIterator).second;
                unsigned int indexSet = (*meshAttributeIterator).first;
                
                shared_ptr <GLTFBufferView> referenceBufferView = selectedMeshAttribute->getBufferView();
                
                unsigned int vertexAttributeCount = subMesh->indexToRemappedIndex.size();
                
                //FIXME: this won't work with interleaved
                unsigned int *targetBufferPtr = (unsigned int*)malloc(selectedMeshAttribute->getVertexAttributeByteLength() * vertexAttributeCount);
                
                void *context[2];
                context[0] = targetBufferPtr;
                context[1] = subMesh;
                selectedMeshAttribute->apply(__RemapMeshAttribute, (void*)context);
                                        
                shared_ptr <GLTFBufferView> remappedBufferView =
                createBufferViewWithAllocatedBuffer(referenceBufferView->getID(), targetBufferPtr, 0, selectedMeshAttribute->getVertexAttributeByteLength() * vertexAttributeCount, true);
                
                shared_ptr <GLTFMeshAttribute> remappedMeshAttribute(new GLTF::GLTFMeshAttribute(selectedMeshAttribute.get()));
                remappedMeshAttribute->setBufferView(remappedBufferView);
                remappedMeshAttribute->setCount(vertexAttributeCount);
                
                targetIndexSetToMeshAttribute[indexSet] = remappedMeshAttribute;
            }
        }
    }
    
    bool CreateMeshesWithMaximumIndicesCountFromMeshIfNeeded(GLTFMesh *sourceMesh, unsigned int maxiumIndicesCount, MeshVector &meshes)
    {
        bool splitNeeded = false;
        
        //First, check every primitive indices count to figure out if we really need to split anything at all.
        //TODO: what about making a sanity check, to ensure we don't have points not referenced by any primitive. (I wonder if that's would be considered compliant with the SPEC. need to check.
        PrimitiveVector primitives = sourceMesh->getPrimitives();
        
        for (size_t i = 0 ; i < primitives.size() ; i++) {
            if (primitives[i]->getIndices()->getCount() >= maxiumIndicesCount) {
                splitNeeded = true;
                break;
            }
        }
        
        if (!splitNeeded)
            return false;
        
        SubMeshContext *subMesh = NULL;

        bool stillHavePrimitivesElementsToBeProcessed = false;
        bool primitiveCompleted = false;
        
        int *allNextPrimitiveIndices = (int*)calloc(primitives.size(), sizeof(int));
        unsigned int meshIndex = 0;
        for (size_t i = 0 ; i < primitives.size() ; i++) {
            if (allNextPrimitiveIndices[i] == -1)
                continue;
            
            if (subMesh == 0) {
                subMesh = __CreateSubMeshContext();
                meshes.push_back(subMesh->targetMesh);
                std::string meshID = "";
                std::string meshName = "";
                
                meshID += sourceMesh->getID();
                meshName += sourceMesh->getName();
                if (meshIndex) {
                    meshID += "-"+ GLTFUtils::toString(meshIndex);
                    meshName += "-"+ GLTFUtils::toString(meshIndex);
                }
                subMesh->targetMesh->setID(meshID);
                subMesh->targetMesh->setName(meshName);
                
                stillHavePrimitivesElementsToBeProcessed = false;
                meshIndex++;
            }
            
            shared_ptr <GLTFPrimitive> targetPrimitive;
            //when we are done with a primitive we mark its nextIndice with a -1

            targetPrimitive = shared_ptr <GLTFPrimitive> (new GLTFPrimitive((*primitives[i])));
            
            unsigned int nextPrimitiveIndex = (unsigned int)allNextPrimitiveIndices[i];
            
            shared_ptr<GLTFPrimitive> &primitive = primitives[i];
            shared_ptr<GLTFIndices> indices = primitive->getIndices();
            
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
            if (primitive->getType() == "TRIANGLES") {
                unsigned int indicesPerElementCount = 3;
                primitiveCount = indices->getCount() / indicesPerElementCount;
                for (j = nextPrimitiveIndex ; j < primitiveCount ; j++) {
                    unsigned int *indicesPtrAtPrimitiveIndex = indicesPtr + (j * indicesPerElementCount);
                    //will we still have room to store coming indices from this mesh ?
                    //note: this is tied to the policy described above in (*)
                    size_t currentSize = subMesh->indexToRemappedIndex.size();
                    if ((currentSize + indicesPerElementCount) < maxiumIndicesCount) {
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
            
            allNextPrimitiveIndices[i] = nextPrimitiveIndex;

            if (targetIndicesCount > 0) {
                //FIXME: here targetIndices takes too much memory
                //To avoid this we would need to make a smaller copy.
                //In our case not sure if that's really a problem since this buffer won't be around for too long, as each buffer is deallocated once the callback from OpenCOLLADA to handle geomery has completed.
                
                shared_ptr <GLTFBufferView> targetBufferView = createBufferViewWithAllocatedBuffer(targetIndicesPtr, 0,targetIndicesCount * sizeof(unsigned int), true);
                
                shared_ptr <GLTFIndices> indices(new GLTFIndices(targetBufferView, targetIndicesCount));
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
        
        return true;
    }

}
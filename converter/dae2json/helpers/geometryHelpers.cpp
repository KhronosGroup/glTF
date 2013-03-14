#include "JSONExport.h"
#include <stdio.h>
#include <stdlib.h>
#include "geometryHelpers.h"

using namespace rapidjson;
using namespace std::tr1;
using namespace std;

namespace JSONExport
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
    
    std::string _KeyWithSemanticAndSet(JSONExport::Semantic semantic, unsigned int indexSet)
    {
        std::string semanticIndexSetKey = "";
        semanticIndexSetKey += "semantic";
        semanticIndexSetKey += JSONUtils::toString(semantic);
        semanticIndexSetKey += ":indexSet";
        semanticIndexSetKey += JSONUtils::toString(indexSet);
        
        return semanticIndexSetKey;
    }
    
    //---- JSONPrimitiveRemapInfos -------------------------------------------------------------
    
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
    
    //FIXME: this could be just an intermediate anonymous(no id) JSONBuffer
    class JSONPrimitiveRemapInfos
    {
    public:
        JSONPrimitiveRemapInfos(unsigned int* generatedIndices, unsigned int generatedIndicesCount, unsigned int *originalCountAndIndexes);
        virtual ~JSONPrimitiveRemapInfos();
        
        unsigned int generatedIndicesCount();
        unsigned int* generatedIndices();
        unsigned int* originalCountAndIndexes();
        
    private:
        unsigned int _generatedIndicesCount;
        unsigned int* _generatedIndices;
        unsigned int* _originalCountAndIndexes;
    };
    
    //---- JSONPrimitiveRemapInfos -------------------------------------------------------------
    JSONPrimitiveRemapInfos::JSONPrimitiveRemapInfos(unsigned int* generatedIndices, unsigned int generatedIndicesCount, unsigned int *originalCountAndIndexes):
    _generatedIndicesCount(generatedIndicesCount),
    _generatedIndices(generatedIndices),
    _originalCountAndIndexes(originalCountAndIndexes)
    {
    }
    
    JSONPrimitiveRemapInfos::~JSONPrimitiveRemapInfos()
    {
        if (this->_generatedIndices)
            free(this->_generatedIndices);
        if (this->_originalCountAndIndexes)
            free(this->_originalCountAndIndexes);
    }
    
    unsigned int JSONPrimitiveRemapInfos::generatedIndicesCount()
    {
        return _generatedIndicesCount;
    }
    
    unsigned int* JSONPrimitiveRemapInfos::generatedIndices()
    {
        return _generatedIndices;
    }
    
    unsigned int* JSONPrimitiveRemapInfos::originalCountAndIndexes()
    {
        return _originalCountAndIndexes;
    }
    
    typedef struct {
        unsigned char* remappedBufferData;
        //size_t remappedAccessorByteOffset;
        size_t remappedAccessorByteStride;
        
        unsigned char* originalBufferData;
        //size_t originalAccessorByteOffset;
        size_t originalAccessorByteStride;
        
        size_t elementByteLength;
        
    } AccessorsBufferInfos;
    
    static AccessorsBufferInfos* createAccessorsBuffersInfos(AccessorVector allOriginalAccessors ,AccessorVector allRemappedAccessors, unsigned int*indicesInRemapping, unsigned int count)
    {
        AccessorsBufferInfos* allBufferInfos = (AccessorsBufferInfos*)malloc(count * sizeof(AccessorsBufferInfos));
        for (size_t accessorIndex = 0 ; accessorIndex < count; accessorIndex++) {
            AccessorsBufferInfos *bufferInfos = &allBufferInfos[accessorIndex];
            
            shared_ptr <JSONExport::JSONAccessor> remappedAccessor = allRemappedAccessors[indicesInRemapping[accessorIndex]];
            shared_ptr <JSONExport::JSONAccessor> originalAccessor = allOriginalAccessors[indicesInRemapping[accessorIndex]];
            
            if (originalAccessor->getVertexAttributeByteLength() != remappedAccessor->getVertexAttributeByteLength()) {
                // FIXME : report error
                free(allBufferInfos);
                return 0;
            }
            
            bufferInfos->remappedBufferData = (unsigned char*)remappedAccessor->getBufferView()->getBufferDataByApplyingOffset();
            bufferInfos->remappedAccessorByteStride = remappedAccessor->getByteStride();
            
            bufferInfos->originalBufferData = (unsigned char*)originalAccessor->getBufferView()->getBufferDataByApplyingOffset();;
            bufferInfos->originalAccessorByteStride = originalAccessor->getByteStride();
            
            bufferInfos->elementByteLength = remappedAccessor->getVertexAttributeByteLength();
        }
        
        return allBufferInfos;
    }
    
    bool __RemapPrimitiveVertices(shared_ptr<JSONExport::JSONPrimitive> primitive,
                                  std::vector< shared_ptr<JSONExport::JSONIndices> > allIndices,
                                  AccessorVector allOriginalAccessors,
                                  AccessorVector allRemappedAccessors,
                                  unsigned int* indicesInRemapping,
                                  shared_ptr<JSONExport::JSONPrimitiveRemapInfos> primitiveRemapInfos)
    {
        size_t indicesSize = allIndices.size();
        if (allOriginalAccessors.size() < indicesSize) {
            //TODO: assert & inconsistency check
        }
        
        unsigned int vertexAttributesCount = (unsigned int)indicesSize;
        
        //get the primitive infos to know where we need to "go" for remap
        unsigned int count = primitiveRemapInfos->generatedIndicesCount();
        unsigned int* indices = primitiveRemapInfos->generatedIndices();
        
        AccessorsBufferInfos *allBufferInfos = createAccessorsBuffersInfos(allOriginalAccessors , allRemappedAccessors, indicesInRemapping, vertexAttributesCount);
        
        unsigned int* uniqueIndicesBuffer = (unsigned int*)primitive->getIndices()->getBufferView()->getBufferDataByApplyingOffset();
        
        unsigned int *originalCountAndIndexes = primitiveRemapInfos->originalCountAndIndexes();
        
        for (unsigned int k = 0 ; k < count ; k++) {
            unsigned int idx = indices[k];
            unsigned int* remappedIndex = &originalCountAndIndexes[idx * (allOriginalAccessors.size() + 1)];
            
            for (size_t accessorIndex = 0 ; accessorIndex < vertexAttributesCount  ; accessorIndex++) {
                AccessorsBufferInfos *bufferInfos = &allBufferInfos[accessorIndex];
                void *ptrDst = bufferInfos->remappedBufferData + (uniqueIndicesBuffer[idx] * bufferInfos->remappedAccessorByteStride);
                void *ptrSrc = (unsigned char*)bufferInfos->originalBufferData + (remappedIndex[1 /* skip count */ + indicesInRemapping[accessorIndex]] * bufferInfos->originalAccessorByteStride);
                //FIXME: optimize / secure this a bit, too many indirections without testing for invalid pointers
                /* copy the vertex attributes at the right offset and right indice (using the generated uniqueIndexes table */
                memcpy(ptrDst, ptrSrc , bufferInfos->elementByteLength);
            }
        }
        
        if (allBufferInfos)
            free(allBufferInfos);
        
        return true;
    }
    
    
    
    shared_ptr<JSONExport::JSONPrimitiveRemapInfos> __BuildPrimitiveUniqueIndexes(shared_ptr<JSONExport::JSONPrimitive> primitive,
                                                                                  std::vector< shared_ptr<JSONExport::JSONIndices> > allIndices,
                                                                                  RemappedMeshIndexesHashmap& remappedMeshIndexesMap,
                                                                                  unsigned int* indicesInRemapping,
                                                                                  unsigned int startIndex,
                                                                                  unsigned int accessorsCount,
                                                                                  unsigned int &endIndex)
    {
        size_t allIndicesSize = allIndices.size();
        size_t vertexAttributeCount = allIndices[0]->getCount();
        
        unsigned int generatedIndicesCount = 0;
        unsigned int vertexAttributesCount = accessorsCount;
        size_t sizeOfRemappedIndex = (vertexAttributesCount + 1) * sizeof(unsigned int);
        
        unsigned int* originalCountAndIndexes = (unsigned int*)calloc( (vertexAttributeCount * sizeOfRemappedIndex), sizeof(unsigned char));
        //this is useful for debugging.
        
        unsigned int *uniqueIndexes = (unsigned int*)calloc( vertexAttributeCount * sizeof(unsigned int), 1);
        unsigned int *generatedIndices = (unsigned int*) calloc (vertexAttributeCount * sizeof(unsigned int) , 1); //owned by PrimitiveRemapInfos
        unsigned int currentIndex = startIndex;
        
        for (size_t k = 0 ; k < vertexAttributeCount ; k++) {
            unsigned int* remappedIndex = &originalCountAndIndexes[k * (vertexAttributesCount + 1)];
            
            remappedIndex[0] = vertexAttributesCount;
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
        shared_ptr <JSONExport::JSONPrimitiveRemapInfos> primitiveRemapInfos(new JSONExport::JSONPrimitiveRemapInfos(generatedIndices, generatedIndicesCount, originalCountAndIndexes));
        shared_ptr <JSONExport::JSONBufferView> indicesBufferView = createBufferViewWithAllocatedBuffer(uniqueIndexes, 0, vertexAttributeCount * sizeof(unsigned int), true);
        
        shared_ptr <JSONExport::JSONIndices> indices = shared_ptr <JSONExport::JSONIndices> (new JSONExport::JSONIndices(indicesBufferView, vertexAttributeCount));
        
        primitive->setIndices(indices);
        
        return primitiveRemapInfos;
    }
    
    
    shared_ptr <JSONMesh> CreateUnifiedIndexesMeshFromMesh(JSONMesh *sourceMesh, std::vector< shared_ptr<IndicesVector> > &vectorOfIndicesVector)
    {
        AccessorVector originalAccessors;
        AccessorVector remappedAccessors;
        shared_ptr <JSONMesh> targetMesh(new JSONMesh(*sourceMesh));
        
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
        
        //in originalAccessors we'll get the flattened list of all the accessors as a vector.
        //fill semanticAndSetToIndex with key: (semantic, indexSet) value: index in originalAccessors vector.
        vector <JSONExport::Semantic> allSemantics = sourceMesh->allSemantics();
        std::map<string, unsigned int> semanticAndSetToIndex;
        
        for (unsigned int i = 0 ; i < allSemantics.size() ; i++) {
            IndexSetToAccessorHashmap& indexSetToAccessor = sourceMesh->getAccessorsForSemantic(allSemantics[i]);
            IndexSetToAccessorHashmap::const_iterator accessorIterator;
            for (accessorIterator = indexSetToAccessor.begin() ; accessorIterator != indexSetToAccessor.end() ; accessorIterator++) {
                //(*it).first;             // the key value (of type Key)
                //(*it).second;            // the mapped value (of type T)
                shared_ptr <JSONExport::JSONAccessor> selectedAccessor = (*accessorIterator).second;
                unsigned int indexSet = (*accessorIterator).first;
                JSONExport::Semantic semantic = allSemantics[i];
                std::string semanticIndexSetKey = _KeyWithSemanticAndSet(semantic, indexSet);
                unsigned int size = (unsigned int)originalAccessors.size();
                semanticAndSetToIndex[semanticIndexSetKey] = size;
                
                originalAccessors.push_back(selectedAccessor);
            }
        }
        
        maxVertexAttributes = (unsigned int)originalAccessors.size();
        
        vector <shared_ptr<JSONExport::JSONPrimitiveRemapInfos> > allPrimitiveRemapInfos;
        
        //build a array that maps the accessors that the indices points to with the index of the indice.
        JSONExport::RemappedMeshIndexesHashmap remappedMeshIndexesMap;
        for (unsigned int i = 0 ; i < primitiveCount ; i++) {
            shared_ptr<IndicesVector>  allIndicesSharedPtr = vectorOfIndicesVector[i];
            IndicesVector *allIndices = allIndicesSharedPtr.get();
            unsigned int* indicesInRemapping = (unsigned int*)malloc(sizeof(unsigned int) * allIndices->size());
            
            
            VertexAttributeVector vertexAttributes = sourcePrimitives[i]->getVertexAttributes();
            for (unsigned int k = 0 ; k < allIndices->size() ; k++) {
                JSONExport::Semantic semantic = vertexAttributes[k]->getSemantic();
                unsigned int indexSet = vertexAttributes[k]->getIndexOfSet();
                std::string semanticIndexSetKey = _KeyWithSemanticAndSet(semantic, indexSet);
                unsigned int idx = semanticAndSetToIndex[semanticIndexSetKey];
                indicesInRemapping[k] = idx;
            }
            
            shared_ptr<JSONExport::JSONPrimitiveRemapInfos> primitiveRemapInfos = __BuildPrimitiveUniqueIndexes(targetPrimitives[i], *allIndices, remappedMeshIndexesMap, indicesInRemapping, startIndex, maxVertexAttributes, endIndex);
            
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
            IndexSetToAccessorHashmap& indexSetToAccessor = sourceMesh->getAccessorsForSemantic(allSemantics[i]);
            IndexSetToAccessorHashmap& destinationIndexSetToAccessor = targetMesh->getAccessorsForSemantic(allSemantics[i]);
            IndexSetToAccessorHashmap::const_iterator accessorIterator;
            
            //FIXME: consider turn this search into a method for mesh
            for (accessorIterator = indexSetToAccessor.begin() ; accessorIterator != indexSetToAccessor.end() ; accessorIterator++) {
                //(*it).first;             // the key value (of type Key)
                //(*it).second;            // the mapped value (of type T)
                shared_ptr <JSONExport::JSONAccessor> selectedAccessor = (*accessorIterator).second;
                
                size_t sourceSize = vertexCount * selectedAccessor->getVertexAttributeByteLength();
                void* sourceData = malloc(sourceSize);
                
                shared_ptr <JSONBufferView> referenceBufferView = selectedAccessor->getBufferView();
                shared_ptr <JSONBufferView> remappedBufferView = createBufferViewWithAllocatedBuffer(referenceBufferView->getID(), sourceData, 0, sourceSize, true);

                shared_ptr <JSONAccessor> remappedAccessor(new JSONAccessor(selectedAccessor.get()));
                remappedAccessor->setBufferView(remappedBufferView);
                remappedAccessor->setCount(vertexCount);
                
                destinationIndexSetToAccessor[(*accessorIterator).first] = remappedAccessor;
                
                remappedAccessors.push_back(remappedAccessor);
            }
        }
        
        /*
         if (_allOriginalAccessors.size() != allIndices.size()) {
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
                JSONExport::Semantic semantic = vertexAttributes[k]->getSemantic();
                unsigned int indexSet = vertexAttributes[k]->getIndexOfSet();
                std::string semanticIndexSetKey = _KeyWithSemanticAndSet(semantic, indexSet);
                unsigned int idx = semanticAndSetToIndex[semanticIndexSetKey];
                indicesInRemapping[k] = idx;
            }
            
            bool status = __RemapPrimitiveVertices(targetPrimitives[i],
                                                   (*allIndices),
                                                   originalAccessors ,
                                                   remappedAccessors,
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
        shared_ptr <JSONMesh> targetMesh;
        //For each sub mesh being built, maintain 2 maps,
        //with key:indice value: remapped indice
        IndicesMap indexToRemappedIndex;
    } ;

    SubMeshContext* __CreateSubMeshContext()
    {
        SubMeshContext *subMesh = new SubMeshContext();
        shared_ptr <JSONMesh> targetMesh = shared_ptr <JSONMesh> (new JSONMesh());
        subMesh->targetMesh = targetMesh;
        
        return subMesh;
    }
    
    void __PushAndRemapIndicesInSubMesh(SubMeshContext *subMesh, unsigned int* indices, int count)
    {
        for (unsigned int i = 0 ; i < count ; i++) {
            unsigned int index = indices[i];
            
            bool shouldRemap = subMesh->indexToRemappedIndex.count(index) == 0;
            if (shouldRemap) {
                unsigned int remappedIndex = subMesh->indexToRemappedIndex.size();
                
                subMesh->indexToRemappedIndex[index] = remappedIndex;
            } 
        }
    }
    
    static void __RemapAccessor(void *value,
                          JSONExport::ElementType type,
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
    void __RemapSubMesh(SubMeshContext *subMesh, JSONMesh *sourceMesh)
    {
        //remap the subMesh using the original mesh
        //we walk through all accessors
        vector <JSONExport::Semantic> allSemantics = sourceMesh->allSemantics();
        std::map<string, unsigned int> semanticAndSetToIndex;
        
        for (unsigned int i = 0 ; i < allSemantics.size() ; i++) {
            IndexSetToAccessorHashmap& indexSetToAccessor = sourceMesh->getAccessorsForSemantic(allSemantics[i]);
            IndexSetToAccessorHashmap& targetIndexSetToAccessor = subMesh->targetMesh->getAccessorsForSemantic(allSemantics[i]);
            IndexSetToAccessorHashmap::const_iterator accessorIterator;
            for (accessorIterator = indexSetToAccessor.begin() ; accessorIterator != indexSetToAccessor.end() ; accessorIterator++) {
                //(*it).first;             // the key value (of type Key)
                //(*it).second;            // the mapped value (of type T)
                shared_ptr <JSONAccessor> selectedAccessor = (*accessorIterator).second;
                unsigned int indexSet = (*accessorIterator).first;
                
                shared_ptr <JSONBufferView> referenceBufferView = selectedAccessor->getBufferView();
                
                unsigned int vertexAttributeCount = subMesh->indexToRemappedIndex.size();
                
                //FIXME: this won't work with interleaved
                unsigned int *targetBufferPtr = (unsigned int*)malloc(selectedAccessor->getVertexAttributeByteLength() * vertexAttributeCount);
                
                void *context[2];
                context[0] = targetBufferPtr;
                context[1] = subMesh;
                selectedAccessor->apply(__RemapAccessor, (void*)context);
                                        
                shared_ptr <JSONBufferView> remappedBufferView =
                createBufferViewWithAllocatedBuffer(referenceBufferView->getID(), targetBufferPtr, 0, selectedAccessor->getVertexAttributeByteLength() * vertexAttributeCount, true);
                
                shared_ptr <JSONAccessor> remappedAccessor(new JSONExport::JSONAccessor(selectedAccessor.get()));
                remappedAccessor->setBufferView(remappedBufferView);
                remappedAccessor->setCount(vertexAttributeCount);
                
                targetIndexSetToAccessor[indexSet] = remappedAccessor;
            }
        }
    }
    
    bool CreateMeshesWithMaximumIndicesCountFromMeshIfNeeded(JSONMesh *sourceMesh, unsigned int maxiumIndicesCount, MeshVector &meshes)
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
        for (int i = 0 ; i < primitives.size() ; i++) {
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
                    meshID += "-"+ JSONUtils::toString(meshIndex);
                    meshName += "-"+ JSONUtils::toString(meshIndex);
                }
                subMesh->targetMesh->setID(meshID);
                subMesh->targetMesh->setName(meshName);
                
                stillHavePrimitivesElementsToBeProcessed = false;
                meshIndex++;
            }
            
            shared_ptr <JSONPrimitive> targetPrimitive;
            //when we are done with a primitive we mark its nextIndice with a -1

            targetPrimitive = shared_ptr <JSONPrimitive> (new JSONPrimitive((*primitives[i])));
            
            unsigned int nextPrimitiveIndex = (unsigned int)allNextPrimitiveIndices[i];
            
            shared_ptr<JSONPrimitive> &primitive = primitives[i];
            shared_ptr<JSONIndices> indices = primitive->getIndices();
            
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
                
                shared_ptr <JSONBufferView> targetBufferView = createBufferViewWithAllocatedBuffer(targetIndicesPtr, 0,targetIndicesCount * sizeof(unsigned int), true);
                
                shared_ptr <JSONIndices> indices(new JSONIndices(targetBufferView, targetIndicesCount));
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
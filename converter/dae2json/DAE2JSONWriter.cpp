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

// TODO: check for '-' in JSON keys
// TODO: consider LOD
// TODO: normal generation when needed
// DESIGN: justification generation shader. geometry may need regeneration if lambert
// DESIGN: difference between COLLADA and JSON format. In JSON format it is possible to make an interleaved array not only made by float.
// reminder; to run recursively against all dae files: find . -name '*.dae' -exec dae2json {} \;

#include <algorithm>

#include "JSONExport.h"
#include "DAE2JSONWriter.h"
#include "COLLADAFWPolygons.h"
#include "shaders/commonProfileShaders.h"
#include "helpers/geometryHelpers.h"

namespace JSONExport
{    
    std::string uniqueIdWithType(std::string type, const UniqueId& uniqueId) 
    {
        std::string id = "";        
        id += type + "_" + JSONExport::JSONUtils::toString(uniqueId.getObjectId());
        return id;
    }
    
    //converted to C++ from gl-matrix by Brandon Jones ( https://github.com/toji/gl-matrix )
    void buildLookAtMatrix(Lookat *lookat, COLLADABU::Math::Matrix4& matrix)
    {
        assert(lookat);
        
        const COLLADABU::Math::Vector3& eye = lookat->getEyePosition();
        const COLLADABU::Math::Vector3& center = lookat->getInterestPointPosition();
        const COLLADABU::Math::Vector3& up = lookat->getUpAxisDirection();
        
        if ((eye.x == center.x) && (eye.y == center.y) && (eye.z == center.z)) {
            matrix = COLLADABU::Math::Matrix4::IDENTITY;
            return;
        }
        
        COLLADABU::Math::Vector3 z = (eye - center);
        z.normalise(); // TODO: OpenCOLLADA typo should be normalize (with a z).
        COLLADABU::Math::Vector3 x = up.crossProduct(z);
        x.normalise();
        COLLADABU::Math::Vector3 y = z.crossProduct(x);
        y.normalise();
                
        matrix.setAllElements(x.x,
                              y.x,
                              z.x,
                              0,
                              x.y,
                              y.y,
                              z.y,
                              0,
                              x.z,
                              y.z,
                              z.z,
                              0,
                              -(x.x * eye.x + x.y  * eye.y + x.z * eye.z),
                              -(y.x * eye.x + y.y * eye.y + y.z * eye.z),
                              -(z.x * eye.x + z.y * eye.y + z.z * eye.z),
                              1);
        matrix = matrix.inverse();        
        matrix = matrix.transpose();                
    }
    
    BBOX::BBOX() {
        this->_min = COLLADABU::Math::Vector3(DBL_MAX, DBL_MAX, DBL_MAX);
        this->_max = COLLADABU::Math::Vector3(DBL_MIN, DBL_MIN, DBL_MIN);
    }
    
    BBOX::BBOX(const COLLADABU::Math::Vector3 &min, const COLLADABU::Math::Vector3 &max) {
        this->_min = min;
        this->_max = max;
    }
    
    const COLLADABU::Math::Vector3&  BBOX::getMin3() {
        return this->_min;
    }

    const COLLADABU::Math::Vector3& BBOX::getMax3() {
        return this->_max;
    }

    void BBOX::merge(BBOX* bbox) {
        this->_min.makeFloor(bbox->getMin3());
        this->_max.makeCeil(bbox->getMax3());
    }
    
    void BBOX::transform(const COLLADABU::Math::Matrix4& mat4) {
        COLLADABU::Math::Vector3 min = COLLADABU::Math::Vector3(DBL_MAX, DBL_MAX, DBL_MAX);
        COLLADABU::Math::Vector3 max = COLLADABU::Math::Vector3(DBL_MIN, DBL_MIN, DBL_MIN);
        
        COLLADABU::Math::Vector3 pt0 = mat4 * COLLADABU::Math::Vector3(this->_min.x, this->_min.y, this->_min.z);
        COLLADABU::Math::Vector3 pt1 = mat4 * COLLADABU::Math::Vector3(this->_max.x, this->_min.y, this->_min.z);
        COLLADABU::Math::Vector3 pt2 = mat4 * COLLADABU::Math::Vector3(this->_min.x, this->_max.y, this->_min.z);
        COLLADABU::Math::Vector3 pt3 = mat4 * COLLADABU::Math::Vector3(this->_max.x, this->_max.y, this->_min.z);
        COLLADABU::Math::Vector3 pt4 = mat4 * COLLADABU::Math::Vector3(this->_min.x, this->_min.y, this->_max.z);
        COLLADABU::Math::Vector3 pt5 = mat4 * COLLADABU::Math::Vector3(this->_max.x, this->_min.y, this->_max.z);
        COLLADABU::Math::Vector3 pt6 = mat4 * COLLADABU::Math::Vector3(this->_min.x, this->_max.y, this->_max.z);
        COLLADABU::Math::Vector3 pt7 = mat4 * COLLADABU::Math::Vector3(this->_max.x, this->_max.y, this->_max.z);
        
        min.makeFloor(pt0); max.makeCeil(pt0);
        min.makeFloor(pt1); max.makeCeil(pt1);
        min.makeFloor(pt2); max.makeCeil(pt2);
        min.makeFloor(pt3); max.makeCeil(pt3);
        min.makeFloor(pt4); max.makeCeil(pt4);
        min.makeFloor(pt5); max.makeCeil(pt5);
        min.makeFloor(pt6); max.makeCeil(pt6);
        min.makeFloor(pt7); max.makeCeil(pt7);
        
        this->_min = min;
        this->_max = max;
    }
        
    //---- Convert OpenCOLLADA mesh to mesh -------------------------------------------
  
    static unsigned int ConvertOpenCOLLADAMeshVertexDataToJSONAccessors(const COLLADAFW::MeshVertexData &vertexData, JSONExport::IndexSetToAccessorHashmap &accessors) 
    {
        // The following are OpenCOLLADA fmk issues preventing doing a totally generic processing of sources
        //1. "set"(s) other than texCoord don't have valid input infos
        //2. not the original id in the source
        
        std::string name; 
        size_t length, elementsCount;
        size_t stride = 0;
        size_t size = 0;
        
        size_t setCount = vertexData.getNumInputInfos();    
        bool unpatchedOpenCOLLADA = (setCount == 0); // reliable heuristic to know if the input have not been set
        
        if (unpatchedOpenCOLLADA)
            setCount = 1;
        
        for (size_t indexOfSet = 0 ; indexOfSet < setCount ; indexOfSet++) {
            
            if (!unpatchedOpenCOLLADA) {
                name = vertexData.getName(indexOfSet);
                size = vertexData.getStride(indexOfSet);
            } else {
                // for unpatched version of OpenCOLLADA we need this work-around.
                name = JSONExport::JSONUtils::generateIDForType("buffer").c_str();
                size = 3; //only normal and positions should reach this code
            }
                    
            //name is the id
            length = vertexData.getValuesCount();
            elementsCount = length / size;
            void *sourceData = 0;
            size_t sourceSize = 0;
        
            JSONExport::ElementType elementType = JSONExport::NOT_AN_ELEMENT_TYPE;
            switch (vertexData.getType()) {
                case MeshVertexData::DATA_TYPE_FLOAT: {
                    elementType = JSONExport::FLOAT;
                    stride = sizeof(float) * size;
                    const FloatArray& array = vertexData.getFloatValues()[indexOfSet];
                    const size_t count = array.getCount();
                    sourceData = (void*)array.getData();
                    sourceSize = count * sizeof(float);           
                }
                break;
                case MeshVertexData::DATA_TYPE_DOUBLE: {
                    /*
                        sourceType = DOUBLE;
                
                        const DoubleArray& array = vertexData.getDoubleValues()[indexOfSet];                
                        const size_t count = array.getCount();
                        sourceData = (void*)array.getData();
                        sourceSize = count * sizeof(double); 
                     */
                    // Warning if can't make "safe" conversion
                }
                    
                    break;
                default:
                case MeshVertexData::DATA_TYPE_UNKNOWN:
                    //FIXME report error
                break;
            }
        
            // FIXME: the source could be shared, store / retrieve it here
            shared_ptr <JSONExport::JSONBuffer> cvtBuffer(new JSONExport::JSONBuffer(name, sourceData, sourceSize, false));
            shared_ptr <JSONExport::JSONAccessor> cvtAccessor(new JSONExport::JSONAccessor());
        
            cvtAccessor->setBuffer(cvtBuffer);
            cvtAccessor->setElementsPerVertexAttribute(size);
            cvtAccessor->setByteStride(stride);
            cvtAccessor->setElementType(elementType);
            cvtAccessor->setCount(elementsCount);
            
            accessors[(unsigned int)indexOfSet] = cvtAccessor;
        }
        
        return (unsigned int)setCount;
    }
    
    static void __AppendIndices(shared_ptr <JSONExport::JSONPrimitive> &primitive, IndicesVector &primitiveIndicesVector, shared_ptr <JSONExport::JSONIndices> &indices, JSONExport::Semantic semantic, unsigned int indexOfSet)
    {
        primitive->appendVertexAttribute(shared_ptr <JSONExport::JSONVertexAttribute>( new JSONExport::JSONVertexAttribute(semantic,indexOfSet)));
        primitiveIndicesVector.push_back(indices);
    }
    
    static shared_ptr <JSONExport::JSONPrimitive> ConvertOpenCOLLADAMeshPrimitive(
        COLLADAFW::MeshPrimitive *openCOLLADAMeshPrimitive,
        IndicesVector &primitiveIndicesVector)
    {
        shared_ptr <JSONExport::JSONPrimitive> cvtPrimitive(new JSONExport::JSONPrimitive());
        
        // We want to match OpenGL/ES mode , as WebGL spec points to OpenGL/ES spec...
        // "Symbolic constants GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, and GL_TRIANGLES are accepted."
        std::string type;
        bool shouldTriangulate = false;
        
        switch(openCOLLADAMeshPrimitive->getPrimitiveType()) {
                //these 2 requires transforms
            case MeshPrimitive::POLYLIST:
            case MeshPrimitive::POLYGONS:
                // FIXME: perform conversion, but until not done report error
                //these mode are supported by WebGL
                shouldTriangulate = true;
                //force triangles
                type = "TRIANGLES";
                break;
            case  MeshPrimitive::LINES:
                type = "LINES";
                break;
            case  MeshPrimitive::LINE_STRIPS:
                type = "LINE_STRIP";
                break;
                
            case  MeshPrimitive::TRIANGLES:
                type = "TRIANGLES";
                break;
                
            case  MeshPrimitive::TRIANGLE_FANS:
                type = "TRIANGLE_FANS";
                break;
                
            case  MeshPrimitive::TRIANGLE_STRIPS:
                type = "TRIANGLE_STRIPS";
                break;
                
            case  MeshPrimitive::POINTS:
                type = "POINTS";
                break;
            default:
                break;
        }

        cvtPrimitive->setMaterialObjectID((unsigned int)openCOLLADAMeshPrimitive->getMaterialId());
        cvtPrimitive->setType(type);
        
        //count of indices , it must be the same for all kind of indices                
        size_t count = openCOLLADAMeshPrimitive->getPositionIndices().getCount();
        
        //vertex
        //IndexList &positionIndexList = openCOLLADAMeshPrimitive->getPositionIndices();
        unsigned int *indices = openCOLLADAMeshPrimitive->getPositionIndices().getData();
        unsigned int *verticesCountArray = 0;
        unsigned int vcount = 0;   //count of elements in the array containing the count of indices per polygon & polylist.
        unsigned int triangulatedIndicesCount = 0;
        
        if (shouldTriangulate) {
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
        
        shared_ptr <JSONExport::JSONBuffer> positionBuffer(new JSONExport::JSONBuffer(indices, count * sizeof(unsigned int), shouldTriangulate ? true : false));
        
        shared_ptr <JSONExport::JSONIndices> positionIndices(new JSONExport::JSONIndices(positionBuffer,count));
        
        __AppendIndices(cvtPrimitive, primitiveIndicesVector, positionIndices, VERTEX, 0);
        
        if (openCOLLADAMeshPrimitive->hasNormalIndices()) {
            indices = openCOLLADAMeshPrimitive->getNormalIndices().getData();
            if (shouldTriangulate) {
                indices = createTrianglesFromPolylist(verticesCountArray, indices, vcount, &triangulatedIndicesCount);
                count = triangulatedIndicesCount;
            }
            
            shared_ptr <JSONExport::JSONBuffer> normalBuffer(new JSONExport::JSONBuffer(indices, count * sizeof(unsigned int), shouldTriangulate ? true : false));
            shared_ptr <JSONExport::JSONIndices> normalIndices(new JSONExport::JSONIndices(normalBuffer,
                                                                                           count));
            __AppendIndices(cvtPrimitive, primitiveIndicesVector, normalIndices, NORMAL, 0);
        }
        
        if (openCOLLADAMeshPrimitive->hasColorIndices()) {
            IndexListArray& colorListArray = openCOLLADAMeshPrimitive->getColorIndicesArray();
            
            for (size_t i = 0 ; i < colorListArray.getCount() ; i++) {
                IndexList* indexList = openCOLLADAMeshPrimitive->getColorIndices(i);
                
                indices = indexList->getIndices().getData();
                
                if (shouldTriangulate) {
                    indices = createTrianglesFromPolylist(verticesCountArray, indices, vcount, &triangulatedIndicesCount);
                    count = triangulatedIndicesCount;
                }
                
                shared_ptr <JSONExport::JSONBuffer> colorBuffer(new JSONExport::JSONBuffer(indices, count * sizeof(unsigned int), shouldTriangulate ? true : false));
                
                shared_ptr <JSONExport::JSONIndices> colorIndices(new JSONExport::JSONIndices(colorBuffer,
                                                                                              count));
                __AppendIndices(cvtPrimitive, primitiveIndicesVector, colorIndices, COLOR, indexList->getSetIndex());
            }
        }
        
        if (openCOLLADAMeshPrimitive->hasUVCoordIndices()) {                        
            IndexListArray& uvListArray = openCOLLADAMeshPrimitive->getUVCoordIndicesArray();
            
            for (size_t i = 0 ; i < uvListArray.getCount() ; i++) {
                IndexList* indexList = openCOLLADAMeshPrimitive->getUVCoordIndices(i);
                                
                indices = indexList->getIndices().getData();
                
                if (shouldTriangulate) {
                    indices = createTrianglesFromPolylist(verticesCountArray, indices, vcount, &triangulatedIndicesCount);
                    count = triangulatedIndicesCount;
                }
                
                shared_ptr <JSONExport::JSONBuffer> uvBuffer(new JSONExport::JSONBuffer(indices, count * sizeof(unsigned int), shouldTriangulate ? true : false)); 
                
                //FIXME: Looks like for texcoord indexSet begin at 1, this is out of the sync with the index used in ConvertOpenCOLLADAMeshVertexDataToJSONAccessors that begins at 0
                //for now forced to 0, to be fixed for multi texturing.
                unsigned int idx = 0; //(unsigned int)indexList->getSetIndex();                
                
                shared_ptr <JSONExport::JSONIndices> uvIndices(new JSONExport::JSONIndices(uvBuffer, count));
                        
                __AppendIndices(cvtPrimitive, primitiveIndicesVector, uvIndices, TEXCOORD, idx);
            }
        }
        
        if (verticesCountArray) {
            free(verticesCountArray);
        }
        
        return cvtPrimitive;
    }
    
    static void __InvertV(void *value,
                          JSONExport::ElementType type,
                          size_t elementsPerVertexAttribute,
                          size_t index,
                          size_t vertexAttributeByteSize,
                          void *context) {
        char* bufferData = (char*)value;
        
        if (elementsPerVertexAttribute > 1) {
            switch (type) {
                case JSONExport::FLOAT: {
                    float* vector = (float*)bufferData;
                    vector[1] = (float) (1.0 - vector[1]);
                }
                    break;
                default:
                    break;
            }
        }
    }
    
    void ConvertOpenCOLLADAMesh(COLLADAFW::Mesh* openCOLLADAMesh,
                                MeshVector &meshes)
    {
        shared_ptr <JSONExport::JSONMesh> cvtMesh(new JSONExport::JSONMesh());
                
        cvtMesh->setID(openCOLLADAMesh->getOriginalId());
        cvtMesh->setName(openCOLLADAMesh->getName());
        
        const MeshPrimitiveArray& primitives =  openCOLLADAMesh->getMeshPrimitives();
        size_t primitiveCount = primitives.getCount();
        
        std::vector< shared_ptr<IndicesVector> > allPrimitiveIndicesVectors;

        // get all primitives
        for (size_t i = 0 ; i < primitiveCount ; i++) {
            shared_ptr <JSONExport::IndicesVector> primitiveIndicesVector(new JSONExport::IndicesVector());
            allPrimitiveIndicesVectors.push_back(primitiveIndicesVector);

            shared_ptr <JSONExport::JSONPrimitive> primitive = ConvertOpenCOLLADAMeshPrimitive(primitives[i],*primitiveIndicesVector);
            if (primitive->getType() == "TRIANGLES") {
                cvtMesh->appendPrimitive(primitive);
            } else {
                //TODO: support lines...
                continue;
            }
            
            VertexAttributeVector vertexAttributes = primitive->getVertexAttributes();
            primitiveIndicesVector = allPrimitiveIndicesVectors[i];
            
            // once we got a primitive, keep track of its accessors
            std::vector< shared_ptr<JSONExport::JSONIndices> > allIndices = *primitiveIndicesVector;
            for (size_t k = 0 ; k < allIndices.size() ; k++) {
                shared_ptr<JSONExport::JSONIndices> indices = allIndices[k];
                JSONExport::Semantic semantic = vertexAttributes[k]->getSemantic();
                JSONExport::IndexSetToAccessorHashmap& accessors = cvtMesh->getAccessorsForSemantic(semantic);
                    
                switch (semantic) {
                    case JSONExport::VERTEX: 
                        ConvertOpenCOLLADAMeshVertexDataToJSONAccessors(openCOLLADAMesh->getPositions(), accessors);
                        break;
                        
                    case JSONExport::NORMAL: 
                        ConvertOpenCOLLADAMeshVertexDataToJSONAccessors(openCOLLADAMesh->getNormals(), accessors);
                        break;
                        
                    case JSONExport::TEXCOORD: 
                        ConvertOpenCOLLADAMeshVertexDataToJSONAccessors(openCOLLADAMesh->getUVCoords(), accessors);
                        break;

                    case JSONExport::COLOR: 
                        ConvertOpenCOLLADAMeshVertexDataToJSONAccessors(openCOLLADAMesh->getColors(), accessors);
                        break;
                            
                    default: 
                        break;
                }                   
                    
                cvtMesh->setAccessorsForSemantic(semantic, accessors);
            }
        }
        
        //https://github.com/KhronosGroup/collada2json/issues/41
        //Goes through all texcoord and invert V
        JSONExport::IndexSetToAccessorHashmap& texcoordAccessors = cvtMesh->getAccessorsForSemantic(JSONExport::TEXCOORD);
        JSONExport::IndexSetToAccessorHashmap::const_iterator accessorIterator;
        
        //FIXME: consider turn this search into a method for mesh
        for (accessorIterator = texcoordAccessors.begin() ; accessorIterator != texcoordAccessors.end() ; accessorIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            shared_ptr <JSONExport::JSONAccessor> accessor = (*accessorIterator).second;
            
            accessor->apply(__InvertV, NULL);
        }
        
         //After this point cvtMesh should be referenced anymore and will be deallocated
        shared_ptr <JSONExport::JSONMesh> unifiedMesh = CreateUnifiedIndexesMeshFromMesh(cvtMesh.get(), allPrimitiveIndicesVectors);
        
        if  (CreateMeshesWithMaximumIndicesCountFromMeshIfNeeded(unifiedMesh.get(), 65535, meshes) == false) {
            meshes.push_back(unifiedMesh);
        }
    }
        
    //--------------------------------------------------------------------
	DAE2JSONWriter::DAE2JSONWriter( const COLLADA2JSONContext &converterArgs, PrettyWriter <FileStream> *jsonWriter ):
    _converterContext(converterArgs),
     _visualScene(0)
	{
        this->_writer.setWriter(jsonWriter);
	}
    
	//--------------------------------------------------------------------
	DAE2JSONWriter::~DAE2JSONWriter()
	{
	}
    
	//--------------------------------------------------------------------
	void DAE2JSONWriter::reportError( const std::string& method, const std::string& message)
	{
	}
    
	//--------------------------------------------------------------------
	bool DAE2JSONWriter::write()
	{
        this->_converterContext.shaderIdToShaderString.clear();
        this->_uniqueIDToMeshes.clear();
        COLLADABU::URI inputURI(this->_converterContext.inputFilePath.c_str());
        COLLADABU::URI outputURI(this->_converterContext.outputFilePath.c_str());
        
        std::string sharedBufferID = inputURI.getPathFileBase() + ".bin";
        std::string outputFilePath = outputURI.getPathDir() + sharedBufferID;
        
        this->_fileOutputStream.open (outputFilePath.c_str(), ios::out | ios::ate | ios::binary);        
        //this->_fileOutputStream.seekp(-this->_fileOutputStream.tellp());
        
        this->_converterContext.root = shared_ptr <JSONExport::JSONObject> (new JSONExport::JSONObject());
        this->_converterContext.root->setString("version", "0.1");
        this->_converterContext.root->setValue("nodes", shared_ptr <JSONExport::JSONObject> (new JSONExport::JSONObject()));
                                
        COLLADASaxFWL::Loader loader;
		COLLADAFW::Root root(&loader, this);
         
		if (!root.loadDocument( this->_converterContext.inputFilePath))
			return false;
        
        shared_ptr <JSONExport::JSONBuffer> sharedBuffer(new JSONExport::JSONBuffer(sharedBufferID, static_cast<size_t>(this->_fileOutputStream.tellp())));
        
        UniqueIDToMeshes::const_iterator UniqueIDToMeshesIterator;

        // ----
        shared_ptr <JSONExport::JSONObject> meshesObject(new JSONExport::JSONObject());
        
        this->_converterContext.root->setValue("meshes", meshesObject);
        
        for (UniqueIDToMeshesIterator = this->_uniqueIDToMeshes.begin() ; UniqueIDToMeshesIterator != this->_uniqueIDToMeshes.end() ; UniqueIDToMeshesIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            MeshVectorSharedPtr meshes = (*UniqueIDToMeshesIterator).second;
            
            for (size_t j = 0 ; j < meshes->size() ; j++) {
                shared_ptr<JSONMesh> mesh = (*meshes)[j];
                if (mesh) {
                    shared_ptr <JSONExport::JSONObject> meshObject = serializeMesh(mesh.get(), (void*)sharedBuffer.get());
                    
                    meshesObject->setValue(mesh->getID(), meshObject);
                }
            }
        }
        
        // ----
        shared_ptr <JSONExport::JSONObject> materialsObject(new JSONExport::JSONObject());

        this->_converterContext.root->setValue("materials", materialsObject);

        UniqueIDToEffect::const_iterator UniqueIDToEffectIterator;
        
        for (UniqueIDToEffectIterator = this->_uniqueIDToEffect.begin() ; UniqueIDToEffectIterator != this->_uniqueIDToEffect.end() ; UniqueIDToEffectIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            shared_ptr <JSONExport::JSONEffect> effect = (*UniqueIDToEffectIterator).second;
            shared_ptr <JSONExport::JSONObject> effectObject = serializeEffect(effect.get(), 0);
            //FIXME:HACK: effects are exported as materials
            materialsObject->setValue(effect->getID(), effectObject);
        }

        // ----
        
        
        shared_ptr <JSONExport::JSONObject> buffersObject(new JSONExport::JSONObject());
        shared_ptr <JSONExport::JSONObject> bufferObject = serializeBuffer(sharedBuffer.get(), 0);

        this->_converterContext.root->setValue("buffers", buffersObject);
        buffersObject->setValue(sharedBufferID, bufferObject);
                
        //--- 
        this->_converterContext.root->write(&this->_writer);
                
        bool sceneFlatteningEnabled = false;
        if (sceneFlatteningEnabled) {
            //second pass to actually output the JSON of the node themselves (and for all sub nodes)
            processSceneFlatteningInfo(&this->_sceneFlatteningInfo);
        }
        
        this->_fileOutputStream.flush();
        this->_fileOutputStream.close();
        
		return true;
	}
    
	//--------------------------------------------------------------------
	void DAE2JSONWriter::cancel( const std::string& errorMessage )
	{
	}
    
	//--------------------------------------------------------------------
	void DAE2JSONWriter::start()
	{
	}
    
	//--------------------------------------------------------------------
	void DAE2JSONWriter::finish()
	{        
	}
    
	//--------------------------------------------------------------------
	bool DAE2JSONWriter::writeGlobalAsset( const COLLADAFW::FileInfo* asset )
	{
		return true;
	}
    
	//--------------------------------------------------------------------
    
    shared_ptr <JSONExport::JSONArray> DAE2JSONWriter::serializeMatrix4Array(const COLLADABU::Math::Matrix4 &matrix) 
    {
        shared_ptr <JSONExport::JSONArray> array(new JSONExport::JSONArray());
        
        COLLADABU::Math::Matrix4 transpose = matrix.transpose();

        for (int i = 0 ; i < 4 ; i++)  {
            const COLLADABU::Math::Real * real = transpose[i];
          
            array->appendValue(shared_ptr <JSONExport::JSONValue> (new JSONExport::JSONNumber((double)real[0])));           
            array->appendValue(shared_ptr <JSONExport::JSONValue> (new JSONExport::JSONNumber((double)real[1])));           
            array->appendValue(shared_ptr <JSONExport::JSONValue> (new JSONExport::JSONNumber((double)real[2])));           
            array->appendValue(shared_ptr <JSONExport::JSONValue> (new JSONExport::JSONNumber((double)real[3])));           
        }        

        return array;
    }

    float DAE2JSONWriter::getTransparency(const COLLADAFW::EffectCommon* effectCommon)
    {
        //super naive for now, also need to check sketchup work-around
        if (effectCommon->getOpacity().isTexture()) {
            return 1;
        }
        float transparency = static_cast<float>(effectCommon->getOpacity().getColor().getAlpha());
        
        return this->_converterContext.invertTransparency ? 1 - transparency : transparency;
    }
    
    float DAE2JSONWriter::isOpaque(const COLLADAFW::EffectCommon* effectCommon)
    {
        return getTransparency(effectCommon)  >= 1;
    }
    
    bool DAE2JSONWriter::writeNode( const COLLADAFW::Node* node, 
                                    shared_ptr <JSONExport::JSONObject> nodesObject, 
                                    COLLADABU::Math::Matrix4 parentMatrix,
                                    SceneFlatteningInfo* sceneFlatteningInfo) 
    {
        const NodePointerArray& nodes = node->getChildNodes();
        const std::string& originalID = uniqueIdWithType("node", node->getUniqueId());
        COLLADABU::Math::Matrix4 matrix = COLLADABU::Math::Matrix4::IDENTITY;
        
        shared_ptr <JSONExport::JSONObject> nodeObject(new JSONExport::JSONObject());
        nodeObject->setString("name",node->getName());
        
        bool nodeContainsLookAtTr = false;
        const InstanceCameraPointerArray& instanceCameras = node->getInstanceCameras();
        size_t camerasCount = instanceCameras.getCount();
        if (camerasCount > 0) {
            InstanceCamera* instanceCamera = instanceCameras[0];
            shared_ptr <JSONExport::JSONObject> cameraObject(new JSONExport::JSONObject());

            std::string cameraId = uniqueIdWithType("camera", instanceCamera->getInstanciatedObjectId());
            nodeObject->setString("camera", cameraId);
            
            //FIXME: just handle the first camera within a node now
            //for (size_t i = 0 ; i < camerasCount ; i++) {
            //}
            //Checks if we have a "look at" transformm because it is not handled by getTransformationMatrix when baking the matrix. (TODO: file a OpenCOLLADA issue for that).        
            const TransformationPointerArray& transformations = node->getTransformations();
            size_t transformationsCount = transformations.getCount();
            for (size_t i = 0 ; i < transformationsCount ; i++) {
                const Transformation* tr = transformations[i];
                if (tr->getTransformationType() == Transformation::LOOKAT) {
                    Lookat* lookAt = (Lookat*)tr;
                    buildLookAtMatrix(lookAt, matrix);
                    nodeContainsLookAtTr = true;
                    break;
                }
            }
                    
            if (nodeContainsLookAtTr && (transformationsCount > 1)) {
                //FIXME: handle warning/error
                printf("WARNING: node contains a look at transform combined with other transforms\n");
            }
        } 
        
        if (!nodeContainsLookAtTr) { 
            node->getTransformationMatrix(matrix);
        }
                
        const COLLADABU::Math::Matrix4 worldMatrix = parentMatrix * matrix;

        //Need to FIX OpenCOLLADA typo for isIdentity to reenable this
        //if (!matrix.isIdentity())
        nodeObject->setValue("matrix", this->serializeMatrix4Array(matrix));
        
        // save mesh
		const InstanceGeometryPointerArray& instanceGeometries = node->getInstanceGeometries();
        
        unsigned int count = (unsigned int)instanceGeometries.getCount();
        if (count > 0) {
            //FIXME: should not have to have a special attribute name for multiple meshes.
            // this might end up as making mode node, one per mesh.
            //JSONExport
            shared_ptr <JSONExport::JSONArray> meshesArray(new JSONExport::JSONArray());
            nodeObject->setValue("meshes", meshesArray);
            
            for (unsigned int i = 0 ; i < count; i++) {
                InstanceGeometry* instanceGeometry = instanceGeometries[i];
                
                MaterialBindingArray& materialBindings = instanceGeometry->getMaterialBindings();
                
                unsigned int meshUID = (unsigned int)instanceGeometry->getInstanciatedObjectId().getObjectId();
                MeshVectorSharedPtr meshes = this->_uniqueIDToMeshes[meshUID];

                for (size_t meshIndex = 0 ; meshIndex < meshes->size() ; meshIndex++) {
                    shared_ptr <JSONMesh> mesh = (*meshes)[meshIndex];
                    
                    if (!mesh) {
                        continue;
                    }
                    
                    if (sceneFlatteningInfo) {
                        JSONExport::IndexSetToAccessorHashmap& semanticMap = mesh->getAccessorsForSemantic(JSONExport::VERTEX);
                        shared_ptr <JSONExport::JSONAccessor> vertexAccessor = semanticMap[0];
                        
                        BBOX vertexBBOX(COLLADABU::Math::Vector3(vertexAccessor->getMin()),
                                        COLLADABU::Math::Vector3(vertexAccessor->getMax()));
                        vertexBBOX.transform(worldMatrix);
                        
                        sceneFlatteningInfo->sceneBBOX.merge(&vertexBBOX);
                    }
                    
                    PrimitiveVector primitives = mesh->getPrimitives();
                    for (size_t j = 0 ; j < primitives.size() ; j++) {
                        shared_ptr <JSONExport::JSONPrimitive> primitive = primitives[j];
                        
                        //FIXME: consider optimizing this with a hashtable, would be better if it was coming that way from OpenCOLLADA
                        int materialBindingIndex = -1;
                        for (size_t k = 0; k < materialBindings.getCount() ; k++) {
                            if (materialBindings[k].getMaterialId() == primitive->getMaterialObjectID()) {
                                materialBindingIndex = (unsigned int)k;
                            }
                        }
                        if (materialBindingIndex != -1) {
                            unsigned int referencedMaterialID = (unsigned int)materialBindings[materialBindingIndex].getReferencedMaterial().getObjectId();
                            
                            unsigned int effectID = this->_materialUIDToEffectUID[referencedMaterialID];
                            std::string materialName = this->_materialUIDToName[referencedMaterialID];
                            shared_ptr <JSONExport::JSONEffect> effect = this->_uniqueIDToEffect[effectID];
                            effect->setName(materialName);
                            primitive->setMaterialID(effect->getID());
                        }
                    }
                    
                    meshesArray->appendValue(shared_ptr <JSONExport::JSONString> (new JSONExport::JSONString(mesh->getID())));
                    if (sceneFlatteningInfo) {
                        shared_ptr <MeshFlatteningInfo> meshFlatteningInfo(new MeshFlatteningInfo(meshUID, parentMatrix));
                        sceneFlatteningInfo->allMeshes.push_back(meshFlatteningInfo); 
                    }
                }

            }
        }
        
        shared_ptr <JSONExport::JSONArray> childrenArray(new JSONExport::JSONArray());
        nodeObject->setValue("children", childrenArray);
        
        count = (unsigned int)nodes.getCount();
        
        for (unsigned int i = 0 ; i < count ; i++)  {
            std::string id = uniqueIdWithType("node", nodes[i]->getUniqueId());
            childrenArray->appendValue(shared_ptr <JSONExport::JSONString> (new JSONExport::JSONString(id)));
        }
        
        nodesObject->setValue(originalID, static_pointer_cast <JSONExport::JSONValue> (nodeObject));
        
        for (unsigned int i = 0 ; i < count ; i++)  {
            this->writeNode(nodes[i], nodesObject, worldMatrix, sceneFlatteningInfo);
        }
        
        const InstanceNodePointerArray& instanceNodes = node->getInstanceNodes();
        count = (unsigned int)instanceNodes.getCount();
        for (unsigned int i = 0 ; i < count ; i++) {
            InstanceNode* instanceNode  = instanceNodes[i];
            
            std::string id = uniqueIdWithType("node", instanceNode->getInstanciatedObjectId());
            childrenArray->appendValue(shared_ptr <JSONExport::JSONString> (new JSONExport::JSONString(id)));
        }
        
        return true;
    }

    // Flattening   [UNFINISHED CODE]
    //conditions for flattening
    // -> same material
    // option to merge of not non-opaque geometry
    //  -> check per primitive that sources / semantic layout matches 
    // [accessors]
    // -> for all meshes 
    //   -> collect all kind of semantic
    // -> for all meshes
    //   -> get all accessors 
    //   -> transforms & write vtx attributes
    bool DAE2JSONWriter::processSceneFlatteningInfo(SceneFlatteningInfo* sceneFlatteningInfo) 
    {
        /*
        MeshFlatteningInfoVector allMeshes = sceneFlatteningInfo->allMeshes;
        //First collect all kind of accessors available
        size_t count = allMeshes.size();
        for (size_t i = 0 ; i < count ; i++) {
            shared_ptr <MeshFlatteningInfo> meshInfo = allMeshes[i];
            MeshVectorSharedPtr *meshes = this->_uniqueIDToMeshes[meshInfo->getUID()];
           // shared_ptr <AccessorVector> accessors = mesh->accessors();
        }
        */
        return true;
    }
    
    bool DAE2JSONWriter::writeVisualScene( const COLLADAFW::VisualScene* visualScene )
	{
        //FIXME: only one visual scene assumed/handled
        shared_ptr <JSONExport::JSONObject> scenesObject(new JSONExport::JSONObject());
        shared_ptr <JSONExport::JSONObject> sceneObject(new JSONExport::JSONObject());
        shared_ptr <JSONExport::JSONObject> nodesObject = static_pointer_cast <JSONExport::JSONObject> (this->_converterContext.root->getValue("nodes"));
        shared_ptr <JSONExport::JSONObject> rootObject(new JSONExport::JSONObject());

		const NodePointerArray& nodePointerArray = visualScene->getRootNodes();
        size_t nodeCount = nodePointerArray.getCount();
        
        this->_converterContext.root->setValue("scenes", scenesObject);
        sceneObject->setString("node", "root");
        scenesObject->setValue("defaultScene", sceneObject); //FIXME: should use this id -> visualScene->getOriginalId()
        nodesObject->setValue("root", rootObject);
                
        //first pass to output children name of our root node
        shared_ptr <JSONExport::JSONArray> childrenArray(new JSONExport::JSONArray());
        
        for (size_t i = 0 ; i < nodeCount ; i++) { 
            std::string id = uniqueIdWithType("node", nodePointerArray[i]->getUniqueId());

            shared_ptr <JSONExport::JSONString> nodeIDValue(new JSONExport::JSONString(id));            
            childrenArray->appendValue(static_pointer_cast <JSONExport::JSONValue> (nodeIDValue));
        }
        
        rootObject->setValue("children", childrenArray);
        
        for (size_t i = 0 ; i < nodeCount ; i++) {
            //FIXME: &this->_sceneFlatteningInfo
            this->writeNode(nodePointerArray[i], nodesObject, COLLADABU::Math::Matrix4::IDENTITY, NULL);
        }
        
		return true;
	}
    
	//--------------------------------------------------------------------
	bool DAE2JSONWriter::writeScene( const COLLADAFW::Scene* scene )
	{
		return true;
	}
    
	//--------------------------------------------------------------------
	bool DAE2JSONWriter::writeLibraryNodes( const COLLADAFW::LibraryNodes* libraryNodes )
	{
        const NodePointerArray& nodes = libraryNodes->getNodes();

        shared_ptr <JSONExport::JSONObject> nodesObject = static_pointer_cast <JSONExport::JSONObject> (this->_converterContext.root->getValue("nodes"));
        
        size_t count = nodes.getCount();
        for (size_t i = 0 ; i < count ; i++) {
            const COLLADAFW::Node *node = nodes[i];

            if (!this->writeNode(node,  nodesObject, COLLADABU::Math::Matrix4::IDENTITY, 0))
                return false;
        }
        
        return true;
	}   
            
	//--------------------------------------------------------------------
    bool DAE2JSONWriter::writeGeometry( const COLLADAFW::Geometry* geometry )
	{
        switch (geometry->getType()) {
            case Geometry::GEO_TYPE_MESH:
            {
                const COLLADAFW::Mesh* mesh = (COLLADAFW::Mesh*)geometry;
                unsigned int meshID = (unsigned int)geometry->getUniqueId().getObjectId();
                MeshVectorSharedPtr meshes;
                
                if (this->_uniqueIDToMeshes.count(meshID) > 0)
                    meshes = this->_uniqueIDToMeshes[meshID];
                else {
                    meshes =  shared_ptr<MeshVector> (new MeshVector);
                    ConvertOpenCOLLADAMesh((COLLADAFW::Mesh*)mesh, (*meshes));
                    for (size_t i = 0 ; i < meshes->size() ; i++) {
                        if ((*meshes)[i]->getPrimitives().size() > 0) {
                            (*meshes)[i]->writeAllBuffers(this->_fileOutputStream);
                        }
                    }
                    
                    this->_uniqueIDToMeshes[meshID] = meshes;
                }
            }
                break;
            case Geometry::GEO_TYPE_SPLINE:
            case Geometry::GEO_TYPE_CONVEX_MESH:
                // FIXME: handle convertion to mesh
            case Geometry::GEO_TYPE_UNKNOWN:
                //FIXME: handle error
                return false;
        }
        
        return true;
	}
    
	//--------------------------------------------------------------------
	bool DAE2JSONWriter::writeMaterial( const COLLADAFW::Material* material )
	{
        const UniqueId& effectUID = material->getInstantiatedEffect();
		unsigned int materialID = (unsigned int)material->getUniqueId().getObjectId();
        this->_materialUIDToName[materialID] = material->getName();
        this->_materialUIDToEffectUID[materialID] = (unsigned int)effectUID.getObjectId();
		return true;
	}
            
    const std::string DAE2JSONWriter::writeTechniqueForCommonProfileIfNeeded(const COLLADAFW::EffectCommon* effectCommon) {
        //compute/retrieve the technique matching the specified common profile (TODO).
        //for now we just have one shader available :)..
        std::string techniqueName = getTechniqueNameForProfile(effectCommon, this->_converterContext);
        
        shared_ptr <JSONExport::JSONObject> techniquesObject;
        
        //get or create if necessary the techniques object
        techniquesObject = this->_converterContext.root->createObjectIfNeeded("techniques");
        
        if (!techniquesObject->contains(techniqueName)) {
            shared_ptr <JSONExport::JSONObject> techniqueObject = createTechniqueForProfile(effectCommon, this->_converterContext);
            techniquesObject->setValue(techniqueName, techniqueObject);
        }
        
        return techniqueName;
    }
    
	//--------------------------------------------------------------------
	bool DAE2JSONWriter::writeEffect( const COLLADAFW::Effect* effect )
	{
        const COLLADAFW::CommonEffectPointerArray& commonEffects = effect->getCommonEffects();
        
        if (commonEffects.getCount() > 0) {
            const COLLADAFW::EffectCommon* effectCommon = commonEffects[0];
            const std::string& techniqueID = this->writeTechniqueForCommonProfileIfNeeded(effectCommon);
            const ColorOrTexture& diffuse = effectCommon->getDiffuse ();
            
            std::string uniqueId = "";
#if EXPORT_MATERIALS_AS_EFFECTS        
            uniqueId += "material.";
#else
            uniqueId += "effect.";
#endif
            uniqueId += JSONExport::JSONUtils::toString(effect->getUniqueId().getObjectId());;
            
            shared_ptr <JSONExport::JSONEffect> cvtEffect(new JSONExport::JSONEffect(uniqueId));
            shared_ptr <JSONExport::JSONObject> techniques(new JSONExport::JSONObject());
            shared_ptr <JSONExport::JSONObject> technique(new JSONExport::JSONObject());
            shared_ptr <JSONExport::JSONObject> parameters(new JSONExport::JSONObject());
            //shared_ptr <JSONExport::JSONObject> parameter(new JSONExport::JSONObject());

            techniques->setValue(techniqueID.c_str(), technique);
            technique->setValue("parameters", parameters);
            
            //retrive the type, parameterName -> symbol -> type
            
            double red = 1, green = 1, blue = 1;
            bool hasDiffuseTexture = diffuse.isTexture();
            if (!hasDiffuseTexture) {
                const Color& color = diffuse.getColor();
                if (diffuse.getType() != COLLADAFW::ColorOrTexture::UNSPECIFIED) {
                    red = color.getRed();
                    green = color.getGreen();
                    blue = color.getBlue();
                }
                parameters->setValue("diffuseColor", serializeVec3(red,green,blue));
                
            } else {
                const Texture&  diffuseTexture = diffuse.getTexture();
                const SamplerPointerArray& samplers = effectCommon->getSamplerPointerArray();
                const Sampler* sampler = samplers[diffuseTexture.getSamplerId()]; 
                const UniqueId& imageUID = sampler->getSourceImage();
                
                //TODO: remove this from here
                COLLADABU::URI imageURI = this->_imageIdToImageURL[uniqueIdWithType("image",imageUID)];               
                
                std::string pathDir = imageURI.getPathDir();
                bool relativePath = true;
                if (pathDir.size() > 0) {
                    if ((pathDir[0] != '.') || (pathDir[0] == '/')) {
                        relativePath = false;
                    }
                }
                //FIXME: handle absolute path case
                //FIXME: right now this is a short-cut, should point to a texture object that has sampler info
                
                //parameters->setString("diffuseTexture",imageURI.getPathDir() + imageURI.getPathFile());
                shared_ptr <JSONExport::JSONObject> sampler2D(new JSONExport::JSONObject());
                
                sampler2D->setString("wrapS", "REPEAT");
                sampler2D->setString("wrapT", "REPEAT");
                sampler2D->setString("minFilter", "LINEAR");
                sampler2D->setString("maxFilter", "LINEAR");
                sampler2D->setString("image", uniqueIdWithType("image",imageUID));
                
                parameters->setValue("diffuseTexture", sampler2D);
            }
            
            if (!isOpaque(effectCommon)) {
                parameters->setValue("transparency", shared_ptr <JSONExport::JSONNumber> (new JSONExport::JSONNumber((double)getTransparency(effectCommon))));
            }
            
            cvtEffect->setTechniques(techniques);
            cvtEffect->setTechniqueID(techniqueID);
            this->_uniqueIDToEffect[(unsigned int)effect->getUniqueId().getObjectId()] = cvtEffect;            
            
        }
		return true;                
	}
    
	//--------------------------------------------------------------------
	bool DAE2JSONWriter::writeCamera( const COLLADAFW::Camera* camera )
	{
        shared_ptr <JSONExport::JSONObject> camerasObject = static_pointer_cast <JSONExport::JSONObject> (this->_converterContext.root->getValue("cameras"));
        if (!camerasObject) {
            camerasObject = shared_ptr <JSONExport::JSONObject> (new JSONExport::JSONObject());
            this->_converterContext.root->setValue("cameras", camerasObject);
        }
        
        shared_ptr <JSONExport::JSONObject> cameraObject(new JSONExport::JSONObject());
        
        std::string id = uniqueIdWithType("camera", camera->getUniqueId());
        
        camerasObject->setValue(id, cameraObject);
        
        switch (camera->getCameraType()) {
            case Camera::UNDEFINED_CAMERATYPE:
                printf("WARNING: unknown camera type: using perspective\n");
                cameraObject->setString("cameraType", "perspective");
                break;
            case Camera::ORTHOGRAPHIC:
            {
                cameraObject->setString("projection", "orthographic");
                switch (camera->getDescriptionType()) {
                    case Camera::UNDEFINED: //!< The perspective camera object is invalid
                        //FIXME: handle error
                        break;
                    case Camera::SINGLE_X: //!< Only xfov or xmag, respectively describes the camera
                        cameraObject->setDouble("xmag", camera->getXMag().getValue());
                        break;
                    case Camera::SINGLE_Y: //!< Only yfov or ymag, respectively describes the camera
                        cameraObject->setDouble("ymag", camera->getYMag().getValue());
                        break;
                    case Camera::X_AND_Y: //!< xfov and yfov or xmag and ymag, respectively describe the camera
                        cameraObject->setDouble("xmag", camera->getXMag().getValue());
                        cameraObject->setDouble("ymag", camera->getYMag().getValue());
                        break;
                    case Camera::ASPECTRATIO_AND_X: //!< aspect ratio and xfov or xmag, respectively describe the camera
                        cameraObject->setDouble("xmag", camera->getXMag().getValue());
                        cameraObject->setDouble("aspect_ratio", camera->getAspectRatio().getValue());
                        break;
                    case Camera::ASPECTRATIO_AND_Y: //!< aspect ratio and yfov or <mag, respectivelydescribe the camera
                        cameraObject->setDouble("ymag", camera->getYMag().getValue());
                        cameraObject->setDouble("aspect_ratio", camera->getAspectRatio().getValue());
                        break;
                }
                
            }
                break;
            case Camera::PERSPECTIVE: 
            {
                cameraObject->setString("projection", "perspective");
                switch (camera->getDescriptionType()) {
                    case Camera::UNDEFINED: //!< The perspective camera object is invalid
                        //FIXME: handle error
                        break;
                    case Camera::SINGLE_X: //!< Only xfov or xmag, respectively describes the camera
                        cameraObject->setDouble("xfov", camera->getXFov().getValue());
                        break;
                    case Camera::SINGLE_Y: //!< Only yfov or ymag, respectively describes the camera
                        cameraObject->setDouble("yfov", camera->getYFov().getValue());
                        break;
                    case Camera::X_AND_Y: //!< xfov and yfov or xmag and ymag, respectively describe the camera
                        cameraObject->setDouble("xfov", camera->getXFov().getValue());
                        cameraObject->setDouble("yfov", camera->getYFov().getValue());
                        break;
                    case Camera::ASPECTRATIO_AND_X: //!< aspect ratio and xfov or xmag, respectively describe the camera
                        cameraObject->setDouble("xfov", camera->getXFov().getValue());
                        cameraObject->setDouble("aspect_ratio", camera->getAspectRatio().getValue());
                        break;
                    case Camera::ASPECTRATIO_AND_Y: //!< aspect ratio and yfov or <mag, respectivelydescribe the camera
                        cameraObject->setDouble("yfov", camera->getYFov().getValue());
                        cameraObject->setDouble("aspect_ratio", camera->getAspectRatio().getValue());
                        break;
                }
                
            }
                break;
        }      
        
        cameraObject->setDouble("znear", camera->getNearClippingPlane().getValue());
        cameraObject->setDouble("zfar", camera->getFarClippingPlane().getValue());
        
		return true;
	}
    
	//--------------------------------------------------------------------
	bool DAE2JSONWriter::writeImage( const COLLADAFW::Image* openCOLLADAImage )
	{
        shared_ptr <JSONExport::JSONObject> images = this->_converterContext.root->createObjectIfNeeded("images");
        shared_ptr <JSONExport::JSONObject> image(new JSONExport::JSONObject());

        images->setValue(uniqueIdWithType("image",openCOLLADAImage->getUniqueId()), image);
        
        std::string pathDir = openCOLLADAImage->getImageURI().getPathDir();
        std::string imagePath = pathDir + openCOLLADAImage->getImageURI().getPathFile();
        //parameters->setString("diffuseTexture",imageURI.getPathDir() + imageURI.getPathFile());
        
        /*
        bool relativePath = true;
        if (pathDir.size() > 0) {
            if ((pathDir[0] != '.') || (pathDir[0] == '/')) {
                relativePath = false;
            }
        }*/
        
        image->setString("path", imagePath);
        
        this->_imageIdToImageURL[uniqueIdWithType("image",openCOLLADAImage->getUniqueId()) ] = openCOLLADAImage->getImageURI();       
        return true;        
	}
    
	//--------------------------------------------------------------------
	bool DAE2JSONWriter::writeLight( const COLLADAFW::Light* light )
	{
		return true;
	}
    
	//--------------------------------------------------------------------
	bool DAE2JSONWriter::writeAnimation( const COLLADAFW::Animation* animation )
	{
		return true;
	}
    
	//--------------------------------------------------------------------
	bool DAE2JSONWriter::writeAnimationList( const COLLADAFW::AnimationList* animationList )
	{
		return true;
	}
    
	//--------------------------------------------------------------------
	bool DAE2JSONWriter::writeSkinControllerData( const COLLADAFW::SkinControllerData* skinControllerData )
	{
		return true;
	}
    
	//--------------------------------------------------------------------
	bool DAE2JSONWriter::writeController( const COLLADAFW::Controller* Controller )
	{
		return true;
	}
    
} // namespace DAE2JSON

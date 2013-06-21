#include "GLTF.h"
#include "../GLTF-OpenCOLLADA.h"
#include "../GLTFConverterContext.h"

#include "meshConverter.h"
#include "../helpers/mathHelpers.h"
#include "../helpers/geometryHelpers.h"

//WebGL Loader..
#include "bounds.h"
#include "compress.h"
#include "mesh.h"
#include "optimize.h"
#include "stream.h"

namespace GLTF
{
    //webgl-loader compression
    static void __FeedAttribs(void *value,
                              GLTF::ComponentType type,
                              size_t componentsPerAttribute,
                              size_t index,
                              size_t vertexAttributeByteSize,
                              void *context) {
        char* bufferData = (char*)value;
        AttribList *attribList = (AttribList*)context;
        
        if (componentsPerAttribute > 1) {
            switch (type) {
                case GLTF::FLOAT: {
                    float* vector = (float*)bufferData;
                    for (int  i = 0; i < componentsPerAttribute ; i++) {
                        attribList->push_back(vector[i]);
                    }
                }
                    break;
                default:
                    break;
            }
        }
    }
    
    static shared_ptr<JSONObject> getDecodeParams(webgl_loader::BoundsParams& bp) {
        shared_ptr<JSONObject> decodeParams(new JSONObject());
        shared_ptr<JSONArray> decodeOffsets(new JSONArray());
        shared_ptr<JSONArray> decodeScales(new JSONArray());

        for (size_t i = 0 ; i < 8 ; i++) {
            decodeOffsets->appendValue(shared_ptr<JSONNumber>(new JSONNumber((double)bp.decodeOffsets[i])));
            decodeScales->appendValue(shared_ptr<JSONNumber>(new JSONNumber((double)bp.decodeScales[i])));
        }
        
        decodeParams->setValue("decodeOffsets", decodeOffsets);
        decodeParams->setValue("decodeScales", decodeScales);
        
        return decodeParams;
    }
    
    static bool compress(shared_ptr <GLTF::GLTFMesh> mesh) {
        //Compresss wavefront
        WavefrontObjFile dummyObj;
        
        AttribList *positions = dummyObj.positions();
        shared_ptr <GLTFMeshAttribute> positionAttrib = mesh->getMeshAttributesForSemantic(POSITION)[0];
        positionAttrib->apply(__FeedAttribs, positions);
        
        AttribList *normals = dummyObj.normals();
        shared_ptr <GLTFMeshAttribute> normalAttrib = mesh->getMeshAttributesForSemantic(NORMAL)[0];
        normalAttrib->apply(__FeedAttribs, normals);
        
        AttribList *texcoords = dummyObj.texcoords();
        shared_ptr <GLTFMeshAttribute> texcoordAttrib = mesh->getMeshAttributesForSemantic(TEXCOORD)[0];
        texcoordAttrib->apply(__FeedAttribs, texcoords);
        
        DrawBatch* drawBatch = dummyObj.drawBatch();
        
        PrimitiveVector primitives = mesh->getPrimitives();
        int dummyIndices[9];
        
        //FILE* utf8_out_fp = fopen("testglloader.bin", "wb");
        //webgl_loader::FileSink utf8_sink(utf8_out_fp);
        
        std::vector<char> compressedOutput;
        webgl_loader::VectorSink utf8_sink(&compressedOutput);
        
        for (size_t i = 0 ; i < primitives.size(); i++) {
            shared_ptr <GLTFPrimitive> primitive = primitives[i];
            shared_ptr <GLTFIndices> indices = primitive->getUniqueIndices();
            
            //FIXME: assume triangles
            size_t count = indices->getCount();
            
            unsigned int* intIndices = (unsigned int*)indices->getBufferView()->getBufferDataByApplyingOffset();
                        
            for (size_t k = 0 ; k < count ; k += 3) {
                unsigned int i1 = intIndices[k];
                unsigned int i2 = intIndices[k + 1];
                unsigned int i3 = intIndices[k + 2];
                
                //simulate OBJ files 1-based indexing
                i1 += 1;
                i2 += 1;
                i3 += 1;
                
                dummyIndices[0] = dummyIndices[1] = dummyIndices[2] = (int)i1;
                dummyIndices[3] = dummyIndices[4] = dummyIndices[5] = (int)i2;
                dummyIndices[6] = dummyIndices[7] = dummyIndices[8] = (int)i3;
                
                drawBatch->AddTriangle(0, dummyIndices);
            }
            
            const MaterialBatches& batches = dummyObj.material_batches();
            
            // Pass 1: compute bounds.
            webgl_loader::Bounds bounds;
            bounds.Clear();
            for (MaterialBatches::const_iterator iter = batches.begin();
                 iter != batches.end(); ++iter) {
                const DrawBatch& draw_batch = iter->second;
                bounds.Enclose(draw_batch.draw_mesh().attribs);
            }
            FILE* json_out = stdout;
            
            webgl_loader::BoundsParams bounds_params =
            webgl_loader::BoundsParams::FromBounds(bounds);
            
            shared_ptr<JSONObject> decodeParams = getDecodeParams(bounds_params);
                                       
            fputs("  \"decodeParams\": ", json_out);
            bounds_params.DumpJson(json_out);
            fputs(", \"urls\": {\n", json_out);
            // Pass 2: quantize, optimize, compress, report.
            //FILE* utf8_out_fp = fopen("testglloader.bin", "wb");
            //CHECK(utf8_out_fp != NULL);
            fprintf(json_out, "    \"%s\": [\n", "testglloader.bin");
                        
            size_t offset = 0;
            MaterialBatches::const_iterator iter = batches.begin();
            while (iter != batches.end()) {
                const DrawMesh& draw_mesh = iter->second.draw_mesh();
                if (draw_mesh.indices.empty()) {
                    ++iter;
                    continue;
                }
                QuantizedAttribList quantized_attribs;
                webgl_loader::AttribsToQuantizedAttribs(draw_mesh.attribs, bounds_params,
                                                        &quantized_attribs);
                VertexOptimizer vertex_optimizer(quantized_attribs);
                const std::vector<GroupStart>& group_starts = iter->second.group_starts();
                WebGLMeshList webgl_meshes;
                std::vector<size_t> group_lengths;
                for (size_t i = 1; i < group_starts.size(); ++i) {
                    const size_t here = group_starts[i-1].offset;
                    const size_t length = group_starts[i].offset - here;
                    group_lengths.push_back(length);
                    vertex_optimizer.AddTriangles(&draw_mesh.indices[here], length,
                                                  &webgl_meshes);
                }
                const size_t here = group_starts.back().offset;
                const size_t length = draw_mesh.indices.size() - here;
                CHECK(length % 3 == 0);
                group_lengths.push_back(length);
                vertex_optimizer.AddTriangles(&draw_mesh.indices[here], length,
                                              &webgl_meshes);
                
                std::vector<std::string> material;
                std::vector<size_t> attrib_start, attrib_length, index_start, index_length;
                for (size_t i = 0; i < webgl_meshes.size(); ++i) {
                    const size_t num_attribs = webgl_meshes[i].attribs.size();
                    const size_t num_indices = webgl_meshes[i].indices.size();
                    CHECK(num_attribs % 8 == 0);
                    CHECK(num_indices % 3 == 0);
                    webgl_loader::CompressQuantizedAttribsToUtf8(webgl_meshes[i].attribs,
                                                                 &utf8_sink);
                    webgl_loader::CompressIndicesToUtf8(webgl_meshes[i].indices, &utf8_sink);
                    material.push_back(iter->first);
                    attrib_start.push_back(offset);
                    attrib_length.push_back(num_attribs / 8);
                    index_start.push_back(offset + num_attribs);
                    index_length.push_back(num_indices / 3);
                    offset += num_attribs + num_indices;
                }
                
                int meshIndex = 0;
                
                shared_ptr<JSONArray> attribs(new JSONArray());
                shared_ptr<JSONArray> indices(new JSONArray());
                attribs->appendValue(shared_ptr<JSONNumber>(new JSONNumber((double)attrib_start[meshIndex])));
                attribs->appendValue(shared_ptr<JSONNumber>(new JSONNumber((double)attrib_length[meshIndex])));
                indices->appendValue(shared_ptr<JSONNumber>(new JSONNumber((double)index_start[meshIndex])));
                indices->appendValue(shared_ptr<JSONNumber>(new JSONNumber((double)index_length[meshIndex])));
                
                decodeParams->setValue("attribRange", attribs);
                decodeParams->setValue("indexRange", indices);
                
                mesh->getExtensions()->setValue("won-compression", decodeParams);
                
                for (size_t i = 0; i < webgl_meshes.size(); ++i) {
                    fprintf(json_out,
                            "      { \"material\": \"%s\",\n"
                            "        \"attribRange\": [" PRIuS ", " PRIuS "],\n"
                            "        \"indexRange\": [" PRIuS ", " PRIuS "]\n"
                            "      }",
                            material[i].c_str(),
                            attrib_start[i], attrib_length[i],
                            index_start[i], index_length[i]);
                    if (i != webgl_meshes.size() - 1) {
                        fputs(",\n", json_out);
                    }
                }
                const bool last = (++iter == batches.end());
                fputs(",\n" + last, json_out);
                
                
            }
            fputs("    ]\n", json_out);
            fputs("  }\n}", json_out);
            
            printf("compressed output size:%d\n",compressedOutput.size());
            
            //FIXME: remove that crappy copy here
            char *bufferPtr = ( char*)malloc(compressedOutput.size());
            for (size_t idx = 0 ; idx < compressedOutput.size() ; idx++) {
                bufferPtr[idx] = compressedOutput[idx];
            }
            shared_ptr<GLTFBuffer> compressedBuffer(new GLTFBuffer(bufferPtr, compressedOutput.size(), true));
            
            mesh->setCompressedBuffer(compressedBuffer);
             
        }
        
        return true;
    }
    
    
    //
    
    
    
    static unsigned int ConvertOpenCOLLADAMeshVertexDataToGLTFMeshAttributes(const COLLADAFW::MeshVertexData &vertexData, GLTF::IndexSetToMeshAttributeHashmap &meshAttributes)
    {
        // The following are OpenCOLLADA fmk issues preventing doing a totally generic processing of sources
        //1. "set"(s) other than texCoord don't have valid input infos
        //2. not the original id in the source
        
        std::string name;
        size_t length, elementsCount;
        size_t stride = 0;
        size_t size = 0;
        size_t byteOffset = 0;
        size_t inputLength = 0;
        
        size_t setCount = vertexData.getNumInputInfos();
        bool unpatchedOpenCOLLADA = (setCount == 0); // reliable heuristic to know if the input have not been set
        
        if (unpatchedOpenCOLLADA)
            setCount = 1;
        
        for (size_t indexOfSet = 0 ; indexOfSet < setCount ; indexOfSet++) {
            
            if (!unpatchedOpenCOLLADA) {
                name = vertexData.getName(indexOfSet);
                size = vertexData.getStride(indexOfSet);
                inputLength = vertexData.getLength(indexOfSet);
            } else {
                // for unpatched version of OpenCOLLADA we need this work-around.
                name = GLTF::GLTFUtils::generateIDForType("buffer").c_str();
                size = 3; //only normal and positions should reach this code
                inputLength = vertexData.getLength(0);
            }
            
            //name is the id
            length = inputLength ? inputLength : vertexData.getValuesCount();
            elementsCount = length / size;
            unsigned char *sourceData = 0;
            size_t sourceSize = 0;
            
            GLTF::ComponentType componentType = GLTF::NOT_AN_ELEMENT_TYPE;
            switch (vertexData.getType()) {
                case COLLADAFW::MeshVertexData::DATA_TYPE_FLOAT: {
                    componentType = GLTF::FLOAT;
                    stride = sizeof(float) * size;
                    const COLLADAFW::FloatArray* array = vertexData.getFloatValues();
                    
                    sourceData = (unsigned char*)array->getData() + byteOffset;
                    
                    sourceSize = length * sizeof(float);
                    byteOffset += sourceSize; //Doh! - OpenCOLLADA store all sets contiguously in the same array
                }
                    break;
                case COLLADAFW::MeshVertexData::DATA_TYPE_DOUBLE: {
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
                case COLLADAFW::MeshVertexData::DATA_TYPE_UNKNOWN:
                    //FIXME report error
                    break;
            }
            
            // FIXME: the source could be shared, store / retrieve it here
            shared_ptr <GLTFBufferView> cvtBufferView = createBufferViewWithAllocatedBuffer(name, sourceData, 0, sourceSize, false);
            shared_ptr <GLTFMeshAttribute> cvtMeshAttribute(new GLTFMeshAttribute());
            
            cvtMeshAttribute->setBufferView(cvtBufferView);
            cvtMeshAttribute->setComponentsPerAttribute(size);
            cvtMeshAttribute->setByteStride(stride);
            cvtMeshAttribute->setComponentType(componentType);
            cvtMeshAttribute->setCount(elementsCount);
            
            meshAttributes[(unsigned int)indexOfSet] = cvtMeshAttribute;
        }
        
        return (unsigned int)setCount;
    }
    
    static void __AppendIndices(shared_ptr <GLTF::GLTFPrimitive> &primitive, IndicesVector &primitiveIndicesVector, shared_ptr <GLTF::GLTFIndices> &indices, GLTF::Semantic semantic, unsigned int indexOfSet)
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
                                  IndicesVector &primitiveIndicesVector
                                  )
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
        unsigned int initialIndex = indexList->getInitialIndex();
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
        
        //FIXME: Looks like for texcoord indexSet begin at 1, this is out of the sync with the index used in ConvertOpenCOLLADAMeshVertexDataToGLTFMeshAttributes that begins at 0
        //for now forced to 0, to be fixed for multi texturing.
        
        //unsigned int idx = (unsigned int)indexList->getSetIndex();
        
        shared_ptr <GLTFIndices> jsonIndices(new GLTFIndices(uvBuffer, count));
        __AppendIndices(cvtPrimitive, primitiveIndicesVector, jsonIndices, semantic, idx);
    }
    
    static shared_ptr <GLTF::GLTFPrimitive> ConvertOpenCOLLADAMeshPrimitive(
                                                                            COLLADAFW::MeshPrimitive *openCOLLADAMeshPrimitive,
                                                                            IndicesVector &primitiveIndicesVector)
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
        cvtPrimitive->setType(type);
        
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
        
        shared_ptr <GLTF::GLTFIndices> positionIndices(new GLTF::GLTFIndices(positionBuffer,count));
        
        __AppendIndices(cvtPrimitive, primitiveIndicesVector, positionIndices, POSITION, 0);
        
        if (openCOLLADAMeshPrimitive->hasNormalIndices()) {
            unsigned int triangulatedIndicesCount = 0;
            indices = openCOLLADAMeshPrimitive->getNormalIndices().getData();
            if (shouldTriangulate) {
                indices = createTrianglesFromPolylist(verticesCountArray, indices, vcount, &triangulatedIndicesCount);
                count = triangulatedIndicesCount;
            }
            
            shared_ptr <GLTF::GLTFBufferView> normalBuffer = createBufferViewWithAllocatedBuffer(indices, 0, count * sizeof(unsigned int), shouldTriangulate ? true : false);
            shared_ptr <GLTF::GLTFIndices> normalIndices(new GLTF::GLTFIndices(normalBuffer,
                                                                               count));
            __AppendIndices(cvtPrimitive, primitiveIndicesVector, normalIndices, NORMAL, 0);
        }
        
        if (openCOLLADAMeshPrimitive->hasColorIndices()) {
            COLLADAFW::IndexListArray& colorListArray = openCOLLADAMeshPrimitive->getColorIndicesArray();
            for (size_t i = 0 ; i < colorListArray.getCount() ; i++) {
                COLLADAFW::IndexList* indexList = openCOLLADAMeshPrimitive->getColorIndices(i);
                __HandleIndexList(i,
                                  indexList,
                                  GLTF::COLOR,
                                  shouldTriangulate,
                                  count,
                                  vcount,
                                  verticesCountArray,
                                  cvtPrimitive,
                                  primitiveIndicesVector);
            }
        }
        
        if (openCOLLADAMeshPrimitive->hasUVCoordIndices()) {
            COLLADAFW::IndexListArray& uvListArray = openCOLLADAMeshPrimitive->getUVCoordIndicesArray();
            for (size_t i = 0 ; i < uvListArray.getCount() ; i++) {
                COLLADAFW::IndexList* indexList = openCOLLADAMeshPrimitive->getUVCoordIndices(i);
                __HandleIndexList(i,
                                  indexList,
                                  GLTF::TEXCOORD,
                                  shouldTriangulate,
                                  count,
                                  vcount,
                                  verticesCountArray,
                                  cvtPrimitive,
                                  primitiveIndicesVector);
            }
        }
        
        if (verticesCountArray) {
            free(verticesCountArray);
        }
        
        return cvtPrimitive;
    }
    
    static void __InvertV(void *value,
                          GLTF::ComponentType type,
                          size_t componentsPerAttribute,
                          size_t index,
                          size_t vertexAttributeByteSize,
                          void *context) {
        char* bufferData = (char*)value;
        
        if (componentsPerAttribute > 1) {
            switch (type) {
                case GLTF::FLOAT: {
                    float* vector = (float*)bufferData;
                    vector[1] = (float) (1.0 - vector[1]);
                }
                    break;
                default:
                    break;
            }
        }
    }
    
    void convertOpenCOLLADAMesh(COLLADAFW::Mesh* openCOLLADAMesh,
                                MeshVector &meshes)
    {
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
                (primitiveType != COLLADAFW::MeshPrimitive::TRIANGLE_STRIPS) &&
                (primitiveType != COLLADAFW::MeshPrimitive::POLYLIST) &&
                (primitiveType != COLLADAFW::MeshPrimitive::POLYGONS))
                continue;
            
            shared_ptr <GLTF::IndicesVector> primitiveIndicesVector(new GLTF::IndicesVector());
            allPrimitiveIndicesVectors.push_back(primitiveIndicesVector);
            
            shared_ptr <GLTF::GLTFPrimitive> primitive = ConvertOpenCOLLADAMeshPrimitive(primitives[i],*primitiveIndicesVector);
            cvtMesh->appendPrimitive(primitive);
            
            VertexAttributeVector vertexAttributes = primitive->getVertexAttributes();
            primitiveIndicesVector = allPrimitiveIndicesVectors[i];
            
            // once we got a primitive, keep track of its meshAttributes
            std::vector< shared_ptr<GLTF::GLTFIndices> > allIndices = *primitiveIndicesVector;
            for (size_t k = 0 ; k < allIndices.size() ; k++) {
                shared_ptr<GLTF::GLTFIndices> indices = allIndices[k];
                GLTF::Semantic semantic = vertexAttributes[k]->getSemantic();
                GLTF::IndexSetToMeshAttributeHashmap& meshAttributes = cvtMesh->getMeshAttributesForSemantic(semantic);
                
                switch (semantic) {
                    case GLTF::POSITION:
                        ConvertOpenCOLLADAMeshVertexDataToGLTFMeshAttributes(openCOLLADAMesh->getPositions(), meshAttributes);
                        break;
                        
                    case GLTF::NORMAL:
                        ConvertOpenCOLLADAMeshVertexDataToGLTFMeshAttributes(openCOLLADAMesh->getNormals(), meshAttributes);
                        break;
                        
                    case GLTF::TEXCOORD:
                        ConvertOpenCOLLADAMeshVertexDataToGLTFMeshAttributes(openCOLLADAMesh->getUVCoords(), meshAttributes);
                        break;
                        
                    case GLTF::COLOR:
                        ConvertOpenCOLLADAMeshVertexDataToGLTFMeshAttributes(openCOLLADAMesh->getColors(), meshAttributes);
                        break;
                        
                    default:
                        break;
                }
                
                cvtMesh->setMeshAttributesForSemantic(semantic, meshAttributes);
            }
        }
        
        //https://github.com/KhronosGroup/collada2json/issues/41
        //Goes through all texcoord and invert V
        GLTF::IndexSetToMeshAttributeHashmap& texcoordMeshAttributes = cvtMesh->getMeshAttributesForSemantic(GLTF::TEXCOORD);
        GLTF::IndexSetToMeshAttributeHashmap::const_iterator meshAttributeIterator;
        
        //FIXME: consider turn this search into a method for mesh
        for (meshAttributeIterator = texcoordMeshAttributes.begin() ; meshAttributeIterator != texcoordMeshAttributes.end() ; meshAttributeIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            shared_ptr <GLTF::GLTFMeshAttribute> meshAttribute = (*meshAttributeIterator).second;
            
            meshAttribute->apply(__InvertV, NULL);
        }
        
        if (cvtMesh->getPrimitives().size() > 0) {
            //After this point cvtMesh should be referenced anymore and will be deallocated
            shared_ptr <GLTF::GLTFMesh> unifiedMesh = createUnifiedIndexesMeshFromMesh(cvtMesh.get(), allPrimitiveIndicesVectors);
            if  (createMeshesWithMaximumIndicesCountFromMeshIfNeeded(unifiedMesh.get(), 65535, meshes) == false) {
                meshes.push_back(unifiedMesh);
            }
        }
        
        //now compress meshes
        for (size_t i = 0 ; i < meshes.size() ; i++) {
            shared_ptr <GLTF::GLTFMesh> mesh = meshes[i];
            compress(mesh);
        }
    }

    
}

#include "GLTF.h"
#include "../GLTF-OpenCOLLADA.h"
#include "../GLTFConverterContext.h"

#include "meshConverter.h"
#include "../helpers/mathHelpers.h"
#include "../helpers/geometryHelpers.h"

#ifdef USE_WEBGLLOADER
//WebGL Loader..
#include "bounds.h"
#include "compress.h"
#include "mesh.h"
#include "optimize.h"
#include "stream.h"
#endif

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
#include "o3dgcSC3DMCEncodeParams.h"
#include "o3dgcIndexedFaceSet.h"
#include "o3dgcSC3DMCEncoder.h"
#include "o3dgcSC3DMCDecoder.h"

using namespace o3dgc;
#endif
//--- X3DGC

#define DUMP_O3DGC_OUTPUT 1

#if DUMP_O3DGC_OUTPUT
void SaveIFSUnsignedShortArray(std::ofstream & fout, const std::string & name, unsigned int a, const unsigned short * const tab, unsigned long nElement, unsigned long dim)
{
    if (tab)
    {
        fout << name << "\t" << a << "\t" << nElement << "\t" << dim << std::endl;
    }
    else
    {
        fout << name << "\t" << a << "\t" << 0 << "\t" << 0 << std::endl;
        return;
    }
    if (!tab) return;
    for (unsigned long i = 0; i < nElement; ++i){
        fout << "[" << i << "]\t";
        for (unsigned long j = 0; j < dim; ++j){
            fout << tab[i*dim + j] << "\t";
        }
        fout << std::endl;
    }
}

void SaveIFSIntArray(std::ofstream & fout, const std::string & name, unsigned int a, const long * const tab, unsigned long nElement, unsigned long dim)
{
    if (tab)
    {
        fout << name << "\t" << a << "\t" << nElement << "\t" << dim << std::endl;
    }
    else
    {
        fout << name << "\t" << a << "\t" << 0 << "\t" << 0 << std::endl;
        return;
    }
    if (!tab) return;
    for (unsigned long i = 0; i < nElement; ++i){
        fout << "[" << i << "]\t";
        for (unsigned long j = 0; j < dim; ++j){
            fout << tab[i*dim + j] << "\t";
        }
        fout << std::endl;
    }
}


void SaveIFSFloatArray(std::ofstream & fout, const std::string & name, unsigned int a, const Real * const tab, unsigned long nElement, unsigned long dim)
{
    if (tab)
    {
        fout << name << "\t" << a << "\t" << nElement << "\t" << dim << std::endl;
    }
    else
    {
        fout << name << "\t" << a << "\t" << 0 << "\t" << 0 << std::endl;
        return;
    }
    for (unsigned long i = 0; i < nElement; ++i){
        fout << "[" << i << "]\t";
        for (unsigned long j = 0; j < dim; ++j){
            fout << tab[i*dim + j] << "\t";
        }
        fout << std::endl;
    }
}
bool SaveIFS(std::string & fileName, const IndexedFaceSet<unsigned short> & ifs)
{
    std::ofstream fout;
    fout.open(fileName.c_str());
    if (!fout.fail())
    {
        
        
        SaveIFSUnsignedShortArray(fout, "* CoordIndex", 0, (const unsigned short * const) ifs.GetCoordIndex(), ifs.GetNCoordIndex(), 3);
        SaveIFSIntArray(fout, "* MatID", 0, (const long * const) ifs.GetIndexBufferID(), ifs.GetNCoordIndex(), 1);
        SaveIFSFloatArray(fout, "* Coord", 0, ifs.GetCoord(), ifs.GetNCoord(), 3);
        SaveIFSFloatArray(fout, "* Normal", 0, ifs.GetNormal(), ifs.GetNNormal(), 3);
        for(unsigned long a = 0; a < ifs.GetNumFloatAttributes(); ++a)
        {
            SaveIFSFloatArray(fout, "* FloatAttribute", a, ifs.GetFloatAttribute(a), ifs.GetNFloatAttribute(a), ifs.GetFloatAttributeDim(a));
        }
        for(unsigned long a = 0; a < ifs.GetNumIntAttributes(); ++a)
        {
            SaveIFSIntArray(fout, "* IntAttribute", a, ifs.GetIntAttribute(a), ifs.GetNIntAttribute(a), ifs.GetIntAttributeDim(a));
        }
        fout.close();
    }
    else
    {
        std::cout << "Not able to create file" << std::endl;
        return false;
    }
    return true;
}
#endif
namespace GLTF
{
    
#ifdef USE_OPEN3DGC
#if 1    
    void testDecode(shared_ptr <GLTFMesh> mesh, BinaryStream &bstream)
    {
        SC3DMCDecoder <unsigned short> decoder;
        IndexedFaceSet <unsigned short> ifs;
        unsigned char* outputData;
        
        decoder.DecodeHeader(ifs, bstream);
        
        unsigned int vertexSize = ifs.GetNCoord() * 3 * sizeof(float);
        unsigned int normalSize = ifs.GetNNormal() * 3 * sizeof(float);
        unsigned int texcoordSize = ifs.GetNFloatAttribute(0) * 2 * sizeof(float);
        unsigned int indicesSize = ifs.GetNCoordIndex() * 3 * sizeof(unsigned short);
        
        outputData = (unsigned char*)malloc(vertexSize + normalSize + texcoordSize + indicesSize);
        
        size_t vertexOffset = indicesSize;

        float* uncompressedVertices = (Real * const )(outputData + vertexOffset);
        
        ifs.SetCoordIndex((unsigned short * const ) outputData );
        ifs.SetCoord((Real * const )uncompressedVertices);
        
        if (ifs.GetNNormal() > 0) {
            ifs.SetNormal((Real * const )(outputData + indicesSize + vertexSize));
        }
        if (ifs.GetNFloatAttribute(0)) {
            ifs.SetFloatAttribute(0, (Real * const )(outputData + indicesSize + vertexSize + normalSize));
        }
        
        decoder.DecodePlayload(ifs, bstream);
        
        //---
        
        shared_ptr <GLTFMeshAttribute> meshAttribute = mesh->getMeshAttribute(POSITION, 0);
        
        meshAttribute->computeMinMax();
        const double* min = meshAttribute->getMin();
        const double* max = meshAttribute->getMax();
        
        float* vertices = (float*)meshAttribute->getBufferView()->getBufferDataByApplyingOffset();
        
        printf("coord nb:%d\n",(int)meshAttribute->getCount());
        printf("min: %f %f %f\n", min[0], min[1], min[2]);
        printf("max: %f %f %f\n", max[0], max[1], max[2]);
        
        float maxQuantError[3];
        maxQuantError[0] = (max[0] - min[0]) / (float)(2^12 - 1);
        maxQuantError[1] = (max[1] - min[1]) / (float)(2^12 - 1);
        maxQuantError[2] = (max[2] - min[2]) / (float)(2^12 - 1);
        
        if (meshAttribute->getCount() == ifs.GetNCoord()) {
            for (size_t i = 0 ; i < (meshAttribute->getCount() * 3) ; i++ ) {
                float error = vertices[i] - uncompressedVertices[i];
                
                if (error > maxQuantError[i%3]) {
                   printf("%d:input:%f compressed:%f\n",(int) i%3, vertices[i], uncompressedVertices[i]);
                   printf("delta is: %f\n", error);
                } else {
                    //printf("ok\n");
                }
            }
            
        } else {
            printf("Fatal error: vertex count do not match\n");
        }
        
        free(outputData);
        
    }
#endif
    
    //All these limitations will be fixed in coming iterations:
    //Do we have only triangles, only one set of texcoord and no skinning ?
    //TODO:Also check that the same buffer is not shared by 2 different semantics or set
    bool canEncodeOpen3DGCMesh(shared_ptr <GLTFMesh> mesh)
    {
        PrimitiveVector primitives = mesh->getPrimitives();
        unsigned int primitivesCount =  (unsigned int)primitives.size();
        
        for (unsigned int i = 0 ; i < primitivesCount ; i++) {
            shared_ptr<GLTF::GLTFPrimitive> primitive = primitives[i];
            if (primitive->getType() != "TRIANGLES") {
                return false;
            }
        }
            
        return true;
    }
    
    void encodeOpen3DGCMesh(shared_ptr <GLTFMesh> mesh,
                            SC3DMCEncodeParams &params,
                            IndexedFaceSet <unsigned short> &ifs,
                            shared_ptr<JSONObject> floatAttributeIndexMapping,
                            const GLTFConverterContext& converterContext)
    {
        //setup options
        int qcoord    = 12;
        int qtexCoord = 10;
        int qnormal   = 10;
        int qcolor   = 10;
        int qWeights = 8;
        
        GLTFOutputStream *outputStream = converterContext._compressionOutputStream;
        size_t bufferOffset = outputStream->length();
        
        O3DGCSC3DMCPredictionMode floatAttributePrediction = O3DGC_SC3DMC_PARALLELOGRAM_PREDICTION;
        
        unsigned int nFloatAttributes = 0;                
        
        PrimitiveVector primitives = mesh->getPrimitives();
        unsigned int primitivesCount =  (unsigned int)primitives.size();
        unsigned int allIndicesCount = 0;
        unsigned int allTrianglesCount = 0;

        std::vector <unsigned int> trianglesPerPrimitive;
        
        //First run through primitives to gather the number of indices and infer the number of triangles.
        for (unsigned int i = 0 ; i < primitivesCount ; i++) {
            shared_ptr<GLTF::GLTFPrimitive> primitive = primitives[i];
            shared_ptr <GLTF::GLTFIndices> uniqueIndices = primitive->getUniqueIndices();
            unsigned int indicesCount = (unsigned int)(uniqueIndices->getCount());
            //FIXME: assumes triangles, but we are guarded from issues by canEncodeOpen3DGCMesh
            allIndicesCount += indicesCount;
            trianglesPerPrimitive.push_back(indicesCount / 3);
        }
        
        //Then we setup the matIDs array and at the same time concatenate all triangle indices
        unsigned long *primitiveIDs = (unsigned long*)malloc(sizeof(unsigned long) * (allIndicesCount / 3));
        unsigned long *primitiveIDsPtr = primitiveIDs;
        unsigned short* allConcatenatedIndices = (unsigned short*)malloc(allIndicesCount * sizeof(unsigned short));
        unsigned short* allConcatenatedIndicesPtr = allConcatenatedIndices;

        for (unsigned int i = 0 ; i < trianglesPerPrimitive.size() ; i++) {
            unsigned int trianglesCount = trianglesPerPrimitive[i];
            for (unsigned int j = 0 ; j < trianglesCount ; j++) {
                primitiveIDsPtr[j] = i;
            }
            primitiveIDsPtr += trianglesCount;
            allTrianglesCount += trianglesCount;
            shared_ptr<GLTF::GLTFPrimitive> primitive = primitives[i];
            shared_ptr <GLTF::GLTFIndices> uniqueIndices = primitive->getUniqueIndices();
            unsigned int indicesCount = (unsigned int)(uniqueIndices->getCount());
            unsigned int* indicesPtr = (unsigned int*)uniqueIndices->getBufferView()->getBufferDataByApplyingOffset();
            for (unsigned int j = 0 ; j < indicesCount ; j++) {
                allConcatenatedIndicesPtr[j] = indicesPtr[j];
            }
            allConcatenatedIndicesPtr += indicesCount;
        }
        
        //FIXME:Open3DGC SetNCoordIndex is not a good name here (file against o3dgc)
        ifs.SetNCoordIndex(allTrianglesCount);
        ifs.SetCoordIndex((unsigned short * const ) allConcatenatedIndices);
        ifs.SetIndexBufferID(primitiveIDs);
        
        size_t vertexCount = 0;
        
        std::vector <GLTF::Semantic> semantics = mesh->allSemantics();
        for (unsigned int i = 0 ; i < semantics.size() ; i ++) {
            GLTF::Semantic semantic  = semantics[i];
            shared_ptr <GLTFMeshAttribute> meshAttribute = mesh->getMeshAttribute(semantic, 0);
            vertexCount = meshAttribute->getCount();
            size_t componentsPerAttribute = meshAttribute->getComponentsPerAttribute();
            char *buffer = (char*)meshAttribute->getBufferView()->getBufferDataByApplyingOffset();
            switch (semantic) {
                case POSITION:
                    params.SetCoordQuantBits(qcoord);
                    params.SetCoordPredMode(floatAttributePrediction);
                    ifs.SetNCoord(vertexCount);
                    ifs.SetCoord((Real * const)buffer);
                    break;
                case NORMAL:
                    params.SetNormalQuantBits(qnormal);
                    params.SetNormalPredMode(O3DGC_SC3DMC_SURF_NORMALS_PREDICTION);
                    ifs.SetNNormal(vertexCount);
                    ifs.SetNormal((Real * const)buffer);
                    break;
                case TEXCOORD:
                    params.SetFloatAttributeQuantBits(nFloatAttributes, qtexCoord);
                    params.SetFloatAttributePredMode(nFloatAttributes, floatAttributePrediction);
                    ifs.SetNFloatAttribute(nFloatAttributes, vertexCount);
                    ifs.SetFloatAttributeDim(nFloatAttributes, componentsPerAttribute);
                    ifs.SetFloatAttributeType(nFloatAttributes, O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_TEXCOORD);
                    ifs.SetFloatAttribute(nFloatAttributes, (Real * const)buffer);
                    floatAttributeIndexMapping->setUnsignedInt32(meshAttribute->getID(), nFloatAttributes);
                    nFloatAttributes++;
                    break;
                case COLOR:
                    params.SetFloatAttributeQuantBits(nFloatAttributes, qcolor);
                    params.SetFloatAttributePredMode(nFloatAttributes, floatAttributePrediction);
                    ifs.SetNFloatAttribute(nFloatAttributes, vertexCount);
                    ifs.SetFloatAttributeDim(nFloatAttributes, componentsPerAttribute);
                    ifs.SetFloatAttributeType(nFloatAttributes, O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_COLOR);
                    ifs.SetFloatAttribute(nFloatAttributes, (Real * const)buffer);
                    floatAttributeIndexMapping->setUnsignedInt32(meshAttribute->getID(), nFloatAttributes);
                    nFloatAttributes++;
                    break;
                case WEIGHT:
                    params.SetFloatAttributeQuantBits(nFloatAttributes, qWeights);
                    params.SetFloatAttributePredMode(nFloatAttributes, O3DGC_SC3DMC_DIFFERENTIAL_PREDICTION);
                    ifs.SetNFloatAttribute(nFloatAttributes, vertexCount);
                    ifs.SetFloatAttributeDim(nFloatAttributes, componentsPerAttribute);
                    //ifs.SetFloatAttributeType(nFloatAttributes, O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_WEIGHT);
                    ifs.SetFloatAttributeType(nFloatAttributes, O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_TEXCOORD);
                    ifs.SetFloatAttribute(nFloatAttributes, (Real * const)buffer);
                    floatAttributeIndexMapping->setUnsignedInt32(meshAttribute->getID(), nFloatAttributes);
                    nFloatAttributes++;
                    break;
                case JOINT:
                    
                    params.SetFloatAttributeQuantBits(nFloatAttributes, 10);
                    params.SetFloatAttributePredMode(nFloatAttributes, O3DGC_SC3DMC_DIFFERENTIAL_PREDICTION);
                    ifs.SetNFloatAttribute(nFloatAttributes, vertexCount);
                    ifs.SetFloatAttributeDim(nFloatAttributes, componentsPerAttribute);
                    ifs.SetFloatAttributeType(nFloatAttributes, O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_TEXCOORD);
                    ifs.SetFloatAttribute(nFloatAttributes, (Real * const)buffer);
                    floatAttributeIndexMapping->setUnsignedInt32(meshAttribute->getID(), nFloatAttributes);
                    nFloatAttributes++;
                    break;
                default:
                    break;
            }
            
        }
        params.SetNumFloatAttributes(nFloatAttributes);
        ifs.SetNumFloatAttributes(nFloatAttributes);
        shared_ptr<JSONObject> compressionObject = static_pointer_cast<JSONObject>(mesh->getExtensions()->createObjectIfNeeded("Open3DGC-compression"));
        
        ifs.ComputeMinMax(O3DGC_SC3DMC_MAX_ALL_DIMS);
        BinaryStream bstream(vertexCount * 8);
        SC3DMCEncoder <unsigned short> encoder;
        shared_ptr<JSONObject> compressedData(new JSONObject());
        compressedData->setInt32("verticesCount", vertexCount);
        compressedData->setInt32("indicesCount", allIndicesCount);
        //Open3DGC binary is disabled
        params.SetStreamType(converterContext.compressionMode == "binary" ? O3DGC_STREAM_TYPE_BINARY : O3DGC_STREAM_TYPE_ASCII);
#if DUMP_O3DGC_OUTPUT
        static int dumpedId = 0;
        COLLADABU::URI outputURI(converterContext.outputFilePath.c_str());
        std::string outputFilePath = outputURI.getPathDir() + GLTFUtils::toString(dumpedId) + ".txt";
        dumpedId++;
        SaveIFS(outputFilePath, ifs);
#endif
        encoder.Encode(params, ifs, bstream);
        
        compressedData->setString("mode", converterContext.compressionMode);
        compressedData->setUnsignedInt32("count", bstream.GetSize());
        compressedData->setUnsignedInt32("type", converterContext.profile->getGLenumForString("UNSIGNED_BYTE"));
        compressedData->setUnsignedInt32("byteOffset", bufferOffset);
        compressedData->setValue("floatAttributesIndexes", floatAttributeIndexMapping);
        
        compressionObject->setValue("compressedData", compressedData);
        
        //testDecode(mesh, bstream);
        outputStream->write((const char*)bstream.GetBuffer(0), bstream.GetSize());
        
        if (ifs.GetCoordIndex()) {
            free(ifs.GetCoordIndex());
        }
        
        if (primitiveIDs) {
            free(primitiveIDs);
        }
    }

#endif


    bool writeAllMeshBuffers(shared_ptr <GLTFMesh> mesh, const GLTFConverterContext& converterContext)
    {
        bool shouldOGCompressMesh = false;
        
        GLTFOutputStream* vertexOutputStream = converterContext._vertexOutputStream;
        GLTFOutputStream* indicesOutputStream = converterContext._indicesOutputStream;
        GLTFOutputStream* compressionOutputStream = converterContext._compressionOutputStream;
        
#ifdef USE_WEBGLLOADER

        if (mesh->getExtensions()->contains("won-compression")) {
            shared_ptr<JSONObject> compressionObject = static_pointer_cast<JSONObject>(mesh->getExtensions()->getValue("won-compression"));
            
            shared_ptr<GLTFBuffer> buffer = mesh->getCompressedBuffer();
            
            shared_ptr<JSONObject> compressedData(new JSONObject());
            compressedData->setUnsignedInt32("count", buffer->getByteLength());
            compressedData->setUnsignedInt32("type", converterContext.profile->getGLenumForString("UNSIGNED_BYTE"));
            compressedData->setUnsignedInt32("byteOffset", static_cast<size_t>(genericStream.tellp()));
            compressionObject->setValue("compressedData", compressedData);
            genericStream.write((const char*)buffer->getData(), buffer->getByteLength());
            
            shared_ptr <MeshAttributeVector> attributes = mesh->meshAttributes();
            for (size_t i = 0 ; i < attributes->size() ; i++) {
                shared_ptr <GLTFMeshAttribute> meshAttribute = (*attributes)[i];
                meshAttribute->computeMinMax();
            }
            //bufferView will be set when the mesh is serialized
            return true;
        }
#endif
        
#ifdef USE_OPEN3DGC
        SC3DMCEncodeParams params;
        IndexedFaceSet <unsigned short> ifs;
        shared_ptr <JSONObject> floatAttributeIndexMapping(new JSONObject());
        shouldOGCompressMesh = (converterContext.compressionType == "Open3DGC") && canEncodeOpen3DGCMesh(mesh);
        unsigned compressedBufferStart = compressionOutputStream->length();
        if (shouldOGCompressMesh) {
            encodeOpen3DGCMesh(mesh, params, ifs, floatAttributeIndexMapping, converterContext);
        }
#endif
        typedef std::map<std::string , shared_ptr<GLTF::GLTFBuffer> > IDToBufferDef;
        IDToBufferDef IDToBuffer;
        
        shared_ptr <MeshAttributeVector> allMeshAttributes = mesh->meshAttributes();
        shared_ptr <GLTFBufferView> dummyBuffer(new GLTFBufferView());
        int vertexCount;
        unsigned int indicesCount, allIndicesCount = 0;
        PrimitiveVector primitives = mesh->getPrimitives();
        unsigned int primitivesCount =  (unsigned int)primitives.size();
        for (unsigned int i = 0 ; i < primitivesCount ; i++) {
            shared_ptr<GLTF::GLTFPrimitive> primitive = primitives[i];
            shared_ptr <GLTF::GLTFIndices> uniqueIndices = primitive->getUniqueIndices();
            /*
             Convert the indices to unsigned short and write the blob
             */
            indicesCount = (unsigned int)uniqueIndices->getCount();
            shared_ptr <GLTFBufferView> indicesBufferView = uniqueIndices->getBufferView();
            unsigned int* uniqueIndicesBuffer = (unsigned int*) indicesBufferView->getBufferDataByApplyingOffset();
            if (indicesCount <= 0) {
                // FIXME: report error
            } else {
                allIndicesCount += indicesCount;
                size_t indicesLength = sizeof(unsigned short) * indicesCount;
                unsigned short* ushortIndices = 0;
                
                if (!shouldOGCompressMesh) {
                    ushortIndices = (unsigned short*)calloc(indicesLength, 1);
                    for (unsigned int idx = 0 ; idx < indicesCount ; idx++) {
                        ushortIndices[idx] = (unsigned short)uniqueIndicesBuffer[idx];
                    }
                    uniqueIndices->setByteOffset(indicesOutputStream->length());
                    indicesOutputStream->write((const char*)ushortIndices, indicesLength);
                }
#ifdef USE_OPEN3DGC
                else {
                    uniqueIndices->setByteOffset(compressedBufferStart);
                    compressedBufferStart += indicesLength; //we simulate how will be the uncompressed data here, so this is the length in short *on purpose*
                }
#endif
                //now that we wrote to the stream we can release the buffer.
                uniqueIndices->setBufferView(dummyBuffer);
                
                if (!shouldOGCompressMesh)
                    free(ushortIndices);
            }
        }
        
        int attributeCount = 0;
        for (unsigned int j = 0 ; j < allMeshAttributes->size() ; j++) {
            shared_ptr <GLTFMeshAttribute> meshAttribute = (*allMeshAttributes)[j];
            shared_ptr <GLTFBufferView> bufferView = meshAttribute->getBufferView();
            shared_ptr <GLTFBuffer> buffer = bufferView->getBuffer();
            
            if (!bufferView.get()) {
                // FIXME: report error
                return false;
            }
            
            if (!IDToBuffer[bufferView->getBuffer()->getID()].get()) {
                meshAttribute->computeMinMax();
                
                vertexCount = meshAttribute->getCount();
                attributeCount++;
#ifdef USE_OPEN3DGC
                if (shouldOGCompressMesh) {
                    meshAttribute->setByteOffset(compressedBufferStart);
                    compressedBufferStart += buffer->getByteLength();
                } else
#endif
                {
                    meshAttribute->setByteOffset(vertexOutputStream->length());
                    vertexOutputStream->write(buffer);
                }
                
                //now that we wrote to the stream we can release the buffer.
                meshAttribute->setBufferView(dummyBuffer);
                IDToBuffer[bufferView->getBuffer()->getID()] = buffer;
            }
        }
        
        return true;
    }

    
    
#ifdef USE_WEBGLLOADER
    //webgl-loader compression
    static void __FeedAttribs(void *value,
                              GLTF::ComponentType type,
                              size_t componentsPerAttribute,
                              size_t index,
                              size_t vertexAttributeByteSize,
                              void *context) {
        char* bufferData = (char*)value;
        AttribList *attribList = (AttribList*)context;
        
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
    
    
    bool compress(shared_ptr <GLTF::GLTFMesh> mesh) {
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
                webgl_loader::AttribsToQuantizedAttribs(draw_mesh.attribs, bounds_params,&quantized_attribs);
                VertexOptimizer vertex_optimizer(quantized_attribs);
                const std::vector<GroupStart>& group_starts = iter->second.group_starts();
                WebGLMeshList webgl_meshes;
                std::vector<size_t> group_lengths;
                for (size_t i = 1; i < group_starts.size(); ++i) {
                    const size_t here = group_starts[i-1].offset;
                    const size_t length = group_starts[i].offset - here;
                    group_lengths.push_back(length);
                    vertex_optimizer.AddTriangles(&draw_mesh.indices[here], length,&webgl_meshes);
                }
                const size_t here = group_starts.back().offset;
                const size_t length = draw_mesh.indices.size() - here;
                CHECK(length % 3 == 0);
                group_lengths.push_back(length);
                vertex_optimizer.AddTriangles(&draw_mesh.indices[here], length,&webgl_meshes);
                
                std::vector<std::string> material;
                std::vector<size_t> attrib_start, attrib_length, index_start, index_length;
                for (size_t i = 0; i < webgl_meshes.size(); ++i) {
                    const size_t num_attribs = webgl_meshes[i].attribs.size();
                    const size_t num_indices = webgl_meshes[i].indices.size();
                    CHECK(num_attribs % 8 == 0);
                    CHECK(num_indices % 3 == 0);
                    webgl_loader::CompressQuantizedAttribsToUtf8(webgl_meshes[i].attribs, &utf8_sink);
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
    
    
    
    
#if 0
    bool compress(shared_ptr <GLTF::GLTFMesh> mesh) {
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
        
        const DrawMesh& draw_mesh = drawBatch->draw_mesh();
    
        QuantizedAttribList quantized_attribs;
        
        
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
        }
        
        WebGLMeshList webgl_meshes;
        
        webgl_meshes.push_back(WebGLMesh());

        // Pass 1: compute bounds.
        webgl_loader::Bounds bounds;
        bounds.Clear();
        bounds.Enclose(drawBatch->draw_mesh().attribs);
        FILE* json_out = stdout;
        
        webgl_loader::BoundsParams bounds_params =
        webgl_loader::BoundsParams::FromBounds(bounds);
        
        shared_ptr<JSONObject> decodeParams = getDecodeParams(bounds_params);
        
        webgl_loader::AttribsToQuantizedAttribs(draw_mesh.attribs, bounds_params,
                                                &quantized_attribs);
        
        fputs("  \"decodeParams\": ", json_out);
        bounds_params.DumpJson(json_out);
        fputs(", \"urls\": {\n", json_out);
        // Pass 2: quantize, optimize, compress, report.
        //FILE* utf8_out_fp = fopen("testglloader.bin", "wb");
        //CHECK(utf8_out_fp != NULL);
        fprintf(json_out, "    \"%s\": [\n", "testglloader.bin");
        
        size_t offset = 0;
        
        VertexOptimizer vertex_optimizer(quantized_attribs);
        const size_t length = draw_mesh.indices.size();
        CHECK(length % 3 == 0);
        
        bool *knownIndices = (bool*)malloc(sizeof(bool) * quantized_attribs.size());
        memset(knownIndices, 0, sizeof(bool) * quantized_attribs.size());
        
        const int* drawMeshIndices = &draw_mesh.indices[0];
        for (size_t k = 0 ; k <  length ; k++) {            
            webgl_meshes[0].indices.push_back(drawMeshIndices[k]);
            if (knownIndices[drawMeshIndices[k]] == false) {
                for (size_t j = 0; j < 8; ++j) {
                    webgl_meshes[0].attribs.push_back(quantized_attribs[8*drawMeshIndices[k] + j]);
                }
                knownIndices[drawMeshIndices[k]] = true;
            }
        }
        free(knownIndices);
        
        webgl_loader::CompressQuantizedAttribsToUtf8(webgl_meshes[0].attribs, &utf8_sink);

        std::vector<std::string> material;
        std::vector<size_t> attrib_start, attrib_length, index_start, index_length;
        
        vertex_optimizer.AddTriangles(&draw_mesh.indices[0], length, &webgl_meshes);
        
        
        for (size_t i = 0; i < webgl_meshes.size(); ++i) {
            const size_t num_attribs = webgl_meshes[i].attribs.size();
            const size_t num_indices = webgl_meshes[i].indices.size();
            CHECK(num_attribs % 8 == 0);
            CHECK(num_indices % 3 == 0);
            webgl_loader::CompressIndicesToUtf8(webgl_meshes[i].indices, &utf8_sink);
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
        
        printf("compressed output size:%d\n",(int)compressedOutput.size());
        
        //FIXME: remove that crappy copy here
        char *bufferPtr = ( char*)malloc(compressedOutput.size());
        for (size_t idx = 0 ; idx < compressedOutput.size() ; idx++) {
            bufferPtr[idx] = compressedOutput[idx];
        }
        shared_ptr<GLTFBuffer> compressedBuffer(new GLTFBuffer(bufferPtr, compressedOutput.size(), true));
        
        mesh->setCompressedBuffer(compressedBuffer);
        
        
        return true;
    }
#endif
    
#endif
    //
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
    
    
    static unsigned int ConvertOpenCOLLADAMeshVertexDataToGLTFMeshAttributes(const COLLADAFW::MeshVertexData &vertexData, GLTF::IndexSetToMeshAttributeHashmap &meshAttributes, size_t allowedComponentsPerAttribute)
    {
        // The following are OpenCOLLADA fmk issues preventing doing a totally generic processing of sources
        //1. "set"(s) other than texCoord don't have valid input infos
        //2. not the original id in the source
        
        std::string id;
        size_t length, elementsCount;
        size_t stride = 0;
        size_t componentsPerAttribute = 0;
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
                componentsPerAttribute = vertexData.getStride(indexOfSet);
                inputLength = vertexData.getLength(indexOfSet);
            } else {
                // for unpatched version of OpenCOLLADA we need this work-around.
                id = GLTF::GLTFUtils::generateIDForType("buffer").c_str();
                componentsPerAttribute = 3; //only normal and positions should reach this code
                inputLength = vertexData.getLength(0);
            }
            
            length = inputLength ? inputLength : vertexData.getValuesCount();
            elementsCount = length / componentsPerAttribute;
            unsigned char *sourceData = 0;
            size_t sourceSize = 0;
            
            GLTF::ComponentType componentType = GLTF::NOT_AN_ELEMENT_TYPE;
            switch (vertexData.getType()) {
                case COLLADAFW::MeshVertexData::DATA_TYPE_FLOAT: {
                    componentType = GLTF::FLOAT;
                    stride = sizeof(float) * componentsPerAttribute;
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
            if (allowedComponentsPerAttribute != componentsPerAttribute) {
                sourceSize = elementsCount * sizeof(float) * allowedComponentsPerAttribute;
                
                float *adjustedSource = (float*)malloc(sourceSize);
                float *originalSource = (float*)sourceData;
                size_t adjustedStride = sizeof(float) * allowedComponentsPerAttribute;
                
                if (allowedComponentsPerAttribute < componentsPerAttribute) {
                    for (size_t i = 0 ; i < elementsCount ; i++) {
                        for (size_t j= 0 ; j < allowedComponentsPerAttribute ; j++) {
                            adjustedSource[(i*allowedComponentsPerAttribute) + j] = originalSource[(i*componentsPerAttribute) + j];
                        }
                    }
                } else {
                    //FIXME: unlikely but should be taken care of
                }

                //Free source before replacing it
                if (meshAttributeOwnsBuffer) {
                    free(sourceData);
                }
                
                componentsPerAttribute = allowedComponentsPerAttribute;
                meshAttributeOwnsBuffer = true;
                sourceData = (unsigned char*)adjustedSource;
                stride = adjustedStride;
            }
            
            // FIXME: the source could be shared, store / retrieve it here
            shared_ptr <GLTFBufferView> cvtBufferView = createBufferViewWithAllocatedBuffer(id, sourceData, 0, sourceSize, meshAttributeOwnsBuffer);
            shared_ptr <GLTFMeshAttribute> cvtMeshAttribute(new GLTFMeshAttribute());
            
            cvtMeshAttribute->setBufferView(cvtBufferView);
            cvtMeshAttribute->setComponentsPerAttribute(componentsPerAttribute);
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
    
    shared_ptr<GLTFMesh> convertOpenCOLLADAMesh(COLLADAFW::Mesh* openCOLLADAMesh, GLTFConverterContext& context) {
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
                    printf("WARNING: some primitives failed to convert\nCurrently supported are TRIANGLES, POLYLIST and POLYGONS\nMore: https://github.com/KhronosGroup/glTF/issues/129\nand https://github.com/KhronosGroup/glTF/issues/135\n");
                    printedOnce = true;
                }
                
                continue;
            }
            
            shared_ptr <GLTF::IndicesVector> primitiveIndicesVector(new GLTF::IndicesVector());
            allPrimitiveIndicesVectors.push_back(primitiveIndicesVector);
            
            shared_ptr <GLTF::GLTFPrimitive> primitive = ConvertOpenCOLLADAMeshPrimitive(primitives[i],*primitiveIndicesVector);
            cvtMesh->appendPrimitive(primitive);
            
            VertexAttributeVector vertexAttributes = primitive->getVertexAttributes();
            primitiveIndicesVector = allPrimitiveIndicesVectors[allPrimitiveIndicesVectors.size()-1];
            
            // once we got a primitive, keep track of its meshAttributes
            std::vector< shared_ptr<GLTF::GLTFIndices> > allIndices = *primitiveIndicesVector;
            for (size_t k = 0 ; k < allIndices.size() ; k++) {
                shared_ptr<GLTF::GLTFIndices> indices = allIndices[k];
                GLTF::Semantic semantic = vertexAttributes[k]->getSemantic();
                GLTF::IndexSetToMeshAttributeHashmap& meshAttributes = cvtMesh->getMeshAttributesForSemantic(semantic);
                
                switch (semantic) {
                    case GLTF::POSITION:
                        ConvertOpenCOLLADAMeshVertexDataToGLTFMeshAttributes(openCOLLADAMesh->getPositions(), meshAttributes, 3);
                        break;
                        
                    case GLTF::NORMAL:
                        ConvertOpenCOLLADAMeshVertexDataToGLTFMeshAttributes(openCOLLADAMesh->getNormals(), meshAttributes, 3);
                        break;
                        
                    case GLTF::TEXCOORD:
                        ConvertOpenCOLLADAMeshVertexDataToGLTFMeshAttributes(openCOLLADAMesh->getUVCoords(), meshAttributes,2);
                        break;
                        
                    case GLTF::COLOR:
                        ConvertOpenCOLLADAMeshVertexDataToGLTFMeshAttributes(openCOLLADAMesh->getColors(), meshAttributes, 4);
                        break;
                        
                    default:
                        break;
                }
                
                cvtMesh->setMeshAttributesForSemantic(semantic, meshAttributes);
            }
        }
        
        if (cvtMesh->hasSemantic(GLTF::TEXCOORD)) {
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
        }
        
        if (cvtMesh->getPrimitives().size() > 0) {
            //After this point cvtMesh should be referenced anymore and will be deallocated
            return createUnifiedIndexesMeshFromMesh(cvtMesh.get(), allPrimitiveIndicesVectors);
        }

        return shared_ptr <GLTF::GLTFMesh> ((GLTFMesh*)0);
    }
}

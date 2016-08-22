// Copyright (c) 2013, Fabrice Robinet.
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
#include "../GLTFOpenCOLLADA.h"
#include "GLTFAsset.h"
#include "GLTF-Open3DGC.h"

#include "o3dgcSC3DMCEncodeParams.h"
#include "o3dgcIndexedFaceSet.h"
#include "o3dgcSC3DMCEncoder.h"
#include "o3dgcSC3DMCDecoder.h"

#include "o3dgcTimer.h"
#include "o3dgcDVEncodeParams.h"
#include "o3dgcDynamicVectorEncoder.h"
#include "o3dgcDynamicVectorDecoder.h"

#define DUMP_O3DGC_OUTPUT 0

using namespace o3dgc;
#if __cplusplus <= 199711L
using namespace std::tr1;
#endif
using namespace std;

namespace GLTF
{
    
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
#if 0
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
        
        shared_ptr <GLTFAccessor> meshAttribute = mesh->getMeshAttribute(POSITION, 0);
        
        const double* min = meshAttribute->getMin();
        const double* max = meshAttribute->getMax();
        
        float* vertices = (float*)meshAttribute->getBufferView()->getBufferDataByApplyingOffset();
        
        printf("coord nb:%d\n",(int)meshAttribute->getCount());
        printf("min: %f %f %f\n", min[0], min[1], min[2]);
        printf("max: %f %f %f\n", max[0], max[1], max[2]);
        float maxQuantError[3];
        maxQuantError[0] = (float)(max[0] - min[0]) / (2^12 - 1);
        maxQuantError[1] = (float)(max[1] - min[1]) / (2^12 - 1);
        maxQuantError[2] = (float)(max[2] - min[2]) / (2^12 - 1);
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
    bool canEncodeOpen3DGCMesh(shared_ptr <GLTFMesh> mesh, shared_ptr<GLTFProfile> profile)
    {
        JSONValueVector primitives = mesh->getPrimitives()->values();
        unsigned int primitivesCount =  (unsigned int)primitives.size();
        
        for (unsigned int i = 0 ; i < primitivesCount ; i++) {
            shared_ptr<GLTF::GLTFPrimitive> primitive = static_pointer_cast<GLTFPrimitive>(primitives[i]);
            if (primitive->getMode() != profile->getGLenumForString("TRIANGLES")) {
                return false;
            }
        }
        
        return true;
    }
    
    O3DGCSC3DMCPredictionMode _predictionModeForString(const std::string &prediction)
    {
        O3DGCSC3DMCPredictionMode o3dPredictionMode = O3DGC_SC3DMC_PARALLELOGRAM_PREDICTION;
        
        if (prediction == "PARALLELOGRAM") {
            o3dPredictionMode = O3DGC_SC3DMC_PARALLELOGRAM_PREDICTION;
        } else if (prediction == "DIFFERENTIAL") {
            o3dPredictionMode = O3DGC_SC3DMC_DIFFERENTIAL_PREDICTION;
        } else if (prediction == "NORMAL") {
            o3dPredictionMode = O3DGC_SC3DMC_SURF_NORMALS_PREDICTION;
        }
        
        return o3dPredictionMode;
    }
    
    void encodeOpen3DGCMesh(shared_ptr <GLTFMesh> mesh,
                            shared_ptr<JSONObject> floatAttributeIndexMapping,
                            GLTFAsset* asset)
    {
        o3dgc::SC3DMCEncodeParams params;
        o3dgc::IndexedFaceSet <unsigned short> ifs;
        shared_ptr <GLTFConfig> config = asset->converterConfig();
        
        //setup options
        int qcoord    = 12;
        int qtexCoord = 10;
        int qnormal   = 10;
        int qcolor   = 10;
        int qWeights = 8;
        
        qcoord = config->unsignedInt32ForKeyPath("extensions.Open3DGC.quantization.POSITION");
        qnormal = config->unsignedInt32ForKeyPath("extensions.Open3DGC.quantization.NORMAL");
        qtexCoord = config->unsignedInt32ForKeyPath("extensions.Open3DGC.quantization.TEXCOORD");
        qcolor = config->unsignedInt32ForKeyPath("extensions.Open3DGC.quantization.COLOR");
        qWeights = config->unsignedInt32ForKeyPath("extensions.Open3DGC.quantization.WEIGHT");
        
        O3DGCSC3DMCPredictionMode positionPrediction = _predictionModeForString(config->stringForKeyPath("extensions.Open3DGC.quantization.POSITION"));
        O3DGCSC3DMCPredictionMode texcoordPrediction = _predictionModeForString(config->stringForKeyPath("extensions.Open3DGC.quantization.TEXCOORD"));
        O3DGCSC3DMCPredictionMode normalPrediction = _predictionModeForString(config->stringForKeyPath("extensions.Open3DGC.quantization.NORMAL"));
        O3DGCSC3DMCPredictionMode colorPrediction = _predictionModeForString(config->stringForKeyPath("extensions.Open3DGC.quantization.COLOR"));
        O3DGCSC3DMCPredictionMode weightPrediction = _predictionModeForString(config->stringForKeyPath("extensions.Open3DGC.quantization.WEIGHT"));
        O3DGCSC3DMCPredictionMode jointPrediction = _predictionModeForString(config->stringForKeyPath("extensions.Open3DGC.quantization.JOINT"));
        
        GLTFOutputStream *outputStream = asset->createOutputStreamIfNeeded(kCompressionOutputStream).get();
        int bufferOffset = (int)outputStream->length();
        
        O3DGCSC3DMCPredictionMode floatAttributePrediction = O3DGC_SC3DMC_PARALLELOGRAM_PREDICTION;
        
        unsigned int nFloatAttributes = 0;
        
        GLTF::JSONValueVector primitives = mesh->getPrimitives()->values();
        unsigned int primitivesCount =  (unsigned int)primitives.size();
        unsigned int allIndicesCount = 0;
        unsigned int allTrianglesCount = 0;
        
        std::vector <unsigned int> trianglesPerPrimitive;
        
        //First run through primitives to gather the number of indices and infer the number of triangles.
        for (unsigned int i = 0 ; i < primitivesCount ; i++) {
            shared_ptr<GLTF::GLTFPrimitive> primitive = static_pointer_cast<GLTFPrimitive>(primitives[i]);
            shared_ptr <GLTF::GLTFAccessor> uniqueIndices = primitive->getIndices();
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
            shared_ptr<GLTF::GLTFPrimitive> primitive = static_pointer_cast<GLTFPrimitive>(primitives[i]);
            shared_ptr <GLTF::GLTFAccessor> uniqueIndices = primitive->getIndices();
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
        
        long vertexCount = 0;
        
        std::vector <GLTF::Semantic> semantics = mesh->allSemantics();
        for (unsigned int i = 0 ; i < semantics.size() ; i ++) {
            GLTF::Semantic semantic  = semantics[i];
            
            size_t attributesCount = mesh->getMeshAttributesCountForSemantic(semantic);
            
            for (size_t j = 0 ; j < attributesCount ; j++) {
                shared_ptr <GLTFAccessor> meshAttribute = mesh->getMeshAttribute(semantic, j);
                vertexCount = (long)meshAttribute->getCount();
                long componentsPerElement = (long)meshAttribute->componentsPerElement();
                char *buffer = (char*)meshAttribute->getBufferView()->getBufferDataByApplyingOffset();
                switch (semantic) {
                    case POSITION:
                        params.SetCoordQuantBits(qcoord);
                        params.SetCoordPredMode(positionPrediction);
                        params.SetCoordPredMode(floatAttributePrediction);
                        ifs.SetNCoord(vertexCount);
                        ifs.SetCoord((Real * const)buffer);
                        break;
                    case NORMAL:
                        params.SetNormalQuantBits(qnormal);
                        params.SetNormalPredMode(normalPrediction);
                        ifs.SetNNormal(vertexCount);
                        ifs.SetNormal((Real * const)buffer);
                        break;
                    case TEXCOORD:
                        params.SetFloatAttributeQuantBits(nFloatAttributes, qtexCoord);
                        params.SetFloatAttributePredMode(nFloatAttributes, texcoordPrediction);
                        ifs.SetNFloatAttribute(nFloatAttributes, vertexCount);
                        ifs.SetFloatAttributeDim(nFloatAttributes, componentsPerElement);
                        ifs.SetFloatAttributeType(nFloatAttributes, O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_TEXCOORD);
                        ifs.SetFloatAttribute(nFloatAttributes, (Real * const)buffer);
                        floatAttributeIndexMapping->setUnsignedInt32(meshAttribute->getID(), nFloatAttributes);
                        nFloatAttributes++;
                        break;
                    case COLOR:
                        params.SetFloatAttributeQuantBits(nFloatAttributes, qcolor);
                        params.SetFloatAttributePredMode(nFloatAttributes, colorPrediction);
                        ifs.SetNFloatAttribute(nFloatAttributes, vertexCount);
                        ifs.SetFloatAttributeDim(nFloatAttributes, componentsPerElement);
                        ifs.SetFloatAttributeType(nFloatAttributes, O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_COLOR);
                        ifs.SetFloatAttribute(nFloatAttributes, (Real * const)buffer);
                        floatAttributeIndexMapping->setUnsignedInt32(meshAttribute->getID(), nFloatAttributes);
                        nFloatAttributes++;
                        break;
                    case WEIGHT:
                        params.SetFloatAttributeQuantBits(nFloatAttributes, qWeights);
                        params.SetFloatAttributePredMode(nFloatAttributes, weightPrediction);
                        ifs.SetNFloatAttribute(nFloatAttributes, vertexCount);
                        ifs.SetFloatAttributeDim(nFloatAttributes, componentsPerElement);
                        ifs.SetFloatAttributeType(nFloatAttributes, O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_WEIGHT);
                        ifs.SetFloatAttribute(nFloatAttributes, (Real * const)buffer);
                        floatAttributeIndexMapping->setUnsignedInt32(meshAttribute->getID(), nFloatAttributes);
                        nFloatAttributes++;
                        break;
                    case JOINT:
                        /*
                         params.SetIntAttributePredMode(nIntAttributes, O3DGC_SC3DMC_DIFFERENTIAL_PREDICTION);
                         ifs.SetNIntAttribute(nIntAttributes, jointIDs.size() / numJointsPerVertex);
                         ifs.SetIntAttributeDim(nIntAttributes, numJointsPerVertex);
                         ifs.SetIntAttributeType(nIntAttributes, O3DGC_IFS_INT_ATTRIBUTE_TYPE_JOINT_ID);
                         ifs.SetIntAttribute(nIntAttributes, (long * const ) & (jointIDs[0]));
                         nIntAttributes++;
                         */
                        params.SetFloatAttributeQuantBits(nFloatAttributes, 10);
                        params.SetFloatAttributePredMode(nFloatAttributes, jointPrediction);
                        ifs.SetNFloatAttribute(nFloatAttributes, vertexCount);
                        ifs.SetFloatAttributeDim(nFloatAttributes, componentsPerElement);
                        ifs.SetFloatAttributeType(nFloatAttributes, O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_UNKOWN);
                        ifs.SetFloatAttribute(nFloatAttributes, (Real * const)buffer);
                        floatAttributeIndexMapping->setUnsignedInt32(meshAttribute->getID(), nFloatAttributes);
                        nFloatAttributes++;
                        break;
                    default:
                        break;
                }
            }
        }
        
        params.SetNumFloatAttributes(nFloatAttributes);
        ifs.SetNumFloatAttributes(nFloatAttributes);
        shared_ptr<JSONObject> compressionObject = static_pointer_cast<JSONObject>(mesh->getExtensions()->createObjectIfNeeded("Open3DGC-compression"));
        
        ifs.ComputeMinMax(O3DGC_SC3DMC_MAX_ALL_DIMS);
        BinaryStream bstream(vertexCount * 8);
        SC3DMCEncoder <unsigned short> encoder;
        shared_ptr<JSONObject> compressedData(new JSONObject());
        compressedData->setInt32("verticesCount", (int)vertexCount);
        compressedData->setInt32("indicesCount", allIndicesCount);
        //Open3DGC binary is disabled
        params.SetStreamType(CONFIG_STRING(asset, "compressionMode") == "binary" ? O3DGC_STREAM_TYPE_BINARY : O3DGC_STREAM_TYPE_ASCII);
#if DUMP_O3DGC_OUTPUT
        static int dumpedId = 0;
        COLLADABU::URI outputURI(asset->outputFilePath.c_str());
        std::string outputFilePath = outputURI.getPathDir() + GLTFUtils::toString(dumpedId) + ".txt";
        dumpedId++;
        SaveIFS(outputFilePath, ifs);
#endif
        encoder.Encode(params, ifs, bstream);
        
        compressedData->setString("mode", CONFIG_STRING(asset, "compressionMode") );
        compressedData->setUnsignedInt32("count", bstream.GetSize());
        compressedData->setString(kType, "SCALAR");
        compressedData->setUnsignedInt32(kComponentType, asset->profile()->getGLenumForString("UNSIGNED_BYTE"));
        compressedData->setUnsignedInt32("byteOffset", bufferOffset);
        compressedData->setValue("floatAttributesIndexes", floatAttributeIndexMapping);
        
        compressionObject->setValue("compressedData", compressedData);
        
        //testDecode(mesh, bstream);
        outputStream->write((const char*)bstream.GetBuffer(0), bstream.GetSize());
        asset->setGeometryByteLength(asset->getGeometryByteLength() + bstream.GetSize());

        if (ifs.GetCoordIndex()) {
            free(ifs.GetCoordIndex());
        }
        
        if (primitiveIDs) {
            free(primitiveIDs);
        }
    }
    
    void encodeDynamicVector(float *buffer, const std::string &path, long componentsCount, long count, GLTFAsset* asset) {
        GLTFOutputStream *outputStream = asset->createOutputStreamIfNeeded(kCompressionOutputStream).get();
        Real max[32];
        Real min[32];
        O3DGCStreamType streamType = CONFIG_STRING(asset, "compressionMode")  == "ascii" ? O3DGC_STREAM_TYPE_ASCII : O3DGC_STREAM_TYPE_BINARY;
        
        shared_ptr<GLTFConfig> config = asset->converterConfig();
        
        DynamicVector dynamicVector;
        dynamicVector.SetVectors(buffer);
        dynamicVector.SetDimVector(componentsCount);
        dynamicVector.SetMax(max);
        dynamicVector.SetMin(min);
        dynamicVector.SetNVector(count);
        dynamicVector.SetStride(componentsCount);
        dynamicVector.ComputeMinMax(O3DGC_SC3DMC_MAX_SEP_DIM);//O3DGC_SC3DMC_MAX_ALL_DIMS        
        DVEncodeParams params;
        
        int quantization = 17;
        if (path == "TIME") {
            quantization = config->unsignedInt32ForKeyPath("extensions.Open3DGC.quantization.TIME");
        } else if (path == "translation") {
            quantization = config->unsignedInt32ForKeyPath("extensions.Open3DGC.quantization.transform.translation");
        } else if (path == "rotation") {
            quantization = config->unsignedInt32ForKeyPath("extensions.Open3DGC.quantization.transform.rotation");
        } else if (path == "scale") {
            quantization = config->unsignedInt32ForKeyPath("extensions.Open3DGC.quantization.transform.scale");
        }
        
        params.SetQuantBits(quantization);
        params.SetStreamType(streamType);
        
        DynamicVectorEncoder encoder;
        encoder.SetStreamType(streamType);
        Timer timer;
        timer.Tic();
        BinaryStream bstream(componentsCount * count * 16);
        encoder.Encode(params, dynamicVector, bstream);
        timer.Toc();
        outputStream->write((const char*)bstream.GetBuffer(), bstream.GetSize());
        
        /*
        if (componentsCount == 4) {
            DynamicVector  dynamicVector1;
            DynamicVectorDecoder decoder;
            decoder.DecodeHeader(dynamicVector1, bstream);
            dynamicVector1.SetStride(dynamicVector1.GetDimVector());
            
            std::vector<Real> oDV;
            std::vector<Real> oDVMin;
            std::vector<Real> oDVMax;
            oDV.resize(dynamicVector1.GetNVector() * dynamicVector1.GetDimVector());
            oDVMin.resize(dynamicVector1.GetDimVector());
            oDVMax.resize(dynamicVector1.GetDimVector());
            dynamicVector1.SetVectors(& oDV[0]);
            dynamicVector1.SetMin(& oDVMin[0]);
            dynamicVector1.SetMax(& oDVMax[0]);
            decoder.DecodePlayload(dynamicVector1, bstream);
            
            float* dbuffer = (float*)dynamicVector1.GetVectors();
            
            printf("****dump axis-angle. Axis[3] / Angle[1]\n");
            for (int i = 0 ; i < count ; i++) {
                int offset = i * 4;
                printf("[raw]%f %f %f %f [10bits]%f %f %f %f\n",
                       buffer[offset+0],buffer[offset+1],buffer[offset+2],buffer[offset+3],
                       dbuffer[offset+0],dbuffer[offset+1],dbuffer[offset+2],dbuffer[offset+3]);
            }
        }
        */
    }
    
    /*
     Handles Parameter creation / addition
     */
    static std::string __SetupSamplerForParameter(GLTFAnimation *cvtAnimation,
                                                  shared_ptr<JSONObject> parameter,
                                                  const std::string &name) {
        shared_ptr<JSONObject> sampler(new JSONObject());
        std::string samplerID = cvtAnimation->getSamplerIDForName(name);
        sampler->setString("input", "TIME");           //FIXME:harcoded for now
        sampler->setString("interpolation", "LINEAR"); //FIXME:harcoded for now
        sampler->setString("output", name);
        cvtAnimation->samplers()->setValue(samplerID, sampler);
        
        return samplerID;
    }
    
    static shared_ptr <JSONObject> __WriteAnimationParameter(GLTFAnimation *cvtAnimation,
                                                             const std::string& parameterSID,
                                                             const std::string& accessorUID,
                                                             const std::string& parameterType,
                                                             unsigned char* buffer, int byteLength,
                                                             bool isInputParameter,
                                                             GLTFAsset *asset) {
        //setup
        shared_ptr <GLTFProfile> profile = asset->profile();
        shared_ptr <GLTF::JSONObject> accessors = asset->root()->createObjectIfNeeded(kAccessors);
        unsigned int glType = profile->getGLenumForString(parameterType);
        shared_ptr<GLTFAccessor> parameter(new GLTFAccessor(profile, "FLOAT", profile->getTypeForGLType(glType)));
        parameter->setByteStride(profile->sizeOfGLType(glType));
        parameter->setCount((int)cvtAnimation->getCount());

        accessors->setValue(accessorUID, parameter);
        cvtAnimation->parameters()->setString(parameterSID, accessorUID);
        
        //write
        int byteOffset = 0;
        bool shouldEncodeOpen3DGC = CONFIG_STRING(asset, "compressionType")  == "Open3DGC";
        GLTFOutputStream *outputStream = shouldEncodeOpen3DGC ? asset->createOutputStreamIfNeeded(kCompressionOutputStream).get() : asset->createOutputStreamIfNeeded(asset->getSharedBufferId()).get();;
        byteOffset = (int)outputStream->length();
        parameter->setUnsignedInt32("byteOffset", byteOffset);
        
        if (shouldEncodeOpen3DGC) {
            long componentsCount = (long)profile->getComponentsCountForType(parameter->getString(kType));
            if (componentsCount) {
                encodeDynamicVector((float*)buffer, parameterSID, componentsCount, (long)cvtAnimation->getCount(), asset);
                
                byteLength = (int)outputStream->length() - byteOffset;
                
                shared_ptr<JSONObject> extensionsObject = parameter->createObjectIfNeeded(kExtensions);
                shared_ptr<JSONObject> compressionObject = extensionsObject->createObjectIfNeeded("Open3DGC-compression");
                shared_ptr<JSONObject> compressionDataObject = compressionObject->createObjectIfNeeded("compressedData");
                
                compressionDataObject->setUnsignedInt32("byteOffset", byteOffset);
                compressionDataObject->setUnsignedInt32("count", byteLength);
                compressionDataObject->setString("mode", CONFIG_STRING(asset, "compressionMode"));
                compressionDataObject->setString(kType, "SCALAR");
                compressionDataObject->setUnsignedInt32(kComponentType, profile->getGLenumForString("UNSIGNED_BYTE"));

            }
        } else {
            outputStream->write((const char*)buffer, byteLength);
        }
        asset->setAnimationByteLength(asset->getAnimationByteLength() + byteLength);

        return parameter;
    }
    
    /*
     FIXME: this is more general than Open3DGC and should be move out of there.
     We should add compression on top of accessor in a more generic way
     
     Handles Parameter creation / addition / write
     */
    void setupAndWriteAnimationParameter(GLTFAnimation *cvtAnimation,
                                         const std::string& parameterSID,
                                         const std::string& parameterType,
                                         unsigned char* buffer, int byteLength,
                                         bool isInputParameter,
                                         GLTFAsset *asset) {
        
        shared_ptr <JSONObject> parameter;
        shared_ptr <GLTF::JSONObject> accessors = asset->root()->createObjectIfNeeded(kAccessors);
        std::string accessorUID = "";
        if (CONFIG_BOOL(asset, "shareAnimationAccessors")) {
            GLTFAccessorCache accessorCache(buffer, byteLength);
            UniqueIDToAccessor::iterator it = asset->_uniqueIDToAccessorObject.find(accessorCache);
            if (it != asset->_uniqueIDToAccessorObject.end()) {
                cvtAnimation->parameters()->setString(parameterSID, it->second);
                parameter = accessors->getObject(it->second);
            } else {
                //build an id based on number of accessors
                accessorUID = "animAccessor_" + GLTFUtils::toString(accessors->getKeysCount());
                parameter = __WriteAnimationParameter(cvtAnimation, parameterSID, accessorUID, parameterType, buffer, byteLength, isInputParameter, asset);
                asset->_uniqueIDToAccessorObject.insert(std::make_pair(accessorCache, accessorUID));
            }
        } else {
            accessorUID = "animAccessor_" + GLTFUtils::toString(accessors->getKeysCount());
            parameter = __WriteAnimationParameter(cvtAnimation, parameterSID, accessorUID, parameterType, buffer, byteLength, isInputParameter, asset);
        }       

        if (!isInputParameter) {
            __SetupSamplerForParameter(cvtAnimation, parameter, parameterSID);
        }

        if (accessorUID != "") {
            shared_ptr <GLTFAccessor> accessor = static_pointer_cast<GLTFAccessor>(accessors->getObject(accessorUID));
            accessor->exposeMinMax(buffer);
        }
    }
}


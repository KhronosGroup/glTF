// Copyright (c) 2013, Fabrice Robinet; 2015, Analytical Graphics, Inc.
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
using namespace rapidjson;
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
            if (primitive->getPrimitive() != profile->getGLenumForString("TRIANGLES")) {
                return false;
            }
        }
        
        for (auto semantic : mesh->allSemantics()) {
            if (mesh->getMeshAttributesCountForSemantic(semantic) > 1) {
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
    
    inline bool semanticIsCompressible(Semantic s) {
        return true;
    }
    
    void encodeOpen3DGCMesh(shared_ptr<GLTFMesh> mesh, GLTFAsset* asset)
    {
        shared_ptr<GLTFConfig> config = asset->converterConfig();
        string decompressedViewName = kDecompressedView + "_" + mesh->getName();
        
        // Retrieve quantization level parameters.
        int quantPosition = config->unsignedInt32ForKeyPath("extensions.Open3DGC.quantization.POSITION");
        int quantNormal   = config->unsignedInt32ForKeyPath("extensions.Open3DGC.quantization.NORMAL");
        int quantTexCoord = config->unsignedInt32ForKeyPath("extensions.Open3DGC.quantization.TEXCOORD");
        int quantColor    = config->unsignedInt32ForKeyPath("extensions.Open3DGC.quantization.COLOR");
        int quantWeight   = config->unsignedInt32ForKeyPath("extensions.Open3DGC.quantization.WEIGHT");
        int quantJoint    = config->unsignedInt32ForKeyPath("extensions.Open3DGC.quantization.JOINT");
        
        // Retrieve prediction mode parameters.
        O3DGCSC3DMCPredictionMode predPosition = _predictionModeForString(config->stringForKeyPath("extensions.Open3DGC.prediction.POSITION"));
        O3DGCSC3DMCPredictionMode predNormal   = _predictionModeForString(config->stringForKeyPath("extensions.Open3DGC.prediction.NORMAL"));
        O3DGCSC3DMCPredictionMode predTexCoord = _predictionModeForString(config->stringForKeyPath("extensions.Open3DGC.prediction.TEXCOORD"));
        O3DGCSC3DMCPredictionMode predColor    = _predictionModeForString(config->stringForKeyPath("extensions.Open3DGC.prediction.COLOR"));
        O3DGCSC3DMCPredictionMode predWeight   = _predictionModeForString(config->stringForKeyPath("extensions.Open3DGC.prediction.WEIGHT"));
        O3DGCSC3DMCPredictionMode predJoint    = _predictionModeForString(config->stringForKeyPath("extensions.Open3DGC.prediction.JOINT"));
        
        // Get a list of the glTF primitives
        GLTF::JSONValueVector primitives = mesh->getPrimitives()->values();
        unsigned int primitivesCount = (unsigned int) primitives.size();
        
        // The following loop runs through primitives to gather the total number of indices...
        unsigned int allIndicesCount = 0;
        // ... and compute the number of triangles in each primitive.
        std::vector <unsigned int> trianglesPerPrimitive;

        for (unsigned int i = 0; i < primitivesCount; i++) {
            shared_ptr<GLTF::GLTFPrimitive> primitive = static_pointer_cast<GLTFPrimitive>(primitives[i]);
            shared_ptr<GLTF::GLTFAccessor> uniqueIndices = primitive->getIndices();
            unsigned int indicesCount = (unsigned int) uniqueIndices->getCount();
            // FIXME: assumes triangles. (For now, we are guarded from issues by
            // canEncodeOpen3DGCMesh, which will prevent compression of anything
            // that isn't triangles.)
            allIndicesCount += indicesCount;
            trianglesPerPrimitive.push_back(indicesCount / 3);
        }
        
        // The following loop fills the primitiveIDs array (a mapping from triangle index to primitive index), ...
        unsigned long *primitiveIDs = new unsigned long[allIndicesCount / 3];
        unsigned int primitiveIDsIdx = 0;
        // ... fills the allConcatenatedIndices array (an array of all vertex indices), ...
        unsigned short *allConcatenatedIndices = new unsigned short[allIndicesCount];
        unsigned int allConcatenatedIndicesIdx = 0;
        // ... and counts the number of triangles.
        unsigned int allTrianglesCount = 0;

        for (unsigned int i = 0; i < primitivesCount; i++) {
            unsigned int trianglesCount = trianglesPerPrimitive[i];
            for (unsigned int j = 0; j < trianglesCount; j++) {
                primitiveIDs[primitiveIDsIdx] = i;
                primitiveIDsIdx++;
            }
            allTrianglesCount += trianglesCount;
            shared_ptr<GLTF::GLTFPrimitive> primitive = static_pointer_cast<GLTFPrimitive>(primitives[i]);
            shared_ptr<GLTF::GLTFAccessor> uniqueIndices = primitive->getIndices();
            unsigned int indicesCount = (unsigned int) uniqueIndices->getCount();
            auto indicesBufferView = uniqueIndices->getBufferView();
            unsigned int *indicesPtr = static_cast<unsigned int *>(indicesBufferView->getBufferDataByApplyingOffset());
            for (unsigned int j = 0; j < indicesCount; j++) {
                allConcatenatedIndices[allConcatenatedIndicesIdx] = indicesPtr[j];
                allConcatenatedIndicesIdx++;
            }
        }
        
        // Create the IFS object for compression.
        o3dgc::IndexedFaceSet<unsigned short> ifs;
        // Tell it how many triangles there are.
        // FIXME: Open3DGC SetNCoordIndex is not a good name here (file against o3dgc)
        ifs.SetNCoordIndex(allTrianglesCount);
        // Tell it where the indices for those triangles are.
        ifs.SetCoordIndex(allConcatenatedIndices);
        ifs.SetIndexBufferID(primitiveIDs);

        // The following loop, for each semantic (type of attribute),
        // extracts the encoding parameters and points the IFS at the attribute data.
        std::vector<GLTF::Semantic> semantics = mesh->allSemantics();
        o3dgc::SC3DMCEncodeParams params;
        // The vertex count for all attributes will be the same - the number of vertices total.
        unsigned long vertexCount = 0;
        // This is used to define an index for each float attribute (any float attribute other than POSITION or NORMAL).
        unsigned int nFloatAttributes = 0;
        // This is used to define an index for each float attribute (any int attribute).
        unsigned int nIntAttributes = 0;

        for (Semantic semantic : semantics) {
            size_t attributesCount = mesh->getMeshAttributesCountForSemantic(semantic);
            if (attributesCount > 1) {
                // This should never happen. It is detected in canEncodeOpen3DGCMesh.
                printf("fatal error: cannot compress meshes which have multiple primitives with different attributes\n");
                exit(EXIT_FAILURE);
            }
            
            for (size_t j = 0; j < attributesCount; j++) {
                shared_ptr<GLTFAccessor> meshAttribute = mesh->getMeshAttribute(semantic, j);
                vertexCount = (unsigned long) meshAttribute->getCount();
                unsigned long componentsPerElement = static_cast<unsigned long>(meshAttribute->componentsPerElement());

                if (!semanticIsCompressible(semantic)) {
                    continue;
                }

                // Use the temporary bufferView to get data to send to the compressor
                meshAttribute->exposeMinMax();
                void *buffer = meshAttribute->getBufferView()->getBufferDataByApplyingOffset();
                switch (semantic) {
                    case POSITION:
                        params.SetCoordQuantBits(quantPosition);
                        params.SetCoordPredMode(predPosition);
                        ifs.SetNCoord(vertexCount);
                        ifs.SetCoord(static_cast<Real *>(buffer));
                        break;
                    case NORMAL:
                        params.SetNormalQuantBits(quantNormal);
                        params.SetNormalPredMode(predNormal);
                        ifs.SetNNormal(vertexCount);
                        ifs.SetNormal(static_cast<Real *>(buffer));
                        break;
                    case TEXCOORD:
                        params.SetFloatAttributeQuantBits(nFloatAttributes, quantTexCoord);
                        params.SetFloatAttributePredMode(nFloatAttributes, predTexCoord);
                        ifs.SetNFloatAttribute(nFloatAttributes, vertexCount);
                        ifs.SetFloatAttributeDim(nFloatAttributes, componentsPerElement);
                        ifs.SetFloatAttributeType(nFloatAttributes, O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_TEXCOORD);
                        ifs.SetFloatAttribute(nFloatAttributes, static_cast<Real *>(buffer));
                        nFloatAttributes++;
                        break;
                    case COLOR:
                        params.SetFloatAttributeQuantBits(nFloatAttributes, quantColor);
                        params.SetFloatAttributePredMode(nFloatAttributes, predColor);
                        ifs.SetNFloatAttribute(nFloatAttributes, vertexCount);
                        ifs.SetFloatAttributeDim(nFloatAttributes, componentsPerElement);
                        ifs.SetFloatAttributeType(nFloatAttributes, O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_COLOR);
                        ifs.SetFloatAttribute(nFloatAttributes, static_cast<Real *>(buffer));
                        nFloatAttributes++;
                        break;
                    case WEIGHT:
                        params.SetFloatAttributeQuantBits(nFloatAttributes, quantWeight);
                        params.SetFloatAttributePredMode(nFloatAttributes, predWeight);
                        ifs.SetNFloatAttribute(nFloatAttributes, vertexCount);
                        ifs.SetFloatAttributeDim(nFloatAttributes, componentsPerElement);
                        ifs.SetFloatAttributeType(nFloatAttributes, O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_WEIGHT);
                        ifs.SetFloatAttribute(nFloatAttributes, static_cast<Real *>(buffer));
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
                        params.SetFloatAttributeQuantBits(nFloatAttributes, quantJoint);
                        params.SetFloatAttributePredMode(nFloatAttributes, predJoint);
                        ifs.SetNFloatAttribute(nFloatAttributes, vertexCount);
                        ifs.SetFloatAttributeDim(nFloatAttributes, componentsPerElement);
                        ifs.SetFloatAttributeType(nFloatAttributes, O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_UNKOWN);
                        ifs.SetFloatAttribute(nFloatAttributes, static_cast<Real *>(buffer));
                        nFloatAttributes++;
                        break;
                    default:
                        break;
                }
            }
        }
        
        // Inform the encoder and IFS of a few things.
        ifs.SetNumFloatAttributes(nFloatAttributes);
        ifs.SetNumIntAttributes(nIntAttributes);
        ifs.ComputeMinMax(O3DGC_SC3DMC_MAX_ALL_DIMS);
        params.SetNumFloatAttributes(nFloatAttributes);
        //Open3DGC binary is disabled  // TODO: What does this mean? It looks like it works, at least. But o3dgc.js can't load it. -kainino0x
        params.SetStreamType(CONFIG_STRING(asset, "compressionMode") == "binary" ? O3DGC_STREAM_TYPE_BINARY : O3DGC_STREAM_TYPE_ASCII);

#if DUMP_O3DGC_OUTPUT
        static int dumpedId = 0;
        COLLADABU::URI outputURI(asset->outputFilePath.c_str());
        std::string outputFilePath = outputURI.getPathDir() + GLTFUtils::toString(dumpedId) + ".txt";
        dumpedId++;
        SaveIFS(outputFilePath, ifs);
#endif
        
        // Get (or create) stream to output the compressed blob.
        GLTFOutputStream *const outputStream = asset->createOutputStreamIfNeeded(asset->getSharedBufferId()).get();
        unsigned int bufferOffset = static_cast<unsigned int>(outputStream->length());
        
        // Encode the parameters and IFS into a bytestream.
        BinaryStream bstream(vertexCount * 8);  // allocate some arbitrary amount of space
        SC3DMCEncoder <unsigned short> encoder;
        encoder.Encode(params, ifs, bstream);

        unsigned int decompressedByteLength = 0;

        // Create final bufferView for indices
        auto indexBufferView = make_shared<GLTFBufferView>();
        indexBufferView->removeValue(kBuffer);
        indexBufferView->setByteOffset(0);
        indexBufferView->setByteLength(allIndicesCount * sizeof(unsigned short));
        indexBufferView->setInt32(kTarget, asset->profile()->getGLenumForString("ELEMENT_ARRAY_BUFFER"));
        {
            auto extensions = indexBufferView->createObjectIfNeeded(kExtensions);
            auto extension = extensions->createObjectIfNeeded(kExtensionOpen3DGC);
            extension->setString(kDecompressedView, decompressedViewName);
        }
        decompressedByteLength += allIndicesCount * sizeof(unsigned short);

        // Pad to the next 4-byte boundary so that floats are aligned
        decompressedByteLength = ((decompressedByteLength + 3) / 4) * 4;

        // Create final bufferview for vertex data
        auto vertexBufferView = make_shared<GLTFBufferView>();
        vertexBufferView->removeValue(kBuffer);
        vertexBufferView->setByteOffset(decompressedByteLength);
        vertexBufferView->setInt32(kTarget, asset->profile()->getGLenumForString("ARRAY_BUFFER"));
        // byteLength set in loop below
        {
            auto extensions = vertexBufferView->createObjectIfNeeded(kExtensions);
            auto extension = extensions->createObjectIfNeeded(kExtensionOpen3DGC);
            extension->setString(kDecompressedView, decompressedViewName);
        }

        // Once the temporary bufferViews have been handled, replace them with the real ones.
        // (But don't do this until after compression has accessed the buffer data.)
        // For indices:
        size_t indicesOffset = 0;
        for (unsigned int i = 0; i < primitivesCount; i++) {
            shared_ptr<GLTF::GLTFPrimitive> primitive = static_pointer_cast<GLTFPrimitive>(primitives[i]);
            shared_ptr<GLTF::GLTFAccessor> uniqueIndices = primitive->getIndices();
            uniqueIndices->setBufferView(indexBufferView);
            uniqueIndices->setByteOffset(indicesOffset);
            indicesOffset += uniqueIndices->getCount() * uniqueIndices->elementByteLength();
        }

        // Since accessors point into _decompressed_ data, we have to simulate the layout.
        unsigned int accessorOffset = 0;

        // For vertex data:
        for (Semantic semantic : semantics) {
            size_t attributesCount = mesh->getMeshAttributesCountForSemantic(semantic);
            
            if (semanticIsCompressible(semantic)) {
                for (size_t j = 0; j < attributesCount; j++) {
                    shared_ptr<GLTFAccessor> meshAttribute = mesh->getMeshAttribute(semantic, j);
                    unsigned int vCount = (unsigned int) meshAttribute->getCount();
                    size_t componentsPerElement = meshAttribute->componentsPerElement();

                    meshAttribute->setBufferView(vertexBufferView);
                    meshAttribute->setByteOffset(accessorOffset);
                    size_t attributeSize = vCount * meshAttribute->elementByteLength();
                    accessorOffset += (unsigned int) attributeSize;
                    vertexBufferView->setByteLength(vertexBufferView->getByteLength() + attributeSize);
                }
            }
        }

        decompressedByteLength += accessorOffset;

        shared_ptr<JSONObject> decompressedView(new JSONObject());
        decompressedView->setString(kBuffer, asset->getSharedBufferId());
        decompressedView->setInt32(kByteOffset, bufferOffset);
        decompressedView->setInt32(kByteLength, bstream.GetSize());
        decompressedView->setInt32(kDecompressedByteLength, decompressedByteLength);
        decompressedView->setString(kCompressionMode, CONFIG_STRING(asset, "compressionMode"));

        {
            // Get (or create) the extensions.mesh_compression_open3dgc object.
            shared_ptr<JSONObject> extension = asset->root()->createObjectIfNeeded(kExtensions)->createObjectIfNeeded(kExtensionOpen3DGC);
            shared_ptr<JSONObject> decompressedViews = extension->createObjectIfNeeded(kDecompressedViews);
            // And create a decompressedView in it.
            decompressedViews->setValue(decompressedViewName, decompressedView);
        }
        //testDecode(mesh, bstream);

        // Write the bytestream out as the final compressed data.
        outputStream->write((const char*)bstream.GetBuffer(0), bstream.GetSize());
        asset->setGeometryByteLength(asset->getGeometryByteLength() + bstream.GetSize());
        bufferOffset = (unsigned int) outputStream->length();

        size_t rem = bufferOffset % 4;
        if (rem) {
            char pad[3] = { 0 };
            size_t paddingForAlignment = 4 - rem;
            outputStream->write(pad, paddingForAlignment);
            bufferOffset += (unsigned int) paddingForAlignment;
        }

        // Write any vertex data that wasn't compressed
        auto uncompressedVertexBufferView = make_shared<GLTFBufferView>();
        uncompressedVertexBufferView->setString(kBuffer, asset->getSharedBufferId());
        uncompressedVertexBufferView->setByteOffset(bufferOffset);
        uncompressedVertexBufferView->setByteLength(0);
        uncompressedVertexBufferView->setInt32(kTarget, asset->profile()->getGLenumForString("ARRAY_BUFFER"));

        accessorOffset = 0;
        for (Semantic semantic : semantics) {
            size_t attributesCount = mesh->getMeshAttributesCountForSemantic(semantic);
            
            if (!semanticIsCompressible(semantic)) {
                for (size_t j = 0; j < attributesCount; j++) {
                    shared_ptr<GLTFAccessor> meshAttribute = mesh->getMeshAttribute(semantic, j);
                    unsigned int vCount = (unsigned int) meshAttribute->getCount();
                    size_t componentsPerElement = meshAttribute->componentsPerElement();

                    void *buffer = meshAttribute->getBufferView()->getBufferDataByApplyingOffset();
                    size_t byteLength = vCount * meshAttribute->elementByteLength();
                    outputStream->write(static_cast<const char *>(buffer), byteLength);
                    uncompressedVertexBufferView->setByteLength(uncompressedVertexBufferView->getByteLength() + byteLength);

                    meshAttribute->setBufferView(uncompressedVertexBufferView);
                    meshAttribute->setByteOffset(accessorOffset);
                    accessorOffset += (unsigned int) byteLength;
                }
            }
        }

        asset->convertionResults()->setUnsignedInt32("trianglesCount", allTrianglesCount);
        asset->convertionResults()->setUnsignedInt32("verticesCount", allIndicesCount);

        if (allConcatenatedIndices) {
            delete allConcatenatedIndices;
        }
        
        if (primitiveIDs) {
            delete primitiveIDs;
        }
    }
    
    void encodeDynamicVector(float *buffer, const std::string &path, size_t componentsCount, size_t count, GLTFAsset* asset) {
        GLTFOutputStream *const outputStream = asset->createOutputStreamIfNeeded(asset->getSharedBufferId()).get();
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
                                                             unsigned char* buffer, size_t byteLength,
                                                             bool isInputParameter,
                                                             GLTFAsset *asset) {
        //setup
        shared_ptr <GLTFProfile> profile = asset->profile();
        shared_ptr <GLTF::JSONObject> accessors = asset->root()->createObjectIfNeeded(kAccessors);
        shared_ptr<JSONObject> parameter(new JSONObject());
        unsigned int glType = profile->getGLenumForString(parameterType);
        parameter->setUnsignedInt32("count", cvtAnimation->getCount());
        parameter->setString(kType, profile->getTypeForGLType(glType));
        parameter->setUnsignedInt32(kComponentType, profile->getGLComponentTypeForGLType(glType));

        accessors->setValue(accessorUID, parameter);
        cvtAnimation->parameters()->setString(parameterSID, accessorUID);
        
        //write
        size_t byteOffset = 0;
        // TODO: animation compression?
        bool shouldEncodeOpen3DGC = false; //CONFIG_STRING(asset, "compressionType") == "Open3DGC";
        GLTFOutputStream *const outputStream = asset->createOutputStreamIfNeeded(asset->getSharedBufferId()).get();
        byteOffset = outputStream->length();
        parameter->setUnsignedInt32("byteOffset", byteOffset);
        
        if (shouldEncodeOpen3DGC) {
            size_t componentsCount = profile->getComponentsCountForType(parameter->getString(kType));
            if (componentsCount) {
                encodeDynamicVector((float*)buffer, parameterSID, componentsCount, cvtAnimation->getCount(), asset);
                
                byteLength = outputStream->length() - byteOffset;
                
                shared_ptr<JSONObject> extensionsObject = parameter->createObjectIfNeeded(kExtensions);
                shared_ptr<JSONObject> compressionObject = extensionsObject->createObjectIfNeeded(kExtensionOpen3DGC);
                shared_ptr<JSONObject> compressionDataObject = compressionObject->createObjectIfNeeded(kDecompressedView);
                
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
                                         unsigned char* buffer, size_t byteLength,
                                         bool isInputParameter,
                                         GLTFAsset *asset) {
        
        shared_ptr <JSONObject> parameter;
        shared_ptr <GLTF::JSONObject> accessors = asset->root()->createObjectIfNeeded(kAccessors);
        if (CONFIG_BOOL(asset, "shareAnimationAccessors")) {
            GLTFAccessorCache accessorCache(buffer, byteLength);
            UniqueIDToAccessor::iterator it = asset->_uniqueIDToAccessorObject.find(accessorCache);
            if (it != asset->_uniqueIDToAccessorObject.end()) {
                cvtAnimation->parameters()->setString(parameterSID, it->second);
                parameter = accessors->getObject(it->second);
            } else {
                //build an id based on number of accessors
                std::string accessorUID = "animAccessor_" + GLTFUtils::toString(accessors->getKeysCount());
                parameter = __WriteAnimationParameter(cvtAnimation, parameterSID, accessorUID, parameterType, buffer, byteLength, isInputParameter, asset);
                asset->_uniqueIDToAccessorObject.insert(std::make_pair(accessorCache, accessorUID));
            }
        } else {
            std::string accessorUID = "animAccessor_" + GLTFUtils::toString(accessors->getKeysCount());
            parameter = __WriteAnimationParameter(cvtAnimation, parameterSID, accessorUID, parameterType, buffer, byteLength, isInputParameter, asset);
        }        
        if (!isInputParameter)
            __SetupSamplerForParameter(cvtAnimation, parameter, parameterSID);
        
    }
}


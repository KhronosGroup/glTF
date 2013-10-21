/*
Copyright (c) 2013 Khaled Mammou - Advanced Micro Devices, Inc.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#define _CRT_SECURE_NO_WARNINGS
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <vector>
#include "o3dgcCommon.h"
#include "o3dgcVector.h"
#include "o3dgcSC3DMCEncodeParams.h"
#include "o3dgcIndexedFaceSet.h"
#include "o3dgcSC3DMCEncoder.h"
#include "o3dgcSC3DMCDecoder.h"
#include "o3dgcTimer.h"
#include "o3dgcDVEncodeParams.h"
#include "o3dgcDynamicVectorEncoder.h"
#include "o3dgcDynamicVectorDecoder.h"

//#define ADD_FAKE_ANIMATION_WEIGHTS
//#define TEST_DYNAMIC_VECTOR_ENCODING


#ifdef WIN32
#define PATH_SEP "\\"
#else
#define PATH_SEP "/"
#endif

using namespace o3dgc;

class IVec3Cmp 
{
   public:
      bool operator()(const Vec3<unsigned long> a,const Vec3<unsigned long> b) const
      { 
          if (a.X() != b.X())
          {
              return (a.X() > b.X());
          }
          else if (a.Y() != b.Y())
          {
              return (a.Y() > b.Y());
          }
          return (a.Z() > b.Z());
      }
};

class Material
{
public:
    Material(unsigned long id, unsigned long numTriangles, const std::string & name)
    {
        m_id           = id;
        m_numTriangles = numTriangles;
        m_name         = name;
    };
    Material()
    {
        m_id           = 0;
        m_numTriangles = 0;
    };
    unsigned long	m_id;
    unsigned long   m_numTriangles;
    std::string     m_name;
};

bool LoadOBJ(const std::string & fileName, 
             std::vector< Vec3<Real> > & upoints,
             std::vector< Vec2<Real> > & utexCoords,
             std::vector< Vec3<Real> > & unormals,
             std::vector< Vec3<unsigned long> > & triangles,
             std::vector< unsigned long > & indexBufferIDs,
             std::vector< Material > & materials,
             std::string & materialLib);

bool LoadIFS(const std::string & fileName, 
             std::vector< Vec3<Real> > & points,
             std::vector< Vec2<Real> > & texCoords,
             std::vector< Vec3<Real> > & normals,
             std::vector< Vec3<unsigned long> > & triangles,
             std::vector< unsigned long > & indexBufferIDs);

bool SaveOBJ(const std::string & fileName, 
             const std::vector< Vec3<Real> > & points,
             const std::vector< Vec2<Real> > & texCoords,
             const std::vector< Vec3<Real> > & normals,
             const std::vector< Vec3<unsigned long> > & triangles,
             const std::vector< Material > & materials,
             const std::vector< unsigned long > indexBufferIDs,
             const std::string & materialLib);

bool LoadMaterials(const std::string & fileName, 
                   std::vector< Material > & materials, 
                   std::string & materialLib);

bool SaveMaterials(const std::string & fileName, 
                   const std::vector< Material > & materials, 
                   const std::string & materialLib);
bool SaveIFS(const std::string & fileName, 
             const IndexedFaceSet<unsigned long> & ifs);
bool Check(const IndexedFaceSet<unsigned long> & ifs);

int testEncode(const std::string & fileName, int qcoord, int qtexCoord, int qnormal, O3DGCStreamType streamType)
{
    std::string folder;
    long found = (long) fileName.find_last_of(PATH_SEP);
    if (found != -1)
    {
        folder = fileName.substr(0,found);
    }
    if (folder == "")
    {
        folder = ".";
    }
    std::string file(fileName.substr(found+1));
    std::string outFileName = folder + PATH_SEP + file.substr(0, file.find_last_of(".")) + ".s3d";
    std::vector< Vec3<Real> > points;
    std::vector< Vec3<Real> > normals;
    std::vector< Vec2<Real> > texCoords;
    std::vector< Vec3<unsigned long> > triangles;
    std::vector< unsigned long > indexBufferIDs;
    std::vector< Material > materials;

    std::string materialLib;
    std::cout << "Loading " << fileName << " ..." << std::endl;
    bool ret;
    if (fileName.find(".obj") != std::string::npos )
    {
        ret = LoadOBJ(fileName, points, texCoords, normals, triangles, indexBufferIDs, materials, materialLib);
        if (!ret)
        {
            std::cout << "Error: LoadOBJ()\n" << std::endl;
            return -1;
        }
    }
    else
    {
        ret = LoadIFS(fileName, points, texCoords, normals, triangles, indexBufferIDs);
        if (!ret)
        {
            std::cout << "Error: LoadIFS()\n" << std::endl;
            return -1;
        }
    }
    if (points.size() == 0 || triangles.size() == 0)
    {
        std::cout <<  "Error: points.size() == 0 || triangles.size() == 0 \n" << std::endl;
        return -1;
    }
    std::cout << "Done." << std::endl;

    if (materials.size() > 0)
    {
        std::string matFileName = folder + PATH_SEP + file.substr(0, file.find_last_of(".")) + ".mat";
        ret = SaveMaterials(matFileName.c_str(), materials, materialLib);
    }
    if (!ret)
    {
        std::cout << "Error: SaveMatrials()\n" << std::endl;
        return -1;
    }

#ifdef ADD_FAKE_ANIMATION_WEIGHTS
    std::vector< Real > weights;
    std::vector< long > jointIDs;
    const unsigned int nV = points.size();
    const unsigned int numJointsPerVertex = 4;
    const unsigned int qWeights = 8;
    {
        const unsigned int size = nV * numJointsPerVertex;
        weights.resize(size);
        jointIDs.resize(size);
        for(unsigned int i = 0; i < size; ++i)
        {
            weights[i]  = (Real) (rand() % 1024) / 1024;
            jointIDs[i] = rand() % numJointsPerVertex;
        }
    }
#endif


/*
    ret = SaveOBJ("debug.obj", points, texCoords, normals, triangles, materials, indexBufferIDs, materialLib);
    if (!ret)
    {
        std::cout << "Error: SaveOBJ()\n" << std::endl;
        return -1;
    }
*/



    
    SC3DMCEncodeParams params;
    params.SetStreamType(streamType);
    IndexedFaceSet<unsigned long> ifs;

    ifs.SetNCoordIndex((unsigned long)triangles.size());
    ifs.SetCoordIndex((unsigned long * const ) &(triangles[0]));
    if (materials.size() > 1)
    {
        ifs.SetIndexBufferID((unsigned long * const ) &(indexBufferIDs[0]));
    }

    params.SetCoordQuantBits(qcoord);
    params.SetCoordPredMode(O3DGC_SC3DMC_PARALLELOGRAM_PREDICTION);
    ifs.SetNCoord((unsigned long) points.size());
    ifs.SetCoord((Real * const) & (points[0]));

    params.SetNormalQuantBits(qnormal);
    ifs.SetNNormal((unsigned long)normals.size());
    if (normals.size() > 0)
    {
        ifs.SetNormal((Real * const) & (normals[0]));
        params.SetNormalPredMode(O3DGC_SC3DMC_SURF_NORMALS_PREDICTION);
    }

    unsigned int nIntAttributes   = 0;
    unsigned int nFloatAttributes = 0;
    if (texCoords.size() > 0)
    {
        params.SetFloatAttributeQuantBits(nFloatAttributes, qtexCoord);
        params.SetFloatAttributePredMode(nFloatAttributes, O3DGC_SC3DMC_PARALLELOGRAM_PREDICTION);
        ifs.SetNFloatAttribute(nFloatAttributes, texCoords.size());
        ifs.SetFloatAttributeDim(nFloatAttributes, 2);
        ifs.SetFloatAttributeType(nFloatAttributes, O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_TEXCOORD);
        ifs.SetFloatAttribute(nFloatAttributes, (Real * const ) & (texCoords[0]));
        nFloatAttributes++;
    }

#ifdef ADD_FAKE_ANIMATION_WEIGHTS
    if (weights.size() > 0)
    {
        params.SetFloatAttributeQuantBits(nFloatAttributes, qWeights);
        params.SetFloatAttributePredMode(nFloatAttributes, O3DGC_SC3DMC_DIFFERENTIAL_PREDICTION);
        ifs.SetNFloatAttribute(nFloatAttributes, weights.size() / numJointsPerVertex);
        ifs.SetFloatAttributeDim(nFloatAttributes, numJointsPerVertex);
        ifs.SetFloatAttributeType(nFloatAttributes, O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_WEIGHT);
        ifs.SetFloatAttribute(nFloatAttributes, (Real * const ) & (weights[0]));
        nFloatAttributes++;
    }

    if (jointIDs.size() > 0)
    {
        params.SetIntAttributePredMode(nIntAttributes, O3DGC_SC3DMC_DIFFERENTIAL_PREDICTION);
        ifs.SetNIntAttribute(nIntAttributes, jointIDs.size() / numJointsPerVertex);
        ifs.SetIntAttributeDim(nIntAttributes, numJointsPerVertex);
        ifs.SetIntAttributeType(nIntAttributes, O3DGC_IFS_INT_ATTRIBUTE_TYPE_JOINT_ID);
        ifs.SetIntAttribute(nIntAttributes, (long * const ) & (jointIDs[0]));
        nIntAttributes++;
    }
#endif

    params.SetNumFloatAttributes(nFloatAttributes);
    ifs.SetNumFloatAttributes(nFloatAttributes);

    params.SetNumIntAttributes(nIntAttributes);
    ifs.SetNumIntAttributes(nIntAttributes);
    


    std::cout << "Mesh info "<< std::endl;
    std::cout << "\t# triangles " << ifs.GetNCoordIndex() << std::endl;
    std::cout << "\t# coords    " << ifs.GetNCoord() << std::endl;
    std::cout << "\t# normals   " << ifs.GetNNormal() << std::endl;
    for(unsigned int a = 0; a < ifs.GetNumFloatAttributes(); ++a)
    {
        std::cout << "\t# FloatAttribute[" << a << "] " << ifs.GetNFloatAttribute(a) << "," << ifs.GetFloatAttributeDim(a) <<" (" << ifs.GetFloatAttributeType(a) <<")"<< std::endl;
    }
    for(unsigned int a = 0; a < ifs.GetNumIntAttributes(); ++a)
    {
        std::cout << "\t# IntAttribute[" << a << "]  " << ifs.GetNIntAttribute(a) << "," << ifs.GetIntAttributeDim(a) << " (" << ifs.GetIntAttributeType(a) <<")"<< std::endl;
    }

    // compute min/max
    ifs.ComputeMinMax(O3DGC_SC3DMC_MAX_ALL_DIMS); // O3DGC_SC3DMC_DIAG_BB

    BinaryStream bstream((unsigned long)points.size()*8);

    SaveIFS("debug_enc.txt", ifs);
    SC3DMCEncoder<unsigned long> encoder;
    Timer timer;
    timer.Tic();
    encoder.Encode(params, ifs, bstream);
    timer.Toc();
    std::cout << "Encode time (ms) " << timer.GetElapsedTime() << std::endl;

    FILE * fout = fopen(outFileName.c_str(), "wb");
    if (!fout)
    {
        return -1;
    }
    fwrite(bstream.GetBuffer(), 1, bstream.GetSize(), fout);
    fclose(fout);
    std::cout << "Bitstream size (bytes) " << bstream.GetSize() << std::endl;

    std::cout << "Details" << std::endl;
    const SC3DMCStats & stats = encoder.GetStats();
    std::cout << "\t CoordIndex         " << stats.m_timeCoordIndex     << " ms, " << stats.m_streamSizeCoordIndex     <<" bytes (" << 8.0 * stats.m_streamSizeCoordIndex     / ifs.GetNCoord() <<" bpv)" <<std::endl;
    std::cout << "\t Coord              " << stats.m_timeCoord          << " ms, " << stats.m_streamSizeCoord          <<" bytes (" << 8.0 * stats.m_streamSizeCoord          / ifs.GetNCoord() <<" bpv)" <<std::endl;
    std::cout << "\t Normal             " << stats.m_timeNormal         << " ms, " << stats.m_streamSizeNormal         <<" bytes (" << 8.0 * stats.m_streamSizeNormal         / ifs.GetNCoord() <<" bpv)" <<std::endl;
    for(unsigned int a = 0; a < ifs.GetNumFloatAttributes(); ++a)
    {
        std::cout << "\t# FloatAttribute[" << a << "] " << stats.m_timeFloatAttribute[a] << " ms, " << stats.m_streamSizeFloatAttribute[a] <<" bytes (" << 8.0 * stats.m_streamSizeFloatAttribute[a] / ifs.GetNCoord() <<" bpv)" <<std::endl;
    }
    for(unsigned int a = 0; a < ifs.GetNumIntAttributes(); ++a)
    {
        std::cout << "\t# IntAttribute[" << a << "] " << stats.m_timeIntAttribute[a] << " ms, " << stats.m_streamSizeIntAttribute[a] <<" bytes (" << 8.0 * stats.m_streamSizeIntAttribute[a] / ifs.GetNCoord() <<" bpv)" <<std::endl;
    }

    return 0;
}
int testDecode(std::string & fileName)
{
    std::string folder;
    long found = (long)fileName.find_last_of(PATH_SEP);
    if (found != -1)
    {
        folder = fileName.substr(0,found);
    }
    if (folder == "")
    {
        folder = ".";
    }
    std::string file(fileName.substr(found+1));
    std::string outFileName = folder + PATH_SEP + file.substr(0, file.find_last_of(".")) + "_dec.obj";


    std::vector< Vec3<Real> > points;
    std::vector< Vec3<Real> > normals;
    std::vector< Vec2<Real> > texCoords;
    std::vector< Vec3<unsigned long> > triangles;
    std::vector< unsigned long > indexBufferIDs;
    std::vector< Material > materials;
    std::string materialLib;
    std::vector< Real > weights;
    std::vector< long > jointIDs;

    std::string matFileName = folder + PATH_SEP + file.substr(0, file.find_last_of(".")) + ".mat";
    bool ret = LoadMaterials(matFileName.c_str(), materials, materialLib);
    if (ret)
    {
        const size_t numMaterials = materials.size();
        unsigned long n, shift = 0;
        for(size_t i = 0; i < numMaterials; ++i)
        {
            n = materials[i].m_numTriangles + shift;
            indexBufferIDs.resize(n, materials[i].m_id);
            shift = n;
        }
    }
    

    BinaryStream bstream;
    IndexedFaceSet<unsigned long> ifs;


    FILE * fin = fopen(fileName.c_str(), "rb");
    if (!fin)
    {
        return -1;
    }
    fseek(fin, 0, SEEK_END);
    unsigned long size = ftell(fin);
    bstream.Allocate(size);
    rewind(fin);
    unsigned long nread = (unsigned long)fread((void *) bstream.GetBuffer(), 1, size, fin);
    bstream.SetSize(size);
    if (nread != size)
    {
        return -1;
    }
    fclose(fin);
    std::cout << "Bitstream size (bytes) " << bstream.GetSize() << std::endl;

    SC3DMCDecoder<unsigned long> decoder;
    // load header
    Timer timer;
    timer.Tic();
    decoder.DecodeHeader(ifs, bstream);
    timer.Toc();
    std::cout << "DecodeHeader time (ms) " << timer.GetElapsedTime() << std::endl;

    // allocate memory
    triangles.resize(ifs.GetNCoordIndex());
    ifs.SetCoordIndex((unsigned long * const ) &(triangles[0]));

    points.resize(ifs.GetNCoord());
    ifs.SetCoord((Real * const ) &(points[0]));

    if (ifs.GetNNormal() > 0)
    {
        normals.resize(ifs.GetNNormal());
        ifs.SetNormal((Real * const ) &(normals[0]));  
    }
    unsigned int nFloatAttributes = ifs.GetNumFloatAttributes();
    for(unsigned int a  = 0; a < nFloatAttributes; ++a)
    {
        if (ifs.GetFloatAttributeType(a) == O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_TEXCOORD)
        {
            texCoords.resize(ifs.GetNFloatAttribute(a));
            ifs.SetFloatAttribute(a, (Real * const ) &(texCoords[0]));
        }
        else if (ifs.GetFloatAttributeType(a) == O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_WEIGHT)
        {
            weights.resize(ifs.GetNFloatAttribute(a) * ifs.GetFloatAttributeDim(a));
            ifs.SetFloatAttribute(a, (Real * const ) &(weights[0]));
        }
    }
    unsigned int nIntAttributes = ifs.GetNumIntAttributes();
    for(unsigned int a  = 0; a < nIntAttributes; ++a)
    {
        if (ifs.GetIntAttributeType(a) == O3DGC_IFS_INT_ATTRIBUTE_TYPE_JOINT_ID)
        {
            jointIDs.resize(ifs.GetNIntAttribute(a) * ifs.GetIntAttributeDim(a));
            ifs.SetIntAttribute(a, (long * const ) &(jointIDs[0]));
        }
    }

    std::cout << "Mesh info "<< std::endl;
    std::cout << "\t# triangles " << ifs.GetNCoordIndex() << std::endl;
    std::cout << "\t# coords    " << ifs.GetNCoord() << std::endl;
    std::cout << "\t# normals   " << ifs.GetNNormal() << std::endl;
    for(unsigned int a = 0; a < ifs.GetNumFloatAttributes(); ++a)
    {
        std::cout << "\t# FloatAttribute[" << a << "] " << ifs.GetNFloatAttribute(a) << "," << ifs.GetFloatAttributeDim(a) <<" (" << ifs.GetFloatAttributeType(a) <<")"<< std::endl;
    }
    for(unsigned int a = 0; a < ifs.GetNumIntAttributes(); ++a)
    {
        std::cout << "\t# IntAttribute[" << a << "]  " << ifs.GetNIntAttribute(a) << "," << ifs.GetIntAttributeDim(a) << " (" << ifs.GetIntAttributeType(a) <<")"<< std::endl;
    }

    // decode mesh
    timer.Tic();
    decoder.DecodePlayload(ifs, bstream);
    timer.Toc();
    std::cout << "DecodePlayload time (ms) " << timer.GetElapsedTime() << std::endl;

    std::cout << "Details" << std::endl;
    const SC3DMCStats & stats = decoder.GetStats();
    std::cout << "\t CoordIndex         " << stats.m_timeCoordIndex     << " ms, " << stats.m_streamSizeCoordIndex     <<" bytes (" << 8.0 * stats.m_streamSizeCoordIndex     / ifs.GetNCoord() <<" bpv)" <<std::endl;
    std::cout << "\t Coord              " << stats.m_timeCoord          << " ms, " << stats.m_streamSizeCoord          <<" bytes (" << 8.0 * stats.m_streamSizeCoord          / ifs.GetNCoord() <<" bpv)" <<std::endl;
    std::cout << "\t Normal             " << stats.m_timeNormal         << " ms, " << stats.m_streamSizeNormal         <<" bytes (" << 8.0 * stats.m_streamSizeNormal         / ifs.GetNCoord() <<" bpv)" <<std::endl;
    for(unsigned int a = 0; a < ifs.GetNumFloatAttributes(); ++a)
    {
        std::cout << "\t# FloatAttribute[" << a << "] " << stats.m_timeFloatAttribute[a] << " ms, " << stats.m_streamSizeFloatAttribute[a] <<" bytes (" << 8.0 * stats.m_streamSizeFloatAttribute[a] / ifs.GetNCoord() <<" bpv)" <<std::endl;
    }
    for(unsigned int a = 0; a < ifs.GetNumIntAttributes(); ++a)
    {
        std::cout << "\t# IntAttribute[" << a << "] " << stats.m_timeIntAttribute[a] << " ms, " << stats.m_streamSizeIntAttribute[a] <<" bytes (" << 8.0 * stats.m_streamSizeIntAttribute[a] / ifs.GetNCoord() <<" bpv)" <<std::endl;
    }
    std::cout << "\t Reorder            " << stats.m_timeReorder        << " ms,  " << 0 <<" bytes (" << 0.0 <<" bpv)" <<std::endl;

    std::cout << "Saving " << outFileName << " ..." << std::endl;

    SaveIFS("debug_dec.txt", ifs);
    ret = SaveOBJ(outFileName.c_str(), points, texCoords, normals, triangles, materials, indexBufferIDs, materialLib);
    if (!ret)
    {
        std::cout << "Error: SaveOBJ()\n" << std::endl;
        return -1;
    }
    ret = Check(ifs);
    if (!ret)
    {
        std::cout << "Error: SaveOBJ()\n" << std::endl;
        return -1;
    }

    std::cout << "Done." << std::endl;
    return 0;
}

enum Mode
{
    UNKNOWN = 0,
    ENCODE  = 1,
    DECODE  = 2
};

int testDynamicVectorCompression()
{
    const std::string outFileName("lift.s3d");
    const unsigned long N = 500;
    const unsigned long D = 3;
    const unsigned long S = D*N;
    Real vectors[N*D];
    Real max[D];
    Real min[D];
    const Real e = Real(6.30) / N;
    Real x = Real(0.0);
    long p = 0;
    for(long n = 0; n < N; ++n)
    {
        vectors[p++] = cos(x);
        vectors[p++] = sin(x);
        vectors[p++] = sin(x) * cos(x);
        x += e;
    }

    DynamicVector dynamicVector0;
    dynamicVector0.SetVectors(vectors);
    dynamicVector0.SetDimVector(3);
    dynamicVector0.SetMax(max);
    dynamicVector0.SetMin(min);
    dynamicVector0.SetNVector(N);
    dynamicVector0.SetStride(3);
    dynamicVector0.ComputeMinMax(O3DGC_SC3DMC_MAX_ALL_DIMS);

    DVEncodeParams params;
    params.SetQuantBits(10);
    params.SetStreamType(O3DGC_STREAM_TYPE_ASCII);
//    params.SetStreamType(O3DGC_STREAM_TYPE_BINARY);
    
    BinaryStream bstream(N*D*16);

    DynamicVectorEncoder encoder;

    encoder.SetStreamType(O3DGC_STREAM_TYPE_BINARY);
    Timer timer;
    timer.Tic();
    encoder.Encode(params, dynamicVector0, bstream);
    timer.Toc();
    std::cout << "Encode time (ms) " << timer.GetElapsedTime() << std::endl;


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

    FILE * fout = fopen(outFileName.c_str(), "wb");
    if (!fout)
    {
        return -1;
    }
    fwrite(bstream.GetBuffer(), 1, bstream.GetSize(), fout);
    fclose(fout);
    std::cout << "Bitstream size (bytes) " << bstream.GetSize() << " (" << bstream.GetSize() * 8.0 / dynamicVector1.GetNVector() << " bpv)" <<std::endl;
    return 0;
}

int testIFSCompression(int argc, char * argv[])
{
        Mode mode = UNKNOWN;
    std::string inputFileName;
    int qcoord    = 12;
    int qtexCoord = 10;
    int qnormal   = 8;
    O3DGCStreamType streamType = O3DGC_STREAM_TYPE_BINARY;
    for(int i = 1; i < argc; ++i)
    {
        if ( !strcmp(argv[i], "-c"))
        {
            mode = ENCODE;
        }
        else if ( !strcmp(argv[i], "-d"))
        {
            mode = DECODE;
        }
        else if ( !strcmp(argv[i], "-i"))
        {
            ++i;
            if (i < argc)
            {
                inputFileName = argv[i];
            }
        }
        else if ( !strcmp(argv[i], "-qc"))
        {
            ++i;
            if (i < argc)
            {
                qcoord = atoi(argv[i]);
            }
        }
        else if ( !strcmp(argv[i], "-qn"))
        {
            ++i;
            if (i < argc)
            {
                qnormal = atoi(argv[i]);
            }
        }
        else if ( !strcmp(argv[i], "-qt"))
        {
            ++i;
            if (i < argc)
            {
                qtexCoord = atoi(argv[i]);
            }
        }
        else if ( !strcmp(argv[i], "-st"))
        {
            ++i;
            if (i < argc)
            {
                if (!strcmp(argv[i], "ascii"))
                {
                    streamType = O3DGC_STREAM_TYPE_ASCII;
                }
            }
        }
    }

    if (inputFileName.size() == 0 || mode == UNKNOWN)
    {
        std::cout << "Usage: ./test_o3dgc [-c|d] [-qc QuantBits] [-qt QuantBits] [-qn QuantBits] -i fileName.obj "<< std::endl;
        std::cout << "\t -c \t Encode"<< std::endl;
        std::cout << "\t -d \t Decode"<< std::endl;
        std::cout << "\t -qc \t Quantization bits for positions (default=11, range = {8,...,15})"<< std::endl;
        std::cout << "\t -qn \t Quantization bits for normals (default=10, range = {8,...,15})"<< std::endl;
        std::cout << "\t -qt \t Quantization bits for texture coordinates (default=10, range = {8,...,15})"<< std::endl;
        std::cout << "\t -st \t Stream type (default=Bin, range = {binary, ascii})"<< std::endl;
        std::cout << "Examples:"<< std::endl;
        std::cout << "\t Encode binary: test_o3dgc -c -i fileName.obj -st binary"<< std::endl;
        std::cout << "\t Encode ascii:  test_o3dgc -c -i fileName.obj -st ascii "<< std::endl;
        std::cout << "\t Decode:        test_o3dgc -d -i fileName.s3d"<< std::endl;
        return -1;
    }

    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Encode Parameters " << std::endl;
    std::cout << "   Input           \t "<< inputFileName << std::endl;

    int ret;
    if (mode == ENCODE)
    {
        std::cout << "   Coord Quant.    \t "<< qcoord << std::endl;
        std::cout << "   Normal Quant.   \t "<< qnormal << std::endl;
        std::cout << "   TexCoord Quant. \t "<< qtexCoord << std::endl;
        std::cout << "   Stream Type     \t "<< ((streamType == O3DGC_STREAM_TYPE_ASCII)? "ASCII" : "Binary") << std::endl;
        ret = testEncode(inputFileName, qcoord, qtexCoord, qnormal, streamType);
    }
    else
    {
        ret = testDecode(inputFileName);
    }
    if (ret)
    {
        std::cout << "Error " << ret << std::endl;
        return ret;
    }
    return 0;
}

int main(int argc, char * argv[])
{
#ifdef TEST_DYNAMIC_VECTOR_ENCODING
    return testDynamicVectorCompression();
#else
    return testIFSCompression(argc, argv);
#endif
}
bool LoadOBJ(const std::string & fileName, 
             std::vector< Vec3<Real> > & upoints,
             std::vector< Vec2<Real> > & utexCoords,
             std::vector< Vec3<Real> > & unormals,
             std::vector< Vec3<unsigned long> > & triangles,
             std::vector< unsigned long > & indexBufferIDs,
             std::vector< Material > & materials,
             std::string & materialLib) 
{   
    const char ObjDelimiters[]=" /";
    const unsigned long BufferSize = 1024;
    FILE * fid = fopen(fileName.c_str(), "r");
    
    if (fid) 
    {        
        char buffer[BufferSize];
        Real  x[3];
        unsigned long ip[3] = {(unsigned long)(-1), (unsigned long)(-1), (unsigned long)(-1)};
        unsigned long in[3] = {(unsigned long)(-1), (unsigned long)(-1), (unsigned long)(-1)};
        unsigned long it[3] = {(unsigned long)(-1), (unsigned long)(-1), (unsigned long)(-1)};
        unsigned long indexBufferID = 0;
        unsigned long numMatID = 0;
        char * pch;
        char * str;
        unsigned long nv = 0;
        Vec3<unsigned long> vertex;
        Vec3<unsigned long> triangle;
        std::vector< Vec3<Real> > points;
        std::vector< Vec2<Real> > texCoords;
        std::vector< Vec3<Real> > normals;
        std::map< Vec3<unsigned long>, unsigned long, IVec3Cmp > vertices;
        std::map< std::string, unsigned long > matMap;
        materialLib.clear();        
        indexBufferIDs.clear();
        materials.clear();
        while (!feof(fid)) 
        {
            if (!fgets(buffer, BufferSize, fid))
            {
                break;
            }
            if (buffer[0] == 'u')
            {
                str = buffer;
                pch = strtok (str, " ");
                if ( !strcmp(pch, "usemtl") )
                {
                    pch = strtok (NULL, " ");
                    std::map< std::string, unsigned long >::iterator it = matMap.find(pch);
                    if ( it == matMap.end() )
                    {
                        indexBufferID  = numMatID++;
                        matMap[pch]    = indexBufferID;
                        materials.push_back(Material(indexBufferID, 0, pch));
                    }
                    else
                    {
                        indexBufferID = it->second; 
                    }
                }
            }
            else if (buffer[0] == 'm')
            {
                str = buffer;
                pch = strtok (str, " ");
                if ( !strcmp(pch, "mtllib") )
                {
                    pch = strtok (NULL, " ");
                    materialLib = pch;
                }
            } 
            else if (buffer[0] == 'v')
            {
                if (buffer[1] == ' ')
                {                    
                    str = buffer+2;
                    for(int k = 0; k < 3; ++k)
                    {
                        pch = strtok (str, " ");
                        if (pch) x[k] = (Real) atof(pch);
                        else
                        {
                            return false;
                        }
                        str = NULL;
                    }
                    points.push_back( Vec3<Real>(x[0], x[1], x[2]) );
                }
                else if (buffer[1] == 'n')
                {
                    str = buffer+2;
                    for(int k = 0; k < 3; ++k)
                    {
                        pch = strtok (str, " ");
                        if (pch) x[k] = (Real) atof(pch);
                        else
                        {
                            return false;
                        }
                        str = NULL;
                    }
                    normals.push_back( Vec3<Real>(x[0], x[1], x[2]) );
                }
                else if (buffer[1] == 't')
                {
                    str = buffer+2;
                    for(int k = 0; k < 2; ++k)
                    {
                        pch = strtok (str, " ");
                        if (pch) x[k] = (Real) atof(pch);
                        else
                        {
                            return false;
                        }
                        str = NULL;
                    }                  
                    texCoords.push_back( Vec2<Real>(x[0], x[1]) );
                }
            }
            else if (buffer[0] == 'f')
            {

                str = buffer+2;
                for(int k = 0; k < 3; ++k)
                {
                    pch = strtok (str, ObjDelimiters);
                    if (pch) ip[k] = atoi(pch) - 1;
                        else
                        {
                            return false;
                        }
                    str = NULL;
                    if (texCoords.size() > 0)
                    {
                        pch = strtok (NULL, ObjDelimiters);
                        if (pch)  it[k] = atoi(pch) - 1;
                        else
                        {
                            return false;
                        }
                    }
                    if (normals.size() > 0)
                    {
                        pch = strtok (NULL, ObjDelimiters);
                        if (pch)  in[k] = atoi(pch) - 1;
                        else
                        {
                            return false;
                        }
                    }
                }
                for(int k = 0; k < 3; ++k)
                {
                    vertex.X() = ip[k];
                    vertex.Y() = in[k];
                    vertex.Z() = it[k];
                    std::map< Vec3<unsigned long>, unsigned long, IVec3Cmp >::iterator it = vertices.find(vertex);
                    if ( it == vertices.end() )
                    {
                        vertices[vertex] = nv;
                        triangle[k]      = nv;
                        ++nv;
                    }
                    else
                    {
                        triangle[k]         =  it->second;
                    }
                }                
                triangles.push_back(triangle);
                if (materials.size() > 0)
                {
                    ++materials[indexBufferID].m_numTriangles;
                    indexBufferIDs.push_back(indexBufferID);
                }
            }
        }
        if (points.size() > 0)
        {
            upoints.resize(nv);
        }
        if (normals.size() > 0)
        {
            unormals.resize(nv);
        }
        if (texCoords.size() > 0)
        {
            utexCoords.resize(nv);
        }
        for (std::map< Vec3<unsigned long>, unsigned long, IVec3Cmp >::iterator it = vertices.begin(); it != vertices.end(); ++it)
        {
            if (points.size() > 0)
            {
                upoints   [it->second]    = points   [(it->first).X()];
            }
            if (normals.size() > 0)
            {
                unormals  [it->second]    = normals  [(it->first).Y()];
            }
            if (texCoords.size() > 0)
            {
                utexCoords[it->second]    = texCoords[(it->first).Z()];
            }
        }
        fclose(fid);
    }
    else 
    {
        std::cout << "File not found" << std::endl;
        return false;
    }
    return true;
}
bool SaveOBJ(const std::string & fileName, 
             const std::vector< Vec3<Real> > & points,
             const std::vector< Vec2<Real> > & texCoords,
             const std::vector< Vec3<Real> > & normals,
             const std::vector< Vec3<unsigned long> > & triangles,
             const std::vector< Material > & materials,
             const std::vector< unsigned long > indexBufferIDs,
             const std::string & materialLib)
{
    std::ofstream fout;
    fout.open(fileName.c_str());
    if (!fout.fail()) 
    {
        const unsigned long np = (unsigned long) points.size();
        const unsigned long nn = (unsigned long) normals.size();
        const unsigned long nt = (unsigned long) texCoords.size();
        const unsigned long nf = (unsigned long) triangles.size();
        const bool useMaterial = (materials.size() > 0 && indexBufferIDs.size());
        unsigned long indexBufferID;

        fout << "####" << std::endl;
        fout << "#" << std::endl;
        fout << "# OBJ File Generated by test_o3dgc" << std::endl;
        fout << "#" << std::endl;
        fout << "####" << std::endl;
        fout << "# Object " << fileName << std::endl;
        fout << "#" << std::endl;
        fout << "# Coord:     " << np << std::endl;
        fout << "# Normals:   " << nn << std::endl;
        fout << "# TexCoord:  " << nt << std::endl;;
        fout << "# Triangles: " << nf << std::endl;;
        fout << "#" << std::endl;
        fout << "####" << std::endl;
        for(unsigned long i = 0; i < np; ++i)
        {
            fout << "v " << points[i].X() << " " << points[i].Y() << " " << points[i].Z() << std::endl;
        }
        for(unsigned long i = 0; i < nn; ++i)
        {
            fout << "vn " << normals[i].X() << " " << normals[i].Y() << " " << normals[i].Z() << std::endl;
        }
        for(unsigned long i = 0; i < nt; ++i)
        {
            fout << "vt " << texCoords[i].X() << " " << texCoords[i].Y() << std::endl;
        }
        if (!materialLib.empty())
        {
            fout <<"mtllib " << materialLib << std::endl;
        }
        if (useMaterial)
        {
            indexBufferID = indexBufferIDs[0];
            fout <<"usemtl " << materials[indexBufferID].m_name << std::endl;
        }
        if (nt > 0 && nn >0)
        {
            for(unsigned long i = 0; i < nf; ++i)
            {
                if (useMaterial && indexBufferID != indexBufferIDs[i])
                {
                    indexBufferID = indexBufferIDs[i];
                    fout <<"usemtl " << materials[indexBufferID].m_name << std::endl;
                }
                fout << "f " << triangles[i].X()+1 << "/" << triangles[i].X()+1 << "/" << triangles[i].X()+1;
                fout << " "  << triangles[i].Y()+1 << "/" << triangles[i].Y()+1 << "/" << triangles[i].Y()+1;
                fout << " "  << triangles[i].Z()+1 << "/" << triangles[i].Z()+1 << "/" << triangles[i].Z()+1 << std::endl;
            }
        }
        else if (nt == 0 && nn > 0)
        {
            for(unsigned long i = 0; i < nf; ++i)
            {
                if (useMaterial && indexBufferID != indexBufferIDs[i])
                {
                    indexBufferID = indexBufferIDs[i];
                    fout <<"usemtl " << materials[indexBufferID].m_name << std::endl;
                }
                fout << "f " << triangles[i].X()+1 << "//" << triangles[i].X()+1;
                fout << " "  << triangles[i].Y()+1 << "//" << triangles[i].Y()+1;
                fout << " "  << triangles[i].Z()+1 << "//" << triangles[i].Z()+1 << std::endl;
            }
        }
        else if (nt > 0 && nn == 0)
        {
            for(unsigned long i = 0; i < nf; ++i)
            {
                if (useMaterial && indexBufferID != indexBufferIDs[i])
                {
                    indexBufferID = indexBufferIDs[i];
                    fout <<"usemtl " << materials[indexBufferID].m_name << std::endl;
                }
                fout << "f " << triangles[i].X()+1 << "/" << triangles[i].X()+1;
                fout << " "  << triangles[i].Y()+1 << "/" << triangles[i].Y()+1;
                fout << " "  << triangles[i].Z()+1 << "/" << triangles[i].Z()+1 << std::endl;
            }
        }
        else
        {
            for(unsigned long i = 0; i < nf; ++i)
            {
                if (useMaterial && indexBufferID != indexBufferIDs[i])
                {
                    indexBufferID = indexBufferIDs[i];
                    fout <<"usemtl " << materials[indexBufferID].m_name << std::endl;
                }
                fout << "f " << triangles[i].X()+1;
                fout << " "  << triangles[i].Y()+1;
                fout << " "  << triangles[i].Z()+1 << std::endl;
            }
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
bool SaveMaterials(const std::string & fileName, const std::vector< Material > & materials, const std::string & materialLib)
{
    std::ofstream fout;
    fout.open(fileName.c_str());
    if (!fout.fail()) 
    {
        const size_t numMaterials = materials.size();
        fout << "MaterialLib " << materialLib << std::endl;
        fout << "Materials " << numMaterials << std::endl;
        for(size_t i = 0; i < numMaterials; ++i)
        {
            fout << materials[i].m_id << " " << materials[i].m_numTriangles << " " << materials[i].m_name << std::endl;
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

bool LoadMaterials(const std::string & fileName, std::vector< Material > & materials, std::string & materialLib)
{
    std::ifstream fin;
    materials.clear();
    materialLib.clear();
    fin.open(fileName.c_str());
    if (!fin.fail()) 
    {
        size_t numMaterials = 0;
        std::string tmp;
        while(!fin.eof())
        {
            fin >> tmp;
            if (tmp == "MaterialLib")
            {
                fin >> materialLib;
            }
            else if (tmp == "Materials")
            {
                fin >> numMaterials;
                materials.resize(numMaterials);
                for(size_t i = 0; i < numMaterials; ++i)
                {
                    fin >> materials[i].m_id;
                    fin >> materials[i].m_numTriangles;
                    fin >> materials[i].m_name;
                }
                fin.close();
                return true;
            }        
        }
        fin.close();
    }
    else 
    {
        std::cout << "Not able to load file" << std::endl;
    }
    return false;
}
template <class T>
void SaveIFSArray(std::ofstream & fout, 
                  const std::string & name, 
                  unsigned int a, 
                  const T * const tab, 
                  unsigned long nElement, 
                  unsigned long dim,
                  int type)
{
    if (tab)
    {
        fout << name << "\t" << a << "\t" << type << "\t" << nElement << "\t" << dim << std::endl;
    }
    else
    {
        fout << name << "\t" << a << "\t" << type << "\t" << 0 << "\t" << 0 << std::endl;
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
bool SaveIFS(const std::string & fileName, const IndexedFaceSet<unsigned long> & ifs)
{
    std::ofstream fout;
    fout.open(fileName.c_str());
    if (!fout.fail()) 
    {
        SaveIFSArray(fout, "* CoordIndex", 0, ifs.GetCoordIndex(), ifs.GetNCoordIndex(), 3, (int) O3DGC_IFS_INT_ATTRIBUTE_TYPE_INDEX);
        SaveIFSArray(fout, "* IndexBufferID", 0, (const long * const) ifs.GetIndexBufferID(), ifs.GetNCoordIndex(), 1, (int) O3DGC_IFS_INT_ATTRIBUTE_TYPE_INDEX_BUFFER_ID);
        SaveIFSArray(fout, "* Coord", 0, ifs.GetCoord(), ifs.GetNCoord(), 3, (int) O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_POSITION);
        SaveIFSArray(fout, "* Normal", 0, ifs.GetNormal(), ifs.GetNNormal(), 3, (int) O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_NORMAL);
        for(unsigned long a = 0; a < ifs.GetNumFloatAttributes(); ++a)
        {
            SaveIFSArray(fout, "* FloatAttribute", a, ifs.GetFloatAttribute(a), ifs.GetNFloatAttribute(a), ifs.GetFloatAttributeDim(a), ifs.GetFloatAttributeType(a));
        }
        for(unsigned long a = 0; a < ifs.GetNumIntAttributes(); ++a)
        {
            SaveIFSArray(fout, "* IntAttribute", a, ifs.GetIntAttribute(a), ifs.GetNIntAttribute(a), ifs.GetIntAttributeDim(a), ifs.GetIntAttributeType(a));
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
bool LoadIFS(const std::string & fileName, 
             std::vector< Vec3<Real> > & points,
             std::vector< Vec2<Real> > & texCoords,
             std::vector< Vec3<Real> > & normals,
             std::vector< Vec3<unsigned long> > & triangles,
             std::vector< unsigned long > & indexBufferIDs)
{
    std::ifstream fin;
    fin.open(fileName.c_str());
    if (!fin.fail()) 
    {
        unsigned long a, n, dim;
        int type;
        std::string tmp;
        while(!fin.eof())
        {
            fin >> tmp;
            if (tmp == "IndexBufferID")
            {
                fin >> a >> type >> n >> dim;
                indexBufferIDs.resize(n);
                for(unsigned long i = 0, p = 0; i < n; ++i)
                {
                    fin >> tmp;
                    fin >> indexBufferIDs[i];
                }
            }
            else if (tmp == "CoordIndex")
            {
                fin >> a >> type >> n >> dim;
                triangles.resize(n);
                for(unsigned long i = 0, p = 0; i < n; ++i)
                {
                    fin >> tmp;
                    for(unsigned long d = 0; d < dim; ++d)
                    {
                        fin >> triangles[i][d];
                    }
                }
            }
            else if (tmp == "Coord")
            {
                fin >> a >> type >> n >> dim;
                points.resize(n);
                for(unsigned long i = 0, p = 0; i < n; ++i)
                {
                    fin >> tmp;
                    for(unsigned long d = 0; d < dim; ++d)
                    {
                        fin >> points[i][d];
                    }
                }
            }
            else if (tmp == "Normal")
            {
                fin >> a >> type >> n >> dim;
                normals.resize(n);
                for(unsigned long i = 0, p = 0; i < n; ++i)
                {
                    fin >> tmp;
                    for(unsigned long d = 0; d < dim; ++d)
                    {
                        fin >> normals[i][d];
                    }
                }
            }
            else if (tmp == "FloatAttribute")
            {
                fin >> a >> type >> n >> dim;
                if (type == O3DGC_IFS_FLOAT_ATTRIBUTE_TYPE_TEXCOORD)
                {
                    texCoords.resize(n);
                    for(unsigned long i = 0, p = 0; i < n; ++i)
                    {
                        fin >> tmp;
                        for(unsigned long d = 0; d < dim; ++d)
                        {
                            fin >> texCoords[i][d];
                        }
                    }
                }
            }
        }
        fin.close();
        return true;
    }
    else 
    {
        std::cout << "Not able to load file" << std::endl;
        return false;
    }
}
bool Check(const IndexedFaceSet<unsigned long> & ifs)
{
    unsigned int nt = ifs.GetNCoordIndex() * 3;
    unsigned int nv = ifs.GetNCoord();
    const unsigned long * const triangles = ifs.GetCoordIndex();
    for(unsigned int i = 0; i < nt; ++i)
    {
        if ( triangles[i] < 0 || triangles[i] >= nv )
        {
            std::cout << "CoordIndex[" << i << "] = " << triangles[i] << " out of range" << std::endl;
            return false;
        }
    }
    return true;
}

// Copyright (c) 2011-2012, Motorola Mobility, Inc.
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

// TODO: check for - in JSON keys
// TODO: consider LOD
// TODO: normal generation when needed

// DESIGN: justification generation shader. geometry may need regeneration if lambert
// DESIGN: difference between COLLADA and JSON format. In JSON format it is possible to make an interleaved array not only made by float.

#define WRITE_SINGLE_BLOB 1

#include "JSONExport.h"

#include "DAE2JSONWriter.h"

namespace DAE2JSON
{    
    //---- Convert OpenCOLLADA mesh to mesh -------------------------------------------
  
    static unsigned int ConvertOpenCOLLADAMeshVertexDataToJSONAccessors(const COLLADAFW::MeshVertexData &vertexData, JSONExport::IndexSetToAccessorHashmap &accessors) 
    {
        // The following are OpenCOLLADA fmk issues preventing doing a totally generic processing of sources
        //- "set"(s) other than texCoord don't have valid input infos
        //- not the original id in the source
        
        std::string name; 
        size_t length, elementsCount;
        size_t stride = 0;
        size_t size = 0;
        
        size_t setCount = vertexData.getNumInputInfos();    
        bool unpatchedOpenCOLLADA = (setCount == 0); // reliable heuristic to know if the input have not been set
        
        if (unpatchedOpenCOLLADA)
            setCount = 1;
        
        for (int indexOfSet = 0 ; indexOfSet < setCount ; indexOfSet++) {
        
            if (setCount > 0) {
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
        
            JSONExport::SourceType sourceType = JSONExport::NOT_A_SOURCE_TYPE;
            switch (vertexData.getType()) {
                case MeshVertexData::DATA_TYPE_FLOAT: {
                    sourceType = JSONExport::FLOAT;
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
            shared_ptr <JSONExport::JSONDataBuffer> cvtBuffer(new JSONExport::JSONDataBuffer(name, sourceData, sourceSize, false));
            
            shared_ptr <JSONExport::JSONAccessor> cvtAccessor(new JSONExport::JSONAccessor());
        
            cvtAccessor->setBuffer(cvtBuffer);
            cvtAccessor->setElementsPerVertexAttribute(size);
            cvtAccessor->setByteStride(stride);
            cvtAccessor->setElementType(sourceType);
            cvtAccessor->setByteOffset(0);
            cvtAccessor->setCount(elementsCount);
            
            accessors[indexOfSet] = cvtAccessor;
        }
        
        return setCount;
    }
    
    static shared_ptr <JSONExport::JSONPrimitive> ConvertOpenCOLLADAMeshPrimitive(COLLADAFW::MeshPrimitive* openCOLLADAMeshPrimitive) 
    {
        shared_ptr <JSONExport::JSONPrimitive> cvtPrimitive(new JSONExport::JSONPrimitive());
        
        // We want to match OpenGL/ES mode , as WebGL spec points to OpenGL/ES spec...
        // "Symbolic constants GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, and GL_TRIANGLES are accepted."
        String type;
        switch(openCOLLADAMeshPrimitive->getPrimitiveType()) {
                //these 2 requires transforms
            case MeshPrimitive::POLYLIST:
            case MeshPrimitive::POLYGONS:
                // FIXME: perform conversion, but until done report error
                //these mode are supported by WebGL                    
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
        shared_ptr <JSONExport::JSONDataBuffer> positionBuffer(new JSONExport::JSONDataBuffer(openCOLLADAMeshPrimitive->getPositionIndices().getData(), count * sizeof(unsigned int), false)); 
        shared_ptr <JSONExport::JSONIndices> positionIndices(new JSONExport::JSONIndices(positionBuffer,
                                                                                        count,
                                                                                         JSONExport::VERTEX,
                                                                                         0));
        
        cvtPrimitive->appendIndices(positionIndices);
        
        if (openCOLLADAMeshPrimitive->hasNormalIndices()) {
            if (count != openCOLLADAMeshPrimitive->getNormalIndices().getCount()) {
                // FIXME: report error
            }
            
            shared_ptr <JSONExport::JSONDataBuffer> normalBuffer(new JSONExport::JSONDataBuffer(openCOLLADAMeshPrimitive->getNormalIndices().getData(), count * sizeof(unsigned int), false)); 
            shared_ptr <JSONExport::JSONIndices> normalIndices(new JSONExport::JSONIndices(normalBuffer,
                                                                                           count,
                                                                                           JSONExport::NORMAL,
                                                                                           0));
            
            cvtPrimitive->appendIndices(normalIndices);
        }
        
        if (openCOLLADAMeshPrimitive->hasColorIndices()) {
            IndexListArray& colorListArray = openCOLLADAMeshPrimitive->getColorIndicesArray();
            
            for (int i = 0 ; i < colorListArray.getCount() ; i++) {
                IndexList* indexList = openCOLLADAMeshPrimitive->getColorIndices(i);
                
                UIntValuesArray& indices = indexList->getIndices();
                if (count != indices.getCount()) {
                    // FIXME: report error
                }

                shared_ptr <JSONExport::JSONDataBuffer> colorBuffer(new JSONExport::JSONDataBuffer(indices.getData(), count * sizeof(unsigned int), false)); 

                shared_ptr <JSONExport::JSONIndices> colorIndices(new JSONExport::JSONIndices(indexList->getName(),
                                                                                              colorBuffer,
                                                                                              count,
                                                                                              JSONExport::COLOR,
                                                                                              indexList->getSetIndex()));

                cvtPrimitive->appendIndices(colorIndices);
            }
        }
        
        if (openCOLLADAMeshPrimitive->hasUVCoordIndices()) {                        
            IndexListArray& uvListArray = openCOLLADAMeshPrimitive->getUVCoordIndicesArray();
            
            for (int i = 0 ; i < uvListArray.getCount() ; i++) {
                IndexList* indexList = openCOLLADAMeshPrimitive->getUVCoordIndices(i);
                                
                UIntValuesArray& indices = indexList->getIndices();
                if (count != indices.getCount()) {
                    // FIXME: report error
                }
                
                shared_ptr <JSONExport::JSONDataBuffer> uvBuffer(new JSONExport::JSONDataBuffer(indices.getData(), count * sizeof(unsigned int), false)); 
                shared_ptr <JSONExport::JSONIndices> uvIndices(new JSONExport::JSONIndices(indexList->getName(),
                                                                                           uvBuffer,
                                                                                           count,
                                                                                           JSONExport::TEXCOORD,
                                                                                           indexList->getSetIndex()));
                        
                cvtPrimitive->appendIndices(uvIndices);
            }
        }
        
        return cvtPrimitive;
    }
    
    shared_ptr <JSONExport::JSONMesh> ConvertOpenCOLLADAMesh(COLLADAFW::Mesh* openCOLLADAMesh, std::string folder) 
    {
        shared_ptr <JSONExport::JSONMesh> cvtMesh(new JSONExport::JSONMesh());
                
        cvtMesh->setID(openCOLLADAMesh->getOriginalId());
        cvtMesh->setName(openCOLLADAMesh->getName());

        URI dataURL(folder);
        String dataURLString = dataURL.getURIString();        
        cvtMesh->setDirectory(dataURLString);
        
        const MeshPrimitiveArray& primitives =  openCOLLADAMesh->getMeshPrimitives();
        size_t primitiveCount = primitives.getCount();
        
        // get all primitives
        for (int i = 0 ; i < primitiveCount ; i++) {
            
            shared_ptr <JSONExport::JSONPrimitive> primitive = ConvertOpenCOLLADAMeshPrimitive(primitives[i]);
            cvtMesh->appendPrimitive(primitive);

            // once we got a primitive, keep track of its accessors
            std::vector< shared_ptr<JSONExport::JSONIndices> > allIndices = primitive->allIndices();
            for (int k = 0 ; k < allIndices.size() ; k++) {
                shared_ptr<JSONExport::JSONIndices> indices = allIndices[k];
                JSONExport::Semantic semantic = indices->getSemantic();
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
        
        return cvtMesh;
    }
        
    //--------------------------------------------------------------------
	DAE2JSONWriter::DAE2JSONWriter( const COLLADABU::URI& inputFile, PrettyWriter <FileStream> *jsonWriter ):
    _inputFile(inputFile),
     _visualScene(0)
	{
        this->_writer.setWriter(jsonWriter);
	}
    
	//--------------------------------------------------------------------
	DAE2JSONWriter::~DAE2JSONWriter()
	{
	}
    
	//--------------------------------------------------------------------
	void DAE2JSONWriter::reportError( const String& method, const String& message)
	{
	}
    
	//--------------------------------------------------------------------
	bool DAE2JSONWriter::write()
	{
        this->_uniqueIDToMesh.clear();   
        this->_allBuffers.clear();

#if WRITE_SINGLE_BLOB
        URI dataURL(this->_inputFile.getPathDir());
        String dataURLString = dataURL.getURIString() + "buffer.1";        
        
        this->_fileBuffer = shared_ptr <JSONExport::JSONFileBuffer> (new JSONExport::JSONFileBuffer(dataURLString.c_str()));    
        this->_allBuffers.push_back(this->_fileBuffer);
#endif
        
        this->_rootJSONObject = shared_ptr <JSONExport::JSONObject> (new JSONExport::JSONObject());
        this->_rootJSONObject->setString("version", "0.1");
        this->_rootJSONObject->setValue("nodes", shared_ptr <JSONExport::JSONObject> (new JSONExport::JSONObject()));
                                
        COLLADASaxFWL::Loader loader;
		COLLADAFW::Root root(&loader, this);
         
		if (!root.loadDocument(_inputFile.toNativePath()))
			return false;
        
        UniqueIDToMesh::const_iterator UniqueIDToMeshIterator;

        // ----
        shared_ptr <JSONExport::JSONObject> meshesObject(new JSONExport::JSONObject());
        
        this->_rootJSONObject->setValue("meshes", meshesObject);
        
        for (UniqueIDToMeshIterator = this->_uniqueIDToMesh.begin() ; UniqueIDToMeshIterator != this->_uniqueIDToMesh.end() ; UniqueIDToMeshIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            shared_ptr <JSONExport::JSONMesh> mesh = (*UniqueIDToMeshIterator).second;
            
            shared_ptr <JSONExport::JSONObject> meshObject = this->_writer.serializeMesh(mesh.get(), 0);
            
            meshesObject->setValue(mesh->getID(), meshObject);
        }
        
        // ----
        shared_ptr <JSONExport::JSONObject> materialsObject(new JSONExport::JSONObject());

        this->_rootJSONObject->setValue("materials", materialsObject);

        UniqueIDToEffect::const_iterator UniqueIDToEffectIterator;
        
        for (UniqueIDToEffectIterator = this->_uniqueIDToEffect.begin() ; UniqueIDToEffectIterator != this->_uniqueIDToEffect.end() ; UniqueIDToEffectIterator++) {
            //(*it).first;             // the key value (of type Key)
            //(*it).second;            // the mapped value (of type T)
            shared_ptr <JSONExport::JSONEffect> effect = (*UniqueIDToEffectIterator).second;
            shared_ptr <JSONExport::JSONObject> effectObject = this->_writer.serializeEffect(effect.get(), 0);
            materialsObject->setValue(effect->getID(), effectObject);
        }

        // ----

        shared_ptr <JSONExport::JSONObject> buffersObject(new JSONExport::JSONObject());
        
        this->_rootJSONObject->setValue("buffers", buffersObject);

        for (int i = 0 ; i < this->_allBuffers.size() ; i++) {
            shared_ptr <JSONExport::JSONObject> bufferObject = this->_writer.serializeBuffer(this->_allBuffers[i].get(), 0);
            buffersObject->setValue(this->_allBuffers[i]->getID(), bufferObject);
        }

        // ----
        
        this->_rootJSONObject->write(&this->_writer);
        
		return true;
	}
    
	//--------------------------------------------------------------------
	void DAE2JSONWriter::cancel( const String& errorMessage )
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
    
    String getNodeID(const COLLADAFW::Node* node) 
    {
        String id = "";
        
        id += "node_";
        id += JSONExport::JSONUtils::toString(node->getUniqueId().getObjectId());
         return id;
        
        /* 
        String originalID = node->getOriginalId();
        if (originalID.size() == 0) {
            originalID = node->getName(); //work-around when we don't have a name
        }
        
        //FIXME: report error
        return originalID;
         */
    }
        
    bool DAE2JSONWriter::writeNode(const COLLADAFW::Node* node, shared_ptr <JSONExport::JSONObject> nodesObject) 
    {
        const String& originalID = getNodeID(node);
        
        shared_ptr <JSONExport::JSONObject> nodeObject(new JSONExport::JSONObject());
        
        const NodePointerArray& nodes = node->getChildNodes();
        
        nodeObject->setString("type", "node");
        const COLLADABU::Math::Matrix4 &matrix = node->getTransformationMatrix();
        if (!matrix.isIdentity())
            nodeObject->setValue("matrix", this->serializeMatrix4Array(node->getTransformationMatrix()));
        
        // save mesh
		const InstanceGeometryPointerArray& instanceGeometries = node->getInstanceGeometries();
        
        unsigned count = instanceGeometries.getCount();
        if (count > 0) {
            //FIXME: should not have to have a special attribute name for multiple meshes.
            // this might end up as making mode node, one per mesh.
            
            //JSONExport
            shared_ptr <JSONExport::JSONArray> meshesArray(new JSONExport::JSONArray());
            nodeObject->setValue("meshes", meshesArray);
            
            for (int i = 0 ; i < count; i++) {
                InstanceGeometry* instanceGeometry = instanceGeometries[i];
                
                MaterialBindingArray& materialBindings = instanceGeometry->getMaterialBindings();
                
                unsigned int meshUID = instanceGeometry->getInstanciatedObjectId().getObjectId();
                shared_ptr <JSONExport::JSONMesh> mesh = this->_uniqueIDToMesh[meshUID];
                
                std::vector< shared_ptr<JSONExport::JSONPrimitive> > primitives = mesh->getPrimitives();
                /*
                size_t effectIDsCount = effectIDs.size();
                size_t primitivesCount = primitives.size();
                */
                std::map<unsigned int , unsigned int> materialIDToReferenceID;

                
                for (int j = 0 ; j < primitives.size() ; j++) {
                    shared_ptr <JSONExport::JSONPrimitive> primitive = primitives[j];
                    
                    //FIXME: consider optimizing this with a hashtable, would be better if it was coming that way from OpenCOLLADA
                    unsigned int materialBindingIndex = -1;
                    for (int k = 0; k < materialBindings.getCount() ; k++) {
                        if (materialBindings[k].getMaterialId() == primitive->getMaterialObjectID()) {
                            materialBindingIndex = k;
                        }
                    }
                    
                    //FIXME: asset materialBindingIndex != -1
                    
                    unsigned int referencedMaterialID = (unsigned int)materialBindings[materialBindingIndex].getReferencedMaterial().getObjectId();
                        
                    materialIDToReferenceID[primitive->getMaterialObjectID()] = referencedMaterialID;
                        
                    unsigned int effectID = this->_materialUIDToEffectUID[referencedMaterialID];
                    shared_ptr <JSONExport::JSONEffect> effect = this->_uniqueIDToEffect[effectID];
                        
                    primitive->setMaterialID(effect->getID());
                }
            
                meshesArray->appendValue(shared_ptr <JSONExport::JSONString> (new JSONExport::JSONString(1 + /* HACK: skip first # character" */ instanceGeometry->getURI().getURIString().c_str())));
                
            }
        }
        
        shared_ptr <JSONExport::JSONArray> childrenArray(new JSONExport::JSONArray());
        nodeObject->setValue("children", childrenArray);
        
        count = nodes.getCount();
        
        for (int i = 0 ; i < count ; i++)  {
            childrenArray->appendValue(shared_ptr <JSONExport::JSONString> (new JSONExport::JSONString(getNodeID(nodes[i]))));
        }
        
        nodesObject->setValue(originalID, static_pointer_cast <JSONExport::JSONValue> (nodeObject));
        
        /*
         walk through the tree
         */
        
        for (int i = 0 ; i < count ; i++)  {
            this->writeNode(nodes[i], nodesObject);
        }
        
        const InstanceNodePointerArray& instanceNodes = node->getInstanceNodes();
        count = instanceNodes.getCount();
        for (int i = 0 ; i < count ; i++) {
            InstanceNode* instanceNode  = instanceNodes[i];
            
            String id = "";
            id += "node_";
            id += JSONExport::JSONUtils::toString(instanceNode->getInstanciatedObjectId().getObjectId());
            
            childrenArray->appendValue(shared_ptr <JSONExport::JSONString> (new JSONExport::JSONString(id)));
        }
        
        return true;
    }

    /* 
        For nodes, lights, cameras and materials, contrary to meshes we export directly the nodes without using the JSON intermediate model.
        The main reason is that we only need to make important transformations for meshes.
     */         
    bool DAE2JSONWriter::writeVisualScene( const COLLADAFW::VisualScene* visualScene )
	{
        shared_ptr <JSONExport::JSONObject> scenesObject(new JSONExport::JSONObject());
        
		const NodePointerArray& nodePointerArray = visualScene->getRootNodes();
        size_t nodeCount = nodePointerArray.getCount();
        
        //FIXME: only one visual scene assumed/handled
        shared_ptr <JSONExport::JSONObject> sceneObject(new JSONExport::JSONObject());
        
        
        this->_rootJSONObject->setValue("scenes", scenesObject);

        sceneObject->setString("type", "scene");
        sceneObject->setString("node", "root");

        scenesObject->setValue("defaultScene", sceneObject); //FIXME: should use this id -> visualScene->getOriginalId()
        
        shared_ptr <JSONExport::JSONObject> nodesObject = static_pointer_cast <JSONExport::JSONObject> (this->_rootJSONObject->getValue("nodes"));
        shared_ptr <JSONExport::JSONObject> rootObject(new JSONExport::JSONObject());
        
        //this->_rootJSONObject->setValue("nodes", nodesObject);

        nodesObject->setValue("root", rootObject);
        
        rootObject->setString("type", "node");                
        //rootObject->setValue("matrix",this->serializeMatrix4Array(COLLADABU::Math::Matrix4::IDENTITY));
                
        //first pass to output children name of our root node
        shared_ptr <JSONExport::JSONArray> childrenArray(new JSONExport::JSONArray());
        
        for (int i = 0 ; i < nodeCount ; i++) { 
            shared_ptr <JSONExport::JSONString> nodeIDValue(new JSONExport::JSONString(getNodeID(nodePointerArray[i])));            
            childrenArray->appendValue(static_pointer_cast <JSONExport::JSONValue> (nodeIDValue));
        }
        
        rootObject->setValue("children", childrenArray);
                                      
        //second pass to actually output the JSON of the node themselves (and for all sub nodes)
        for (int i = 0 ; i < nodeCount ; i++) {
            this->writeNode(nodePointerArray[i], nodesObject);
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

        shared_ptr <JSONExport::JSONObject> nodesObject = static_pointer_cast <JSONExport::JSONObject> (this->_rootJSONObject->getValue("nodes"));
        
        size_t count = nodes.getCount();
        for (int i = 0 ; i < count ; i++) {
            const COLLADAFW::Node *node = nodes[i];

            if (!this->writeNode(node,  nodesObject))
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
                shared_ptr <JSONExport::JSONMesh> cvtMesh = this->_uniqueIDToMesh[meshID];
                
                if (!cvtMesh) {
                    cvtMesh = ConvertOpenCOLLADAMesh((COLLADAFW::Mesh*)mesh, _inputFile.getPathDir());
                
                    cvtMesh->buildUniqueIndexes();
                    cvtMesh->writeAllBuffers(this->_fileBuffer, this->_allBuffers);               

                    this->_uniqueIDToMesh[meshID] = cvtMesh;
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
        
        this->_materialUIDToEffectUID[material->getUniqueId().getObjectId()] = (unsigned int)effectUID.getObjectId();
        //printf("material:%s\n",material->getUniqueId().toAscii().c_str());        
		return true;
	}
    
	//--------------------------------------------------------------------
	bool DAE2JSONWriter::writeEffect( const COLLADAFW::Effect* effect )
	{
        const COLLADAFW::CommonEffectPointerArray& commonEffects = effect->getCommonEffects();
        const ColorOrTexture& diffuse = commonEffects[0]->getDiffuse ();
        
        float red = 1, green = 1, blue = 1;
        
        if (diffuse.isColor()) {
            const Color& color = diffuse.getColor();
            red = color.getRed();
            green = color.getGreen();
            blue = color.getBlue();
        }

        std::string uniqueId = "";
#if EXPORT_MATERIALS_AS_EFFECTS        
        uniqueId += "material.";
#else
        uniqueId += "effect.";
#endif
        uniqueId += JSONExport::JSONUtils::toString(effect->getUniqueId().getObjectId());;
        
        shared_ptr <JSONExport::JSONEffect> cvtEffect(new JSONExport::JSONEffect(uniqueId));

        float rgb[3];
        rgb[0] = red;
        rgb[1] = green;
        rgb[2] = blue;
        cvtEffect->setRGBColor(rgb);
        
        this->_uniqueIDToEffect[effect->getUniqueId().getObjectId()] = cvtEffect;

		return true;                
	}
    
	//--------------------------------------------------------------------
	bool DAE2JSONWriter::writeCamera( const COLLADAFW::Camera* camera )
	{
		return true;
	}
    
	//--------------------------------------------------------------------
	bool DAE2JSONWriter::writeImage( const COLLADAFW::Image* image )
	{
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

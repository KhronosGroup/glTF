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

define( ["loader/webgl-tf-loader", "helpers/resource-manager"],
    function(WebGLTFLoader, ResourceManager) {

    // Utilities

    function RgbArraytoHex(colorArray) {
        var r = Math.floor(colorArray[0] * 255),
            g = Math.floor(colorArray[1] * 255),
            b = Math.floor(colorArray[2] * 255),
            a = Math.floor(colorArray[3] ? colorArray[3] * 255 : 255);

        var color = (a << 24) + (r << 16) + (g << 8) + b;

        return color;
    }

    // Geometry processing

    var ColladaJsonClassicGeometry = function() {
        THREE.Geometry.call( this );

        this.totalAttributes = 0;
        this.loadedAttributes = 0;
        this.indicesLoaded = false;
        this.finished = false;

        this.onload = null;

        this.normals = null;
        this.indexArray = null;
    };

    ColladaJsonClassicGeometry.prototype = new THREE.Geometry();
    ColladaJsonClassicGeometry.prototype.constructor = ColladaJsonClassicGeometry;

    ColladaJsonClassicGeometry.prototype.checkFinished = function() {
        if(this.indexArray && this.loadedAttributes === this.totalAttributes) {
            // Build indexed mesh
            var indexArray = this.indexArray;
            var normals = this.normals;
            var a, b, c;
            var i, l;
            var faceNormals = null;

            for(i = 0, l = this.indexArray.length; i < l; i += 3) {
                a = indexArray[i];
                b = indexArray[i+1];
                c = indexArray[i+2];
                if(normals) {
                    faceNormals = [normals[a], normals[b], normals[c]];
                }
                this.faces.push( new THREE.Face3( a, b, c, faceNormals, null, null ) );
            }

            // Allow Three.js to calculate some values for us
            this.computeCentroids();
            if(!normals) {
                this.computeFaceNormals();
            }

            this.finished = true;

            if(this.onload) {
                this.onload();
            }
        }
    };

    // Delegate for processing index buffers
    var IndicesDelegate = function() {};

    IndicesDelegate.prototype.handleError = function(errorCode, info) {
        // FIXME: report error
        console.log("ERROR(IndicesDelegate):"+errorCode+":"+info);
    };

    IndicesDelegate.prototype.convert = function(resource, ctx) {
        return new Uint16Array(resource, 0, ctx.indices.length);
    };

    IndicesDelegate.prototype.resourceAvailable = function(glResource, ctx) {
        var geometry = ctx.geometry;
        geometry.indexArray = glResource;
        geometry.checkFinished();
    };

    var indicesDelegate = new IndicesDelegate();

    var IndicesContext = function(indices, geometry) {
        this.indices = indices;
        this.geometry = geometry;
    };
    
    // Delegate for processing vertex attribute buffers
    var VertexAttributeDelegate = function() {};

    VertexAttributeDelegate.prototype.handleError = function(errorCode, info) {
        // FIXME: report error
        console.log("ERROR(VertexAttributeDelegate):"+errorCode+":"+info);
    };

    VertexAttributeDelegate.prototype.convert = function(resource, ctx) {
        return resource;
    };

    VertexAttributeDelegate.prototype.resourceAvailable = function(glResource, ctx) {
        var geometry = ctx.geometry;
        var attribute = ctx.attribute;
        var accessor = ctx.accessor;
        var floatArray;
        var i, l;

        if(attribute.semantic == "VERTEX") {
            // TODO: Should be easy to take strides into account here
            floatArray = new Float32Array(glResource, 0, accessor.count * accessor.elementsPerValue);
            for(i = 0, l = floatArray.length; i < l; i += 3) {
                geometry.vertices.push( new THREE.Vector3( floatArray[i], floatArray[i+1], floatArray[i+2] ) );
            }
        } else if(attribute.semantic == "NORMAL") {
            geometry.normals = [];
            floatArray = new Float32Array(glResource, 0, accessor.count * accessor.elementsPerValue);
            for(i = 0, l = floatArray.length; i < l; i += 3) {
                geometry.normals.push( new THREE.Vector3( floatArray[i], floatArray[i+1], floatArray[i+2] ) );
            }
        }
        geometry.loadedAttributes++;
        geometry.checkFinished();
    };

    var vertexAttributeDelegate = new VertexAttributeDelegate();

    var VertexAttributeContext = function(attribute, accessor, geometry) {
        this.attribute = attribute;
        this.accessor = accessor;
        this.geometry = geometry;
    };

    var ThreeColladaMesh = function() {
        this.primitives = [];
        this.loadedGeometry = 0;
        this.onCompleteCallbacks = [];
    };

    ThreeColladaMesh.prototype.addPrimitive = function(geometry, material) {
        var self = this;
        geometry.onload = function() {
            self.loadedGeometry++;
            self.checkComplete();
        };
        this.primitives.push({
            geometry: geometry,
            material: material,
            mesh: null
        });
    };

    ThreeColladaMesh.prototype.onComplete = function(callback) {
        this.onCompleteCallbacks.push(callback);
        this.checkComplete();
    };

    ThreeColladaMesh.prototype.checkComplete = function() {
        var self = this;
        if(this.onCompleteCallbacks.length && this.primitives.length == this.loadedGeometry) {
            this.onCompleteCallbacks.forEach(function(callback) {
                callback(self);
            });
            this.onCompleteCallbacks = [];
        }
    };

    ThreeColladaMesh.prototype.attachToNode = function(threeNode) {
        // Assumes that the geometry is complete
        this.primitives.forEach(function(primitive) {
            if(!primitive.mesh) {
                primitive.mesh = new THREE.Mesh(primitive.geometry, primitive.material);
            }
            threeNode.add(primitive.mesh);
        });
    };

    // Resource management

    var ThreeEntry = function(entryID, object, description) {
        this.entryID = entryID;
        this.object = object;
        this.description = description;
    };

    var ThreeResourceManager = function() {
        this._entries = {};
        this.binaryManager = Object.create(ResourceManager);
        this.binaryManager.init();
    };

    ThreeResourceManager.prototype.setEntry = function(entryID, object, description) {
        if (!entryID) {
            console.error("No EntryID provided, cannot store", description);
            return;
        }

        if (this._entries[entryID]) {
            console.warn("entry["+entryID+"] is being overwritten");
        }
    
        this._entries[entryID] = new ThreeEntry(entryID, object, description );
    };
    
    ThreeResourceManager.prototype.getEntry = function(entryID) {
        return this._entries[entryID];
    };

    ThreeResourceManager.prototype.clearEntries = function() {
        this._entries = {};
    };

    // Loader

    var ThreeWebGLTFLoader = Object.create(WebGLTFLoader, {

        load: {
            enumerable: true,
            value: function(userInfo, options) {
                this.threeResources = new ThreeResourceManager();
                WebGLTFLoader.load.call(this, userInfo, options);
            }
        },

        threeResources: {
            value: null
        },

        // Implement WebGLTFLoader handlers

        handleBuffer: {
            value: function(entryID, description, userInfo) {
                this.threeResources.setEntry(entryID, null, description);
                return true;
            }
        },

        handleShader: {
            value: function(entryID, description, userInfo) {
                // No shader handling at this time
                return true;
            }
        },

        handleTechnique: {
            value: function(entryID, description, userInfo) {
                // No technique handling at this time
                return true;
            }
        },

        handleMaterial: {
            value: function(entryID, description, userInfo) {
                var material = new THREE.MeshLambertMaterial({
                    color: RgbArraytoHex(description.inputs.diffuseColor)
                });

                this.threeResources.setEntry(entryID, material, description);

                // TODO: Need more robust material support, currently only accepts flat colors
                return true;
            }
        },

        handleLight: {
            value: function(entryID, description, userInfo) {
                // No light handling at this time
                return true;
            }
        },

        handleMesh: {
            value: function(entryID, description, userInfo) {
                var mesh = new ThreeColladaMesh();
                this.threeResources.setEntry(entryID, mesh, description);

                var primitivesDescription = description.primitives;
                if (!primitivesDescription) {
                    //FIXME: not implemented in delegate
                    console.log("MISSING_PRIMITIVES for mesh:"+ entryID);
                    return false;
                }

                for (var i = 0 ; i < primitivesDescription.length ; i++) {
                    var primitiveDescription = primitivesDescription[i];
                    
                    if (primitiveDescription.primitive === "TRIANGLES") {

                        var geometry = new ColladaJsonClassicGeometry();
                        var materialEntry = this.threeResources.getEntry(primitiveDescription.material);

                        mesh.addPrimitive(geometry, materialEntry.object);

                        var indicesID = entryID + "_indices"+"_"+i;
                        var indicesEntry = this.threeResources.getEntry(indicesID);
                        if (!indicesEntry) {
                            indices = primitiveDescription.indices;
                            indices.id = indicesID;
                            var bufferEntry = this.threeResources.getEntry(indices.buffer);
                            indices.buffer = bufferEntry;
                            this.threeResources.setEntry(indicesID, indices, indices);
                            indicesEntry = this.threeResources.getEntry(indicesID);
                        }
                        primitiveDescription.indices = indicesEntry.object;

                        var indicesContext = new IndicesContext(primitiveDescription.indices, geometry);
                        this.threeResources.binaryManager.getResource(primitiveDescription.indices, indicesDelegate, indicesContext);

                        // Load Vertex Attributes
                        primitiveDescription.vertexAttributes.forEach( function(vertexAttribute) {
                            geometry.totalAttributes++;

                            var accessor;
                            var accessorID = vertexAttribute.accessor;
                            var accessorEntry = this.threeResources.getEntry(accessorID);
                            if (!accessorEntry) {
                                //let's just use an anonymous object for the accessor
                                accessor = description[accessorID];
                                accessor.id = accessorID;
                                this.threeResources.setEntry(accessorID, accessor, accessor);
            
                                var bufferEntry = this.threeResources.getEntry(accessor.buffer);
                                accessor.buffer = bufferEntry;
                                accessorEntry = this.threeResources.getEntry(accessorID);
                                //this is a set ID, it has to stay a string
                            } else {
                                accessor = accessorEntry.object;
                            }

                            var attribContext = new VertexAttributeContext(vertexAttribute, accessor, geometry);
                            this.threeResources.binaryManager.getResource(accessorEntry.object, vertexAttributeDelegate, attribContext);
                        }, this);
                    }
                }
                return true;
            }
        },

        handleCamera: {
            value: function(entryID, description, userInfo) {
                // No camera handling at this time
                return true;
            }
        },

        buildNodeHirerachy: {
            value: function(nodeEntryId, parentThreeNode) {
                var nodeEntry = this.threeResources.getEntry(nodeEntryId);
                var threeNode = nodeEntry.object;
                parentThreeNode.add(threeNode);

                var children = nodeEntry.description.children;
                if (children) {
                    children.forEach( function(childID) {
                        this.buildNodeHirerachy(childID, threeNode);
                    }, this);
                }

                return threeNode;
            }
        },

        handleScene: {
            value: function(entryID, description, userInfo) {

                if (!description.node) {
                    console.log("ERROR: invalid file required node property is missing from scene");
                    return false;
                }

                this.buildNodeHirerachy(description.node, userInfo.rootObj);

                /*if (this.delegate) {
                    this.delegate.loadCompleted(scene);
                }*/

                return true;
            }
        },

        handleNode: {
            value: function(entryID, description, userInfo) {

                var threeNode = new THREE.Object3D();
                threeNode.name = description.name;

                this.threeResources.setEntry(entryID, threeNode, description);

                var m = description.matrix;
                if(m) {
                    threeNode.matrixAutoUpdate = false;
                    threeNode.applyMatrix(new THREE.Matrix4(
                        m[0],  m[4],  m[8],  m[12],
                        m[1],  m[5],  m[9],  m[13],
                        m[2],  m[6],  m[10], m[14],
                        m[3],  m[7],  m[11], m[15]
                    ));
                }

                // Iterate through all node meshes and attach the appropriate objects
                //FIXME: decision needs to be made between these 2 ways, probably meshes will be discarded.
                var meshEntry;
                if (description.mesh) {
                    meshEntry = this.threeResources.getEntry(description.mesh);
                    meshEntry.object.onComplete(function(mesh) {
                        mesh.attachToNode(threeNode);
                    });
                }

                if (description.meshes) {
                    description.meshes.forEach( function(meshID) {
                        meshEntry = this.threeResources.getEntry(meshID);
                        meshEntry.object.onComplete(function(mesh) {
                            mesh.attachToNode(threeNode);
                        });
                    }, this);
                }

                /*if (description.camera) {
                    var cameraEntry = this.threeResources.getEntry(description.camera);
                    node.cameras.push(cameraEntry.entry);
                }*/

                return true;
            }
        },

        _delegate: {
            value: null,
            writable: true
        },

        delegate: {
            enumerable: true,
            get: function() {
                return this._delegate;
            },
            set: function(value) {
                this._delegate = value;
            }
        }
    });


    // Loader

    var ColladaJsonContext = function(rootObj, callback) {
        this.rootObj = rootObj;
        this.callback = callback;
    };

    THREE.ColladaJsonLoader = function ( context, showStatus ) {
        THREE.Loader.call( this, showStatus );
    };

    THREE.ColladaJsonLoader.prototype = new THREE.Loader();
    THREE.ColladaJsonLoader.prototype.constructor = THREE.ColladaJsonLoader;

    THREE.ColladaJsonLoader.prototype.load = function( url, callback ) {
        var rootObj = new THREE.Object3D();

        var loader = Object.create(ThreeWebGLTFLoader);
            loader.initWithPath(url);
            //loader.delegate = colladaJsonDelegate;
            loader.load(new ColladaJsonContext(rootObj, callback) /* userInfo */, null /* options */);

        return rootObj;
    };

    return {
        ColladaJsonLoader: THREE.ColladaJsonLoader
    };
});

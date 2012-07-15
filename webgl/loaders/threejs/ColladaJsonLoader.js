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

define( ["backend/utilities", "backend/node", "backend/camera", "backend/view", "backend/glsl-program", "backend/reader", "backend/resource-manager", "dependencies/gl-matrix"],
    function(Utilities, Node, Camera, View, GLSLProgram, Reader, ResourceManager) {

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
        var accessor = attribute.accessor;
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

    var VertexAttributeContext = function(attribute, geometry) {
        this.attribute = attribute;
        this.geometry = geometry;
    };

    // Utilities

    function RgbArraytoHex(colorArray) {
        var r = Math.floor(colorArray[0] * 255),
            g = Math.floor(colorArray[1] * 255),
            b = Math.floor(colorArray[2] * 255),
            a = Math.floor(colorArray[3] ? colorArray[3] * 255 : 255);

        var color = (a << 24) + (r << 16) + (g << 8) + b;

        return color;
    }

    // Loader

    var ColladaJsonDelegate = function() {
        this.resourceManager = Object.create(ResourceManager);
        this.resourceManager.init();
    };

    ColladaJsonDelegate.prototype.readCompleted = function(key, value, ctx) {
        if (key === "entries") {
            this.processNodes(value[0], ctx.rootObj, ctx.callback);
        }
    };

    ColladaJsonDelegate.prototype.processNodes = function(scene, rootObj, callback) {
        var self = this;

        var totalMeshes = 0;
        var loadedMeshes = 0;

        rootObj.treeMatrix = new THREE.Matrix4();

        scene.rootNode.apply( function(node, parentObj) {
            var obj = new THREE.Object3D();
            obj.name = node.id || "";

            var nodeMatrix;
            var t = node.transform;
            if(t) {
                // Not sure why I need to transpose this...
                nodeMatrix = new THREE.Matrix4(
                    t[0],  t[4],  t[8],  t[12],
                    t[1],  t[5],  t[9],  t[13],
                    t[2],  t[6],  t[10], t[14],
                    t[3],  t[7],  t[11], t[15]
                );

                obj.matrixAutoUpdate = false;
                obj.applyMatrix(nodeMatrix);
            }

            parentObj.add(obj);

            self.processNodeMeshes(node, obj, callback);

            return obj;
        }, true, rootObj);

        return;
    };

    ColladaJsonDelegate.prototype.processNodeMeshes = function(node, obj, callback) {
        var self = this;

        if (node.meshes) {
            node.meshes.forEach( function(mesh) {
                if (mesh.primitives) {
                    mesh.primitives.forEach( function (primitive) {
                        var material = new THREE.MeshLambertMaterial({
                            color: RgbArraytoHex(primitive.material.inputs.diffuseColor)
                        });
                        //totalMeshes++;

                        var geometry = new ColladaJsonClassicGeometry();
                        geometry.onload = function() {
                            //geometry.applyMatrix(obj.treeMatrix);
                            var mesh = new THREE.Mesh(geometry, material);
                            obj.add(mesh);
                            /*loadedMeshes++;
                            if(loadedMeshes === totalMeshes) {
                                if(callback) {
                                    callback(rootObj);
                                }
                            }*/
                        };

                        self.processMeshPrimitive(primitive, geometry);
                        
                    }, this);
                }
            }, this);
        }
    };

    ColladaJsonDelegate.prototype.processMeshPrimitive = function(primitive, geometry) {
        // Load Indices
        var range = [primitive.indices.byteOffset, primitive.indices.byteOffset + ( primitive.indices.length * Uint16Array.BYTES_PER_ELEMENT)];
        var indicesContext = new IndicesContext(primitive.indices, geometry);
        this.resourceManager.getResource(primitive.indices, indicesDelegate, indicesContext);

        // Load Vertex Attributes
        primitive.vertexAttributes.forEach( function(vertexAttribute) {
            var accessor = vertexAttribute.accessor;
            geometry.totalAttributes++;
            var range = [accessor.byteOffset ? accessor.byteOffset : 0 , (accessor.byteStride * accessor.count) + accessor.byteOffset];
            var attribContext = new VertexAttributeContext(vertexAttribute, geometry);
            this.resourceManager.getResource(accessor, vertexAttributeDelegate, attribContext);
        }, this);
    };

    var colladaJsonDelegate = new ColladaJsonDelegate();

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

        var reader = Object.create(Reader);
        reader.initWithPath(url);
        reader.readEntries(["defaultScene"], colladaJsonDelegate, new ColladaJsonContext(rootObj, callback));

        return rootObj;
    };

    return {
        ColladaJsonLoader: THREE.ColladaJsonLoader
    };
});

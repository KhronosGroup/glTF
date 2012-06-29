/**
 * Loader for Collada Json models
 *
 * @author toji / http://tojicode.com/
 */

define( ["backend/utilities", "backend/node", "backend/camera", "backend/view", "backend/glsl-program", "backend/reader", "backend/resource-manager", "dependencies/gl-matrix"],
    function(Utilities, Node, Camera, View, GLSLProgram, Reader, ResourceManager) {

    var ColladaJsonClassicGeometry = function(gl) {
        THREE.Geometry.call( this );

        var self = this;
        this.totalAttributes = 0;
        this.loadedAttributes = 0;
        this.indicesLoaded = false;
        this.finished = false;

        this.onload = null;

        this.normals = null;
        this.indexArray = null;

        this.indicesDelegate = {
            handleError: function(errorCode, info) {
                // FIXME: report error
                console.log("ERROR:vertexAttributeBufferDelegate:"+errorCode+" :"+info);
            },

            //should be called only once
            convert: function (resource, ctx) {
                return new Uint16Array(resource, 0, ctx.length);
            },
    
            resourceAvailable: function (indexArray, ctx) {
                self.indexArray = indexArray;
                self.checkFinished();
            }
        };
    
        this.vertexAttributeBufferDelegate = {
            handleError: function(errorCode, info) {
                // FIXME: report error
                console.log("ERROR:vertexAttributeBufferDelegate:"+errorCode+" :"+info);
            },

            //should be called only once
            convert: function (resource, ctx) {
                return resource;
            },
    
            resourceAvailable: function (resource, ctx) {
                var accessor = ctx.accessor;
                var floatArray;
                var i, l;

                if(ctx.semantic == "VERTEX") {
                    // TODO: Should be easy to take strides into account here
                    floatArray = new Float32Array(resource, 0, accessor.count * accessor.elementsPerValue);
                    for(i = 0, l = floatArray.length; i < l; i += 3) {
                        self.vertices.push( new THREE.Vector3( floatArray[i], floatArray[i+1], floatArray[i+2] ) );
                    }
                } else if(ctx.semantic == "NORMAL") {
                    self.normals = [];
                    floatArray = new Float32Array(resource, 0, accessor.count * accessor.elementsPerValue);
                    for(i = 0, l = floatArray.length; i < l; i += 3) {
                        self.normals.push( new THREE.Vector3( floatArray[i], floatArray[i+1], floatArray[i+2] ) );
                    }
                }
                self.loadedAttributes++;
                self.checkFinished();
            }
        };

        this.checkFinished = function() {
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
    };

    ColladaJsonClassicGeometry.prototype = new THREE.Geometry();
    ColladaJsonClassicGeometry.prototype.constructor = ColladaJsonClassicGeometry;

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

    THREE.ColladaJsonLoader = function ( context, showStatus ) {
        this.context = context;
        this.resourceManager = Object.create(ResourceManager);
        this.resourceManager.init();

        THREE.Loader.call( this, showStatus );
    };

    THREE.ColladaJsonLoader.prototype = new THREE.Loader();
    THREE.ColladaJsonLoader.prototype.constructor = THREE.ColladaJsonLoader;

    THREE.ColladaJsonLoader.prototype.load = function( url, callback ) {

        var self = this;

        var rootObj = new THREE.Object3D();

        var readerDelegate = {
            readCompleted: function (key, value, userInfo) {
                console.log("key:"+key+" value:"+value.length);
                if (key === "entries") {
                    var scene = value[0];
                    self.processNodes(rootObj, scene, callback);
                    //scene.rootNode.children.push(cameraNode);
                    //glView.scene = scene;
                }
            }
        };

        var reader = Object.create(Reader);
        reader.initWithPath(url);
        reader.readEntries(["defaultScene"], readerDelegate, null); // TODO: How do we know what to put here? Can we request "all" or "*"?

        return rootObj;
    };

    // Load multiple CTM parts defined in JSON

    THREE.ColladaJsonLoader.prototype.processNodes = function(rootObj, scene, callback ) {
        var self = this;
        
        var materialProps = {
            color: 0xFF00EEFF
        };
        var defaultMaterial = new THREE.MeshLambertMaterial( materialProps );

        var totalMeshes = 0;
        var loadedMeshes = 0;

        scene.rootNode.apply( function(node, parentObj) {
            var obj = new THREE.Object3D();
            obj.up = new THREE.Vector3( 0, 0, 1 );

            var nodeMatrix;
            var t = node.transform;
            if(t) {
                // Normal
                /*nodeMatrix = new THREE.Matrix4(
                    t[0],  t[1],  t[2],  t[3],
                    t[4],  t[5],  t[6],  t[7],
                    t[8],  t[9],  t[10], t[11],
                    t[12], t[13], t[14], t[15]
                );*/
                
                // Transposed
                nodeMatrix = new THREE.Matrix4(
                    t[0],  t[4],  t[8],  t[12],
                    t[1],  t[5],  t[9],  t[13],
                    t[2],  t[6],  t[10], t[14],
                    t[3],  t[7],  t[11], t[15]
                );

                //obj.matrix = nodeMatrix;
                //obj.updateWorldMatrix();
                //obj.applyMatrix(nodeMatrix);

                // Ugh...
                obj.name = node.id || "";
                obj.useQuaternion = true;
                obj.matrix = nodeMatrix;
                var props = nodeMatrix.decompose();

                obj.position = props[ 0 ];
                obj.quaternion = props[ 1 ];
                obj.scale = props[ 2 ];
            }

            parentObj.add(obj);

            if (node.meshes) {
                node.meshes.forEach( function(mesh) {
            
                    if (mesh.primitives) {
                        mesh.primitives.forEach( function (primitive) {
                            var material = new THREE.MeshLambertMaterial({
                                color: RgbArraytoHex(primitive.material.color)
                            });
                            totalMeshes++;

                            var geometry = new ColladaJsonClassicGeometry(self.context);
                            geometry.onload = function() {
                                var mesh = new THREE.Mesh(geometry, material);
                                obj.add(mesh);
                                loadedMeshes++;
                                if(loadedMeshes === totalMeshes) {
                                    if(callback) {
                                        callback(rootObj);
                                    }
                                }
                            };

                            // Load Indices
                            var range = [primitive.indices.byteOffset, primitive.indices.byteOffset + ( primitive.indices.length * Uint16Array.BYTES_PER_ELEMENT)];
                            glIndices = self.resourceManager.getWebGLResource(primitive.indices.id, primitive.indices.buffer, range, geometry.indicesDelegate, primitive.indices);

                            // Load Attributes
                            primitive.vertexAttributes.forEach( function(vertexAttribute) {
                                var accessor = vertexAttribute.accessor;
                                geometry.totalAttributes++;
                                //FIXME: do not assume continuous data, this will break with interleaved arrays (should not use byteStride here).
                                var range = [accessor.byteOffset ? accessor.byteOffset : 0 , (accessor.byteStride * accessor.count) + accessor.byteOffset];
                                var glResource = self.resourceManager.getWebGLResource(accessor.id, accessor.buffer, range, geometry.vertexAttributeBufferDelegate, vertexAttribute);
                                // this call will bind the resource when available
                                if (glResource) {
                                    gl.bindBuffer(gl.ARRAY_BUFFER, glResource);
                                } else {
                                     available = false;
                                }
                            }, this);

                            // TODO: Three.js doesn't handle interleaved arrays or odd element sizes. Need to normalize that somewhere.
                            // TODO: What happens with the geometry at this point?
                            // TODO: Need a way to identify when the mesh is completely loaded
                        }, this);
                    }
                }, this);
            }
                    
            return obj;
        }, true, rootObj);

        return;
    };

    return {
        ColladaJsonLoader: THREE.ColladaJsonLoader
    };
});

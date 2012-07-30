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

require("dependencies/gl-matrix");
var WebGLTFLoader = require("c2j/webgl-tf-loader").WebGLTFLoader;
var ResourceDescription = require("resource-description").ResourceDescription;
var Technique = require("technique").Technique;
var Pass = require("pass").Pass;
var GLSLProgram = require("glsl-program").GLSLProgram;
var Material = require("material").Material;
var Mesh = require("mesh").Mesh;
var Node = require("node").Node;
var Primitive = require("primitive").Primitive;
var Projection = require("projection").Projection;
var Camera = require("camera").Camera;
var Scene = require("scene").Scene;

exports.RuntimeTFLoader = Object.create(WebGLTFLoader, {

    _scenes: { writable:true, value: null },

    //----- implements WebGLTFLoader ----------------------------

   handleBuffer: {
        value: function(entryID, description, userInfo) {
            var buffer = Object.create(ResourceDescription).init(entryID, description);
            buffer.id = entryID;
            this.storeEntry(entryID, buffer, description);
            return true;
        }
    },

    handleShader: {
        value: function(entryID, description, userInfo) {
            var shader = Object.create(ResourceDescription).init(entryID, description);
            shader.id = entryID;
            shader.type = "shader"; //FIXME should I pass directly the description ? add a type property in resource-description ? (probably first solution...)
            this.storeEntry(entryID, shader, description);
            return true;
        }
    },

    handleTechnique: {
        value: function(entryID, description, userInfo) {

            var technique = Object.create(Technique);
            technique.id = entryID;
            this.storeEntry(entryID, technique, description);

            technique.rootPass = Object.create(Pass).init();
            technique.rootPass.id = entryID+"_"+rootPassID;
            var rootPassID = description.pass;
            var passDescription = description[rootPassID];
            var vsShaderEntry = this.getEntry(passDescription[GLSLProgram.VERTEX_SHADER]);
            var fsShaderEntry = this.getEntry(passDescription[GLSLProgram.FRAGMENT_SHADER]);

            var programs = {};
            programs[GLSLProgram.VERTEX_SHADER] = vsShaderEntry.entry;
            programs[GLSLProgram.FRAGMENT_SHADER] = fsShaderEntry.entry;
            technique.rootPass.program = Object.create(ResourceDescription).init(entryID+"_"+rootPassID+"_program", programs);
            technique.rootPass.program.type = "program"; //add this here this program object is not defined in the JSON format, we need to set the type manually.
            technique.rootPass.states = passDescription.states;
            return true;
        }
    },

    handleMaterial: {
        value: function(entryID, description, userInfo) {
            var material = Object.create(Material).init(entryID);
            this.storeEntry(entryID, material, description);
            if (description.inputs.diffuseTexture) {
                description.inputs.diffuseTexture = this.resolvePathIfNeeded(description.inputs.diffuseTexture)                
            }

            material.inputs = description.inputs;
            material.name = description.name;
            var techniqueEntry = this.getEntry(description.technique);
            if (techniqueEntry) {
                material.technique = techniqueEntry.entry;
                return true;
            } else {
                console.log("ERROR: invalid file, cannot find referenced technique:"+description.technique);
                return false;
            }
        }
    },

    handleLight: {
        value: function(entryID, description, userInfo) {
            //no lights yet.
            return true;
        }
    },

    handleMesh: {
        value: function(entryID, description, userInfo) {

            var mesh = Object.create(Mesh).init();
            mesh.id = entryID;
            mesh.name = description.name;

            this.storeEntry(entryID, mesh, description);

            var primitivesDescription = description[Mesh.PRIMITIVES];
            if (!primitivesDescription) {
                //FIXME: not implemented in delegate
                console.log("MISSING_PRIMITIVES for mesh:"+ entryID);
                return false;
            }

            for (var i = 0 ; i < primitivesDescription.length ; i++) {
                var primitiveDescription = primitivesDescription[i];

                if (primitiveDescription.primitive === "TRIANGLES") {
                    var primitive = Object.create(Primitive).init();

                    //read material
                    var materialEntry = this.getEntry(primitiveDescription.material);
                    primitive.material = materialEntry.entry;

                    primitive.mesh = mesh; //FIXME: demo crap to be removed.
                    mesh.primitives.push(primitive);

                    var vertexAttributesDescription = primitiveDescription.vertexAttributes;

                    vertexAttributesDescription.forEach( function(vertexAttributeDescription) {
                        var accessorID = vertexAttributeDescription.accessor;
                        var accessorEntry = this.getEntry(accessorID);
                        if (!accessorEntry) {
                            //let's just use an anonymous object for the accessor
                            var accessor = description[accessorID];
                            accessor.id = accessorID;
                            this.storeEntry(accessorID, accessor, accessor);

                            var bufferEntry = this.getEntry(accessor.buffer);
                            accessor.buffer = bufferEntry.entry;
                            accessorEntry = this.getEntry(accessorID);
                            //this is a set ID, it has to stay a string
                        }
                        var set = vertexAttributeDescription.set ? vertexAttributeDescription.set : "0";

                        primitive.addVertexAttribute( { "semantic" :  vertexAttributeDescription.semantic,
                                                        "set" : set,
                                                        "accessor" : accessorEntry.entry });

                    }, this);

                    //set indices
                    var indicesID = entryID + "_indices"+"_"+i;
                    var indicesEntry = this.getEntry(indicesID);
                    if (!indicesEntry) {
                        indices = primitiveDescription.indices;
                        indices.id = indicesID;
                        var bufferEntry = this.getEntry(indices.buffer);
                        indices.buffer = bufferEntry.entry;
                        this.storeEntry(indicesID, indices, indices);
                        indicesEntry = this.getEntry(indicesID);
                    }
                    primitive.indices = indicesEntry.entry;
                }
            }
            return true;
        }
    },

    handleCamera: {
        value: function(entryID, description, userInfo) {

            var camera = Object.create(Camera).init();
            camera.id = entryID;
            this.storeEntry(entryID, camera, description);

            var projection = Object.create(Projection);
            projection.initWithDescription(description);
            camera.projection = projection;
            return true;
        }
    },

    handleLight: {
        value: function(entryID, description, userInfo) {
            return true;
        }
    },

    buildNodeHirerachy: {
        value: function(parentEntry) {
            var parentNode = parentEntry.entry;
            var children = parentEntry.description.children;
            if (children) {
                children.forEach( function(childID) {
                    var nodeEntry = this.getEntry(childID);
                    parentNode.children.push(nodeEntry.entry);
                    this.buildNodeHirerachy(nodeEntry);
                }, this);
            }
        }
    },

    handleScene: {
        value: function(entryID, description, userInfo) {

            if (!this._scenes) {
                this._scenes = [];
            }

            if (!description.node) {
                console.log("ERROR: invalid file required node property is missing from scene");
                return false;
            }

            var scene = Object.create(Scene).init();
            scene.id = entryID;
            scene.name = description.name;
            this.storeEntry(entryID, scene, description);

            var nodeEntry = this.getEntry(description.node);

            scene.rootNode = nodeEntry.entry;
            this._scenes.push(scene);
            //now build the hirerarchy
            this.buildNodeHirerachy(nodeEntry);

            return true;
        }
    },

    handleNode: {
        value: function(entryID, description, userInfo) {
            var childIndex = 0;
            var self = this;

            var node = Object.create(Node).init();
            node.id = entryID;
            node.name = description.name;

            this.storeEntry(entryID, node, description);

            node.transform = description.matrix ? mat4.create(description.matrix) : mat4.identity();

            //FIXME: decision needs to be made between these 2 ways, probably meshes will be discarded.
            var meshEntry;
            if (description.mesh) {
                meshEntry = this.getEntry(description.mesh);
                node.meshes.push(meshEntry.entry);
            }

            if (description.meshes) {
                description.meshes.forEach( function(meshID) {
                    meshEntry = this.getEntry(meshID);
                    node.meshes.push(meshEntry.entry);
                }, this);
            }

            if (description.camera) {
                var cameraEntry = this.getEntry(description.camera);
                node.cameras.push(cameraEntry.entry);
            }

            return true;
        }
    },

    handleLoadCompleted: {
        value: function(success) {
            if (this._scenes && this.delegate) {
                if (this._scenes.length > 0) {
                    this.delegate.loadCompleted(this._scenes[0]);
                }
            }
        }
    },

    handleError: {
        value: function(reason) {
            //TODO: propagate in the delegate
        }
    },

    //----- store model values

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
    },

    _entries: {
        enumerable: false,
        value: null,
        writable: true
    },

    removeAllEntries: {
        value: function() {
            this._entries = {};
        }
    },

    containsEntry: {
        enumerable: false,
        value: function(entryID) {
            if (!this._entries)
                return false;
            return this._entries[entryID] ? true : false;
        }
    },

    storeEntry: {
        enumerable: false,
        value: function(id, entry, description) {
            if (!this._entries) {
                this._entries = {};
            }

            if (!id) {
                console.log("ERROR: not id provided, cannot store");
                return;
            }

            if (this.containsEntry[id]) {
                console.log("WARNING: entry:"+id+" is already stored, overriding");
            }

            this._entries[id] = { "id" : id, "entry" : entry, "description" : description };
        }
    },

    getEntry: {
        enumerable: false,
        value: function(entryID) {
            return this._entries ? this._entries[entryID] : null;
        }
    }


});

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

define(["dependencies/gl-matrix"], function() {

    var Primitive = Object.create(Object, {
        
        /* step is temporary demo stuff */
       _step: {
            value: 0,
            writable: true
        },
    
        step: {
            enumerable: true,
            get: function() {
                return this._step;
            },
            set: function(value) {
                this._step = value;
            }
        },

        init: {
            value: function() {
                this.step = 0;
                this.vertexAttributes = null;
            }
        },

        _vertexAttributes: {
            enumerable: false,
            value: null,
            writable: true
        },
    
        vertexAttributes: {
            enumerable: true,
            get: function() {
                if (!this._vertexAttributes)
                    this._vertexAttributes = [];
                return this._vertexAttributes;
            },
           set: function(value) {
                this._vertexAttributes = value;
            }
        },
    
        addVertexAttribute: {
            enumerable: false,
            value: function(vertexAttribute) {
                if (vertexAttribute.semantic === "VERTEX") {
                    var bbox = null;
                    var accessor = vertexAttribute.accessor;
                    if (accessor.min && accessor.max) {
                        bbox = [ accessor.min, accessor.max];
                    }
                    this.boundingBox = bbox;
                }
                this.vertexAttributes.push(vertexAttribute);
            }
        },

        getAccessorAssociatedWithSemanticAndSet: {
            enumerable: false,
            value: function(semantic, set) {
                // not efficient but typically not called at runtime
                for (var i = 0 ; i < this.vertexAttributes.length ; i++) {
                    var vertexAttribute = this.vertexAttributes[i];
                    if ((vertexAttribute.semantic === semantic) && (vertexAttribute.set === set)) {
                        return vertexAttribute.accessor;
                    }
                }
            }
        },
    
        _computeBBOXIfNeeded: {
            enumerable: false,
            value: function() {
                if (!this._boundingBox) {
                    //FIXME: TODO, this needs to rework a bit the resource manager to not only provide WebGL representation which is arguable for a very simple viewer...
                    /*
                    var verticesAccessor = this.getAccessorAssociatedWithSemanticAndSet("VERTEX", "0");
                    if (verticesAccessor) {
                    if (verticesAccessor.elementType === "Float32") {
                        var verticesArray = new DataView(verticesAccessor.buffer);
                        
                        var elementSize = Float32Array.BYTES_PER_ELEMENT;
                        var bboxMin = [verticesArray.getFloat32(elementSize * 0), verticesArray.getFloat32(elementSize * 1), verticesArray.getFloat32(elementSize * 2)];
                        var bboxMax = [verticesArray.getFloat32(elementSize * 0), verticesArray.getFloat32(elementSize * 1), verticesArray.getFloat32(elementSize * 2)];
                        
                        for (var i = 0 ; i < verticesAccessor.count ; i++) {
                            
                        }
                        
                        this._boundingBox = [bboxMin, bboxMax];
                        }
                    }
                    */
                }
            }
        },
        
        _boundingBox: {
            value: null,
            writable: true
        },
    
        boundingBox: {
            enumerable: true,
            get: function() {
                this._computeBBOXIfNeeded();
                return this._boundingBox;
            },
            // we let the possibility to override by hand the bounding volume.
            set: function(value) {
                this._boundingBox = value;
            }
        },
    
        _indices: { enumerable: false, value: null, writable: true },
    
        indices: {
            get: function() {
                return this._indices;
            },
            set: function(value) {
                this._indices = value;
            }
        },

        _material: { enumerable: false, value: null, writable: true },
    
        material: {
            get: function() {
                return this._material;
            },
            set: function(value) {
                this._material = value;
            }
        }

    });
    
        return Primitive;
    }
);

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
var Base = require("base").Base;
var Utilities = require("utilities").Utilities;

exports.Mesh = Object.create(Base, {

    PRIMITIVES: { value: "primitives" },

    //FIXME:temporary demo crap
    //client side vertices arraybuffer
    _verticesBuffer: {
        value: null,
        writable: true
    },
    verticesBuffer: { 
        enumerable: true,
        get: function() {
            return this._verticesBuffer;
        },
        set: function(value) {
            this._verticesBuffer = value;
        }
    },

    _hidden: { value: null, writable: true },

    hidden: {
        get: function() {
            return this._hidden;
        },
        set: function(value) {
            this._hidden = value;
        }
    },

    _primitives: {
        value: null,
        writable: true
    },

    primitives: {
        enumerable: true,
        get: function() {
            if (!this._primitives)
                this._primitives = [];  //FIXME: make an init
            return this._primitives;
        },
        set: function(value) {
            this._primitives = value;
        }
    },

    _id: {
        value: null,
        writable: true
    },

    id: {
        enumerable: true,
        get: function() {
            return this._id;
        },
        set: function(value) {
            this._id = value;
        }
    },

    _name: {
        value: null,
        writable: true
    },

    name: {
        enumerable: true,
        get: function() {
            return this._name;
        },
        set: function(value) {
            this._name = value;
        }
    },

    _computeBBOXIfNeeded: {
        enumerable: false,
        value: function() {
            if ( (!this._boundingBox) && this.primitives) {

                var count = this.primitives.length;

                if (count > 0) {
                    var bbox = this.primitives[0].boundingBox;
                    if (bbox) {
                        var i;
                        for (i = 1 ; i <  count ; i++) {
                            if (this.primitives[i].boundingBox) { //it could be not here here as we are loading everything asynchronously
                                bbox = Utilities.mergeBBox(bbox, this.primitives[i].boundingBox);
                            }
                        }
                        this._boundingBox = bbox;
                    }
                }
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

    init: {
        value: function() {
            this.__Base_init();
            return this;
        }
    }

});
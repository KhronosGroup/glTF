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

var global = window;
(function (root, factory) {
    if (typeof exports === 'object') {
        // Node. Does not work with strict CommonJS, but
        // only CommonJS-like enviroments that support module.exports,
        // like Node.
      
        module.exports = factory(global);
        module.exports.Projection = module.exports;
    } else if (typeof define === 'function' && define.amd) {
        // AMD. Register as an anonymous module.
        define([], function () {
            return factory(root);
        });
    } else {
        // Browser globals
        factory(root);
    }
}(this, function (root) {
    var Base;
    if (typeof exports === 'object') {
        require("dependencies/gl-matrix");
        Base = require("base").Base;
    } else {
        Base = global.Base;
    }

    var Projection = Object.create(Base, {

        _matrix: { value: null, writable: true },
        _type: { value: null, writable: true },
        _xfov: { value: 0, writable: true },
        _yfov: { value: 0, writable: true },
        _xmag: { value: 0, writable: true },
        _ymag: { value: 0, writable: true },
        _znear: { value: 0, writable: true },
        _zfar: { value: 0, writable: true },
        _aspectRatio: { value: 0, writable: true },

        _dirtyFlag: { value : false, writable: true},

        projection: {
            get: function() {
                return this._type;
            },
            set: function(value) {
                if (this._type !== value) {
                    this._type = value;
                    this._dirtyFlag = true;
                }
            }
        },

        xfov: {
            get: function() {
                return this._xfov;
            },
            set: function(value) {
                if (this._xfov !== value) {
                    this._xfov = value;
                    this._dirtyFlag = true;
                }
            }
        },

        yfov: {
            get: function() {
                return this._yfov;
            },
            set: function(value) {
                if (this._yfov !== value) {
                    this._yfov = value;
                    this._dirtyFlag = true;
                }
            }
        },

        xmag: {
            get: function() {
                return this._xmag;
            },
            set: function(value) {
                if (this._xmag !== value) {
                    this._xmag = value;
                    this._dirtyFlag = true;
                }
            }
        },

        ymag: {
            get: function() {
                return this._ymag;
            },
            set: function(value) {
                if (this._ymag !== value) {
                    this._ymag = value;
                    this._dirtyFlag = true;
                }
            }
        },

        znear: {
            get: function() {
                return this._znear;
            },
            set: function(value) {
                if (this._znear !== value) {
                    this._znear = value;
                    this._dirtyFlag = true;
                }
            }
        },

        zfar: {
            get: function() {
                return this._zfar;
            },
            set: function(value) {
                if (this._zfar !== value) {
                    this._zfar = value;
                    this._dirtyFlag = true;
                }
            }
        },

        aspectRatio: {
            get: function() {
                return this._aspectRatio;
            },
            set: function(value) {
                if (this._aspectRatio !== value) {
                    this._aspectRatio = value;
                    this._dirtyFlag = true;
                }
            }
        },

        matrix: {
            get: function() {
                if (this._dirtyFlag) {
                    if (this.projection === "perspective") {
                        var yfov = this.yfov;
                        if (yfov === 0) {
                            yfov = this.aspectRatio * this.xfov;
                        }

                        if (this.aspectRatio !== 0.) {
                            aspectRatio = this.aspectRatio;
                        } else  if ((this.xfov !== 0.) && ( this.yfov !== 0.)) {
                            aspectRatio = this.xfov/this.yfov;
                        }

                        this._matrix = mat4.perspective(yfov, aspectRatio, this.znear, this.zfar);
                    } else if (type == "orthographic") {
                        //TODO:
                    } else {
                        console.log("WARNING: unhandled camera type:"+type)
                    }

                    this._dirtyFlag = false;
                }
                return this._matrix;
            },
            set: function(value ) {
                this._matrix = value;
            }
        },

        initWithDescription: {
            value: function(description) {
                this.__Base_init();
                this.projection = description.projection ? description.projection : null;
                this.xfov = description.xfov ? description.xfov : 0;
                this.yfov = description.yfov ? description.yfov : 0;
                this.xmag = description.xmag ? description.xmag : 0;
                this.ymag = description.ymag ? description.ymag : 0;
                this.znear = description.znear ? description.znear : 1;
                this.zfar = description.zfar ? description.zfar : 100;
                this.aspectRatio = description.aspect_ratio ? description.aspect_ratio : 0; //deprecate this one
                if (!this.aspectRatio)
                    this.aspectRatio = description.aspectRatio ? description.aspectRatio : 0;
                this._dirtyFlag = true;
            }
        },

        init: {
            value: function(description) {
                this.__Base_init();

                this.projection = null;
                this.xfov = 0;
                this.yfov = 0;
                this.xmag = 0;
                this.ymag = 0;
                this.znear = 1;
                this.zfar = 100;
                this.aspectRatio = 4./3;
                this._dirtyFlag = true;
            }
        },

    });

    if(root) {
        root.Projection = Projection;
    }

    return Projection;

}));
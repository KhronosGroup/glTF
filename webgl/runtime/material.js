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
        module.exports.Material = module.exports;
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
    var Base , Technique, Pass , GLSLProgram , ResourceDescription;
    if (typeof exports === 'object') {
        require("dependencies/gl-matrix");
        Base = require("base").Base;
        Technique = require("technique").Technique;
        Pass = require("pass").Pass;
        GLSLProgram = require("glsl-program").GLSLProgram;
        ResourceDescription = require("resource-description").ResourceDescription;

    } else {
        Base = global.Base;
        Technique=global.Technique;
        Pass=global.Pass;
        GLSLProgram =global.GLSLProgram;
        ResourceDescription=global.ResourceDescription;
    }


        var Material = Object.create(Base, {

        /*
        animationDuration: { value: 0.7, writable: false},
        animationDelegate: { value: null, writable: true},
        _techniqueForID: { value: {}, writable: true },
        inputWillChange: {
            value: function(name, value) {
            }
        },

        //update the technique when change parameters
        //the combinatory possibilties could explode here, and should not be handled that way for future devs..
        inputDidChange: {
            value: function(name) {

            }
        },

        _addInputPropertyIfNeeded: {
            value: function(property) {
                var privateName = "_" + property;
                var self = this;
                if (this.inputs.hasOwnProperty(property) === false) {
                    Object.defineProperty(this.inputs, privateName, { writable:true , value: null });

                    Object.defineProperty(this.inputs, property, {
                        get: function() { return self.inputs[privateName]; },
                        set: function(value) {
                            var currentValue = self.inputs[property];
                            self.inputWillChange.call(self, property, value);
                            if ((property === "diffuseTexture")||(property === "reflectionTexture")) {
                                if (typeof value === "string") {
                                    var texture = value;
                                    var imageResource = Object.create(ResourceDescription).init(texture, { path: texture });
                                    imageResource.type = "image";
                                    value = imageResource;
                                }  else if (value !== null) {
                                    var image = value;
                                    //check if the uuid that we potentially stored is here
                                    var uuid = image["__uuid"];
                                    if (!uuid) {
                                        image["__uuid"] = image.src ? src.split("/").join("_") : Uuid.generate();
                                    }

                                    var imageResource = Object.create(ResourceDescription).init(uuid, { "image": image });
                                    imageResource.type = "image";
                                    value = imageResource;
                                }
                            }  
                            self.inputs[privateName] = value;
                            self.inputDidChange.call(self, property);
                        }
                    });
                }
            }
        },

        _prepareInputsProperties: {
            value: function() {
                var properties = ["diffuseColor", "diffuseTexture", "transparency", "reflectionTexture", "reflectionIntensity", "shininess", "specularColor"];
                properties.forEach( function(property) {
                    this._addInputPropertyIfNeeded(property);
                }, this);
            }
        },
        */

        techniqueDidChange: {
            value: function() {
                //first thing when a technique is changed check for symbols.
                //these symbols will provides the names of the parameters to be tracked.
                
            }
        },

        _technique: { value: null, writable: true },

        technique: {
            enumerable: false,
            get: function() {
                return this._technique;
            },
            set: function(value) {
                if (this._technique !== value) {
                   this._technique = value;
                   this.techniqueDidChange();
                }
            }
        },

        _parameters: { value: null, writable: true },

        parameters: {
            enumerable: false,
            get: function() {
                return this._parameters;
            },
            set: function(value) {
                this._parameters = value;
            }
        },

        init: {
            value: function(id) {
                this.id = id;
                this.__Base_init();
                this._parameters = {};
                return this;
            }
        },

    });

    if(root) {
        root.Material = Material;
    }

    return Material;

}));
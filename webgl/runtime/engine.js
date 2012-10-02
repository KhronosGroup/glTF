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
var Renderer = require("renderer").Renderer;
var Technique = require("technique").Technique;
var ScenePass = require("pass").ScenePass;

exports.Engine = Object.create(Object.prototype, {

    createTechniqueIfNeeded: {
        value: function() {
            if (!this._technique) {
                this._technique = Object.create(Technique).init();
                var pass = Object.create(ScenePass);
                //there is just one pass, so passName will be automatically set to "defaultPass"
                this._technique.passes = { "defaultPass": pass };
            }
        }
    },

    _renderer: { value: null, writable: true },
    
    _technique: { value: null, writable: true },

    technique: {
        get: function() {
            this.createTechniqueIfNeeded();
            return this._technique;
        },
        set: function(value) {
            this.createTechniqueIfNeeded();
            this._technique = value;
        }
    },

    scene: {
        get: function() {
            return this._scene;
        },
        set: function(value) {
            var scene = this.technique.rootPass.scene;
            if (scene != value) {
                scene = value;
            }
        }
    },

    renderer: {
        get: function() {
            return this._renderer;
        },
        set: function(value) {
            this._renderer = value;
        }
    },

    init: {
        value: function( webGLContext, options) {
            this.renderer = Object.create(Renderer).initWithWebGLContext(webGLContext);
            return this;
        }
    },

    render: {
        value: function() {
            if (this.technique) {
                this.technique.execute(this.renderer);
            } else {
                console.log("WARNING render invoke in engine but no technique");
            }   
        }
    }

});

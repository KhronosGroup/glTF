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

define(["backend/renderer","backend/pass", "backend/command-queue"], function(Renderer, Pass, CommandQueue) {

    var Engine = Object.create(Object, {

        _commandQueue: { value: null, writable: true },

        _renderer: { value: null, writable: true },

        _rootPass: { value: null, writable: true },
    
        commandQueue: {
            get: function() {
                return this._commandQueue;
            },
            set: function(value) {
                this._commandQueue = value;
            }
        },

        rootPass: {
            get: function() {
                return this._rootPass;
            },
            set: function(value) {
                this._rootPass = value;
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
                this.renderer = Object.create(Renderer);
                this.rootPass = Object.create(Pass);
                this.rootPass.init();
                this.commandQueue = Object.create(CommandQueue);
                this.renderer.webGLContext = webGLContext;      
                return this;
            }
        },
    
        render: {
            value: function() {
                this.renderer.resetStates();
                this.commandQueue.reset();
                this.rootPass.execute(this);
                this.commandQueue.execute(this.renderer);
            }
        }

    });
    
        return Engine;
    }
);

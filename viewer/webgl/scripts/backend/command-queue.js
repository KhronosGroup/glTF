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

define(function() {

    var CommandQueue = Object.create(Object, {

        _queue: { value: null, writable: true },
    
        enqueuePrimitive: {
            value: function(primitive) {
                this._queue.push(primitive);
            }
        },

        execute: {
            value: function(renderer) {
                var unvavailable = 0;
                var count = this._queue.length;
                //give the loading some space, if we keep on being queued don't render/queue more
                //TODO: improve more this by keeping track of the "available" primitives.
                //Now it works well because the rendering order is always the same but this needs to be improved when we will re-order for blending.
                for (var i = 0 ; (i < count) && (unvavailable < 7) ; i++) {
                    if (renderer.renderPrimitive(this._queue[i]) === false) {
                        unvavailable++;
                    }
                }
            }
        },
    
        reset: {
            value: function() {
                this._queue = [];
            }
        }
    });
    
        return CommandQueue;
    }
);

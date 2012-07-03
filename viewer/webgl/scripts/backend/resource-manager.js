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

    var ResourceManager = Object.create(Object, {

        // errors
        MISSING_DESCRIPTION: { value: "MISSING_DESCRIPTION" },
        INVALID_PATH: { value: "INVALID_PATH" },
        INVALID_TYPE: { value: "INVALID_TYPE" },
        XMLHTTPREQUEST_STATUS_ERROR: { value: "XMLHTTPREQUEST_STATUS_ERROR" },
        NOT_FOUND: { value: "NOT_FOUND" },
        MAX_CONCURRENT_XHR: { value: 7 },
        // misc constants
        ARRAY_BUFFER: { value: "ArrayBuffer" },

        _resources: { value: null, writable: true },
        
        //manage entries
        _containsResource: {
            enumerable: false,
            value: function(resourceID) {
                return this._resources[resourceID] ? true : false;
            }
        },
    
        init: {
            value: function() {
                this._resources = {};
                this._resourcesToBeProcessed = [];
                this._resourcesBeingProcessedCount = 0;
            }
        },
    
        _storeResource: {
            enumerable: false,
            value: function(resourceID, resource) {
                if (!resourceID) {
                    console.log("ERROR: entry does not contain id, cannot store");
                    return;
                }
        
                if (this._containsResource[resourceID]) {
                    console.log("WARNING: resource:"+resourceID+" is already stored, overriding");
                }
            
               this._resources[resourceID] = resource;
            }
        },
    
        _getResource: {
            enumerable: false,
            value: function(resourceID) {
                return this._resources[resourceID];
            }
        },
    
        _resourcesBeingProcessedCount: { value: 0, writable: true },
    
        _resourcesToBeProcessed: { value: null, writable: true },
    
        _loadResource: {
            value: function(delegate, resource, range) {

                var self = this;
                var description = resource.resourceDescription.description;
            
                if (!description.type) {
                    delegate.handleError(ResourceManager.INVALID_TYPE, null);
                    return;
                }
                if (description.type === this.ARRAY_BUFFER) {
                    if (!description.path) {
                        delegate.handleError(ResourceManager.INVALID_PATH);
                        return;
                    }
                    
                    var xhr = new XMLHttpRequest();
                
                  xhr.open('GET', description.path, true);
                    xhr.responseType = 'arraybuffer';
                    if (range) {
                        var header = "bytes=" + range[0] + "-" + (range[1] - 1);
                        xhr.setRequestHeader("Range", header);
                    }
                    //if this is not specified, 1 "big blob" scenes fails to load.
                    xhr.setRequestHeader("If-Modified-Since", "Sat, 1 Jan 1970 00:00:00 GMT");
                    xhr.onload = function(e) {
                        if ((this.status == 200) || (this.status == 206)) {
                            self._resource = this.response;
                            delegate.resourceAvailable(resource, self._resource);
                        } else {
                            delegate.handleError(ResourceManager.XMLHTTPREQUEST_STATUS_ERROR, this.status);
                        }
                    };
                    xhr.send(null);
                       
                } else {
                    delegate.handleError(ResourceManager.INVALID_TYPE, description.type);
                }
            }
        },
    
        /*
            process the queue of resources one at a time and pick them in "_resourcesToBeProcessed"
            this method should be cleaned up a bit. dequeuing code is redundant.
        
            FIXME: make sure to check with resource id / url before putting it again in the queue again
        */
        getWebGLResource: {
                value: function(id, resourceDescription, range, delegate, ctx, fromQueue) {

                var resource = this._getResource(id);
                if (resource) {
                    return resource;
                }

                var resourceToBeProcessed = {"resourceDescription" : resourceDescription,
                                            "delegate" : delegate,
                                            "ctx" : ctx,
                                            "range" : range,
                                            "id" : id  };
                
                if (this._resourcesBeingProcessedCount >= ResourceManager.MAX_CONCURRENT_XHR) {
                    if (!fromQueue) {
                        this._resourcesToBeProcessed.push(resourceToBeProcessed);
                    }
                    return null;
                }
            
                if (delegate) {
                    var self = this;
                    var processResourceDelegate = {};
                
                    processResourceDelegate.resourceAvailable = function(resourceInProcess, resource) {
                        var resourceDescription = resourceInProcess.resourceDescription;
                        // ask the delegate to convert the resource, typically here, the delegate is the renderer and will produce a webGL array buffer
                        // this could get more general and flexbile by make an unique key with the id from the resource + the converted type (day "ARRAY_BUFFER" or "TEXTURE"..)
                        //, but as of now, this flexibily does not seem necessary.
                        // console.log("call convert for:"+resourceDescription.id);
                        convertedResource = delegate.convert(resource, ctx);
                        self._storeResource(id, convertedResource);
                        delegate.resourceAvailable(convertedResource, ctx);
                        self._resourcesBeingProcessedCount--;
                        //process next element
                        var existingResource = null;
                        if (self._resourcesToBeProcessed.length > 0) {
                            do {
                                var nextResourceToBeProcessed = self._resourcesToBeProcessed.pop();
                                existingResource = self.getWebGLResource(nextResourceToBeProcessed.id,
                                                        nextResourceToBeProcessed.resourceDescription,
                                                        nextResourceToBeProcessed.range,
                                                        nextResourceToBeProcessed.delegate,
                                                        nextResourceToBeProcessed.ctx,
                                                        true);
                            } while ((self._resourcesToBeProcessed.length > 0) && (existingResource !== null));
                        }
                    };
                
                    processResourceDelegate.handleError = function(errorCode, info) {
                        delegate.handleError(errorCode, info);
                    };

                    self._resourcesBeingProcessedCount++;
                    this._loadResource(processResourceDelegate, resourceToBeProcessed, range);
                }
                
                return null;
            }
        },

        removeAllResources: {
            value: function() {
                this._resources = {};
            }
        }
        
    });
    
        return ResourceManager;
    }
);

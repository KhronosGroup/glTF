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

    var LinkedListNode = Object.create(Object, {

        _content: { value: null, writable:true},

        content: {
            get: function() {
                return this._content;
            },
            set: function(value) {
                this._content = value;
            }
        },

        _previous: { value: null, writable:true},

        previous: {
            get: function() {
                return this._previous;
            },
            set: function(value) {
                this._previous = value;
            }
        },

        _next: { value: null, writable:true},

        next: {
            get: function() {
                return this._next;
            },
            set: function(value) {
                this._next = value;
            }
        },

        init: {
            value: function(content) {
                this.content = content;
                this.previous = null;
                this.next = null;
            }
        },

        removeFromList: {
            value: function() {
                if (this.previous) {
                    this.previous.next = this.next;
                }
                if (this.next) {
                    this.next.previous = this.previous;
                }
                this.next = null;
                this.previous = null;
            }
        }

    });

    var LinkedList = Object.create(Object, {

        _tail: { value: null, writable:true},

        tail: {
            get: function() {
                return this._tail;
            },
            set: function(value) {
                this._tail = value;
            }
        },

        _head: { value: null, writable:true},

        head: {
            get: function() {
                return this._head;
            },
            set: function(value) {
                this._head = value;
            }
        },

        append: {
            value: function(node) {
                if (!this.head) {
                    this.head = node;
                }
                if (this.tail) {
                    this.tail.next = node;
                } 
                this.tail = node;
            }
        },

        remove: {
            value: function(node) {
                if (this.tail === node) {
                    this.tail = node.previous;
                }

                if (this.head === node) {
                    this.head = node.next;
                }

                node.removeFromList();
            }
        }

    });

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
                this._resourcesStatus = {};
                this._resourcesToBeProcessed = Object.create(LinkedList);
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

        _resourcesStatus: { value: null, writable: true },
    
        _resourcesBeingProcessedCount: { value: 0, writable: true },
    
        _resourcesToBeProcessed: { value: null, writable: true },
    
        _loadResource: {
            value: function(request, delegate) {

                var self = this;
                if (!request.type) {
                    debugger;
                    delegate.handleError(ResourceManager.INVALID_TYPE, null);
                    return;
                }
                if (request.type === this.ARRAY_BUFFER) {
                    if (!request.path) {
                        delegate.handleError(ResourceManager.INVALID_PATH);
                        return;
                    }
                    
                    var xhr = new XMLHttpRequest();
                
                    xhr.open('GET', request.path, true);
                    xhr.responseType = 'arraybuffer';
                    if (request.range) {
                        var header = "bytes=" + request.range[0] + "-" + (request.range[1] - 1);
                        xhr.setRequestHeader("Range", header);
                    }
                    //if this is not specified, 1 "big blob" scenes fails to load.
                    xhr.setRequestHeader("If-Modified-Since", "Sat, 1 Jan 1970 00:00:00 GMT");                                                
                    xhr.onload = function(e) {
                        if ((this.status == 200) || (this.status == 206)) {
                            delegate.resourceAvailable(request, this.response);
                        } else {
                            delegate.handleError(ResourceManager.XMLHTTPREQUEST_STATUS_ERROR, this.status);
                        }
                    };
                    xhr.send(null);
                       
                } else {
                    delegate.handleError(ResourceManager.INVALID_TYPE, request.type);
                }
            }
        },
    
        _processNextResource: {
            value: function() {
                var nextNode = this._resourcesToBeProcessed.head;
                if (nextNode) {
                    var nextRequest = nextNode.content;
                    this._handleRequest(nextRequest, nextNode);
                } 
            }
        },

        _handleRequest: {
            value: function(request, node) {
                if (this._resourcesStatus[request.id] === "loading") 
                    return null;
 
                if (this._resourcesBeingProcessedCount >= ResourceManager.MAX_CONCURRENT_XHR) {
                    if (!this._resourcesStatus[request.id]) {
                        var listNode = Object.create(LinkedListNode);                
                        this._resourcesStatus[request.id] = "queued";
                        listNode.init(request);
                        this._resourcesToBeProcessed.append(listNode);
                    }
                    return null;
                }
            
                if (request.delegate) {
                    var self = this;
                    var processResourceDelegate = {};
                    
                    this._resourcesStatus[request.id] = "loading";
                    if (node) {
                        this._resourcesToBeProcessed.remove(node);
                    }

                    processResourceDelegate.resourceAvailable = function(request, resource) {
                        // ask the delegate to convert the resource, typically here, the delegate is the renderer and will produce a webGL array buffer
                        // this could get more general and flexbile by make an unique key with the id from the resource + the converted type (day "ARRAY_BUFFER" or "TEXTURE"..)
                        //, but as of now, this flexibily does not seem necessary.
                        
                        convertedResource = request.delegate.convert(resource, request.ctx);
                        self._storeResource(request.id, convertedResource);
                        request.delegate.resourceAvailable(convertedResource, request.ctx);
                        self._resourcesBeingProcessedCount--;
                        
                        delete self._resourcesStatus[request.id];

                        if (self._resourcesBeingProcessedCount  >= 4) {
                            setTimeout(self._processNextResource.bind(self), 1);
                        } else {
                           self._processNextResource();
                        }
                    };
                
                    processResourceDelegate.handleError = function(errorCode, info) {
                        request.delegate.handleError(errorCode, info);
                    }

                    self._resourcesBeingProcessedCount++;
                    this._loadResource(request, processResourceDelegate);
                }
                
                return null;

            }
        },

        _handleAccessorResourceLoading: {
            value: function(accessor, delegate, ctx) {
                var range = [accessor.byteOffset ? accessor.byteOffset : 0 , (accessor.byteStride * accessor.count) + accessor.byteOffset];
                this._handleRequest({   "id" : accessor.id,
                                        "range" : range,
                                        "type" : accessor.buffer.description.type,
                                        "path" : accessor.buffer.description.path,
                                        "delegate" : delegate,
                                        "ctx" : ctx }, null);
            }
        },

        _handleTypedArrayLoading: {
            value: function(typedArrayDescr, delegate, ctx) {
            
                if (typedArrayDescr.type === "Uint16Array") {
                    var range = [typedArrayDescr.byteOffset, typedArrayDescr.byteOffset + ( typedArrayDescr.length * Uint16Array.BYTES_PER_ELEMENT)];

                    this._handleRequest({   "id":typedArrayDescr.id,
                                            "range" : range,
                                            "type" : typedArrayDescr.buffer.description.type,
                                            "path" : typedArrayDescr.buffer.description.path,
                                            "delegate" : delegate,
                                            "ctx" : ctx }, null);
                }
            }
        },

        getResource: {
                value: function(resource, delegate, ctx, node) {

                var managedResource = this._getResource(resource.id);
                if (managedResource) {
                    return managedResource;
                }

                if (resource.type === "accessor") {
                    this._handleAccessorResourceLoading(resource, delegate, ctx, node);
                } else {
                    this._handleTypedArrayLoading(resource, delegate, ctx, node);
                }

                return null;
            }
        },

        removeAllResources: {
            value: function() {
                this._resources = {};
            }
        },
        
    });
    
        return ResourceManager;
    }
);

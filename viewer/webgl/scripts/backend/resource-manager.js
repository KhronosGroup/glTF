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

    var ContiguousRequests = Object.create(Object, {

        range: { value: null, writable:true },

        _requests: { value: null, writable:true },

        requests: {
            get: function() {
                return this._requests;
            },
            set: function(value) {
                this._requests = value;
            }
        },

        canMergeRequest: {
            value: function(request) {
                return  ((request.range[0] === this.range[1]) ||
                         (request.range[1] === this.range[0]));
            }
        },

        mergeRequest: {
            value: function(request) {
                if (this.requests.length === 0) {
                    this.requests.push(request);
                    this.range = request.range;
                } else {
                    if (request.range[0] === this.range[1]) {
                        this.requests.push(request);
                        this.range[1] = request.range[1];
                    } else if (request.range[1] === this.range[0]) {
                        this.requests.unshift(request);
                        this.range[0] = request.range[0];
                    } else {
                        console.log("ERROR: should not reach");
                        debugger;
                    }
                }
            }
        },

        initWithRequests: {
            value: function(requests) {
                this.requests = [];

                requests.forEach(function(request) {
                    this.mergeRequest(request);
                }, this);

                return this;
            }
        }

    })

    var RequestTreeNode = Object.create(Object, {

        _content: { value: null, writable:true},

        content: {
            get: function() {
                return this._content;
            },
            set: function(value) {
                this._content = value;
            }
        },

        _left: { value: null, writable:true},

        left: {
            get: function() {
                return this._left;
            },
            set: function(value) {
                this._left = value;
            }
        },

        _right: { value: null, writable:true},

        right: {
            get: function() {
                return this._right;
            },
            set: function(value) {
                this._right = value;
            }
        },

        insert: {
            value: function(requests) {
                //insert before ?
                if (requests.range[1] <= this.content.range[0]) {
                    if (this.left) {
                        this.left.insert(requests);
                    } else {
                        var treeNode = Object.create(RequestTreeNode);
                        treeNode.content = requests; 
                        this.left = treeNode;
                    }

                //insert after ?
                } else if (requests.range[0] >= this.content.range[1]) {
                    if (this.right) {
                        this.right.insert(requests);
                    } else {
                        var treeNode = Object.create(RequestTreeNode);
                        treeNode.content = requests; 
                        this.right = treeNode;
                    }
                } else {
                    console.log("ERROR: should not reach");
                    debugger;
                }
            }
        }

    });

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
                    node.previous = this.tail;
                    this.tail.next = node;
                } 
                this.tail = node;

                if (!this.tail) {
                    debugger;
                }
            }
        },

        remove: {
            value: function(node) {
                var id = node.content.id;

                var isTail = false,isHead = false;
                if (this.tail === node) {
                    isTail = true;
                    this.tail = node.previous;
                }

                if (this.head === node) {
                    isHead = true;
                    this.head = node.next;
                }

                node.removeFromList();
                /* consistency check 
                for (cnode = this.head ; cnode != null ; cnode = cnode.next) {
                    if (id === cnode.content.id) {
                        console.log("isTail:"+isTail+" isHead:"+isHead);
                        debugger;
                    }
                }
                */
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

        _requestTree: { value: null, writable: true },
        
        //manage entries
        _containsResource: {
            enumerable: false,
            value: function(resourceID) {
                return this._resources[resourceID] ? true : false;
            }
        },
    
        init: {
            value: function() {
                this._requestTree = null;
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

                    if (!this._resourcesStatus[nextRequest.id]) {
                        debugger;
                    }

                    this._handleRequest(nextRequest);
                } 
            }
        },

        _handleRequest: {
            value: function(request) {
                var resourceStatus = this._resourcesStatus[request.id];
                var node = null;
                var status = null;
                if (resourceStatus) {
                    if (resourceStatus.status === "loading" )
                        return null;
                    node = resourceStatus.node;
                    status = resourceStatus.status;
                }
 
                if (this._resourcesBeingProcessedCount >= ResourceManager.MAX_CONCURRENT_XHR) {
                    if (!status) {     
                        if (node) {
                            debugger;
                        }

                        if (request.id === "accessor.6")
                            debugger;

                        var listNode = Object.create(LinkedListNode);                
                        listNode.init(request);

                        this._resourcesStatus[request.id] =  { "status": "queued", "node": listNode };
                        /*
                        var contRequests = Object.create(ContiguousRequests);
                        contRequests.initWithRequests([request]);
                        if (!this._requestTree) {
                            var rootTreeNode = Object.create(RequestTreeNode);
                            rootTreeNode.content = contRequests;
                            this._requestTree = rootTreeNode;
                        } else {
                            this._requestTree.insert(contRequests);
                        }
                        */
                        this._resourcesToBeProcessed.append(listNode);
                    }
                    return null;
                }
            
                if (request.delegate) {
                    var self = this;
                    var processResourceDelegate = {};
                    
                    //if (request.id === "accessor.6")
                    //     console.log("load node:"+request.id+":node:"+node)

                    if (node) {
                        this._resourcesToBeProcessed.remove(node);
                    }

                    this._resourcesStatus[request.id] =  { "status": "loading"};

                    processResourceDelegate.resourceAvailable = function(req_, res_) {
                        // ask the delegate to convert the resource, typically here, the delegate is the renderer and will produce a webGL array buffer
                        // this could get more general and flexbile by make an unique key with the id from the resource + the converted type (day "ARRAY_BUFFER" or "TEXTURE"..)
                        //, but as of now, this flexibily does not seem necessary.
                        convertedResource = req_.delegate.convert(res_, req_.ctx);
                        self._storeResource(req_.id, convertedResource);
                        req_.delegate.resourceAvailable(convertedResource, req_.ctx);
                        self._resourcesBeingProcessedCount--;
                        
                        //console.log("delete:"+req_.id)
                        delete self._resourcesStatus[req_.id];

                        if (self._resourcesBeingProcessedCount  >= 3) {
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
                value: function(resource, delegate, ctx) {

                var managedResource = this._getResource(resource.id);
                if (managedResource) {
                    return managedResource;
                }                

                if (resource.type === "accessor") {
                    this._handleAccessorResourceLoading(resource, delegate, ctx);
                } else {
                    this._handleTypedArrayLoading(resource, delegate, ctx);
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

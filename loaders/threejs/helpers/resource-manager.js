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
        module.exports.ResourceManager = module.exports;
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
    "use strict";

    var ContiguousRequests = Object.create(Object, {

        kind: { value:"multi-parts", writable:true },

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
            value: function(request, bytesLimit) {
                var requestSize = request.range[1] - request.range[0];
                var size = this.range[1] - this.range[0];
                if ((requestSize + size) > bytesLimit)
                    return false;
                return  (((request.range[0] === this.range[1]) ||
                         ((request.range[1]) === this.range[0])) &&
                         (request.type ===  this.type));
            }
        },

        mergeRequest: {
            value: function(request) {
                if (this.requests.length === 0) {
                    this.requests.push(request);
                    this.range = [request.range[0], request.range[1]];
                } else {
                    //are we merging at end ?
                    if (request.range[0] === this.range[1]) {
                        this.requests.push(request);
                        this.range[1] = request.range[1];
                    //or at the beginning ?
                    } else if (request.range[1] === this.range[0]) {
                        this.requests.unshift(request);
                        this.range[0] = request.range[0];
                    } else {
                        console.log("ERROR: should not reach");
                    }
                }
            }
        },

        mergeRequests: {
            value: function(requests) {
                if (this.range) {
                    if (requests[0].range[1] < this.range[0]) {
                        for (var i = requests.length-1 ; i >= 0 ; i--) {
                            this.mergeRequest(requests[i]);
                        }
                    } else {
                        for (var i = 0 ; i < requests.length ; i++) {
                            this.mergeRequest(requests[i]);
                        }
                    }
                } else {
                    requests.forEach( function(request) {
                        this.mergeRequest(request);
                    }, this);
                }
            }
        },

        id: { value: null, writable:true},

        initWithRequests: {
            value: function(requests) {
                this.requests = [];

                this.mergeRequests(requests);
                this.id = requests[0].id;
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

        _parent: { value: null, writable:true},

        parent: {
            get: function() {
                return this._parent;
            },
            set: function(value) {
                this._parent = value;
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

        _checkConsistency: {
            value: function(ranges) {

                if (ranges.length >= 50) //to avoid max stack size
                    return;

                if (this.left)
                    this.left._checkConsistency(ranges);

                ranges.push(this.content.range);

                if (this.right)
                    this.right._checkConsistency(ranges);

            }
        },

        checkConsistency: {
            value: function() {
                var ranges = [];
                this._checkConsistency(ranges);
                if (ranges.length > 1) {
                    for (var i = 0; i < ranges.length-1 ;i++) {
                        var rangeA = ranges[0];
                        var rangeB = ranges[1];
                        if (!(rangeA[1] <= rangeB[0])) {
                            console("ERROR: INCONSISTENCY CHECK Failed, ranges are not ordered in btree")
                        }
                    }
                }

            }
        },

        _collect: {
            value: function(nodes, nb) {

                if (this.left)
                    this.left._collect(nodes, nb);

                if (nodes.length >= nb)
                    return;

                nodes.push(this);

                if (this.right)
                    this.right._collect(nodes, nb);

            }
        },

        collect: {
            value: function(nb) {
                var nodes = [];
                this._collect(nodes, nb);
                return nodes;
            }
        },


        insert: {
            value: function(requests, bytesLimit) {
                //insert before ?
                if (requests.range[1] <= this.content.range[0]) {
                    if ( (requests.range[1] === this.content.range[0])) {
                        if (requests.kind === "multi-parts")
                            this.content.mergeRequests(requests.requests);
                        else
                            this.content.mergeRequests(requests);

                        if (this.left) {
                            if(this.content.canMergeRequest(this.left.content, bytesLimit)) {
                                this.content.mergeRequests(this.left.content._requests);
                                this.left.remove(this.left.content);
                            }
                        } 
                        if (this.parent) {
                            if (this.parent.content.canMergeRequest(this.content, bytesLimit)) {
                                this.parent.content.mergeRequests(this.content._requests);
                                this.remove(this.content);
                            }
                        }

                        //console.log("requests:"+this.content.requests.length);
                        return null;
                    } else if (this.left) {
                        return this.left.insert(requests, bytesLimit);
                    } else {
                        var treeNode = Object.create(RequestTreeNode);
                        treeNode.parent = this;
                        treeNode.content = requests;
                        this.left = treeNode;
                        return treeNode;
                    }
                //insert after ?
                } else if (requests.range[0] >= this.content.range[1]) {
                    if ( requests.range[0] === this.content.range[1]) {
                        if (requests.kind === "multi-parts")
                            this.content.mergeRequests(requests.requests);
                        else
                            this.content.mergeRequests(requests);

                        
                        if (this.right) {
                            if(this.content.canMergeRequest(this.right.content, bytesLimit)) {
                                this.content.mergeRequests(this.right.content._requests);
                                this.right.remove(this.right.content);
                            }
                        }  
                        if (this.parent) {
                            if (this.parent.content.canMergeRequest(this.content, bytesLimit)) {
                                this.parent.content.mergeRequests(this.content._requests);
                                this.remove(this.content);
                            }
                        }


                        //console.log("requests:"+this.content.requests.length);
                        return null;
                    } else if (this.right) {
                        return this.right.insert(requests, bytesLimit);
                    } else {
                        var treeNode = Object.create(RequestTreeNode);
                        treeNode.parent = this;
                        treeNode.content = requests;
                        this.right = treeNode;
                        return treeNode;
                    }
                } else {
                    console.log("ERROR: should not reach");
                }
            }
        },

        contains: {
            value: function(node) {
                if (this.node === node) {
                    return true;
                }

                if (this.left) {
                    if (this.left.contains(node))
                        return true;
                }

                if (this.right) {
                    if (this.right.contains(node))
                        return true;
                }

                return false;
            }
        },

        min: {
            value: function() {
                var node = this;
                while (node.left) {
                    node = node.left;
                }
                return node;
            }
        },

        _updateParentWithNode: {
            value: function(node) {
                if (this.parent) {
                    if (this === this.parent.left) {
                        this.parent.left = node;
                    } else {
                        this.parent.right = node;
                    }
                }
                if (node) {
                    node.parent = this.parent;
                }
            }
        },

        removeMin: {
            value: function() {
                if (this.parent) {
                }

                var node = this;
                while (node.left || node.right) {
                    if (node.left) {
                       node = node.min();
                    } 
                    if (node.right) {
                        node = node.right.min();
                    } 
                }
                node._updateParentWithNode(null);
                return node;
            }
        },

        remove: {
            value: function(content) {
                if (content.range[1] <= this.content.range[0]) {
                    this.left.remove(content);
                } else if (content.range[0] >= this.content.range[1]) {
                    this.right.remove(content);
                } else {
                    if (content === this.content) {
                        if (this.left && this.right) {
                            var successor = this.right.min();
                            this.content = successor.content;
                            successor._updateParentWithNode(successor.right);
                        } else if (this.left || this.right) {
                            if (this.left) {
                                this._updateParentWithNode(this.left);
                            } else {
                                this._updateParentWithNode(this.right);
                            }
                        } else {
                            this._updateParentWithNode(null);
                        }
                    } else {
                        //FIXME: report error
                    }
                }
            }
        }

    });

    var WebGLTFResourceManager = Object.create(Object, {

        // errors
        MISSING_DESCRIPTION: { value: "MISSING_DESCRIPTION" },
        INVALID_PATH: { value: "INVALID_PATH" },
        INVALID_TYPE: { value: "INVALID_TYPE" },
        XMLHTTPREQUEST_STATUS_ERROR: { value: "XMLHTTPREQUEST_STATUS_ERROR" },
        NOT_FOUND: { value: "NOT_FOUND" },
        // misc constants
        ARRAY_BUFFER: { value: "ArrayBuffer" },

        _resources: { value: null, writable: true },

        _requestTrees: { value: null, writable: true },

        requestTrees : {
           get: function() {
               return this._requestTrees;
           }
        },

        _resourcesStatus: { value: null, writable: true },

        _resourcesBeingProcessedCount: { value: 0, writable: true },

        _observers: { value: null, writable: true },
        
        _maxConcurrentRequests: { value: 1, writable: true },

        observers: {
            get: function() {
                return this._observers;
            },
            set: function(value) {
                this._observers = value;
            }
        },

        maxConcurrentRequests: {
            get: function() {
                return this._maxConcurrentRequests;
            },
            set: function(value) {
                this._maxConcurrentRequests = value;
            }
        },

        reset: {
            value: function() {
                if (this._resourcesStatus) {
                    var ids = Object.keys(this._resourcesStatus);
                    ids.forEach(function(id) {
                        var status = this._resourcesStatus[id];
                        if (status) {
                            if (status.xhr)
                                status.xhr.abort();
                        }
                    }, this);
                }

                this._resources = {};
                this._requestTrees = {};
                this._resourcesStatus = {};
                this._resourcesBeingProcessedCount = 0;
            }
        },

        _bytesLimit: { value: 500000, writable: true },
        
        bytesLimit: {
            get: function() {
                return this._bytesLimit;
            },
            set: function(value) {
                if (this._bytesLimit !== value) {
                    this._bytesLimit = value;
                }
            }
        },

        //manage entries
        _containsResource: {
            enumerable: false,
            value: function(resourceID) {
                return this._resources[resourceID] ? true : false;
            }
        },

        init: {
            value: function() {
                this._requestTrees = {};
                this._resources = {};
                this._resourcesStatus = {};
                this._observers = [];
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

        _loadResource: {
            value: function(request, delegate) {
                var self = this;
                var type;
                var path = request.path;
                if (request.kind === "multi-parts") {
                    type = request.requests[0].type;
                    path = request.requests[0].path;
                } else {
                    type = request.type;
                    path = request.path;
                }

                if (!type) {
                    delegate.handleError(WebGLTFResourceManager.INVALID_TYPE, null);
                    return;
                }

                if (!path) {
                    delegate.handleError(WebGLTFResourceManager.INVALID_PATH);
                    return;
                }

                var xhr = new XMLHttpRequest();
                xhr.open('GET', path, true);
                xhr.responseType = (type === this.ARRAY_BUFFER) ? "arraybuffer" : "text";
                if (request.range) {
                    var header = "bytes=" + request.range[0] + "-" + (request.range[1] - 1);
                    xhr.setRequestHeader("Range", header);
                }
                //if this is not specified, 1 "big blob" scenes fails to load.
                xhr.setRequestHeader("If-Modified-Since", "Sat, 01 Jan 1970 00:00:00 GMT");
                xhr.onload = function(e) {
                    if ((this.status == 200) || (this.status == 206)) {
                        self._resourcesBeingProcessedCount--;

                        if (request.kind === "multi-parts") {
                            request.requests.forEach( function(req_) {
                                var subArray = this.response.slice(req_.range[0] - request.range[0], req_.range[1] - request.range[0]);
                                delegate.resourceAvailable(self, req_, subArray);
                            }, this);
                        } else {
                            delegate.resourceAvailable(self, request, this.response);
                        }

                    } else {
                        delegate.handleError(WebGLTFResourceManager.XMLHTTPREQUEST_STATUS_ERROR, this.status);
                    }
                };
                xhr.send(null);
                var resourceStatus = this._resourcesStatus[request.id];
                if (resourceStatus) {
                    resourceStatus.xhr = xhr;
                }
            }
        },

        _processNextResource: {
            value: function(requestTree) {
                if (requestTree) {
                    var rootIsLeaf = !requestTree.left && !requestTree.right;
                    if (rootIsLeaf) {
                        this._handleRequest(requestTree.content);
                        return false;
                    } else {
                        var min = requestTree.removeMin();
                        this._handleRequest(min.content);
                    }
                }
                return true;
            }
        },

        fireResourceAvailable: {
            value: function(request) {
                if (this.observers) {   
                    this.observers.forEach(function(observer) {
                        if (observer.resourceAvailable) {
                            observer.resourceAvailable(request);
                        }
                    }, this);
                }
            }
        },

        send: { value: 0, writable: true },
        requested: { value: 0, writable: true },

        _handleRequest: {
            value: function(request) {
                var resourceStatus = this._resourcesStatus[request.id];
                var node = null;
                var status = null;
                var requestTree = this.requestTrees ? this.requestTrees[request.path] : null;
                if (resourceStatus) {
                    if (resourceStatus.status === "loading" )
                        return;
                    node = resourceStatus.node;
                    status = resourceStatus.status;
                }

                if ((this._resourcesBeingProcessedCount >= this.maxConcurrentRequests) && (request.type === "ArrayBuffer")) {
                    if (!status) {
                        var trNode = null;
                        var contRequests;

                        if (request.kind === "multi-parts") {
                             contRequests = Object.create(ContiguousRequests).initWithRequests(request.requests);
                        } else {
                             contRequests = Object.create(ContiguousRequests).initWithRequests([request]);
                        }

                        if (!requestTree) {
                            var rootTreeNode = Object.create(RequestTreeNode);
                            rootTreeNode.content = contRequests;
                            this._requestTrees[request.path] = rootTreeNode;
                            requestTree = rootTreeNode;
                            trNode = rootTreeNode;
                        } else {
                            trNode = requestTree.insert(contRequests, this.bytesLimit);
                        }

                        if (request.kind ==="multi-parts") {
                            request.requests.forEach( function(req_) {
                                this._resourcesStatus[req_.id] =  { "status": "queued", "node": trNode };
                            }, this);

                        } else {
                            this._resourcesStatus[request.id] =  { "status": "queued", "node": trNode };
                        }
                    }
                    return;
                }
                var self = this;
                var processResourceDelegate = {};

                if (request.kind ==="multi-parts") {
                    request.requests.forEach( function(req_) {
                        this._resourcesStatus[req_.id] =  { "status": "loading" };
                    }, this);

                } else {
                    this._resourcesStatus[request.id] =  { "status": "loading"};
                }

                processResourceDelegate.resourceAvailable = function(resourceManager, req_, res_) {
                    // ask the delegate to convert the resource, typically here, the delegate is the renderer and will produce a webGL array buffer
                    // this could get more general and flexible by making an unique key with the id from the resource + the converted type (day "ARRAY_BUFFER" or "TEXTURE"..)
                    //, but as of now, this fexibility does not seem necessary.
                    var convertedResource = req_.delegate.convert(res_, req_.ctx);
                    self._storeResource(req_.id, convertedResource);
                    req_.delegate.resourceAvailable(convertedResource, req_.ctx);

                    delete self._resourcesStatus[req_.id];

                    self.fireResourceAvailable.call(self, req_);

                    if (self._resourcesBeingProcessedCount < self.maxConcurrentRequests) {
                        var requestTree  = resourceManager.requestTrees ? resourceManager.requestTrees[req_.path] : null;
                        if (!self._processNextResource(requestTree)) {
                            if (requestTree) {
                                delete resourceManager.requestTrees[req_.path];
                            }
                        };
                    } 

                };

                processResourceDelegate.handleError = function(errorCode, info) {
                    request.delegate.handleError(errorCode, info);
                }

                self._resourcesBeingProcessedCount++;
                this._loadResource(request, processResourceDelegate);
            }
        },


        _elementSizeForGLType: {
            value: function(glType) {
                switch (glType) {
                    case "FLOAT" :
                        return Float32Array.BYTES_PER_ELEMENT;
                    case "UNSIGNED_BYTE" :
                        return Uint8Array.BYTES_PER_ELEMENT;
                    case "UNSIGNED_SHORT" :
                        return Uint16Array.BYTES_PER_ELEMENT;
                    case "FLOAT_VEC2" :
                        return Float32Array.BYTES_PER_ELEMENT * 2;
                    case "FLOAT_VEC3" :
                        return Float32Array.BYTES_PER_ELEMENT * 3;
                    case "FLOAT_VEC4" :
                        return Float32Array.BYTES_PER_ELEMENT * 4;
                    default:
                        return null;
                }
            }
        },

        _handleWrappedBufferViewResourceLoading: {
            value: function(wrappedBufferView, delegate, ctx) {
                var bufferView = wrappedBufferView.bufferView;
                var buffer = bufferView.buffer;
                var byteOffset = wrappedBufferView.byteOffset + bufferView.description.byteOffset;
                var range = [byteOffset , (this._elementSizeForGLType(wrappedBufferView.type) * wrappedBufferView.count) + byteOffset];

                this._handleRequest({   "id" : wrappedBufferView.id,
                                        "range" : range,
                                        "type" : buffer.description.type,
                                        "path" : buffer.description.path,
                                        "delegate" : delegate,
                                        "ctx" : ctx,
                                        "kind" : "single-part" }, null);
            }
        },

        //TODO: move this in the renderer, to do so, add dynamic handler to the resource manager.
        shaderDelegate: {
            value: {
                handleError: function(errorCode, info) {
                    console.log("ERROR:shaderDelegate:"+errorCode+" :"+info);
                },

                convert: function (resource, ctx) {
                    return resource;
                },

                resourceAvailable: function (resource, ctx) {
                    //FIXME: ...
                    ctx.sources[ctx.stage] = resource;
                    var self = this;
                    if (ctx.sources["x-shader/x-fragment"] && ctx.sources["x-shader/x-vertex"]) {
                        var delegate = ctx.programCtx.delegate;
                        var convertedResource = delegate.convert(ctx.sources, ctx.programCtx.ctx);
                        ctx.programCtx.resourceManager._storeResource(ctx.programCtx.id, convertedResource);
                        delegate.resourceAvailable(convertedResource, ctx.programCtx.ctx);
                    }
                }
            }
        },

        _handleProgramLoading: {
            value: function(program, delegate, ctx) {
                var programCtx = { "delegate" : delegate, "ctx"  : ctx, "resourceManager" : this, "id" : program.id };

                var sources = {};
                var fsCtx = { stage: "x-shader/x-fragment", "sources" : sources,  "programCtx" : programCtx };
                var vsCtx = { stage: "x-shader/x-vertex", "sources" : sources,  "programCtx" : programCtx };

                this.getResource(program.description["x-shader/x-fragment"], this.shaderDelegate, fsCtx);
                this.getResource(program.description["x-shader/x-vertex"], this.shaderDelegate, vsCtx);
            }
        },

        _handleShaderLoading: {
            value: function(shader, delegate, ctx) {
                this._handleRequest({   "id":shader.id,
                                            "type" : "text",
                                            "path" : shader.description.path,
                                            "delegate" : delegate,
                                            "ctx" : ctx,
                                            "kind" : "single-part" }, null);
            }
        },

        _handleImageLoading: {
            value: function(resource, textureLoadedCallback, ctx) {
                //TODO: unify with binaries
                var resourceStatus = this._resourcesStatus[resource.id];
                var status = null;
                if (resourceStatus) {
                    if (resourceStatus.status === "loading" )
                        return;
                }
                this._resourcesStatus[resource.id] = { status: "loading" };
                var self = this;

                if (resource.description.path) {
                    var imageObject = new Image();  
                    imageObject.onload = function() { 
                        delete self._resourcesStatus[resource.id];
                        self._storeResource(resource.id, imageObject);
                        textureLoadedCallback(imageObject, resource.id, ctx); 
                    }  
                    imageObject.src = resource.description.path;  
                } else if (resource.description.image) {
                    textureLoadedCallback(resource.description.image, resource.id, ctx);                  
                }
            }
        },

        _handleSampler2DLoading: {
            value: function(resource, delegate, ctx) {
                //TODO: unify with binaries
                var resourceStatus = this._resourcesStatus[resource.id];
                var status = null;
                if (resourceStatus) {
                    if (resourceStatus.status === "loading" )
                        return;
                }
                this._resourcesStatus[resource.id] = { status: "loading" };

                var self = this;
                if (resource.description.image) {
                    this._handleImageLoading(resource.description.image, 
                    function(image, id, ctx) {
                        var gl = ctx;                            
                        var convertedResource = delegate.convert(resource, image);

                        delete self._resourcesStatus[resource.id];

                        self._storeResource(resource.id, convertedResource);
                        delegate.resourceAvailable(convertedResource, gl);
                        self.fireResourceAvailable.call(self, resource.id);
                    }, ctx);
                }
            }
        },
        
        getResource: {
                value: function(resource, delegate, ctx) {

                var managedResource = this._getResource(resource.id);
                if (managedResource) {
                    return managedResource;
                }

                if (resource.type === "program") {
                    this._handleProgramLoading(resource, delegate, ctx);
                } else if (resource.type === "shader") {
                    this._handleShaderLoading(resource, delegate, ctx);
                } else if (resource.type === "image") {
                    this._handleImageLoading(resource, delegate, ctx);
                } else if (resource.type === "SAMPLER_2D") {
                    this._handleSampler2DLoading(resource, delegate, ctx);
                } else {
                    this._handleWrappedBufferViewResourceLoading(resource, delegate, ctx);
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

    if(root) {
        root.ResourceManager = WebGLTFResourceManager;
    }

    return ResourceManager;

}));

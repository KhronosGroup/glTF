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

(function (root, factory) {
    if (typeof exports === 'object') {
        // Node. Does not work with strict CommonJS, but
        // only CommonJS-like enviroments that support module.exports,
        // like Node.
        module.exports = factory(global);
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
            value: function(request) {
                return  ((request.range[0] === this.range[1]) ||
                         ((request.range[1]) === this.range[0]));
            }
        },

        mergeRequest: {
            value: function(request) {
                if (this.requests.length === 0) {
                    this.requests.push(request);
                    this.range = [request.range[0], request.range[1]];
                } else {
                    if (request.range[0] === this.range[1]) {
                        this.requests.push(request);
                        this.range[1] = request.range[1];
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

                requests.forEach(function(request) {
                    this.mergeRequest(request);
                }, this);
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
            value: function(requests, parent) {
                //insert before ?
                
                if (requests.range[1] <= this.content.range[0]) {
                    if ( (requests.range[1] === this.content.range[0])) {
                        if (requests.kind === "multi-parts")
                            this.content.mergeRequests(requests.requests);
                        else
                            this.content.mergeRequests(requests);
                        
                        if (this.left) {
                            if(this.content.canMergeRequest(this.left.content)) {
                                this.content.mergeRequests(this.left.content._requests);
                                this.left.remove();
                            }
                        }
                            
                        //console.log("requests:"+this.content.requests.length);
                        return null;
                    } else if (this.left) {
                        return this.left.insert(requests, this);
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
                            if(this.content.canMergeRequest(this.right.content)) {
                                this.content.mergeRequests(this.right.content._requests);
                                this.right.remove();
                            }
                        }
                        
                        //console.log("requests:"+this.content.requests.length);
                        return null;
                    } else if (this.right) {
                        return this.right.insert(requests, this);
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

        remove: {
            value: function() {

                if (!this.left  && !this.right) {
                    if (this.parent) {
                        if (this.parent.left === this) {
                            this.parent.left = null;
                        } else {
                            this.parent.right = null;
                        }
                    }
                    return null;
                } else {
                    //if right tree is null, move left in place of element
                    if (!this.right) {
                        if (this.parent) {
                            if (this.parent.left === this) {
                                this.parent.left = this.left;
                            } else {
                                this.parent.right = this.left;
                            }
                            this.left.parent = this.parent;
                        }
                        return this.left;
                    } else {
                        //otherwise take the minimum  of the right tree
                        var minNode = this.right;

                        while (minNode.left) {
                            minNode = minNode.left;
                        }

                        minNode.left = this.left;
                        if (this.left)
                            this.left.parent = minNode;

                        if (minNode.parent !== this) {
                            if (minNode.parent)
                                minNode.parent.left = minNode.right;
                            if (minNode.right) {
                                if (minNode.parent)
                                    minNode.right.parent = minNode.parent;
                            }

                            minNode.right = this.right;
                            this.right.parent = minNode;
                        }
                        
                        if (this.parent) {
                            if (this.parent.left === this) {
                                this.parent.left = minNode;
                            } else {
                                this.parent.right = minNode;
                            }
                        }
                        minNode.parent = this.parent;

                        return minNode;
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
        MAX_CONCURRENT_XHR: { value: 6 },
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
                //this._resourcesToBeProcessed = Object.create(LinkedList);
                this._resourcesBeingProcessedCount = 0;
            }
        },
    
        _storeResource: {
            enumerable: false,
            value: function(resourceID, resource) {
                if (!resourceID) {
                    debugger;
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
                xhr.setRequestHeader("If-Modified-Since", "Sat, 1 Jan 1970 00:00:00 GMT");                                                
                xhr.onload = function(e) {
                    if ((this.status == 200) || (this.status == 206)) {
                        if (request.kind === "multi-parts") {
                            request.requests.forEach( function(req_) {
                                var subArray = this.response.slice(req_.range[0] - request.range[0], req_.range[1] - request.range[0]);
                                delegate.resourceAvailable(req_, subArray);
                            }, this);
                        } else {
                            delegate.resourceAvailable(request, this.response);
                        }

                    } else {
                        delegate.handleError(WebGLTFResourceManager.XMLHTTPREQUEST_STATUS_ERROR, this.status);
                    }
                };
                xhr.send(null);
            }
        },
    
        _processNextResource: {
            value: function() {
                /*
                var nextNode = this._resourcesToBeProcessed.head;
                if (nextNode) {
                    var nextRequest = nextNode.content;


                    if (!this._resourcesStatus[nextRequest.id]) {
                        console("ERROR: should not reach");
                    }

                    this._handleRequest(nextRequest);
                } */


                if (this._requestTree) {
                    this._handleRequest(this._requestTree.content);
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
                        return;
                    node = resourceStatus.node;
                    status = resourceStatus.status;
                }
 
                //if (this._requestTree) {
                if ((this._resourcesBeingProcessedCount >= WebGLTFResourceManager.MAX_CONCURRENT_XHR) && (request.type !== "text")) {
                    if (!status) {     
                        //var listNode = Object.create(LinkedListNode);                
                        //listNode.init(request);
                        
                        var trNode = null;

                        var contRequests;

                        if (request.kind === "multi-parts") {
                             contRequests = Object.create(ContiguousRequests).initWithRequests(request.requests);
                        } else {
                             contRequests = Object.create(ContiguousRequests).initWithRequests([request]);                            
                        }

                        if (!this._requestTree) {
                            var rootTreeNode = Object.create(RequestTreeNode);
                            rootTreeNode.content = contRequests;
                            this._requestTree = rootTreeNode;
                            trNode = rootTreeNode;
                        } else {
                            trNode = this._requestTree.insert(contRequests);
                            //this._requestTree.checkConsistency();
                        }

                        if (request.kind ==="multi-parts") {
                            request.requests.forEach( function(req_) {
                                this._resourcesStatus[req_.id] =  { "status": "queued", "node": trNode };
                            }, this);

                        } else {
                            this._resourcesStatus[request.id] =  { "status": "queued", "node": trNode };
                        }

                        //this._resourcesStatus[request.id] =  { "status": "queued", "node": listNode };
                        //this._resourcesToBeProcessed.append(listNode);
                    }
                    return;
                }
            
                //if (request.delegate) {
                    var self = this;
                    var processResourceDelegate = {};

                    if (node && this._resourcesToBeProcessed) {
                        this._resourcesToBeProcessed.remove(node);
                    }
                    
                    if (node) {
                        
                        if (node === this._requestTree) {
                            this._requestTree = node.remove();
                        } else {
                            node.remove();
                        }
                        //if (this._requestTree)
                         //   this._requestTree.checkConsistency();

                    }

                    if (request.kind ==="multi-parts") {
                        request.requests.forEach( function(req_) {
                            this._resourcesStatus[req_.id] =  { "status": "loading" };
                        }, this);

                    } else {
                        this._resourcesStatus[request.id] =  { "status": "loading"};
                    }

                    processResourceDelegate.resourceAvailable = function(req_, res_) {
                        // ask the delegate to convert the resource, typically here, the delegate is the renderer and will produce a webGL array buffer
                        // this could get more general and flexbile by make an unique key with the id from the resource + the converted type (day "ARRAY_BUFFER" or "TEXTURE"..)
                        //, but as of now, this flexibily does not seem necessary.
                        var convertedResource = req_.delegate.convert(res_, req_.ctx);
                        self._storeResource(req_.id, convertedResource);
                        req_.delegate.resourceAvailable(convertedResource, req_.ctx);
                        self._resourcesBeingProcessedCount--;
                        
                        //console.log("delete:"+req_.id)
                        delete self._resourcesStatus[req_.id];

                        if (self._resourcesBeingProcessedCount <  WebGLTFResourceManager.MAX_CONCURRENT_XHR) {
                            //self._processNextResource();
                            setTimeout(self._processNextResource.bind(self), 100);
                        }

                    };
                
                    processResourceDelegate.handleError = function(errorCode, info) {
                        request.delegate.handleError(errorCode, info);
                    }

                    self._resourcesBeingProcessedCount++;
                    this._loadResource(request, processResourceDelegate);
                }
            //}
        },

        _handleAccessorResourceLoading: {
            value: function(accessor, delegate, ctx) {
                var range = [accessor.byteOffset ? accessor.byteOffset : 0 , (accessor.byteStride * accessor.count) + accessor.byteOffset];
                this._handleRequest({   "id" : accessor.id,
                                        "range" : range,
                                        "type" : accessor.buffer.description.type,
                                        "path" : accessor.buffer.description.path,
                                        "delegate" : delegate,
                                        "ctx" : ctx,
                                        "kind" : "single-part" }, null);
            }
        },

        _handleTypedArrayLoading: {
            value: function(typedArrayDescr, delegate, ctx) {
                //FIXME: extend testing to UInt8, 32, float and so on ...
                if (typedArrayDescr.type === "Uint16Array") {
                    var range = [typedArrayDescr.byteOffset, typedArrayDescr.byteOffset + ( typedArrayDescr.length * Uint16Array.BYTES_PER_ELEMENT)];

                    this._handleRequest({   "id":typedArrayDescr.id,
                                            "range" : range,
                                            "type" : typedArrayDescr.buffer.description.type,
                                            "path" : typedArrayDescr.buffer.description.path,
                                            "delegate" : delegate,
                                            "ctx" : ctx,
                                            "kind" : "single-part" }, null);
                }
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

        getResource: {
                value: function(resource, delegate, ctx) {

                var managedResource = this._getResource(resource.id);
                if (managedResource) {
                    return managedResource;
                }                

                if (resource.type === "accessor") {
                    this._handleAccessorResourceLoading(resource, delegate, ctx);
                } else if (resource.type === "program") {
                    this._handleProgramLoading(resource, delegate, ctx);
                } else if (resource.type === "shader") {
                    this._handleShaderLoading(resource, delegate, ctx);
                } else {
                    //FIXME: better testing
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

    if(root) {
        root.WebGLTFResourceManager = WebGLTFResourceManager;
    }

    return WebGLTFResourceManager;

}));

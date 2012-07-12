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

define(["backend/entry-manager", "backend/mesh", "backend/scene", "backend/node", "backend/camera", "backend/material", "backend/technique", "dependencies/gl-matrix"], 
    function(EntryManager, Mesh, Scene, Node, Camera, Material, Technique, glMatrix) {

    var Reader = Object.create(Object, {

        MESH: { value: "mesh" },
        MATERIAL: { value: "material" },
        TECHNIQUE: { value: "technique" },
        SHADER: { value: "shader" },
        SCENE: { value: "scene" },
        NODE: { value: "node" },
        CAMERA: { value: "camera" },
        VERTEX_ATTRIBUTES: { value: "vertexAttributes" },
        TYPE: { value: "type" },
        BUFFER : { value: "buffer" },

        _entryManager: { value: null, writable: true },
        
        _rootDescription: { value: null, writable: true },
                
        rootDescription: {
            set: function(value) {
                this._rootDescription = value;
            },
            get: function() {
                return this._rootDescription;
            }
        },

        baseURL: { value: null, writable: true },

        entryManager: {
            get: function() {            
                return this._entryManager;
            }, 
            set: function(value) {
                this._entryManager = value;
            }
        },

        //detect absolute path following the same protocol than window.location
        _isAbsolutePath: {
            value: function(path) {
                var isAbsolutePathRegExp = new RegExp("^"+window.location.protocol, "i");

                return path.match(isAbsolutePathRegExp) ? true : false;
            }
        },

        resolvePathIfNeeded: {
            value: function(path) {
                if (this._isAbsolutePath(path)) {
                    return path;
                } else {
                    var pathComponents = path.split("/");
                    var lastPathComponent = pathComponents.pop();
                    return this.baseURL + lastPathComponent;
                }
            }
        },

        _resolveBufferPaths: {
            value: function() {
                var buffers = this.json.buffers;
                if (buffers) {
                    var bufferKeys = Object.keys(buffers);
                    bufferKeys.forEach( function(bufferKey) {
                        var buffer = buffers[bufferKey];
                        buffer.path = this.resolvePathIfNeeded(buffer.path);
                    }, this);
                }
            }
        },

        _loadJSONIfNeeded: {
            enumerable: true,
            value: function(callback) {        
                var self = this;

                //FIXME: handle error
                if (!this._json)  {
                    var baseURL;
                    var parser = document.createElement("a");
                    
                    parser.href = window.location.href;
                    var port = "";
                    if (parser.port)
                        port += ":"+parser.port;

                    baseURL = parser.protocol+"//"+parser.hostname+ port;                    
                    if (parser.pathname.charAt(parser.pathname.length - 1) !== '/') {
                        var filebase = parser.pathname.split("/");
                        filebase.pop();
                        baseURL += filebase.join("/") + "/";  
                    } else {
                        baseURL += parser.pathname;
                    }

                    if (!this._isAbsolutePath(this._path)) {
                        //we don't want the last component of the path
                        var pathBase = this._path.split("/");
                        if (pathBase.length > 1) {
                            pathBase.pop();
                            baseURL += pathBase.join("/") + "/";
                        }
                    } 
                    this.baseURL = baseURL;

                    var jsonfile = new XMLHttpRequest();
                    jsonfile.open("GET", this._path, true);
                    jsonfile.onreadystatechange = function() {
                        if (jsonfile.readyState == 4) {
                            if (jsonfile.status == 200) {
                                self.json = JSON.parse(jsonfile.responseText);     
                                if (callback) {
                                    callback(self.json);
                                }
                            }
                        }
                    };
                    jsonfile.send(null);
               } else {
                    if (callback) {
                        callback(this.json);
                    }
                }
            }
        },

        _json: {
            value: null,
            writable: true
        },

        json: {
            enumerable: true,
            get: function() {        
                return this._json;
            },
            set: function(value) {
                if (this._json !== value) {
                    this._json = value; 
                    this._resolveBufferPaths();                   
                }
            }
        },
    
        _path: {
            value: null,
            writable: true
        },

        _commonInit: {
            value: function() {
                this.entryManager = Object.create(EntryManager);
                this.entryManager.init();
            }
        },

        initWithPath: {
            value: function(path) {
                this._path = path;
                this._json = null;
                this._commonInit();
                return this;
            }
        },
    
        initWithJSON: {
            value: function(json, baseURL) {
                this.json = json;
                this.baseURL = baseURL;
                if (!baseURL) {
                    console.log("WARNING: no base URL passed to Reader:initWithJSON");
                }
                this._commonInit();
                return this;
            }
        },
      
        getEntryFromRootDescription: {
            value: function (entryID) {
                var entryDescription = this.rootDescription[entryID];
                if (!entryDescription) {
                    //FIXME: infer directly the main (higher level) property out of the entryDescription type
                    var entryLevels = ["scenes", "meshes", "nodes", "lights", "materials", "buffers", "cameras", "techniques", "shaders"];
                
                    for (var i = 0 ; !entryDescription && i < entryLevels.length ; i++) {
                        var entries = this.rootDescription[entryLevels[i]];
                        if (entries) {
                            entryDescription = entries[entryID];
                        }
                    }
                }
            
                return entryDescription;
            }
        },
            
        _readEntry: {
            enumerable: false,
            value: function(entryID, delegate, userInfo) { 
                var entryManager = this.entryManager;
                var entryDescription = this.getEntryFromRootDescription(entryID);
                if (entryDescription) {
                    var entryDelegate = {};
                                        
                    entryDelegate.readCompleted = function(entryType, entry, userInfo) {
                        entryManager.storeEntry(entry);
                        delegate.readCompleted(entryType, entry, userInfo);
                    } 
                    var entry = null;
                    var type = entryDescription.type;
                    if (entryManager.containsEntry(entryID)) {
                        delegate.readCompleted(entryDescription.type, entryManager.getEntry(entryID), useInfo);                    
                        return;
                    }

                    //TODO: make a factory..
                    if (type === this.MESH) {
                        entry = Object.create(Mesh);
                    } else if (type === this.SCENE) {
                        entry = Object.create(Scene);
                    } else if (type === this.NODE) {                
                        entry = Object.create(Node);
                    } else if (type === this.CAMERA) {  
                        entry = Object.create(Camera);
                    } else if (type === this.MATERIAL) {
                        entry = Object.create(Material);
                    } else if (type == this.TECHNIQUE) {
                        entry = Object.create(Technique);
                    } else if (type == this.SHADER) {
                        //local handling of shaders
                        entryDescription.id = entryID;
                        delegate.readCompleted("shader", entryDescription, userInfo);
                        return;
                    }


                    if (entry) {
                        entry.init();
                        entry.id = entryID;
                        entry.read(entryID, entryDescription, entryDelegate, this, userInfo);
                    }
                
                } else {
                    delegate.handleError(this.NOT_FOUND);
                }
            }    
        },
    
        _readEntries: {
            enumerable: false,
            value: function(entryIDs, delegate, userInfo) {
            
                var self = this;
                var count = entryIDs.length;
                var idx = 0;
                var allEntries = [];
                var entryDelegate = {};
            
                entryDelegate.readCompleted = function(entryType, entry, userInfo) {
                    //console.log("readCompleted for entry:"+entryType+" id:"+entry.id);
                
                    allEntries[idx++] = entry;
                    if (idx == count) {
                        if (delegate) {
                            delegate.readCompleted("entries", allEntries, userInfo);
                        }
                    } else {
                        self._readEntry(entryIDs[idx], entryDelegate, userInfo);
                    }                
                }
            
                if (count > 0) 
                    this._readEntry(entryIDs[0], entryDelegate, userInfo);
            }
        },
    
        /* load JSON and assign it as description to the reader */
        _buildReader: {
            value: function(callback) {
                var self = this;
                function JSONReady(json) {
                    self.rootDescription = json;
                    if (callback) 
                        callback(this);
                }

                this._loadJSONIfNeeded(JSONReady);
            }
        },
    
        readEntry: {
            enumerable: false,
            value: function(entryID, delegate, userInfo) {
            
                var self = this;
                this._buildReader(function readerReady(reader) {
                    self._readEntry(entryID, delegate,  userInfo);
                });
            }    
        },
    
        readEntries: {
            enumerable: true,
            value: function(entryIDs, delegate, userInfo) {

                var self = this;
                this._buildReader(function readerReady(reader) {
                    self._readEntries(entryIDs, delegate, userInfo);
                });            
            }
        }
    });
    
        return Reader;
    }
);

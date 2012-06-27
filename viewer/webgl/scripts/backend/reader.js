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

define(["backend/entry-manager", "backend/mesh", "backend/scene", "backend/node", "dependencies/gl-matrix"], function(EntryManager, Mesh, Scene, Node) {

    var Reader = Object.create(Object, {

        MESH: { value: "mesh" },
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
                this._entryManager = null;
            },
            get: function() {
                return this._rootDescription;
            }
        },
    
        entryManager: {
            get: function() {
                if (!this._entryManager) {
                    this._entryManager = Object.create(EntryManager);
                    this._entryManager.init();
                }
            
                return this._entryManager;
            }
        },

        _loadJSONIfNeeded: {
            enumerable: true,
            value: function(callback) {
                var self = this;
            
                //FIXME: handle error
                if (!this._json)  {
                    var jsonfile = new XMLHttpRequest();
                    jsonfile.open("GET", this._path, true);
                    jsonfile.onreadystatechange = function() {
                        if (jsonfile.readyState == 4) {
                            if (jsonfile.status == 200) {
                                self._json = JSON.parse(jsonfile.responseText);
                                if (callback) {
                                    callback(self._json);
                                }
                            }
                        }
                    };
                    jsonfile.send(null);
               } else {
                    if (callback) {
                        callback(this._json);
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
                this._json = value;
            }
        },
    
        _path: {
            value: null,
            writable: true
        },

        initWithPath: {
            value: function(path) {
                this._path = path;
                this._json = null;
            }
        },
    
        initWithJSON: {
            value: function(json) {
                this._json = json;
            }
        },
      
        getEntryFromRootDescription: {
            value: function (entryID) {
                var entryDescription = this.rootDescription[entryID];
                if (!entryDescription) {
                    var entryLevels = ["scenes", "meshes", "nodes", "lights", "materials", "buffers"];
                
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
                    };
                    var entry = null;
                    var type = entryDescription.type;
                    if (entryManager.containsEntry(entryID)) {
                        delegate.readCompleted(entryDescription.type, entryManager.getEntry(entryID), useInfo);
                        return;
                    } else if (type === this.MESH) {
                        entry = Object.create(Mesh);
                        //entry.init();
                    } else if (type === this.SCENE) {
                        entry = Object.create(Scene);
                        entry.init();
                    } else if (type === this.NODE) {
                        entry = Object.create(Node);
                        entry.init();
                    }
                    if (entry) {
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
                };
            
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

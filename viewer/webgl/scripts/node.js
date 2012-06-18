define(["base","utilities"], function(Base, Utilities) {

	var Node = Object.create(Base, {

    	_children: { value: null, writable: true },
    
	    children: {
    	    get: function() {         
        	    return this._children; 
	        },
        	set: function(value) { 
            	this._children = value; 
        	}
    	},

	    _id: { value: null, writable: true },
    
    	id: {
        	get: function() { 
            	return this._id; 
        	},
        	set: function(value) { 
            	this._id = value; 
        	}
    	},
    
    	_computeBBOXIfNeeded: {
        	enumerable: false,
        	value: function() {
            	if (!this._boundingBox) {
                
                	var meshes = this._properties["meshes"];                
                	var count = this.meshes.length;
                	if (count > 0) {
                    	var bbox = this.meshes[0].boundingBox;
                    	//bbox = Utilities.transformBBox(bbox, this.transform);                   
                    	if (bbox) {
                        	var i;
                        	for (i = 1 ; i <  count ; i++) {
                            	var aBBox = this.meshes[i].boundingBox;
                            	if (aBBox) { //it could be not here here as we are loading everything asynchronously

                                	//aBBox = Utilities.transformBBox(aBBox, this.transform);
                                	bbox = Utilities.mergeBBox(bbox, aBBox);
                            	}
                        	}
                        
                        	this._boundingBox = Utilities.transformBBox(bbox, this.transform);
                    	}
                	}                
            	}
        	}
    	},
        
    	_boundingBox: {
        	enumerable: false,
        	value: null,
        	writable: true
    	},
    
    	boundingBox: {
        	enumerable: true,
        	get: function() {
            	this._computeBBOXIfNeeded();
            	return this._boundingBox;
        	},
        	// we let the possibility to override by hand the bounding volume.
        	set: function(value) {
            	this._boundingBox = value;
        	}
    	},

 	   meshesDidChange: {
        	value: function(meshes) {
            	this._boundingBox = null; //invalidate bounding box 
        	}
    	},

    	init: {
        	value: function() {
        		this.__Base_init();
            	this._children = [];
            	this._transform = mat4.identity();
            	this._properties["meshes"] = [];
            	
            	var self = this;
    	        this._properties["meshes"].push = function(data) {
        	        var result = Array.prototype.push.call(this, data);
           	     	self.meshesDidChange(this);
           	     	return result;
            	}
            
	            this._properties["cameras"] = [];
   	         	this._properties["lights"] = [];
        	}
    	},

    	getPropertyArrayNamed: {
       		value: function(name) {        
            	return this._properties[name]
        	}
    	},
        
	    _transform: { value: null, writable: true },

   	 	transform: {
        	get: function() { 
            	return this._transform; 
        	},
       	 	set: function(value) { 
            	this._transform = value; 
        	}
    	},

    	meshes: {
        	get: function() { 
            	return this.getPropertyArrayNamed("meshes"); 
        	},
        	set: function(value) { 
            	this._properties["meshes"] = value;
            	this.meshesDidChange(value);
        	}
    	},
    
    	cameras: {
        	get: function() { 
            	return this.getPropertyArrayNamed("cameras"); 
        	},
        	set: function(value) { 
            	this._properties["cameras"] = value;
        	}
    	},

    	lights: {
        	get: function() { 
            	return this.getPropertyArrayNamed("lights"); 
        	},
        	set: function(value) { 
            	this._properties["lights"] = value;
        	}
    	},
    
    	apply: {
        	value: function( callback, recurse, ctx) {
    
            	if (callback) {
                	ctx = callback(this, ctx);
                
                	if (recurse) {
                    	this.children.forEach( function(node) {
                        	node.apply(callback, recurse, ctx);
                    	}, this);
                	}
            	}
        	}
    	},
    
    	read: {
        	enumerable: true,
        	value: function(entryID, nodeDescription, delegate, reader, userInfo) {

            	var childIndex = 0;
            	var self = this;
            	var childrenCount = nodeDescription.children ? nodeDescription.children.length : 0;
            	if (!entryID) {
                	debugger;
            	}
            	this.id = entryID;
            	this.name = nodeDescription.name;
            
            	if (nodeDescription.matrix) {
                	this.transform = mat4.transpose(mat4.create(nodeDescription.matrix));
           	 	}

            	// load mesh info (but not the binaries yet)
            	var meshDelegate = {};
            	meshDelegate.readCompleted = function(entryType, entry, userInfo) {
                	self.meshes.push(entry);
            	}

            	if (nodeDescription.mesh) {
                	reader._readEntry(nodeDescription.mesh, meshDelegate, userInfo);            
            	}

            	// load meshes info (but not the binaries yet)
            	var meshesDelegate = {};
            	meshesDelegate.readCompleted = function(entryType, entry, userInfo) {
                	entry.forEach( function(mesh) {
                    	self.meshes.push(mesh);
                	}, this);
            	}

            	if (nodeDescription.meshes) {
                	reader._readEntries(nodeDescription.meshes, meshesDelegate, userInfo);            
            	}
            
            	// load node
            	var entryNodeDelegate = {};
            	entryNodeDelegate.readCompleted = function(entryType, entry, userInfo) {
                	//debugger;
                	self.children.push(entry);
                	childIndex++;
                	if (delegate && (childIndex == childrenCount)) {
                    	delegate.readCompleted(entryType, self, userInfo);
                	} else {
                    	reader._readEntry(nodeDescription.children[childIndex], entryNodeDelegate, userInfo);
                	}
            	}
            
            	if (childrenCount > 0) {
                	reader._readEntry(nodeDescription.children[childIndex], entryNodeDelegate, userInfo);
            	} else if (delegate) {
                	delegate.readCompleted("node", this, userInfo);
            	}       
        	}
    	}    
	});
		return Node;
	}
);

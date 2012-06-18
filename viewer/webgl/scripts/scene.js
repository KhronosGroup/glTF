define(["base", "node"], function(Base, Node) {

	var Scene = Object.create(Base, {

    	_rootNode: { value : null, writable: true },
    
    	rootNode: {
        	get: function() {
            	return this._rootNode;
        	},
        	set: function(value) {
            	this._rootNode = value;
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
    
    	init: {
        	value: function() {
        		this.__Base_init();
            	this.rootNode = Object.create(Node);
            	this.rootNode.init();
        	}
    	},

    	read: {
        	enumerable: true,
        	value: function(entryID, sceneDescription, delegate, reader, userInfo) {
            	var self = this;
            	this.id = entryID;
            	this.name = sceneDescription.name;
            
       	     	if (!sceneDescription.node) {
                	// FIXME: todo
            	} else {
                
                	var entryDelegate = {};
                	entryDelegate.readCompleted = function(entryType, entry, userInfo) {
                    	self.rootNode = entry;
                    	if (delegate) {
                        	delegate.readCompleted("scene", self, userInfo);
                    	}
                	}

                	reader._readEntry(sceneDescription.node, entryDelegate, userInfo);
            	}
        	}
    	}
	});
	
		return Scene;
	}
);

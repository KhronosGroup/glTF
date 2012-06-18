define(function() {

	var ResourceDescription = Object.create(Object, {

    	_description: { value: null, writable: true },

	    _id: { value: null, writable: true },
            
    	init: {
        	enumerable: true,
        	value: function(id, description) {
            	this._id = id;
            	this._description = description;
        	}
    	},
    
	    description: {
    	    enumerable: true,
        	get: function() {
       	 		return this._description;
        	}
    	},
    
	    id: {
    	    enumerable: true,
        	get: function() {
            	return this._id;
        	}
    	}

	});
	
		return ResourceDescription;
	}
);

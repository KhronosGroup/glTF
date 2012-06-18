define(["base"], function(Base) {

	var Camera = Object.create(Base, {

    	_matrix: { value: null, writable: true },
    
    	matrix: {
        	get: function() { 
            	return this._matrix; 
        	},
        	set: function(value) { 
            	this._matrix = value; 
        	}
    	},
    
 	   init: {
    	    value: function() {
        		this.__Base_init();
        		this._matrix = mat4.identity();
        	}
    	}

	});
	
		return Camera;
	}
);

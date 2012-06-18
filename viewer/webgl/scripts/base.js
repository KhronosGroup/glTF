define(function() {

	var Base = Object.create(Object, {

    	_properties: { value: null, writable: true },

	    properties: {
    	    get: function() {         
        	    return this._properties; 
	        },
    	    set: function(value) { 
        	    this._properties = value; 
        	}
    	},

		__Base_init: {
			value: function() {
				this._properties = {};
			}
		}

	});
	
		return Base;
	}
);

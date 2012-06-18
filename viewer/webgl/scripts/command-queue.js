define(function() {

	var CommandQueue = Object.create(Object, {

    	_queue: { value: null, writable: true },
    
	    enqueuePrimitive: {
    	    value: function(primitive) {
        	    this._queue.push(primitive);
        	}
    	},

	    execute: {
    	    value: function(renderer) {
            
        	    this._queue.forEach( function(primitive) {
            	    renderer.renderPrimitive(primitive);
            	}, this);
    	    }
    	},
    
		reset: {
    	    value: function() {
        	    this._queue = [];
       		}
    	}
	});
	
		return CommandQueue;
	}
);

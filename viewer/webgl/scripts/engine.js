define(["renderer","pass", "command-queue"], function(Renderer, Pass, CommandQueue) {

	var Engine = Object.create(Object, {

    	_commandQueue: { value: null, writable: true },

	    _renderer: { value: null, writable: true },

    	_rootPass: { value: null, writable: true },
    
	    commandQueue: {
    	    get: function() {
        	    return this._commandQueue;
	        },
    	    set: function(value) {
        	    this._commandQueue = value;
	        }
    	},

	    rootPass: {
    	    get: function() {
        	    return this._rootPass;
	        },
    	    set: function(value) {
        	    this._rootPass = value;
	        }
    	},
    
    	renderer: {
        	get: function() {
            	return this._renderer;
	        },
    	    set: function(value) {
        	    this._renderer = value;
        	}
	    },
    
    	init: { 
        	value: function( webGLContext, options) {
            	this.renderer = Object.create(Renderer);
            	this.rootPass = Object.create(Pass);
            	this.commandQueue = Object.create(CommandQueue);
        
	            this.renderer.webGLContext = webGLContext;      
    	    }
    	},
    
	    render: {
    	    value: function() {
        	    this.commandQueue.reset();
            	this.renderer.resetStates();
            	this.rootPass.render(this);
            	this.commandQueue.execute(this.renderer);
        	}
    	}

	});
	
		return Engine;
	}
);

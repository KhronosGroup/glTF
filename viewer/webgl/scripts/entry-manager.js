define(function() {

	var EntryManager = Object.create(Object, {

    	_entries: {
        	enumerable: false,
        	value: null,
        	writable: true
    	},
    
    	removeAllEntries: {
        	value: function() {
            	this._entries = {};
        	}
    	},
    
	    //manage entries
    	containsEntry: {
        	enumerable: false,
        	value: function(entry) {
            	return this._entries[entry.id] ? true : false;
        	}
    	},
    
	    storeEntry: {
    	    enumerable: false,
        	value: function(entry) {
            	if (!entry.id) {
                	debugger;
                	console.log("ERROR: entry does not contain id, cannot store");
                	return;
            	}
        
	            if (this.containsEntry[entry.id]) {
    	            console.log("WARNING: entry:"+entry.id+" is already stored, overriding");
        	    }
            
	            this._entries[entry.id] = entry;
    	    }
    	},
    
	    getEntry: {
    	    enumerable: false,
        	value: function(entryID) {
            	return this._entries[entryID];
        	}
    	},
    	
    	init: {
    		value: function() {
				this._entries = {};    		
    		}
    	}

	});
	
		return EntryManager;
	}
);

// Copyright (c) 2011-2012, Motorola Mobility, Inc.
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

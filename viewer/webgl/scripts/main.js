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

require( ["backend/utilities", "backend/node", "backend/camera", "backend/view", "backend/glsl-program", "backend/reader", "dependencies/gl-matrix"],
    function(Utilities, Node, Camera, View, GLSLProgram, Reader) {
		var Test = Object.create(Object, {
			run: {
				value: function() {
    				// camera
				    var camera = Object.create(Camera);
				    camera.init();
				    camera.matrix = mat4.perspective(45, 1., 0.1, 2.0);    
    
			    	// node
    				var cameraNode = Object.create(Node);
    				cameraNode.init();
    				cameraNode.id = "Camera";
    				cameraNode.cameras.push(camera);
    
				    var glView = Object.create(View);
    				glView.init("gl-canvas");    
    
	    			var readerDelegate = {};
    				readerDelegate.readCompleted = function (key, value, userInfo) {
        				console.log("key:"+key+" value:"+value.length);
        				if (key === "entries") {
    	    		    	var scene = value[0];
					    	scene.rootNode.children.push(cameraNode);
         	        		glView.scene = scene;
		        		} 
    				};
    
				    var angle = 0;    
	    			var viewDelegate = {};
   					viewDelegate.willDraw = function(view) {
        			if (!view.scene)
            			return;
                    
			        /* ------------------------------------------------------------------------------------------------------------
            		    draw gradient 
		             ------------------------------------------------------------------------------------------------------------ */        
          			view.engine.renderer.bindedProgram = null;

	            	var orthoMatrix = mat4.ortho(-1, 1, 1.0, -1, 0, 1000);
    	       	 	var gl = view.getWebGLContext();
            
		            gl.clear(gl.DEPTH_BUFFER_BIT | gl.COLOR_BUFFER_BIT);
       			 	gl.disable(gl.DEPTH_TEST);
            		gl.disable(gl.CULL_FACE);
            
            		if (!this._gradientProgram) {
                		this._gradientProgram = Object.create(GLSLProgram);
                
		                var vertexShader = 	"precision highp float;" +
        		                            "attribute vec3 vert;"  +
                		                    "attribute vec3 color;"  +
                               			     "uniform mat4 u_projMatrix; " +
                        		            "varying vec3 v_color;"  +
                                    		"void main(void) { " +
                                    		"v_color = color;" +
                                    		"gl_Position = u_projMatrix * vec4(vert,1.0); }" 
                
                		var fragmentShader = 	"precision highp float;" +
                        		            	"varying vec3 v_color;"  +
							                    " void main(void) { " +
                    							" gl_FragColor = vec4(v_color, 1.); }";
                
                		this._gradientProgram.initWithShaders( { "x-shader/x-vertex" : vertexShader , "x-shader/x-fragment" : fragmentShader } );
                		if (!this._gradientProgram.build(gl))
                    		console.log(this._gradientProgram.errorLogs);
 		           }
            
        		   if (!this.vertexBuffer) {
            			/*
               				2/3----5
		                 	| \   |   
         		         	|  \  |
                         	|   \ |
                		 	0----1/4
            			*/
 		               	var vertices = [
        	            	-1.0,-1, 0.0,       1.0, 1.0, 1.0,
            	        	1.0,-1, 0.0,        1.0, 1.0, 1.0,
                	    	-1.0, 1.0, 0.0,     0.3, 0.4, 0.5,

	                	    -1.0, 1.0, 0.0,     0.3, 0.4, 0.5,
    	                	1.0,-1, 0.0,        1.0, 1.0, 1,
        	            	1.0, 1.0, 0.0,      0.3, .4, 0.5
	            	    ];

    	            	// Init the buffer
        	        	this.vertexBuffer = gl.createBuffer();
            	    	gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexBuffer);
                		gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
            		}
	            	gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexBuffer);

    	            gl.disableVertexAttribArray(0);
        	        gl.disableVertexAttribArray(1);
                	gl.disableVertexAttribArray(2);
                	gl.disableVertexAttribArray(3);
        			var attributeLocation = this._gradientProgram.getLocationForSymbol("vert");
        			if (typeof attributeLocation !== "undefined") {
            			gl.enableVertexAttribArray(attributeLocation);
            			gl.vertexAttribPointer(attributeLocation, 3, gl.FLOAT, false, 24, 0);
			        }
        			attributeLocation = this._gradientProgram.getLocationForSymbol("color");
			        if (typeof attributeLocation !== "undefined") {
           				gl.enableVertexAttribArray(attributeLocation);
		            	gl.vertexAttribPointer(attributeLocation, 3, gl.FLOAT, false, 24, 12);
        			}
        
        			view.engine.renderer.bindedProgram = this._gradientProgram;
        
			        var projectionMatrixLocation = this._gradientProgram.getLocationForSymbol("u_projMatrix");
        			if (projectionMatrixLocation) {
            			this._gradientProgram.setValueForSymbol("u_projMatrix",orthoMatrix);
        			}
        
			        this._gradientProgram.commit(gl);
			        gl.drawArrays(gl.TRIANGLES, 0, 6);
        			gl.disableVertexAttribArray(attributeLocation);

       				/* ------------------------------------------------------------------------------------------------------------
            		 	Update scene 
           			 ------------------------------------------------------------------------------------------------------------ */        
       				var node = view.scene.rootNode.children[0];
       				node.transform = mat4.identity();
        			var sceneBBox = view.sceneBBox;
        
        	
			        var sceneSize = [   (sceneBBox[1][0] - sceneBBox[0][0]) , 
        				                (sceneBBox[1][1] - sceneBBox[0][1]) , 
                        			    (sceneBBox[1][2] - sceneBBox[0][2]) ];

					//size to fit
        			var scaleFactor = sceneSize[0] > sceneSize[1] ? sceneSize[0] : sceneSize[1];
        			scaleFactor = sceneSize[2] > scaleFactor ? sceneSize[2] : scaleFactor;
        			
        			scaleFactor = 1.0 / scaleFactor;
        
        			var scaleMatrix = mat4.scale(mat4.identity(), [scaleFactor, scaleFactor, scaleFactor]);
			        var translation = mat4.translate(scaleMatrix, [ 
            							((-sceneSize[0] / 2) - sceneBBox[0][0] ) , 
							            ((-sceneSize[1] / 2) - sceneBBox[0][1] ) , 
							            ((-sceneSize[2] / 2) - sceneBBox[0][2] ) ]);
        
			        var translation2 = mat4.translate(mat4.identity(), [ 0 , 0 , - 1.5 ]);
        
			        var rotationa = mat4.rotate(mat4.identity(), -1.6, [1.0, 0.0, 0.0]);
        			var rotationb = mat4.rotate(mat4.identity(), 0.2, [0.0, 0.0, 1.0]);
        			var rotation = mat4.rotate(mat4.identity(), angle, [0.0, 1.0, 0.0]);
        			mat4.multiply(rotationb, rotationa , rotationb); 
			        mat4.multiply(rotation, rotationb , rotation); 

			        var centeredTr = mat4.create();
			        mat4.multiply(rotation, translation , centeredTr); 
			        mat4.multiply(translation2 , centeredTr, node.transform); 

			        angle += 0.02;
				};

			    viewDelegate.didDraw = function(view) {
		    	};
    
   				glView.delegate = viewDelegate;
    
			    var reader = Object.create(Reader);
    			//reader.initWithPath("seymour_plane_triangles.json");
    			reader.initWithPath("output.json");
    
			   // reader.readEntries(["planeShape-lib","propShape-lib"], readerDelegate, null);    
    			reader.readEntries(["defaultScene"], readerDelegate, null);    
			}
		}
	});  
		
        Test.run();
    }
);

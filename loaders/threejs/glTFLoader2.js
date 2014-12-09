/**
 * @author Tony Parisi / http://www.tonyparisi.com/
 */


THREE.glTFLoader = function ( container, showStatus ) {
	this.container = container;
    THREE.Loader.call( this, showStatus );
}

THREE.glTFLoader.prototype = new THREE.Loader();
THREE.glTFLoader.prototype.constructor = THREE.glTFLoader;

THREE.glTFLoader.prototype.load = function( url, callback ) {

	// Utilities

    function RgbArraytoHex(colorArray) {
        if(!colorArray) return 0xFFFFFFFF;
        var r = Math.floor(colorArray[0] * 255),
            g = Math.floor(colorArray[1] * 255),
            b = Math.floor(colorArray[2] * 255),
            a = 255;

        var color = (a << 24) + (r << 16) + (g << 8) + b;

        return color;
    }
    
    function convertAxisAngleToQuaternion(rotations, count, yup)
    {
    	var q = new THREE.Quaternion;
    	var axis = new THREE.Vector3;
    	var euler = new THREE.Vector3;
    	
    	var i;
    	for (i = 0; i < count; i++) {
    		axis.set(rotations[i * 4], rotations[i * 4 + 1],
    				rotations[i * 4 + 2]).normalize();
    		var angle = rotations[i * 4 + 3];
    		q.setFromAxisAngle(axis, angle);
    		rotations[i * 4] = q.x;
    		rotations[i * 4 + 1] = q.y;
    		rotations[i * 4 + 2] = q.z;
    		rotations[i * 4 + 3] = q.w;
    	}
    }

    function componentsPerElementForGLType(glType) {
        switch (glType) {
            case "FLOAT" :
            case "UNSIGNED_BYTE" :
            case "UNSIGNED_SHORT" :
                return 1;
            case "FLOAT_VEC2" :
                return 2;
            case "FLOAT_VEC3" :
                return 3;
            case "FLOAT_VEC4" :
                return 4;
            default:
                return null;
        }
    }


    function LoadTexture(src) {
        if(!src) { return null; }
        return THREE.ImageUtils.loadTexture(src);
    }

    // Geometry processing

    var ClassicGeometry = function() {

    	this.geometry = new THREE.BufferGeometry;
        this.totalAttributes = 0;
        this.loadedAttributes = 0;
        this.indicesLoaded = false;
        this.finished = false;

        this.onload = null;

        this.uvs = null;
        this.indexArray = null;
    };

    ClassicGeometry.prototype.constructor = ClassicGeometry;

    ClassicGeometry.prototype.checkFinished = function() {
        if(this.indexArray && this.loadedAttributes === this.totalAttributes) {
            // Build indexed mesh
            var geometry = this.geometry;
            geometry.attributes.index = {
            		itemSize: 1,
            		array : this.indexArray
            };

			var offset = {
					start: 0,
					index: 0,
					count: this.indexArray.length
				};

			geometry.offsets.push( offset );
            
            // Allow Three.js to calculate some values for us
//            geometry.computeCentroids();
//           if(!normals) {
//               geometry.computeFaceNormals();
//            }

            geometry.computeBoundingSphere();
            this.finished = true;

            if(this.onload) {
                this.onload();
            }
        }
    };

    // Delegate for processing index buffers
    var IndicesDelegate = function() {};

    IndicesDelegate.prototype.handleError = function(errorCode, info) {
        // FIXME: report error
        console.log("ERROR(IndicesDelegate):"+errorCode+":"+info);
    };

    IndicesDelegate.prototype.convert = function(resource, ctx) {
        return new Uint16Array(resource, 0, ctx.indices.count);
    };

    IndicesDelegate.prototype.resourceAvailable = function(glResource, ctx) {
        var geometry = ctx.geometry;
        geometry.indexArray = glResource;
        geometry.checkFinished();
        return true;
    };

    var indicesDelegate = new IndicesDelegate();

    var IndicesContext = function(indices, geometry) {
        this.indices = indices;
        this.geometry = geometry;
    };
    
    // Delegate for processing vertex attribute buffers
    var VertexAttributeDelegate = function() {};

    VertexAttributeDelegate.prototype.handleError = function(errorCode, info) {
        // FIXME: report error
        console.log("ERROR(VertexAttributeDelegate):"+errorCode+":"+info);
    };

    VertexAttributeDelegate.prototype.convert = function(resource, ctx) {
        return resource;
    };




    VertexAttributeDelegate.prototype.resourceAvailable = function(glResource, ctx) {
        var geom = ctx.geometry;
        var attribute = ctx.attribute;
        var semantic = ctx.semantic;
        var floatArray;
        var i, l;
        var nComponents;
        //FIXME: Float32 is assumed here, but should be checked.

        if(semantic == "POSITION") {
            // TODO: Should be easy to take strides into account here
            floatArray = new Float32Array(glResource, 0, attribute.count * componentsPerElementForGLType(attribute.type));
            geom.geometry.attributes.position = {
            		itemSize: 3,
            		array : floatArray
            };
        } else if(semantic == "NORMAL") {
            floatArray = new Float32Array(glResource, 0, attribute.count * componentsPerElementForGLType(attribute.type));
            geom.geometry.attributes.normal = {
            		itemSize: 3,
            		array : floatArray
            };
        } else if ((semantic == "TEXCOORD_0") || (semantic == "TEXCOORD" )) {
        	
        	nComponents = componentsPerElementForGLType(attribute.type);
            floatArray = new Float32Array(glResource, 0, attribute.count * nComponents);
            // N.B.: flip Y value... should we just set texture.flipY everywhere?
            for (i = 0; i < floatArray.length / 2; i++) {
            	floatArray[i*2+1] = 1.0 - floatArray[i*2+1];
            }
            geom.geometry.attributes.uv = {
            		itemSize: nComponents,
            		array : floatArray
            };
        }
        geom.loadedAttributes++;
        geom.checkFinished();
        return true;
    };

    var vertexAttributeDelegate = new VertexAttributeDelegate();

    var VertexAttributeContext = function(attribute, semantic, geometry) {
        this.attribute = attribute;
        this.semantic = semantic;
        this.geometry = geometry;
    };

    var Mesh = function() {
        this.primitives = [];
        this.loadedGeometry = 0;
        this.onCompleteCallbacks = [];
    };

    Mesh.prototype.addPrimitive = function(geometry, material) {
        var self = this;
        geometry.onload = function() {
            self.loadedGeometry++;
            self.checkComplete();
        };
        this.primitives.push({
            geometry: geometry,
            material: material,
            mesh: null
        });
    };

    Mesh.prototype.onComplete = function(callback) {
        this.onCompleteCallbacks.push(callback);
        this.checkComplete();
    };

    Mesh.prototype.checkComplete = function() {
        var self = this;
        if(this.onCompleteCallbacks.length && this.primitives.length == this.loadedGeometry) {
            this.onCompleteCallbacks.forEach(function(callback) {
                callback(self);
            });
            this.onCompleteCallbacks = [];
        }
    };

    Mesh.prototype.attachToNode = function(threeNode) {
        // Assumes that the geometry is complete
        this.primitives.forEach(function(primitive) {
            /*if(!primitive.mesh) {
                primitive.mesh = new THREE.Mesh(primitive.geometry, primitive.material);
            }*/
            var threeMesh = new THREE.Mesh(primitive.geometry.geometry, primitive.material);
            primitive.material.side = THREE.FrontSide;
            threeMesh.castShadow = true;
            threeNode.add(threeMesh);
        });
    };

    // Delegate for processing animation parameter buffers
    var AnimationParameterDelegate = function() {};

    AnimationParameterDelegate.prototype.handleError = function(errorCode, info) {
        // FIXME: report error
        console.log("ERROR(AnimationParameterDelegate):"+errorCode+":"+info);
    };

    AnimationParameterDelegate.prototype.convert = function(resource, ctx) {
    	var parameter = ctx.parameter;

    	var glResource = null;
    	switch (parameter.type) {
	        case "FLOAT" :
	        case "FLOAT_VEC2" :
	        case "FLOAT_VEC3" :
	        case "FLOAT_VEC4" :
	        	glResource = new Float32Array(resource, 0, parameter.count * componentsPerElementForGLType(parameter.type));
	        	break;
	        default:
	        	break;
    	}
    	
        return glResource;
    };

    AnimationParameterDelegate.prototype.resourceAvailable = function(glResource, ctx) {
    	var animation = ctx.animation;
    	var parameter = ctx.parameter;
    	parameter.data = glResource;
    	animation.handleParameterLoaded(parameter);
        return true;
    };

    var animationParameterDelegate = new AnimationParameterDelegate();

    var AnimationParameterContext = function(parameter, animation) {
        this.parameter = parameter;
        this.animation = animation;
    };

    // Animations
    var Animation = function() {

    	// create Three.js keyframe here
        this.totalParameters = 0;
        this.loadedParameters = 0;
        this.parameters = {};
        this.finishedLoading = false;
        this.onload = null;

    };

    Animation.prototype.constructor = Animation;

    Animation.prototype.handleParameterLoaded = function(parameter) {
    	this.parameters[parameter.name] = parameter;
    	this.loadedParameters++;
    	this.checkFinished();
    };
    
    Animation.prototype.checkFinished = function() {
        if(this.loadedParameters === this.totalParameters) {
            // Build animation
            this.finishedLoading = true;

            if (this.onload) {
                this.onload();
            }
        }
    };
    
    // Resource management

    var ResourceEntry = function(entryID, object, description) {
        this.entryID = entryID;
        this.object = object;
        this.description = description;
    };

    var Resources = function() {
        this._entries = {};
    };

    Resources.prototype.setEntry = function(entryID, object, description) {
        if (!entryID) {
            console.error("No EntryID provided, cannot store", description);
            return;
        }

        if (this._entries[entryID]) {
            console.warn("entry["+entryID+"] is being overwritten");
        }
    
        this._entries[entryID] = new ResourceEntry(entryID, object, description );
    };
    
    Resources.prototype.getEntry = function(entryID) {
        return this._entries[entryID];
    };

    Resources.prototype.clearEntries = function() {
        this._entries = {};
    };

    LoadDelegate = function() {
    }
    
    LoadDelegate.prototype.loadCompleted = function(callback, obj) {
    	callback.call(Window, obj);
    }
    
    // Loader

    var ThreeGLTFLoader = Object.create(WebGLTFLoader, {

        load: {
            enumerable: true,
            value: function(userInfo, options) {
                this.resources = new Resources();
                this.cameras = [];
                this.lights = [];
                this.animations = [];
                THREE.GLTFLoaderUtils.init();
                WebGLTFLoader.load.call(this, userInfo, options);
            }
        },

        yup: {
        	enumerable: true,
        	writable: true,
        	value : true
        },

        container: {
        	enumerable: true,
        	writable: true,
        	value : null
        },
        
        cameras: {
        	enumerable: true,
        	writable: true,
        	value : []
        },

        lights: {
        	enumerable: true,
        	writable: true,
        	value : []
        },
        
        animations: {
        	enumerable: true,
        	writable: true,
        	value : []
        },
        
        // Implement WebGLTFLoader handlers

        handleBuffer: {
            value: function(entryID, description, userInfo) {
                this.resources.setEntry(entryID, null, description);
                description.type = "ArrayBuffer";
                return true;
            }
        },

        handleBufferView: {
            value: function(entryID, description, userInfo) {
                this.resources.setEntry(entryID, null, description);

                var buffer =  this.resources.getEntry(description.buffer);
                description.type = "ArrayBufferView";

                var bufferViewEntry = this.resources.getEntry(entryID);
                bufferViewEntry.buffer = buffer;
                return true;
            }
        },

        handleShader: {
            value: function(entryID, description, userInfo) {
                // No shader handling at this time
                return true;
            }
        },

        handleProgram: {
            value: function(entryID, description, userInfo) {
                // No program handling at this time
                return true;
            }
        },

        handleTechnique: {
            value: function(entryID, description, userInfo) {
        		this.resources.setEntry(entryID, null, description);
                return true;
            }
        },

        threeJSMaterialType : {
            value: function(technique, values, params) {
        	
        		var materialType = THREE.MeshBasicMaterial;
        
        		if (technique && technique.description && technique.description.passes &&
        				technique.description.passes.defaultPass && technique.description.passes.defaultPass.details &&
        				technique.description.passes.defaultPass.details.commonProfile)
        		{
            		var profile = technique.description.passes.defaultPass.details.commonProfile;
            		if (profile)
            		{
	            		switch (profile.lightingModel)
	            		{
	            			case 'Blinn' :
	            			case 'Phong' :
	            				materialType = THREE.MeshPhongMaterial;
	            				break;

	            			case 'Lambert' :
	            				materialType = THREE.MeshLambertMaterial;
	            				break;
	            				
	            			default :
	            				materialType = THREE.MeshBasicMaterial;
	            				break;
	            		}
	            		
	            		if (profile.extras && profile.extras.doubleSided)
	            		{
	            			params.side = THREE.DoubleSide;
	            		}
            		}

        		}
        		
                var texturePath = null;
                var textureParams = null;
                var diffuse = values.diffuse;
                if (diffuse)
                {
                	var texture = diffuse.value;
                    if (texture) {
                        var textureEntry = this.resources.getEntry(texture);
                        if (textureEntry) {
                        	{
                        		var imageEntry = this.resources.getEntry(textureEntry.description.source);
                        		if (imageEntry) {
                        			texturePath = imageEntry.description.uri;
                        		}
                        		
                        		var samplerEntry = this.resources.getEntry(textureEntry.description.sampler);
                        		if (samplerEntry) {
                        			textureParams = samplerEntry.description;
                        		}
                        	}
                        }
                    }                    
                }

                var texture = LoadTexture(texturePath);
                if (texture && textureParams) {
                	
                	if (textureParams.wrapS == "REPEAT")
                		texture.wrapS = THREE.RepeatWrapping;

                	if (textureParams.wrapT == "REPEAT")
                		texture.wrapT = THREE.RepeatWrapping;
                	
                	if (textureParams.magFilter == "LINEAR")
                		texture.magFilter = THREE.LinearFilter;

//                	texture.flipY = false;
//                	if (textureParams.minFilter == "LINEAR")
//               		texture.minFilter = THREE.LinearFilter;
                	
                    params.map = texture;
                }

                var envMapPath = null;
                var envMapParams = null;
                var reflective = values.reflective;
                if (reflective)
                {
                	var texture = reflective.value;
                    if (texture) {
                        var textureEntry = this.resources.getEntry(texture);
                        if (textureEntry) {
                        	{
                        		var imageEntry = this.resources.getEntry(textureEntry.description.source);
                        		if (imageEntry) {
                        			envMapPath = imageEntry.description.uri;
                        		}
                        		
                        		var samplerEntry = this.resources.getEntry(textureEntry.description.sampler);
                        		if (samplerEntry) {
                        			envMapParams = samplerEntry.description;
                        		}
                        	}
                        }
                    }                    
                }

                var texture = LoadTexture(envMapPath);
                if (texture && envMapParams) {
                	
                	if (envMapParams.wrapS == "REPEAT")
                		texture.wrapS = THREE.RepeatWrapping;

                	if (envMapParams.wrapT == "REPEAT")
                		texture.wrapT = THREE.RepeatWrapping;
                	
                	if (envMapParams.magFilter == "LINEAR")
                		texture.magFilter = THREE.LinearFilter;

//                	if (envMapParams.minFilter == "LINEAR")
//               		texture.minFilter = THREE.LinearFilter;
                	
                    params.envMap = texture;
                }
                
                var shininess = values.shininesss || values.shininess; // N.B.: typo in converter!
                if (shininess)
                {
                	shininess = shininess.value;
                }
                
                var diffuseColor = !texturePath ? diffuse.value : null;
                var opacity = 1.0;
                if (values.transparency)
                {
                	var USE_A_ONE = true; // for now, hack because file format isn't telling us
                	opacity =  USE_A_ONE ? values.transparency.value : (1.0 - values.transparency.value);
                }
                
                // if (diffuseColor) diffuseColor = [0, 1, 0];
                                    
                params.color = RgbArraytoHex(diffuseColor);
                params.opacity = opacity;
                params.transparent = opacity < 1.0;
                // hack hack hack
                if (texturePath && texturePath.toLowerCase().indexOf(".png") != -1)
                	params.transparent = true;
                
                if (!(shininess === undefined))
                {
                	params.shininess = shininess;
                }
                
                if (!(values.ambient === undefined) && !(typeof(values.ambient.value) == 'string'))
                {
                	params.ambient = RgbArraytoHex(values.ambient.value);
                }

                if (!(values.emission === undefined))
                {
                	params.emissive = RgbArraytoHex(values.emission.value);
                }
                
                if (!(values.specular === undefined))
                {
                	params.specular = RgbArraytoHex(values.specular.value);
                }

        		return materialType;
        		
        	}
        },
        
        handleMaterial: {
            value: function(entryID, description, userInfo) {
                //this should be rewritten using the meta datas that actually create the shader.
                //here we will infer what needs to be pass to Three.js by looking inside the technique parameters.
                var technique = this.resources.getEntry(description.instanceTechnique.technique);
                var materialParams = {};
                var vals = description.instanceTechnique.values;
                var values = {};
                var i, len = vals.length;
                for (i = 0; i < len; i++)
                {
                	values[vals[i].parameter] = vals[i];
                }

                var materialType = this.threeJSMaterialType(technique, values, materialParams);

                var material = new materialType(materialParams);

                this.resources.setEntry(entryID, material, description);

                return true;
            }
        },

        handleMesh: {
            value: function(entryID, description, userInfo) {
                var mesh = new Mesh();
                this.resources.setEntry(entryID, mesh, description);
                var primitivesDescription = description.primitives;
                if (!primitivesDescription) {
                    //FIXME: not implemented in delegate
                    console.log("MISSING_PRIMITIVES for mesh:"+ entryID);
                    return false;
                }

                for (var i = 0 ; i < primitivesDescription.length ; i++) {
                    var primitiveDescription = primitivesDescription[i];
                    
                    if (primitiveDescription.primitive === "TRIANGLES") {

                        var geometry = new ClassicGeometry();
                        var materialEntry = this.resources.getEntry(primitiveDescription.material);

                        mesh.addPrimitive(geometry, materialEntry.object);

                        var indices = this.resources.getEntry(primitiveDescription.indices);
                        var bufferEntry = this.resources.getEntry(indices.description.bufferView);
                        var indicesObject = {
                        		bufferView : bufferEntry,
                        		byteOffset : indices.description.byteOffset,
                        		count : indices.description.count,
                        		id : indices.description.id,
                        		type : indices.description.type
                        };
                        
                        var indicesContext = new IndicesContext(indicesObject, geometry);
                        var alreadyProcessedIndices = THREE.GLTFLoaderUtils.getBuffer(indicesObject, indicesDelegate, indicesContext);
                        /*if(alreadyProcessedIndices) {
                            indicesDelegate.resourceAvailable(alreadyProcessedIndices, indicesContext);
                        }*/

                        // Load Vertex Attributes
                        var allSemantics = Object.keys(primitiveDescription.semantics);
                        allSemantics.forEach( function(semantic) {
                            geometry.totalAttributes++;

                            var attribute;
                            var attributeID = primitiveDescription.semantics[semantic];
                            var attributeEntry = this.resources.getEntry(attributeID);
                            if (!attributeEntry) {
                                //let's just use an anonymous object for the attribute
                                attribute = description.attributes[attributeID];
                                attribute.id = attributeID;
                                this.resources.setEntry(attributeID, attribute, attribute);
            
                                var bufferEntry = this.resources.getEntry(attribute.bufferView);
                                attributeEntry = this.resources.getEntry(attributeID);

                            } else {
                                attribute = attributeEntry.object;
                                attribute.id = attributeID;
                                var bufferEntry = this.resources.getEntry(attribute.bufferView);
                            }

                            var attributeObject = {
                            		bufferView : bufferEntry,
                            		byteOffset : attribute.byteOffset,
                            		byteStride : attribute.byteStride,
                            		count : attribute.count,
                            		max : attribute.max,
                            		min : attribute.min,
                            		type : attribute.type,
                            		id : attributeID             
                            };
                            
                            var attribContext = new VertexAttributeContext(attributeObject, semantic, geometry);

                            var alreadyProcessedAttribute = THREE.GLTFLoaderUtils.getBuffer(attributeObject, vertexAttributeDelegate, attribContext);
                            /*if(alreadyProcessedAttribute) {
                                vertexAttributeDelegate.resourceAvailable(alreadyProcessedAttribute, attribContext);
                            }*/
                        }, this);
                    }
                }
                return true;
            }
        },

        handleCamera: {
            value: function(entryID, description, userInfo) {
                var camera;
        		var znear = description.znear;
        		var zfar = description.zfar;
                if (description.projection == "perspective")
                {
                	var yfov = description.yfov;                	
                	var xfov = description.xfov;
            		var aspect_ratio = description.aspect_ratio;
                	if (yfov === undefined)
                	{
                		if (xfov)
                		{
                			// N.B.: if no aspect ratio supplied, assume 1?
	                		if (!aspect_ratio)
	                			aspect_ratio = 4 / 3; // container.offsetWidth / container.offsetHeight;
	                		
                			// According to COLLADA spec...
                			// aspect_ratio = xfov / yfov
                			yfov = xfov / aspect_ratio;
                			// yfov = 50;
                		}
                	}
                	
                	if (yfov)
                	{
                		camera = new THREE.PerspectiveCamera(yfov, aspect_ratio, znear, zfar);
                	}
                }
                else
                {
    				camera = new THREE.OrthographicCamera( window.innerWidth / - 2, window.innerWidth / 2, window.innerHeight / 2, window.innerHeight / - 2, znear, zfar );
                }
                
                if (camera)
                {
                	this.resources.setEntry(entryID, camera, description);
                }
                
                return true;
            }
        },

        handleLight: {
            value: function(entryID, description, userInfo) {

        		var light = null;
        		var type = description.type;
        		if (type && description[type])
        		{
        			var lparams = description[type];
            		var color = RgbArraytoHex(lparams.color);
            		
            		switch (type) {
            			case "directional" :
            				light = new THREE.DirectionalLight(color);
            			break;
            			
            			case "point" :
            				light = new THREE.PointLight(color);
            			break;
            			
            			case "spot " :
            				light = new THREE.SpotLight(color);
            			break;
            			
            			case "ambient" : 
            				light = new THREE.AmbientLight(color);
            			break;
            		}
        		}

        		if (light)
        		{
                	this.resources.setEntry(entryID, light, description);	
        		}
        		
        		return true;
            }
        },

        handleNode: {
            value: function(entryID, description, userInfo) {

                var threeNode = new THREE.Object3D();
                threeNode.name = description.name;

                this.resources.setEntry(entryID, threeNode, description);

                var m = description.matrix;
                if(m) {
                    threeNode.matrixAutoUpdate = false;
                    threeNode.applyMatrix(new THREE.Matrix4(
                        m[0],  m[4],  m[8],  m[12],
                        m[1],  m[5],  m[9],  m[13],
                        m[2],  m[6],  m[10], m[14],
                        m[3],  m[7],  m[11], m[15]
                    ));                    
                }

                // Iterate through all node meshes and attach the appropriate objects
                //FIXME: decision needs to be made between these 2 ways, probably meshes will be discarded.
                var meshEntry;
                if (description.mesh) {
                    meshEntry = this.resources.getEntry(description.mesh);
                    meshEntry.object.onComplete(function(mesh) {
                        mesh.attachToNode(threeNode);
                    });
                }

                if (description.meshes) {
                    description.meshes.forEach( function(meshID) {
                        meshEntry = this.resources.getEntry(meshID);
                        meshEntry.object.onComplete(function(mesh) {
                            mesh.attachToNode(threeNode);
                        });
                    }, this);
                }

                if (description.camera) {
                    var cameraEntry = this.resources.getEntry(description.camera);
                    if (cameraEntry) {
                    	threeNode.add(cameraEntry.object);
                    	this.cameras.push(cameraEntry.object);
                    }
                }

                if (description.lights) {
                	description.lights.forEach( function(lightID) {                	
	                    var lightEntry = this.resources.getEntry(lightID);
	                    if (lightEntry) {
	                    	threeNode.add(lightEntry.object);
	                    	this.lights.push(lightEntry.object);
	                    }
                	}, this);
                }
                
                return true;
            }
        },
        
        buildNodeHirerachy: {
            value: function(nodeEntryId, parentThreeNode) {
                var nodeEntry = this.resources.getEntry(nodeEntryId);
                var threeNode = nodeEntry.object;
                parentThreeNode.add(threeNode);

                var children = nodeEntry.description.children;
                if (children) {
                    children.forEach( function(childID) {
                        this.buildNodeHirerachy(childID, threeNode);
                    }, this);
                }

                return threeNode;
            }
        },

        handleScene: {
            value: function(entryID, description, userInfo) {

                if (!description.nodes) {
                    console.log("ERROR: invalid file required nodes property is missing from scene");
                    return false;
                }

                description.nodes.forEach( function(nodeUID) {
                    this.buildNodeHirerachy(nodeUID, userInfo.rootObj);
                }, this);

                if (this.delegate) {
                    this.delegate.loadCompleted(userInfo.callback, userInfo.rootObj);
                }

                return true;
            }
        },

        handleImage: {
            value: function(entryID, description, userInfo) {
                this.resources.setEntry(entryID, null, description);
                return true;
            }
        },
        
        buildAnimation: {
        	value : function(animation) {
        	
        		var interps = [];
	            var i, len = animation.channels.length;
	            for (i = 0; i < len; i++) {
	            	
	            	var channel = animation.channels[i];
	            	var sampler = animation.samplers[channel.sampler];
	            	if (sampler) {
	
	            		var input = animation.parameters[sampler.input];
	            		if (input && input.data) {
	            			
	            			var output = animation.parameters[sampler.output];
	            			if (output && output.data) {
	            				
	            				var target = channel.target;
	            				var node = this.resources.getEntry(target.id);
	            				if (node) {

	            					var path = target.uri;
		            				
		            				if (path == "rotation")
		            				{
		            					convertAxisAngleToQuaternion(output.data, output.count, this.yup);
		            				}
		            				
			            			var interp = {
			            					keys : input.data,
			            					values : output.data,
			            					count : input.count,
			            					target : node.object,
			            					path : path,
			            					type : sampler.interpolation
			            			};
			            			
			            			interps.push(interp);
	            				}
	            			}
	            		}
	            	}
	            }
	            
	            if (interps.length)
	            {
	            	var anim = new THREE.glTFAnimation(interps);
	            	this.animations.push(anim);
	            	anim.loop = true;
	            	anim.play();
	            }
        	}
        },
        
        handleAnimation: {
            value: function(entryID, description, userInfo) {
        	
        		var self = this;
	            var animation = new Animation();
	            animation.onload = function() {
	            	self.buildAnimation(animation);
	            };	            
	            
	            animation.channels = description.channels;
	            animation.samplers = description.samplers;
	            this.resources.setEntry(entryID, animation, description);
	            var parameters = description.parameters;
	            if (!parameters) {
	                //FIXME: not implemented in delegate
	                console.log("MISSING_PARAMETERS for animation:"+ entryID);
	                return false;
	            }
	
                // Load Vertex Attributes
                var params = Object.keys(parameters);
                params.forEach( function(param) {

                	animation.totalParameters++;
                    var parameter = parameters[param];
                    var bufferEntry = this.resources.getEntry(parameter.bufferView);

                    var paramObject = {
                    		bufferView : bufferEntry,
                    		byteOffset : parameter.byteOffset,
                    		count : parameter.count,
                    		type : parameter.type,
                    		id : parameter.bufferView,
                    		name : param             
                    };
                    
                    var paramContext = new AnimationParameterContext(paramObject, animation);

                    var alreadyProcessedAttribute = THREE.GLTFLoaderUtils.getBuffer(paramObject, animationParameterDelegate, paramContext);
                    /*if(alreadyProcessedAttribute) {
                        vertexAttributeDelegate.resourceAvailable(alreadyProcessedAttribute, attribContext);
                    }*/
                }, this);

	            return true;
            }
        },

        handleIndices: {
            value: function(entryID, description, userInfo) {
        		// Save indices entry
        		description.id = entryID;
        		this.resources.setEntry(entryID, null, description);
                return true;
            }
        },

        handleAttribute: {
            value: function(entryID, description, userInfo) {
	    		// Save attribute entry
	    		this.resources.setEntry(entryID, description, description);
                return true;
            }
        },

        handleSkin: {
            value: function(entryID, description, userInfo) {
            	// No skin handling at this time
                return true;
            }
        },

        handleSampler: {
            value: function(entryID, description, userInfo) {
	    		// Save attribute entry
	    		this.resources.setEntry(entryID, description, description);
                return true;
            }
        },

        handleTexture: {
            value: function(entryID, description, userInfo) {
	    		// Save attribute entry
	    		this.resources.setEntry(entryID, null, description);
                return true;
            }
        },
        
        handleError: {
            value: function(msg) {

        		throw new Error(msg);
        		return true;
        	}
        },
        
        _delegate: {
            value: new LoadDelegate,
            writable: true
        },

        delegate: {
            enumerable: true,
            get: function() {
                return this._delegate;
            },
            set: function(value) {
                this._delegate = value;
            }
        }
    });


    // Loader

    var Context = function(rootObj, callback) {
        this.rootObj = rootObj;
        this.callback = callback;
    };

    var rootObj = new THREE.Object3D();

    var self = this;
    
    var loader = Object.create(ThreeGLTFLoader);
    loader.yup = this.yup;
    loader.container = this.container;
    loader.initWithPath(url);
    loader.load(new Context(rootObj, 
    					function(obj) {
    						self.cameras = loader.cameras;
    						self.animations = loader.animations;
    						callback(obj);
    					}), 
    			null);

    return rootObj;
}



/**
 * @author Tony Parisi / http://www.tonyparisi.com/
 */


THREE.glTFLoader = function (showStatus) {
    this.useBufferGeometry = (THREE.glTFLoader.useBufferGeometry !== undefined ) ?
            THREE.glTFLoader.useBufferGeometry : true;
    this.useShaders = (THREE.glTFLoader.useShaders !== undefined ) ?
            THREE.glTFLoader.useShaders : true;
    this.meshesRequested = 0;
    this.meshesLoaded = 0;
    this.pendingMeshes = [];
    this.animationsRequested = 0;
    this.animationsLoaded = 0;
    this.animations = [];
    this.shadersRequested = 0;
    this.shadersLoaded = 0;
    this.shaders = {};
    THREE.glTFShaders.removeAll();
    THREE.Loader.call( this, showStatus );

    this.idx = {};
    for (n in WebGLRenderingContext) { z = WebGLRenderingContext[n]; this.idx[z] = n; }
}

THREE.glTFLoader.prototype = new THREE.Loader();
THREE.glTFLoader.prototype.constructor = THREE.glTFLoader;

THREE.glTFLoader.prototype.load = function( url, callback ) {
    
    var theLoader = this;
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
    
    function convertAxisAngleToQuaternion(rotations, count)
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

    function componentsPerElementForGLType(type) {
        switch(type) {          
            case "SCALAR" :
                nElements = 1;
                break;
            case "VEC2" :
                nElements = 2;
                break;
            case "VEC3" :
                nElements = 3;
                break;
            case "VEC4" :
                nElements = 4;
                break;
            case "MAT2" :
                nElements = 4;
                break;
            case "MAT3" :
                nElements = 9;
                break;
            case "MAT4" :
                nElements = 16;
                break;
            default :
                debugger;
                break;
        }
        
        return nElements;
    }

    function replaceShaderDefinitions(shader, material) {
/*        var s = shader;
        s = s.replace(/a_position/g, 'position');
        s = s.replace(/a_normal/g, 'normal');
        s = s.replace(/a_texcoord0/g, 'uv');
        s = s.replace(/u_normalMatrix/g, 'normalMatrix');
        s = s.replace(/u_modelViewMatrix/g, 'modelViewMatrix');
        s = s.replace(/u_projectionMatrix/g, 'projectionMatrix');
        return s;*/

        var program = material.params.program;
        var shaderParams = material.params.parameters;
        var params = {};

        for (var uniform in program.uniforms) {
            var pname = program.uniforms[uniform];
            var shaderParam = shaderParams[pname];
            var semantic = shaderParam.semantic;
            if (semantic) {
                params[uniform] = shaderParam;
            }
        }

        for (var attribute in program.attributes) {
            var pname = program.attributes[attribute];
            var shaderParam = shaderParams[pname];
            var semantic = shaderParam.semantic;
            if (semantic) {
                params[attribute] = shaderParam;
            }
        }


        var s = shader;
        var r = "";
        for (var pname in params) {
            var param = params[pname];
            var semantic = param.semantic;

            // THIS: for (n in WebGLRenderingContext) { z = WebGLRenderingContext[n]; idx[z] = n; }
            //console.log("shader uniform param type: ", ptype, "-", theLoader.idx[ptype])

            r = eval("/" + pname + "/g");
            
            switch (semantic) {
                case "POSITION" :
                    s = s.replace(r, 'position');
                    break;
                case "NORMAL" :
                    s = s.replace(r, 'normal');
                    break;
                case "TEXCOORD_0" :
                    s = s.replace(r, 'uv');
                    break;
                case "MODELVIEW" :
                    if (!param.source) {
                        s = s.replace(r, 'modelViewMatrix');
                    }
                    break;
                case "MODELVIEWINVERSETRANSPOSE" :
                    if (!param.source) {
                       s = s.replace(r, 'normalMatrix');
                    }
                    break;
                case "PROJECTION" :
                    if (!param.source) {
                        s = s.replace(r, 'projectionMatrix');
                    }
                    break;
                case "WEIGHT" :
                    s = s.replace(r, 'skinWeight');
                    break;
                case "JOINT" :
                    s = s.replace(r, 'skinIndex');
                    break;

                default :
                    break;
            }

        }

        return s;
    }

    function replaceShaderSemantics(material) {

        var fragmentShader = theLoader.shaders[material.params.fragmentShader];
        if (fragmentShader) {
            fragmentShader = replaceShaderDefinitions(fragmentShader, material);
            theLoader.shaders[material.params.fragmentShader] = fragmentShader;
        }
        
        var vertexShader = theLoader.shaders[material.params.vertexShader];
        if (vertexShader) {
            vertexShader = replaceShaderDefinitions(vertexShader, material);
            theLoader.shaders[material.params.vertexShader] = vertexShader;
        }

    }

    function createShaderMaterial(material, geometry) {
        
        // replace named attributes and uniforms with Three.js built-ins    
        replaceShaderSemantics(material);

        var fragmentShader = theLoader.shaders[material.params.fragmentShader];
        if (!fragmentShader) {
            console.log("ERROR: Missing fragment shader definition:", material.params.fragmentShader);
            return new THREE.MeshPhongMaterial;
        }
        
        var vertexShader = theLoader.shaders[material.params.vertexShader];
        if (!vertexShader) {
            console.log("ERROR: Missing vertex shader definition:", material.params.vertexShader);
            return new THREE.MeshPhongMaterial;
        }

        var shaderMaterial = new THREE.RawShaderMaterial( {

            fragmentShader: fragmentShader,
            vertexShader: vertexShader,
            uniforms: material.params.uniforms,
            transparent: material.params.transparent,

        } );

/*        var attributes = material.params.attributes;
        var idx = 0;
        for (var aname in attributes) {
            var attribute = attributes[aname];
            var semantic = attribute.semantic;
            if (semantic == "POSITION") {
                console.log("Position attribute", attribute);
                var attr = geometry.attributes.position;
                var array = new Float32Array(attr.array.buffer, 0, attr.array.length);
                geometry.addAttribute(aname, new THREE.BufferAttribute(array, attr.itemSize));
//                shaderMaterial.index0AttributeName = aname;
            }
            else if (semantic == "NORMAL") {
                console.log("Normal attribute", attribute);
                var attr = geometry.attributes.normal;
                var array = new Float32Array(attr.array.buffer, 0, attr.array.length);
                geometry.addAttribute(aname, new THREE.BufferAttribute(array, attr.itemSize));
            }
            else if (semantic == "TEXCOORD_0") {
                console.log("Texcoord0 attribute", attribute);
                var attr = geometry.attributes.uv;
                var array = new Float32Array(attr.array.buffer, 0, attr.array.length);
                geometry.addAttribute(aname, new THREE.BufferAttribute(array, attr.itemSize));
            }
        }*/

        return shaderMaterial;

        return new THREE.MeshPhongMaterial(material.params);
    }


    function LoadTexture(src) {
        if(!src) { return null; }
        return THREE.ImageUtils.loadTexture(src);
    }

    function CreateTexture(resources, resource) {
        var texturePath = null;
        var textureParams = null;

        if (resource)
        {
            var texture = resource;
            if (texture) {
                var textureEntry = resources.getEntry(texture);
                if (textureEntry) {
                    {
                        var imageEntry = resources.getEntry(textureEntry.description.source);
                        if (imageEntry) {
                            texturePath = imageEntry.description.uri;
                        }
                        
                        var samplerEntry = resources.getEntry(textureEntry.description.sampler);
                        if (samplerEntry) {
                            textureParams = samplerEntry.description;
                        }
                    }
                }
            }                    
        }

        var texture = LoadTexture(texturePath);
        if (texture && textureParams) {
            
            if (textureParams.wrapS == WebGLRenderingContext.REPEAT)
                texture.wrapS = THREE.RepeatWrapping;

            if (textureParams.wrapT == WebGLRenderingContext.REPEAT)
                texture.wrapT = THREE.RepeatWrapping;
            
            if (textureParams.magFilter == WebGLRenderingContext.LINEAR)
                texture.magFilter = THREE.LinearFilter;

//                  if (textureParams.minFilter == "LINEAR")
//                      texture.minFilter = THREE.LinearFilter;
        }

        return texture;
    }

    // Geometry processing

    var ClassicGeometry = function() {

        if (theLoader.useBufferGeometry) {
            this.geometry = new THREE.BufferGeometry;
        }
        else {
            this.geometry = new THREE.Geometry;
        }
        this.totalAttributes = 0;
        this.loadedAttributes = 0;
        this.indicesLoaded = false;
        this.finished = false;

        this.onload = null;

        this.uvs = null;
        this.indexArray = null;
    };

    ClassicGeometry.prototype.constructor = ClassicGeometry;

    ClassicGeometry.prototype.buildArrayGeometry = function() {

        // Build indexed mesh
        var geometry = this.geometry;
        var normals = geometry.normals;
        var indexArray = this.indexArray;
        var uvs = this.uvs;
        var a, b, c;
        var i, l;
        var faceNormals = null;
        var faceTexcoords = null;
        
        for(i = 0, l = this.indexArray.length; i < l; i += 3) {
            a = indexArray[i];
            b = indexArray[i+1];
            c = indexArray[i+2];
            if(normals) {
                faceNormals = [normals[a], normals[b], normals[c]];
            }
            geometry.faces.push( new THREE.Face3( a, b, c, faceNormals, null, null ) );
            if(uvs) {
                geometry.faceVertexUvs[0].push([ uvs[a], uvs[b], uvs[c] ]);
            }
        }

        // Allow Three.js to calculate some values for us
        geometry.computeBoundingBox();
        geometry.computeBoundingSphere();
        geometry.computeFaceNormals();
        if(!normals) {
            geometry.computeVertexNormals();
        }

    }

    ClassicGeometry.prototype.buildBufferGeometry = function() {
        // Build indexed mesh
        var geometry = this.geometry;
        geometry.addAttribute( 'index', new THREE.BufferAttribute( this.indexArray, 1 ) );

        var offset = {
                start: 0,
                index: 0,
                count: this.indexArray.length
            };

        geometry.offsets.push( offset );

        geometry.computeBoundingSphere();
    }
    
    ClassicGeometry.prototype.checkFinished = function() {
        if(this.indexArray && this.loadedAttributes === this.totalAttributes) {
            
            if (theLoader.useBufferGeometry) {
                this.buildBufferGeometry();
            }
            else {
                this.buildArrayGeometry();
            }
            
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



    VertexAttributeDelegate.prototype.arrayResourceAvailable = function(glResource, ctx) {
        var geom = ctx.geometry;
        var attribute = ctx.attribute;
        var semantic = ctx.semantic;
        var floatArray;
        var i, l;
        //FIXME: Float32 is assumed here, but should be checked.

        if(semantic == "POSITION") {
            // TODO: Should be easy to take strides into account here
            floatArray = new Float32Array(glResource, 0, attribute.count * componentsPerElementForGLType(attribute.type));
            for(i = 0, l = floatArray.length; i < l; i += 3) {
                geom.geometry.vertices.push( new THREE.Vector3( floatArray[i], floatArray[i+1], floatArray[i+2] ) );
            }
        } else if(semantic == "NORMAL") {
            geom.geometry.normals = [];
            floatArray = new Float32Array(glResource, 0, attribute.count * componentsPerElementForGLType(attribute.type));
            for(i = 0, l = floatArray.length; i < l; i += 3) {
                geom.geometry.normals.push( new THREE.Vector3( floatArray[i], floatArray[i+1], floatArray[i+2] ) );
            }
        } else if ((semantic == "TEXCOORD_0") || (semantic == "TEXCOORD" )) {
            geom.uvs = [];
            floatArray = new Float32Array(glResource, 0, attribute.count * componentsPerElementForGLType(attribute.type));
            for(i = 0, l = floatArray.length; i < l; i += 2) {
                geom.uvs.push( new THREE.Vector2( floatArray[i], 1.0 - floatArray[i+1] ) );
            }
        }
        else if (semantic == "WEIGHT") {
            nComponents = componentsPerElementForGLType(attribute.type);
            floatArray = new Float32Array(glResource, 0, attribute.count * nComponents);
            for(i = 0, l = floatArray.length; i < l; i += 4) {
                geom.geometry.skinWeights.push( new THREE.Vector4( floatArray[i], floatArray[i+1], floatArray[i+2], floatArray[i+3] ) );
            }
        }
        else if (semantic == "JOINT") {
            nComponents = componentsPerElementForGLType(attribute.type);
            floatArray = new Float32Array(glResource, 0, attribute.count * nComponents);
            for(i = 0, l = floatArray.length; i < l; i += 4) {
                geom.geometry.skinIndices.push( new THREE.Vector4( floatArray[i], floatArray[i+1], floatArray[i+2], floatArray[i+3] ) );
            }
        }
    }
    
    VertexAttributeDelegate.prototype.bufferResourceAvailable = function(glResource, ctx) {
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
            geom.geometry.addAttribute( 'position', new THREE.BufferAttribute( floatArray, 3 ) );
        } else if(semantic == "NORMAL") {
            nComponents = componentsPerElementForGLType(attribute.type);
            floatArray = new Float32Array(glResource, 0, attribute.count * nComponents);
            geom.geometry.addAttribute( 'normal', new THREE.BufferAttribute( floatArray, 3 ) );
        } else if ((semantic == "TEXCOORD_0") || (semantic == "TEXCOORD" )) {
            
            nComponents = componentsPerElementForGLType(attribute.type);
            floatArray = new Float32Array(glResource, 0, attribute.count * nComponents);
            // N.B.: flip Y value... should we just set texture.flipY everywhere?
            for (i = 0; i < floatArray.length / 2; i++) {
                floatArray[i*2+1] = 1.0 - floatArray[i*2+1];
            }
            geom.geometry.addAttribute( 'uv', new THREE.BufferAttribute( floatArray, nComponents ) );
        }
        else if (semantic == "WEIGHT") {
            nComponents = componentsPerElementForGLType(attribute.type);
            floatArray = new Float32Array(glResource, 0, attribute.count * nComponents);
            geom.geometry.addAttribute( 'skinWeight', new THREE.BufferAttribute( floatArray, nComponents ) );
        }
        else if (semantic == "JOINT") {
            nComponents = componentsPerElementForGLType(attribute.type);
            floatArray = new Float32Array(glResource, 0, attribute.count * nComponents);
            geom.geometry.addAttribute( 'skinIndex', new THREE.BufferAttribute( floatArray, nComponents ) );
        }
    }
    
    VertexAttributeDelegate.prototype.resourceAvailable = function(glResource, ctx) {
        if (theLoader.useBufferGeometry) {
            this.bufferResourceAvailable(glResource, ctx);
        }
        else {
            this.arrayResourceAvailable(glResource, ctx);
        }
        
        var geom = ctx.geometry;
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
        this.materialsPending = [];
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
            var material = primitive.material;
            var materialParams = material.params;
            if (!(material instanceof THREE.Material)) {
                material = createShaderMaterial(material, primitive.geometry.geometry);
            }

            var threeMesh = new THREE.Mesh(primitive.geometry.geometry, material);
            threeMesh.castShadow = true;
            threeNode.add(threeMesh);

            if (material instanceof THREE.ShaderMaterial) {
                var glTFShader = new THREE.glTFShader(material, materialParams, threeMesh, theLoader.rootObj);
                THREE.glTFShaders.add(glTFShader);

            }
        });
    };

    // Delayed-loaded material
    var Material = function(params) {
        this.params = params;
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
            case "SCALAR" :
            case "VEC2" :
            case "VEC3" :
            case "VEC4" :
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
    
    // Delegate for processing inverse bind matrices buffer
    var InverseBindMatricesDelegate = function() {};

    InverseBindMatricesDelegate.prototype.handleError = function(errorCode, info) {
        // FIXME: report error
        console.log("ERROR(InverseBindMatricesDelegate):"+errorCode+":"+info);
    };

    InverseBindMatricesDelegate.prototype.convert = function(resource, ctx) {
        var parameter = ctx.parameter;

        var glResource = null;
        switch (parameter.type) {
            case "MAT4" :
                glResource = new Float32Array(resource, 0, parameter.count * componentsPerElementForGLType(parameter.type));
                break;
            default:
                break;
        }
        
        return glResource;
    };

    InverseBindMatricesDelegate.prototype.resourceAvailable = function(glResource, ctx) {
        var skin = ctx.skin;
        skin.inverseBindMatrices = glResource;
        return true;
    };

    var inverseBindMatricesDelegate = new InverseBindMatricesDelegate();

    var InverseBindMatricesContext = function(param, skin) {
        this.parameter = param;
        this.skin = skin;
    };

    // Delegate for processing shaders from external files
    var ShaderDelegate = function() {};

    ShaderDelegate.prototype.handleError = function(errorCode, info) {
        // FIXME: report error
        console.log("ERROR(ShaderDelegate):"+errorCode+":"+info);
    };

    ShaderDelegate.prototype.convert = function(resource, ctx) {
        return resource; 
    }
    
    ShaderDelegate.prototype.resourceAvailable = function(data, ctx) {
        theLoader.shadersLoaded++;
        theLoader.shaders[ctx.id] = data;
        return true;
    };

    var shaderDelegate = new ShaderDelegate();

    var ShaderContext = function(id, path) {
        this.id = id;
        this.uri = path;
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

    var ThreeGLTFLoader = Object.create(glTFParser, {

        load: {
            enumerable: true,
            value: function(userInfo, options) {
                this.resources = new Resources();
                this.cameras = [];
                this.lights = [];
                this.animations = [];
                this.joints = {};
                this.skeltons = {};
                THREE.GLTFLoaderUtils.init();
                glTFParser.load.call(this, userInfo, options);
            }
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
                this.resources.setEntry(entryID, null, description);
                var shaderRequest = {
                        id : entryID,
                        uri : description.uri,
                };

                var shaderContext = new ShaderContext(entryID, description.uri);

                theLoader.shadersRequested++;
                THREE.GLTFLoaderUtils.getFile(shaderRequest, shaderDelegate, shaderContext);
                
                return true;
            }
        },

        handleProgram: {
            value: function(entryID, description, userInfo) {
                this.resources.setEntry(entryID, null, description);
                return true;
            }
        },

        handleTechnique: {
            value: function(entryID, description, userInfo) {
                this.resources.setEntry(entryID, null, description);
                return true;
            }
        },

        
        createShaderParams : {
            value: function(materialId, values, params, instanceProgram, shaderParams) {
                var program = this.resources.getEntry(instanceProgram.program);
                
                params.uniforms = {};
                params.attributes = {};
                params.program = instanceProgram;
                params.parameters = shaderParams;
                if (program) {
                    params.fragmentShader = program.description.fragmentShader;
                    params.vertexShader = program.description.vertexShader;
                    for (var uniform in instanceProgram.uniforms) {
                        var pname = instanceProgram.uniforms[uniform];
                        var shaderParam = shaderParams[pname];
                        var ptype = shaderParam.type;
                        var pcount = shaderParam.count;
                        var value = values[pname];
                        var utype = "";
                        var uvalue;
                        var ulength;

                        // THIS: for (n in WebGLRenderingContext) { z = WebGLRenderingContext[n]; idx[z] = n; }
                        //console.log("shader uniform param type: ", ptype, "-", theLoader.idx[ptype])

                        
                        switch (ptype) {
                            case WebGLRenderingContext.FLOAT :
                                utype = "f";
                                uvalue = value;
                                if (pname == "transparency") {
                                    var USE_A_ONE = true; // for now, hack because file format isn't telling us
                                    var opacity =  USE_A_ONE ? value : (1.0 - value);
                                    uvalue = opacity;
                                    params.transparent = true;                                    
                                }
                                break;
                            case WebGLRenderingContext.FLOAT_VEC3 :
                                utype = "v3";
                                uvalue = new THREE.Vector3;
                                if (shaderParam && shaderParam.value) {
                                    var v3 = shaderParam.value;
                                    uvalue.fromArray(v3);
                                }
                                if (value) {
                                    uvalue.fromArray(value);
                                }
                                break;
                            case WebGLRenderingContext.FLOAT_VEC4 :
                                utype = "v4";
                                uvalue = new THREE.Vector4;
                                if (shaderParam && shaderParam.value) {
                                    var v4 = shaderParam.value;
                                    uvalue.fromArray(v4);
                                }
                                if (value) {
                                    uvalue.fromArray(value);
                                }
                                break;
                            case WebGLRenderingContext.FLOAT_MAT3 :
                                utype = "m3";
                                uvalue = new THREE.Matrix3;
                                if (shaderParam && shaderParam.value) {
                                    var m3 = shaderParam.value;
                                    uvalue.fromArray(m3);
                                }
                                if (value) {
                                    uvalue.fromArray(value);
                                }
                                break;
                            case WebGLRenderingContext.FLOAT_MAT4 :
                                if (pcount !== undefined) {
                                    utype = "m4v";
                                    uvalue = new Array(pcount);
                                    for (var mi = 0; mi < pcount; mi++) {
                                        uvalue[mi] = new THREE.Matrix4;
                                    }
                                    ulength = pcount;

                                    if (shaderParam && shaderParam.value) {
                                        var m4v = shaderParam.value;
                                        uvalue.fromArray(m4v);
                                    }
                                    if (value) {
                                        uvalue.fromArray(value);

                                    }                                    
                                }
                                else {
                                    utype = "m4";
                                    uvalue = new THREE.Matrix4;

                                    if (shaderParam && shaderParam.value) {
                                        var m4 = shaderParam.value;
                                        uvalue.fromArray(m4);
                                    }
                                    if (value) {
                                        uvalue.fromArray(value);

                                    }                                    
                                }
/*                                if (pname == "light0Transform") {
                                    uvalue.set(
                                        -0.954692,
                                        0.218143,
                                        -0.202428,
                                        0,
                                        0.0146721,
                                        0.713885,
                                        0.700109,
                                        0,
                                        0.297235,
                                        0.665418,
                                        -0.684741,
                                        0,
                                        1.48654,
                                        1.83672,
                                        -2.92179,
                                        1);
                                }*/


                                break;
                            case WebGLRenderingContext.SAMPLER_2D :
                                utype = "t";
                                uvalue = value ? CreateTexture(this.resources, value) : null;
                                break;
                            default :
                                console.log("Unknown shader uniform param type: ", ptype, "-", theLoader.idx[ptype])
                                break;
                        }


                        var udecl = { type : utype, value : uvalue, length : ulength };

                        params.uniforms[uniform] = udecl;
                    }

                    for (var attribute in instanceProgram.attributes) {
                        var aname = instanceProgram.attributes[attribute];
                        var atype = shaderParams[aname].type;
                        var semantic = shaderParams[aname].semantic;
                        var adecl = { type : atype, semantic : semantic };

                        params.attributes[attribute] = adecl;
                    }
                    
                }
            }
        },
        
        threeJSMaterialType : {
            value: function(materialId, technique, values, params) {
            
                var materialType = THREE.MeshPhongMaterial;
                var defaultPass = null;
                var description = technique ? technique.description : null;
                if (description && description.passes)
                    defaultPass = description.passes.defaultPass;
                
                if (defaultPass) {
                    if (!theLoader.useShaders && defaultPass.details && defaultPass.details.commonProfile) {
                        var profile = description.passes.defaultPass.details.commonProfile;
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
                    else if (defaultPass.instanceProgram) {
                        
                        var instanceProgram = defaultPass.instanceProgram;
                        this.createShaderParams(materialId, values, params, instanceProgram, technique.description.parameters);
                        
                        var loadshaders = true;
                        
                        if (loadshaders) {
                            materialType = Material;
                        }
                    }
                }
                
                params.map = CreateTexture(this.resources, values.diffuse);
                params.envMap = CreateTexture(this.resources, values.reflective);;

         
                var shininess = values.shininesss || values.shininess; // N.B.: typo in converter!
                if (shininess)
                {
                    shininess = shininess;
                }
                
                var diffuseColor = null;
                if (!params.map) {
                    diffuseColor = values.diffuse;
                }
                var opacity = 1.0;
                if (values.hasOwnProperty("transparency"))
                {
                    var USE_A_ONE = true; // for now, hack because file format isn't telling us
                    opacity =  USE_A_ONE ? values.transparency : (1.0 - values.transparency);
                }
                
                // if (diffuseColor) diffuseColor = [0, 1, 0];
                                    
                params.color = RgbArraytoHex(diffuseColor);
                params.opacity = opacity;
                params.transparent = opacity < 1.0;
                // hack hack hack
                if (params.map && params.map.sourceFile.toLowerCase().indexOf(".png") != -1)
                    params.transparent = true;
                
                if (!(shininess === undefined))
                {
                    params.shininess = shininess;
                }
                
                if (!(values.ambient === undefined) && !(typeof(values.ambient) == 'string'))
                {
                    params.ambient = RgbArraytoHex(values.ambient);
                }

                if (!(values.emission === undefined))
                {
                    params.emissive = RgbArraytoHex(values.emission);
                }
                
                if (!(values.specular === undefined))
                {
                    params.specular = RgbArraytoHex(values.specular);
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
                var values = description.instanceTechnique.values;
                
                var materialType = this.threeJSMaterialType(entryID, technique, values, materialParams);

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
                    
                    if (primitiveDescription.primitive === WebGLRenderingContext.TRIANGLES) {

                        var geometry = new ClassicGeometry();
                        var materialEntry = this.resources.getEntry(primitiveDescription.material);

                        mesh.addPrimitive(geometry, materialEntry.object);

                        var indices = this.resources.getEntry(primitiveDescription.indices);
                        var bufferEntry = this.resources.getEntry(indices.description.bufferView);
                        var indicesObject = {
                                bufferView : bufferEntry,
                                byteOffset : indices.description.byteOffset,
                                count : indices.description.count,
                                id : indices.entryID,
                                componentType : indices.description.componentType,
                                type : indices.description.type
                        };
                        
                        var indicesContext = new IndicesContext(indicesObject, geometry);
                        var alreadyProcessedIndices = THREE.GLTFLoaderUtils.getBuffer(indicesObject, indicesDelegate, indicesContext);
                        /*if(alreadyProcessedIndices) {
                            indicesDelegate.resourceAvailable(alreadyProcessedIndices, indicesContext);
                        }*/

                        // Load Vertex Attributes
                        var allAttributes = Object.keys(primitiveDescription.attributes);
                        allAttributes.forEach( function(semantic) {
                            geometry.totalAttributes++;

                            var attribute;
                            var attributeID = primitiveDescription.attributes[semantic];
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
                                    componentType : attribute.componentType,
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
                if (description.type == "perspective")
                {
                    var znear = description.perspective.znear;
                    var zfar = description.perspective.zfar;
                    var yfov = description.perspective.yfov;                    
                    var xfov = description.perspective.xfov;
                    var aspect_ratio = description.perspective.aspect_ratio;

                    if (!aspect_ratio)
                        aspect_ratio = 1; 

                    if (xfov === undefined) {
                        if (yfov)
                        {
                            // According to COLLADA spec...
                            // aspect_ratio = xfov / yfov
                            xfov = yfov * aspect_ratio;
                        }
                    }
                    
                    if (yfov === undefined)
                    {
                        if (xfov)
                        {
                            // According to COLLADA spec...
                            // aspect_ratio = xfov / yfov
                            yfov = xfov / aspect_ratio;
                        }
                        
                    }
                    
                    if (xfov)
                    {
                        camera = new THREE.PerspectiveCamera(xfov, aspect_ratio, znear, zfar);
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
                            light.position.set(0, 0, 1);
                        break;
                        
                        case "point" :
                            light = new THREE.PointLight(color);
                        break;
                        
                        case "spot " :
                            light = new THREE.SpotLight(color);
                            light.position.set(0, 0, 1);
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

        addPendingMesh: {
            value: function(mesh, threeNode) {
                theLoader.pendingMeshes.push({
                    mesh: mesh,
                    node: threeNode
                });
            }
        },
        
        handleNode: {
            value: function(entryID, description, userInfo) {

                var threeNode = null;
                if (description.jointName) {
                    threeNode = new THREE.Bone();
                    threeNode.jointName = description.jointName;
                    this.joints[description.jointName] = entryID;
                }
                else {
                    threeNode = new THREE.Object3D();
                }
                
                threeNode.name = description.name;
                threeNode.glTFID = entryID;

                this.resources.setEntry(entryID, threeNode, description);

                var m = description.matrix;
                if(m) {
                    threeNode.matrixAutoUpdate = false;
                    threeNode.applyMatrix(new THREE.Matrix4().set(
                        m[0],  m[4],  m[8],  m[12],
                        m[1],  m[5],  m[9],  m[13],
                        m[2],  m[6],  m[10], m[14],
                        m[3],  m[7],  m[11], m[15]
                    ));                    
                }
                else {
                    var t = description.translation;
                    var r = description.rotation;
                    var s = description.scale;
                    
                    var position = t ? new THREE.Vector3(t[0], t[1], t[2]) :
                        new THREE.Vector3;
                    if (r) {
                        convertAxisAngleToQuaternion(r, 1);
                    }
                    var rotation = r ? new THREE.Quaternion(r[0], r[1], r[2], r[3]) :
                        new THREE.Quaternion;
                    var scale = s ? new THREE.Vector3(s[0], s[1], s[2]) :
                        new THREE.Vector3(1, 1, 1);
                    
                    var matrix = new THREE.Matrix4;
                    matrix.compose(position, rotation, scale);
                    threeNode.matrixAutoUpdate = false;
                    threeNode.applyMatrix(matrix);                    
                }

                var self = this;
                
                // Iterate through all node meshes and attach the appropriate objects
                //FIXME: decision needs to be made between these 2 ways, probably meshes will be discarded.
                var meshEntry;
                if (description.mesh) {
                    meshEntry = this.resources.getEntry(description.mesh);
                    theLoader.meshesRequested++;
                    meshEntry.object.onComplete(function(mesh) {
                        self.addPendingMesh(mesh, threeNode);
                        theLoader.meshesLoaded++;
                        theLoader.checkComplete();
                    });
                }

                if (description.meshes) {
                    description.meshes.forEach( function(meshID) {
                        meshEntry = this.resources.getEntry(meshID);
                        theLoader.meshesRequested++;
                        meshEntry.object.onComplete(function(mesh) {
                            self.addPendingMesh(mesh, threeNode);
                            theLoader.meshesLoaded++;
                            theLoader.checkComplete();
                        });
                    }, this);
                }

                if (description.instanceSkin) {

                    var skinEntry =  this.resources.getEntry(description.instanceSkin.skin);
                    
                    if (skinEntry) {

                        var skin = skinEntry.object;
                        description.instanceSkin.skin = skin;
                        threeNode.instanceSkin = description.instanceSkin;

                        var meshes = description.instanceSkin.meshes;
                        skin.meshes = [];
                        meshes.forEach( function(meshID) {
                            meshEntry = this.resources.getEntry(meshID);
                            theLoader.meshesRequested++;
                            meshEntry.object.onComplete(function(mesh) {
                                
                                skin.meshes.push(mesh);
                                theLoader.meshesLoaded++;
                                theLoader.checkComplete();
                            });
                        }, this);
                        
                    }
                }
                                
                if (description.camera) {
                    var cameraEntry = this.resources.getEntry(description.camera);
                    if (cameraEntry) {
                        threeNode.add(cameraEntry.object);
                        this.cameras.push(cameraEntry.object);
                    }
                }

                if (description.light) {
                    var lightEntry = this.resources.getEntry(description.light);
                    if (lightEntry) {
                        threeNode.add(lightEntry.object);
                        this.lights.push(lightEntry.object);
                    }
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

        buildSkin: {
            value: function(node) {
            
                var skin = node.instanceSkin.skin;
                if (skin) {
                    node.instanceSkin.skeletons.forEach(function(skeleton) {
                        var nodeEntry = this.resources.getEntry(skeleton);
                        if (nodeEntry) {

                            var rootSkeleton = nodeEntry.object;

                            var dobones = true;

                            var i, len = skin.meshes.length;
                            for (i = 0; i < len; i++) {
                                var mesh = skin.meshes[i];
                                var threeMesh = null;
                                mesh.primitives.forEach(function(primitive) {

                                    var material = primitive.material;
                                    var materialParams = material.params;
                                    if (!(material instanceof THREE.Material)) {
                                        material = createShaderMaterial(material, primitive.geometry.geometry);
                                    }

                                    threeMesh = new THREE.SkinnedMesh(primitive.geometry.geometry, material, false);
                                    threeMesh.add(rootSkeleton);
                                    
                                    var geometry = primitive.geometry.geometry;
                                    var j;
                                    if (geometry.vertices) {
                                        for ( j = 0; j < geometry.vertices.length; j ++ ) {
                                            geometry.vertices[j].applyMatrix4( skin.bindShapeMatrix );  
                                        }
                                    }
                                    else if (geometry.attributes.position) {
                                        var a = geometry.attributes.position.array;
                                        var v = new THREE.Vector3;
                                        for ( j = 0; j < a.length / 3; j++ ) {
                                            v.set(a[j * 3], a[j * 3 + 1], a[j * 3 + 2]);
                                            v.applyMatrix4( skin.bindShapeMatrix );
                                            a[j * 3] = v.x;
                                            a[j * 3 + 1] = v.y;
                                            a[j * 3 + 2] = v.z;
                                        }
                                    }

                                    if (threeMesh && dobones) {

                                        material.skinning = true;
                                        
                                        var jointNames = skin.jointNames;
                                        var joints = [];
                                        threeMesh.skeleton.bones = [];
                                        threeMesh.skeleton.boneInverses = [];
                                        threeMesh.skeleton.boneMatrices = new Float32Array( 16 * jointNames.length );
                                        var i, len = jointNames.length;
                                        for (i = 0; i < len; i++) {
                                            var jointName = jointNames[i];
                                            var nodeForJoint = this.joints[jointName];
                                            var joint = this.resources.getEntry(nodeForJoint).object;
                                            if (joint) {
                                                
                                                joint.skin = threeMesh;
                                                joints.push(joint);
                                                threeMesh.skeleton.bones.push(joint);
                                                
                                                var m = skin.inverseBindMatrices;
                                                var mat = new THREE.Matrix4().set(
                                                        m[i * 16 + 0],  m[i * 16 + 4],  m[i * 16 + 8],  m[i * 16 + 12],
                                                        m[i * 16 + 1],  m[i * 16 + 5],  m[i * 16 + 9],  m[i * 16 + 13],
                                                        m[i * 16 + 2],  m[i * 16 + 6],  m[i * 16 + 10], m[i * 16 + 14],
                                                        m[i * 16 + 3],  m[i * 16 + 7],  m[i * 16 + 11], m[i * 16 + 15]
                                                    );
                                                threeMesh.skeleton.boneInverses.push(mat);
                                                threeMesh.skeleton.pose();
                                                
                                            } else {
                                                console.log("WARNING: jointName:"+jointName+" cannot be found in skeleton:"+skeleton);
                                            }
                                        }
                                    }
                                    
                                    if (threeMesh) {
                                        threeMesh.castShadow = true;
                                        node.add(threeMesh);

                                        if (material instanceof THREE.ShaderMaterial) {
                                            materialParams.joints = joints;
                                            var glTFShader = new THREE.glTFShader(material, materialParams, threeMesh, theLoader.rootObj);
                                            THREE.glTFShaders.add(glTFShader);

                                        }
                                    }
                                    
                                }, this);                               
                            }
                            
                        }

                    
                    }, this);
                    
                }
            }
        },
         
        buildSkins: {
            value: function(node) {

                if (node.instanceSkin)
                    this.buildSkin(node);
                
                var children = node.children;
                if (children) {
                    children.forEach( function(child) {
                        this.buildSkins(child);
                    }, this);
                }
            }
        },
        
        createMeshAnimations : {
            value : function(root) {
                    this.buildSkins(root);
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
        
        addNodeAnimationChannel : {
            value : function(name, channel, interp) {
                if (!this.nodeAnimationChannels)
                    this.nodeAnimationChannels = {};
                
                if (!this.nodeAnimationChannels[name]) {
                    this.nodeAnimationChannels[name] = [];
                }
                
                this.nodeAnimationChannels[name].push(interp);
            },
        },
        
        createAnimations : {
            value : function() {
                for (var name in this.nodeAnimationChannels) {
                    var nodeAnimationChannels = this.nodeAnimationChannels[name];
                    var i, len = nodeAnimationChannels.length;
                    //console.log(" animation channels for node " + name);
                    //for (i = 0; i < len; i++) {
                    //  console.log(nodeAnimationChannels[i]);
                    //}
                    var anim = new THREE.glTFAnimation(nodeAnimationChannels);
                    anim.name = "animation_" + name;
                    this.animations.push(anim);                     
                }
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

                                    var path = target.path;
                                    
                                    if (path == "rotation")
                                    {
                                        convertAxisAngleToQuaternion(output.data, output.count);
                                    }
                                    
                                    var interp = {
                                            keys : input.data,
                                            values : output.data,
                                            count : input.count,
                                            target : node.object,
                                            path : path,
                                            type : sampler.interpolation
                                    };
                                    
                                    this.addNodeAnimationChannel(target.id, channel, interp);
                                    interps.push(interp);
                                }
                            }
                        }
                    }
                }               
            }
        },
        
        handleAnimation: {
            value: function(entryID, description, userInfo) {
            
                var self = this;
                theLoader.animationsRequested++;
                var animation = new Animation();
                animation.name = entryID;
                animation.onload = function() {
                    // self.buildAnimation(animation);
                    theLoader.animationsLoaded++;
                    theLoader.animations.push(animation);
                    theLoader.checkComplete();
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
    
                // Load parameter buffers
                var params = Object.keys(parameters);
                params.forEach( function(param) {

                    animation.totalParameters++;
                    var parameter = parameters[param];
                    var accessor = this.resources.getEntry(parameter);
                    if (!accessor)
                        debugger;
                    accessor = accessor.object;
                    var bufferView = this.resources.getEntry(accessor.bufferView);
                    var paramObject = {
                            bufferView : bufferView,
                            byteOffset : accessor.byteOffset,
                            count : accessor.count,
                            componentType : accessor.componentType,
                            type : accessor.type,
                            id : accessor.bufferView,
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

        handleAccessor: {
            value: function(entryID, description, userInfo) {
                // Save attribute entry
                this.resources.setEntry(entryID, description, description);
                return true;
            }
        },

        handleSkin: {
            value: function(entryID, description, userInfo) {
                // Save skin entry
            
                var skin = {
                };
                
                var m = description.bindShapeMatrix;
                skin.bindShapeMatrix = new THREE.Matrix4().set(
                        m[0],  m[4],  m[8],  m[12],
                        m[1],  m[5],  m[9],  m[13],
                        m[2],  m[6],  m[10], m[14],
                        m[3],  m[7],  m[11], m[15]
                    );
                
                skin.jointNames = description.jointNames;
                var inverseBindMatricesDescription = this.resources.getEntry(description.inverseBindMatrices);
                inverseBindMatricesDescription = inverseBindMatricesDescription.description;
                skin.inverseBindMatricesDescription = inverseBindMatricesDescription;
                skin.inverseBindMatricesDescription.id = description.inverseBindMatrices;

                var bufferEntry = this.resources.getEntry(inverseBindMatricesDescription.bufferView);

                var paramObject = {
                        bufferView : bufferEntry,
                        byteOffset : inverseBindMatricesDescription.byteOffset,
                        count : inverseBindMatricesDescription.count,
                        componentType : inverseBindMatricesDescription.componentType,
                        type : inverseBindMatricesDescription.type,
                        id : inverseBindMatricesDescription.bufferView,
                        name : skin.inverseBindMatricesDescription.id             
                };
                
                var context = new InverseBindMatricesContext(paramObject, skin);

                var alreadyProcessedAttribute = THREE.GLTFLoaderUtils.getBuffer(paramObject, inverseBindMatricesDelegate, context);

                var bufferView = this.resources.getEntry(skin.inverseBindMatricesDescription.bufferView);
                skin.inverseBindMatricesDescription.bufferView = 
                    bufferView.object;
                this.resources.setEntry(entryID, skin, description);
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
    loader.initWithPath(url);
    loader.load(new Context(rootObj, 
                        function(obj) {
                        }), 
                null);

    this.loader = loader;
    this.callback = callback;
    this.rootObj = rootObj;
    return rootObj;
}

THREE.glTFLoader.prototype.callLoadedCallback = function() {
    var result = {
            scene : this.rootObj,
            cameras : this.loader.cameras,
            animations : this.loader.animations,
            shaders : this.loader.shaders,
    };
    
    this.callback(result);
}

THREE.glTFLoader.prototype.checkComplete = function() {
    if (this.meshesLoaded == this.meshesRequested 
            && this.shadersLoaded == this.shadersRequested
            && this.animationsLoaded == this.animationsRequested)
    {
        
        for (var i = 0; i < this.pendingMeshes.length; i++) {
            var pending = this.pendingMeshes[i];
            pending.mesh.attachToNode(pending.node);
        }
        
        for (var i = 0; i < this.animationsLoaded; i++) {
            var animation = this.animations[i];
            this.loader.buildAnimation(animation);
        }

        this.loader.createAnimations();
        this.loader.createMeshAnimations(this.rootObj);
        THREE.glTFShaders.bindShaderParameters(this.rootObj);

        this.callLoadedCallback();
    }
}




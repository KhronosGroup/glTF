/* <copyright>
Copyright (c) 2012, Motorola Mobility LLC.
All Rights Reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of Motorola Mobility LLC nor the names of its
  contributors may be used to endorse or promote products derived from this
  software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
</copyright> */
/**
    @module "montage/ui/view.reel"
    @requires montage
    @requires montage/ui/component
*/
var Montage = require("montage").Montage;
var Component = require("montage/ui/component").Component;
require("runtime/dependencies/gl-matrix");
var GLSLProgram = require("runtime/glsl-program").GLSLProgram;
var ResourceManager = require("c2j/helpers/resource-manager").ResourceManager;
var Engine = require("runtime/engine").Engine;
var Material = require("runtime/material").Material;
var Utilities = require("runtime/utilities").Utilities;
var dom = require("montage/ui/dom");
var Point = require("montage/core/geometry/point").Point;
var OrbitCamera = require("runtime/dependencies/camera.js").OrbitCamera;
var TranslateComposer = require("montage/ui/composer/translate-composer").TranslateComposer;
var RuntimeTFLoader = require("runtime/runtime-tf-loader").RuntimeTFLoader;

Material.implicitAnimationsEnabled = true;

/**
    Description TODO
    @class module:"montage/ui/view.reel".view
    @extends module:montage/ui/component.Component
*/
exports.View = Montage.create(Component, /** @lends module:"montage/ui/view.reel".view# */ {

    modelController: {
        value: null
    },

    // API for modelController to trigger a draw
    // TODO I didn't want to leak component needsDraw details, nor did I want to force view.js to
    // react to some specific event from the modelController. This seems the most congenial route.
    update: {
        value: function() {
            this.needsDraw = true;
        }
    },

    scaleFactor: { value: window.devicePixelRatio, writable: true},

    canvas: {
        get: function() {
            if (this.templateObjects) {
                return this.templateObjects.canvas;
            } 
            return null;
        }
    },

    _camera: { value: null, writable: true },

    camera: {
        get: function() {
            return this._camera;
        },
        set: function(value) {
            this._camera = value;
        }
    },

    _engine: { value: null, writable: true },

    engine: {
        get: function() {
            return this._engine;
        },
        set: function(value) {
            this._engine = value;
        }
    },

    scene: {
        get: function() {
            if (this.engine) {
                if (this.engine.rootPass) {
                    return this.engine.rootPass.inputs.scene;
                }
            }
            return null;
        },

        set: function(value) {
            if (this.scene !== value) {
                this._scene = value;
                this.applyScene(value);
                if (this.delegate) {
                    if (this.delegate.sceneDidChange) {
                        this.delegate.sceneDidChange();
                    }
                }
            }
       }
    },

    _scenePath: { value: null, writable: true },

    scenePath: {
        set: function(value) {
            if (value !== this._scenePath) {

                var loader = Object.create(RuntimeTFLoader);

                var readerDelegate = {};
                readerDelegate.loadCompleted = function (scene) {
                    this.totalBufferSize =  loader.totalBufferSize;
                    this.scene = scene;
                    this.needsDraw = true;
                    //FIXME:HACK: loader should be passed as arg, also multiple observers should pluggable here so that the top level could just pick that size info. (for the progress)
                }.bind(this);

                var loader = Object.create(RuntimeTFLoader);
                loader.initWithPath(value);
                loader.delegate = readerDelegate;
                loader.load(null /* userInfo */, null /* options */);

                this._scenePath = value;
            }
        }, 
        get: function() {
            return this._scenePath;
        }
    },

    applyScene: {
        value:function (scene) {
            if (this.engine) {
                if (this.engine.rootPass) {
                    //compute hierarchical bbox for the whole scene
                    //this will be removed from this place when node bounding box become is implemented as hierarchical
                    var ctx = mat4.identity();
                    var node = scene.rootNode;
                    var sceneBBox = null;

                    node.apply( function(node, parent, parentTransform) {
                        var modelMatrix = mat4.create();
                        mat4.multiply( parentTransform, node.transform, modelMatrix);

                        if (node.boundingBox) {
                            var bbox = Utilities.transformBBox(node.boundingBox, modelMatrix);
                            if (sceneBBox) {
                                sceneBBox = Utilities.mergeBBox(bbox, sceneBBox);
                            } else {
                                sceneBBox = bbox;
                            }
                        }

                        return modelMatrix;
                    }, true, ctx);

                    var sceneSize = [(sceneBBox[1][0] - sceneBBox[0][0]) ,
                            (sceneBBox[1][1] - sceneBBox[0][1]) ,
                            (sceneBBox[1][2] - sceneBBox[0][2]) ];

                    //size to fit
                    var scaleFactor = sceneSize[0] > sceneSize[1] ? sceneSize[0] : sceneSize[1];
                    scaleFactor = sceneSize[2] > scaleFactor ? sceneSize[2] : scaleFactor;

                    scaleFactor =  1 / scaleFactor;
                     var scaleMatrix = mat4.scale(mat4.identity(), [scaleFactor, scaleFactor, scaleFactor]);
            
                    var translationVector = vec3.createFrom(    -((sceneSize[0] / 2) + sceneBBox[0][0]), 
                                                        -((sceneSize[1] / 2) + sceneBBox[0][1]),
                                                        -( sceneBBox[0][2]));
                         
                    var translation = mat4.translate(scaleMatrix, [
                                        translationVector[0],
                                        translationVector[1],
                                        translationVector[2]]);

                    mat4.set(translation, scene.rootNode.transform);
                    this.engine.rootPass.inputs.scene = scene;
                    this.needsDraw = true;
                }
            }
        }
    },

    getRelativePositionToCanvas: {
        value: function(event) {
            return dom.convertPointFromPageToNode(this.canvas, Point.create().init(event.pageX, event.pageY));
        }
    },

    prepareForDraw: {
        value: function() {

            var webGLContext = this.canvas.getContext("experimental-webgl", { antialias: true}) ||this.canvas.getContext("webgl", { antialias: true});
            var options = null;
            this.engine = Object.create(Engine);
            this.engine.init(webGLContext, options);
            this.engine.renderer.resourceManager.observers.push(this);

            this.canvas.setAttribute("height", this._height);
            this.canvas.setAttribute("width", this._width);

            this.camera = new MontageOrbitCamera(this.canvas);

            this.camera.maxDistance = 200;
            this.camera.minDistance = 0.0;
            this.camera.setDistance(1.3);//0.9999542236328);
            this.camera.distanceStep = 0.0001;
            this.camera.constrainDistance = false;
            this.camera.setYUp(true);
            this.camera.orbitX = 0.6750000000000003; //values taken out the camera while manipulating the model...
            this.camera.orbitY = -0.5836293856408279;

            this.camera.minOrbitX = 0;//this.camera.orbitX - 0.6;
            this.camera.maxOrbitX = 1.6;

            this.camera.constrainXOrbit = true;

            var center = vec3.createFrom(0,0,0.2);
            this.camera.setCenter(center);

            this.needsDraw = true;

            // TODO the camera does its own listening but doesn't know about our draw system
            // I'm minimizing impact to the dependencies as we get this all working so the listeners
            // here really don't do much other than trigger drawing. They listen on capture
            // to handle the event before the camera stopsPropagation (for whatever reason it does that)
            this.canvas.addEventListener('touchstart', this.start.bind(this), true);
            document.addEventListener('touchend', this.end.bind(this), true);
            document.addEventListener('touchcancel', this.end.bind(this), true);
            document.addEventListener('touchmove', this.move.bind(this), true);
            document.addEventListener('gesturechange', this, true);

            this.canvas.addEventListener('mousedown', this.start.bind(this), true);
            document.addEventListener('mouseup', this.end.bind(this), true);
            document.addEventListener('mousemove', this.move.bind(this), true);
            document.addEventListener('mousewheel', this, true);
        }
    },

    captureMousewheel: {
        value: function() {
            this.needsDraw = true;
        }
    },

    captureGesturechange: {
        value: function() {
            this.needsDraw = true;
        }
    },

    move:{
        value: function (event) {
            this.needsDraw = true;
        }
    },

    start: {
        value: function (event) {
            event.preventDefault();
            this._consideringPointerForPicking = true;
            var position = this.getRelativePositionToCanvas(event);
            this._mousePosition = [position.x, position.y];
        }
    },

    end:{
        value: function (event) {

            if (this._consideringPointerForPicking && event.target === this.canvas) {
                event.preventDefault();
            }

            this._consideringPointerForPicking = false;
            this._mousePosition = null;
        }
    },

    /* returns an array of test results */
    hitTest: {
        value: function(position, options) {
            if (this.engine) {
                if ((this.engine.rootPass) && (this.canvas)) {
                    var viewport = [0, 0, parseInt(this.canvas.getAttribute("width")), parseInt(this.canvas.getAttribute("height"))];
                    return this.engine.rootPass.hitTest(position, viewport, options);
                }
            }
            return null;
        }
    },

    getRenderer: {
        value: function() {
            return this.engine ? this.engine.renderer : null;
        }
    },

    getWebGLContext: {
        value: function() {
            var renderer = this.getRenderer();
            return renderer ? renderer.webGLContext : null;
        }
    },

    getResourceManager: {
        value: function() {
            var renderer = this.getRenderer();
            return renderer ? renderer.resourceManager : null;
        }
    },

    _consideringPointerForPicking: { writable: true, value: false },
    _mousePosition: { writable: true, value : null },
    _floorTextureLoaded : { writable: true, value: false },

    enableReflection: {
        value: true
    },

    drawGradient: {
        value: function() {
            if (!this.engine || !this.scene)
                return;
            if (!this.engine.rootPass.inputs.viewPoint)
                return;
            var gl = this.getWebGLContext();
            var self = this;

            this.engine.renderer.bindedProgram = null;

            var orthoMatrix = mat4.ortho(-1, 1, 1.0, -1, 0, 1000);

            gl.disable(gl.DEPTH_TEST);
            gl.disable(gl.CULL_FACE);
            gl.enable(gl.BLEND);
            gl.blendFunc (gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);

            if (!this._gradientProgram) {
                this._gradientProgram = Object.create(GLSLProgram);

                var vertexShader =  "precision highp float;" +
                                        "attribute vec3 vert;"  +
                                        "attribute vec3 color;"  +
                                         "uniform mat4 u_projMatrix; " +
                                        "varying vec3 v_color;"  +
                                        "void main(void) { " +
                                        "v_color = color;" +
                                        "gl_Position = u_projMatrix * vec4(vert,1.0); }"

                var fragmentShader =    "precision highp float;" +
                                            "varying vec3 v_color;"  +
                                            " void main(void) { " +
                                            " gl_FragColor = vec4(v_color, 0.7); }";

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
                var c2 = [.8, .8, .8];
                var c1 = [0., 0., 0.];
                var vertices = [
                        - 1.0,-1, 0.0,       c1[0], c1[1], c1[2],
                        1.0,-1, 0.0,        c1[0], c1[1], c1[2],
                        -1.0, 1.0, 0.0,     c2[0], c2[1], c2[2],
                        -1.0, 1.0, 0.0,     c2[0], c2[1], c2[2],
                        1.0,-1, 0.0,        c1[0], c1[1], c1[2],
                        1.0, 1.0, 0.0,      c2[0], c2[1], c2[2]];

                    // Init the buffer
                this.vertexBuffer = gl.createBuffer();
                gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexBuffer);
                gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
            }

            gl.bindBuffer(gl.ARRAY_BUFFER, this.vertexBuffer);

            var vertLocation = this._gradientProgram.getLocationForSymbol("vert");
            if (typeof vertLocation !== "undefined") {
                gl.enableVertexAttribArray(vertLocation);
                gl.vertexAttribPointer(vertLocation, 3, gl.FLOAT, false, 24, 0);
            }
            var colorLocation = this._gradientProgram.getLocationForSymbol("color");
            if (typeof colorLocation !== "undefined") {
                gl.enableVertexAttribArray(colorLocation);
                gl.vertexAttribPointer(colorLocation, 3, gl.FLOAT, false, 24, 12);
            }

            this.engine.renderer.bindedProgram = this._gradientProgram;

            var projectionMatrixLocation = this._gradientProgram.getLocationForSymbol("u_projMatrix");
            if (projectionMatrixLocation) {
                this._gradientProgram.setValueForSymbol("u_projMatrix",orthoMatrix);
            }

            this._gradientProgram.commit(gl);
            gl.drawArrays(gl.TRIANGLES, 0, 6);
            gl.disableVertexAttribArray(vertLocation);
            gl.disableVertexAttribArray(colorLocation);
        }
    },

    drawFloor: {
        value: function(cameraMatrix) {

            if (!this.engine || !this.scene)
                return;
            if (!this.engine.rootPass.inputs.viewPoint)
                return;
            var gl = this.getWebGLContext();
            var self = this;
            
            function handleTextureLoaded(image, texture) {
                gl.bindTexture(gl.TEXTURE_2D, texture);
                gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, image);

                gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
                gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);

                gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
                gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
//                gl.generateMipmap(gl.TEXTURE_2D);
                gl.bindTexture(gl.TEXTURE_2D, null);
                self._floorTextureLoaded = true;
                self.needsDraw = true;
            }

            if (!this.floorTexture) {
                this.floorTexture = gl.createTexture();
                var floorImage = new Image();
                floorImage.onload = function() { handleTextureLoaded(floorImage, self.floorTexture); }
                floorImage.src = "assets/images/dropshadow-1.png";
            }

            if (!this._floorTextureLoaded) {
                return;
            }

            this.engine.renderer.bindedProgram = null;

            var viewPoint = this.engine.rootPass.inputs.viewPoint;

            var projectionMatrix = viewPoint.cameras[0].projection.matrix;

            //gl.disable(gl.DEPTH_TEST);
            gl.disable(gl.CULL_FACE);
            //gl.enable(gl.BLEND);

            gl.blendFunc (gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);

               // var textureLoader = new TextureUtil.TextureLoader(ctx);
//                var texture = textureLoader.load(, function(texture) {
  //              });

            if (!this._floorProgram) {
                this._floorProgram = Object.create(GLSLProgram);

                var vertexShader =  "precision highp float;" +
                                    "attribute vec3 vert;"  +
                                    "attribute vec3 color;"  +
                                    "attribute vec2 texcoord;"  +
                                    "varying vec2 v_texcoord;"  +
                                    "uniform mat4 u_projMatrix; " +
                                    "uniform mat4 u_mvMatrix; " +
                                    "varying vec3 v_color;"  +
                                    "varying vec3 v_coord;" +
                                    "void main(void) { " +
                                    "v_color = color;" +
                                    "v_texcoord = vec2(texcoord.x, 1.-texcoord.y) - vec2(-0.5,0.35);" +
                                    "v_coord = vert;" +
                                    "gl_Position = u_projMatrix * u_mvMatrix * vec4(vert,1.0); }";
//                                    " v_coord = (u_mvMatrix * vec4(vert,1.0)).xyz;" +

                var fragmentShader =    "precision highp float;" +
                                        "varying vec3 v_color;"  +
                                        "varying vec3 v_coord;"  +
                                        "varying vec2 v_texcoord;"  +
                                        "uniform sampler2D u_image;" +
                                        " void main(void) { " +
                                         " vec4 imgcol =  vec4(texture2D(u_image, v_texcoord));" +
                                        // " float dist =   sqrt(dot(v_coord,v_coord));" +
                                        //" dist = 1. - min(dist * (0.009 - 0.007), 1.);" +
                                     " gl_FragColor = vec4(1.,1.,0.,1.);  }";
                                   //   " gl_FragColor = vec4( imgcol.xyz,  imgcol.a);  }";
                                    //  " gl_FragColor = vec4(0.,0.,0.,1.);  }";

//                                       " gl_FragColor = vec4(imgcol.xyz * dist, imgcol.a * dist * 0.1);  }";
                                       // " gl_FragColor = vec4(dist,dist,dist,1);  }";
                                      // " gl_FragColor = vec4( ( imgcol.xyz) * ( imgcol.a) * dist, imgcol.a * dist);  }";

                this._floorProgram.initWithShaders( { "x-shader/x-vertex" : vertexShader , "x-shader/x-fragment" : fragmentShader } );
                if (!this._floorProgram.build(gl))
                    console.log(this._floorProgram.errorLogs);
            }

                if (!this.floorVertexBuffer) {
                    /*
                        2/3----5
                        | \   |
                        |  \  |
                        |   \ |
                        0----1/4
                    */
                    var size = 160;
                    var texSize = 1;
                    var offset = 100;
                    var vertices = [
                        -size,-size, 0.0,       1.0, 1.0, 1.0,      -texSize,-texSize,
                        size,-size, 0.0,        1.0, 1.0, 1.0,      texSize,-texSize,
                        -size, size, 0.0,     0.0, 0.0, 0.0,        -texSize,texSize,

                        -size, size, 0.0,     0.0, 0.0, 0.0,        -texSize,texSize,
                        size,-size, 0.0,        1.0, 1.0, 1,        texSize,-texSize,
                        size, size, 0.0,      0.0, .0, 0.0,         texSize,texSize
                   ];

                    // Init the buffer
                    this.floorVertexBuffer = gl.createBuffer();
                    gl.bindBuffer(gl.ARRAY_BUFFER, this.floorVertexBuffer);
                    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);
                }

                gl.activeTexture(gl.TEXTURE0);

                gl.bindTexture(gl.TEXTURE_2D, this.floorTexture);

                gl.bindBuffer(gl.ARRAY_BUFFER, this.floorVertexBuffer);

                var vertLocation = this._floorProgram.getLocationForSymbol("vert");
                if (typeof vertLocation !== "undefined") {
                    gl.enableVertexAttribArray(vertLocation);
                    gl.vertexAttribPointer(vertLocation, 3, gl.FLOAT, false, 32, 0);
                }
                var colorLocation = this._floorProgram.getLocationForSymbol("color");
                if (typeof colorLocation !== "undefined") {
                    gl.enableVertexAttribArray(colorLocation);
                    gl.vertexAttribPointer(colorLocation, 3, gl.FLOAT, false, 32, 12);
                }
                var texLocation = this._floorProgram.getLocationForSymbol("texcoord");
                if (typeof texLocation !== "undefined") {
                    gl.enableVertexAttribArray(texLocation);
                    gl.vertexAttribPointer(texLocation, 2, gl.FLOAT, false, 32, 24);
                }


                this._floorProgram.setValueForSymbol("u_image", 0);

                this.engine.renderer.bindedProgram = this._floorProgram;

                var projectionMatrixLocation = this._floorProgram.getLocationForSymbol("u_projMatrix");
                if (projectionMatrixLocation) {
                    this._floorProgram.setValueForSymbol("u_projMatrix",projectionMatrix);
                }

                var mvMatrixLocation = this._floorProgram.getLocationForSymbol("u_mvMatrix");
                if (mvMatrixLocation) {
                    this._floorProgram.setValueForSymbol("u_mvMatrix",cameraMatrix);
                }

                this._floorProgram.commit(gl);
                gl.drawArrays(gl.TRIANGLES, 0, 6);
                gl.disableVertexAttribArray(vertLocation);
                gl.disableVertexAttribArray(colorLocation);
                gl.disableVertexAttribArray(texLocation);

                gl.disable(gl.BLEND);
                gl.bindTexture(gl.TEXTURE_2D, null);
            }
    },

    displayBBOX: {
        value: function(mesh, cameraMatrix, modelMatrix) {
            var bbox = mesh.boundingBox;
            if (mesh.step === 0)
                return;

            if (!this.engine || !this.scene)
                return;
            if (!this.engine.rootPass.inputs.viewPoint)
                return;
            var gl = this.getWebGLContext();
            var self = this;

            this.engine.renderer.bindedProgram = null;

            var viewPoint = this.engine.rootPass.inputs.viewPoint;
            var projectionMatrix = viewPoint.cameras[0].projection.matrix;

            if (mesh.step < 1.) {
                gl.enable(gl.BLEND);
                gl.blendFunc (gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);
            }

            gl.disable(gl.CULL_FACE);

            if (!this._BBOXProgram) {
                this._BBOXProgram = Object.create(GLSLProgram);

                var vertexShader =  "precision highp float;" +
                                    "attribute vec3 vert;"  +
                                    "uniform mat4 u_projMatrix; " +
                                    "uniform mat4 u_vMatrix; " +
                                    "uniform mat4 u_mMatrix; " +
                                    "void main(void) { " +
                                    "gl_Position = u_projMatrix * u_vMatrix * u_mMatrix * vec4(vert,1.0); }";

                var fragmentShader =    "precision highp float;" +
                                    "uniform float u_transparency; " +
                                        " void main(void) { " +
                                     " gl_FragColor = vec4(vec3(1.,1.,1.) , u_transparency);" +
                                    "}";

                this._BBOXProgram.initWithShaders( {    "x-shader/x-vertex" : vertexShader , 
                                                        "x-shader/x-fragment" : fragmentShader } );
                if (!this._BBOXProgram.build(gl))
                    console.log(this._BBOXProgram.errorLogs);
            }

            var min = [bbox[0][0], bbox[0][1], bbox[0][2]];
            var max = [bbox[1][0], bbox[1][1], bbox[1][2]];

            var X = 0;
            var Y = 1;
            var Z = 2;

            if (!this._BBOXIndices) {
                //should be strip that but couldn't figure yet why I can't couldn't close the strip
                // i was expecting that repeating the index (like for triangles) would work
                var indices = [ 0, 1,
                                1, 2,
                                2, 3,
                                3, 0,
                                4, 5,
                                5, 6,
                                6, 7,
                                7, 4,
                                3, 7,
                                2, 6,
                                0, 4,
                                1, 5];

                this._BBOXIndices = gl.createBuffer();
                gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this._BBOXIndices);
                gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(indices), gl.STATIC_DRAW);
            }
            
            gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, this._BBOXIndices);

            if (!this._BBOXVertexBuffer) {
                this._BBOXVertexBuffer = gl.createBuffer();
                gl.bindBuffer(gl.ARRAY_BUFFER, this._BBOXVertexBuffer);
            }
            gl.bindBuffer(gl.ARRAY_BUFFER, this._BBOXVertexBuffer);

            var incrBox = (1 - mesh.step) * 5.;

            if (mesh.loaded) {
                min[X] -= incrBox;
                min[Y] -= incrBox;
                min[Z] -= incrBox;
                max[X] += incrBox;
                max[Y] += incrBox;
                max[Z] += incrBox;
            }
            var vertices = [
                    max[X], min[Y], min[Z], 
                    max[X], max[Y], min[Z], 
                    min[X], max[Y], min[Z], 
                    min[X], min[Y], min[Z], 
                    max[X], min[Y], max[Z], 
                    max[X], max[Y], max[Z], 
                    min[X], max[Y], max[Z], 
                    min[X], min[Y], max[Z]
            ];
            gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);

            var vertLocation = this._BBOXProgram.getLocationForSymbol("vert");
            if (typeof vertLocation !== "undefined") {
                gl.enableVertexAttribArray(vertLocation);
                gl.vertexAttribPointer(vertLocation, 3, gl.FLOAT, false, 12, 0);
            }

            this.engine.renderer.bindedProgram = this._BBOXProgram;

            var projectionMatrixLocation = this._BBOXProgram.getLocationForSymbol("u_projMatrix");
            if (projectionMatrixLocation) {
                this._BBOXProgram.setValueForSymbol("u_projMatrix",projectionMatrix);
            }

            var mMatrixLocation = this._BBOXProgram.getLocationForSymbol("u_mMatrix");
            if (mMatrixLocation) {
                this._BBOXProgram.setValueForSymbol("u_mMatrix",modelMatrix);
            }

            var vMatrixLocation = this._BBOXProgram.getLocationForSymbol("u_vMatrix");
            if (vMatrixLocation) {
                this._BBOXProgram.setValueForSymbol("u_vMatrix",cameraMatrix);
            }

            var transparency = this._BBOXProgram.getLocationForSymbol("u_transparency");
            if (transparency) {
                this._BBOXProgram.setValueForSymbol("u_transparency",mesh.step);
            }

            this._BBOXProgram.commit(gl);
            //void drawElements(GLenum mode, GLsizei count, GLenum type, GLintptr offset);
            gl.drawElements(gl.LINES, 24, gl.UNSIGNED_SHORT, 0);
            gl.disableVertexAttribArray(vertLocation);

            gl.disable(gl.BLEND);
            gl.enable(gl.CULL_FACE);

        }
    },


    displayAllBBOX: {
        value: function(cameraMatrix) {
            if (!this.scene)
                return;

            var ctx = mat4.identity();
            var node = this.scene.rootNode;
            var self = this;

            node.apply( function(node, parent, parentTransform) {

                var modelMatrix = mat4.create();
                mat4.multiply( parentTransform, node.transform, modelMatrix);
                if (node.boundingBox) {
                    if (node.meshes) {
                        if (node.meshes.length > 0) {
                            node.meshes.forEach( function(mesh) {
                                var incr = 0.02;
                                if (!mesh.loaded) {
                                    if (mesh.step < 1)
                                        mesh.step += incr;
                                    else 
                                        mesh.step = 1;
                                } else {
                                    if (mesh.step > 0)
                                        mesh.step -= incr;
                                    else 
                                        mesh.step = 0;
                                }
                                if (mesh.step !== 0) {
                                    var nodeMatrix = mat4.create();
                                    var scaledModelMatrix = mat4.create();
                                    var scale = 1.0;//1. + (1 - mesh.step) * 0.3;
                                    var scaleMatrix = mat4.scale(mat4.identity(), vec3.createFrom(scale,scale,scale));
                                    mat4.multiply( node.transform, scaleMatrix , nodeMatrix);
                                    mat4.multiply( parentTransform, nodeMatrix, scaledModelMatrix);
                                }

                                self.displayBBOX(mesh, cameraMatrix, scaledModelMatrix);
                            }, this);
                        }
                    }
                }
                return modelMatrix;
            }, true, ctx);
        }
    },


    _width: {
        value: null
    },

    width: {
        get: function() {
            return this._width;
        },
        set: function(value) {
            if (value === this._width) {
                return;
            }

            this._width = value * this.scaleFactor;

            this.needsDraw = true;
        }
    },

    _height: {
        value: null
    },

    height: {
        get: function() {
            return this._height;
        },
        set: function(value) {
            if (value === this._height) {
                return;
            }

            this._height = value * this.scaleFactor;

            this.needsDraw = true;
        }
    },
    _cameraAnimating:{
        value:true
    },

    cameraAnimating:{
        get:function () {
            return this._cameraAnimating;
        },
        set:function (value) {
            this.cameraAnimatingXVel = 0;
            this.cameraAnimatingYVel = 0;
            this._cameraAnimating = value;
        }
    },

    cameraAnimatingXVel:{
        value: 0
    },
    cameraAnimatingYVel:{
        value: 0
    },

    draw: {
        value: function() {
            var self = this;

            if (null == this.width || null == this.height) {
                return;
            }

            if(this.cameraAnimating) {
                if (this.cameraAnimatingXVel < 0.0013) {
                    this.cameraAnimatingXVel += 0.00001
                }
                if (this.cameraAnimatingYVel > -0.0005) {
                    this.cameraAnimatingYVel -= 0.000005
                }

                this.camera.orbit(this.cameraAnimatingXVel, this.cameraAnimatingYVel);
                this.needsDraw = true;
            }

            var webGLContext = this.getWebGLContext(),
                renderer,
                width,
                height;

            if (webGLContext) {
                webGLContext.clearColor(0,0,0,0.);
                webGLContext.clear(webGLContext.DEPTH_BUFFER_BIT | webGLContext.COLOR_BUFFER_BIT);
            }
            if (this.delegate) {
                if (this.delegate.willDraw)   
                    this.delegate.willDraw(this);
            }
            if (this.scene) {
                renderer = this.engine.renderer;
                if (webGLContext) {
                    this.canvas.setAttribute("width", this._width);
                    width = this._width;

                    this.canvas.setAttribute("height", this._height);
                    height = this._height;

                    if (this.scaleFactor > 1) {
                        var offsetX = -(this.width/2);
                        var offsetY = -(this.height/2);
                        var scale = 1./( this.scaleFactor);

                        this.canvas.style["-webkit-transform"] =  "scale("+scale+","+scale+") translateZ(0) translate("+ offsetX +"px,"+offsetY+"px)";
                    }
                    var viewPoint = this.engine.rootPass.inputs.viewPoint;
                    if (!viewPoint) {
                        return;
                    }
                    var cameraMatrix = this.camera.getViewMat();
                    mat4.inverse(cameraMatrix, viewPoint.transform);

                    webGLContext.viewport(0, 0, width, height);

                    //configure projection
                    viewPoint.cameras[0].projection.aspectRatio = width / height;
                    viewPoint.cameras[0].projection.zfar = 10000;
                    viewPoint.cameras[0].projection.zmin = 1;

                    /* ------------------------------------------------------------------------------------------------------------
                        Draw reflected car
                            - enable depth testing
                            - enable culling
                     ------------------------------------------------------------------------------------------------------------ */
                    if(this.enableReflection) {
                        webGLContext.depthFunc(webGLContext.LESS);
                        webGLContext.enable(webGLContext.DEPTH_TEST);
                        webGLContext.frontFace(webGLContext.CW);
                        var savedTr = mat4.create();

                        var node = this.scene.rootNode;

                        //save car matrix
                        mat4.set(this.scene.rootNode.transform, savedTr);
                        webGLContext.depthMask(true);

                        var translationMatrix = mat4.translate(mat4.identity(), [0, 0, 0 ]);
                        var scaleMatrix = mat4.scale(translationMatrix, [1, 1, -1]);
                        mat4.multiply(scaleMatrix, node.transform) ;
                        mat4.set(scaleMatrix, node.transform);

                        this.engine.render();
                        webGLContext.depthMask(true);

                        //restore car matrix
                        mat4.set(savedTr, node.transform);
                    }
                    
                    //restore culling order
                    webGLContext.frontFace(webGLContext.CCW);

                    webGLContext.disable(webGLContext.DEPTH_TEST);
                    webGLContext.depthMask(false);
                    this.drawGradient();
                    //this.drawFloor(cameraMatrix);
                    webGLContext.depthMask(true);

                    webGLContext.depthFunc(webGLContext.LESS);
                    webGLContext.enable(webGLContext.DEPTH_TEST);
                    webGLContext.enable(webGLContext.CULL_FACE);
                    webGLContext.disable(webGLContext.BLEND);

                    this.engine.render();

                    webGLContext.flush();

                    var error = webGLContext.getError();
                    if (error != webGLContext.NO_ERROR) {
                        console.log("gl error"+webGLContext.getError());
                    }
                    
                    this.displayAllBBOX(cameraMatrix);
                }
            }
        }
    },

    didDraw: {
        value: function() {
            if (this.delegate && typeof this.delegate.didDraw === "function") {
                this.delegate.didDraw(this);
            }
        }
    },

    resourceAvailable: {
        value: function(resource) {
            this.needsDraw = true;
        }
    },

    templateDidLoad: {
        value: function() {            
            var composer = TranslateComposer.create(),
                self = this;
            translateComposer = composer;
            translateComposer.hasMomentum = true;
            translateComposer.allowFloats = true;
            translateComposer.pointerSpeedMultiplier = 0.15;
            this.addComposerForElement(composer, this.canvas);

            var animationTimeout = null;

            translateComposer.addPropertyChangeListener("translateY", function(notification) {
                self._consideringPointerForPicking = false;
                self.needsDraw = true;
            });

            translateComposer.addPropertyChangeListener("translateX", function(notification) {
                self._consideringPointerForPicking = false;
                self.needsDraw = true;
            });

            translateComposer.addEventListener('translateStart', function (event) {
                self.cameraAnimating = false;
                if(animationTimeout) {
                    clearTimeout(animationTimeout);
                }
            }, false);

            translateComposer.addEventListener('translateEnd', function () {
                animationTimeout = setTimeout(function() {
                    self.cameraAnimating = true;
                    self.needsDraw = true;
                }, 3000)
            }, false);

        }
    },

});

var translateComposer = null;

var MontageOrbitCamera = OrbitCamera;
MontageOrbitCamera.prototype = Montage.create(OrbitCamera.prototype);

MontageOrbitCamera.prototype._hookEvents = function (element) {
    var self = this, moving = false,
        lastX, lastY;

    //==============
    // Mouse Events
    //==============
    translateComposer.addEventListener('translateStart', function (event) {
        moving = true;
        lastX = event.translateX;
        lastY = event.translateY;
    }, false);

    translateComposer.addPropertyChangeListener("translateY", function(notification) {
        if (moving) {
            var xDelta = notification.target.translateX  - lastX,
                yDelta = notification.target.translateY  - lastY;

            lastX = notification.target.translateX;
            lastY = notification.target.translateY;

            self.orbit(xDelta * 0.013, yDelta * 0.013);
        }
    });

    translateComposer.addPropertyChangeListener("translateX", function(notification) {
        if (moving) {
            var xDelta = notification.target.translateX  - lastX,
                yDelta = notification.target.translateY  - lastY;

            lastX = notification.target.translateX;
            lastY = notification.target.translateY;

            self.orbit(xDelta * 0.013, yDelta * 0.013);
        }
    });

    translateComposer.addEventListener('translateEnd', function () {
        moving = false;
    }, false);

    element.addEventListener('mousewheel', function (event) {
        self.setDistance(-self._distance[2] + (event.wheelDeltaY * self.distanceStep));
        event.preventDefault();
    }, false);

    element.addEventListener('gesturestart', function (event) {
        self.initialDistance = self._distance[2];
        event.preventDefault();
    }, false);

    element.addEventListener('gesturechange', function (event) {
        self.setDistance(-1 * self.initialDistance / event.scale);
        event.preventDefault();
    }, false);

};



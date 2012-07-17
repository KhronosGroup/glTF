/**
 * @fileoverview native-img - Utilities for loading textures from formats natively supported by the browser
 * @author Brandon Jones
 * @version 0.1
 */

/*
 * Copyright (c) 2012 Brandon Jones
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 *    2. Altered source versions must be plainly marked as such, and must not
 *    be misrepresented as being the original software.
 *
 *    3. This notice may not be removed or altered from any source
 *    distribution.
 */

define([], function () {

    "use strict";

    var MAX_CACHE_IMAGES = 16;

    var textureImageCache = new Array(MAX_CACHE_IMAGES);
    var cacheTop = 0;
    var remainingCacheImages = MAX_CACHE_IMAGES;
    var pendingTextureRequests = [];

    var TextureImageLoader = function(loadedCallback) {
        var self = this;

        this.gl = null;
        this.texture = null;
        this.callback = null;
        this.generateMipmaps = true;

        this.image = new Image();
        this.image.addEventListener("load", function() {
            var gl = self.gl;
            gl.bindTexture(gl.TEXTURE_2D, self.texture);
            gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA, gl.UNSIGNED_BYTE, self.image);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
            gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, self.generateMipmaps > 1 ? gl.LINEAR_MIPMAP_LINEAR : gl.LINEAR);
            if(self.generateMipmaps) {
                gl.generateMipmap(gl.TEXTURE_2D);
            }

            loadedCallback(self);
            if(self.callback) { self.callback(self.texture); }
        });
    };

    TextureImageLoader.prototype.loadTexture = function(gl, src, texture, generateMipmaps, callback) {
        this.gl = gl;
        this.texture = texture;
        this.generateMipmaps = (generateMipmaps !== false);
        this.callback = callback;

        this.image.src = src;
    };

    var PendingTextureRequest = function(gl, src, texture, generateMipmaps, callback) {
        this.gl = gl;
        this.src = src;
        this.texture = texture;
        this.generateMipmaps = (generateMipmaps !== false);
        this.callback = callback;
    };

    function releaseTextureImageLoader(til) {
        var req;
        if(pendingTextureRequests.length) {
            req = pendingTextureRequests.shift();
            til.loadTexture(req.gl, req.src, req.texture, req.generateMipmaps, req.callback);
        } else {
            textureImageCache[cacheTop++] = til;
        }
    }

    /* Public Functions */

    function loadIMGTextureEx(gl, src, texture, generateMipmaps, callback) {
        var til;

        if(cacheTop) {
            til = textureImageCache[--cacheTop];
            til.loadTexture(gl, src, texture, generateMipmaps, callback);
        } else if (remainingCacheImages) {
            til = new TextureImageLoader(releaseTextureImageLoader);
            til.loadTexture(gl, src, texture, generateMipmaps, callback);
            --remainingCacheImages;
        } else {
            pendingTextureRequests.push(new PendingTextureRequest(gl, src, texture, generateMipmaps, callback));
        }
    }

    function loadIMGTexture(gl, src, callback) {
        var texture = gl.createTexture();
        loadIMGTextureEx(gl, src, texture, true, callback);
        return texture;
    }

    return {
        loadIMGTextureEx: loadIMGTextureEx,
        loadIMGTexture: loadIMGTexture
    };

});
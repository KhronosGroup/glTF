/**
 * @fileoverview texture-util - Utilities for loading texture files
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

define([
    "texture-util/native-img",
    "texture-util/dds",
    "texture-util/crunch",
    "texture-util/tga"
], function (nativeImg, dds, crunch, tga) {

    var vendorPrefixes = ["", "WEBKIT_", "MOZ_"];

    function getExtension(gl, name) {
        var i, ext;
        for(i in vendorPrefixes) {
            ext = gl.getExtension(vendorPrefixes[i] + name);
            if (ext) {
                return ext;
            }
        }
        return null;
    }

    function TextureLoader(gl) {
        this.gl = gl;
        this.s3tc = getExtension(gl, "WEBGL_compressed_texture_s3tc");
    }

    TextureLoader.prototype.loadEx = function(src, texture, mipmaps, callback, type) {
        var idx;

        if(!type) {
            idx = src.lastIndexOf(".");
            type = src.substring(idx+1);
        }

        type = type.toLowerCase();
        
        switch(type) {
            case "jpg":
            case "jpeg":
            case "png":
            case "gif":
            case "bmp":
                nativeImg.loadIMGTextureEx(this.gl, src, texture, mipmaps, callback);
                break;

            case "dds":
                dds.loadDDSTextureEx(this.gl, this.s3tc, src, texture, mipmaps, callback);
                break;
                
            case "crn":
                crunch.loadCRNTextureEx(this.gl, this.s3tc, src, texture, mipmaps, callback);
                break;

            case "tga":
                tga.loadTGATextureEx(this.gl, src, texture, mipmaps, callback);
                break;

            // Really should support PVR in here as well

            default:
                console.error("Unknown image type:", type);
                break;
        }
    };

    TextureLoader.prototype.load = function(src, callback, type) {
        var texture = this.gl.createTexture();
        this.loadEx(src, texture, true, callback);
        return texture;
    };

    return {
        TextureLoader: TextureLoader,
        nativeImg: nativeImg,
        dds: dds,
        crunch: crunch,
        tga: tga
    };
});
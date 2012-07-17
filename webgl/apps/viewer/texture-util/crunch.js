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
    "texture-util/dds",
    "texture-util/crn_decomp.js"
], function (dss) {

    // If you want to force crunch to decode on the main browser process
    // set this to false. Otherwise it will decode in a worker by default.
    var CRUNCH_DECODE_IN_WORKER = true;

    // Taken from crnlib.h
    var cCRNFmtInvalid = -1;

    var cCRNFmtDXT1 = 0;
    var cCRNFmtDXT3 = 1;
    var cCRNFmtDXT5 = 2;

    // Various DXT5 derivatives
    var cCRNFmtDXT5_CCxY = 3;    // Luma-chroma
    var cCRNFmtDXT5_xGxR = 4;    // Swizzled 2-component
    var cCRNFmtDXT5_xGBR = 5;    // Swizzled 3-component
    var cCRNFmtDXT5_AGBR = 6;    // Swizzled 4-component

    // ATI 3DC and X360 DXN
    var cCRNFmtDXN_XY = 7;
    var cCRNFmtDXN_YX = 8;

    // DXT5 alpha blocks only
    var cCRNFmtDXT5A = 9;

    function arrayBufferCopy(src, dst, dstByteOffset, numBytes) {
        var dst32Offset = dstByteOffset / 4,
            tail = (numBytes % 4),
            src32 = new Uint32Array(src.buffer, 0, (numBytes - tail) / 4),
            dst32 = new Uint32Array(dst.buffer),
            i;

        for (i = 0; i < src32.length; i++) {
            dst32[dst32Offset + i] = src32[i];
        }
        for (i = numBytes - tail; i < numBytes; i++) {
            dst[dstByteOffset + i] = src[i];
        }
    }

    function uploadCRNLevels(gl, ext, arrayBuffer, loadMipmaps) {
        var bytes = new Uint8Array(arrayBuffer),
            srcSize = arrayBuffer.byteLength,
            src = Module._malloc(srcSize),
            format, internalFormat, dst, dstSize,
            width, height, levels, dxtData, rgb565Data, i;
        
        arrayBufferCopy(bytes, Module.HEAPU8, src, srcSize);

        format = Module._crn_get_dxt_format(src, srcSize);
        
        switch(format) {
            case cCRNFmtDXT1:
                internalFormat = ext ? ext.COMPRESSED_RGB_S3TC_DXT1_EXT : null;
                break;

            case cCRNFmtDXT3:
                internalFormat = ext ? ext.COMPRESSED_RGBA_S3TC_DXT3_EXT : null;
                break;

            case cCRNFmtDXT5:
                internalFormat = ext ? ext.COMPRESSED_RGBA_S3TC_DXT5_EXT : null;
                break;

            default:
                console.error("Unsupported image format");
                return 0;
        }

        width = Module._crn_get_width(src, srcSize);
        height = Module._crn_get_height(src, srcSize);
        levels = Module._crn_get_levels(src, srcSize);
        dstSize = Module._crn_get_uncompressed_size(src, srcSize, 0);
        dst = Module._malloc(dstSize);

        if(ext) {
            /*Module._crn_decompress(src, srcSize, dst, dstSize);
            dxtData = new Uint8Array(Module.HEAPU8.buffer, dst, dstSize);
            gl.compressedTexImage2D(gl.TEXTURE_2D, 0, internalFormat, width, height, 0, dxtData);*/

            for(i = 0; i < levels; ++i) {
                if(i) {
                    dstSize = Module._crn_get_uncompressed_size(src, srcSize, i);
                }
                Module._crn_decompress(src, srcSize, dst, dstSize, i);
                dxtData = new Uint8Array(Module.HEAPU8.buffer, dst, dstSize);

                gl.compressedTexImage2D(gl.TEXTURE_2D, i, internalFormat, width, height, 0, dxtData);
                width *= 0.5;
                height *= 0.5;
            }
        } else {
            if(format == cCRNFmtDXT1) {
                Module._crn_decompress(src, srcSize, dst, dstSize, 0);
                rgb565Data = dss.dxtToRgb565(Module.HEAPU16, dst / 2, width, height);
                gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGB, width, height, 0, gl.RGB, gl.UNSIGNED_SHORT_5_6_5, rgb565Data);
                if(loadMipmaps) {
                    gl.generateMipmap(gl.TEXTURE_2D);
                }
            } else {
                console.error("No manual decoder for format and no native support");
                return 0;
            }
        }
        
        Module._free(src);
        Module._free(dst);

        return 1;
    }

    var loadCRNTextureEx;

    if(CRUNCH_DECODE_IN_WORKER) {
        loadCRNTextureEx = (function() {
            var PendingCrunchTextureRequest = function(gl, ext, texture, callback) {
                this.gl = gl;
                this.ext = ext;
                this.texture = texture;
                this.callback = callback;
            };

            var pendingCrunchTextures = {};
            var crunchWorker = new Worker("texture-util/crunch-worker.js");
            
            crunchWorker.onmessage = function(msg) {
                var data = msg.data;
                var pendingTexture = pendingCrunchTextures[data.src];

                if(data.error) {
                    console.error("Error in Crunch worker:", data.error);
                    if(pendingTexture.callback) {
                        pendingTexture.callback(pendingTexture.texture);
                    }
                    delete pendingCrunchTextures[data.src];
                }

                var gl = pendingTexture.gl;
                var ext = pendingTexture.ext;

                var internalFormat;
                switch(data.format) {
                    case cCRNFmtDXT1:
                        internalFormat = ext ? ext.COMPRESSED_RGB_S3TC_DXT1_EXT : null;
                        break;

                    case cCRNFmtDXT3:
                        internalFormat = ext ? ext.COMPRESSED_RGBA_S3TC_DXT3_EXT : null;
                        break;

                    case cCRNFmtDXT5:
                        internalFormat = ext ? ext.COMPRESSED_RGBA_S3TC_DXT5_EXT : null;
                        break;

                    default:
                        console.error("Unsupported image format");
                        return;
                }

                gl.bindTexture(gl.TEXTURE_2D, pendingTexture.texture);

                if(ext) {
                    gl.compressedTexImage2D(gl.TEXTURE_2D, data.level, internalFormat, data.width, data.height, 0, data.dxtData);
                } else {
                    gl.texImage2D(gl.TEXTURE_2D, data.level, gl.RGB, data.width, data.height, 0, gl.RGB, gl.UNSIGNED_SHORT_5_6_5, data.dxtData);
                    if(data.completed && data.levels) {
                        gl.generateMipmap(gl.TEXTURE_2D);
                    }
                }

                if(data.completed) {
                    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
                    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, data.levels > 1 ? gl.LINEAR_MIPMAP_LINEAR : gl.LINEAR);

                    if(pendingTexture.callback) {
                        pendingTexture.callback(pendingTexture.texture);
                    }

                    delete pendingCrunchTextures[data.src];
                }
            };

            return function (gl, ext, src, texture, loadMipmaps, callback) {
                pendingCrunchTextures[src] = new PendingCrunchTextureRequest(gl, ext, texture, callback);
                crunchWorker.postMessage({src: src, ddsSupported: !!ext, loadMipmaps: loadMipmaps});
            };
        })();
    } else {
        /**
         * Creates a texture from the crunched texture at the given URL.
         *
         * @param {WebGLRenderingContext} gl WebGL rendering context
         * @param {WebGLCompressedTextureS3TC} ext WEBGL_compressed_texture_s3tc extension object
         * @param {string} src URL to DDS file to be loaded
         * @param {function} [callback] callback to be fired when the texture has finished loading
         *
         * @returns {WebGLTexture} New texture that will receive the DDS image data
         */
        loadCRNTextureEx = function (gl, ext, src, texture, loadMipmaps, callback) {
            var xhr = new XMLHttpRequest();
            
            xhr.open('GET', src, true);
            xhr.responseType = "arraybuffer";
            xhr.onload = function() {
                if(this.status == 200) {
                    gl.bindTexture(gl.TEXTURE_2D, texture);
                    var mipmaps = uploadCRNLevels(gl, ext, this.response, loadMipmaps);
                    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
                    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, mipmaps > 1 ? gl.LINEAR_MIPMAP_LINEAR : gl.LINEAR);
                }
                
                if(callback) {
                    callback(texture);
                }
            };
            xhr.send(null);
        };
    }

    

    /**
     * Creates a texture from the crunched texture at the given URL. Simple shortcut for the most common use case
     *
     * @param {WebGLRenderingContext} gl WebGL rendering context
     * @param {WebGLCompressedTextureS3TC} ext WEBGL_compressed_texture_s3tc extension object
     * @param {string} src URL to DDS file to be loaded
     * @param {function} [callback] callback to be fired when the texture has finished loading
     *
     * @returns {WebGLTexture} New texture that will receive the DDS image data
     */
    function loadCRNTexture(gl, ext, src, callback) {
        var texture = gl.createTexture();
        loadCRNTextureEx(gl, ext, src, texture, true, callback);
        return texture;
    }

    return {
        uploadCRNLevels: uploadCRNLevels,
        loadCRNTextureEx: loadCRNTextureEx,
        loadCRNTexture: loadCRNTexture
    };
});
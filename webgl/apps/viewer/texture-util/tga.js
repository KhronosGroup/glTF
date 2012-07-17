/**
 * @fileoverview dds - Utilities for loading DDS texture files
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

    function decodeTGA(arrayBuffer) {
        var content = new Uint8Array(arrayBuffer),
            contentOffset = 18 + content[0],
            imagetype = content[2], // 2 = rgb, only supported format for now
            width = content[12] + (content[13] << 8),
            height = content[14] + (content[15] << 8),
            bpp = content[16], // should be 8,16,24,32
            
            bytesPerPixel = bpp / 8,
            bytesPerRow = width * 4,
            data, i, j, x, y;

        if(!width || !height) {
            console.error("Invalid dimensions");
            return null;
        }

        if (imagetype != 2) {
            console.error("Unsupported TGA format:", imagetype);
            return null;
        }

        data = new Uint8Array(width * height * 4);
        i = contentOffset;

        // Oy, with the flipping of the rows...
        for(y = height-1; y >= 0; --y) {
            for(x = 0; x < width; ++x, i += bytesPerPixel) {
                j = (x * 4) + (y * bytesPerRow);
                data[j] = content[i+2];
                data[j+1] = content[i+1];
                data[j+2] = content[i+0];
                data[j+3] = (bpp === 32 ? content[i+3] : 255);
            }
        }

        return {
            width: width,
            height: height,
            data: data
        };
    }

    function loadTGATextureEx(gl, src, texture, generateMipmaps, callback) {
        var xhr = new XMLHttpRequest();
        
        xhr.open('GET', src, true);
        xhr.responseType = "arraybuffer";
        xhr.onload = function() {
            if(this.status == 200) {
                var imageData = decodeTGA(this.response);

                if(imageData) {
                    gl.bindTexture(gl.TEXTURE_2D, texture);
                    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, imageData.width, imageData.height, 0, gl.RGBA, gl.UNSIGNED_BYTE, imageData.data);
                    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
                    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, generateMipmaps > 1 ? gl.LINEAR_MIPMAP_LINEAR : gl.LINEAR);
                    if(generateMipmaps) {
                        gl.generateMipmap(gl.TEXTURE_2D);
                    }
                }
            }

            if(callback) {
                callback(texture);
            }
        };
        xhr.send(null);
    }

    function loadTGATexture(gl, ext, src, callback) {
        var texture = gl.createTexture();
        loadTGATextureEx(gl, src, texture, true, callback);
        return texture;
    }

    return {
        decodeTGA: decodeTGA,
        loadTGATextureEx: loadTGATextureEx,
        loadTGATexture: loadTGATexture
    };

});
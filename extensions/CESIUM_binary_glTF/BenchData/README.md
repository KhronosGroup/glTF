# Benchmarking Procedure

Each `BenchData` directory contains a COLLADA `.dae` file.  Each of these was converted to several different glTF formats (below).  All files except for stand-alone textures were pre-gzipped, and the resulting files were served using `Content-Encoding: gzip`.

* glTF, all resources separate
* glTF, textures separate, other resources embedded (as data uri)
* glTF, all resources embedded (as data uri)
* Binary glTF, textures separate, other resources embedded (as binary)
* Binary glTF, all resources embedded (as binary)

Finally, these files were loaded and timed using the HelloWorld minimal Cesium application, with the code in Listing A1.  
All data and applications were served locally.
Tests were run in Google Chrome 43.0, using device emulation at 1920x1080 with WiFi network emulation (30 Mbps 2ms RTT).
Each test was run 5 times, and the mean of the results for each test is reported.

## Detailed results

| Model                | Format              | Load time | Size (gzip) | # files |
| :------------------- | :------------------ | --------: | ----------: | ------: |
| **Cesium Air**       | gltf, separate      |    0.33 s |    0.53 MiB |       8 |
|                      | gltf, tex-separate  |    0.30 s |    0.55 MiB |       3 |
|                      | gltf, embedded      |    0.37 s |    0.53 MiB |       1 |
|                      | bgltf, tex-separate |    0.29 s |    0.53 MiB |       3 |
|                      | bgltf, embedded     |    0.31 s |    0.50 MiB |       1 |
|                      |                     |           |             |         |
| **LargeTex**         | gltf, separate      |    3.10 s |    8.48 MiB |       5 |
|                      | gltf, tex-separate  |    3.07 s |    8.48 MiB |       2 |
|                      | gltf, embedded      |    4.59 s |    8.41 MiB |       1 |
|                      | bgltf, tex-separate |    3.04 s |    8.48 MiB |       2 |
|                      | bgltf, embedded     |    3.06 s |    8.31 MiB |       1 |
|                      |                     |           |             |         |
| **TenTex**           | gltf, separate      |    1.54 s |    4.91 MiB |      16 |
|                      | gltf, tex-separate  |    1.48 s |    4.91 MiB |      11 |
|                      | gltf, embedded      |    2.42 s |    4.95 MiB |       1 |
|                      | bgltf, tex-separate |    1.49 s |    4.91 MiB |      11 |
|                      | bgltf, embedded     |    1.62 s |    4.90 MiB |       1 |
|                      |                     |           |             |         |
| **100kTris**         | gltf, separate      |    0.97 s |    2.98 MiB |       4 |
|                      | gltf, tex-separate  |    1.41 s |    3.25 MiB |       1 |
|                      | gltf, embedded      |         ^ |           ^ |       ^ |
|                      | bgltf, tex-separate |    0.91 s |    2.98 MiB |       1 |
|                      | bgltf, embedded     |         ^ |           ^ |       ^ |
|                      |                     |           |             |         |
| **BinaryTree**       | gltf, separate      |    0.76 s |    0.25 MiB |       4 |
|                      | gltf, tex-separate  |    0.94 s |    0.25 MiB |       1 |
|                      | gltf, embedded      |         ^ |           ^ |       ^ |
|                      | bgltf, tex-separate |    0.68 s |    0.18 MiB |       1 |
|                      | bgltf, embedded     |         ^ |           ^ |       ^ |
|                      |                     |           |             |         |
| **SpinningTree**     | gltf, separate      |    0.63 s |    1.24 MiB |       4 |
|                      | gltf, tex-separate  |    0.73 s |    1.34 MiB |       1 |
|                      | gltf, embedded      |         ^ |           ^ |       ^ |
|                      | bgltf, tex-separate |    0.58 s |    1.24 MiB |       1 |
|                      | bgltf, embedded     |         ^ |           ^ |       ^ |
|                      |                     |           |             |         |
| **1200 12th Ave**    | gltf, separate      |    0.85 s |    1.29 MiB |      31 |
|                      | gltf, tex-separate  |    0.85 s |    1.35 MiB |      22 |
|                      | gltf, embedded      |    1.30 s |    1.34 MiB |       1 |
|                      | bgltf, tex-separate |    0.75 s |    1.27 MiB |      22 |
|                      | bgltf, embedded     |    1.07 s |    1.25 MiB |       1 |
|                      |                     |           |             |         |
| **Eau Claire Tower** | gltf, separate      |    0.59 s |    0.60 MiB |      23 |
|                      | gltf, tex-separate  |    0.57 s |    0.60 MiB |      16 |
|                      | gltf, embedded      |    0.73 s |    0.53 MiB |       1 |
|                      | bgltf, tex-separate |    0.55 s |    0.60 MiB |      16 |
|                      | bgltf, embedded     |    0.67 s |    0.53 MiB |       1 |

^ Results are excluded for "embedded" for files with no textures, as
this is exactly the same as "tex-separate".

### Benchmarked formats

All files except for stand-alone textures are always pre-gzipped.

| Format       | Description                                                          |
| :----------- | :------------------------------------------------------------------- |
| gltf         | Standard glTF                                                        |
| bgltf        | Binary glTF                                                          |
| separate     | all resources as separate files                                      |
| tex-separate | only textures as separate files, others embedded                     |
| embedded     | all resources embedded (for gltf, as data uri; for bgltf, as binary) |

### Model thumbnails

| Model                | Thumbnail                   |
| :------------------- | :-------------------------: |
| Cesium Air           | ![](thumb/Cesium_Air.png)   |
| 1200 12th Ave [1]    | ![](thumb/1200_12th.png)    |
| Eau Claire Tower [1] | ![](thumb/EauClaire.png)    |
| LargeTex             | ![](thumb/LargeTex.png)     |
| TenTex               | ![](thumb/TenTex.png)       |
| 100kTris             | ![](thumb/100kTris.png)     |
| BinaryTree           | ![](thumb/BinaryTree.png)   |
| SpinningTree         | ![](thumb/SpinningTree.png) |

### Model statistics

These statistics are for the optimized models which are then converted to the various glTF formats.

| Model            | Type                        | Images | Draw calls | Tris, rendered | Tris, stored | Nodes | Meshes | Materials | Animations |
| :--------------- | :-------------------------- | -----: | ---------: | -------------: | -----------: | ----: | -----: | --------: | ---------: |
| Cesium Air       | Typical model               |      2 |          5 |          5,984 |       ~5,984 |     7 |      2 |         2 |          2 |
| LargeTex         | Large texture               |      1 |          1 |             12 |           12 |     2 |      1 |         1 |          0 |
| TenTex           | Multiple textures           |     10 |         10 |             20 |           20 |    11 |     10 |        10 |          0 |
| 100kTris         | Many triangles              |      0 |          5 |         99,372 |       99,372 |     2 |      5 |         1 |          0 |
| BinaryTree       | Many objects                |      0 |       8191 |         98,292 |           12 | 8,192 |      1 |         1 |          0 |
| SpinningTree     | Many animations & keyframes |      0 |         31 |            372 |           12 |    32 |      1 |         1 |         93 |
| 1200 12th Ave    | Building data, complex      |     21 |       3827 |         30,235 |              |    17 |  1,197 |        26 |          0 |
| Eau Claire Tower | Building data, simpler      |     15 |         61 |            387 |              |    45 |     47 |        17 |          0 |

### Appendix

**Listing A1**: Code used for benchmarking.
```javascript
var viewer = new Cesium.Viewer('cesiumContainer', {globe: false, skyBox: false, skyAtmosphere: false});
var scene = viewer.scene;

var timeModelLoad = function(url) {
    var origin = Cesium.Cartesian3.fromDegrees(0.0, 0.0, 0.0);
    var modelMatrix = Cesium.Transforms.headingPitchRollToFixedFrame(origin, 0.0, 0.0, 0.0);

    var timings = [];

    var f = function() {
        var t0 = performance.now();
        var model = scene.primitives.add(Cesium.Model.fromGltf({
            url : url,
            modelMatrix : modelMatrix,
            asynchronous : true,
            minimumPixelSize : 128
        }));

        model.readyPromise.then(function(model) {
            var t1 = performance.now();
            console.log(t1 - t0);
            model.activeAnimations.addAll({
                loop : Cesium.ModelAnimationLoop.REPEAT
            });
        });
    };

    f();
};

timeModelLoad('/BenchData/Cesium_Air/fmt/CesiumAir.gltf');
```

### References

[1] Models provided by Cube Cities.

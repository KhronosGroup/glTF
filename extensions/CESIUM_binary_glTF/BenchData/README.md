# Benchmarking Procedure

Each `BenchData` directory contains a COLLADA `.dae` file.  Each of these was converted to several different glTF formats: standard glTF, data-uri embedded glTF, and Binary glTF.  All files except for stand-alone textures were gzipped, and the resulting files were served using `Content-Encoding: gzip`.

Finally, these files were loaded and timed using the Cesium [Sandcastle demo](http://cesiumjs.org/Cesium/Apps/Sandcastle/), with the code in Listing A1.

Each test was run 5 times and all 5 were rerun if the results were high in variance (to control for changes in CPU temperature, etc.)  The mean of the results for each test is reported.

| Model                                          |
|:----------------------------------------------:|
| ![`100kTris.png`     ](thumb/100kTris.png    ) |
| ![`1200_12th.png`    ](thumb/1200_12th.png   ) |
| ![`BinaryTree.png`   ](thumb/BinaryTree.png  ) |
| ![`Cesium_Air.png`   ](thumb/Cesium_Air.png  ) |
| ![`EauClaire.png`    ](thumb/EauClaire.png   ) |
| ![`LargeTex.png`     ](thumb/LargeTex.png    ) |
| ![`SpinningTree.png` ](thumb/SpinningTree.png) |
| ![`TenTex.png`       ](thumb/TenTex.png      ) |

**Listing A1**: Code used for benchmarking.
```javascript
var viewer = new Cesium.Viewer('cesiumContainer', {globe: false, skyBox: false, skyAtmosphere: false});
var scene = viewer.scene;

function timeModelLoad(url) {
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
        }).otherwise(function(error){
            window.alert(error);
        });
    };

    f();
}

///////////////////////////////////////////////////////////////////////////

(function() {
    var model = 'Cesium_Air';

    timeModelLoad('../../BenchData/%m/%m.gltf).replace(/%m/g, model);
})();
```

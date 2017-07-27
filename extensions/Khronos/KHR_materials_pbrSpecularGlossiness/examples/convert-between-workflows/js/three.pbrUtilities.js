var THREE;
(function (THREE) {
    var PbrUtilities = (function () {
        function PbrUtilities() {
        }

        const dielectricSpecular = new THREE.Color(0.04, 0.04, 0.04);
        const epsilon = 1e-6;

        PbrUtilities.DielectricSpecular = dielectricSpecular;

        PbrUtilities.ConvertToSpecularGlossiness = function (metallicRoughness) {
            var baseColor = metallicRoughness.baseColor;
            var opacity = metallicRoughness.opacity;
            var metallic = metallicRoughness.metallic;
            var roughness = metallicRoughness.roughness;

            var specular = dielectricSpecular.clone().lerp(baseColor, metallic);

            var oneMinusSpecularStrength = 1 - specular.getMaxComponent();
            var diffuse = baseColor.clone().multiplyScalar((1 - dielectricSpecular.r) * (1 - metallic) / Math.max(oneMinusSpecularStrength, epsilon));

            var glossiness = 1 - roughness;

            return {
                specular: specular,
                opacity: opacity,
                diffuse: diffuse,
                glossiness: glossiness
            };
        }

        PbrUtilities.ConvertToMetallicRoughness = function (specularGlossiness) {
            function solveMetallic(dieletricSpecular, diffuse, specular, oneMinusSpecularStrength) {
                var a = dieletricSpecular;
                var b = diffuse * oneMinusSpecularStrength / (1 - dieletricSpecular) + specular - 2 * dieletricSpecular;
                var c = dieletricSpecular - specular;
                var D = Math.max(b * b - 4 * a * c, 0);
                return THREE.Math.clamp((-b + Math.sqrt(D)) / (2 * a), 0, 1);
            }

            var diffuse = specularGlossiness.diffuse;
            var opacity = specularGlossiness.opacity;
            var specular = specularGlossiness.specular;
            var glossiness = specularGlossiness.glossiness;

            var oneMinusSpecularStrength = 1 - specular.getMaxComponent();
            var metallic = solveMetallic(dielectricSpecular.r, diffuse.getPerceivedBrightness(), specular.getPerceivedBrightness(), oneMinusSpecularStrength);

            var baseColorFromDiffuse = diffuse.clone().multiplyScalar(oneMinusSpecularStrength / Math.max((1 - dielectricSpecular.r) * (1 - metallic), epsilon));
            var baseColorFromSpecular = specular.clone().sub(dielectricSpecular.clone().multiplyScalar((1 - metallic) * (1 / Math.max(metallic, epsilon))));
            var baseColor = baseColorFromDiffuse.clone().lerp(baseColorFromSpecular, metallic * metallic).clamp();

            return {
                baseColor: baseColor,
                opacity: opacity,
                metallic: metallic,
                roughness: 1 - glossiness
            };
        }

        return PbrUtilities;
    }());
    THREE.PbrUtilities = PbrUtilities;
})(THREE || (THREE = {}));

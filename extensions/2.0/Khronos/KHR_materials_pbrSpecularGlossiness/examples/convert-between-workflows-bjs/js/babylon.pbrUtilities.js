var BABYLON;
(function (BABYLON) {
    var PbrUtilities = (function () {
        function PbrUtilities() {
        }

        const dielectricSpecular = new BABYLON.Color3(0.04, 0.04, 0.04);
        const epsilon = 1e-6;

        PbrUtilities.DielectricSpecular = dielectricSpecular;

        PbrUtilities.ConvertToSpecularGlossiness = function (metallicRoughness) {
            var baseColor = metallicRoughness.baseColor;
            var opacity = metallicRoughness.opacity;
            var metallic = metallicRoughness.metallic;
            var roughness = metallicRoughness.roughness;

            var specular = BABYLON.Color3.Lerp(dielectricSpecular, baseColor, metallic);

            var oneMinusSpecularStrength = 1 - specular.getMaxComponent();
            var diffuse = oneMinusSpecularStrength < epsilon ? BABYLON.Color3.Black : baseColor.scale((1 - dielectricSpecular.r) * (1 - metallic) / oneMinusSpecularStrength);

            var glossiness = 1 - roughness;

            return {
                specular: specular,
                opacity: opacity,
                diffuse: diffuse,
                glossiness: glossiness
            };
        }

        PbrUtilities.ConvertToMetallicRoughness = function (specularGlossiness) {
            function solveMetallic(diffuse, specular, oneMinusSpecularStrength) {
                if (specular < dielectricSpecular.r) {
                    return 0;
                }

                var a = dielectricSpecular.r;
                var b = diffuse * oneMinusSpecularStrength / (1 - dielectricSpecular.r) + specular - 2 * dielectricSpecular.r;
                var c = dielectricSpecular.r - specular;
                var D = b * b - 4 * a * c;
                return BABYLON.Scalar.Clamp((-b + Math.sqrt(D)) / (2 * a), 0, 1);
            }

            var diffuse = specularGlossiness.diffuse;
            var opacity = specularGlossiness.opacity;
            var specular = specularGlossiness.specular;
            var glossiness = specularGlossiness.glossiness;

            var oneMinusSpecularStrength = 1 - specular.getMaxComponent();
            var metallic = solveMetallic(diffuse.getPerceivedBrightness(), specular.getPerceivedBrightness(), oneMinusSpecularStrength);

            var baseColorFromDiffuse = diffuse.scale(oneMinusSpecularStrength / (1 - dielectricSpecular.r) / Math.max(1 - metallic, epsilon));
            var baseColorFromSpecular = specular.subtract(dielectricSpecular.scale(1 - metallic)).scale(1 / Math.max(metallic, epsilon));
            var baseColor = BABYLON.Color3.Lerp(baseColorFromDiffuse, baseColorFromSpecular, metallic * metallic).clamp();

            return {
                baseColor: baseColor,
                opacity: opacity,
                metallic: metallic,
                roughness: 1 - glossiness
            };
        }

        return PbrUtilities;
    }());
    BABYLON.PbrUtilities = PbrUtilities;
})(BABYLON || (BABYLON = {}));

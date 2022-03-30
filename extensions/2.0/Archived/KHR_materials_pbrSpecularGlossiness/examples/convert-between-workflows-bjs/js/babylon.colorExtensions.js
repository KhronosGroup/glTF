BABYLON.Color3.prototype.clamp = function (min, max) {
    if (!min) min = 0;
    if (!max) max = 1;
    this.r = BABYLON.Scalar.Clamp(this.r, min, max);
    this.g = BABYLON.Scalar.Clamp(this.g, min, max);
    this.b = BABYLON.Scalar.Clamp(this.b, min, max);
    return this;
}

BABYLON.Color3.prototype.getPerceivedBrightness = function () {
    return Math.sqrt(0.299 * this.r * this.r + 0.587 * this.g * this.g + 0.114 * this.b * this.b);
}

BABYLON.Color3.prototype.getMaxComponent = function () {
    return Math.max(this.r, this.g, this.b);
}

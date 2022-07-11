THREE.Color.prototype.clamp = function (min, max) {
    if (!min) min = 0;
    if (!max) max = 1;
    this.r = THREE.Math.clamp(this.r, min, max);
    this.g = THREE.Math.clamp(this.g, min, max);
    this.b = THREE.Math.clamp(this.b, min, max);
    return this;
}

THREE.Color.prototype.getPerceivedBrightness = function () {
    return Math.sqrt(0.299 * this.r * this.r + 0.587 * this.g * this.g + 0.114 * this.b * this.b);
}

THREE.Color.prototype.getMaxComponent = function() {
    return Math.max(this.r, this.g, this.b);
}

/**
 * @author Tony Parisi / http://www.tonyparisi.com/
 */

THREE.glTFShaders = ( function () {

	var shaders = [];

	return	{
		add : function(shader) {
			shaders.push(shader);
		},

		remove: function(shader) {

			var i = shaders.indexOf(shader);

			if ( i !== -1 ) {
				shaders.splice( i, 1 );
			}
		},

		bindShaderParameters: function() {
			for (i = 0; i < shaders.length; i++)
			{
				shaders[i].bindParameters();
			}
		},

		update : function() {
			for (i = 0; i < shaders.length; i++)
			{
				shaders[i].update();
			}
		},
	};
})();

// Construction/initialization
THREE.glTFShader = function(material, parameters, object, scene) {
	this.material = material;
	this.parameters = parameters;
	this.object = object;
	this.scene = scene;
	this.semantics = {};
}


// bindUniforms - connect the uniform values to their source parameters
THREE.glTFShader.prototype.bindParameters = function() {

	function findObject(o, param) { 
		if (o.name == param.source) {
			param.sourceObject = o;
		}
	}

	for (var parameter in this.parameters) {

		var param = this.parameters[parameter];
		if (param.semantic) {

			if (param.source) {
				this.scene.traverse(function(o) { findObject(o, param)});
			}
			else {
				param.sourceObject = this.object;
			}			

			console.log("parameter:", param );
		}
	}
}

// Update - update all the uniform values
THREE.glTFShader.prototype.update = function() {

}

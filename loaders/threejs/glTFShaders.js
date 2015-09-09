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
THREE.glTFShader = function(material, parameters, program, object, scene) {
	this.material = material;
	this.parameters = parameters;
	this.program = program;
	this.object = object;
	this.scene = scene;
	this.semantics = {};
}


// bindUniforms - connect the uniform values to their source parameters
THREE.glTFShader.prototype.bindParameters = function() {

	function findObject(o, param) { 
		if (o.glTFID == param.source) {
			param.sourceObject = o;
		}
	}

	for (var uniform in this.program.uniforms) {
		var pname = this.program.uniforms[uniform];
		var param = this.parameters[pname];
		if (param.semantic) {

			if (param.source) {
				this.scene.traverse(function(o) { findObject(o, param)});
			}
			else {
				param.sourceObject = this.object;
			}			

			param.uniform = this.material.uniforms[uniform];
			this.semantics[pname] = param;

			//console.log("parameter:", pname, param );
		}
	}

}

// Update - update all the uniform values
THREE.glTFShader.prototype.update = function() {

	for (var sname in this.semantics) {
		var semantic = this.semantics[sname];
        if (semantic && semantic.sourceObject) {
	        switch (semantic.semantic) {
	            case "MODELVIEW" :
	            	var m4 = semantic.uniform.value;
	            	if (semantic.sourceObject._modelViewMatrix) {
		            	m4.copy(semantic.sourceObject._modelViewMatrix);            		
	            	}
	                break;

	            case "MODELVIEWINVERSETRANSPOSE" :
	            	var m4 = semantic.uniform.value;
	            	if (semantic.sourceObject._normalMatrix) {
		            	m4.copy(semantic.sourceObject._normalMatrix);            		
	            	}
	                break;

	            case "PROJECTION" :
	            	var m4 = semantic.uniform.value;
	            	if (semantic.sourceObject._projectionMatrix) {
		            	m4.copy(semantic.sourceObject._projectionMatrix);            		
	            	}
	                break;

	            default :
	                console.log("Unhandled shader semantic", semantic)
	                break;
	        }
        }
	}
}

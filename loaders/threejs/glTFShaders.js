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

		removeAll: function(shader) {

			// probably want to clean up the shaders, too, but not for now
			shaders = [];
		},

		bindShaderParameters: function(scene) {
			for (i = 0; i < shaders.length; i++)
			{
				shaders[i].bindParameters(scene);
			}
		},

		update : function(scene, camera) {
			for (i = 0; i < shaders.length; i++)
			{
				shaders[i].update(scene, camera);
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
	this.semantics = {};
	this.m4 = new THREE.Matrix4;
}


// bindParameters - connect the uniform values to their source parameters
THREE.glTFShader.prototype.bindParameters = function(scene) {

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
				scene.traverse(function(o) { findObject(o, param)});
			}
			else {
				param.sourceObject = this.object;
			}			

			param.uniform = this.material.uniforms[uniform];
			this.semantics[pname] = param;

			console.log("parameter:", pname, param );
		}
	}

}

// Update - update all the uniform values
THREE.glTFShader.prototype.update = function(scene, camera) {

	for (var sname in this.semantics) {
		var semantic = this.semantics[sname];
        if (semantic && semantic.sourceObject) {
	        switch (semantic.semantic) {
	            case "MODELVIEW" :
	            	var m4 = semantic.uniform.value;
	            	m4.multiplyMatrices( camera.matrixWorldInverse, 
	            		semantic.sourceObject.matrixWorld );
	                break;

	            case "MODELVIEWINVERSETRANSPOSE" :
	            	var m3 = semantic.uniform.value;
	            	this.m4.multiplyMatrices( camera.matrixWorldInverse, 
	            		semantic.sourceObject.matrixWorld );
					m3.getNormalMatrix( this.m4 );
	                break;

	            case "PROJECTION" :
	            	var m4 = semantic.uniform.value;
	            	m4.copy(camera.projectionMatrix);            		
	                break;

	            case "JOINTMATRIX" :
	                console.log("Joint:", semantic)
	                break;

	            default :
	                console.log("Unhandled shader semantic", semantic)
	                break;
	        }
        }
	}
}

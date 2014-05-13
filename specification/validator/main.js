// Some code from https://github.com/garycourt/JSV/blob/master/examples/index.html
// Some code from https://github.com/AnalyticalGraphicsInc/czml-writer/blob/master/Schema/Validator.html
(function() {
    "use strict";
    function validate(schemaUri, jsonUri) {
        try {
            var json;
            var schema;
        
            try {
                json = JSON.parse(document.getElementById('jsonText').value);
            } catch (e) {
                throw new Error('glTF example is not valid JSON');
            }
            
            try {
                schema = JSON.parse(document.getElementById('schemaText').value);
            } catch (e) {
                throw new Error('Schema is not valid JSON');
            }
            
            var environment = JSV.createEnvironment();
            var originalFindSchema = environment.findSchema;
            var loaded = {};

            // Workaround to load schemas from $ref
            environment.findSchema = function (uri) {
                var foundSchema = originalFindSchema.call(this, uri);
                if (!foundSchema) {
                    if (loaded[uri]) {
                        return foundSchema;
                    }
                    loaded[uri] = true;
                    var xhr = new XMLHttpRequest();
                    xhr.open('GET', uri, false);
                    xhr.send();
                    try {
                        environment.createSchema(JSON.parse(xhr.responseText), undefined, uri);
                    } catch (e) {
                        throw new Error("Error loading schema " + uri + ": " + e.message);
                    }
                    foundSchema = originalFindSchema.call(this, uri);
                }
                return foundSchema;
            };

            var report = environment.validate(environment.createInstance(json), environment.createSchema(schema));
            var reportElement = document.getElementById('report');
            var output;
            
            if (report.errors.length) {
                reportElement.className = 'invalid';
                output = '';
                for (var x = 0, xl = report.errors.length; x < xl; ++x) {
                    output += '<div class="error"><span class="error_uri">Problem with <code>' +
                        report.errors[x].uri +
                        '</code> : </span><span class="error_message">' +
                        report.errors[x].message +
                        '</span><br/><span class="error_schemaUri">Reported by <code>' +
                        report.errors[x].schemaUri +
                        '</code></span><br/><span class="error_attribute">Attribute "<code>' +
                        report.errors[x].attribute + 
                        '</code>"</span><span class="error_details"> (<code>' +
                        JSON.stringify(report.errors[x].details) +
                        '</code>)</span></div>';
                }
                reportElement.innerHTML = output;
            } else {
                reportElement.className = 'valid';
                reportElement.innerHTML = 'JSON is valid: ' + jsonUri + '<br />Validated against: ' + schemaUri;
            }
        } catch (e) {
            var reportElement = document.getElementById('report');
            reportElement.className = 'invalid';
            reportElement.textContent = e;
        }
    }

    function clearReport() {
        var reportElement = document.getElementById('report');
        reportElement.innerHTML = '';
        reportElement.className = '';
    }
     
    var schemaUriElement = document.getElementById('schemaUri');
    var schemaTextElement = document.getElementById('schemaText');
    var jsonUriElement = document.getElementById('jsonUri');
    var jsonTextElement = document.getElementById('jsonText');

    var qp = getQueryParameters();

    // Show json to validate if provided as query parameters
    if (typeof qp.json !== 'undefined') {
        jsonTextElement.innerHTML = getText(qp.json);
    }

    // Schema and JSON uri shown in the validation output.
    var schemaUri = document.getElementById('schemaUri').value;
    var jsonUri = defaultValue(qp.json, document.getElementById('jsonUri').value);

    schemaTextElement.innerHTML = getText(schemaUri);

    schemaUriElement.addEventListener('change', function(event) {
        schemaUri = schemaUriElement.value;
        schemaTextElement.value = getText(schemaUriElement.value);
        clearReport();
    });
    schemaUriElement.addEventListener('focus', function(event) {
        // Allow `change` event to fire even when user selects the
        // item that was already selected in order to clear their edits.
        schemaUriElement.selectedIndex = -1;
    });

    jsonUriElement.addEventListener('change', function(event) {
        jsonUri = jsonUriElement.value;

        if (jsonUriElement.value !== '---') {
            jsonTextElement.value = getText(jsonUriElement.value);
            validate(schemaUri, jsonUri);
        } else {
            jsonTextElement.value = '';
            clearReport();
        }
    });
    jsonUriElement.addEventListener('focus', function(event) {
        // Allow `change` event to fire even when user selects the
        // item that was already selected in order to clear their edits.
        jsonUriElement.selectedIndex = -1;
    });

    document.getElementById('jsv').addEventListener('submit', function (event) {
        validate(schemaUri, jsonUri);
        event.preventDefault();
        return false;
    });

    // Validate immediately if query parameter indicates
    if (typeof qp.validate !== 'undefined') {
        validate(schemaUri, jsonUri);
    }
    
}());
The glTF schema is written using [JSON Schema 03](http://tools.ietf.org/html/draft-zyp-json-schema-03).

An asset can be validated against the schema using the glTF Validator (based on [JSV](https://github.com/garycourt/JSV)).  Setup a local webserver to point to this directory, and browse to index.html.  For example:
```
http://localhost/gltf/
```

There are a few query parameters.

Use `schema` to provide the schema to validate against:
```
http://localhost/gltf/?schema=states.schema.json
```

Use `json` to provide the json to validate:
```
http://localhost/gltf/?json=examples/states/translucency.schema.json
```

Use `validate` to immediately validate the json with the schema:
```
http://localhost/gltf/?validate=true
```

The most common use is something like:
```
http://localhost/gltf/?schema=states.schema.json&json=examples/states/translucency.schema.json&validate=true
```

Also, JSON in general can be valdiated with [JSONLint](http://jsonlint.com/), which can be useful for validating the glTF schema itself.
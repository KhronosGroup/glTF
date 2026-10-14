// Copyright 2026 Ben Houston
// SPDX-License-Identifier: Apache-2.0
import assert from 'node:assert/strict';
import { readFileSync, readdirSync, existsSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { resolve } from 'node:path';
import test from 'node:test';
import Ajv from 'ajv/dist/2020.js';
import addFormats from 'ajv-formats';

const extension = fileURLToPath(new URL('..', import.meta.url));
const root = resolve(extension, '../../../..');
const core = resolve(root, 'specification/2.1/schema');
const schemaDir = resolve(extension, 'schema');
const json = path => JSON.parse(readFileSync(path, 'utf8'));
const ajv = new Ajv({ strict: false, allErrors: true, validateFormats: false });
addFormats(ajv);
const uri = path => new URL(`file:///${path.replaceAll('\\', '/')}`).href;
for (const directory of [core, schemaDir]) {
  for (const name of readdirSync(directory).filter(name => name.endsWith('.json'))) {
    const path = resolve(directory, name);
    const schema = json(path);
    schema.$id = uri(path);
    ajv.addSchema(schema);
  }
}
const validator = name => ajv.getSchema(uri(resolve(schemaDir, name)));
const material = validator('material.KHR_lightmap.schema.json');
const node = validator('node.KHR_lightmap.schema.json');

test('all extension schemas have identifiers, resolve their refs and compile', () => {
  for (const name of readdirSync(schemaDir)) {
    assert.equal(json(resolve(schemaDir, name)).$id, name);
    assert.equal(typeof validator(name), 'function');
  }
});

test('bindings reuse core UV defaults and validate transform structure', () => {
  assert(material({ lightmapTexture: { index: 0 } }));
  assert(material({ lightmapTexture: { index: 0, extensions: { KHR_texture_transform: { scale: [0.5, 0.5] } } } }));
  assert(material({ lightmapTexture: { index: 1, texCoord: 2, transform: { offset: [-1, 0], rotation: Math.PI / 2, scale: [-1, 0] } }, intensity: 1000 }));
  assert.equal(json(resolve(core, 'textureInfo.schema.json')).properties.texCoord.default, 0);
  for (const value of [
    {}, { lightmapTexture: {} }, { lightmapTexture: { index: -1 } },
    { lightmapTexture: { index: 0, texCoord: 'UVMap' } },
    { lightmapTexture: { index: 0, texCoord: 0.5 } },
    { lightmapTexture: { index: 0, transform: { offset: [0] } } },
    { lightmapTexture: { index: 0, transform: { scale: [1, 1, 1] } } },
    { lightmapTexture: { index: 0, transform: { rotation: '90deg' } } },
    { lightmapTexture: { index: 0, transform: {}, extensions: { KHR_texture_transform: {} } } },
    { lightmapTexture: { index: 0 }, intensity: -1 }
  ]) assert.equal(material(value), false, JSON.stringify(value));
});

test('node replacements are complete bindings', () => {
  assert(node({ lightmaps: [{ primitive: 0, lightmapTexture: { index: 1 }, intensity: 0 }] }));
  assert(!node({ lightmaps: [] }));
  assert(!node({ lightmaps: [{ primitive: 0 }] }));
  assert(!node({ lightmaps: [{ primitive: -1, lightmapTexture: { index: 0 } }] }));
});

test('README JSON fragments parse and match their extension schemas', () => {
  const text = readFileSync(resolve(extension, 'README.md'), 'utf8');
  const blocks = [...text.matchAll(/```json\n([\s\S]*?)\n```/g)];
  assert.equal(blocks.length, 2);
  const [materialExample, nodeExample] = blocks.map(block => JSON.parse(block[1]));
  assert(material(materialExample.extensions.KHR_lightmap));
  assert(node(nodeExample.extensions.KHR_lightmap));
});

test('local Markdown links resolve', () => {
  const text = readFileSync(resolve(extension, 'README.md'), 'utf8');
  for (const [, target] of text.matchAll(/\]\(([^)]+)\)/g)) {
    if (/^https?:/.test(target)) continue;
    assert(existsSync(resolve(extension, target.split('#')[0])), target);
  }
});

test('complete atlas examples validate core data and extension bindings', () => {
  const validateAsset = ajv.getSchema(uri(resolve(core, 'glTF.schema.json')));
  for (const name of ['node-atlas.gltf', 'gpu-atlas.gltf']) {
    const asset = json(resolve(extension, 'examples', name));
    assert(validateAsset(asset), JSON.stringify(validateAsset.errors));
    for (const binding of asset.materials) assert(material(binding.extensions.KHR_lightmap));
    for (const meshNode of asset.nodes) {
      const replacement = meshNode.extensions?.KHR_lightmap;
      if (replacement) {
        assert(node(replacement));
        const ids = replacement.lightmaps.map(value => value.primitive);
        assert.equal(new Set(ids).size, ids.length);
        for (const id of ids) assert(id < asset.meshes[meshNode.mesh].primitives.length);
      }
      const instances = meshNode.extensions?.EXT_mesh_gpu_instancing;
      if (instances) {
        const attrs = Object.entries(instances.attributes);
        assert.equal(new Set(attrs.map(([, id]) => asset.accessors[id].count)).size, 1);
        for (const [semantic, id] of attrs.filter(([key]) => key.startsWith('_LIGHTMAP_'))) {
          assert.equal(asset.accessors[id].componentType, 5126);
          assert.equal(asset.accessors[id].type, semantic === '_LIGHTMAP_ROTATION' ? 'SCALAR' : 'VEC2');
          assert(!asset.accessors[id].normalized);
        }
      }
    }
    const data = Buffer.from(asset.buffers[0].uri.split(',')[1], 'base64');
    assert.equal(data.length, asset.buffers[0].byteLength);
    for (const view of asset.bufferViews) assert((view.byteOffset ?? 0) + view.byteLength <= data.length);
  }
});

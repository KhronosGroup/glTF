## Texture Metadata Example: Microcosm

This glTF example is a mesh that represents a lot of different types of
terrain in one small tile

This example simulates using per-texel metadata for land cover
classification as well as the Normalized Difference Vegetation Index (NDVI)
values.

The data here is hand-made until we can get more realistic data.

### Script to generate metadata

To generate the metadata buffer, run `node microcosm-metadata.js`. This
will overwrite `microcosm.bin` and print out JSON to add to the bufferViews
of the glTF.

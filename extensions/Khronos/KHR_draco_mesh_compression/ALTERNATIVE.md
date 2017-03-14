## Alternative Approach

We also thought about other designs for adding geometry compression as an extension, but we don't think they are as good as the proposed one. We will describe one here in case someone is interested.


The idea is to add an extension object `decompressedBuffer` to `bufferView` instead of `primitive`. See the following picture for the structure of the alternative approach.
`decompressedBuffer` will work like `bufferView` but pointing to compressed data. The extension will basically declare that the data pointed by the `bufferView` is compressed geometry data. Then for bottom-up loading, the process is to decompress the data when the `bufferView` is loaded, and store the data as a normal buffer of `bufferView`. So after decompression `bufferView` should work the same with or without the extension. For top-down loading, when `accessor` requires the data through `bufferView` the first time, the loader needs to look at the decompressed mesh data of `bufferView` instead of regular `bufferView` --> `buffer` approach.


The advantage of this alternative approch is that it has less impact on the modularity of layers of standard glTF spec. For example, for bottom-up loading, it only requires to load `decompressedBuffer` between `buffer` and `bufferView` so that it could replace the data used by `bufferView`. However, it will change the `bufferView` that its `buffer` property will not be used any more. Another disadvantage is that it is not good for high level understanding and looks hacky. The proposed approach is mush easier to understand and the structure is clear since the compression is applied on the mesh/primitive objects. But we are definitely open to discussions about the choice we made.


**Figure 2**: Structure of extension.
![](figures/decompressed.png)

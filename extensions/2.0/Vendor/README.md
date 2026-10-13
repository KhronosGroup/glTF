<!--
Copyright 2026 The Khronos Group Inc.
SPDX-License-Identifier: CC-BY-4.0
-->

# glTF 2.0 Vendor Extensions

## Vendor extensions have moved to glTF-Community

glTF 2.0 vendor extensions are now hosted in the [glTF-Community](https://github.com/KhronosGroup/glTF-Community/) repository. glTF-Community has license terms suited to vendor-owned extensions and lets each vendor choose the license for its extensions.

- **New vendor extensions and prefix requests** are accepted only in glTF-Community. See the [vendor instructions](https://github.com/KhronosGroup/glTF-Community/blob/main/vendors/README.md). Pull requests that add vendor extensions to this folder will be closed.
- **Existing vendor extensions** in this folder must be moved to glTF-Community **by the organization that owns them**, following the [migration instructions](https://github.com/KhronosGroup/glTF-Community/blob/main/vendors/README.md#migrating-an-existing-extension-from-khronosgroupgltf).
- **Transition period.** The transition period ends 31 December 2026. This date may be extended.
- **After an extension moves**, its folder here is replaced with a short README that links to the new location. These links stay in place for a period still to be determined.
- **Extensions that are never moved** will be archived and removed from this repository some time after the transition period ends.

For the list of vendor extensions still in this folder, and those that have already moved, see the [Extension Registry](../../README.md#vendor-extensions).

## `EXT` extensions in this folder

The `EXT_` extensions stored in this folder are multi-vendor extensions managed by the Khronos 3D Formats Working Group. They are **not** moving to glTF-Community and remain in this repository. See [Multi-Vendor Extensions](../../README.md#multi-vendor-extensions).

## glTF 1.0 vendor extensions

glTF 1.0 vendor extensions (in [extensions/1.0/Vendor](../../1.0/Vendor)) remain in this repository.

## Replacing a moved extension

When an extension has been merged into glTF-Community, its owner (or a maintainer) opens a pull request here that:

1. Deletes every file in `extensions/2.0/Vendor/PREFIX_extension_name/` except `README.md`.
2. Replaces that `README.md` with:

    ```markdown
    # PREFIX_extension_name

    This extension has moved to the
    [glTF-Community repository](https://github.com/KhronosGroup/glTF-Community/blob/main/extensions/PREFIX/PREFIX_extension_name/README.md).

    The copy previously stored here is no longer maintained.
    ```

3. Updates the extension's entry in the [Extension Registry](../../README.md#vendor-extensions) to link to the new location and marks it _(moved)_.

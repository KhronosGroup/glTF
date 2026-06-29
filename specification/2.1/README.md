<!--
Copyright 2014-2021 The Khronos Group Inc.
SPDX-License-Identifier: CC-BY-4.0
-->

<p align="center">
<img src="../figures/glTF_RGB_June16.svg" width="340" height="170" />
</p>

# The glTF 2.0 Specification

The full specification is available in two forms:

- [Specification as HTML](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.html) - for viewing in a browser
- [Specification as PDF](https://www.khronos.org/registry/glTF/specs/2.0/glTF-2.0.pdf) - for viewing as a document

## Extensions

Features not covered by the core glTF specification may be contributed by way of extensions. See the [glTF Extension Registry](https://github.com/KhronosGroup/glTF/tree/main/extensions) for details.

## Contributing

Although the file structure is finalized for glTF 2.0, error corrections and clarifications are most welcome here. Begin by checking the [list of issues](https://github.com/KhronosGroup/glTF/issues) to see if there has already been a discussion of the topic, and if not, file a new issue to get one started.

After discussing the issue, if corrections are needed to the core glTF 2.0 specification, they may be submitted in the form of GitHub Pull Requests to this repository. The choice of file to edit depends on the section that needs editing:

- **Section 5 (Properties Reference)** - This section is auto-generated using the [JSON Schema files](https://github.com/KhronosGroup/glTF/tree/main/specification/2.0/schema). Corrections to the wording must be made directly in those JSON files. Changes to the JSON structure are not being accepted at this time.

- **Appendix A (JSON Schema Reference)** - This section is likewise auto-generated using the [JSON Schema files](https://github.com/KhronosGroup/glTF/tree/main/specification/2.0/schema).

- **All other sections** - The remainder of the specification is contained in [Specification.adoc](https://github.com/KhronosGroup/glTF/blob/main/specification/2.0/Specification.adoc). General edits may be made here.

## Building with GitHub Actions

Opening a PR here will trigger a GitHub Action. Click on the "Actions" tab of the repository containing the edited branch, making careful note that this is likely to be your own fork of the glTF repository, not the Khronos one. The most recent Action should display the name of your commit or PR.

On that Action's Summary Page, if it was successful, there should be an "artifact" at the bottom called `spec-outputs`. This will be a ZIP file containing the newly built HTML and PDF versions, incorporating the proposed changes.

## Building Locally

Building the specification locally can be useful when proposing changes to the actual tooling used during the build.

The most straightforward way is to install [Docker](https://www.docker.com/) such that the specification can be built inside a preconfigured container.  After installing Docker, pull the following image from Docker Hub:

```
docker pull khronosgroup/docker-images:asciidoctor-spec
```

Then, open a terminal inside the new container, and do the following:

1. `git clone ...` this repository, or your fork.
2. `git checkout ...` the branch with your changes.
3. `cd specification/2.0`
4. `make`

This should build `Specification.html` and `Specification.pdf`, containing the HTML and PDF versions of the specification, including any changes from the current git branch.

## Publishing

Once a new version has been built and accepted, the maintainers here will publish it to the [glTF Registry](https://www.khronos.org/registry/glTF) for distribution.

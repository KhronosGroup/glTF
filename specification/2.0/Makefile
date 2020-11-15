# Copyright 2020 The Khronos Group Inc.
# SPDX-License-Identifier: Apache-2.0

# Output specification targets
all: allhtml allpdf

allhtml: README.html SingleSpec.html

allpdf: README.pdf SingleSpec.pdf

# Generate the Properties Reference section of the spec from JSON schema.
# This assumes the 'wetzel' repository is cloned in a directory parallel
# to the glTF repository.
# Note that by default, the schema used, $($SCHEMA), is the version in
# the glTF spec repository rather than local versions.
WETZELROOT = ../../../wetzel
WETZEL = $(WETZELROOT)/bin/wetzel.js
SCHEMA = https://github.com/KhronosGroup/glTF/tree/master/specification/2.0/schema
# Base name of the generated properties reference file
PROPREF = PropertiesReference
$(PROPREF).adoc: $(WETZEL) $(wildcard schema/*.json)
	$(WETZEL) -n -a=cqo -m=a -p "$(SCHEMA)" \
	    -i '["gltfchildofrootproperty.schema.json", "gltfid.schema.json", "gltfproperty.schema.json"]' \
	    schema/glTF.schema.json > $@

# Spec targets for offline generation
# Requires an up-to-date asciidoctor and asciidoctor-pdf be installed
# Recommended to use the khronosgroup/vulkan-docs-base Docker image
ASCIIDOCTOR = asciidoctor
SPECDEPS = README.adoc PropertiesReference.adoc

README.html: $(SPECDEPS)
	$(ASCIIDOCTOR) -b html5 README.adoc -o $@

# :allow-url-read: is necessary for the imbedded render.githubusers.com
# math images to be processed for the PDF target. See
# https://github.com/asciidoctor/asciidoctor-pdf/issues/369
README.pdf: $(SPECDEPS)
	$(ASCIIDOCTOR) -b pdf -a allow-uri-read -r asciidoctor-pdf README.adoc -o $@

# Generate a full single-file spec source by merging
# PropertiesReference.adoc into README.adoc at the point it would
# otherwise be included by asciidoctor. This is a horrid workaround for
# the problem that github's asciidoctor implementation does not support
# the include:: directive.

SINGLESPEC = SingleSpec.adoc

$(SINGLESPEC): $(SPECDEPS)
	sed -e "/include::$(PROPREF).adoc/r$(PROPREF).adoc" \
	    -e "/include::$(PROPREF).adoc/d" < README.adoc > $@

# HTML and PDF targets for the single-file spec source. Should be
# identical to the README outputs - this is intended only for github
# consumption.

SingleSpec.html: SingleSpec.adoc
	$(ASCIIDOCTOR) -b html5 $? -o $@

SingleSpec.pdf: SingleSpec.adoc
	$(ASCIIDOCTOR) -b pdf -a allow-uri-read -r asciidoctor-pdf $? -o $@


GENERATED = $(PROPREF).adoc $(SINGLESPEC)
clean:
	-rm -f $(GENERATED)

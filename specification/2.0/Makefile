# Copyright 2020 The Khronos Group Inc.
# SPDX-License-Identifier: Apache-2.0

# Output specification targets
TARGETS = README.html README.pdf
all: $(TARGETS)

# Generate the Properties Reference section of the spec from JSON schema.
# This uses 'npx' to minimally install the 'wetzel' tool, if needed.
# While the schema used by wetzel is the locally installed version, it
# generates inline links to the schema under $(SCHEMALINK), typically
# the canonical schema in the github default branch.

WETZEL = npx wetzel
SCHEMALINK = https://github.com/KhronosGroup/glTF/tree/master/specification/2.0/schema

# Base name of the generated properties reference file
PROPREF = PropertiesReference
$(PROPREF).adoc: $(wildcard schema/*.json)
	$(WETZEL) -n -a=cqo -m=a -p "$(SCHEMALINK)" \
	    -i '["gltfchildofrootproperty.schema.json", "gltfid.schema.json", "gltfproperty.schema.json"]' \
	    schema/glTF.schema.json > $@

# Spec targets for offline generation
# Requires an up-to-date asciidoctor and asciidoctor-pdf be installed
# We recommend using the khronosgroup/vulkan-docs-base image on dockerhub
ASCIIDOCTOR = asciidoctor
SPECDEPS = README.adoc PropertiesReference.adoc

README.html: $(SPECDEPS)
	$(ASCIIDOCTOR) -b html5 README.adoc -o $@

# :allow-url-read: is necessary for the imbedded render.githubusers.com
# math images to be processed for the PDF target. See
# https://github.com/asciidoctor/asciidoctor-pdf/issues/369
README.pdf: $(SPECDEPS)
	$(ASCIIDOCTOR) -b pdf -a allow-uri-read -r asciidoctor-pdf README.adoc -o $@

GENERATED = $(PROPREF).adoc $(TARGETS)
clean:
	-rm -f $(GENERATED) $(TARGETS)

// Copyright (c) 2012, Motorola Mobility, Inc.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of the Motorola Mobility, Inc. nor the names of its
//    contributors may be used to endorse or promote products derived from this
//    software without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
// DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
// (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
// ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//find . -name '*.dae' -exec dae2json {} \;

#include <getopt.h>

#include <iostream>
#include <sstream>
#include <iomanip>

#include "GitSHA1.h"
#include "GLTF.h"
#include "GLTF-OpenCOLLADA.h"
#include "GLTFAsset.h"

#include "COLLADA2GLTFWriter.h"
#include "JSONObject.h"

#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

#define STDOUT_OUTPUT 0
#if USE_OPEN3DGC
#define OPTIONS_COUNT 13
#else
#define OPTIONS_COUNT 11
#endif


typedef struct {
    const char* name;
    int has_arg;
    const char* help;
} OptionDescriptor;

option* opt_options;
std::string helpMessage = "";

static const OptionDescriptor options[] = {
    { "z",				required_argument,  "-z -> path of configuration file [string]" },
	{ "f",				required_argument,  "-f -> path of input file, argument [string]" },
	{ "o",				required_argument,  "-o -> path of output file argument [string]" },
	{ "b",				required_argument,  "-b -> path of output bundle argument [string]" },
	{ "a",              required_argument,  "-a -> export animations, argument [bool], default:true" },
	{ "i",              no_argument,        "-i -> invert-transparency" },
	{ "d",              no_argument,        "-d -> export pass details to be able to regenerate shaders and states" },
	{ "p",              no_argument,        "-p -> output progress" },
	{ "l",              required_argument,  "-l -> enable default lighting (if no lights in scene) [bool], default:true" },
#if USE_OPEN3DGC
	{ "c",              required_argument,  "-c -> compression type: available: Open3DGC and won [string]" },
	{ "m",              required_argument,  "-m -> compression mode: for Open3DGC can be \"ascii\"(default) or \"binary\" [string]" },
#endif
    { "v",              no_argument,        "-v -> print version" },
    { "s",              no_argument,        "-s -> experimental mode"},
	{ "h",              no_argument,        "-h -> help" }
};

static void buildOptions() {
    helpMessage += "usage: collada2gltlf -f [file] [options]\n";
    helpMessage += "options:\n";
    
    opt_options = (option*)malloc(sizeof(option) * OPTIONS_COUNT);
    
    for (size_t i = 0 ; i < OPTIONS_COUNT ; i++) {
        opt_options[i].flag = 0;
        opt_options[i].val = 'f';
        opt_options[i].name = options[i].name;
        opt_options[i].has_arg = options[i].has_arg;
        
        helpMessage += options[i].help;
        helpMessage += "\n";
    }
}

static void dumpHelpMessage() {
    printf("%s\n", helpMessage.c_str());
}

static std::string replacePathExtensionWith(const std::string& inputFile, const std::string& extension)
{
    COLLADABU::URI inputFileURI(inputFile.c_str());
    
    std::string pathDir = inputFileURI.getPathDir();
    std::string fileBase = inputFileURI.getPathFileBase();
    
    return pathDir + fileBase + "." + extension;
}

bool fileExists(const char * filename) {
    if (FILE * file = fopen(filename, "r")) {
        fclose(file);
        return true;
    }
    return false;
}

static bool processArgs(int argc, char * const * argv, GLTF::GLTFAsset *asset) {
	int ch;
    std::string file;
    std::string output;
    bool hasOutputPath = false;
    bool hasInputPath = false;
    bool shouldShowHelp = false;
    
    //asset->configObject = shared_ptr<GLTF::JSONObject> (new GLTF::JSONObject());
    //asset->configObject
    buildOptions();
    
    if (argc == 1) {
        shouldShowHelp = true;
    }
    
    if (argc == 2) {
        if (fileExists(argv[1])) {
            asset->setInputFilePath(argv[1]);
            asset->setOutputFilePath(replacePathExtensionWith(asset->getInputFilePath(), "json"));
            return true;
        }
    }
    
    shared_ptr<GLTF::GLTFConfig> converterConfig = asset->converterConfig();
    
    while ((ch = getopt_long(argc, argv, "z:f:o:b:a:idpl:c:m:vhs", opt_options, 0)) != -1) {
        switch (ch) {
            case 'z':
                converterConfig->initWithPath(optarg);
                break;
            case 'h':
                dumpHelpMessage();
                return false;
            case 'f':
                asset->setInputFilePath(optarg);
                hasInputPath = true;
				break;
            case 'b':
                asset->setBundleOutputPath(replacePathExtensionWith(optarg, "glTF"));
                hasOutputPath = true;
				break;
            case 'o':
                asset->setOutputFilePath(replacePathExtensionWith(optarg, "json"));
                hasOutputPath = true;
				break;
            case 'i':
                converterConfig->config()->setBool("invertTransparency", true);
                break;
            case 'p':
                converterConfig->config()->setBool("outputProgress", true);
                break;
                
            case 'c':
                //compression type
                converterConfig->config()->setString("compressionType", optarg);
                break;
                
            case 'v':
                printf("collada2gltf@%s\n",g_GIT_SHA1);
                break;
                
            case 'm':
                //compression mode
                converterConfig->config()->setString("compressionMode", optarg);
                break;
                
            case 'l':
                if (optarg != NULL) {
                    bool useDefaultLight;
                    if (strcmp(optarg, "true") == 0) {
                        useDefaultLight = true;
                    }
                    if (strcmp(optarg, "false") == 0) {
                        useDefaultLight = false;
                    } else {
                        useDefaultLight = atoi(optarg) != 0;
                    }
                    converterConfig->config()->setBool("useDefaultLight", useDefaultLight);
                }
                
                break;
            case 'a':
                //asset->exportAnimations = true;
                break;
            case 'd':
                converterConfig->config()->setBool("exportPassDetails", true);
                break;
            case 's':
                //special mode - not exposed - yet.
                converterConfig->config()->setBool("alwaysExportTRS", true);
                converterConfig->config()->setBool("alwaysExportFilterColor", true);
                converterConfig->config()->setBool("alwaysExportTransparency", true);
                converterConfig->config()->setBool("useDefaultLight", false);
                converterConfig->config()->setBool("optimizeParameters", true);
                break;
                
			default:
                shouldShowHelp = true;
				break;
		}
	}
    
    if (shouldShowHelp) {
        dumpHelpMessage();
        return false;
    }
    
    if (!hasOutputPath & hasInputPath) {
        asset->setOutputFilePath(replacePathExtensionWith(asset->getInputFilePath(), "json"));
        
    }
    
    return true;
}

int main (int argc, char * const argv[]) {
    GLTF::GLTFAsset asset;
    
    if (processArgs(argc, argv, &asset)) {
        if (asset.getInputFilePath().length() == 0) {
            return -1;
        }
        const char* inputFilePathCStr = asset.getInputFilePath().c_str();
        
        if (!fileExists(asset.getInputFilePath().c_str())) {
            printf("path:%s does not exists or is not accessible, please check file path and permissions\n",inputFilePathCStr);
            return -1;
        }
        
        clock_t start = clock();
        if (asset.converterConfig()->config()->getBool("outputProgress")) {
            asset.log("convertion 0%%");
            asset.log("\n\033[F\033[J");
        } else {
            asset.log("converting:%s ... as %s \n",asset.getInputFilePath().c_str(), asset.getOutputFilePath().c_str());
        }
        GLTF::COLLADA2GLTFWriter* writer = new GLTF::COLLADA2GLTFWriter(asset);
        writer->write();
        if (asset.converterConfig()->config()->getBool("outputProgress")) {
            asset.log("convertion 100%%");
            asset.log("\n");
        } else {
            asset.log("[completed conversion]\n");
        }
#if WIN32
        double clocks = CLK_TCK;
#else
        double clocks = CLOCKS_PER_SEC;
#endif
        std::stringstream s;
        s << std::setiosflags(std::ios::fixed) << std::setprecision(2) << float(clock() - start) / clocks ;
        asset.log("Runtime: %s seconds\n", s.str().c_str());
    }
    return 0;
}

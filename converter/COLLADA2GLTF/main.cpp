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

#import <getopt.h>

#include <iostream>

#include "GitSHA1.h"
#include "GLTF.h"
#include "GLTF-OpenCOLLADA.h"
#include "GLTFConverterContext.h"

#include "COLLADA2GLTFWriter.h"

#define STDOUT_OUTPUT 0
#if USE_OPEN3DGC
#define OPTIONS_COUNT 11
#else
#define OPTIONS_COUNT 9
#endif


typedef struct {
    const char* name;
    int has_arg;
    const char* help;
} OptionDescriptor;

option* opt_options;
std::string helpMessage = "";

static const OptionDescriptor options[] = {
	{ "f",				required_argument,  "-f -> path of input file, argument [string]" },
	{ "o",				required_argument,  "-o -> path of output file argument [string]" },
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

static std::string replacePathExtensionWithJSON(const std::string& inputFile)
{
    COLLADABU::URI inputFileURI(inputFile.c_str());
    
    std::string pathDir = inputFileURI.getPathDir();
    std::string fileBase = inputFileURI.getPathFileBase();
    
    return pathDir + fileBase + ".json";
}

bool fileExists(const char * filename) {
    if (FILE * file = fopen(filename, "r")) {
        fclose(file);
        return true;
    }
    return false;
}

static bool processArgs(int argc, char * const * argv, GLTF::GLTFConverterContext *converterArgs) {
	int ch;
    std::string file;
    std::string output;
    bool hasOutputPath = false;
    bool hasInputPath = false;
    bool shouldShowHelp = false;
    converterArgs->invertTransparency = false;
    converterArgs->exportAnimations = true;
    converterArgs->exportPassDetails = false;
    converterArgs->outputProgress = false;
    converterArgs->useDefaultLight = true;
    
    converterArgs->compressionType = "none";
    converterArgs->compressionMode = "";
    
    buildOptions();
    
    if (argc == 1) {
        shouldShowHelp = true;
    }
    
    if (argc == 2) {
        if (fileExists(argv[1])) {
            converterArgs->inputFilePath = argv[1];
            converterArgs->outputFilePath = replacePathExtensionWithJSON(converterArgs->inputFilePath);
            return true;
        }
    }
    
    while ((ch = getopt_long(argc, argv, "f:o:a:idpl:c:m:vh", opt_options, 0)) != -1) {
        switch (ch) {
            case 'h':
                dumpHelpMessage();
                return false;
            case 'f':
                converterArgs->inputFilePath = optarg;
                hasInputPath = true;
				break;
            case 'o':
                converterArgs->outputFilePath = replacePathExtensionWithJSON(optarg);
                hasOutputPath = true;
				break;
            case 'i':
                converterArgs->invertTransparency = true;
                break;
            case 'p':
                converterArgs->outputProgress = true;
                break;
                
            case 'c':
                //compression type
                converterArgs->compressionType = optarg;
                printf("[option] compression type:%s\n",optarg);

                break;
                
            case 'v':
                printf("collada2gltf@%s\n",g_GIT_SHA1);
                break;
                
            case 'm':
                //compression mode
                converterArgs->compressionMode = optarg;
                printf("[option] compression mode:%s\n",optarg);

                break;
                
            case 'l':
                if (optarg != NULL) {
                    if (strcmp(optarg, "true") == 0) {
                        converterArgs->useDefaultLight = true;
                    }
                    if (strcmp(optarg, "false") == 0) {
                        converterArgs->useDefaultLight = false;
                    } else {
                        converterArgs->useDefaultLight = atoi(optarg) != 0;
                    }
                }
                
                break;
            case 'a':
                //converterArgs->exportAnimations = true;
                break;
            case 'd':
                converterArgs->exportPassDetails = true;
                printf("[option] export pass details\n");
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
        converterArgs->outputFilePath = replacePathExtensionWithJSON(converterArgs->inputFilePath);
    }
        
    return true;
}

int main (int argc, char * const argv[]) {
    GLTF::GLTFConverterContext converterContext;
    
    if (processArgs(argc, argv, &converterContext)) {
        if (converterContext.inputFilePath.length() == 0) {
            return -1;
        }
        const char* inputFilePathCStr = converterContext.inputFilePath.c_str();
        
        if (!fileExists(converterContext.inputFilePath.c_str())) {
            printf("path:%s does not exists or is not accessible, please check file path and permissions\n",inputFilePathCStr);
            return -1;
        }
        
#if !STDOUT_OUTPUT
        FILE* fd = fopen(converterContext.outputFilePath.c_str(), "w");
        if (fd) {
            rapidjson::FileStream s(fd);
#else
            rapidjson::FileStream s(stdout);
#endif
            rapidjson::PrettyWriter <rapidjson::FileStream> jsonWriter(s);
            if (converterContext.outputProgress) {
                printf("convertion 0%%");
                printf("\n\033[F\033[J");
            } else {
                printf("converting:%s ... as %s \n",converterContext.inputFilePath.c_str(), converterContext.outputFilePath.c_str());
            }
            GLTF::COLLADA2GLTFWriter* writer = new GLTF::COLLADA2GLTFWriter(converterContext, &jsonWriter);
            writer->write();
            if (converterContext.outputProgress) {
                printf("convertion 100%%");
                printf("\n");
            } else {
                printf("[completed conversion]\n");
            }
#if !STDOUT_OUTPUT
            fclose(fd);
            delete writer;
        }
#endif
    }
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>

#define LANGS_SUPPORTED 1
#define OS_SUPPORTED    2 

// Task 1: Save the inputs to generate build files
// 0        1            2          3        4
// zen <directory> <programming language> <os type>
// example: zen . c win
//
//
// what if the no. of arguments is 0,1,2,3?
// it must have 4 arguments
// respond with the help message

typedef struct {
    char *dir; // directory
    char *lang; // programming language
    char *os; // os type
    char *version; // zen current version
} Zen;

void zen_help() {
    printf("usage: zen [-v | --version] [-h | --help] <dir> <lang> <os>\n");
    printf("example: zen . c win\n");
    printf("arugments:\n"
                      "dir  -> directory where all build files will be generated\n"
                      "lang -> programming language that will be used to generate a basic main file\n"
                      "os   -> generate a build file specifically for your os\n");
}

void zen_version(Zen *zen) {
   printf("Version: [%s]\n", zen->version); 
}

bool zen_genLangFile(Zen *zen) {
    // grab the lang file and copy it over to the directory
    FILE *fp;
    char ch;
    const char *code = "#include <stdio.h>\n" 
                       "#include <string.h>\n" 
                       "#include <stdlib.h>\n\n" 
                       "int main(void) {\n" 
                       " puts(\"Zen.\\n\");\n" 
                       " return 0;\n" "}\n";
    
    // create full path where the file will be stored
    char lang_path[128];
    snprintf(lang_path, sizeof(lang_path), "%s/main.%s", zen->dir, zen->lang);
    printf("Lang path: %s\n", lang_path);

    fp = fopen(lang_path, "w");
    if(!fp) {
        perror("fopen");
        return false;
    }
    
    fprintf(fp, "%s", code);
    return true;
}

bool zen_genBuildScript(Zen *zen) {
    // grab the script file and copy it over to the directory
    const char *code = "#include <stdio.h>\n" 
                       "#include <string.h>\n" 
                       "#include <stdlib.h>\n\n" 
                       "int main(void) {\n" 
                       " puts(\"Zen.\\n\");\n" 
                       " return 0;\n" "}\n";


    printf("Under construction\n");
    return true;
}

bool zen_generateBuild(Zen *zen) {
    // zen . c win 
    printf("Generating build files for:\n"
    "Build Directory: %s\n"
    "Programming Language: %s\n"
    "OS Type: %s\n", zen->dir, zen->lang, zen->os);

    printf("Generating %s file.\n", zen->lang);
    if(!zen_genLangFile(zen)) {
        printf("Failed to generate %s file.\n", zen->lang);
        return false;
    }
   
    printf("Generated main.%s file under %s successful\n", zen->lang, zen->dir);
    
    printf("Generating build script.\n");
    if(!zen_genBuildScript(zen)) {
        printf("Failed to generate build script file.\n");
        return false;
    }
    

    printf("Build successfull.\n");
    return true;
}


// @refer to me: -> zai
int main(int argc, char *argv[]) {
    Zen zen;
    zen.version = "v.0.0.0";

    char *langs_supported[LANGS_SUPPORTED] = {
        "c"
    };

    char *os_supported[OS_SUPPORTED] = {
        "win", 
        "linux"
    };
    
    for(int i = 1; i < argc; ++i) {
        // dir
        if(i == 1) {
            if(strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
                zen_version(&zen); 
                break;
            }

            if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
                zen_help(); 
                break;
            }

            // save directory
            zen.dir = argv[1];
            
            // check if directory exist
            DIR *d;
            bool dir_exists = true;
            d = opendir(zen.dir);
            if(!d) {
                perror("opendir");
                printf("Creating directory -> [%s]\n", zen.dir);
                dir_exists = false;
            }
       
            // create directory            
            if(!dir_exists) {
                int status;
                status = mkdir(zen.dir);
                if(status == 0) {
                    printf("Directory created successfully.\n");
                } else {
                    perror("mkdir"); 
                    closedir(d);
                    return 1;
                }
            }
            
            closedir(d);
        }
        
        // lang
        if(i == 2) {
            // check if language is supported (for now only c)
            zen.lang = argv[2];
            for(int i = 0; i < LANGS_SUPPORTED; ++i) {
                if(strcmp(zen.lang, langs_supported[i]) != 0) {
                    printf("Programming Language: [%s] is not supported.\n", zen.lang);
                    return 1;
                }
            }
        }
        
        // os
        if(i == 3) {
            // check if os is supported (should only support WIN and LINUX)
            zen.os = argv[3];
            int res;
            bool found_os = false;
            for(int i = 0; i < OS_SUPPORTED; ++i) {
                res = strcmp(zen.os, os_supported[i]);
                if(res == 0) found_os = true;
            }

            if(!found_os) {
                printf("OS: [%s] is not supported\n", zen.os);
                return 1;
            }
        }
    }
    
    printf("Zen Arugments:\n");
    printf("dir: [%s]\n", zen.dir);
    printf("lang: [%s]\n", zen.lang);
    printf("os: [%s]\n", zen.os);
    zen_generateBuild(&zen);
    
    return 0;
}

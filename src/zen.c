#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <sys/stat.h>

#define LANGS_SUPPORTED 5
#define OS_SUPPORTED    3

#define BUFF_SIZE 512

typedef struct {
    char *dir; 
    char *lang;    
    char *os; 
    char *day;
    char *version; 
} Zen;

typedef struct {
    char *data;
    char *content;
} ZenData;

ZenData lang[LANGS_SUPPORTED] = {
    {"c",   "#include <stdio.h>\n" 
            "#include <string.h>\n" 
            "#include <stdlib.h>\n\n" 
            "int main(void) {\n" 
            " puts(\"Zen.\\n\");\n" 
            " return 0;\n" "}\n"},
    {"java", ""},
    {"python", ""},
    {"cpp", ""},
    {"rust", ""},
};

// TODO: Support each language for the content side. I could set up the data after the arugments have been processed
// BUG: I made it static by hardcoding for c. oops
// TODO: Needs hella work
ZenData os[OS_SUPPORTED] = {
    {"win",     "@echo off\n"
                "gcc src/main.c -o main.exe\n"
                "REM Perfect Example\n"
                "main.exe . c win\n"},

    {"linux", "#!/bin/bash\n"
              "gcc src/main.c -o main\n"
              "chmod +x main\n"
              "./main\n"},

    {"macos", ""},
};

void zen_help() {
    printf("usage: zen [-v | --version] [-h | --help] <dir> <lang> <os> <aoc_day>\n");
    printf("example: zen . c win 1\n\n");
    printf("arugments:\n"
                      "dir  -> directory where all build files will be generated\n"
                      "lang -> programming language that will be used to generate a basic main file\n"
                      "os   -> generate a build file specifically for your os\n"
                      "aoc_day -> pick a day you want to solve");
}

void zen_version(Zen *zen) {
   printf("Version: [%s]\n", zen->version); 
}

bool zen_genLangFile(Zen *zen) {
    char lang_path[128];
    snprintf(lang_path, sizeof(lang_path), "%s/main.%s", zen->dir, zen->lang);
    printf("Lang path: %s\n\n", lang_path);

    FILE *fp = fopen(lang_path, "w");
    if(!fp) {
        perror("fopen");
        return false;
    }
    
    for(int i = 0; i < LANGS_SUPPORTED; ++i) {
        if(strcmp(zen->lang, lang[i].data) == 0) {
            fprintf(fp, "%s", lang[i].content);
            break;
        }
    }
    
    return true;
}

bool zen_genBuildScript(Zen *zen) {
    char script[BUFF_SIZE];
    char *extension;
    
    if(strcmp(zen->os, "win") == 0) {
        extension = ".bat";
    } else if(strcmp(zen->os, "linux") == 0) {
        extension = ".sh";
    }
    
    snprintf(script, sizeof(script), "%s/%s-build%s", zen->dir, zen->lang, extension);
    printf("Build script directory: %s\n", script);
    
    FILE *fp = fopen(script, "w");
    if(!fp) {
        perror("fopen");
        return false;
    }

    for(int i = 0; i < OS_SUPPORTED; ++i) {
        if(strcmp(zen->os, os[i].data) == 0) {
            fprintf(fp, "%s", os[i].content);
        }
    }

    return true;
}

// @Example: zen . c win 
bool zen_generateBuild(Zen *zen) {
    printf("Generating build files:\n"
    "Build Directory: %s\n"
    "Programming Language: %s\n"
    "OS Type: %s\n"
    "AoC Day: %s\n\n", zen->dir, zen->lang, zen->os, zen->day);

    printf("Generating %s template.\n", zen->lang);
    if(!zen_genLangFile(zen)) {
        printf("Failed to generate %s.\n", zen->lang);
        return false;
    }
   
    printf("Generating build script.\n");
    if(!zen_genBuildScript(zen)) {
        printf("Failed to generate build script.\n");
        return false;
    }

    return true;
}


// @refer to me: -> zai
int main(int argc, char *argv[]) {
    if(argc != 2 && argc != 5) {
        printf("Check your build script... missing arguments.\n");
        return 1;
    }
    
    Zen zen;
    zen.version = "v.0.0.2";
    
    for(int i = 1; i < argc; ++i) {
        // dir
        if(i == 1) {
            if(strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
                zen_version(&zen); 
                return 0;
            } else if(strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
                zen_help(); 
                return 0;
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
                status = mkdir(zen.dir, 0777); // @PossibleIssue #001 (mkdir compatability issues? for win and linux code?)
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
            bool found_lang = false;
            int res;
            for(int i = 0; i < LANGS_SUPPORTED; ++i) {
                res = strcmp(zen.lang, lang[i].data);
                if(res == 0) found_lang = true;
            }

            if(!found_lang) {
                printf("Language: [%s] is not supported\n", zen.os);
                return 1;
            }
        }

        
        // os
        if(i == 3) {
            // check if os is supported (should only support WIN and LINUX)
            zen.os = argv[3];
            int res;
            bool found_os = false;
            for(int i = 0; i < OS_SUPPORTED; ++i) {
                res = strcmp(zen.os, os[i].data);
                if(res == 0) found_os = true;
            }

            if(!found_os) {
                printf("OS: [%s] is not supported\n", zen.os);
                return 1;
            }
        }

        // aoc day
        if(i == 4) {
            zen.day = argv[4];
            char *cookie_sesh = getenv("AOC_COOKIE_SESSION");

            if(!cookie_sesh) {
                puts("AOC_COOKIE_SESSION environment variable not found");
                return 1;
            } else {
                char wget[BUFF_SIZE];
                snprintf(wget, sizeof(wget), "wget --header 'Cookie: session=%s' https://adventofcode.com/2024/day/%s/input -O %s/input.txt", cookie_sesh, zen.day, zen.dir);
                system(wget); // WARNING: unsafe function
            }

        }
    }
    
    printf("Zen Arugments:\n");
    printf("dir: [%s]\n", zen.dir);
    printf("lang: [%s]\n", zen.lang);
    printf("os: [%s]\n\n", zen.os);
    if(zen_generateBuild(&zen)) {
        printf("[\033[32mOK\033[0m] Build successful.\n");
    } else {
        printf("[\033[31mFAILED\033[0m] Build failed.\n");
    }
    
    return 0;
}

// WARNING: Duplicate code when error checking for the arguments: <lang> and <os>
// WARNING: Duplicate code when generating scripts and files

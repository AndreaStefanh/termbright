#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

#include "macros.h"


typedef struct Args {
    bool printHelp;
    bool printVersion;
    bool printMaxBright;
    bool printCurrentBright;

    bool printTheCurrentBrightness;

    bool directBrightness;
    bool percenteBrightness;

    bool directBrightnessWithoutSign;
    bool percenteBrightnessWithoutSign;

    char *intensity;
} Args;


void isNull(void *check, char *error)
{
    if (check == NULL)
    {
        fprintf(stderr, "Error: %s\n", error);
        exit(EXIT_FAILURE);
    }

    return;
}

#if defined __GNUC__ && !defined __clang__
#  pragma GCC diagnostic push
#  pragma GCC diagnostic ignored "-Wreturn-local-addr"
#elif __clang__
#  pragma clang diagnostic push
#  pragma clang diagnostic ignored "-Wreturn-stack-address"
#endif

char *errorMessageC(char *errorMessage, int errorCode)
{
    char *errnoMessage = strerror(errorCode);

    size_t len = strlen(errorMessage);
    char result[len + 1];
    memset(result, 0, len + 1);

    for (size_t i = 0; i <= len; i++) result[i] = errorMessage[i];
    strcat(result, errnoMessage);

    return result;
}

#if defined __GNUC__ && !defined __clang__
#  pragma GCC diagnostic pop
#elif __clang__
#  pragma clang diagnostic pop
#endif

void *xmalloc(size_t len)
{
    void *ReturnValue = malloc(len);
    isNull(ReturnValue, errorMessageC("unable to allocate to memory: ", errno) );

    return ReturnValue;
}

char *strConcat(char *src1, char *src2)
{
    size_t len1 = strlen(src1);
    size_t len2 = strlen(src2);
    size_t i = 0;

    char *str = xmalloc(sizeof(char) * ((len1 + len2) + 1));


    for (size_t j = 0; j <= len1; ++j, ++i) str[i] = (char) src1[j];
    --i;
    for (size_t j = 0; j <= len2; ++j, ++i) str[i] = (char) src2[j];

    return str;
}

void removeNewLine(char *str)
{
    size_t line = strlen(str);
    str[line - 1] = '\0';
    
    return;
}

void checkRoot(void)
{
    if (getuid()) 
    {
        fprintf(stderr, "ERROR: this application needs root permissions\n");
        exit(EXIT_FAILURE);
    }

    return;
}


char *grubBrightness(char *fileName)
{
    DIR *dir = opendir(PATH_BRIGHTNESS);
    isNull(dir, errorMessageC("unable to open '"PATH_BRIGHTNESS"': ", errno));

    char *ReturnMaxBrightness = xmalloc(sizeof(char) * 11);
    memset(ReturnMaxBrightness, 0, 11);


    struct dirent *folders = readdir(dir);
    isNull(folders, errorMessageC("unable to read in '"PATH_BRIGHTNESS"': ", errno));

    while (folders != NULL)
    {
        if (folders->d_type == DT_LNK)
        {
            char *tmpPath = strConcat(PATH_BRIGHTNESS, folders->d_name);
            char *tmpError = strConcat("unable to open '", strConcat( strConcat(PATH_BRIGHTNESS, folders->d_name), "': "));
            
            DIR *tmpDir = opendir(tmpPath);
            isNull(dir, errorMessageC(tmpError, errno));
            

            struct dirent *tmp = readdir(tmpDir);
            isNull(folders, errorMessageC(tmpError, errno));

            while (tmp != NULL)
            {
                if (tmp->d_type == DT_REG && strcmp(tmp->d_name, fileName) == 0)
                {
                    char *pathBright = strConcat(PATH_BRIGHTNESS, strConcat(folders->d_name, strConcat("/", fileName)));
                    int fd = open(pathBright, O_RDONLY);

                    if (fd == -1)
                    {
                        fprintf(stderr, "ERROR: unable to read file %s: %s", pathBright, strerror(errno));

                        free(pathBright);
                        free(tmpPath);
                        free(tmpError);
                        closedir(tmpDir);
                        exit(EXIT_FAILURE);
                    }

                    free(pathBright);
                    
                    ssize_t errorRead = read(fd, ReturnMaxBrightness, 10);

                    if (errorRead == -1)
                    {
                        fprintf(stderr, "ERROR: unable to read file %s: %s", pathBright, strerror(errno));

                        free(tmpPath);
                        free(tmpError);
                        closedir(tmpDir);
                        closedir(dir);
                        exit(EXIT_FAILURE);
                    }

                    close(fd);
                }

                tmp = readdir(tmpDir);
            }

            free(tmpPath);
            free(tmpError);
            closedir(tmpDir);
        }

        folders = readdir(dir);
    }

    closedir(dir);
    return ReturnMaxBrightness;
}

void writeBright(char *intensity)
{
    DIR *dir = opendir(PATH_BRIGHTNESS);
    isNull(dir, errorMessageC("unable to open '"PATH_BRIGHTNESS"': ", errno));

    struct dirent *folders = readdir(dir);
    isNull(folders, errorMessageC("unable to read in '"PATH_BRIGHTNESS"': ", errno));

    while (folders != NULL)
    {
        if (folders->d_type == DT_LNK)
        {
            char *tmpPath = strConcat(PATH_BRIGHTNESS, folders->d_name);
            char *tmpError = strConcat("unable to open '", strConcat( strConcat(PATH_BRIGHTNESS, folders->d_name), "': "));
            
            DIR *tmpDir = opendir(tmpPath);
            isNull(dir, errorMessageC(tmpError, errno));
            

            struct dirent *tmp = readdir(tmpDir);
            isNull(dir, errorMessageC(tmpError, errno));

            while (tmp != NULL)
            {
                if (tmp->d_type == DT_REG && strcmp(tmp->d_name, "brightness") == 0)
                {
                    char *pathBright = strConcat(PATH_BRIGHTNESS, strConcat(folders->d_name, strConcat("/", "brightness")));
                    int fd = open(pathBright, O_WRONLY);

                    if (fd == -1)
                    {
                        fprintf(stderr, "ERROR: unable to read file %s: %s", pathBright, strerror(errno));

                        free(pathBright);
                        free(tmpPath);
                        free(tmpError);
                        closedir(tmpDir);
                        exit(EXIT_FAILURE);
                    }

                    free(pathBright);
                    
                    ssize_t errorWrite = write(fd, intensity, 10);

                    if (errorWrite == -1)
                    {
                        fprintf(stderr, "ERROR: unable to write the file %s: %s", pathBright, strerror(errno));

                        free(tmpPath);
                        free(tmpError);
                        closedir(tmpDir);
                        closedir(dir);
                        exit(EXIT_FAILURE);
                    }

                    close(fd);
                }

                tmp = readdir(tmpDir);
            }

            free(tmpPath);
            free(tmpError);
            closedir(tmpDir);
        }

        folders = readdir(dir);
    }

    closedir(dir);
    return;
}

char *percentage(char *PercentageOfTheUser, char *maxBrightness, char *actualBrightness, bool withoutSign, bool add, bool sum)
{
    size_t PercentageOfTheUserD = atol(PercentageOfTheUser);
    size_t maxBrightnessD = atol(maxBrightness);
    size_t actualBrightnessD = atol(actualBrightness);

    size_t ReturnPer = ((PercentageOfTheUserD * maxBrightnessD) / 100);

    if (withoutSign == true && (add == false && sum == false)) {
        ReturnPer = ReturnPer;
    } else if (add == true && (withoutSign == false && sum == false)) {
        ReturnPer = actualBrightnessD + ReturnPer;
    } else if (sum == true && (withoutSign == false && add == false)) {
        ReturnPer = actualBrightnessD - ReturnPer;
    } else {
        assert(0 && "unreachable");
    }
    
    
    char *ReturnPerC = xmalloc(sizeof(char) * 11);
    memset(ReturnPerC, 0, 11);
    
    sprintf(ReturnPerC, "%zu", ReturnPer);

    return ReturnPerC;
}

char *percentage2(char *PercentageOfTheUser, char *maxBrightness)
{
    size_t PercentageOfTheUserD = (size_t) atol(PercentageOfTheUser);
    size_t maxBrightnessD = (size_t) atol(maxBrightness);

    size_t ReturnPer = ((100 * PercentageOfTheUserD) / maxBrightnessD);

    char *ReturnPerC = xmalloc(sizeof(char) * 11);
    memset(ReturnPerC, 0, 11);
    
    sprintf(ReturnPerC, "%zu", ReturnPer);

    return ReturnPerC;
}

bool control(char *intensity, char *argsIntensity)
{
    size_t i = 0;
    size_t len = strlen(intensity);

    (void) argsIntensity;

    #define NUMBERS         \
    intensity[i] == '1' ||  \
    intensity[i] == '2' ||  \
    intensity[i] == '3' ||  \
    intensity[i] == '4' ||  \
    intensity[i] == '5' ||  \
    intensity[i] == '6' ||  \
    intensity[i] == '7' ||  \
    intensity[i] == '8' ||  \
    intensity[i] == '9' ||  \
    intensity[i] == '0' ||  \
    intensity[i] == '%'


    if (len == 0)
    {
        fprintf(stderr, "ERROR: the length of the intensity is equal to 0\n");
        argsIntensity = xmalloc(0);
        return false;
    }
    

    if (intensity[0] == '+' || intensity[0] == '-') {
        
        for (i = 1; i < len; i++)
        {
            if (NUMBERS) {

                continue;

            } else {
                
                fprintf(stderr, "ERROR: character '%c' not recognized at the position: %zu\n", intensity[i], i);
                argsIntensity = xmalloc(0);
                return false;
            }
        }

        return true;

    } else if (NUMBERS) {

        for (i = 0; i < len; i++)
        {
            if (NUMBERS) {

                continue;

            } else {
                
                fprintf(stderr, "ERROR: character '%c' not recognized at the position: %zu\n", intensity[i], i);
                argsIntensity = xmalloc(0);
                return false;
            }
        }

        return true;

    } else {
        fprintf(stderr, "ERROR: character '%c' not recognized at the position: %zu", intensity[0], i);
        argsIntensity = xmalloc(0);
        return false;
    }
    
    argsIntensity = xmalloc(0);
    return false;
}

bool checkIntensity(char *intensity, char *maxBrightness, char *actualBrightness)
{
    size_t intensityD = (size_t) atol(intensity);
    size_t maxBrightnessD = (size_t) atol(maxBrightness);
    size_t actualBrightnessD = (size_t) atol(actualBrightness);
    
    if (actualBrightnessD == intensityD)
    {
        fprintf(stderr, "INFO: you set the brightness as the current brightness\n");
        return false;
    }

    if (intensityD == 0 || (actualBrightnessD - intensityD) <= 0)
    {
        fprintf(stderr, "ERROR: you can't set brightness under or 0%%\n");
        return false;
    }

    if (intensityD > maxBrightnessD)
    {
        fprintf(stderr, "ERROR: you can't set brightness upper or 100%%\n");
        return false;
    }
    
    return true;
}

Args parseArgs(int argc, char **argv, char *maxBrightness, char *actualBrightness)
{
    Args ReturnArgs;
    bool labelGotoCheck = false;

resetLabel:
    ReturnArgs.printHelp = false;
    ReturnArgs.printVersion = false;
    ReturnArgs.printMaxBright = false;
    ReturnArgs.printCurrentBright = false;
    ReturnArgs.printTheCurrentBrightness = false;
    ReturnArgs.directBrightnessWithoutSign = false;
    ReturnArgs.percenteBrightnessWithoutSign = false;
    ReturnArgs.directBrightness = false;
    ReturnArgs.percenteBrightness = false;

    if (labelGotoCheck == true) return ReturnArgs;


    if (argc == 1) 
    {
        ReturnArgs.printTheCurrentBrightness = true;
        ReturnArgs.intensity = xmalloc(0);
        return ReturnArgs;
    }

    if (argc >= (2 + MAX_ARGS))
    {
        ReturnArgs.intensity = xmalloc(0);
        fprintf(stderr, "ERROR: you pass %d arguments\n", argc);
        fprintf(stderr, "Max arguments %d\n", MAX_ARGS);
        return ReturnArgs;
    }
    

    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-?") == 0) {
            ReturnArgs.printHelp = true;
            ReturnArgs.intensity = xmalloc(0);
            return ReturnArgs;

        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            ReturnArgs.printVersion = true;
            ReturnArgs.intensity = xmalloc(0);
            return ReturnArgs;

        } else if (strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--max") == 0) {
            ReturnArgs.printMaxBright = true;
            ReturnArgs.intensity = xmalloc(0);
            return ReturnArgs;
        
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--current") == 0) {
            ReturnArgs.printCurrentBright = true;
            ReturnArgs.intensity = xmalloc(0);
            return ReturnArgs;

        } else if (argv[i][0] == '+') {

            size_t len = strlen(argv[i]);

            if (argv[i][len - 1] == '%')
            {
                ReturnArgs.percenteBrightness = true;

                if (control(argv[i], ReturnArgs.intensity) == false) { labelGotoCheck = true; goto resetLabel; }

                char percen[len];
                memset(percen, 0, len);
                size_t z = 0;
                for (size_t j = 1; j <= len - 2; ++j, ++z) percen[z] = argv[i][j];

                ReturnArgs.intensity = percentage(percen, maxBrightness, actualBrightness, false, true, false);
                if (checkIntensity(ReturnArgs.intensity, maxBrightness, actualBrightness) == false) { labelGotoCheck = true; goto resetLabel; }
            } else {
                ReturnArgs.directBrightness = true;
                
                if (control(argv[i], ReturnArgs.intensity) == false) { labelGotoCheck = true; goto resetLabel; }

                char percen[len];
                memset(percen, 0, len);
                size_t z = 0;
                for (size_t j = 1; j <= len; ++j, ++z) percen[z] = argv[i][j];

                ReturnArgs.intensity = xmalloc(sizeof(char) * 11);
                memset(ReturnArgs.intensity, 0, 11);

                sprintf(ReturnArgs.intensity, "%ld", (atol(actualBrightness) + atol(percen)));
                if (checkIntensity(ReturnArgs.intensity, maxBrightness, actualBrightness) == false) { labelGotoCheck = true; goto resetLabel; }
            }
            

        } else if (argv[i][0] == '-') {
            size_t len = strlen(argv[i]);

            if (argv[i][len - 1] == '%')
            {                
                ReturnArgs.percenteBrightness = true;

                if (control(argv[i], ReturnArgs.intensity) == false) { labelGotoCheck = true; goto resetLabel; }

                char percen[len];
                memset(percen, 0, len);
                size_t z = 0;
                for (size_t j = 1; j < len - 1; ++j, ++z) percen[z] = argv[i][j];

                ReturnArgs.intensity = percentage(percen, maxBrightness, actualBrightness, false, false, true);
                if (checkIntensity(ReturnArgs.intensity, maxBrightness, actualBrightness) == false) { labelGotoCheck = true; goto resetLabel; }
            } else {                
                ReturnArgs.directBrightness = true;
                
                if (control(argv[i], ReturnArgs.intensity) == false) { labelGotoCheck = true; goto resetLabel; }

                char percen[len];
                memset(percen, 0, len);
                size_t z = 0;
                for (size_t j = 1; j <= len; ++j, ++z) percen[z] = argv[i][j];

                ReturnArgs.intensity = xmalloc(sizeof(char) * 11);
                memset(ReturnArgs.intensity, 0, 11);
                sprintf(ReturnArgs.intensity, "%ld", (atol(actualBrightness) - atol(percen)));
                if (checkIntensity(ReturnArgs.intensity, maxBrightness, actualBrightness) == false) { labelGotoCheck = true; goto resetLabel; }
            }

        } else if (argv[i][0] == '0' || argv[i][0] == '1' || argv[i][0] == '2' || argv[i][0] == '3' || argv[i][0] == '4' || argv[i][0] == '5' || argv[i][0] == '6' || argv[i][0] == '7' || argv[i][0] == '8' || argv[i][0] == '9') {

            size_t len = strlen(argv[i]);

            if (argv[i][len - 1] == '%')
            {                
                ReturnArgs.percenteBrightnessWithoutSign = true;
                
                if (control(argv[i], ReturnArgs.intensity) == false) { labelGotoCheck = true; goto resetLabel; }

                char percen[len];
                memset(percen, 0, len);
                for (size_t j = 0; j <= len - 2; ++j) percen[j] = argv[i][j];
                

                ReturnArgs.intensity = percentage(percen, maxBrightness, actualBrightness, true, false, false);
                if (checkIntensity(ReturnArgs.intensity, maxBrightness, actualBrightness) == false) { labelGotoCheck = true; goto resetLabel; }

            } else {
                ReturnArgs.directBrightnessWithoutSign = true;
                
                if (control(argv[i], ReturnArgs.intensity) == false) { labelGotoCheck = true; goto resetLabel; }

                ReturnArgs.intensity = xmalloc(sizeof(char) * len);
                memset(ReturnArgs.intensity, 0, len);

                for (size_t j = 0; j <= len; j++) ReturnArgs.intensity[j] = argv[i][j];
                if (checkIntensity(ReturnArgs.intensity, maxBrightness, actualBrightness) == false) { labelGotoCheck = true; goto resetLabel; }
            }
        } else {
            fprintf(stderr, "ERROR: no such option: '%s'\n", argv[i]);
            labelGotoCheck = true;
            goto resetLabel;
        }

    }
    
    return ReturnArgs;
}

void printTheCurrentBrightness(char *actualBrightness, char *maxBrightness)
{
    char *perc = percentage2(actualBrightness, maxBrightness);
    fprintf(stdout, "Current brightness: %s%%\n", perc);
    free(perc);
    return;
}

void printVersion(void)
{
    fprintf(stdout, "%s\n", VERSION);
    return;
}

void printMaxBrightness(char *maxBrightness)
{
    fprintf(stdout, "%s\n", maxBrightness);
    return;
}

void printCurrentBrightness(char *currentBrightness)
{
    fprintf(stdout, "%s\n", currentBrightness);
    return;
}

void printHelp(void)
{
    fprintf(stdout, "TermBright v%s - Copyright © 2022 Andrea Stefan\n", VERSION);
    fprintf(stdout, "License GPLv3: GNU GPL version 3 <https://gnu.org/licenses/gpl.html>\n");
    fprintf(stdout, "The source is hosted in github <https://www.github.com/AndreaStefanh/termbright>\n");
    fprintf(stdout, "usage: termbright [options]\n\n");
    fprintf(stdout, "options:\n");
    fprintf(stdout, "  -h, --help, -? |   Print this help screen in stdout\n");
    fprintf(stdout, "  -v, --version  |   Print the version of this program in stdout\n");
    fprintf(stdout, "  -m, --max      |   Print the max brightness (in a file format) in stdout\n");
    fprintf(stdout, "  -c, --current  |   Print the current brightness (in a file format) in stdout\n");
    fprintf(stdout, "       +50%%      |   Add 50%% to the current brightness\n");
    fprintf(stdout, "       -50%%      |   Substract 50%% to the current brightness\n");
    fprintf(stdout, "        50%%      |   Set the brightness at 50%%\n");
    fprintf(stdout, "       +100      |   Add 100 to the current brightness (in a file format)\n");
    fprintf(stdout, "       -100      |   Substract 100 to the current brightness (in a file format)\n");
    fprintf(stdout, "        100      |   Set the brightness at 100 (in a file format)\n");
    fprintf(stdout, "\n");
    return;
}


int main(int argc, char *argv[])
{
    checkRoot();

    char *maxBrightness = grubBrightness("max_brightness");
    char *actualBrightness = grubBrightness("actual_brightness");
    removeNewLine(maxBrightness);
    removeNewLine(actualBrightness);


    Args args = parseArgs(argc, argv, maxBrightness, actualBrightness);


    if (args.printHelp == true) {
        printHelp();
    } else if (args.printVersion == true) {
        printVersion();
    } else if (args.printMaxBright == true) {
        printMaxBrightness(maxBrightness);
    } else if (args.printCurrentBright == true) {
        printCurrentBrightness(actualBrightness);
    } else if (args.printTheCurrentBrightness == true) {
        printTheCurrentBrightness(actualBrightness, maxBrightness);
    } else if (args.directBrightnessWithoutSign == true || args.percenteBrightnessWithoutSign == true || args.directBrightness == true || args.percenteBrightness == true) {
        writeBright(args.intensity);
    }



    free(maxBrightness);
    free(actualBrightness);
    free(args.intensity);
    return 0;
}

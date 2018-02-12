#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ELEMENTS 50

// Param structure
typedef struct
{
    char name[128];
    char value[128];
} Param;

// Function to remove substring from an string
void removeSubstring(char *s, const char *toremove)
{
    while(s = strstr(s, toremove))
        memmove(s, s + strlen(toremove), 1 + strlen(s + strlen(toremove)));
}

// Function used for string replace
void replaceSubstr(char *str, char *orig, char *rep, int start)
{
    static char temp[4096];
    static char buffer[4096];
    char *p;
    
    strcpy(temp, str + start);
    
    if(!(p = strstr(temp, orig)))  // Is 'orig' even in 'temp'?
        return;
    
    strncpy(buffer, temp, p - temp); // Copy characters from 'temp' start to 'orig' str
    buffer[p - temp] = '\0';
    
    sprintf(buffer + (p - temp), "%s%s", rep, p + strlen(orig));
    sprintf(str + start, "%s", buffer);
}

void removeChar(char *str, char garbage)
{
    
    char *src, *dst;
    for(src = dst = str; *src != '\0'; src++)
    {
        *dst = *src;
        if(*dst != garbage) dst++;
    }
    *dst = '\0';
}

void removeCharByIndex(char *str, unsigned int index)
{
    char *src;
    for(src = str + index; *src != '\0'; *src = *(src + 1), ++src);
    *src = '\0';
}

int getFirstIndex(char *str, char c)
{
    char *e = strchr(str, c);
    int index = (int) (e - str);
    
    return index;
}

char *substring(const char *str, size_t begin, size_t len)
{
    if(str == 0 || strlen(str) == 0 || strlen(str) < begin || strlen(str) < (begin + len))
        return 0;
    
    return strndup(str + begin, len);
}

char *strndup(const char *s, size_t n)
{
    char *p = memchr(s, '\0', n);
    if(p != NULL)
        n = p - s;
    p = malloc(n + 1);
    if(p != NULL)
    {
        memcpy(p, s, n);
        p[n] = '\0';
    }
    return p;
}

int main(int argc, char *argv[])
{
    FILE *file;
    
    char filename[] = "input.txt";
    
    if(argv[1])
        file = fopen(argv[1], "r");
    else
        file = fopen(filename, "r");
    
    if(!file)
    {
        fprintf(stderr, "ERROR: Can't read input file!\n");
        exit(1);
    }
    
    printf("[General Information]\n");
    /// Read General Information
    /*******************************************************************************************/
    Param general_information_array[MAX_ELEMENTS]; // An array with 22 params
    for(int i=0; i < MAX_ELEMENTS; i++)
    {
        strcpy(general_information_array[i].name, "");
        strcpy(general_information_array[i].value, "");
    }
    
    // Now proceed with reading lines which contains general information
    char line[256];
    int validReads = 0;
    int foundHeader = 0;
    
    while(fgets(line, sizeof line,
                file) != NULL) /* loop through every line from file in order to get necessery lines */
    {
        if(validReads >= MAX_ELEMENTS)
            break;
    
        if(strstr(line, "General SMART Values") != NULL)    // if end of general information values is reached
            break;
        
        if(!foundHeader && !strstr(line, "=== START OF INFORMATION SECTION ===") != NULL)
        {
            foundHeader = 1;
            continue;
        }
        
        if(strstr(line, ":") != NULL)
        {
            // Prepare the line for split
            removeSubstring(line, "\n");
            removeSubstring(line, "\t");
            removeSubstring(line, "  ");
            replaceSubstr(line, ": ", ":", 0); // At this point, the format is name:val
            
            // Fast way is to just replace ":" with "=" but profi way is to split strings and push back to initial array
            char *p = strtok(line, ":");
            if(p != NULL)
            {
                //general_information_array[validReads].name = p;
                strcpy(general_information_array[validReads].name, p);
                
                p = strtok(NULL, ":");
                if(p)
                strcpy(general_information_array[validReads].value, p);
            }
            
            //Custom operations
            
            // Debug
            printf("%s=%s\n", general_information_array[validReads].name, general_information_array[validReads].value);
            validReads++;
        }
        
    }
    
    printf("\n[S.M.A.R.T. Attributes]\n");
    
    /// Smart ATTRIBTES
    /*******************************************************************************************/
    Param smart_attributes[MAX_ELEMENTS];
    for(int i=0; i < MAX_ELEMENTS; i++)
    {
        strcpy(smart_attributes[i].name, "");
        strcpy(smart_attributes[i].value, "");
    }
    
    validReads = 0;
    foundHeader = 0;
    
    while(fgets(line, sizeof line, file) != NULL) /* loop through every line from file */
    {
        if(validReads > MAX_ELEMENTS)
            break;
    
        if(strstr(line, "_ K auto-keep") != NULL)   // if eand of smart atributes is reached
            break;
        
        if(!foundHeader)
        {
            if(strstr(line, "ATTRIBUTE_NAME") != NULL)
            {
                foundHeader = 1;
                continue;
            }
            else
                continue;
        }
        
        // Prepare the line for split
        removeSubstring(line, "\n");
        removeSubstring(line, "\t");
        removeSubstring(line, "  ");
        if(isspace(line[0])) removeCharByIndex(line, 0);
        char *new = substring(line, (size_t) getFirstIndex(line, ' '),
                              strlen(line) - (size_t) getFirstIndex(line, ' '));
        replaceSubstr(new, " -", "-", 0);
        
        // Get name
        if(strstr(new, "End-to-End_Error"))             // it's special because it contains separator "-"
        {
            strcpy(smart_attributes[validReads].name, "End-to-End_Error");
        }
        else
        {
            removeSubstring(new, "P-S");
            strcpy(smart_attributes[validReads].name, substring(new, 1, (size_t) getFirstIndex(new, '-') - 1));
            removeSubstring(smart_attributes[validReads].name, " POS");
            removeSubstring(smart_attributes[validReads].name, " PO");
            removeSubstring(smart_attributes[validReads].name, " P-S");
            removeSubstring(smart_attributes[validReads].name, " P");
        }
        
        // Get value
        char *tmp = strrchr(new, '-');
        strcpy(smart_attributes[validReads].value, substring(tmp, 1, strlen(tmp) - 1));
        
        // Debug
        printf("%s=%s\n", smart_attributes[validReads].name, smart_attributes[validReads].value);
        validReads++;
    }
    
    printf("\n[Device Statistics]\n");
    
    /// Device statistics
    /*******************************************************************************************/
    Param device_statistics[MAX_ELEMENTS];      // Define array and initialize
    for(int i=0; i < MAX_ELEMENTS; i++)
    {
        strcpy(device_statistics[i].name, "");
        strcpy(device_statistics[i].value, "");
    }
    
    validReads = 0;
    foundHeader = 0;
    
    while(fgets(line, sizeof line,
                file) != NULL) /* loop through every line from file in order to get necessery lines */
    {
        if(validReads > MAX_ELEMENTS)   // Stop if tehere is not space in array anymore!
            break;
    
        if(strstr(line, "_ C monitored condition met") != NULL) // if end of statistics is reached
            break;
        
        if(!foundHeader)
        {
            if(strstr(line, "0x01  =====  =") != NULL)
            {
                foundHeader = 1;
                continue;
            }
            else
                continue;
        }
        
        if(strstr(line, "=====") != NULL)
            continue;
        
        // Prepare the line for split
        removeSubstring(line, "\n");
        removeSubstring(line, "\t");
        removeSubstring(line, "          ");
        replaceSubstr(line, " --- ", "-", 0);
        
        // Get name
        char *new = substring(line, (size_t) getFirstIndex(line, '-'), strlen(line) - getFirstIndex(line, '-'));
        new = substring(new, 2, strlen(new) - 2);
        strcpy(device_statistics[validReads].name, new);
        removeSubstring(device_statistics[validReads].name, "  ");
        
        // Tokenize and stuff for value
        char *tmp = strdup(line);
        char *p = strtok(tmp, " ");
        p = strtok(NULL, " ");
        p = strtok(NULL, " ");
        p = strtok(NULL, " ");
        strcpy(device_statistics[validReads].value, p);
        
        printf("%s=%s\n", device_statistics[validReads].name, device_statistics[validReads].value);
        validReads++;
    }
    fclose(file);
    
    /// Export data to INI file
    /********************************************************************************/
    FILE *fw;
    // Write to output!
    if(argc >= 2 && argv[2] != NULL)
        fw = fopen(argv[2], "w");
    else
        fw = fopen("output.INI", "w");
    
    if(fw == NULL)
    {
        printf("Unable to create/write to file!");
        exit(1);
    }
    
    /* Data to write:
    
    [General Information]
    Param general_information_array[22]; // An array with 22 params
    
    [S.M.A.R.T. Attributes]
    Param smart_attributes[23];
    
    [Device Statistics]
    Param device_statistics[7];
    
    */
    
    fprintf(file, "[General Information]\n" );
    int i=0;
    for(i=0; i < 22; i++)
    {
        if( strcmp(general_information_array[i].name, "") != 0 )
        {
            fprintf(fw, "%s=%s\n", general_information_array[i].name, general_information_array[i].value);
        }
    }
    
    fprintf(fw, "\n[S.M.A.R.T. Attributes]\n");
    for(i=0; i < 23; i++)
    {
        if(strcmp(smart_attributes[i].name, "") != 0 )
            fprintf(fw, "%s=%s\n", smart_attributes[i].name, smart_attributes[i].value);
    }
    
    fprintf(fw, "\n[Device Statistics]\n");
    for(i=0; i < MAX_ELEMENTS; i++)
    {
        if( strcmp(device_statistics[i].name, "") != 0 )
            fprintf(fw, "%s=%s\n", device_statistics[i].name, device_statistics[i].value);
    }
    
    fclose(fw);
    return 0;
}
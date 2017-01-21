#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv) {
    if(argc >= 2) {
        char* sectionPath = argv[1];
        FILE* f = fopen(sectionPath,"rb");

        if(!f){
            printf("Specified Section not found");
            exit(EXIT_FAILURE);
        }

        fseek(f,0,SEEK_END);
        size_t size = (size_t) ftell(f);
        fseek(f,0,SEEK_SET);
        void* section = malloc(size);

        memset(section,0x0,size);
        fread(section,1,size,f);

        fclose(f);

        printf("Section size: 0x%x\n", (unsigned int) size);

        int index = 0;
        int* ncchOffsetList = malloc(size); //We are allocating WAY to much here

        for (int i = 0;i < size;i++){
            if(strncmp(section + i + 0x100,"NCCH",4) == 0){
                ncchOffsetList[index] = i;
                index++;
            }
        }

        printf("NCCH's found: %d\n",index);

        for (int j = 0; j < index; j++) {
            int k = ncchOffsetList[j];
            int local_size;

            if(j+1 >= index){
                local_size = (int)size - k;
            }else {
                local_size = ncchOffsetList[j+1] - k;
            }

            printf("NCCH offset: 0x%x\n",k);
            printf("NCCH size: 0x%x\n",local_size);
            printf("NCCH product code: %s\n",(char*)(section + k + 0x150));

            int pathSize = snprintf(0,0,"%d.bin",j) + 1;
            char* path = malloc((size_t) pathSize);

            snprintf(path, (size_t) pathSize, "%d.bin", j);
            FILE* d = fopen(path,"w");
            fwrite(section + k, 1, (size_t) local_size, d);
            fclose(d);

            free(path);
        }
        free(ncchOffsetList);
        free(section);

        printf("Finished");

        exit(EXIT_SUCCESS);
    } else {
        printf("firmncchextract [firm_section_0]");
        exit(EXIT_FAILURE);
    }
}

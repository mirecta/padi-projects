#include "mirofs.h"
#include "test.h"
#include <stdio.h>


RoMem romem((void*) mfsData);

MFS mFs(romem);

int main(int argc, char *argv[]){
if (argc !=2){
    return -1;
}
int res = mFs.init();
if(res == 0){
    printf("error mFs.init()\n");
    return -1;
}
MFSFile *file = mFs.open(argv[1]);
if(file == 0){
    printf("error mFs.open\n");
    return -1;
}
//write to another file
char buffer[2000];
FILE *f = fopen("out.bin", "wb");
while(true){
    int readlen = file->read(buffer,2000);
    if(readlen){
        fwrite(buffer, 1, readlen, f);
        
    }else{
        fclose(f);
        break;
    }
}

    return 0;
}
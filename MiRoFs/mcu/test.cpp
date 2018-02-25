#include "mirofs.h"
#include "test.h"
#include <stdio.h>

RoMem romem((void*) mfs);

MFS mFs(romem);

int main(int argc, char *argv[]){
if (argc !=2){
    return -1;
}
int res = mFs.init();
if(res == 0){
    printf("error mFs.init()\n");
}
MFSFile *file = mFs.open(argv[1]);
if(file == 0){
    printf("error mFs.open\n");
}


    return 0;
}
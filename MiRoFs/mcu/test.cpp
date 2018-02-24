#include "mirofs.h"
#include "test.h"
#include <stdio.h>

RoMem romem((void*) mfs);

MFS mFs(romem);

int main(){

int res = mFs.init();
if(res == 0){
    printf("error mFs.init()\n");
}
    return 0;
}
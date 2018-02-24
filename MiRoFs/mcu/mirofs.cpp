
#include <memory.h>
#include <string.h>
#include "mirofs.h"
#include <stdio.h>

MFSFHeader mfsFHeader;
MFSFile mfsFile;

uint32_t RoMem::_readBytes(const void *addr, void *data, uint32_t size) const{
    memcpy(data,addr,size);
    return size;
}

uint32_t MFSFile::read(void *buf, uint32_t size){

    if(roMem == 0) return 0;
    if (size > header.size - position){
        size = header.size - position; 
    }
    size = roMem->readBytes(offset + position, buf, size);
    position += size;
    return size;
}
void MFSFile::close(){
   roMem = 0;
   offset = 0;
   position = 0; 
}

int MFS::init(){
    //read header
    uint32_t r = roMem.readBytes(0,&header,sizeof(header));
    if (r != sizeof(header)) return 0;
    //compare filename sizes
    if( header.fnameSize !=FILENAME_SIZE) return 0;
    return 1;
}

MFSFile* MFS::open(const char*filename){
    if (!filename) return 0;
    int len = strlen(filename);
    if (len == 0) return 0;
    if (filename[0] == '/'){
        filename = &filename[1];
    }
    //now we can try find file
    int first = 0;
    int count = header.count;
    int it = 0;
    int step = 0;
    while (count > 0) {
        it = first;
        step = count/2;
        it += step;
        //printf(">>>>count %d, step %d, it %d first %d\n",count,step,it,first);
        //read file header at it
        roMem.readBytes((sizeof(MFSHeader) + it * sizeof(MFSFHeader)),&mfsFHeader,sizeof(MFSFHeader));
        //printf("%d %s\n",it,mfsFHeader.fname);
        if (strcmp(mfsFHeader.fname,filename) < 0) {
            first = ++it;
            count -= step + 1;
        } else {
            count=step;
        }
        //printf("count %d, step %d, it %d first %d\n",count,step,it,first);
    }
    roMem.readBytes((sizeof(MFSHeader) + first * sizeof(MFSFHeader)),&mfsFHeader,sizeof(MFSFHeader));
    int index = (first != (int)header.count && !strcmp(mfsFHeader.fname,filename )) ? first : -1;
    if (index >= 0){
        printf("found:%d %s \n",index,mfsFHeader.fname);
    }else{
       printf("not found:%d %s\n", index,filename); 
    }

    //printf("%d %s\n",l,mfsFHeader.fname);
    //l is possible result
    return 0;
}
/*
int bs_upper_bound(int a[], int n, int x) {
    int l = 0;
    int h = n; // Not n - 1
    while (l < h) {
        int mid = (l + h) / 2;
        if (x >= a[mid]) {
            l = mid + 1;
        } else {
            h = mid;
        }
    }
    return l;
}

int bs_lower_bound(int a[], int n, int x) {
    int l = 0;
    int h = n; // Not n - 1
    while (l < h) {
        int mid = (l + h) / 2;
        if (x <= a[mid]) {
            h = mid;
        } else {
            l = mid + 1;
        }
    }
    return l;
}*/


#include<memory.h>
#include "mirofs.h"


uint32_t RoMem::_readBytes(uint8_t *addr, uint8_t *data, uint32_t size){
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
    //compare filename sizes
}

MFSFile* MFS::open(const char*filename){
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
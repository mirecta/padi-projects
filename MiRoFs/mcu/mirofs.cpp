
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

    if(roMem == 0 || header == 0) return 0;
    if (size > header->size - position){
        size = header->size - position; 
    }
    size = roMem->readBytes(header->offset + position, buf, size);
    position += size;
    return size;
}

void MFSFile::close(){
   roMem = 0;
   header = 0;
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

MFSFHeader* MFS::getFhAt(int i){
    if ((uint32_t)i >= header.count){
        return 0;
    }
    roMem.readBytes((sizeof(MFSHeader) + i * sizeof(MFSFHeader)),&mfsFHeader,sizeof(MFSFHeader));
    return &mfsFHeader;
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
        if (strcmp(getFhAt(it)->fname,filename) < 0) {
            first = ++it;
            count -= step + 1;
        } else {
            count=step;
        }
    }
    if((uint32_t)first != header.count && !strcmp(getFhAt(first)->fname,filename)){
        //printf("found:%d %s \n",first,mfsFHeader.fname);
        mfsFile.close();
        mfsFile.header = &mfsFHeader;
        mfsFile.roMem = &roMem;
        return &mfsFile;
    }else{
       //printf("not found:%d %s\n", first,filename); 
       return 0;
    }
}
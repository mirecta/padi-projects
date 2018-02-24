#ifndef _MIROFS_H_
#define _MIROFS_H_
#include <inttypes.h>

#define FILENAME_SIZE 36

class RoMem{
public:
    RoMem(const void *addr, uint32_t size = 0)
    :addr(addr), memSize(size)
    {}

    uint32_t readBytes(uint32_t offset, void *data, uint32_t size) const {
        if (memSize !=0){
            if (offset >= memSize)
                return 0;
            if (offset + size >= memSize)
                size = memSize - offset;
        }

        return _readBytes((uint8_t*)addr + offset, data, size);
    }
private:
    virtual uint32_t _readBytes(const void *addr, void *data, uint32_t size) const;

private:
    const void* addr;
    uint32_t memSize;
};
/*
fs header
|MiRoFs(6)|fnamesize(1)|size(4)|filecount(4)| = 15
fileheader
|fname(FILENAME_SIZE)|size(4)|offset(4)| = 8 + FILENAME_SIZE
*/
struct MFSHeader {
    char magic[6];
    uint8_t fnameSize;
    uint32_t size;
    uint32_t endOffset;
} __attribute__((packed));

struct MFSFHeader{
   char fname[FILENAME_SIZE];
   uint32_t size;
   uint32_t offset; 
}__attribute__((packed));

class MFSFile {
    friend class MFS;
public:
    MFSFile():roMem(0) {}

    uint32_t size() const 
    {
        return header.size;
    }
    const char* name() const
    {
        return header.fname;
    }
    uint32_t read(void *buf, uint32_t size);
    void close();

private:
    MFSFHeader header;
    uint32_t offset;
    uint32_t position;
    RoMem *roMem;
};

class MFS {
    public: 
        MFS(RoMem& roMem)
        :roMem(roMem){};

        int init();
        MFSFile* open(const char *filename);

    private:
        RoMem &roMem;
        MFSHeader header;
};

#endif //_MIROFS_H_
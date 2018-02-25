#!/usr/bin/python
import argparse
from struct import *
import os
import sys
from cStringIO import StringIO
import zlib

"""
deflate_compress = zlib.compressobj(9, zlib.DEFLATED, -zlib.MAX_WBITS)
zlib_compress = zlib.compressobj(9, zlib.DEFLATED, zlib.MAX_WBITS)
gzip_compress = zlib.compressobj(9, zlib.DEFLATED, zlib.MAX_WBITS | 16)
compressor = gzip_compress
"""
"""
fs header
|MiRoFs(6)|fnamesize(1)|size(4)|filecount(4)| = 15
fileheader
|fname(FILENAME_SIZE)|size(4)|offset(4)| = 8 + FILENAME_SIZE

"""

def main():
    parser = argparse.ArgumentParser(description='Pack MiRoFs image')
    
    parser.add_argument('-u','--sug', dest='suggest', action='store_const', const=1, default=0,
                   help='Make suggestion of filename lenght')
    parser.add_argument('-g','--gzip', dest='gzip', action='store_const', const=1, default=0,
                   help='Gzipped every file - reduce size and good for webserver serving gzipped content')
    parser.add_argument('-t','--type', dest='type', default="bin",
                   help='Output type c or bin ')

    parser.add_argument('-s','--size',dest='size',help="Max filename size" ,type=int ,default=56)

    parser.add_argument('-o','--out',dest='out',required=True, help="Output image filename")

    parser.add_argument('-r','--root',dest='root',required=True, help="Root directory to be packed into image")

    args = parser.parse_args()

    fls = []
    maindir = args.root
    if maindir[-1] != '/':
        maindir += '/'
    fssize = 15
    maxFnSize = 0;
    for root, dirs, files in os.walk(maindir, topdown=False):
        for name in files:
            fln = os.path.join(root, name)
            fln = fln.replace(maindir,'')
            fls.append([fln,0])

            if len(fln) > maxFnSize:
                maxFnSize = len(fln)
            if len(fln) >= args.size:
                print("filename: %s exceed size limit: %d" % (fln,args.size))
                sys.exit(-1)
        #endfor
    #endfor
    if args.suggest:
        print("Suggested filename size is: %d" % (maxFnSize + 10))
        sys.exit(0)
    
    count = len(fls)
    #sort for binary search     
    fls.sort(key=lambda tup: tup[0])

    out = StringIO()
    contentPart = StringIO()
    #write datapart first because possibly compress ..  change the game
    for item in fls:
        infl = open(maindir + item[0],'rb')
        inData = infl.read()
        infl.close()
        if args.gzip:
            compressor = zlib.compressobj(9, zlib.DEFLATED, zlib.MAX_WBITS | 16)
            inData = compressor.compress(inData) + compressor.flush()
        #endif
        contentPart.write(inData)
        item[1] = len(inData)
        fssize += 8 + args.size + item[1]
    #endfor
    #write image header
    out.write('MiRoFs')
    out.write(pack('B',args.size))
    out.write(pack('II',fssize,count))
    offset = 15 + count*(8 + args.size) 
    ext = ''
    if args.gzip:
        ext = ".gz"
    #endif  
    for item in fls:
        out.write(pack('%ssII'%args.size,item[0] + ext,item[1],offset))
        offset += item[1]
    #endfor
    if args.type == 'bin':
        ext = '.iso'
    else:
        ext = '.h'
    #endif
    outfl = open(args.out + ext,'wb')
    data = out.getvalue() + contentPart.getvalue()
    ldata = len(data)
    if args.type == 'bin':
        outfl.write(data)
    else:
        outfl.write("const unsigned char mfsData[] = { \n")
        
        i = 0
        for i in xrange(0,ldata,30):
            outfl.write(','.join(map(hex,map(ord,data[i:i+30]))))
            if (i + 30) < ldata:
                outfl.write(',\n')
            else:
                outfl.write('\n')
            #endif
        #endfor
        outfl.write("};\n")
    #endif
    outfl.close()
    print ("Image contains %d files, and total size is %d bytes."%(count,fssize))


if __name__ == "__main__":
    main()

### BZIP2 ###

set(BZIP2_LIB_NAME "third-party/bzip2")

set(BZIP2_SOURCES
    "${BZIP2_LIB_NAME}/blocksort.c"
    "${BZIP2_LIB_NAME}/bzerror.c"
    "${BZIP2_LIB_NAME}/bzlib.c"
    "${BZIP2_LIB_NAME}/compress.c"
    "${BZIP2_LIB_NAME}/crctable.c"
    "${BZIP2_LIB_NAME}/decompress.c"
    "${BZIP2_LIB_NAME}/huffman.c"
    "${BZIP2_LIB_NAME}/randtable.c"
)

set(BZIP2_HEADERS
    "${BZIP2_LIB_NAME}/bzlib.h"
    "${BZIP2_LIB_NAME}/bzlib_private.h"
)

### LZMA ###

set(LZMA_LIB_NAME "third-party/lzma")

set(LZMA_SOURCES
    "${LZMA_LIB_NAME}/7zAlloc.c"
    "${LZMA_LIB_NAME}/7zBuf.c"
    "${LZMA_LIB_NAME}/7zBuf2.c"
    "${LZMA_LIB_NAME}/7zCrc.c"
    "${LZMA_LIB_NAME}/7zCrcOpt.c"
    "${LZMA_LIB_NAME}/7zDec.c"
    "${LZMA_LIB_NAME}/7zFile.c"
    "${LZMA_LIB_NAME}/7zIn.c"
    "${LZMA_LIB_NAME}/7zStream.c"
    "${LZMA_LIB_NAME}/Alloc.c"
    "${LZMA_LIB_NAME}/Bcj2.c"
    "${LZMA_LIB_NAME}/Bra.c"
    "${LZMA_LIB_NAME}/Bra86.c"
    "${LZMA_LIB_NAME}/BraIA64.c"
    "${LZMA_LIB_NAME}/CpuArch.c"
    "${LZMA_LIB_NAME}/Delta.c"
    "${LZMA_LIB_NAME}/LzFind.c"
    "${LZMA_LIB_NAME}/LzFindMt.c"
    "${LZMA_LIB_NAME}/Lzma2Dec.c"
    "${LZMA_LIB_NAME}/Lzma2Enc.c"
    "${LZMA_LIB_NAME}/Lzma86Dec.c"
    "${LZMA_LIB_NAME}/Lzma86Enc.c"
    "${LZMA_LIB_NAME}/LzmaDec.c"
    "${LZMA_LIB_NAME}/LzmaEnc.c"
    "${LZMA_LIB_NAME}/LzmaLib.c"
    "${LZMA_LIB_NAME}/MtCoder.c"
    "${LZMA_LIB_NAME}/Ppmd7.c"
    "${LZMA_LIB_NAME}/Ppmd7Dec.c"
    "${LZMA_LIB_NAME}/Ppmd7Enc.c"
    "${LZMA_LIB_NAME}/Sha256.c"
    "${LZMA_LIB_NAME}/Threads_.c"
    "${LZMA_LIB_NAME}/Xz.c"
    "${LZMA_LIB_NAME}/XzCrc64.c"
    "${LZMA_LIB_NAME}/XzDec.c"
    "${LZMA_LIB_NAME}/XzEnc.c"
    "${LZMA_LIB_NAME}/XzIn.c"
)

set(LZMA_HEADERS
    "${LZMA_LIB_NAME}/7z.h"
    "${LZMA_LIB_NAME}/7zAlloc.h"
    "${LZMA_LIB_NAME}/7zBuf.h"
    "${LZMA_LIB_NAME}/7zCrc.h"
    "${LZMA_LIB_NAME}/7zFile.h"
    "${LZMA_LIB_NAME}/7zVersion.h"
    "${LZMA_LIB_NAME}/Alloc.h"
    "${LZMA_LIB_NAME}/Bcj2.h"
    "${LZMA_LIB_NAME}/Bra.h"
    "${LZMA_LIB_NAME}/CpuArch.h"
    "${LZMA_LIB_NAME}/Delta.h"
    "${LZMA_LIB_NAME}/LzFind.h"
    "${LZMA_LIB_NAME}/LzFindMt.h"
    "${LZMA_LIB_NAME}/LzHash.h"
    "${LZMA_LIB_NAME}/Lzma2Dec.h"
    "${LZMA_LIB_NAME}/Lzma2Enc.h"
    "${LZMA_LIB_NAME}/Lzma86.h"
    "${LZMA_LIB_NAME}/LzmaDec.h"
    "${LZMA_LIB_NAME}/LzmaEnc.h"
    "${LZMA_LIB_NAME}/LzmaLib.h"
    "${LZMA_LIB_NAME}/MtCoder.h"
    "${LZMA_LIB_NAME}/Ppmd.h"
    "${LZMA_LIB_NAME}/Ppmd7.h"
    "${LZMA_LIB_NAME}/RotateDefs.h"
    "${LZMA_LIB_NAME}/Sha256.h"
    "${LZMA_LIB_NAME}/Threads_.h"
    "${LZMA_LIB_NAME}/Types.h"
    "${LZMA_LIB_NAME}/Xz.h"
    "${LZMA_LIB_NAME}/XzCrc64.h"
    "${LZMA_LIB_NAME}/XzEnc.h"
)

### ZLIB ###

set(ZLIB_LIB_NAME "third-party/zlib")

set(ZLIB_SOURCES
    "${ZLIB_LIB_NAME}/adler32.c"
    "${ZLIB_LIB_NAME}/compress.c"
    "${ZLIB_LIB_NAME}/crc32.c"
    "${ZLIB_LIB_NAME}/deflate.c"
    "${ZLIB_LIB_NAME}/infback.c"
    "${ZLIB_LIB_NAME}/inffast.c"
    "${ZLIB_LIB_NAME}/inflate.c"
    "${ZLIB_LIB_NAME}/inftrees.c"
    "${ZLIB_LIB_NAME}/trees.c"
    "${ZLIB_LIB_NAME}/uncompr.c"
    "${ZLIB_LIB_NAME}/zutil.c"
)

set(ZLIB_HEADERS
    "${ZLIB_LIB_NAME}/crc32.h"
    "${ZLIB_LIB_NAME}/deflate.h"
    "${ZLIB_LIB_NAME}/gzguts.h"
    "${ZLIB_LIB_NAME}/inffast.h"
    "${ZLIB_LIB_NAME}/inffixed.h"
    "${ZLIB_LIB_NAME}/inflate.h"
    "${ZLIB_LIB_NAME}/inftrees.h"
    "${ZLIB_LIB_NAME}/trees.h"
    "${ZLIB_LIB_NAME}/zconf.h"
    "${ZLIB_LIB_NAME}/zlib.h"
    "${ZLIB_LIB_NAME}/zutil.h"
)

### ZIPLIB ###

set(ZIPLIB_LIB_NAME "third-party/ZIPLIB")

set(ZIPLIB_SOURCES
    "${ZIPLIB_LIB_NAME}/detail/EndOfCentralDirectoryBlock.cpp"
    "${ZIPLIB_LIB_NAME}/detail/ZipCentralDirectoryFileHeader.cpp"
    "${ZIPLIB_LIB_NAME}/detail/ZipGenericExtraField.cpp"
    "${ZIPLIB_LIB_NAME}/detail/ZipLocalFileHeader.cpp"
    "${ZIPLIB_LIB_NAME}/ZipArchive.cpp"
    "${ZIPLIB_LIB_NAME}/ZipArchiveEntry.cpp"
    "${ZIPLIB_LIB_NAME}/ZipFile.cpp"
)

set(ZIPLIB_HEADERS
    "${ZIPLIB_LIB_NAME}/compression/bzip2/bzip2_decoder.h"
    "${ZIPLIB_LIB_NAME}/compression/bzip2/bzip2_decoder_properties.h"
    "${ZIPLIB_LIB_NAME}/compression/bzip2/bzip2_encoder.h"
    "${ZIPLIB_LIB_NAME}/compression/bzip2/bzip2_encoder_properties.h"
    "${ZIPLIB_LIB_NAME}/compression/compression_interface.h"
    "${ZIPLIB_LIB_NAME}/compression/deflate/deflate_decoder.h"
    "${ZIPLIB_LIB_NAME}/compression/deflate/deflate_decoder_properties.h"
    "${ZIPLIB_LIB_NAME}/compression/deflate/deflate_encoder.h"
    "${ZIPLIB_LIB_NAME}/compression/deflate/deflate_encoder_properties.h"
    "${ZIPLIB_LIB_NAME}/compression/lzma/detail/lzma_alloc.h"
    "${ZIPLIB_LIB_NAME}/compression/lzma/detail/lzma_handle.h"
    "${ZIPLIB_LIB_NAME}/compression/lzma/detail/lzma_header.h"
    "${ZIPLIB_LIB_NAME}/compression/lzma/detail/lzma_in_stream.h"
    "${ZIPLIB_LIB_NAME}/compression/lzma/detail/lzma_out_stream.h"
    "${ZIPLIB_LIB_NAME}/compression/lzma/lzma_decoder.h"
    "${ZIPLIB_LIB_NAME}/compression/lzma/lzma_decoder_properties.h"
    "${ZIPLIB_LIB_NAME}/compression/lzma/lzma_encoder.h"
    "${ZIPLIB_LIB_NAME}/compression/lzma/lzma_encoder_properties.h"
    "${ZIPLIB_LIB_NAME}/compression/store/store_decoder.h"
    "${ZIPLIB_LIB_NAME}/compression/store/store_decoder_properties.h"
    "${ZIPLIB_LIB_NAME}/compression/store/store_encoder.h"
    "${ZIPLIB_LIB_NAME}/compression/store/store_encoder_properties.h"
    "${ZIPLIB_LIB_NAME}/detail/EndOfCentralDirectoryBlock.h"
    "${ZIPLIB_LIB_NAME}/detail/ZipCentralDirectoryFileHeader.h"
    "${ZIPLIB_LIB_NAME}/detail/ZipGenericExtraField.h"
    "${ZIPLIB_LIB_NAME}/detail/ZipLocalFileHeader.h"
    "${ZIPLIB_LIB_NAME}/methods/Bzip2Method.h"
    "${ZIPLIB_LIB_NAME}/methods/DeflateMethod.h"
    "${ZIPLIB_LIB_NAME}/methods/ICompressionMethod.h"
    "${ZIPLIB_LIB_NAME}/methods/LzmaMethod.h"
    "${ZIPLIB_LIB_NAME}/methods/StoreMethod.h"
    "${ZIPLIB_LIB_NAME}/methods/ZipMethodResolver.h"
    "${ZIPLIB_LIB_NAME}/streams/compression_decoder_stream.h"
    "${ZIPLIB_LIB_NAME}/streams/compression_encoder_stream.h"
    "${ZIPLIB_LIB_NAME}/streams/crc32stream.h"
    "${ZIPLIB_LIB_NAME}/streams/memstream.h"
    "${ZIPLIB_LIB_NAME}/streams/nullstream.h"
    "${ZIPLIB_LIB_NAME}/streams/serialization.h"
    "${ZIPLIB_LIB_NAME}/streams/streambuffs/compression_decoder_streambuf.h"
    "${ZIPLIB_LIB_NAME}/streams/streambuffs/compression_encoder_streambuf.h"
    "${ZIPLIB_LIB_NAME}/streams/streambuffs/crc32_streambuf.h"
    "${ZIPLIB_LIB_NAME}/streams/streambuffs/mem_streambuf.h"
    "${ZIPLIB_LIB_NAME}/streams/streambuffs/null_streambuf.h"
    "${ZIPLIB_LIB_NAME}/streams/streambuffs/sub_streambuf.h"
    "${ZIPLIB_LIB_NAME}/streams/streambuffs/tee_streambuff.h"
    "${ZIPLIB_LIB_NAME}/streams/streambuffs/zip_crypto_streambuf.h"
    "${ZIPLIB_LIB_NAME}/streams/substream.h"
    "${ZIPLIB_LIB_NAME}/streams/teestream.h"
    "${ZIPLIB_LIB_NAME}/streams/zip_cryptostream.h"
    "${ZIPLIB_LIB_NAME}/utils/enum_utils.h"
    "${ZIPLIB_LIB_NAME}/utils/stream_utils.h"
    "${ZIPLIB_LIB_NAME}/utils/time_utils.h"
    "${ZIPLIB_LIB_NAME}/ZipArchive.h"
    "${ZIPLIB_LIB_NAME}/ZipArchiveEntry.h"
    "${ZIPLIB_LIB_NAME}/ZipFile.h"
)

#set(ZIPLIB_INC_PATH "${ZIPLIB_LIB_NAME}/")
#
#add_library(${ZIPLIB_LIB_NAME}
#    STATIC
#        ${ZIPLIB_SOURCES}
#        ${ZIPLIB_HEADERS}
# )
#
#target_include_directories(${ZIPLIB_LIB_NAME}
#    PUBLIC
#        "${ZIPLIB_INC_PATH}"
#        "${ZLIB_INC_PATH}"
#        "${LZMA_INC_PATH}"
#        "${BZIP2_INC_PATH}"
#)

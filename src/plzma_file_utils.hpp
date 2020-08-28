//
// By using this Software, you are accepting original [LZMA SDK] and MIT license below:
//
// The MIT License (MIT)
//
// Copyright (c) 2015 - 2020 Oleh Kulykov <olehkulykov@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//


#ifndef __PLZMA_FILE_UTILS_HPP__
#define __PLZMA_FILE_UTILS_HPP__

#include <cstddef>

#include "plzma_private.h"
#include "../libplzma.hpp"

#include <ctype.h>
#include <stdio.h>

namespace plzma {
namespace fileUtils {
    
    inline int fileSeek(FILE * LIBPLZMA_NONNULL file, const int64_t offset, const int origin) noexcept {
#if defined(LIBPLZMA_MSC)
        return _fseeki64(file, offset, origin);
#elif defined(LIBPLZMA_POSIX)
        return fseeko(file, offset, origin);
#else
        assert(0);
        return -1;
#endif
    }
    
    inline int64_t fileTell(FILE * LIBPLZMA_NONNULL file) noexcept {
#if defined(LIBPLZMA_MSC)
        return _ftelli64(file);
#elif defined(LIBPLZMA_POSIX)
        return ftello(file);
#else
        assert(0);
        return -1;
#endif
    }
    
    extern bool fileErase(const Path & path, const plzma_erase eraseType);
    
    extern Pair<RawHeapMemory, size_t> fileContent(const Path & path);
    
} // namespace fileUtils
} // namespace plzma

#endif // !__PLZMA_FILE_UTILS_HPP__

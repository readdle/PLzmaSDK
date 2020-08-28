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


#ifndef __LIBPLZMA_H__
#define __LIBPLZMA_H__

#include <stdarg.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <wchar.h>

/// @file libplzma.h
/// @brief Single public header of the C library part.
///
/// This file contains type definitions, preprocessor definitions, common C functions
/// and optional bindings to the internal C++ part of the library.
/// Everything what you need to use this library in C | Objective-C | Swift env. is here.


/// @brief Manualy defined required version parts of the library. 0.0.4
/// The optinal `LIBPLZMA_VERSION_BUILD` might be befined by the CI or via cmake during the configuration.
#define LIBPLZMA_VERSION_MAJOR 0
#define LIBPLZMA_VERSION_MINOR 0
#define LIBPLZMA_VERSION_PATCH 4

// check windows
#if defined(WIN32) || defined(_WIN32) || defined(WIN32_LEAN_AND_MEAN) || defined(_WIN64) || defined(WIN64)
#define LIBPLZMA_OS_WINDOWS 1
#endif

// C extern
#if defined(__cplusplus)
#define LIBPLZMA_C_EXTERN extern "C"
#else
#define LIBPLZMA_C_EXTERN extern
#endif

// attribute
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define LIBPLZMA_ATTRIB __attribute__((visibility("default")))
#endif

// check attrib and define empty if not defined
#if !defined(LIBPLZMA_ATTRIB)
#define LIBPLZMA_ATTRIB
#endif

// if not using/building shared or static then static
#if !defined(LIBPLZMA_SHARED) && !defined(LIBPLZMA_STATIC)
#define LIBPLZMA_STATIC
#endif

// dll api
#if defined(LIBPLZMA_OS_WINDOWS) && defined(LIBPLZMA_SHARED)
#if defined(LIBPLZMA_BUILD)
#define LIBPLZMA_DYLIB_API __declspec(dllexport)
#else
#define LIBPLZMA_DYLIB_API __declspec(dllimport)
#endif
#endif

// check dll api and define empty if not defined
#if !defined(LIBPLZMA_DYLIB_API)
#define LIBPLZMA_DYLIB_API
#endif

// combined lib api
#define LIBPLZMA_C_API(return_type) LIBPLZMA_C_EXTERN LIBPLZMA_ATTRIB LIBPLZMA_DYLIB_API return_type
#define LIBPLZMA_CPP_API LIBPLZMA_ATTRIB LIBPLZMA_DYLIB_API

#if defined(__clang__)
#define LIBPLZMA_NULLABLE _Nullable
#define LIBPLZMA_NONNULL _Nonnull
#else
#define LIBPLZMA_NULLABLE
#define LIBPLZMA_NONNULL
#endif

#if !defined(__has_include)
#define __has_include(x) 0
#endif

#if !defined(__has_attribute)
#define __has_attribute(x) 0
#endif

#if __has_attribute(sentinel)
#define LIBPLZMA_REQUIRES_LAST_NULL_ARG __attribute__((sentinel))
#else
#define LIBPLZMA_REQUIRES_LAST_NULL_ARG
#endif

#if __has_attribute(warn_unused_result)
#define LIBPLZMA_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#else
#define LIBPLZMA_WARN_UNUSED_RESULT
#endif

#if defined(__GNUC__) || defined(__clang__)
#define LIBPLZMA_PACKED_ALIGNED_START(ALIGN)
#define LIBPLZMA_PACKED_ALIGNED_END(ALIGN) __attribute__((packed, aligned(ALIGN)))
#elif defined(_MSC_VER)
#define LIBPLZMA_PACKED_ALIGNED_START(ALIGN) __pragma( pack(push, ALIGN) )
#define LIBPLZMA_PACKED_ALIGNED_END(ALIGN) __pragma( pack(pop) )
#else
#define LIBPLZMA_PACKED_ALIGNED_START(ALIGN)
#define LIBPLZMA_PACKED_ALIGNED_END(ALIGN)
#endif

/// Types

/// @brief The type of erasing the content.
typedef enum plzma_erase {
    /// @brief The content will not be erased, i.e. ignored.
    plzma_erase_none    = 0,
    
    /// @brief Erase, rewrite the content with zeros.
    plzma_erase_zero    = 1
} plzma_erase;


/// @brief The type of the file, stream, data buffer, etc.
typedef enum plzma_file_type {
    /// @brief 7-zip type.
    ///
    /// This file type supports multiple archive items, password protected items list of the arhive and
    /// password protected content.
    /// @note Supports 'LZMA', 'LZMA2' and 'PPMd' compression methods.
    /// @link https://www.7-zip.org/7z.html
    plzma_file_type_7z          = 1,
    
    /// @brief XZ type.
    ///
    /// This file type supports only one arhive item, in a rest it same as 7-zip.
    /// @note Supports only 'LZMA2' compression method which is automatically selected.
    /// @link https://www.7-zip.org/7z.html
    plzma_file_type_xz          = 2,
    
    /// @brief TAR type.
    ///
    /// All archive items are combined and stored as one continuous stream without compression.
    /// For this type, the \a plzma_method is ignored.
    /// @link https://en.wikipedia.org/wiki/Tar_(computing)
    plzma_file_type_tar         = 3
} plzma_file_type;


/// Compression method.
typedef enum plzma_method {
    /// @brief Default and general compression method of 7z format.
    /// @link https://www.7-zip.org/7z.html
    plzma_method_LZMA =     1,
    
    /// @brief Improved version of LZMA
    /// @link https://www.7-zip.org/7z.html
    plzma_method_LZMA2 =    2,
    
    /// @brief Dmitry Shkarin's PPMdH with small changes.
    /// @link https://www.7-zip.org/7z.html
    plzma_method_PPMd =     3
} plzma_method;


/// @brief Exception error codes.
typedef enum plzma_error_code {
    /// @brief The error type cannot be determined.
    ///
    /// Might be used as a default value during the initialization
    /// or generic, unknown exception was catched during the execution.
    plzma_error_code_unknown            = 0,
    
    /// @brief Provided function or method arguments are invalid.
    plzma_error_code_invalid_arguments  = 1,
    
    /// @brief The required amount of memory can't be alocated or
    /// can't instantiate some object.
    plzma_error_code_not_enough_memory  = 2,
    
    /// @brief File or directory doesn't exists or there are no permissions to
    /// perform required action.
    plzma_error_code_io                 = 3,
    
    /// @brief Any internal errors or exceptions.
    plzma_error_code_internal           = 4
} plzma_error_code;


/// @brief The maximum type for holding all bitmask combinations of the \a plzma_open_dir_mode enumeration.
typedef uint8_t plzma_open_dir_mode_t;


/// @brief The enumeration with bitmask options for opening directory path.
/// Currently uses for defining behavior of directory iteration.
typedef enum plzma_open_dir_mode {
    /// @brief Follow the symbolic links.
    plzma_open_dir_mode_follow_symlinks     = 1 << 0
} plzma_open_dir_mode;


/// @brief Contains stat info of the path.
typedef struct plzma_path_stat {
    /// @brief Size in bytes.
    uint64_t size;
    
    /// @brief Last path access unix timestamp.
    time_t last_access;
    
    /// @brief Last path modification unix timestamp.
    time_t last_modification;
    
    /// @brief Path creation unix timestamp.
    time_t creation;
} plzma_path_stat;


/// @brief Callback type for deinitializing user defined context.
/// @param context The non-null reference to any user defined context.
typedef void (*plzma_context_deinitializer)(void * LIBPLZMA_NONNULL context);

/// @brief User defined context.
/// @note The deinitializer will be triggered at the end of the context's lifetime and
/// only if both \a context reference and \a deinitializer callback are provided, i.e. non-null.
typedef struct plzma_context {
    /// @brief Reference to any user defined context.
    /// This reference will be provided as an argument to \a deinitializer callback.
    void * LIBPLZMA_NULLABLE context;
    
    /// @brief Deinitializer callback which will be triggered with non-null \a context reference.
    plzma_context_deinitializer LIBPLZMA_NULLABLE deinitializer;
} plzma_context;


/// @brief Generic void pointer.
typedef void * plzma_ptr;


/// @brief The reference type to an exeption object.
///
/// @note To release the exception use \a plzma_exception_release function.
/// @note To release and nullify the \a exception reference of the object
/// use \a plzma_object_exception_release function.
typedef plzma_ptr plzma_exception_ptr;


/// @brief Generic object structure.
///
/// During the creation and execution there are following rulues:
/// @li 1. The created non-null object's \a object and \a exception references
/// must be released when the object is no longer needed.
/// All specific object has object specific \a <type>_release functions prefixed with spefic type, see example:
/// @code
///     // The path object has type 'plzma_path'.
///     plzma_path myPath = plzma_path_create_with_utf8_string("Hello world!");
///     // To release path object use <type>_release function. In this case '<plzma_path>_release'.
///     // Release & nullify 'myPath.object' and 'myPath.exception' if needed.
///     plzma_path_release(&myPath);
/// @endcode
///
/// @li 2. If object successfully created, then the \a object reference is not-null and
/// the \a exception is null.
///
/// @li 3. If during the creation an exception was thrown, then the \a object reference is null
/// and the \a exception contains a reference to the internal exception.
///
/// @li 4. If you provide the object with existed, non-null \a exception, then
/// the execution will not be performed.
/// In this case, the \a object and the \a exception references are unchanged
/// and optinaly returned \a falsy result, if such required for non-void functions.
///
/// @li 5. If you provide the object with existed, non-null \a exception for creating, instantiating
/// another object, then an empty object will be returned,
/// i.e. object with both null \a object and \a exception references.
/// This means that the result was not checked or processed. See warnings below.
///
/// @li 6. If during the creation of the another object from the provided one,
/// an exception was thrown, then the returned object will contain non-null \a exception reference
/// and null \a object reference.
///
/// @li 7. If during the execution, an exception was thrown, then the \a object reference is unchanged
/// and the \a exception contains a reference to an internal exception.
///
/// @warning During the execution or instantiating another object from the provided one,
/// the functionality checks existance only the \a exception reference
/// and \b not the \a object reference!
/// This means, if you provide the object with existed, non-null, unproceseed \a exception reference,
/// then the behavior is \b undefined or
/// you will get the \b null \b pointer \b exception, in case if \a object reference is null.
///
/// @warning It's recommended to check creation or execution result by
/// checking the \a exception reference of the \b provided or \b returned object.
///
/// @warning It's recommended to check creation of another object result by
/// checking the \a exception reference of the \b returned object.
///
/// @note To release and nullify only the \a exception reference of the object,
/// use \a plzma_object_exception_release function.
///
typedef struct plzma_object {
    /// @brief The reference to the actual object.
    plzma_ptr LIBPLZMA_NULLABLE object;
    
    /// @brief The reference to the last thrown exception during the execution or
    /// instantiating of the another object using this one.
    plzma_exception_ptr LIBPLZMA_NULLABLE exception;
} plzma_object;

typedef plzma_object plzma_path;
typedef plzma_object plzma_path_iterator;
typedef plzma_object plzma_item;
typedef plzma_object plzma_in_stream;
typedef plzma_object plzma_out_stream;
typedef plzma_object plzma_item_array;
typedef plzma_object plzma_item_out_stream_array;
typedef plzma_object plzma_decoder;
typedef plzma_object plzma_encoder;

typedef uint32_t plzma_size_t; // limited to 32 bit unsigned integer.
#define PLZMA_SIZE_T_MAX UINT32_MAX


/// @brief The struct represents the heap memory with size.
typedef struct plzma_memory {
    /// @brief The pointer to the allocated heap memory.
    /// @note The memory must be deallocated when no longer needed.
    void * LIBPLZMA_NULLABLE memory;
    
    /// @brief The size of the allocated heap \a memory.
    size_t size;
} plzma_memory;


/// @brief The struct represents pair of the item/out-stream array containing the item as a key and out-stream as a value.
/// @note The pair must be released via \a plzma_item_out_stream_array_pair_release function
///       or individualy via \a plzma_item_release and \a plzma_out_stream_release.
typedef struct plzma_item_out_stream_array_pair {
    /// @brief The archive item object.
    plzma_item item;
    
    /// @brief The out stream object.
    plzma_out_stream stream;
} plzma_item_out_stream_array_pair;


/// @brief The callback requires to free the provided heap memory pointer.
/// @param memory The pointer to a heap memory to free.
typedef void (*plzma_free_callback)(void * LIBPLZMA_NULLABLE memory);


/// @brief The callback requires to open in-stream.
/// @param context The user's context pointer provided with stream creation.
/// @return \a true if stream was successfully opened, otherwice \a false.
typedef bool (*plzma_in_stream_open_callback)(void * LIBPLZMA_NULLABLE context);


/// @brief The callback requires to close in-stream.
/// @param context The user's context pointer provided with stream creation.
typedef void (*plzma_in_stream_close_callback)(void * LIBPLZMA_NULLABLE context);


/// @brief The callback requires to set the position/offset of the in-stream.
///
/// This callback provides similar arguments to \a fseek C function in 64-bit env.
/// @param context The user's context pointer provided with stream creation.
/// @param offset The number of bytes to offset from origin.
/// @param seek_origin The position used as reference for the offset, i.e. \a SEEK_SET or \a SEEK_CUR or \a SEEK_END.
///                    The type is unsigned integer and provides same values as \a SEEK_SET, \a SEEK_CUR and \a SEEK_END.
/// @param new_position The pointer to provide the current position after applying the new offset.
/// @return \a true if stream's offset was successfully set, otherwice \a false.
/// @link http://www.cplusplus.com/reference/cstdio/fseek
typedef bool (*plzma_in_stream_seek_callback)(void * LIBPLZMA_NULLABLE context,
                                              int64_t offset,
                                              uint32_t seek_origin,
                                              uint64_t * LIBPLZMA_NONNULL new_position);

/// @brief The callback requires to read some data from the current offset of the in-stream
///        into provided \a data buffer of \a size size and provide the actual number of read bytes.
///        The position/offset of the in-stream must be increased for a number of successfully read bytes via a pointer.
///
/// The provided number of bytes to read might be bigger than available.
/// In this case, read and write the maximum available, but not bigger than provided.
/// @param context The user's context pointer provided with stream creation.
/// @param data The buffer to write the data.
/// @param size The maximum size of the the data to read and write to a \a data buffer.
/// @param processed_size The number of bytes actualy read from a stream and written to a buffer.
/// @return \a true if operation was successfully done, otherwice \a false.
typedef bool (*plzma_in_stream_read_callback)(void * LIBPLZMA_NULLABLE context,
                                              void * LIBPLZMA_NONNULL data,
                                              uint32_t size,
                                              uint32_t * LIBPLZMA_NONNULL processed_size);


/// @brief The callback provides current encoding/decoding progress. Similar to a \a plzma_progress_delegate_wide_callback callback.
/// @param context The user's provided context pointer.
/// @param utf8_path The UTF8 presentation of the item/archive path if such supported, or empty path string.
/// @param progress The total progress of the current operation in range [0.0; 1.0].
typedef void (*plzma_progress_delegate_utf8_callback)(void * LIBPLZMA_NULLABLE context,
                                                      const char * LIBPLZMA_NONNULL utf8_path,
                                                      const double progress);


/// @brief The callback provides current encoding/decoding progress. Similar to a \a plzma_progress_delegate_utf8_callback callback.
/// @param context The user's provided context pointer.
/// @param wide_path The wide character presentation of the item/archive path if such supported, or empty path string.
/// @param progress The total progress of the current operation in range [0.0; 1.0].
typedef void (*plzma_progress_delegate_wide_callback)(void * LIBPLZMA_NULLABLE context,
                                                      const wchar_t * LIBPLZMA_NONNULL wide_path,
                                                      const double progress);


/// @brief The full version string of the library generated on build time .
///
/// Contains version<major, minor, patch> with optional automatic build number,
/// library type, build date/time, os, compiler, environment, usage, features, etc. and original LZMA SDK version.
/// @return Non-null, full version C string.
LIBPLZMA_C_API(const char * LIBPLZMA_NONNULL) plzma_version(void);


/// @brief Function generates the random value in range [low; up].
/// @param low The low range value.
/// @param up The up range value.
/// @return The signed integer 32-bit random value.
LIBPLZMA_C_API(int32_t) plzma_random_in_range(const int32_t low, const int32_t up);

/// Memory

/// @return Returns the maximum value of \a size_t type casted to a 64-bit unsigned value.
/// Based on the preprocessor definitions.
LIBPLZMA_C_API(uint64_t) plzma_max_size(void);


/// @brief Allocates a heap memory.
/// @param size The required ammount of heap memory for allocation.
/// @return The memory pointer or null.
LIBPLZMA_C_API(void * LIBPLZMA_NULLABLE) plzma_malloc(size_t size);


/// @brief Allocates a heap memory and fills with zeros.
/// @param size The required ammount of heap memory for allocation.
/// @return The zero-filled memory pointer or null.
LIBPLZMA_C_API(void * LIBPLZMA_NULLABLE) plzma_malloc_zero(size_t size);


/// @brief Reallocates previously allocated the heap memory or allocates new.
/// @param mem The previosly allocated memory by the \a plzma_malloc, \a plzma_malloc_zero functions or NULL.
/// @param new_size The size for a new heap memory.
/// @return Reallocated memory pointer or null.
LIBPLZMA_C_API(void * LIBPLZMA_NULLABLE) plzma_realloc(void * LIBPLZMA_NULLABLE mem, size_t new_size);


/// @brief Freeing previosly allocated heap memory by the \a plzma_malloc, \a plzma_malloc_zero or \a plzma_realloc functions.
/// @param mem The memory to free or null.
LIBPLZMA_C_API(void) plzma_free(void * LIBPLZMA_NULLABLE mem);

/// String

/// @brief The constant for a zero length/empty C string.
LIBPLZMA_C_API(const char * LIBPLZMA_NONNULL) plzma_empty_cstring;


/// @brief The constant for a zero length/empty wide character string.
LIBPLZMA_C_API(const wchar_t * LIBPLZMA_NONNULL) plzma_empty_wstring;


/// @brief Makes a heap memory copy of the non-zero length C string.
/// @param str The c string for copy or null.
/// @return The heap memory string of null if source string length is zero
///         or can't allocate required ammount of the heap memory.
/// @note Use \a plzma_free function to free the returned string.
LIBPLZMA_C_API(char * LIBPLZMA_NULLABLE) plzma_cstring_copy(const char * LIBPLZMA_NULLABLE str);


/// @brief Appends heap memory source C string with another string.
/// @param source The source heap memory string or null.
/// @param str The string to append or null.
/// @return The reallocated \a source heap memory string or null if \a source string length is zero.
/// @note Use \a plzma_free function to free the returned string.
LIBPLZMA_C_API(char * LIBPLZMA_NULLABLE) plzma_cstring_append(char * LIBPLZMA_NULLABLE source, const char * LIBPLZMA_NULLABLE str);

/// Settings

/// @brief Receives the current size in bytes of the read block per single request.
///
/// @li \a CFilterCoder allocates buff for reading. Original value: 1 << 20.
/// @li \a ISequentialOutStream max read block size, or available size. Original value: 1 << 31.
LIBPLZMA_C_API(plzma_size_t) plzma_stream_read_size(void);


/// @brief Change the the current size in bytes of the read block per single request.
/// @see Function \a plzma_stream_read_size.
LIBPLZMA_C_API(void) plzma_set_stream_read_size(const plzma_size_t size);


/// @brief Receives the current size in bytes for write per single request.
///
/// @li \a ISequentialOutStream max write block size, or available size. Original value: 1 << 31.
LIBPLZMA_C_API(plzma_size_t) plzma_stream_write_size(void);


/// @brief Change the current size in bytes for write per single request.
/// @see Function \a plzma_stream_write_size.
LIBPLZMA_C_API(void) plzma_set_stream_write_size(const plzma_size_t size);


/// @brief The current size in bytes for internal decoder buffer for holding coded data.
///
/// @li Lzma \a CDecoder in buffer, buff with coded data. Original value: 1 << 20.
/// @li Lzma2 \a CDecoder in buffer, buff with coded data. Original value: 1 << 20.
LIBPLZMA_C_API(plzma_size_t) plzma_decoder_read_size(void);


/// @brief Change the current size in bytes for internal decoder buffer for holding coded data.
/// @see Function \a plzma_decoder_read_size.
LIBPLZMA_C_API(void) plzma_set_decoder_read_size(const plzma_size_t size);


/// @brief Receives the current size in bytes for internal decoder buffer for holding decoded data.
///
/// @li Lzma \a CDecoder out buffer, buff for decoded data. Original value: 1 << 22.
/// @li Lzma2 \a CDecoder out buffer, buff for decoded data. Original value: 1 << 22.
LIBPLZMA_C_API(plzma_size_t) plzma_decoder_write_size(void);


/// Change the current size in bytes for internal decoder buffer for holding decoded data.
/// @see Function \a plzma_decoder_write_size.
LIBPLZMA_C_API(void) plzma_set_decoder_write_size(const plzma_size_t size);

/// Object

/// @brief Releases optional \a exception of the generic object.
///
/// The \a exception pointer might be null.
/// @param object The generic object pointer with optional \a exception for release.
/// @note After calling this function, any \a exception references are invalid.
LIBPLZMA_C_API(void) plzma_object_exception_release(plzma_object * LIBPLZMA_NONNULL object);

/// Exception

/// @return The UTF-8 C string of the exception's \a what property.
LIBPLZMA_C_API(const char * LIBPLZMA_NULLABLE) plzma_exception_what_utf8_string(plzma_exception_ptr LIBPLZMA_NONNULL exception);


/// @return The UTF-8 C string of the exception's \a file name property where exception was thrown.
LIBPLZMA_C_API(const char * LIBPLZMA_NULLABLE) plzma_exception_file_utf8_string(plzma_exception_ptr LIBPLZMA_NONNULL exception);


/// @return The UTF-8 C string of the exception's possible \a reason property.
LIBPLZMA_C_API(const char * LIBPLZMA_NULLABLE) plzma_exception_reason_utf8_string(plzma_exception_ptr LIBPLZMA_NONNULL exception);


/// @return Custom error code.
LIBPLZMA_C_API(plzma_error_code) plzma_exception_code(plzma_exception_ptr LIBPLZMA_NONNULL exception);


/// @return The line number where exception was thrown.
LIBPLZMA_C_API(int) plzma_exception_line(plzma_exception_ptr LIBPLZMA_NONNULL exception);


/// @brief Releases the exception.
/// After calling this function, any references to this exception are invalid.
LIBPLZMA_C_API(void) plzma_exception_release(plzma_exception_ptr LIBPLZMA_NULLABLE exception);

/// Path

/// @brief Creates the new instance of the path object from a wide character string presentation.
/// During the creation, the path separators are converted/normalized to a platform specific ones.
/// @param path The wide character string presentation of the path.
/// @return Created path object following the 'generic object creation' rules.
/// @note Call \a plzma_path_release function to release the path object.
LIBPLZMA_C_API(plzma_path) plzma_path_create_with_wide_string(const wchar_t * LIBPLZMA_NULLABLE path);


/// @brief Creates the new instance of the path object from a UTF-8 C string presentation.
/// During the creation, the path separators are converted/normalized to a platform specific ones.
/// @param path The UTF-8 C string presentation of the path.
/// @return Created path object following the 'generic object creation' rules.
/// @note Call \a plzma_path_release function to release the path object.
LIBPLZMA_C_API(plzma_path) plzma_path_create_with_utf8_string(const char * LIBPLZMA_NULLABLE path);


/// @brief Creates the new instance of the path object with the platform specific temporary directory for the library.
/// The provided directory path, if such exists, has a read-write permissions.
/// @return The path object with existed temporary directory or with empty path.
/// @note Call \a plzma_path_release function to release the path object.
LIBPLZMA_C_API(plzma_path) plzma_path_create_with_tmp_dir(void);


/// @return The number of characters in path or zero if path object contains exception.
/// @note This number may vary from that which returns \a strlen function.
LIBPLZMA_C_API(plzma_size_t) plzma_path_count(const plzma_path * LIBPLZMA_NONNULL path);


/// @brief Set the new path from a wide character string presentation.
/// @param str The wide character string presentation of the path.
LIBPLZMA_C_API(void) plzma_path_set_wide_string(plzma_path * LIBPLZMA_NONNULL path, const wchar_t * LIBPLZMA_NULLABLE str);


/// @brief Set the new path from a wide character string presentation.
/// @param str The UTF-8 C string presentation of the path.
LIBPLZMA_C_API(void) plzma_path_set_utf8_string(plzma_path * LIBPLZMA_NONNULL path, const char * LIBPLZMA_NULLABLE str);


/// @brief Appends path component to path object.
/// @param component The wide character string presentation of the component to append.
LIBPLZMA_C_API(void) plzma_path_append_wide_component(plzma_path * LIBPLZMA_NONNULL path, const wchar_t * LIBPLZMA_NULLABLE component);


/// @brief Appends path component to path object.
/// @param component The UTF-8 C string presentation of the component to append.
LIBPLZMA_C_API(void) plzma_path_append_utf8_component(plzma_path * LIBPLZMA_NONNULL path, const char * LIBPLZMA_NULLABLE component);


/// @brief Appends random path component to the path object.
/// If path successfully updated, then the updated path doesn't exists in a root directory
/// or exception assigned to the path object.
/// The component consists of ASCII characters in range ['a'; 'z'].
LIBPLZMA_C_API(void) plzma_path_append_random_component(plzma_path * LIBPLZMA_NONNULL path);


/// @brief Provides the wide character string presentation of the path object.
/// @return The wide character string presentation of the path or null on any exception.
/// @note Do not cast & free the result. Just use in a current execution context/function.
/// @note The pointer to a string might be changed after any path modification.
LIBPLZMA_C_API(const wchar_t * LIBPLZMA_NULLABLE) plzma_path_wide_string(plzma_path * LIBPLZMA_NONNULL path);


/// @brief Provides the UTF-8 C string presentation of the path object.
/// @return The wide character string presentation of the path or null on any exception.
/// @note Do not cast & free the result. Just use in a current execution context/function.
/// @note The pointer to a string might be changed after any path modification.
LIBPLZMA_C_API(const char * LIBPLZMA_NULLABLE) plzma_path_utf8_string(plzma_path * LIBPLZMA_NONNULL path);


/// @brief Checks the path exists with optional checking for a directory.
/// @param isDir Optional pointer to a boolean variable to store the result of checking for a directory.
/// @return \a true if path exists, otherwise \a false.
LIBPLZMA_C_API(bool) plzma_path_exists(plzma_path * LIBPLZMA_NONNULL path, bool * LIBPLZMA_NULLABLE isDir);


/// @brief Checks the path exists and has read permissions.
/// @return \a true if path exists and readable, otherwise \a false.
LIBPLZMA_C_API(bool) plzma_path_readable(plzma_path * LIBPLZMA_NONNULL path);


/// @brief Checks the path exists and has write permissions.
/// @return \a true if path exists and writable, otherwise \a false.
LIBPLZMA_C_API(bool) plzma_path_writable(plzma_path * LIBPLZMA_NONNULL path);


/// @brief Checks the path exists and has read-write permissions.
/// @return \a true if path exists, readable and writable, otherwise \a false.
LIBPLZMA_C_API(bool) plzma_path_readable_and_writable(plzma_path * LIBPLZMA_NONNULL path);


/// @brief Provides the stat info of the path.
/// @return The stat info of the path or empty/zero-filled struct if operation was failed.
LIBPLZMA_C_API(plzma_path_stat) plzma_path_get_stat(plzma_path * LIBPLZMA_NONNULL path);


/// @brief Clears the path object's string presentation.
/// @param erase_type The type of erasing of the path data.
LIBPLZMA_C_API(void) plzma_path_clear(plzma_path * LIBPLZMA_NONNULL path, const plzma_erase erase_type);


/// @brief Physically removes the directory with all content or file associated with the path.
/// @param skip_errors Skips errors and continuing removing or stop on first error.
/// @return The result of removing directory or file.
LIBPLZMA_C_API(bool) plzma_path_remove(plzma_path * LIBPLZMA_NONNULL path, const bool skip_errors);


/// @brief Receives the last component of the path.
/// @return The new instance of the path object representing the last component.
LIBPLZMA_C_API(plzma_path) plzma_path_last_component(plzma_path * LIBPLZMA_NONNULL path);


/// @brief Removes the last component from the path.
LIBPLZMA_C_API(void) plzma_path_remove_last_component(plzma_path * LIBPLZMA_NONNULL path);


/// @brief Creates the directory at specific path.
/// @param with_intermediates Create intermediate directories for each component or not.
/// @return The creation result of the directory.
LIBPLZMA_C_API(bool) plzma_path_create_dir(plzma_path * LIBPLZMA_NONNULL path, const bool with_intermediates);


/// @brief Opens a file associated with path.
/// @param mode The open file mode string. For Windows, it's possible to provide encoded character set to use(one of UTF-8, UTF-16LE, or UNICODE).
/// @return The file reference or null.
/// @see \a fopen C function.
LIBPLZMA_C_API(LIBPLZMA_WARN_UNUSED_RESULT FILE * LIBPLZMA_NULLABLE) plzma_path_open_file(plzma_path * LIBPLZMA_NONNULL path, const char * LIBPLZMA_NONNULL mode);


/// @brief Opens a directory associated with path for iterating the content.
/// @param mode The open directory mode.
/// @return New instance of the path iterator or null, if exception was thrown.
LIBPLZMA_C_API(plzma_path_iterator) plzma_path_open_dir(plzma_path * LIBPLZMA_NONNULL path, const plzma_open_dir_mode_t mode);


/// @brief Releases the path object.
/// @note All references are invalidated.
LIBPLZMA_C_API(void) plzma_path_release(plzma_path * LIBPLZMA_NULLABLE path);

/// Path Iterator

/// @brief Recevies the current file or directory component.
/// @return The new instance of the path object.
LIBPLZMA_C_API(plzma_path) plzma_path_iterator_component(const plzma_path_iterator * LIBPLZMA_NONNULL iterator);


/// @brief Recevies the current file or directory path.
/// @return The new instance of the path object.
LIBPLZMA_C_API(plzma_path) plzma_path_iterator_path(const plzma_path_iterator * LIBPLZMA_NONNULL iterator);


/// @brief Recevies the current file or directory full path, prefixed with root path.
/// @return The new instance of the path object.
LIBPLZMA_C_API(plzma_path) plzma_path_iterator_full_path(const plzma_path_iterator * LIBPLZMA_NONNULL iterator);


/// @brief Checks the current iterator's path is directory.
/// @return \a true the iterator's path is directory.
LIBPLZMA_C_API(bool) plzma_path_iterator_is_dir(const plzma_path_iterator * LIBPLZMA_NONNULL iterator);


/// @brief Continue iteration.
/// @return \a true The next file or directory located, otherwise iteration is finished.
LIBPLZMA_C_API(bool) plzma_path_iterator_next(plzma_path_iterator * LIBPLZMA_NONNULL iterator);


/// @brief Closes iteration.
LIBPLZMA_C_API(void) plzma_path_iterator_close(plzma_path_iterator * LIBPLZMA_NONNULL iterator);


/// @brief Releases the iterator object.
LIBPLZMA_C_API(void) plzma_path_iterator_release(plzma_path_iterator * LIBPLZMA_NULLABLE iterator);

/// Item

/// @brief Creates archive item object with path.
/// @param path The associated item's path.
/// @param index The index of the item in the archive.
/// @return The item object or null, if exception was thrown.
/// @note Call \a plzma_item_release function to release the item object.
/// @note The item is ARC object.
LIBPLZMA_C_API(plzma_item) plzma_item_create_with_path(const plzma_path * LIBPLZMA_NONNULL path, const plzma_size_t index);


/// @brief Creates archive item object with movable path.
/// @param path The associated item's path. After the successfull creation of the item, the path is empty.
/// @param index The index of the item in the archive.
/// @return The item object or null, if exception was thrown.
/// @note Call \a plzma_item_release function to release the item object.
/// @note The item is ARC object.
LIBPLZMA_C_API(plzma_item) plzma_item_create_with_pathm(plzma_path * LIBPLZMA_NONNULL path, const plzma_size_t index);


/// @brief Receives the item's path object.
/// @return The new instance of the path object.
/// @note Use \a plzma_path_release to release path object.
LIBPLZMA_C_API(plzma_path) plzma_item_path(const plzma_item * LIBPLZMA_NONNULL item);


/// @return Receives the item's index inside the archive.
LIBPLZMA_C_API(plzma_size_t) plzma_item_index(const plzma_item * LIBPLZMA_NONNULL item);


/// @return Receives the size in bytes of the item.
LIBPLZMA_C_API(uint64_t) plzma_item_size(const plzma_item * LIBPLZMA_NONNULL item);


/// @return Receives the packed size in bytes of the item.
LIBPLZMA_C_API(uint64_t) plzma_item_pack_size(const plzma_item * LIBPLZMA_NONNULL item);


/// @return Receives the CRC-32 checksum of the items content.
LIBPLZMA_C_API(uint32_t) plzma_item_crc32(const plzma_item * LIBPLZMA_NONNULL item);


/// @return The creation time of the item. Unix timestamp.
LIBPLZMA_C_API(time_t) plzma_item_creation_time(const plzma_item * LIBPLZMA_NONNULL item);


/// @return The last access time of the item. Unix timestamp.
LIBPLZMA_C_API(time_t) plzma_item_access_time(const plzma_item * LIBPLZMA_NONNULL item);


/// @return The last modification time of the item. Unix timestamp.
LIBPLZMA_C_API(time_t) plzma_item_modification_time(const plzma_item * LIBPLZMA_NONNULL item);


/// @return Checks the item is encrypted.
LIBPLZMA_C_API(bool) plzma_item_encrypted(const plzma_item * LIBPLZMA_NONNULL item);


/// @return Checks the item is directory or file.
LIBPLZMA_C_API(bool) plzma_item_is_dir(const plzma_item * LIBPLZMA_NONNULL item);


/// @brief Updates the size of the item.
/// @param size The size in bytes.
LIBPLZMA_C_API(void) plzma_item_set_size(plzma_item * LIBPLZMA_NONNULL item, const uint64_t size);


/// @brief Updates the packed size of the item.
/// @param size The size in bytes.
LIBPLZMA_C_API(void) plzma_item_set_pack_size(plzma_item * LIBPLZMA_NONNULL item, const uint64_t size);


/// @brief Updates the CRC-32 checksum of the item.
/// @param crc The CRC-32 checksum value.
LIBPLZMA_C_API(void) plzma_item_set_crc32(plzma_item * LIBPLZMA_NONNULL item, const uint32_t crc);


/// @brief Updates creation time of the item.
/// @param time The unix timestamp.
LIBPLZMA_C_API(void) plzma_item_set_creation_time(plzma_item * LIBPLZMA_NONNULL item, const time_t time);


/// @brief Updates last access time of the item.
/// @param time The unix timestamp.
LIBPLZMA_C_API(void) plzma_item_set_access_time(plzma_item * LIBPLZMA_NONNULL item, const time_t time);


/// @brief Updates modification time of the item.
/// @param time The unix timestamp.
LIBPLZMA_C_API(void) plzma_item_set_modification_time(plzma_item * LIBPLZMA_NONNULL item, const time_t time);


/// @brief Marks the item is encrypted.
LIBPLZMA_C_API(void) plzma_item_set_encrypted(plzma_item * LIBPLZMA_NONNULL item, const bool is_encrypted);


/// @brief Marks the item is directory.
LIBPLZMA_C_API(void) plzma_item_set_is_dir(plzma_item * LIBPLZMA_NONNULL item, const bool is_dir);


/// @brief Releases the item object.
LIBPLZMA_C_API(void) plzma_item_release(plzma_item * LIBPLZMA_NONNULL item);

/// In stream

/// @brief Creates the input file stream object with path.
/// @param path The non-empty input file path.
/// @return The input file stream or null, if exception was thrown.
/// @note Call \a plzma_in_stream_release function to release the input file stream.
/// @note The stream is ARC object.
LIBPLZMA_C_API(plzma_in_stream) plzma_in_stream_create_with_path(plzma_path * LIBPLZMA_NONNULL path);


/// @brief Creates the input file stream object with movable path.
/// @param path The non-empty input file path. After the successfull creation of the stream, the path is empty.
/// @return The input file stream or null, if exception was thrown.
/// @note Call \a plzma_in_stream_release function to release the input file stream.
/// @note The stream is ARC object.
LIBPLZMA_C_API(plzma_in_stream) plzma_in_stream_create_with_pathm(plzma_path * LIBPLZMA_NONNULL path);


/// @brief Creates the input file stream object with the file memory content.
/// During the creation, the memory will copyed.
/// @param memory The file memory content.
/// @param size The memory size in bytes.
/// @return The input file stream or null, if exception was thrown.
/// @note Call \a plzma_in_stream_release function to release the input file stream.
/// @note The stream is ARC object.
LIBPLZMA_C_API(plzma_in_stream) plzma_in_stream_create_with_memory_copy(const void * LIBPLZMA_NONNULL memory,
                                                                        const size_t size);


/// @brief Creates the input file stream object with the file memory content.
/// During the creation, the memory will not be copyed.
/// @param memory The file memory content.
/// @param size The memory size in bytes.
/// @param free_callback The callback, which will be triggered with provided \a memory pointer at the end of stream's lifetime.
/// @return The input file stream object or null, if exception was thrown.
/// @note Call \a plzma_in_stream_release function to release the input file stream.
/// @note The stream is ARC object.
LIBPLZMA_C_API(plzma_in_stream) plzma_in_stream_create_with_memory(void * LIBPLZMA_NONNULL memory,
                                                                   const size_t size,
                                                                   plzma_free_callback LIBPLZMA_NONNULL free_callback);


/// @brief Creates the input file stream with user defined callbacks.
/// @param open_callback Opens the file stream for reading. Similar to \a fopen C function.
/// @param close_callback Closes the file stream. Similar to \a fclose C function.
/// @param seek_callback Sets the read offset of the stream. Similar to \a fseek C function.
/// @param read_callback Reads the number of bytes into provided byffer. Similar to \a fread C function.
/// @param context The user defined context provided to all callbacks.
/// @return The input file stream object or null, if exception was thrown.
/// @note Call \a plzma_in_stream_release function to release the input file stream.
/// @note The stream is ARC object.
LIBPLZMA_C_API(plzma_in_stream) plzma_in_stream_create_with_callbacks(plzma_in_stream_open_callback LIBPLZMA_NONNULL open_callback,
                                                                      plzma_in_stream_close_callback LIBPLZMA_NONNULL close_callback,
                                                                      plzma_in_stream_seek_callback LIBPLZMA_NONNULL seek_callback,
                                                                      plzma_in_stream_read_callback LIBPLZMA_NONNULL read_callback,
                                                                      const plzma_context context);


/// @return Checks the input file stream is opened.
/// @note Thread-safe.
LIBPLZMA_C_API(bool) plzma_in_stream_opened(plzma_in_stream * LIBPLZMA_NONNULL stream);


/// @brief Erases and removes the content of the stream.
/// @param erase_type The type of erasing the content.
/// @return The erasing result.
/// @note Thread-safe.
LIBPLZMA_C_API(bool) plzma_in_stream_erase(plzma_in_stream * LIBPLZMA_NONNULL stream, const plzma_erase erase_type);


/// @brief Releases the input file stream object.
LIBPLZMA_C_API(void) plzma_in_stream_release(plzma_in_stream * LIBPLZMA_NONNULL stream);

/// Out stream

/// @brief Creates the output file stream object with path.
/// @param path The non-empty output file path.
/// @return The output file stream or null, if exception was thrown.
/// @note Call \a plzma_out_stream_release function to release the output file stream.
/// @note The stream is ARC object.
LIBPLZMA_C_API(plzma_out_stream) plzma_out_stream_create_with_path(const plzma_path * LIBPLZMA_NONNULL path);


/// @brief Creates the output file stream object with movable path.
/// @param path The non-empty output file path. After the successfull creation of the stream, the path is empty.
/// @return The output file stream or null, if exception was thrown.
/// @note Call \a plzma_out_stream_release function to release the output file stream.
/// @note The stream is ARC object.
LIBPLZMA_C_API(plzma_out_stream) plzma_out_stream_create_with_pathm(plzma_path * LIBPLZMA_NONNULL path);


/// @brief Creates the output file stream object for writing to memory.
/// @return The output file stream or null, if exception was thrown.
/// @note Call \a plzma_out_stream_release function to release the output file stream.
/// @note The stream is ARC object.
LIBPLZMA_C_API(plzma_out_stream) plzma_out_stream_create_memory_stream(void);


/// @return Checks the output file stream is opened.
/// @note Thread-safe.
LIBPLZMA_C_API(bool) plzma_out_stream_opened(plzma_out_stream * LIBPLZMA_NULLABLE stream);


/// @brief Erases and removes the content of the stream.
/// @param erase_type The type of erasing the content.
/// @return The erasing result.
/// @note Thread-safe.
LIBPLZMA_C_API(bool) plzma_out_stream_erase(plzma_out_stream * LIBPLZMA_NONNULL stream, const plzma_erase erase_type);


/// @brief Copies the content of the stream to a heap memory.
///
/// The stream must be closed. Use \a plzma_out_stream_opened to ckeck.
/// @return The newly allocated heap memory with the stream's content. In case if stream is opened
/// or exception was thrown, the \a memory & \a size are null/0.
/// @note Use \a plzma_free to release the heap memory.
/// @note Thread-safe.
LIBPLZMA_C_API(plzma_memory) plzma_out_stream_copy_content(plzma_out_stream * LIBPLZMA_NONNULL stream);


/// @brief Releases the output file stream object.
LIBPLZMA_C_API(void) plzma_out_stream_release(plzma_out_stream * LIBPLZMA_NONNULL stream);

/// Item array

/// @brief Creates the array of items with optional capacity for a container.
///
/// All items inside the array are retained.
/// @code
///   plzma_item_array myArray = plzma_item_array_create(1); // capacity: 1, number of items: 0
///   plzma_item myItem = plzma_item_create_...(); // new retained item
///   plzma_item_array_add(&myArray, &myItem); // retains item
///   plzma_item_release(&myItem); // release item after adding, the array is now exclusive owner of the item
///   plzma_item_array_release(&myArray); // releases array & all items
/// @endcode
/// @param capacity The optinal initial capacity of the array. Not a number of items.
/// @return The array object or null.
/// @note Use \a plzma_item_array_release to release an array.
/// @note The array is ARC object.
LIBPLZMA_C_API(plzma_item_array) plzma_item_array_create(const plzma_size_t capacity);


/// @brief Adds the retained item to the array.
/// @param item The item to add.
/// @note After adding, the item is retained.
LIBPLZMA_C_API(void) plzma_item_array_add(plzma_item_array * LIBPLZMA_NONNULL array,
                                          plzma_item * LIBPLZMA_NONNULL item);


/// @return The number of items inside the array.
LIBPLZMA_C_API(plzma_size_t) plzma_item_array_count(const plzma_item_array * LIBPLZMA_NONNULL array);


/// @brief Receives the retained item at index.
/// @param index The index of the item. Must be less than the number of items, i.e. the value of the \a plzma_item_array_count function.
/// @return The retained item at index.
/// @note Use \a plzma_item_release to release the item when no longer needed.
LIBPLZMA_C_API(plzma_item) plzma_item_array_at(plzma_item_array * LIBPLZMA_NONNULL array,
                                                   const plzma_size_t index);


/// @brief Sorts the array's items by the item's index.
LIBPLZMA_C_API(void) plzma_item_array_sort(plzma_item_array * LIBPLZMA_NONNULL array);


/// @brief Releases the array object and all items inside.
LIBPLZMA_C_API(void) plzma_item_array_release(plzma_item_array * LIBPLZMA_NONNULL array);

/// Item out stream array

/// @brief Creates the array of item/out-stream pairs with optional capacity for a container.
///
/// All item/out-stream inside the array are retained.
/// @param capacity The optinal initial capacity of the array. Not a number of items.
/// @return The array object or null.
/// @note Use \a plzma_item_out_stream_array_release to release an array.
/// @note The array is ARC object.
LIBPLZMA_C_API(plzma_item_out_stream_array) plzma_item_out_stream_array_create(const plzma_size_t capacity);


/// @brief Adds the retained item/out-stream pair to the array.
/// @param item The item to add. Similar to a map key.
/// @param stream The out-stream to add. Similar to a map value for a key.
/// @note After adding, the item and out-scream are retained.
LIBPLZMA_C_API(void) plzma_item_out_stream_array_add(plzma_item_out_stream_array * LIBPLZMA_NONNULL map,
                                                     plzma_item * LIBPLZMA_NONNULL item,
                                                     plzma_out_stream * LIBPLZMA_NONNULL stream);


/// @return The number of item/out-stream pairs inside the array.
LIBPLZMA_C_API(plzma_size_t) plzma_item_out_stream_array_count(const plzma_item_out_stream_array * LIBPLZMA_NONNULL map);


/// @brief Receives the retained item/out-stream pair at index.
/// @param index The index of the item/out-stream pair. Must be less than the number of items, i.e. the value of the \a plzma_item_out_stream_array_count function.
/// @return The retained item/out-stream pair at index.
/// @note Use \a plzma_item_out_stream_array_pair_release to release the item and out-stream when no longer needed.
///       Or use \a plzma_item_release for an item and \a plzma_out_stream_release for an out-stream.
LIBPLZMA_C_API(plzma_item_out_stream_array_pair) plzma_item_out_stream_array_pair_at(plzma_item_out_stream_array * LIBPLZMA_NONNULL map,
                                                                                         const plzma_size_t index);


/// @brief Sorts the array's item/out-stream pairs by the item's index.
LIBPLZMA_C_API(void) plzma_item_out_stream_array_sort(plzma_item_out_stream_array * LIBPLZMA_NONNULL map);


/// @brief Releases the item and out-stream of the pair.
LIBPLZMA_C_API(void) plzma_item_out_stream_array_pair_release(plzma_item_out_stream_array_pair * LIBPLZMA_NONNULL pair);


/// @brief Releases the array object and all item/out-stream pairs inside.
LIBPLZMA_C_API(void) plzma_item_out_stream_array_release(plzma_item_out_stream_array * LIBPLZMA_NONNULL map);

/// Decoder

/// @brief Creates the decoder for extracting or testing the archive items.
///
/// @param stream The input stream which contains the archive file content.
///               After the successful decoder creation, the input file stream is retained and might be released.
/// @param type The type of the arhive file content.
/// @param context The user provided context.
/// @note Use \a plzma_decoder_release to release the decoder.
LIBPLZMA_C_API(plzma_decoder) plzma_decoder_create(plzma_in_stream * LIBPLZMA_NONNULL stream,
                                                   const plzma_file_type type,
                                                   const plzma_context context);

/// @brief Provides the extract or test progress delegate callback.
/// @param callback The callback which accepts UTF-8 item path presentation.
/// @note Thread-safe.
LIBPLZMA_C_API(void) plzma_decoder_set_progress_delegate_utf8_callback(plzma_decoder * LIBPLZMA_NONNULL decoder, plzma_progress_delegate_utf8_callback LIBPLZMA_NULLABLE callback);


/// @brief Provides the extract or test progress delegate callback.
/// @param callback The callback which accepts wide character item path presentation.
/// @note Thread-safe.
LIBPLZMA_C_API(void) plzma_decoder_set_progress_delegate_wide_callback(plzma_decoder * LIBPLZMA_NONNULL decoder, plzma_progress_delegate_wide_callback LIBPLZMA_NULLABLE callback);


/// @brief Provides the archive password for opening, extracting or testing items.
/// @param password The password wide character presentation.
/// @note Thread-safe.
LIBPLZMA_C_API(void) plzma_decoder_set_password_wide_string(plzma_decoder * LIBPLZMA_NONNULL decoder, const wchar_t * LIBPLZMA_NULLABLE password);


/// @brief Provides the archive password for opening, extracting or testing items.
/// @param password The password UTF-8 character presentation.
/// @note Thread-safe.
LIBPLZMA_C_API(void) plzma_decoder_set_password_utf8_string(plzma_decoder * LIBPLZMA_NONNULL decoder, const char * LIBPLZMA_NULLABLE password);


/// @brief Opens the archive.
///
/// During the process, the decoder is self-retained till operation is in progress.
/// @return \a true the archive was successfully opened, otherwice \a false.
/// @note The opening progress might be executed in a separate thread.
/// @note The opening progress might be aborted via \a plzma_decoder_abort function.
/// @note Thread-safe.
LIBPLZMA_C_API(bool) plzma_decoder_open(plzma_decoder * LIBPLZMA_NONNULL decoder);


/// @brief Aborts opening, extracting or testing process.
/// @note The aborted decoder is no longer valid.
/// @note Thread-safe.
LIBPLZMA_C_API(void) plzma_decoder_abort(plzma_decoder * LIBPLZMA_NONNULL decoder);


/// @return Receives the number of items in archive.
/// @note The decoder must be opened.
/// @note Thread-safe.
LIBPLZMA_C_API(plzma_size_t) plzma_decoder_count(plzma_decoder * LIBPLZMA_NONNULL decoder);


/// @brief Receives all archive items.
/// @return Retained array with items.
/// @note Use \a plzma_item_array_release to release the array when it's no longer needed.
/// @note The decoder must be opened.
/// @note Thread-safe.
LIBPLZMA_C_API(plzma_item_array) plzma_decoder_items(plzma_decoder * LIBPLZMA_NONNULL decoder);


/// @brief Receives single, retained archive item at a specific index.
/// @param index The index of the item inside the arhive. Must be less then the number of items reported by the \a plzma_decoder_count fundtion.
/// @return Retained item.
/// @note Use \a plzma_item_release to release the item when it's no longer needed.
/// @note The decoder must be opened.
/// @note Thread-safe.
LIBPLZMA_C_API(plzma_item) plzma_decoder_item_at(plzma_decoder * LIBPLZMA_NONNULL decoder, const plzma_size_t index);


/// @brief Extracts all archive items to a specific path.
///
/// During the process, the decoder is self-retained till operation is in progress.
/// @param path The directory path to extract all items.
/// @param items_full_path Exctract item using it's full path or only last path component.
/// @note The extracting progress might be executed in a separate thread.
/// @note The extracting progress might be aborted via \a plzma_decoder_abort function.
/// @note Thread-safe.
LIBPLZMA_C_API(bool) plzma_decoder_extract_all_items_to_path(plzma_decoder * LIBPLZMA_NONNULL decoder,
                                                             const plzma_path * LIBPLZMA_NONNULL path,
                                                             const bool items_full_path);


/// @brief Extracts some archive items to a specific path.
///
/// During the process, the decoder is self-retained till operation is in progress.
/// @param items The array of items to extract.
/// @param path The directory path to extract all items.
/// @param items_full_path Exctract item using it's full path or only the last path component.
/// @note The extracting progress might be executed in a separate thread.
/// @note The extracting progress might be aborted via \a plzma_decoder_abort function.
/// @note Thread-safe.
LIBPLZMA_C_API(bool) plzma_decoder_extract_items_to_path(plzma_decoder * LIBPLZMA_NONNULL decoder,
                                                         plzma_item_array * LIBPLZMA_NONNULL items,
                                                         const plzma_path * LIBPLZMA_NONNULL path,
                                                         const bool items_full_path);


/// @brief Extracts each archive item to a separate out-stream.
///
/// During the process, the decoder is self-retained till operation is in progress.
/// @param items The array with item/out-stream pairs.
/// @note The extracting progress might be executed in a separate thread.
/// @note The extracting progress might be aborted via \a plzma_decoder_abort function.
/// @note Thread-safe.
LIBPLZMA_C_API(bool) plzma_decoder_extract_item_out_stream_array(plzma_decoder * LIBPLZMA_NONNULL decoder,
                                                                 plzma_item_out_stream_array * LIBPLZMA_NONNULL items);


/// @brief Tests specific archive items.
///
/// During the process, the decoder is self-retained till operation is in progress.
/// @param items The array with items to test.
/// @note The testing progress might be executed in a separate thread.
/// @note The testing progress might be aborted via \a plzma_decoder_abort function.
/// @note Thread-safe.
LIBPLZMA_C_API(bool) plzma_decoder_test_items(plzma_decoder * LIBPLZMA_NONNULL decoder,
                                              plzma_item_array * LIBPLZMA_NONNULL items);


/// @brief Tests all archive items.
///
/// During the process, the decoder is self-retained till operation is in progress.
/// @note The testing progress might be executed in a separate thread.
/// @note The testing progress might be aborted via \a plzma_decoder_abort function.
/// @note Thread-safe.
LIBPLZMA_C_API(bool) plzma_decoder_test(plzma_decoder * LIBPLZMA_NONNULL decoder);


/// @brief Relases the decoder object.
LIBPLZMA_C_API(void) plzma_decoder_release(plzma_decoder * LIBPLZMA_NONNULL decoder);

/// Encoder

/// @brief Creates the encoder.
/// @param stream The output file stream to write the archive's file content.
/// @param type The type of the archive.
/// @param method The compresion method.
/// @param context The user provided context to inform the progress of the operation.
/// @return The encoder object or null in case if exception was thrown.
LIBPLZMA_C_API(plzma_encoder) plzma_encoder_create(plzma_out_stream * LIBPLZMA_NONNULL stream,
                                                   const plzma_file_type type,
                                                   const plzma_method method,
                                                   const plzma_context context);


/// @brief Provides the compression progress delegate callback.
/// @param callback The callback which accepts UTF-8 item path presentation.
/// @note Thread-safe.
LIBPLZMA_C_API(void) plzma_encoder_set_progress_delegate_utf8_callback(plzma_encoder * LIBPLZMA_NONNULL encoder, plzma_progress_delegate_utf8_callback LIBPLZMA_NULLABLE callback);


/// @brief Provides the compression progress delegate callback.
/// @param callback The callback which accepts wide character item path presentation.
/// @note Thread-safe.
LIBPLZMA_C_API(void) plzma_encoder_set_progress_delegate_wide_callback(plzma_encoder * LIBPLZMA_NONNULL encoder, plzma_progress_delegate_wide_callback LIBPLZMA_NULLABLE callback);


/// @brief Provides the password for archive.
/// @param password The password wide character presentation.
/// @note Thread-safe. Must be set before opening.
LIBPLZMA_C_API(void) plzma_encoder_set_password_wide_string(plzma_encoder * LIBPLZMA_NONNULL encoder, const wchar_t * LIBPLZMA_NULLABLE password);


/// @brief Provides the password for archive.
/// @param password The password UTF-8 character presentation.
/// @note Thread-safe. Must be set before opening.
LIBPLZMA_C_API(void) plzma_encoder_set_password_utf8_string(plzma_encoder * LIBPLZMA_NONNULL encoder, const char * LIBPLZMA_NULLABLE password);


/// @brief Getter for a 'solid' archive property.
/// @note Enabled by default, the value is \a true.
/// @note Thread-safe.
LIBPLZMA_C_API(bool) plzma_encoder_should_create_solid_archive(plzma_encoder * LIBPLZMA_NONNULL encoder);


/// @brief Setter for a 'solid' archive property.
/// @note Thread-safe. Must be set before opening.
LIBPLZMA_C_API(void) plzma_encoder_set_should_create_solid_archive(plzma_encoder * LIBPLZMA_NONNULL encoder, const bool solid);


/// @brief Getter for a compression level.
/// @return The level in a range [0; 9].
/// @note Thread-safe.
LIBPLZMA_C_API(uint8_t) plzma_encoder_compression_level(plzma_encoder * LIBPLZMA_NONNULL encoder);


/// @brief Setter for an archive compression level.
/// @param level The level in a range [0; 9].
/// @note Thread-safe. Must be set before opening.
LIBPLZMA_C_API(void) plzma_encoder_set_compression_level(plzma_encoder * LIBPLZMA_NONNULL encoder, const uint8_t level);


/// @brief Should encoder compress the archive header.
/// @note Enabled by default, the value is \a true.
/// @note Thread-safe.
LIBPLZMA_C_API(bool) plzma_encoder_should_compress_header(plzma_encoder * LIBPLZMA_NONNULL encoder);


/// @brief Set encoder will compress the archive header.
/// @note Thread-safe. Must be set before opening.
LIBPLZMA_C_API(void) plzma_encoder_set_should_compress_header(plzma_encoder * LIBPLZMA_NONNULL encoder, const bool compress);


/// @brief Should encoder fully compress the archive header.
/// @note Enabled by default, the value is \a true.
/// @note Thread-safe.
LIBPLZMA_C_API(bool) plzma_encoder_should_compress_header_full(plzma_encoder * LIBPLZMA_NONNULL encoder);


/// @brief Set encoder will fully compress the archive header.
/// @note Thread-safe. Must be set before opening.
LIBPLZMA_C_API(void) plzma_encoder_set_should_compress_header_full(plzma_encoder * LIBPLZMA_NONNULL encoder, const bool compress);


/// @brief Should encoder encrypt the content of the archive items.
/// @note The password will be required to decode/extract archive items.
/// @note Thread-safe.
LIBPLZMA_C_API(bool) plzma_encoder_should_encrypt_content(plzma_encoder * LIBPLZMA_NONNULL encoder);


/// @brief Set encoder will encrypt the content of the archive items.
/// @note The password will be required to decode/extract archive items.
/// @note Thread-safe. Must be set before opening.
LIBPLZMA_C_API(void) plzma_encoder_set_should_encrypt_content(plzma_encoder * LIBPLZMA_NONNULL encoder, const bool encrypt);


/// @brief Should encoder encrypt the header with the list of archive items.
/// @note The password will be required to open archive and list the items.
/// @note Thread-safe.
LIBPLZMA_C_API(bool) plzma_encoder_should_encrypt_header(plzma_encoder * LIBPLZMA_NONNULL encoder);


/// @brief Set encoder will encrypt the header with the list of archive items.
/// @note The password will be required to open archive and list the items.
/// @note Thread-safe. Must be set before opening.
LIBPLZMA_C_API(void) plzma_encoder_set_should_encrypt_header(plzma_encoder * LIBPLZMA_NONNULL encoder, const bool encrypt);


/// @brief Should encoder store the creation time of each item to the header if such available.
/// @note Enabled by default, the value is \a true.
/// @note Thread-safe.
LIBPLZMA_C_API(bool) plzma_encoder_should_store_creation_time(plzma_encoder * LIBPLZMA_NONNULL encoder);


/// @brief Set encoder will store the creation time of each item to the header if such available.
/// @note Thread-safe. Must be set before opening.
LIBPLZMA_C_API(void) plzma_encoder_set_should_store_creation_time(plzma_encoder * LIBPLZMA_NONNULL encoder, const bool store);


/// @brief Should encoder store the access time of each item to the header if such available.
/// @note Enabled by default, the value is \a true.
/// @note Thread-safe.
LIBPLZMA_C_API(bool) plzma_encoder_should_store_access_time(plzma_encoder * LIBPLZMA_NONNULL encoder);


/// @brief Set encoder will store the access time of each item to the header if such available.
/// @note Thread-safe. Must be set before opening.
LIBPLZMA_C_API(void) plzma_encoder_set_should_store_access_time(plzma_encoder * LIBPLZMA_NONNULL encoder, const bool store);


/// @brief Should encoder store the last modification time of each item to the header if such available.
/// @note Enabled by default, the value is \a true.
/// @note Thread-safe.
LIBPLZMA_C_API(bool) plzma_encoder_should_store_modification_time(plzma_encoder * LIBPLZMA_NONNULL encoder);


/// @brief Set encoder will store the last modification time of each item to the header if such available.
/// @note Thread-safe. Must be set before opening.
LIBPLZMA_C_API(void) plzma_encoder_set_should_store_modification_time(plzma_encoder * LIBPLZMA_NONNULL encoder, const bool store);


/// @brief Adds the physical file or directory path to the encoder.
/// @param path The file or directory path.
/// @param open_dir_mode The mode for opening directory in case if \a path is a directory path.
/// @param archive_path The optional path of how the \a path will be presented in archive.
/// @note Thread-safe. Must be set before opening.
LIBPLZMA_C_API(void) plzma_encoder_add_path(plzma_encoder * LIBPLZMA_NONNULL encoder,
                                            const plzma_path * LIBPLZMA_NONNULL path,
                                            const plzma_open_dir_mode_t open_dir_mode,
                                            const plzma_path * LIBPLZMA_NULLABLE archive_path);


/// @brief Adds the file in-sctream to the encoder.
/// @param stream The input file stream to add.
/// @param archive_path The optional path of how the \a path will be presented in archive.
/// @note Thread-safe. Must be set before opening.
LIBPLZMA_C_API(void) plzma_encoder_add_stream(plzma_encoder * LIBPLZMA_NONNULL encoder,
                                              const plzma_in_stream * LIBPLZMA_NONNULL stream,
                                              const plzma_path * LIBPLZMA_NONNULL archive_path);


/// @brief Opens the encoder for compressing.
///
/// During the process, the encoder is self-retained till operation is in progress.
/// @note The opening progress might be executed in a separate thread.
/// @note The opening progress might be aborted via \a plzma_encoder_abort function.
/// @note Thread-safe.
LIBPLZMA_C_API(bool) plzma_encoder_open(plzma_encoder * LIBPLZMA_NONNULL encoder);


/// @brief Aborts the opening or compressing process.
/// @note The aborted encoder is no longer valid.
LIBPLZMA_C_API(void) plzma_encoder_abort(plzma_encoder * LIBPLZMA_NONNULL encoder);


/// @brief Compresses the provided paths and streams.
///
/// During the process, the encoder is self-retained till operation is in progress.
/// @note The compress progress might be executed in a separate thread.
/// @note The compress progress might be aborted via \a plzma_encoder_abort function.
/// @note Thread-safe.
LIBPLZMA_C_API(bool) plzma_encoder_compress(plzma_encoder * LIBPLZMA_NONNULL encoder);


/// @brief Releases the encoder object.
LIBPLZMA_C_API(void) plzma_encoder_release(plzma_encoder * LIBPLZMA_NONNULL encoder);

#endif // !__LIBPLZMA_H__
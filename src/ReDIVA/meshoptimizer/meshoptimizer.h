/**
 * meshoptimizer - version 0.18
 *
 * Copyright (C) 2016-2023, by Arseny Kapoulkine (arseny.kapoulkine@gmail.com)
 * Report bugs and download new versions at https://github.com/zeux/meshoptimizer
 *
 * This library is distributed under the MIT License. See notice at the end of this file.
 */
#pragma once

#include <assert.h>
#include <stddef.h>

/* Version macro; major * 1000 + minor * 10 + patch */
#define MESHOPTIMIZER_VERSION 180 /* 0.18 */

/* If no API is defined, assume default */
#ifndef MESHOPTIMIZER_API
#define MESHOPTIMIZER_API
#endif

/* Set the calling-convention for alloc/dealloc function pointers */
#ifndef MESHOPTIMIZER_ALLOC_CALLCONV
#ifdef _MSC_VER
#define MESHOPTIMIZER_ALLOC_CALLCONV __cdecl
#else
#define MESHOPTIMIZER_ALLOC_CALLCONV
#endif
#endif

/* Experimental APIs have unstable interface and might have implementation that's not fully tested or optimized */
#define MESHOPTIMIZER_EXPERIMENTAL MESHOPTIMIZER_API

/* C interface */
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Vertex attribute stream
 * Each element takes size bytes, beginning at data, with stride controlling the spacing between successive elements (stride >= size).
 */
struct meshopt_Stream
{
	const void* data;
	size_t size;
	size_t stride;
};

/**
 * Mesh stripifier
 * Converts a previously vertex cache optimized triangle list to triangle strip, stitching strips using restart index or degenerate triangles
 * Returns the number of indices in the resulting strip, with destination containing new index data
 * For maximum efficiency the index buffer being converted has to be optimized for vertex cache first.
 * Using restart indices can result in ~10% smaller index buffers, but on some GPUs restart indices may result in decreased performance.
 *
 * destination must contain enough space for the target index buffer, worst case can be computed with meshopt_stripifyBound
 * restart_index should be 0xffff or 0xffffffff depending on index size, or 0 to use degenerate triangles
 */
MESHOPTIMIZER_API size_t meshopt_stripify(unsigned int* destination, const unsigned int* indices, size_t index_count, size_t vertex_count, unsigned int restart_index);
MESHOPTIMIZER_API size_t meshopt_stripifyBound(size_t index_count);

/**
 * Mesh unstripifier
 * Converts a triangle strip to a triangle list
 * Returns the number of indices in the resulting list, with destination containing new index data
 *
 * destination must contain enough space for the target index buffer, worst case can be computed with meshopt_unstripifyBound
 */
MESHOPTIMIZER_API size_t meshopt_unstripify(unsigned int* destination, const unsigned int* indices, size_t index_count, unsigned int restart_index);
MESHOPTIMIZER_API size_t meshopt_unstripifyBound(size_t index_count);

#ifdef __cplusplus
} /* extern "C" */
#endif

/* Internal implementation helpers */
#ifdef __cplusplus
class meshopt_Allocator
{
public:
	template <typename T>
	struct StorageT
	{
		static void* (MESHOPTIMIZER_ALLOC_CALLCONV *allocate)(size_t);
		static void (MESHOPTIMIZER_ALLOC_CALLCONV *deallocate)(void*);
	};

	typedef StorageT<void> Storage;

	meshopt_Allocator()
		: blocks()
		, count(0)
	{
	}

	~meshopt_Allocator()
	{
		for (size_t i = count; i > 0; --i)
			Storage::deallocate(blocks[i - 1]);
	}

	template <typename T> T* allocate(size_t size)
	{
		assert(count < sizeof(blocks) / sizeof(blocks[0]));
		T* result = static_cast<T*>(Storage::allocate(size > size_t(-1) / sizeof(T) ? size_t(-1) : size * sizeof(T)));
		blocks[count++] = result;
		return result;
	}

private:
	void* blocks[24];
	size_t count;
};

// This makes sure that allocate/deallocate are lazily generated in translation units that need them and are deduplicated by the linker
template <typename T> void* (MESHOPTIMIZER_ALLOC_CALLCONV *meshopt_Allocator::StorageT<T>::allocate)(size_t) = operator new;
template <typename T> void (MESHOPTIMIZER_ALLOC_CALLCONV *meshopt_Allocator::StorageT<T>::deallocate)(void*) = operator delete;
#endif

/**
 * Copyright (c) 2016-2023 Arseny Kapoulkine
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

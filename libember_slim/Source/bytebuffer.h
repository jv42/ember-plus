/*
   libember_slim -- ANSI C implementation of the Ember+ Protocol
   Copyright (C) 2012-2014  L-S-B Broadcast Technologies GmbH

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef __LIBEMBER_SLIM_BYTEBUFFER_H
#define __LIBEMBER_SLIM_BYTEBUFFER_H

#include "bertypes.h"

/**
  * Primitive static byte buffer.
  */
typedef struct SByteBuffer
{
   /**
     * Address of the byte buffer in memory.
     */
   byte *pMemory;

   /**
     * Size of the byte buffer.
     */
   unsigned int size;

   /**
     * Current position of the write cursor.
     */
   unsigned int position;

   /**
     * private field
     */
   bool isDynamic;
} ByteBuffer;

/**
  * Initializes a ByteBuffer instance with a memory address
  * and the size of the allocated memory chunk at @p pMemory.
  * Must be called before any operations on the ByteBuffer at
  * @p pThis are invoked.
  * @param pThis pointer to the object to process.
  * @pMemory the address of the memory chunk allocated for the buffer.
  * @param size the size of the memory chunk at @p pMemory in bytes.
  */
void byteBuffer_init(ByteBuffer *pThis, byte *pMemory, unsigned int size);

/**
  * Initializes a dynamic ByteBuffer instance, which allocates
  * memory if needed.
  * Must be called before any operations on the ByteBuffer at
  * @p pThis are invoked.
  * @param pThis pointer to the object to process.
  * @param size the initial size of the buffer.
  */
void byteBuffer_initDynamic(ByteBuffer *pThis, unsigned int size);

/**
  * Add a byte to the buffer, advancing the write position.
  * @param pThis pointer to the object to process.
  * @param b the byte to add to the end of the buffer.
  * @note If the buffer is full, this operation calls the
  *     throwError callback.
  */
void byteBuffer_add(ByteBuffer *pThis, byte b);

/**
  * Resets the write position to the begin of the buffer.
  * @param pThis pointer to the object to process.
  */
void byteBuffer_reset(ByteBuffer *pThis);

/**
  * Ensures that the size of the dynamic buffer is at least @p size bytes.
  * Does nothing if @p pThis is not dynamic.
  * @param pThis pointer to the object to process.
  * @param size new size of the buffer.
  */
void byteBuffer_resize(ByteBuffer *pThis, unsigned int size);

/**
  * Gets a value indicating whether the passed byte buffer
  * is full or not.
  * @param pThis pointer to the object to process.
  * @return True if the byte buffer's write cursor is past the
  *      end of the buffer, otherwise false.
  */
bool byteBuffer_isEmpty(const ByteBuffer *pThis);

/**
  * Frees all memory allocated by the dynamic buffer.
  * @param pThis pointer to the object to process.
  */
void byteBuffer_free(ByteBuffer *pThis);

#endif

//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include "inet/common/packet/chunk/BitCountChunk.h"

namespace inet {

BitCountChunk::BitCountChunk() :
    Chunk(),
    length(-1)
{
}

BitCountChunk::BitCountChunk(const BitCountChunk& other) :
    Chunk(other),
    length(other.length)
{
}

BitCountChunk::BitCountChunk(bit length) :
    Chunk(),
    length(length)
{
    CHUNK_CHECK_USAGE(length >= bit(0), "length is invalid");
}

std::shared_ptr<Chunk> BitCountChunk::peekUnchecked(PeekPredicate predicate, PeekConverter converter, const Iterator& iterator, bit length, int flags) const
{
    bit chunkLength = getChunkLength();
    CHUNK_CHECK_USAGE(bit(0) <= iterator.getPosition() && iterator.getPosition() <= chunkLength, "iterator is out of range");
    // 1. peeking an empty part returns nullptr
    if (length == bit(0) || (iterator.getPosition() == chunkLength && length == bit(-1))) {
        if (predicate == nullptr || predicate(nullptr))
            return nullptr;
    }
    // 2. peeking the whole part returns this chunk
    if (iterator.getPosition() == bit(0) && (length == bit(-1) || length == chunkLength)) {
        auto result = const_cast<BitCountChunk *>(this)->shared_from_this();
        if (predicate == nullptr || predicate(result))
            return result;
    }
    // 3. peeking without conversion returns a BitCountChunk
    if (converter == nullptr) {
        auto result = std::make_shared<BitCountChunk>(length == bit(-1) ? chunkLength - iterator.getPosition() : length);
        result->markImmutable();
        return result;
    }
    // 4. peeking with conversion
    return converter(const_cast<BitCountChunk *>(this)->shared_from_this(), iterator, length, flags);
}

std::shared_ptr<Chunk> BitCountChunk::convertChunk(const std::type_info& typeInfo, const std::shared_ptr<Chunk>& chunk, bit offset, bit length, int flags)
{
    bit chunkLength = chunk->getChunkLength();
    bit resultLength = length == bit(-1) ? chunkLength - offset : length;
    CHUNK_CHECK_IMPLEMENTATION(bit(0) <= resultLength && resultLength <= chunkLength);
    return std::make_shared<BitCountChunk>(resultLength);
}

void BitCountChunk::setLength(bit length)
{
    CHUNK_CHECK_USAGE(length >= bit(0), "length is invalid");
    handleChange();
    this->length = length;
}

bool BitCountChunk::canInsertAtBeginning(const std::shared_ptr<Chunk>& chunk)
{
    return chunk->getChunkType() == CT_BITCOUNT;
}

bool BitCountChunk::canInsertAtEnd(const std::shared_ptr<Chunk>& chunk)
{
    return chunk->getChunkType() == CT_BITCOUNT;
}

void BitCountChunk::insertAtBeginning(const std::shared_ptr<Chunk>& chunk)
{
    CHUNK_CHECK_IMPLEMENTATION(chunk->getChunkType() == CT_BITCOUNT);
    handleChange();
    const auto& bitCountChunk = std::static_pointer_cast<BitCountChunk>(chunk);
    length += bitCountChunk->length;
}

void BitCountChunk::insertAtEnd(const std::shared_ptr<Chunk>& chunk)
{
    CHUNK_CHECK_IMPLEMENTATION(chunk->getChunkType() == CT_BITCOUNT);
    handleChange();
    const auto& bitCountChunk = std::static_pointer_cast<BitCountChunk>(chunk);
    length += bitCountChunk->length;
}

void BitCountChunk::removeFromBeginning(bit length)
{
    CHUNK_CHECK_USAGE(bit(0) <= length && length <= getChunkLength(), "length is invalid");
    handleChange();
    this->length -= length;
}

void BitCountChunk::removeFromEnd(bit length)
{
    CHUNK_CHECK_USAGE(bit(0) <= length && length <= getChunkLength(), "length is invalid");
    handleChange();
    this->length -= length;
}

std::string BitCountChunk::str() const
{
    std::ostringstream os;
    os << "BitCountChunk, length = " << length;
    return os.str();
}

} // namespace

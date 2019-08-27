#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include <list>
#include <mutex>

class GlobalMemBlockPool{
public:
	using MemBlockPtr = std::shared_ptr<std::vector<std::byte>>;
	using MemBlockPtrIter = std::vector<MemBlockPtr>::const_iterator;

	constexpr static size_t BLOCK_SIZE = 512 * 1024;
	MemBlockPtr alloc();
	void free(MemBlockPtrIter begin, MemBlockPtrIter end);

	GlobalMemBlockPool(const GlobalMemBlockPool&) = delete;
	GlobalMemBlockPool& operator =(const GlobalMemBlockPool&) = delete;

	static GlobalMemBlockPool& getInstance() {
		static GlobalMemBlockPool pool;
		return pool;
	}
private:
	GlobalMemBlockPool() {}
	std::list<MemBlockPtr> freeBlocks;
	std::mutex m_mutex;
	size_t m_iTotalMem = 0;

};

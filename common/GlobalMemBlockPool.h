#pragma once

#include <cstddef>
#include <memory>
#include <vector>
#include <list>
#include <mutex>

using MemBuffer=std::vector<std::byte>;
using MemBlockPtr = std::unique_ptr<MemBuffer>;

class GlobalMemBlockPool{
public:
	constexpr static size_t BLOCK_SIZE = 512 * 1024;
	MemBlockPtr alloc();
	void free(std::vector<MemBlockPtr>& buffers);
	void free(MemBlockPtr&& pBuffer);

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

#include <utility>
#include "GlobalMemBlockPool.h"
#include "MetaConfig.h"

MemBlockPtr GlobalMemBlockPool::alloc() {
	m_mutex.lock();
	if (freeBlocks.empty()) {
		m_iTotalMem += BLOCK_SIZE;
		size_t iTotal = m_iTotalMem;

		m_mutex.unlock();

		if(iTotal > MetaConfig::getInstance().getExecutionBuffer()) {
			return MemBlockPtr();
		}
		DLOG(INFO)<< "Allocated Memory Blocks:" << iTotal / BLOCK_SIZE;
		return std::make_unique < std::vector < std::byte >> (BLOCK_SIZE);
	} else {
		MemBlockPtr head = std::move(*freeBlocks.begin());
		freeBlocks.erase(freeBlocks.begin());

		m_mutex.unlock();
		return head;
	}
}
void GlobalMemBlockPool::free(std::vector<MemBlockPtr>& buffers) {
	std::lock_guard < std::mutex > lock(m_mutex);
	for (auto& pBuffer: buffers) {
		freeBlocks.emplace(freeBlocks.end(), std::move(pBuffer));
	}
}

void GlobalMemBlockPool::free(MemBlockPtr&& pBuffer) {
	std::lock_guard < std::mutex > lock(m_mutex);
	freeBlocks.emplace(freeBlocks.end(), std::forward<MemBlockPtr>(pBuffer));
}

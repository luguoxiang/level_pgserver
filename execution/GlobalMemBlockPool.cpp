#include "GlobalMemBlockPool.h"
#include "common/MetaConfig.h"
#include "ExecutionException.h"

GlobalMemBlockPool::MemBlockPtr GlobalMemBlockPool::alloc() {
	std::lock_guard < std::mutex > lock(m_mutex);
	if (freeBlocks.empty()) {
		m_iTotalMem += BLOCK_SIZE;
		if(m_iTotalMem > MetaConfig::getInstance().getExecutionBuffer()) {
			EXECUTION_ERROR("Not enough execution buffer");
		}
		return std::make_unique < std::vector < std::byte >> (BLOCK_SIZE);
	} else {
		MemBlockPtr head = std::move(*freeBlocks.begin());
		freeBlocks.erase(freeBlocks.begin());
		return head;
	}
}
void GlobalMemBlockPool::free(MemBlockPtrIter begin, MemBlockPtrIter end) {
	std::lock_guard < std::mutex > lock(m_mutex);
	for (auto iter = begin; iter != end; ++iter) {
		freeBlocks.emplace(freeBlocks.end(), iter->release());
	}
}

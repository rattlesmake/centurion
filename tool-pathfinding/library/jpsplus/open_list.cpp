#include "open_list.h"

#pragma region Public memmbers:
void OpenList::Push(PathfindingNode* pNode)
{
	this->elements.insert(pNode);
}

PathfindingNode* OpenList::ExtractTop(void)
{
	if (this->elements.empty() == false)
	{
		auto top = this->elements.begin();
		PathfindingNode*const pNode = (*top);
		this->elements.erase(top);
		return pNode;
	}
	return nullptr;
}

void OpenList::Update(PathfindingNode* pNode)
{
	this->elements.erase(pNode);
	this->elements.insert(pNode);
}

bool OpenList::IsEmpty(void) const
{
	return this->elements.empty();
}

uint32_t OpenList::GetSize(void) const
{
	return (uint32_t)this->elements.size();
}
#pragma endregion

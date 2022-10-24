#include "Node.h"
#include "Node.mh"
#include "Node.ic"
#include "Node.mc"
#include "Scene.h"

Node::Node()
{}

paf::BorrowedPtr<Node> Node::PushBack(paf::UniquePtr<Node>&& node, paf::BorrowedPtr<Node> parentNode)
{
	PAF_ASSERT(node != nullptr && parentNode != nullptr);
	PAF_ASSERT(node->m_parent == nullptr && node->m_nextSibling == nullptr && node->m_prevSibling == nullptr && node->m_scene == nullptr);

	paf::BorrowedPtr<Node> result(node);
	node->m_parent = parentNode;
	node->m_scene = parentNode->m_scene;
	if (parentNode->m_childTail != nullptr)
	{
		PAF_ASSERT(parentNode->m_childTail->m_nextSibling == nullptr);
		node->m_prevSibling = parentNode->m_childTail;
		parentNode->m_childTail->m_nextSibling = std::move(node);
		parentNode->m_childTail = parentNode->m_childTail->m_nextSibling;
	}
	else
	{
		PAF_ASSERT(parentNode->m_childHead == nullptr);
		parentNode->m_childTail = node;
		parentNode->m_childHead = std::move(node);
	}
	return result;
}

paf::BorrowedPtr<Node> Node::InsertBefore(paf::UniquePtr<Node>&& node, paf::BorrowedPtr<Node> siblingNode)
{
	PAF_ASSERT(node != nullptr && siblingNode != nullptr);
	PAF_ASSERT(node->m_parent == nullptr && node->m_nextSibling == nullptr && node->m_prevSibling == nullptr && node->m_scene == nullptr);
	PAF_ASSERT(siblingNode->m_parent != nullptr);

	paf::BorrowedPtr<Node> result(node);
	paf::BorrowedPtr<Node> parentNode = siblingNode->m_parent;
	node->m_parent = parentNode;
	node->m_scene = parentNode->m_scene;
	if (parentNode->m_childHead != siblingNode)
	{
		PAF_ASSERT(siblingNode->m_prevSibling != nullptr);

		node->m_prevSibling = siblingNode->m_prevSibling;
		node->m_nextSibling = std::move(siblingNode->m_prevSibling->m_nextSibling);
		siblingNode->m_prevSibling = node;
		node->m_prevSibling->m_nextSibling = std::move(node);

	}
	else
	{
		PAF_ASSERT(siblingNode->m_prevSibling == nullptr);
		siblingNode->m_prevSibling = node;
		node->m_nextSibling = std::move(parentNode->m_childHead);
		parentNode->m_childHead = std::move(node);
	}
	return result;
}

paf::UniquePtr<Node> Node::RemoveFromParent(paf::BorrowedPtr<Node> node)
{
	PAF_ASSERT(node != nullptr && node->m_parent != nullptr && node->m_scene != nullptr);

	paf::BorrowedPtr<Node> parentNode = node->m_parent;
	paf::UniquePtr<Node> result(std::move(node->m_nextSibling));
	bool(result) ? result->m_prevSibling : parentNode->m_childTail = node->m_prevSibling;
	result.swap(bool(node->m_prevSibling) ? node->m_prevSibling->m_nextSibling : parentNode->m_childHead);
	node->m_prevSibling = nullptr;
	node->m_parent = nullptr;
	node->m_scene = nullptr;
	return result;
}

void Node::check__(Scene* scene, Node* parent, int depth)
{
	PAF_ASSERT(m_scene == scene);
	PAF_ASSERT(m_parent == parent);
	for (int i = 0; i < depth; ++i)
	{
		printf("\t");
	}
	printf("Node: %s\n", m_name.c_str());
	if (m_childHead == nullptr)
	{
		PAF_ASSERT(m_childTail == nullptr);
		return;
	}
	PAF_ASSERT(m_childTail != nullptr);
	PAF_ASSERT(m_childHead->m_prevSibling == nullptr);
	PAF_ASSERT(m_childTail->m_nextSibling == nullptr);

	paf::UniquePtr<Node>* childNode = &m_childHead;
	paf::UniquePtr<Node>* nextChildNode = &(*childNode)->m_nextSibling;
	while ((*childNode) != nullptr)
	{
		(*childNode)->check__(scene, this, depth + 1);
		if ((*nextChildNode) != nullptr)
		{
			PAF_ASSERT((*nextChildNode)->m_prevSibling == *childNode);
			childNode = nextChildNode;
			nextChildNode = &(*nextChildNode)->m_nextSibling;
		}
		else
		{
			break;
		}
	}
}

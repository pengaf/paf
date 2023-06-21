#include "Tree.h"
#include "Tree.mh"
#include "Tree.ic"
#include "Tree.mc"

bool TreeNode::addChild(TreeNode* child, TreeNode* nextSibling)
{
	if (!child)
	{
		return false;
	}
	TreeNode* node = this;
	while (node)
	{
		if (node == child)
		{
			return false;
		}
		node = node->m_parent.get();
	}
	if (nextSibling && nextSibling->m_parent != this)
	{
		nextSibling = nullptr;
	}
}

void TreeNode::addFreeChild(TreeNode* child, TreeNode* nextSibling)
{
	PAF_ASSERT(child && child->isFree() && !child->m_nextSibling && !child->m_prevSibling);
	PAF_ASSERT(!nextSibling || nextSibling->m_parent == this);

	if (nextSibling)
	{
		child->m_nextSibling = nextSibling;
		child->m_prevSibling = nextSibling->m_prevSibling;
		if(nextSibling->m_prevSibling)
		{
			nextSibling->m_prevSibling->m_nextSibling = child;
		}
		else
		{
			PAF_ASSERT(m_childHead == nextSibling);
			m_childHead = child;
		}
		nextSibling->m_prevSibling = child;
	}
	else
	{
		child->m_prevSibling = m_childTail;
		if (m_childTail)
		{
			PAF_ASSERT(!m_childTail->m_nextSibling);
			m_childTail->m_nextSibling = child;
		}
		else
		{
			PAF_ASSERT(!m_childHead);
			m_childHead = child;
		}
		m_childTail = child;
	}
}


void Tree::setRoot(TreeNode* node)
{
	m_root = node;
}

bool Tree::attachNode(TreeNode* node, TreeNode* parent, TreeNode* nextSibling)
{
	return false;
}

bool Tree::detachNode(TreeNode* node)
{
	return false;
	//if (!node)
	//{
	//	return false;
	//}
	//if (node->m_tree != this)
	//{
	//	return false;
	//}
	//if (m_root == node)
	//{
	//}
}

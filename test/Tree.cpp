#include "Tree.h"
#include "Tree.mh"
#include "Tree.ic"
#include "Tree.mc"

bool Tree::Node::addChild(Node* node, Node* nextSibling)
{
	if (!node)
	{
		return false;
	}
	Node* tmp = this;
	while (tmp)
	{
		if (tmp == node)
		{
			return false;
		}
		tmp = tmp->m_parent.get();
	}

	if (nextSibling && nextSibling->m_parent != this)
	{
		nextSibling = nullptr;
	}

}

void Tree::setRoot(Tree::Node* node)
{
	m_root = node;
}

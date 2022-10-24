#import "../pafcore/Utility.i"
#import "Node.i"


class Scene : Introspectable
{
	Scene();
	nocode Node+ getRootNode() const;

#{
	friend class Node;
	Scene(const Scene&) = delete;
	Scene(Scene&&) = delete;
public:
	paf::BorrowedPtr<Node> Scene::getRootNode() const
	{
		return paf::BorrowedPtr<Node>(m_rootNode);
	}
public:
	void check__();
protected:
	paf::UniquePtr<Node> m_rootNode;
#}
};

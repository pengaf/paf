#import "../pafcore/Utility.i"
#import "Node.i"


class Scene : Introspectable
{
	Scene();
	nocode Node^ getRootNode() const;

#{
	friend class Node;
	Scene(const Scene&) = delete;
	Scene(Scene&&) = delete;
public:
	paf::SharedPtr<Node> Scene::getRootNode() const
	{
		return m_rootNode;
	}
public:
	void check__();
protected:
	paf::SharedPtr<Node> m_rootNode;
#}
};

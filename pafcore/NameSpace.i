#import "Metadata.i"

#{
#include <unordered_set>
#}

namespace pafcore
{

	abstract class(name_space) NameSpace : Metadata
	{
		size_t _getMemberCount_();
		Metadata* _getMember_(size_t index);
		Metadata* _findMember_(string_t name);
#{
	private:
		struct Hash_Metadata
		{
			size_t operator ()(const Metadata* metadata) const;
		};
		struct Equal_Metadata
		{
			bool operator() (const Metadata* lhs, const Metadata* rhs) const;
		};
	private:
		NameSpace(const char* name);
		~NameSpace();
	public:
		PAFCORE_EXPORT NameSpace* getNameSpace(const char* name);
		PAFCORE_EXPORT ErrorCode registerMember(Metadata* metadata);
		PAFCORE_EXPORT Metadata* findMember(const char* name);
		PAFCORE_EXPORT void unregisterMember(Metadata* metadata);
		Metadata* getEnclosing() const;
	protected:
		typedef std::unordered_set<Metadata*, Hash_Metadata, Equal_Metadata> MetadataContainer;
		MetadataContainer m_members;
		Metadata* m_enclosing;
	public:
		PAFCORE_EXPORT static NameSpace* GetGlobalNameSpace();
#}
	};

#{
	inline Metadata* NameSpace::getEnclosing() const
	{
		return m_enclosing;
	}

#}
}
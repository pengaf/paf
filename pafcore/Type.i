#import "Metadata.i"

namespace pafcore
{
#{
	class NameSpace;
#}
	abstract class #PAFCORE_EXPORT Type : Metadata
	{
		size_t _size_ { get };
#{
		friend class NameSpace;
	public:
		Type(const char* name, MetaCategory category, const char* declarationFile);
		~Type();
	public:
		virtual bool destruct(void* address) = 0;
		virtual bool copyConstruct(void* dst, const void* src) = 0;
		virtual bool copyAssign(void* dst, const void* src) = 0;
		virtual Metadata* findMember(const char* name) = 0;
	public:
		MetaCategory getCategory() const;
		bool isPrimitive() const;
		bool isEnumeration() const;
		bool isString() const;
		bool isBuffer() const;
		bool isObject() const;
		bool isClass() const;
		Metadata* getEnclosing() const;
		const char* getDeclarationFile() const;
	protected:
		MetaCategory m_category;
		size_t m_size;
		Metadata* m_enclosing;
		const char* m_declarationFile;//类型声明所在文件路径
#}
	};

#{

	inline size_t Type::get__size_() const
	{
		return m_size;
	}
	
	inline MetaCategory Type::getCategory() const
	{
		return m_category;
	}

	inline bool Type::isPrimitive() const
	{
		return MetaCategory::primitive == m_category;
	}
	
	inline bool Type::isEnumeration() const
	{
		return MetaCategory::enumeration == m_category;
	}

	inline bool Type::isObject() const
	{
		return MetaCategory::object == m_category;
	}

	inline bool Type::isString() const
	{
		return MetaCategory::string == m_category;
	}

	inline bool Type::isBuffer() const
	{
		return MetaCategory::buffer == m_category;
	}

	inline bool Type::isClass() const
	{
		return MetaCategory::object <= m_category;
	}
	
	inline Metadata* Type::getEnclosing() const
	{
		return m_enclosing;
	}

	inline const char* Type::getDeclarationFile() const
	{
		return m_declarationFile;
	}

#}

}

#import "Metadata.i"

namespace paf
{
#{
	class NameSpace;
	class Variant;
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
		virtual ErrorCode placementNew(void* address, Variant** args, uint32_t numArgs);
		virtual bool placementNewCopy(void* self, const void* src);
		virtual bool placementNewArray(void* address, size_t count);
		virtual bool destruct(void* self);
		virtual bool copyAssign(void* self, const void* src);
		virtual bool assign(void* self, Type* srcType, const void* src);
		virtual bool cast(Type* dstType, void* dst, const void* self);
	public:
		virtual Metadata* findMember(const char* name);
	public:
		MetaCategory getMetaCategory() const;
		bool isPrimitive() const;
		bool isEnumeration() const;
		bool isClass() const;
		bool isString() const;
		bool isBuffer() const;
		bool isObject() const;
		Metadata* getEnclosing() const;
		const char* getDeclarationFile() const;
	protected:
		MetaCategory m_metaCategory;
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
	
	inline MetaCategory Type::getMetaCategory() const
	{
		return m_metaCategory;
	}

	inline bool Type::isPrimitive() const
	{
		return MetaCategory::primitive == m_metaCategory;
	}
	
	inline bool Type::isEnumeration() const
	{
		return MetaCategory::enumeration == m_metaCategory;
	}

	inline bool Type::isString() const
	{
		return MetaCategory::string == m_metaCategory;
	}

	inline bool Type::isBuffer() const
	{
		return MetaCategory::buffer == m_metaCategory;
	}

	inline bool Type::isObject() const
	{
		return MetaCategory::object == m_metaCategory;
	}

	inline bool Type::isClass() const
	{
		return MetaCategory::object <= m_metaCategory;
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

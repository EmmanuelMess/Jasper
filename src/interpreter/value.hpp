#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "../utils/interned_string.hpp"
#include "../utils/span.hpp"
#include "environment_fwd.hpp"
#include "value_fwd.hpp"
#include "value_tag.hpp"

namespace TypedAST {
struct FunctionLiteral;
}

namespace Interpreter {

using Identifier = InternedString;
using StringType = std::string;
using ObjectType = std::unordered_map<Identifier, Value*>;
using DictionaryType = std::unordered_map<StringType, Value*>;
using ArrayType = std::vector<Value*>;
using FunctionType = TypedAST::FunctionLiteral*;
using NativeFunctionType = auto(Span<Value*>, Environment&) -> Value*;

// Returns the value pointed to by a reference
void print(Value* v, int d = 0);
void gc_visit(Value*);

struct Value {
  protected:
	ValueTag m_type;

  public:
	bool m_visited = false;
	int m_cpp_refcount = 0;

	Value(ValueTag type)
	    : m_type(type) {}
	ValueTag type() const {
		return m_type;
	}

	virtual ~Value() = default;
};

struct Null : Value {

	Null();
};

struct Integer : Value {
	int m_value = 0;

	Integer();
	Integer(int v);
};

struct Float : Value {
	float m_value = 0.0;

	Float();
	Float(float v);
};

struct Boolean : Value {
	bool m_value = false;

	Boolean();
	Boolean(bool b);
};

struct String : Value {
	std::string m_value = "";

	String();
	String(std::string s);
};

struct Array : Value {
	ArrayType m_value;

	Array();
	Array(ArrayType l);

	void append(Value* v);
	Value* at(int position);
};

struct Object : Value {
	ObjectType m_value;

	Object();
	Object(ObjectType);

	void addMember(Identifier const& id, Value* v);
	Value* getMember(Identifier const& id);
};

struct Dictionary : Value {
	DictionaryType m_value;

	Dictionary();
	Dictionary(DictionaryType);

	void addMember(StringType const& id, Value* v);
	Value* getMember(StringType const& id);
	void removeMember(StringType const& id);
};

struct Function : Value {
	FunctionType m_def;
	// TODO: store references instead of values
	ObjectType m_captures;

	Function(FunctionType, ObjectType);
};

struct NativeFunction : Value {
	NativeFunctionType* m_fptr;

	NativeFunction(NativeFunctionType* = nullptr);
};

struct Reference : Value {
	Value* m_value;

	Reference(Value* value);
};

} // namespace Interpreter

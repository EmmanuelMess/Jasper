#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "chunked_array.hpp"
#include "typechecker.hpp"

namespace TypedAST {

struct Declaration;
struct FunctionLiteral;
struct FunctionArgument;

} // namespace TypedAST

namespace Frontend {

enum class BindingType { Declaration, Argument };

struct Binding {
	BindingType m_type;

	// this acts as an union. maybe use an actual union, eventually?
	TypedAST::Declaration* m_decl;

	TypedAST::FunctionLiteral* m_func;
	int m_arg_index;

	Binding(TypedAST::Declaration* decl);
	Binding(TypedAST::FunctionLiteral* func, int arg_index);

	TypedAST::Declaration* get_decl();
	TypedAST::FunctionArgument& get_arg();
	TypedAST::FunctionLiteral* get_func();
};

struct Scope {
	bool m_nested {false};
	std::unordered_map<std::string, Binding> m_vars;
	std::unordered_set<VarId> m_type_vars;
};

struct CompileTimeEnvironment {
	Scope m_global_scope;
	std::vector<Scope> m_scopes;
	std::vector<TypedAST::FunctionLiteral*> m_function_stack;
	TypedAST::Declaration* m_current_decl {nullptr};

	ChunkedArray<TypedAST::Declaration> m_builtin_declarations;
	TypeChecker::TypeChecker m_typechecker;

	CompileTimeEnvironment();

	void declare(std::string const&, TypedAST::Declaration*);
	void declare_arg(std::string const&, TypedAST::FunctionLiteral*, int arg_index);
	void declare_builtin(std::string const&);
	void declare_builtin(std::string const&, PolyId);

	TypedAST::Declaration* access(std::string const&);
	Binding* access_binding(std::string const&);

	TypedAST::FunctionLiteral* current_function();
	void enter_function(TypedAST::FunctionLiteral*);
	void exit_function();

	TypedAST::Declaration* current_top_level_declaration();
	void enter_top_level_decl(TypedAST::Declaration*);
	void exit_top_level_decl();

	Scope& current_scope();
	void new_scope();
	void new_nested_scope();
	void end_scope();

	MonoId new_type_var();
	MonoId new_hidden_type_var();
	bool has_type_var(VarId);
};

} // namespace Frontend

#pragma once

#include "typed_ast.hpp"
#include "node_allocator.hpp"

namespace TypedAST {

// TODO: could we use the TYPED_AST_TAGS macro here?
struct Allocator : public NodeAllocator<
    NumberLiteral,
    IntegerLiteral,
    StringLiteral,
    BooleanLiteral,
    NullLiteral,
    ObjectLiteral,
    ArrayLiteral,
    DictionaryLiteral,
    FunctionLiteral,
    Identifier,
    CallExpression,
    IndexExpression,
    RecordAccessExpression,
    TernaryExpression,
    Block,
    ReturnStatement,
    IfElseStatement,
    ForStatement,
    WhileStatement,
    DeclarationList,
    Declaration,
    StructExpression,
    TypeTerm,
    TypeFunctionHandle,
    MonoTypeHandle> {};

} // namespace TypedAST
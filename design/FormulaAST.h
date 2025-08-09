#pragma once

#include "FormulaLexer.h"
#include "common.h"

#include <forward_list>
#include <functional>
#include <stdexcept>

namespace ASTImpl {
class Expr;
}

class ParsingError : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class FormulaAST {
public:
    explicit FormulaAST(std::unique_ptr<ASTImpl::Expr> root_expr);
    FormulaAST(FormulaAST&&) = default;
    FormulaAST& operator=(FormulaAST&&) = default;
    ~FormulaAST();

    // Вычисляет формулу в контексте конкретного листа
    double Execute(const SheetInterface& sheet) const;
    void Print(std::ostream& out) const;
    void PrintFormula(std::ostream& out) const;

    // Список всех упомянутых ссылок (отсортированный, без дублей)
    std::vector<Position> GetReferencedCells() const;

    // Непосредственно собранные при парсинге ссылки (как есть, без сортировки)
    const std::forward_list<Position>& GetReferencedCellsUnordered() const noexcept {
        return cells_;
    }

private:
    // Внутренняя прокладка для вычисления: передаём в AST лямбду получения значения ячейки
    using CellResolver = std::function<double(Position)>;

    double ExecuteWithResolver(const CellResolver& get_cell_value) const;

private:
    std::unique_ptr<ASTImpl::Expr> root_expr_;
    std::forward_list<Position> cells_;  // ссылки CELL, собранные парсером
};

FormulaAST ParseFormulaAST(std::istream& in);
FormulaAST ParseFormulaAST(const std::string& in_str);
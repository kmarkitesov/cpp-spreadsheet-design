#pragma once

#include "cell.h"
#include "common.h"

#include <functional>

class Sheet : public SheetInterface {
public:
    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

    const Cell* GetConcreteCell(Position pos) const;
    Cell* GetConcreteCell(Position pos);

private:
    void MaybeIncreaseSizeToIncludePosition(Position pos);
    void UpdatePrintableSize();
    bool HasCircularDependency(Position pos, const std::vector<Position>& new_deps) const;

    // Обновление графа зависимостей при коммите новой формулы/текста
    void DetachDependencies(Position pos);
    void AttachDependencies(Position pos, const std::vector<Position>& new_deps);

    // Инвалидация кэша по rdeps_ с одним проходом
    void InvalidateCacheFrom(Position start) noexcept;

    std::vector<std::vector<std::unique_ptr<Cell>>> cells_;
    Size printable_size_;
};
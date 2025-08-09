#pragma once

#include "common.h"
#include "formula.h"
#include <optional>

class Sheet;

class Cell : public CellInterface {
public:
    Cell();
    ~Cell() override;

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    // Ссылки из формулы (для текстовой/пустой — пусто)
    std::vector<Position> GetReferencedCells() const override;

    // Кэш
    void InvalidateCache() noexcept; // только эту ячейку

    // Привязка к Sheet и координате — вызывается один раз при создании
    void BindOwner(Sheet* owner, Position pos) noexcept { owner_ = owner; pos_ = pos; }
    Position GetPosition() const noexcept { return pos_; }
    Sheet* GetOwner() const noexcept { return owner_; }

    // --- Кэш ---
    bool IsCacheValid() const noexcept { return cache_valid_; }
    void InvalidateCacheOnly() noexcept { cache_valid_ = false; cache_.reset(); } // без обхода графа

    // --- Граф зависимостей ---
    // На кого ССЫЛАЕТСЯ эта ячейка (ребра pos -> deps_)
    const std::vector<Position>& Dependencies() const noexcept { return deps_; }
    // Кто ЗАВИСИТ от этой ячейки (обратные ребра pos -> rdeps_ у других)
    const std::vector<Position>& Dependents() const noexcept { return rdeps_; }

    // Cлужебные методы для Sheet (обновление графа)
    void ReplaceDependencies(std::vector<Position> deps_sorted_unique); // только присвоение списка
    void AddDependent(Position p);
    void RemoveDependent(Position p);

private:
    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;
    std::unique_ptr<Impl> impl_;

    // Кэш видимого значения (формулы/текста), invalidation управляет Sheet
    mutable std::optional<Value> cache_;
    mutable bool cache_valid_ = false;

    // Граф:
    std::vector<Position> deps_;   // ссылки из этой ячейки
    std::vector<Position> rdeps_;  // ячейки, зависящие от этой

    Sheet* owner_ = nullptr;
    Position pos_{};
};
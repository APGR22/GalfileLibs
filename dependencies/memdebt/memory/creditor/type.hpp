#pragma once

#include <list>
#include <memory>

namespace memdebt::memory::creditor
{
    template<typename T>
    using type = std::list<std::shared_ptr<T>>;
}
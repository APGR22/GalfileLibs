#pragma once

#include <list>
#include <memory>
#include "type.hpp"
#include "../debtor/debtor.hpp"

namespace memdebt::memory::creditor
{
    template<typename T>
    class Creditor
    {
        private:
            type<T> __src_memory;

        public:
            debtor::Debtor<T> add(const T &value)
            {
                auto item = std::make_shared<T>(value);

                this->__src_memory.push_back(item);

                auto it = this->__src_memory.end();
                it--;

                return debtor::Debtor<T>(item, item.get(), it);
            }

            void sub(const type<T>::const_iterator &it)
            {
                this->__src_memory.erase(it);
            }
    };
}
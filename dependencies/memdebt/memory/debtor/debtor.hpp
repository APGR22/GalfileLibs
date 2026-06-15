#pragma once

#include <memory>
#include "../creditor/type.hpp"

namespace memdebt::memory::debtor
{
    template<typename T>
    class Debtor 
    {
        private:
            std::weak_ptr<T> __borrow;
            T *__item;
            creditor::type<T>::const_iterator __it;

        public:
            Debtor(
                const std::weak_ptr<T> &borrow,
                T *item,
                const creditor::type<T>::const_iterator &it
            )
            : __borrow(borrow), __item(item), __it(it)
            {}

            T &get() const
            {
                return *(this->__item);
            }

            const auto &get_it_pos() const
            {
                return this->__it;
            }

            bool check() const
            {
                return !(this->__borrow.expired());
            }

            T *operator->() const
            {
                return this->__item;
            }
    };
}
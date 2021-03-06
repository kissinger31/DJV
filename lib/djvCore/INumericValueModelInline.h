// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvCore/Math.h>

namespace djv
{
    namespace Core
    {
        template<typename T>
        inline void INumericValueModel<T>::_init()
        {
            _range          = ValueSubject<Range::Range<T> >::create();
            _value          = ValueSubject<T>::create();
            _isMin          = ValueSubject<bool>::create();
            _isMax          = ValueSubject<bool>::create();
            _smallIncrement = ValueSubject<T>::create();
            _largeIncrement = ValueSubject<T>::create();
            _overflow       = ValueSubject<NumericValueOverflow>::create(NumericValueOverflow::Clamp);
        }

        template<typename T>
        inline INumericValueModel<T>::INumericValueModel()
        {}

        template<typename T>
        inline INumericValueModel<T>::~INumericValueModel()
        {}

        template<typename T>
        inline std::shared_ptr<IValueSubject<Range::Range<T> > > INumericValueModel<T>::observeRange() const
        {
            return _range;
        }

        template<typename T>
        inline void INumericValueModel<T>::setRange(const Range::Range<T> & value)
        {
            if (_range->setIfChanged(value))
            {
                setValue(_value->get());
            }
        }

        template<typename T>
        inline std::shared_ptr<IValueSubject<T> > INumericValueModel<T>::observeValue() const
        {
            return _value;
        }

        template<typename T>
        inline std::shared_ptr<IValueSubject<bool> > INumericValueModel<T>::observeIsMin() const
        {
            return _isMin;
        }

        template<typename T>
        inline std::shared_ptr<IValueSubject<bool> > INumericValueModel<T>::observeIsMax() const
        {
            return _isMax;
        }

        template<typename T>
        inline void INumericValueModel<T>::setValue(T value)
        {
            const auto & range = _range->get();
            switch (_overflow->get())
            {
            case NumericValueOverflow::Clamp:
                value = Math::clamp(value, range.min, range.max);
                break;
            case NumericValueOverflow::Wrap:
                if (range.min != range.max)
                {
                    while (value > range.max)
                    {
                        value -= range.max;
                    }
                    while (value < range.min)
                    {
                        value += range.min;
                    }
                }
                break;
            default: break;
            }
            _value->setIfChanged(value);
            _isMin->setIfChanged(value == range.min);
            _isMax->setIfChanged(value == range.max);
        }

        template<typename T>
        inline void INumericValueModel<T>::setMin()
        {
            setValue(_range->get().min);
        }

        template<typename T>
        inline void INumericValueModel<T>::setMax()
        {
            setValue(_range->get().max);
        }

        template<typename T>
        inline std::shared_ptr<IValueSubject<T> > INumericValueModel<T>::observeSmallIncrement() const
        {
            return _smallIncrement;
        }

        template<typename T>
        inline std::shared_ptr<IValueSubject<T> > INumericValueModel<T>::observeLargeIncrement() const
        {
            return _largeIncrement;
        }

        template<typename T>
        inline void INumericValueModel<T>::incrementSmall()
        {
            setValue(_value->get() + _smallIncrement->get());
        }

        template<typename T>
        inline void INumericValueModel<T>::incrementLarge()
        {
            setValue(_value->get() + _largeIncrement->get());
        }

        template<typename T>
        inline void INumericValueModel<T>::decrementSmall()
        {
            setValue(_value->get() - _smallIncrement->get());
        }

        template<typename T>
        inline void INumericValueModel<T>::decrementLarge()
        {
            setValue(_value->get() - _largeIncrement->get());
        }

        template<typename T>
        inline void INumericValueModel<T>::setSmallIncrement(T value)
        {
            _smallIncrement->setIfChanged(value);
        }

        template<typename T>
        inline void INumericValueModel<T>::setLargeIncrement(T value)
        {
            _largeIncrement->setIfChanged(value);
        }

        template<typename T>
        inline std::shared_ptr<IValueSubject<NumericValueOverflow> > INumericValueModel<T>::observeOverflow() const
        {
            return _overflow;
        }

        template<typename T>
        inline void INumericValueModel<T>::setOverflow(NumericValueOverflow value)
        {
            _overflow->setIfChanged(value);
        }

    } // namespace Core
} // namespace djv

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/FloatLabel.h>

#include <djvUI/Label.h>

#include <djvCore/NumericValueModels.h>
#include <djvCore/ValueObserver.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        struct FloatLabel::Private
        {
            std::shared_ptr<FloatValueModel> model;
            size_t precision = 2;
            std::shared_ptr<Label> label;
            std::shared_ptr<ValueObserver<FloatRange> > rangeObserver;
            std::shared_ptr<ValueObserver<float> > valueObserver;
        };

        void FloatLabel::_init(const std::shared_ptr<Context>& context)
        {
            Widget::_init(context);

            DJV_PRIVATE_PTR();

            setClassName("djv::UI::FloatLabel");

            p.label = Label::create(context);
            p.label->setTextHAlign(TextHAlign::Right);
            addChild(p.label);

            _textUpdate();
        }

        FloatLabel::FloatLabel() :
            _p(new Private)
        {}

        FloatLabel::~FloatLabel()
        {}

        std::shared_ptr<FloatLabel> FloatLabel::create(const std::shared_ptr<Context>& context)
        {
            auto out = std::shared_ptr<FloatLabel>(new FloatLabel);
            out->_init(context);
            return out;
        }

        const std::shared_ptr<FloatValueModel> & FloatLabel::getModel() const
        {
            return _p->model;
        }

        void FloatLabel::setModel(const std::shared_ptr<FloatValueModel> & model)
        {
            DJV_PRIVATE_PTR();
            if (p.model)
            {
                p.valueObserver.reset();
            }
            p.model = model;
            if (p.model)
            {
                auto weak = std::weak_ptr<FloatLabel>(std::dynamic_pointer_cast<FloatLabel>(shared_from_this()));
                p.rangeObserver = ValueObserver<FloatRange>::create(
                    p.model->observeRange(),
                    [weak](const FloatRange & value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_textUpdate();
                    }
                });
                p.valueObserver = ValueObserver<float>::create(
                    p.model->observeValue(),
                    [weak](float value)
                {
                    if (auto widget = weak.lock())
                    {
                        widget->_textUpdate();
                    }
                });
            }
        }

        size_t FloatLabel::getPrecision()
        {
            return _p->precision;
        }

        void FloatLabel::setPrecision(size_t value)
        {
            DJV_PRIVATE_PTR();
            if (value == p.precision)
                return;
            p.precision = value;
            _textUpdate();
        }
        
        std::string FloatLabel::getSizeString(const FloatRange& range, size_t precision)
        {
            std::string out;
            const size_t digits = std::max(Math::getNumDigits(range.min), Math::getNumDigits(range.max));
            if (range.min < 0 || range.max < 0)
            {
                out += '-';
            }
            out += std::string(digits, '0');
            out += '.';
            for (size_t i = 0; i < precision; ++i)
            {
                out += '0';
            }
            return out;
        }

        void FloatLabel::_preLayoutEvent(Event::PreLayout & event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            _setMinimumSize(p.label->getMinimumSize() + getMargin().getSize(style));
        }

        void FloatLabel::_layoutEvent(Event::Layout & event)
        {
            DJV_PRIVATE_PTR();
            const auto& style = _getStyle();
            p.label->setGeometry(getMargin().bbox(getGeometry(), style));
        }

        void FloatLabel::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            if (p.model)
            {
                {
                    std::stringstream ss;
                    ss.precision(p.precision);
                    ss << std::fixed << p.model->observeValue()->get();
                    p.label->setText(ss.str());
                    p.label->setSizeString(getSizeString(p.model->observeRange()->get(), p.precision));
                }
            }
            _resize();
        }

    } // namespace UI
} // namespace djv

// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvUI/FlowLayout.h>

#include <djvCore/Math.h>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace Layout
        {
            struct Flow::Private
            {
                Spacing spacing = Spacing(MetricsRole::Spacing, MetricsRole::Spacing);
            };

            void Flow::_init(const std::shared_ptr<Context>& context)
            {
                Widget::_init(context);
                setClassName("djv::UI::Layout::Flow");
            }

            Flow::Flow() :
                _p(new Private)
            {}

            Flow::~Flow()
            {}

            std::shared_ptr<Flow> Flow::create(const std::shared_ptr<Context>& context)
            {
                auto out = std::shared_ptr<Flow>(new Flow);
                out->_init(context);
                return out;
            }

            const Spacing & Flow::getSpacing() const
            {
                return _p->spacing;
            }

            void Flow::setSpacing(const Spacing & value)
            {
                DJV_PRIVATE_PTR();
                if (value == p.spacing)
                    return;
                p.spacing = value;
                _resize();
            }

            float Flow::getHeightForWidth(float width) const
            {
                const auto& style = _getStyle();
                const glm::vec2 s = _p->spacing.get(style);
                glm::vec2 pos = glm::vec2(0.F, 0.F);
                float h = 0.F;
                const auto& children = getChildWidgets();
                const int childrenSize = static_cast<int>(children.size());
                for (int i = 0, j = 0; i < childrenSize; ++i, ++j)
                {
                    const auto& ms = children[i]->getMinimumSize();
                    if (j && pos.x + ms.x > width - getMargin().getWidth(style))
                    {
                        pos.x = 0.F;
                        pos.y += h + s.y;
                        h = ms.y;
                        j = 0;
                    }
                    else
                    {
                        h = std::max(h, ms.y);
                    }
                    pos.x += ms.x + s.x;
                }
                float out = pos.y + h + getMargin().getHeight(style);
                return out;
            }

            void Flow::_preLayoutEvent(Event::PreLayout &)
            {
                const auto& style = _getStyle();
                const float tc = style->getMetric(MetricsRole::TextColumn);
                glm::vec2 size = glm::vec2(0.F, 0.F);
                for (const auto & child : getChildWidgets())
                {
                    const auto & childMinimumSize = child->getMinimumSize();
                    size.x = std::max(size.x, childMinimumSize.x);
                    size.y += childMinimumSize.y;
                }
                size.x = std::max(size.x, tc);
                _setMinimumSize(size + getMargin().getSize(style));
            }

            void Flow::_layoutEvent(Event::Layout &)
            {
                const auto& style = _getStyle();
                const BBox2f & g = getMargin().bbox(getGeometry(), style);
                const float gx = g.min.x;
                const float gw = g.w();
                const glm::vec2 s = _p->spacing.get(style);

                glm::vec2 pos = g.min;
                const auto& children = getChildWidgets();
                const int childrenSize = static_cast<int>(children.size());
                typedef std::pair<std::shared_ptr<Widget>, BBox2f> Pair;
                std::map<int, std::vector<Pair> > rows;
                for (int i = 0, j = 0, k = 0; i < childrenSize; ++i, ++j)
                {
                    const auto & childMinimumSize = children[i]->getMinimumSize();
                    if (j && pos.x + childMinimumSize.x > gx + gw)
                    {
                        pos.x = gx;
                        j = 0;
                        ++k;
                    }
                    rows[k].push_back(std::make_pair(children[i], BBox2f(pos.x, pos.y, childMinimumSize.x, childMinimumSize.y)));
                    pos.x += childMinimumSize.x + s.x;
                }

                for (const auto & i : rows)
                {
                    float h = 0.F;
                    for (auto j : i.second)
                    {
                        h = std::max(h, j.second.h());
                    }
                    for (auto j : i.second)
                    {
                        BBox2f g = j.second;
                        g.min.y = pos.y;
                        g.max.y = pos.y + h;
                        j.first->setGeometry(Widget::getAlign(g, j.first->getMinimumSize(), j.first->getHAlign(), j.first->getVAlign()));
                    }
                    pos.y += h + s.y;
                }
            }

        } // namespace Layout
    } // namespace UI
} // namespace djv

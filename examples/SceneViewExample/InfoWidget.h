// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include "ISettingsWidget.h"

#include <djvUI/FormLayout.h>
#include <djvUI/GroupBox.h>
#include <djvUI/Label.h>

class InfoWidget : public ISettingsWidget
{
    DJV_NON_COPYABLE(InfoWidget);

protected:
    void _init(const std::shared_ptr<djv::Core::Context>&);
    InfoWidget();

public:
    virtual ~InfoWidget();

    static std::shared_ptr<InfoWidget> create(const std::shared_ptr<djv::Core::Context>&);

    void setBBox(const djv::Core::BBox3f&);
    void setPrimitivesCount(size_t);
    void setPointCount(size_t);
    void setFPS(float);

    void setSizeGroup(const std::weak_ptr<djv::UI::LabelSizeGroup>&) override;

protected:
    void _initEvent(djv::Core::Event::Init&) override;

private:
    void _textUpdate();

    djv::Core::BBox3f _bbox = djv::Core::BBox3f(0.F, 0.F, 0.F, 0.F, 0.F, 0.F);
    size_t _primitivesCount = 0;
    size_t _pointCount = 0;
    float _fps = 0.F;
    std::map<std::string, std::shared_ptr<djv::UI::Label> > _labels;
    std::map<std::string, std::shared_ptr<djv::UI::FormLayout> > _layouts;
    std::map<std::string, std::shared_ptr<djv::UI::GroupBox> > _groupBoxes;
};

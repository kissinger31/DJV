// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvViewApp/Enum.h>

#include <djvUI/Widget.h>

#include <djvAV/Color.h>

#include <djvCore/PicoJSON.h>
#include <djvCore/ValueObserver.h>

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            class Image;
    
        } // namespace Image

        namespace Render2D
        {
            class ImageOptions;

        } // namespace Render
    } // namespace AV

    namespace ViewApp
    {
        class AnnotatePrimitive;
        class Media;

        //! This class provides grid options.
        class GridOptions
        {
        public:
            GridOptions();

            bool                enabled     = false;
            float               size        = 100.F;
            AV::Image::Color    color       = AV::Image::Color(0.F, 0.F, 0.F, .5F);
            ImageViewGridLabels labels      = ImageViewGridLabels::X_Y;
            AV::Image::Color    labelsColor = AV::Image::Color(1.F, 1.F, 1.F, .65F);

            bool operator == (const GridOptions&) const;
        };

        //! This class provides an image view widget.
        class ImageView : public UI::Widget
        {
            DJV_NON_COPYABLE(ImageView);

        protected:
            void _init(const std::shared_ptr<Core::Context>&);
            ImageView();

        public:
            ~ImageView() override;

            static std::shared_ptr<ImageView> create(const std::shared_ptr<Core::Context>&);

            std::shared_ptr<Core::IValueSubject<std::shared_ptr<AV::Image::Image> > > observeImage() const;
            void setImage(const std::shared_ptr<AV::Image::Image>&);

            std::shared_ptr<Core::IValueSubject<AV::Render2D::ImageOptions> > observeImageOptions() const;
            void setImageOptions(const AV::Render2D::ImageOptions&);

            std::shared_ptr<Core::IValueSubject<glm::vec2> > observeImagePos() const;
            std::shared_ptr<Core::IValueSubject<float> > observeImageZoom() const;
            std::shared_ptr<Core::IValueSubject<UI::ImageRotate> > observeImageRotate() const;
            std::shared_ptr<Core::IValueSubject<UI::ImageAspectRatio> > observeImageAspectRatio() const;
            Core::BBox2f getImageBBox() const;
            void setImagePos(const glm::vec2&, bool animate = false);
            void setImageZoom(float, bool animate = false);
            void setImageZoomFocus(float, const glm::vec2&, bool animate = false);
            void setImageZoomFocus(float, bool animate = false);
            void setImagePosAndZoom(const glm::vec2&, float, bool animate = false);
            void setImageRotate(UI::ImageRotate);
            void setImageAspectRatio(UI::ImageAspectRatio);

            void imageFill(bool animate = false);
            void setImageFrame(const Core::BBox2f&);
            void imageFrame(bool animate = false);
            void imageCenter(bool animate = false);

            std::shared_ptr<Core::IValueSubject<GridOptions> > observeGridOptions() const;
            void setGridOptions(const GridOptions&);

            std::shared_ptr<Core::IValueSubject<AV::Image::Color> > observeBackgroundColor() const;
            void setBackgroundColor(const AV::Image::Color&);

            void setAnnotations(const std::vector<std::shared_ptr<AnnotatePrimitive> >&);

        protected:
            void _preLayoutEvent(Core::Event::PreLayout &) override;
            void _layoutEvent(Core::Event::Layout &) override;
            void _paintEvent(Core::Event::Paint &) override;

        private:
            std::vector<glm::vec3> _getImagePoints() const;
            static glm::vec2 _getCenter(const std::vector<glm::vec3>&);
            static Core::BBox2f _getBBox(const std::vector<glm::vec3>&);

            void _animatePosAndZoom(const glm::vec2&, float);
            void _setPosAndZoom(const glm::vec2&, float);

            DJV_PRIVATE();
        };

    } // namespace ViewApp

    picojson::value toJSON(const ViewApp::GridOptions&);

    //! Throws:
    //! - std::exception
    void fromJSON(const picojson::value&, ViewApp::GridOptions&);

} // namespace djv


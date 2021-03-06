// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvAV/Color.h>

#include <glm/mat4x4.hpp>

#include <memory>

namespace djv
{
    namespace Core
    {
        class Context;
    
    } // namespace Core

    namespace AV
    {
        namespace OpenGL
        {
            class Shader;

        } // namespace OpenGL

        namespace Render3D
        {
            class ILight;

            //! This struct provides binding data.
            struct BindData
            {
                std::vector<std::shared_ptr<ILight> > lights;
            };

            //! This struct provides per-primitive binding data.
            struct PrimitiveBindData
            {
                glm::mat4x4 model;
                glm::mat4x4 camera;
                AV::Image::Color color;
            };

            //! This class provides the base functionality for materials.
            class IMaterial : public std::enable_shared_from_this<IMaterial>
            {
                DJV_NON_COPYABLE(IMaterial);
                
            protected:
                void _init(
                    const std::string& vertex,
                    const std::string& fragment,
                    const std::shared_ptr<Core::Context>&);

                IMaterial();

            public:
                virtual ~IMaterial() = 0;

                virtual void bind(const BindData&);
                virtual void primitiveBind(const PrimitiveBindData&);

                const std::shared_ptr<OpenGL::Shader>& getShader() const;

            protected:
                AV::Image::Color _color;
                std::shared_ptr<OpenGL::Shader> _shader;
            };

            //! This class provides an unlit solid colored material.
            class SolidColorMaterial : public IMaterial
            {
                DJV_NON_COPYABLE(SolidColorMaterial);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);

                SolidColorMaterial();

            public:
                ~SolidColorMaterial() override;

                static std::shared_ptr<SolidColorMaterial> create(const std::shared_ptr<Core::Context>&);

                void primitiveBind(const PrimitiveBindData&) override;

            private:
                std::map<std::string, GLint> _locations;
            };

            //! This enumeration provides the default material modes.
            enum class DefaultMaterialMode
            {
                Default,
                Unlit,
                Normals,
                UVs,

                Count,
                First = Default
            };
            DJV_ENUM_HELPERS(DefaultMaterialMode);

            //! This class provides a default material.
            class DefaultMaterial : public IMaterial
            {
                DJV_NON_COPYABLE(DefaultMaterial);

            protected:
                void _init(const std::shared_ptr<Core::Context>&);

                DefaultMaterial();

            public:
                ~DefaultMaterial() override;

                static std::shared_ptr<DefaultMaterial> create(const std::shared_ptr<Core::Context>&);

                void setMode(DefaultMaterialMode);
                void setAmbient(const AV::Image::Color&);
                void setDiffuse(const AV::Image::Color&);
                void setEmission(const AV::Image::Color&);
                void setSpecular(const AV::Image::Color&);
                void setShine(float);
                void setTransparency(float);
                void setReflectivity(float);
                void setDisableLighting(bool);

                void bind(const BindData&) override;
                void primitiveBind(const PrimitiveBindData&) override;

            private:
                DefaultMaterialMode _mode = DefaultMaterialMode::Default;
                AV::Image::Color _ambient = AV::Image::Color::RGB_F32(0.F, 0.F, 0.4F);
                AV::Image::Color _diffuse = AV::Image::Color::RGB_F32(.5F, .5F, .5F);
                AV::Image::Color _emission = AV::Image::Color::RGB_F32(0.F, 0.F, 0.F);
                AV::Image::Color _specular = AV::Image::Color::RGB_F32(1.F, 1.F, 1.F);
                float _shine = 0.F;
                float _transparency = 0.F;
                float _reflectivity = 0.F;
                bool _disableLighting = false;
                std::map<std::string, GLint> _locations;
            };

        } // namespace Render3D
    } // namespace AV

    DJV_ENUM_SERIALIZE_HELPERS(AV::Render3D::DefaultMaterialMode);

} // namespace djv

#include <djvAV/Render3DMaterialInline.h>

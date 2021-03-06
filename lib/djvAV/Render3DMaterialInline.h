// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace AV
    {
        namespace Render3D
        {
            inline IMaterial::IMaterial()
            {}

            inline IMaterial::~IMaterial()
            {}

            inline void IMaterial::bind(const BindData&)
            {
                // Default implementation does nothing.
            }

            inline void IMaterial::primitiveBind(const PrimitiveBindData&)
            {
                // Default implementation does nothing.
            }

            inline const std::shared_ptr<OpenGL::Shader>& IMaterial::getShader() const
            {
                return _shader;
            }

            inline SolidColorMaterial::SolidColorMaterial()
            {}

            inline SolidColorMaterial::~SolidColorMaterial()
            {}

            inline DefaultMaterial::DefaultMaterial()
            {}

            inline DefaultMaterial::~DefaultMaterial()
            {}

            inline void DefaultMaterial::setMode(DefaultMaterialMode value)
            {
                _mode = value;
            }

            inline void DefaultMaterial::setAmbient(const AV::Image::Color& value)
            {
                _ambient = value;
            }

            inline void DefaultMaterial::setDiffuse(const AV::Image::Color& value)
            {
                _diffuse = value;
            }

            inline void DefaultMaterial::setEmission(const AV::Image::Color& value)
            {
                _emission = value;
            }

            inline void DefaultMaterial::setSpecular(const AV::Image::Color& value)
            {
                _specular = value;
            }

            inline void DefaultMaterial::setShine(float value)
            {
                _shine = value;
            }

            inline void DefaultMaterial::setTransparency(float value)
            {
                _transparency = value;
            }

            inline void DefaultMaterial::setReflectivity(float value)
            {
                _reflectivity = value;
            }

            inline void DefaultMaterial::setDisableLighting(bool value)
            {
                _disableLighting = value;
            }

        } // namespace Render3D
    } // namespace AV
} // namespace djv

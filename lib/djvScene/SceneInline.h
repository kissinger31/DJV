// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

namespace djv
{
    namespace Scene
    {
        inline const std::vector<std::shared_ptr<IPrimitive> >& Scene::getPrimitives() const
        {
            return _primitives;
        }

        inline const std::vector<std::shared_ptr<Layer> >& Scene::getLayers() const
        {
            return _layers;
        }

        inline SceneOrient Scene::getSceneOrient() const
        {
            return _orient;
        }

        inline void Scene::setSceneOrient(SceneOrient value)
        {
            _orient = value;
        }

        inline const glm::mat4x4& Scene::getSceneXForm() const
        {
            return _xform;
        }

        inline void Scene::setSceneXForm(const glm::mat4x4& value)
        {
            _xform = value;
        }

        inline const Core::BBox3f& Scene::getBBox() const
        {
            return _bbox;
        }

        inline const glm::mat4x4& Scene::_getCurrentXForm() const
        {
            return _xforms.size() ? _xforms.back() : _identity;
        }

        inline void Scene::_pushXForm(const glm::mat4x4& value)
        {
            _xforms.push_back(_getCurrentXForm() * value);
        }

        inline void Scene::_popXForm()
        {
            _xforms.pop_back();
        }

    } // namespace Scene
} // namespace djv

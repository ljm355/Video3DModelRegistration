#include "CameraEventHandler.h"
#include <osgViewer/View>


CameraEventHandler::CameraEventHandler(CameraParams* cameraParams, CameraMarker* cameraMarker)
{
	_cameraParams = cameraParams;
	//_keydownmap[osgGA::GUIEventAdapter::KEY_A] = false;
	//_keydownmap[osgGA::GUIEventAdapter::KEY_E] = false;
	//_keydownmap[osgGA::GUIEventAdapter::KEY_H] = false;
	//_keydownmap[osgGA::GUIEventAdapter::KEY_D] = false;
	//_keydownmap[osgGA::GUIEventAdapter::KEY_Up] = false;
	//_keydownmap[osgGA::GUIEventAdapter::KEY_Down] = false;
	//_keydownmap[osgGA::GUIEventAdapter::KEY_Left] = false;
	//_keydownmap[osgGA::GUIEventAdapter::KEY_Right] = false;
	_cameraMarker = cameraMarker;
}

bool CameraEventHandler::handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
{

	if (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
	{
		_keydownmap[ea.getKey()] = true;
	}
	if (ea.getEventType() == osgGA::GUIEventAdapter::KEYUP)
	{
		_keydownmap[ea.getKey()] = false;
	}
	if (ea.getEventType() == osgGA::GUIEventAdapter::KEYDOWN)
	{
		printf("%d\n", ea.getKey());
		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Minus)
		{
			if (_keydownmap[osgGA::GUIEventAdapter::KEY_A] == true)
			{
				if (_cameraParams->azimuth <= 0)
					_cameraParams->azimuth = 360;
				_cameraParams->azimuth -= 1;
			}
			else if (_keydownmap[osgGA::GUIEventAdapter::KEY_E] == true)
			{
				if (_cameraParams->elevation > 0)
					_cameraParams->elevation -= 1;
			}
			else if (_keydownmap[osgGA::GUIEventAdapter::KEY_Z] == true)
			{
				_cameraParams->height -= 0.1;
			}
			else if (_keydownmap[osgGA::GUIEventAdapter::KEY_D] == true)
			{
				_cameraParams->distance2object -= 1;
			}
			else if (_keydownmap[osgGA::GUIEventAdapter::KEY_H] == true)
			{
				if (_cameraParams->fovx < 180)
					_cameraParams->fovx += 1;
			}
			else if (_keydownmap[osgGA::GUIEventAdapter::KEY_V] == true)
			{
				if (_cameraParams->fovy < 180)
					_cameraParams->fovy += 1;
			}
		}
		else if (ea.getKey() == 61 || ea.getKey() == 64451)
		{
			if (_keydownmap[osgGA::GUIEventAdapter::KEY_A] == true)
			{
				if (_cameraParams->azimuth >= 360)
					_cameraParams->azimuth = 0;
				_cameraParams->azimuth += 1;
			}
			else if (_keydownmap[osgGA::GUIEventAdapter::KEY_E] == true)
			{
				if (_cameraParams->elevation < 90)
					_cameraParams->elevation += 1;
			}
			else if (_keydownmap[osgGA::GUIEventAdapter::KEY_Z] == true)
			{
				_cameraParams->height += 0.1;
			}
			else if (_keydownmap[osgGA::GUIEventAdapter::KEY_D] == true)
			{
				_cameraParams->distance2object += 1;
			}
			else if (_keydownmap[osgGA::GUIEventAdapter::KEY_H] == true)
			{
				if (_cameraParams->fovx > 0)
					_cameraParams->fovx -= 1;
			}
			else if (_keydownmap[osgGA::GUIEventAdapter::KEY_V] == true)
			{
				if (_cameraParams->fovy > 0)
					_cameraParams->fovy -= 1;
			}
		}

		if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Up)
		{
			_cameraParams->pos.y() += 0.1;
		}
		else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Down)
		{
			_cameraParams->pos.y() -= 0.1;
		}
		else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Left)
		{
			_cameraParams->pos.x() += 0.1;
		}
		else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_Right)
		{
			_cameraParams->pos.x() -= 0.1;
		}
	}
	if (ea.getEventType() == osgGA::GUIEventAdapter::DOUBLECLICK)
	{
		osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
		if (!view)
			return false;
		osgUtil::LineSegmentIntersector::Intersections intersections;
		if (view->computeIntersections(ea, intersections))
		{
			osg::NodePath npath = intersections.begin()->nodePath;
			osg::Node* firstnode = *npath.begin();
			osg::Vec3d pos = intersections.begin()->getWorldIntersectPoint();
			_cameraParams->pos = pos;
		}
	}
	_cameraParams->setMatrix();
	_cameraMarker->updateMatrix();
	if (ea.getEventType() == osgGA::GUIEventAdapter::KEYUP && ea.getKey() == osgGA::GUIEventAdapter::KEY_V)
	{
		printf("save camera configuration to: %s\n", _cameraParams->_filename.data());
		_cameraParams->save();
	}
	return false;
}
CameraEventHandler::~CameraEventHandler()
{

}

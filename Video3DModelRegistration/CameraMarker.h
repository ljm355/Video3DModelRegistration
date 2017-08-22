#pragma once
#include <Windows.h>
#include "osg/ClampColor"
#include "osg/Depth"
#include "osgUtil/SmoothingVisitor"
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Group>
#include "CameraParams.h"
class CameraMarker : public osg::Group
{
public:
	CameraMarker(CameraParams* cameraParams);
	void updateMatrix();
	~CameraMarker();
	void bindShader(osg::StateSet * stateset, osg::Vec4 color);
	osg::MatrixTransform * createLine(osg::Vec3d center, osg::Vec3d dir, double len, int width, osg::Vec4 color);
	osg::MatrixTransform * createSphere(osg::Vec3d center, osg::Vec4 color, double radius);
private:
	osg::ref_ptr<osg::MatrixTransform> _lineofsight;
	osg::ref_ptr<osg::MatrixTransform> _cameraPos;
	osg::ref_ptr<osg::MatrixTransform> _targetPos;
	CameraParams* _cameraParams;
};


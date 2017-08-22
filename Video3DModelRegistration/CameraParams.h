#pragma once
#include <Windows.h>
#include <osg/Camera>


struct CameraParamsConfig
{
public:
	osg::Vec3d pos;
	double height;
	double azimuth;
	double elevation;
	double distance2object;
	double fovy;
	double fovx;
	double znear;
	double zfar;
};

class CameraParams : public CameraParamsConfig
{
public:
	osg::Camera* _camera;
	std::vector<osg::Uniform*> _matViewProjUniforms;
	osg::Vec3d _cameraPos;
	osg::Vec3d _targetPos;
	osg::Vec3d _cameraDir;
	osg::Vec3d getCameraPos();
	std::string _filename;
	CameraParams(osg::Camera* camera,std::vector<osg::Uniform*> matViewProjUniforms)
	{
		_camera = camera;
		_matViewProjUniforms = matViewProjUniforms;
		fovx = 45;
		fovy = 45;
		znear = 0.5;
		zfar = 10000;
		distance2object = 100;
		height = 0;
		azimuth = 0;
		elevation = 45;
		setMatrix();
	}
	osg::Matrix getCombinedMatrix();
	void setMatrix();
	bool save(std::string filename = "");
	bool open(std::string filename = "");
};
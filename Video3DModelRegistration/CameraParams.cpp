#include "CameraParams.h"
#include <fstream>
osg::Vec3d CameraParams::getCameraPos()
{
	return osg::Vec3d();
}

osg::Matrix CameraParams::getCombinedMatrix()
{
	return _camera->getViewMatrix() * _camera->getProjectionMatrix();
}

void CameraParams::setMatrix()
{
	double elevationAngle = osg::DegreesToRadians(elevation);
	double azimuthAngle = osg::DegreesToRadians(azimuth);
	osg::Vec3d vforward(0, cos(elevationAngle), -sin(elevationAngle));
	osg::Matrix rotmat = osg::Matrix::rotate(azimuthAngle, osg::Vec3(0, 0, -1));
	vforward = vforward * rotmat;
	_targetPos = pos;
	_targetPos.z() += height;
	_cameraPos = _targetPos - vforward * distance2object;
	//_dir.normalize();
	osg::Vec3 crossdir = -vforward ^ osg::Vec3(0, 0, -1);
	osg::Vec3d up = -vforward ^ crossdir;
	_camera->setViewMatrixAsLookAt(_cameraPos, _targetPos, up);
	double aspectRatio = fovx / fovy;
	_camera->setProjectionMatrixAsPerspective(fovy, aspectRatio, znear, zfar);
	osg::Matrix matViewProject = getCombinedMatrix();
	for (size_t i = 0; i < _matViewProjUniforms.size(); i++)
	{
		_matViewProjUniforms[i]->set(matViewProject);
	}
	_cameraDir = vforward;
}

bool CameraParams::save(std::string filename)
{
	if (filename == "")
		filename = _filename;
	std::ofstream ofs;
	ofs.open(filename, std::ios::binary);
	if (!ofs)
	{
		ofs.close();
		return false;
	}
	ofs.write((char*)((CameraParamsConfig*)this), sizeof(CameraParamsConfig));
	ofs.close();
	return true;
}

bool CameraParams::open(std::string filename)
{
	if (filename == "")
		filename = _filename;
	std::ifstream ifs;
	ifs.open(filename, std::ios::binary);
	if (!ifs)
	{
		ifs.close();
		return false;
	}
	ifs.read((char*)((CameraParamsConfig*)this), sizeof(CameraParamsConfig));
	ifs.close();
	return true;
}

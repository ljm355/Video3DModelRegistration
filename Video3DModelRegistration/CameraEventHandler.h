#pragma once
#include <Windows.h>
#include <osgGA/GUIEventHandler>
#include "CameraParams.h"
#include "CameraMarker.h"
class CameraEventHandler :
	public osgGA::GUIEventHandler
{
public:
	CameraEventHandler(CameraParams* cameraParams, CameraMarker* cameraMarker);
	~CameraEventHandler();
	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa);
private:
	std::map<int, bool> _keydownmap;
	CameraParams* _cameraParams;
	CameraMarker* _cameraMarker;
};


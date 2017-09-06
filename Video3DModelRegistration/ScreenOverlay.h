#pragma once
#include <Windows.h>
#include <osg/Texture2D>
#include <osg/Geometry>
#include <osg/Camera>
#include <osg/Geode>
#include <osgViewer/Viewer>
class ScreenOverlay : public osg::Geode
{
public:
	ScreenOverlay(osgViewer::Viewer* viewer);
	~ScreenOverlay();
	void set3DModelTextureLayer(osg::Texture* tex);
	void setVideoCameraTextureLayer(std::string filename);
	osg::ref_ptr<osg::Texture2D> _videoTexture;
};


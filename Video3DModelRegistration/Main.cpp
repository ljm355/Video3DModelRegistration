
#include <Windows.h>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <osgViewer/viewer>
#include <osg/MatrixTransform>
#include <osg/TextureCubeMap>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/SphericalManipulator>
#include <osg/GLExtensions>
#include <osgViewer/Renderer>
#include <osgGA/TrackballManipulator>
#include <osgDB/WriteFile>
#include <osgViewer/ViewerEventHandlers>
#include <opencv2/core/core.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/video/tracking.hpp>
#include "osg/ComputeBoundsVisitor"
#include "CameraBuffer.h"
#include "CameraParams.h"
#include "CameraEventHandler.h"
#include "ScreenOverlay.h"
#include "CameraMarker.h"
#include "ModelLoader.h"
using namespace cv;
osg::ref_ptr<CameraBuffer> _CameraBuffer;
osg::ref_ptr<osg::Group> _3DModelWrapperMainScreen;
osg::ref_ptr<osg::Group> _3DModelWrapperVideoCameraScreen;
osg::ref_ptr<ScreenOverlay> _VideoCameraScreenOverlay;
CameraParams* _cameraParams;

void decodeVideo(std::string videofile, std::string outdir)
{
	VideoCapture cap;                          
	
	// instantiate VideoCapture
												//std::string videofile = "E:/Video3DModelRegistration/JiashanVideo/LX473/20170717-142134-001.mp4";
												//std::string outdir = "E:/Video3DModelRegistration/JiashanVideo/LX473/";
	//std::string videofile = "E:/Video3DModelRegistration/JiashanVideo/WT909/20170717100000-20170717110150.mp4";
	//std::string outdir = "E:/Video3DModelRegistration/JiashanVideo/WT909/";
	//WT909/20170717100000-20170717110150
	cap.open(videofile);                      // open a recorded video

	if (!cap.isOpened())   // check if we succeeded
	{
		//cout << "Could not open the camera device" << endl;
		return;
	}
	Mat frame, frame_vis;
	//std::vector<int> compression_params;
	//compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	//compression_params.push_back(9);
	std::vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
	compression_params.push_back(50);

	int framenum = 0;
	while (cap.read(frame) && waitKey(30) != 27) // capture frame until ESC is pressed
	{
		if (framenum % 100 == 0)
		{
			std::stringstream ss;
			ss << outdir << framenum << ".jpg";
			imwrite(ss.str(), frame, compression_params);
		}

		framenum++;
	}
}
osg::Program* create3DModelShadersForMainScreen()
{

	osg::Program* program = new osg::Program;
	char vertexSource[] =
		"varying vec4 position;\n"
		"void main(void)\n"
		"{\n"
		"position = gl_Vertex;\n"
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"gl_TexCoord[0] = gl_MultiTexCoord0;\n"
		"}\n";
	char fragmentSource[] =
		"uniform sampler2D texture0;\n"
		"uniform sampler2D texture1;\n"
		"varying vec4 position;\n"
		"uniform mat4 videoMatrix; \n"
		"void main(void) \n"
		"{\n"
		"    vec4 color   = texture2D( texture0, gl_TexCoord[0].xy );\n"
		"    vec4 videoPos4 = videoMatrix * position;\n"
		"    vec3 videoPos3 = videoPos4.xyz / videoPos4.w;\n"
		"    if(videoPos4.w > 0 && videoPos3.x > -1 && videoPos3.x < 1 && videoPos3.y > -1 && videoPos3.y < 1)"
		"        color = color + vec4(0.5,0,0,0);\n"
		"    gl_FragColor = color;\n"
		"}\n";

	//geode->setCullCallback(new GeomCB);
	program->setName("create3DModelShadersForMainScreen");
	program->addShader(new osg::Shader(osg::Shader::VERTEX, vertexSource));
	program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragmentSource));
	return program;
}
osg::Program* create3DModelShadersForVideoCameraScreen()
{

	osg::Program* program = new osg::Program;
	char vertexSource[] =
		"void main(void)\n"
		"{\n"
		"gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
		"gl_TexCoord[0] = gl_MultiTexCoord0;\n"
		"}\n";
	char fragmentSource[] =
		"uniform sampler2D texture0;\n"
		"void main(void) \n"
		"{\n"
		"    vec4 color   = texture2D( texture0, gl_TexCoord[0].xy );\n"
		"    gl_FragColor = color;\n"
		"}\n";

	//geode->setCullCallback(new GeomCB);
	program->setName("3DModelShadersForVideoCameraScreen");
	program->addShader(new osg::Shader(osg::Shader::VERTEX, vertexSource));
	program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragmentSource));
	return program;
}
//--------------------------------------------------------------------------
int main(int argc, char **argv)
{

	osgViewer::Viewer viewer;
	viewer.setUpViewAcrossAllScreens();
	viewer.setUpViewInWindow(50, 50, 1024, 768);
	osg::ref_ptr<osg::Node> city = ModelLoader::Load3DTiles("./Xianzhengfu/data/");
	_3DModelWrapperMainScreen = new osg::Group;
	_3DModelWrapperMainScreen->addChild(city.get());
	_3DModelWrapperMainScreen->getOrCreateStateSet()->addUniform(new osg::Uniform("texture0", 0));
	_3DModelWrapperMainScreen->getOrCreateStateSet()->addUniform(new osg::Uniform(osg::Uniform::FLOAT_MAT4, "videoMatrix"));
	_3DModelWrapperMainScreen->getOrCreateStateSet()->setAttributeAndModes(create3DModelShadersForMainScreen(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	_3DModelWrapperVideoCameraScreen = new osg::Group;
	_3DModelWrapperVideoCameraScreen->addChild(city.get());
	_3DModelWrapperVideoCameraScreen->getOrCreateStateSet()->addUniform(new osg::Uniform("texture0", 0));
	_3DModelWrapperVideoCameraScreen->getOrCreateStateSet()->setAttributeAndModes(create3DModelShadersForVideoCameraScreen(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	osg::ref_ptr<osg::Group> root = new osg::Group;
	std::vector<osg::GraphicsContext*> contexts;
	viewer.getContexts(contexts);
	_CameraBuffer = CameraBuffer::createSlave(512, 512, contexts[0]);
	viewer.addSlave(_CameraBuffer.get(),false);
	_CameraBuffer->addChild(_3DModelWrapperVideoCameraScreen.get());


	std::vector<osg::Uniform*> matViewProjUniforms;
	matViewProjUniforms.push_back(_3DModelWrapperMainScreen->getOrCreateStateSet()->getUniform("videoMatrix"));

	_cameraParams = new CameraParams(_CameraBuffer, matViewProjUniforms);
	_cameraParams->_filename = "./camera.data";
	if (!_cameraParams->open())
	{
		osg::ComputeBoundsVisitor cbs;
		city->accept(cbs);
		osg::BoundingBox bb = cbs.getBoundingBox();
		_cameraParams->pos = osg::Vec3d(bb.center().x(), bb.center().y(), bb.center().z() + 5);
		_cameraParams->setMatrix();
	}


	osg::ref_ptr<CameraMarker> _marker = new CameraMarker(_cameraParams);
	CameraEventHandler* eventHandler = new CameraEventHandler(_cameraParams, _marker);

	viewer.addEventHandler(eventHandler);

	_VideoCameraScreenOverlay = new ScreenOverlay(&viewer);
	_VideoCameraScreenOverlay->set3DModelTextureLayer(_CameraBuffer->_texture.get());
	_VideoCameraScreenOverlay->setVideoCameraTextureLayer("0.jpg");

	root->addChild(_marker.get());
	root->addChild(_3DModelWrapperMainScreen.get());
	root->addChild(_VideoCameraScreenOverlay.get());
	viewer.setSceneData(root.get());
	viewer.setThreadingModel(osgViewer::ViewerBase::ThreadingModel::ThreadPerContext); 

	// add the state manipulator
	viewer.addEventHandler( new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()) );
	viewer.setCameraManipulator(new osgGA::TrackballManipulator);
	// add the thread model handler
	viewer.addEventHandler(new osgViewer::ThreadingHandler);

	// add the window size toggle handler
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);

	// add the stats handler
	viewer.addEventHandler(new osgViewer::StatsHandler);

	// add the LOD Scale handler
	viewer.addEventHandler(new osgViewer::LODScaleHandler);

	// add the screen capture handler
	viewer.addEventHandler(new osgViewer::ScreenCaptureHandler);

	//viewer.realize();

   return viewer.run();
}




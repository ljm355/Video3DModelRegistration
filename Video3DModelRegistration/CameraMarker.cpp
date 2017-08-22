#include "CameraMarker.h"
#include "osg/ComputeBoundsVisitor"
#include "osgGA/StateSetManipulator"
#include "osgViewer/ViewerEventHandlers"
#include "osg/MatrixTransform"
#include "osg/Texture2D"
#include "osg/Image"
#include "osgDB/writeFile"
#include "osgDB/readFile"
#include "osg/ClampColor"
#include "osg/Depth"
#include <osg/ShapeDrawable>
#include "osg/LineWidth"


CameraMarker::CameraMarker(CameraParams* cameraParams)
{
	osg::Vec3d centerofline = (cameraParams->_cameraPos + cameraParams->_targetPos) * 0.5;
	double len = (cameraParams->_targetPos - cameraParams->_cameraPos).length();
	_lineofsight = createLine(centerofline, cameraParams->_cameraDir, len, 5, osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
	_cameraPos = createSphere(cameraParams->_cameraPos, osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f),1.0);
	 _targetPos = createSphere(cameraParams->_targetPos, osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f), 1.0);
	 this->addChild(_lineofsight.get());
	 this->addChild(_cameraPos.get());
	 this->addChild(_targetPos.get());
	 _cameraParams = cameraParams;
}

void CameraMarker::updateMatrix()
{
	osg::Vec3d centerofline = (_cameraParams->_cameraPos + _cameraParams->_targetPos) * 0.5;
	double len = (_cameraParams->_targetPos - _cameraParams->_cameraPos).length();
	_lineofsight->setMatrix(osg::Matrix::scale(osg::Vec3d(len, len, len)) * osg::Matrix::rotate(osg::Vec3d(0, 0, 1), _cameraParams->_cameraDir) * osg::Matrix::translate(centerofline));
	_cameraPos->setMatrix(osg::Matrix::translate(_cameraParams->_cameraPos));
	_targetPos->setMatrix(osg::Matrix::translate(_cameraParams->_targetPos));
}

CameraMarker::~CameraMarker()
{

}
void CameraMarker::bindShader(osg::StateSet* stateset, osg::Vec4 color)
{
	osg::ref_ptr<osg::Program> program = new osg::Program;

	/*	char vertexShaderSource[] =
	"void main(void)\n"
	"{\n"
	"   gl_Position    = gl_ModelViewProjectionMatrix *  gl_Vertex;\n"
	"}\n"*/;
	char fragmentShaderSource[] =
		"uniform vec4 color; \n"
		"void main(void) \n"
		"{\n"
		"     gl_FragColor = color;\n"
		"}\n";

	program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragmentShaderSource));
	//program->addShader(new osg::Shader(osg::Shader::VERTEX, vertexShaderSource));
	stateset->getOrCreateUniform("color", osg::Uniform::FLOAT_VEC4)->set(color);
	stateset->setAttribute(program.get(), osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
}

osg::MatrixTransform* CameraMarker::createLine(osg::Vec3d center, osg::Vec3d dir, double len, int width, osg::Vec4 color)
{
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> geom = new osg::Geometry;
	osg::ref_ptr<osg::Vec3dArray> vertices = new osg::Vec3dArray;
	vertices->push_back(osg::Vec3(0, 0, -0.5 ));
	vertices->push_back(osg::Vec3(0, 0, 0.5));
	geom->setVertexArray(vertices.get());
	geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, vertices->size()));
	bindShader(geom->getOrCreateStateSet(), color);
	osg::ref_ptr<osg::LineWidth> linewidth = new osg::LineWidth();
	linewidth->setWidth(width);
	geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	geom->getOrCreateStateSet()->setAttributeAndModes(linewidth.get(), osg::StateAttribute::OVERRIDE | osg::StateAttribute::ON);
	geom->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
	geode->addDrawable(geom.get());
	osg::MatrixTransform* transform = new osg::MatrixTransform;
	transform->setMatrix(osg::Matrix::scale(osg::Vec3d(len, len, len)) * osg::Matrix::rotate(osg::Vec3d(0, 0, 1), dir) * osg::Matrix::translate(center));
	transform->addChild(geode.get());
	return transform;
}

osg::MatrixTransform* CameraMarker::createSphere(osg::Vec3d center, osg::Vec4 color,double radius)
{
	osg::MatrixTransform* transform = new osg::MatrixTransform;
	// Create a sphere centered at the origin, unit radius: 
	osg::ref_ptr<osg::Sphere> unitSphere = new osg::Sphere(osg::Vec3(0, 0, 0), radius);
	osg::ref_ptr<osg::ShapeDrawable> unitSphereDrawable = new osg::ShapeDrawable(unitSphere.get());
	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	bindShader(geode->getOrCreateStateSet(), color);
	geode->addChild(unitSphereDrawable.get());
	geode->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	transform->addChild(geode.get());
	transform->setMatrix( osg::Matrix::translate(center));
	return transform;
}
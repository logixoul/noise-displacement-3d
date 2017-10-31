#include "precompiled.h"
#include "ciextra.h"
#include "util.h"
#include "PerlinCalculator2D.h"

typedef Array2D<Vec3f> Image;
int sx = 50, sy = 50;
float wscale=2.0f; 
gl::Texture tex;
float mouseX, mouseY;
float counter = 0;
gl::GlslProg shader;
bool pause = false;


Array2D<Vec3f> normals;
struct Triangle
{
	Vec3f a, b, c;
	Vec3f a0, b0, c0;
	Vec2i ia, ib, ic;
	Triangle(Vec3f const& a, Vec3f const& b, Vec3f const& c,
		Vec3f a0, Vec3f b0, Vec3f c0, Vec2i ia, Vec2i ib, Vec2i ic) : a(a), b(b), c(c), a0(a0), b0(b0), c0(c0), ia(ia), ib(ib), ic(ic)
	{

	}
};
vector<Triangle> triangles2;
Array2D<Triangle> triangles;
Image img2(sx, sy);
gl::Texture NO_DEPTH_TEX;
gl::Texture maketex(int w, int h, GLint format, GLint internalFormat) {
	gl::Texture::Format fmt; fmt.setInternalFormat(format); return gl::Texture(NULL, format, w, h, fmt);
}
#define checkGL() \
{\
	auto error=glGetError();\
	if(error!=GL_NO_ERROR){\
		cout<<"["<<__LINE__<<"] GL ERROR "<<error<<endl;\
		/*system("PAUSE");*/\
	}\
}
void beginRTT(gl::Texture fbotex, gl::Texture& depthtex=NO_DEPTH_TEX)
{
	static unsigned int fboid = 0;
	if(fboid == 0)
	{
		glGenFramebuffersEXT(1, &fboid);
		checkGL();
	}
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, fboid);
	checkGL();
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, fbotex.getId(), 0);
	checkGL();
	if(&depthtex != &NO_DEPTH_TEX)
	{
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, depthtex.getId(), 0);
		checkGL();
	}
	else
	{
		glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);
		checkGL();
	}
}
void endRTT()
{
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, 0, 0);
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_TEXTURE_2D, 0, 0);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
}
struct SApp : AppBasic {
	ci::Timer timer;
	PerlinCalculator2D perlinCalculator2D;
	PerlinCalculator2D perlinCalculator2D2;
	ci::params::InterfaceGl* params;
	Vec3f cameraPos;
	Vec3f camDir;bool absDir;
	void setup()
	{
		timer.start();
		createConsole();
		cout << fmod(-1.5f, 1.0f) << endl;
		
		gl::Texture::Format fmt;
		fmt.setInternalFormat(GL_RGBA16F);
		tex = gl::Texture(sx, sy, fmt);
		shader = gl::GlslProg(loadFile("shader.vs"), loadFile("shader.fs"));
		setWindowSize(600, 400);
		//setFullScreen(true);
		params = new ci::params::InterfaceGl("abc", Vec2i(200, 300));
		params->addParam("camx",&cameraPos.x,"");
		params->addParam("camy",&cameraPos.y,"");
		params->addParam("camz",&cameraPos.z,"");
		/*params->addParam("dirx",&cameraPos.x,"");
		params->addParam("diry",&cameraPos.y,"");
		params->addParam("dirz",&cameraPos.z,"");*/
		params->addParam("dir", &camDir,"");
		camDir=-Vec3f::zAxis();
		absDir=false;
		params->addParam("dir is abs", &absDir, "");
		cameraPos.z = 30;
	}
	void mouseDown(MouseEvent e)
	{
	}
	void keyDown(KeyEvent e) {
		if(e.getChar() == 'p' || e.getChar() == '2')
			pause = !pause;
		if(e.getCode() == KeyEvent::KEY_ESCAPE)
		{
			std::exit(0);
		}
	}
	static float s1(float f) { return sin(f) * .5f + .5f; }
	static float c1(float f) { return cos(f) * .5f + .5f; }
	float fog()
	{
		float val = .0f;
		Vec3f _out = Vec3f::one() * val;
		float luma = dot(_out, Vec3f(.2f, .7f, .1f));
		float luma2 = luma / (luma + 1.0f);
		_out *= luma2 / luma;
		return pow(_out.ptr()[0], 1.0f / 2.2f);
	}
	void draw()
	{
		cout<<"draw"<<endl;
		ci::Timer timer(true);
		mouseX = getMousePos().x/(float)AppBasic::get()->getWindowWidth();
		mouseY = getMousePos().y/(float)AppBasic::get()->getWindowHeight();
		
		if(!pause)
			counter += 10.0*.6f*.01f / 16.0f;

		glClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FALSE);
		glClampColorARB(GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE);
		glClearColor(fog(), fog(), fog(), 1.0f);
		glClearDepth(1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Image img(sx, sy);
		forxy(img)
		{
			float fscale = .5f;
			float noise = perlinCalculator2D.calcAt(Vec3f(fscale * p.x/(float)sx, fscale * p.y/(float)sy, counter));
			img(p) = Vec3f::one() * (noise * .5f + .5f);
		}
		forxy(img)
		{
			
			img2(p) = img(p);
			img2(p) *= h();
		}
		tex.bind();
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, sx, sy, GL_RGB, GL_FLOAT, img2.data);
		gl::setMatricesWindow(getWindowSize());
		tex.setWrap(GL_REPEAT, GL_REPEAT);
		CameraPersp camera;
		float t = 0.0f * .1f*sin(1.1f*1.f*getElapsedFrames()) < 0.0f ? 0.1f : 0.0f;
		//camera.lookAt(Vec3f(sx/(float)2.0f, sy/(float)2.0f+.6*sy*0.0f, 0.0f), Vec3f(sx/2, sy/2, -1000.0f), Vec3f::zAxis());
		//camera.setEyePoint(camera.getEyePoint() + cameraPos);
		auto eyePos_=Vec3f(sx/(float)2.0f, sy/(float)2.0f+.6*sy*0.0f, 0.0f)+cameraPos;
		camera.lookAt(
			eyePos_,
			absDir ? camDir : (eyePos_+camDir),
			Vec3f::zAxis());
		//camera.lookAt(cameraPos, Vec3f(sx/2, sy/2, 0.0f), -Vec3f::zAxis());
		camera.setAspectRatio(getWindowAspectRatio());
		camera.setFov(90.0f); // degrees
		vector<float> dists;
		dists += -camera.getModelViewMatrix().transformPointAffine(Vec3f(0.0f, 0.0f, 0.0f)).z;
		dists += -camera.getModelViewMatrix().transformPointAffine(Vec3f(sx, 0.0f, 0.0f)).z;
		dists += -camera.getModelViewMatrix().transformPointAffine(Vec3f(0.0f, sy, 0.0f)).z;
		dists += -camera.getModelViewMatrix().transformPointAffine(Vec3f(sx, sy, 0.0f)).z;
		dists += -camera.getModelViewMatrix().transformPointAffine(Vec3f(0.0f, 0.0f, 50.0f)).z;
		dists += -camera.getModelViewMatrix().transformPointAffine(Vec3f(sx, 0.0f, 50.0f)).z;
		dists += -camera.getModelViewMatrix().transformPointAffine(Vec3f(0.0f, sy, 50.0f)).z;
		dists += -camera.getModelViewMatrix().transformPointAffine(Vec3f(sx, sy, 50.0f)).z;
		cout << "*std::min_element(dists.begin(),dists.end()) "<<*std::min_element(dists.begin(),dists.end())<<endl;
		cout << "*std::max_element(dists.begin(),dists.end()) "<<*std::max_element(dists.begin(),dists.end())<<endl;
		//camera.setNearClip(5 * *std::min_element(dists.begin(),dists.end()));
		//camera.setFarClip(5 * *std::max_element(dists.begin(),dists.end()));
		shader.bind();
		shader.uniform("tex", 0); tex.bind(0);
		shader.uniform("mouse", Vec2f(mouseX, mouseY));
		shader.uniform("time", (float)getElapsedFrames());
		shader.uniform("viewportSize", (Vec2f)getWindowSize());
		gl::setMatrices(camera);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		//glEnable(GL_CULL_FACE);
		glDisable(GL_CULL_FACE);
		triangles2.clear();
		for(int x = 0; x < sx - 1; x++)
		{
			for(int y = 0; y < sy - 1; y++)
			{
				Vec2i index(x, y);
				triangles2.push_back(getTriangle(Vec2i(x, y + 1), Vec2i(x, y), Vec2i(x + 1, y)));
				triangles2.push_back(getTriangle(Vec2i(x, y + 1), Vec2i(x + 1, y), Vec2i(x + 1, y + 1)));
			}
		}
		calcNormals();
		for(int i = 0; i < 0; i++){
		foreach(auto& triangle, triangles2)
		{
			float fscale = pow(.8f, i);
			float anoise = .3f * 5.0f * perlinCalculator2D2.calcAt(Vec3f(fscale*triangle.a0.x/(float)sx, fscale*triangle.a0.y/(float)sy, counter));
			float bnoise = .3f * 5.0f * perlinCalculator2D2.calcAt(Vec3f(fscale*triangle.b0.x/(float)sx, fscale*triangle.b0.y/(float)sy, counter));
			float cnoise = .3f * 5.0f * perlinCalculator2D2.calcAt(Vec3f(fscale*triangle.c0.x/(float)sx, fscale*triangle.c0.y/(float)sy, counter));
			
			float factor=3.0f;
			triangle.a += factor*normals(triangle.ia) * anoise;
			triangle.b += factor*normals(triangle.ib) * bnoise;
			triangle.c += factor*normals(triangle.ic) * cnoise;
		}
		calcNormals();
		}
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBegin(GL_TRIANGLES);

		foreach(auto& triangle, triangles2)
		{
			glNormal3f(normals(triangle.ia));
			glVertex3f(triangle.a);
			glNormal3f(normals(triangle.ib));
			glVertex3f(triangle.b);
			glNormal3f(normals(triangle.ic));
			glVertex3f(triangle.c);
		}
		glEnd();
		
		shader.unbind();
		glDisable(GL_DEPTH_TEST);
		gl::setMatricesWindow(getWindowSize());
		gl::enableAlphaBlending();
		
		gl::drawString(ToString((int)getAverageFps()) + " FPS", Vec2f::zero());
		gl::GlslProg::unbind();

		params->draw();
	}
	void _draw() {
		gl::Fbo::Format fbofmt;
		fbofmt.enableColorBuffer();
		//fbofmt.enableDepthBuffer();
		gl::Fbo fbo(getWindowWidth(), getWindowHeight(), fbofmt);
		fbo.bindFramebuffer();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//fbo.unbindFramebuffer();
		gl::Fbo::unbindFramebuffer();
	}
	string loadFile_(string filename) {
		string buffer;
		std::ifstream file(filename.c_str(), std::ios::in|std::ios::binary|std::ios::ate);
		if(!file) throw new runtime_error("file doesn't exist");
		//get filesize
		std::streamsize size = 0;
		if(file.seekg(0, std::ios::end).good()) size = file.tellg();
		if(file.seekg(0, std::ios::beg).good()) size -= file.tellg();

		//read contents of the file into the vector
		if(size > 0)
		{
			buffer.resize((size_t)size);
			file.read((char*)(&buffer[0]), size);
		}
		else buffer.clear();
		return buffer;
	}
	void calcNormals()
	{
		normals = Array2D<Vec3f>(sx, sy);
		
		foreach(auto& triangle, triangles2)
		{
			auto n6 = getNormal(triangle);
			normals(triangle.ia) += n6;
			normals(triangle.ib) += n6;
			normals(triangle.ic) += n6;
		}
		forxy(normals)
		{
			normals(p) = normals(p).safeNormalized();
		}
	}
	float h() { return 10.0f; }
		
	Vec3f transform(Vec3f vec)
	{
		Vec3f a = vec * Vec3f(1.0f, 1.0f, 0.0f);
		float z = vec.z;//pow(vec.z/(float)h(), .1f);
		Vec2f n = (a.xy() - Vec2f(sx / 2, sy / 2)) / Vec2f(sx / 2, sy / 2);
		float c = 10.0f * ((sx/(float)2.0f) / 3.0f) * pow(z / 60.0f, 2.0f);
		Vec3f b = a;
		return a + Vec3f(n.x * c, n.y * c, z);
	}
	Triangle getTriangle(Vec2i ai, Vec2i bi, Vec2i ci)
	{
		Vec3f a(ai.x, ai.y, img2(ai).ptr()[0]);
		Vec3f b(bi.x, bi.y, img2(bi).ptr()[0]);
		Vec3f c(ci.x, ci.y, img2(ci).ptr()[0]);
		return Triangle(transform(a), transform(b), transform(c), a, b, c, ai, bi, ci);
	}
	Vec3f getNormal(Triangle t)
	{
		return (t.b-t.c).cross(t.b-t.a).normalized();
	}
};

//CINDER_APP_BASIC(SApp, RendererGl)
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow) {	
	try{
		cinder::app::AppBasic::prepareLaunch();														
		cinder::app::AppBasic *app = new SApp;														
		cinder::app::Renderer *ren = new RendererGl;													
		cinder::app::AppBasic::executeLaunch( app, ren, "SApp" );										
		cinder::app::AppBasic::cleanupLaunch();														
		return 0;																					
	}catch(ci::gl::GlslProgCompileExc const& e) {
		cout << "caught: " << endl << e.what() << endl;
		//int dummy;cin>>dummy;
		system("pause");

	}catch(std::exception const& e) {
		cout << "caught: " << endl << e.what() << endl;
		//int dummy;cin>>dummy;
		system("pause");
	}
}
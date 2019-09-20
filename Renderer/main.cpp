#include "context.h"				  
#include "render.h"					  
#include "glm/glm.hpp"				  
#include "glm/ext.hpp"	
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

typedef struct {
	double r;       // a fraction between 0 and 1
	double g;       // a fraction between 0 and 1
	double b;       // a fraction between 0 and 1
} rgb;

typedef struct {
	double h;       // angle in degrees
	double s;       // a fraction between 0 and 1
	double v;       // a fraction between 0 and 1
} hsv;

static hsv   rgb2hsv(rgb in);
static rgb   hsv2rgb(hsv in);

hsv rgb2hsv(rgb in)
{
	hsv         out;
	double      min, max, delta;

	min = in.r < in.g ? in.r : in.g;
	min = min < in.b ? min : in.b;

	max = in.r > in.g ? in.r : in.g;
	max = max > in.b ? max : in.b;

	out.v = max;                                // v
	delta = max - min;
	if (delta < 0.00001)
	{
		out.s = 0;
		out.h = 0; // undefined, maybe nan?
		return out;
	}
	if (max > 0.0) { // NOTE: if Max is == 0, this divide would cause a crash
		out.s = (delta / max);                  // s
	}
	else {
		// if max is 0, then r = g = b = 0              
		// s = 0, h is undefined
		out.s = 0.0;
		out.h = NAN;                            // its now undefined
		return out;
	}
	if (in.r >= max)                           // > is bogus, just keeps compilor happy
		out.h = (in.g - in.b) / delta;        // between yellow & magenta
	else
		if (in.g >= max)
			out.h = 2.0 + (in.b - in.r) / delta;  // between cyan & yellow
		else
			out.h = 4.0 + (in.r - in.g) / delta;  // between magenta & cyan

	out.h *= 60.0;                              // degrees

	if (out.h < 0.0)
		out.h += 360.0;

	return out;
}


rgb hsv2rgb(hsv in)
{
	double      hh, p, q, t, ff;
	long        i;
	rgb         out;

	if (in.s <= 0.0) {       // < is bogus, just shuts up warnings
		out.r = in.v;
		out.g = in.v;
		out.b = in.v;
		return out;
	}
	hh = in.h;
	if (hh >= 360.0) hh = 0.0;
	hh /= 60.0;
	i = (long)hh;
	ff = hh - i;
	p = in.v * (1.0 - in.s);
	q = in.v * (1.0 - (in.s * ff));
	t = in.v * (1.0 - (in.s * (1.0 - ff)));

	switch (i) {
	case 0:
		out.r = in.v;
		out.g = t;
		out.b = p;
		break;
	case 1:
		out.r = q;
		out.g = in.v;
		out.b = p;
		break;
	case 2:
		out.r = p;
		out.g = in.v;
		out.b = t;
		break;

	case 3:
		out.r = p;
		out.g = q;
		out.b = in.v;
		break;
	case 4:
		out.r = t;
		out.g = p;
		out.b = in.v;
		break;
	case 5:
	default:
		out.r = in.v;
		out.g = p;
		out.b = q;
		break;
	}
	return out;
}
#define JUSTICE_MAIN 0 //code i read from justice
#define ONYX_FLOOR_MAIN 1
#if JUSTICE_MAIN
int main()
{
	context game;
	game.init(640, 480, "Thonk");

	vertex triVerts[] =
	{
		{ { -.5f, -.5f, 0, 1 }, {1.f,0.f,0.f,1.f},{1.f,0.f,0.f,0.f}, {0.f,0.f}  },
		{ {  .5f, -.5f, 0, 1 }, {0.f,1.f,0.f,1.f},{1.f,0.f,0.f,0.f}, {1.f, 0.f} },
		{ {   0,   .5f, 0, 1 }, {0.f,0.f,1.f,1.f},{1.f,0.f,0.f,0.f}, {.5f, 1.f} }
	};

	unsigned int triIndices[] = { 0, 1, 2 };

#pragma region Load shaders from file
	std::ifstream ifs("shaders/FragmentShader.shader");
	std::string content1((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));
	const char * basicFrag = content1.c_str();

	std::ifstream iffs("shaders/VertexShader.shader");
	std::string content2((std::istreambuf_iterator<char>(iffs)),
		(std::istreambuf_iterator<char>()));
	const char * basicVert = content2.c_str();
#pragma endregion

	geometry triangle = makeGeometry(triVerts, 3, triIndices, 3);
	shader basicShad = makeShader(basicVert, basicFrag);
	//int vertexColorLocation = glGetUniformLocation(basicShad.program, "modColor");
		
	glm::mat4 triModel = glm::identity<glm::mat4>();
	glm::mat4 camProj = glm::perspective(glm::radians(50.f), 640.f / 480.f, 0.1f, 100.0f);

	texture triTex = loadTexture("C:\\Users\\s189064\\source\\repos\\Renderer\\Renderer\\tex.tga");
	setUniform(basicShad, 3, triTex, 0);
	float timeValue = 0;
	while (!game.shouldClose())
	{
		game.tick();
		game.clear();

		timeValue += 1.f / 600.f;
		glm::mat4 camView = glm::lookAt(glm::vec3(0, 0, -10), glm::vec3(sin(timeValue * 3), cos(timeValue * 3), 0), glm::vec3(0, 1, 0));

		triModel = glm::rotate(triModel, glm::radians(5.f), glm::vec3(0, 1, 0));
		setUniform(basicShad, 0, camProj);
		setUniform(basicShad, 1, camView);
		setUniform(basicShad, 2, triModel);
		setUniform(basicShad, 4, glm::vec3(-1, 0, 0));
		

		/*float timeValue = game.getTime();
		float redValue = sin(timeValue * 3) / 2.0f + 0.5f;
		float greenValue = sin(timeValue) / 1 + 0.5f;
		float blueValue = sin(timeValue * 4) / 4.0f + 0.5f;
		glUniform4f(vertexColorLocation, redValue, greenValue, blueValue, 1.0f);*/

		draw(basicShad, triangle);

		//assert(glGetError() == GL_NO_ERROR);
	}

	game.term();
	return 0;
}
#elif ONYX_FLOOR_MAIN
int main()
{
	context notagame;
	notagame.init(640, 480, "BIG BRAIN TIME");
	vertex triVerts[] =
	{
		{ { -5.f, -2.f, -5.f, 1 }, {1.f,0.f,0.f,1.f}, {0.f,1.f,0.f,0.f}, {0.f,1.f}  },
		{ { -5.f, -2.f,  5.f, 1 }, {1.f,1.f,0.f,1.f},{0.f,1.f,0.f,0.f}, {0.f, 0.f} },
		{ {  5.f, -2.f, -5.f, 1 }, {1.f,0.f,1.f,1.f},  {0.f,1.f,0.f,0.f}, {1.f, 1.f} },
		{ {  5.f, -2.f,  5.f, 1 }, {1.f,0.f,1.f,1.f}, {0.f,1.f,0.f,0.f}, {1.f, -1.f} }
	};

	unsigned int triIndices[] = { 0, 1, 2 , 1,2,3 };

#pragma region Load shaders from file
	std::ifstream ifs("shaders/FragmentShader.shader");
	std::string content1((std::istreambuf_iterator<char>(ifs)),
		(std::istreambuf_iterator<char>()));
	const char * basicFrag = content1.c_str();

	std::ifstream iffs("shaders/VertexShader.shader");
	std::string content2((std::istreambuf_iterator<char>(iffs)),
		(std::istreambuf_iterator<char>()));
	const char * basicVert = content2.c_str();
#pragma endregion

	geometry triangle = makeGeometry(triVerts, sizeof(triVerts), triIndices, sizeof(triIndices));
	shader basicShad = makeShader(basicVert, basicFrag);
	//int vertexColorLocation = glGetUniformLocation(basicShad.program, "modColor");

	glm::mat4 triModel = glm::identity<glm::mat4>();
	glm::mat4 camProj = glm::perspective(glm::radians(50.f), 640.f / 480.f, 0.1f, 100.0f);

	texture triTex = loadTexture("C:\\Users\\s189064\\source\\repos\\Renderer\\Renderer\\tex.tga");
	setUniform(basicShad, 3, triTex, 0);
	float timeValue = 0;
	while (!notagame.shouldClose())
	{
		notagame.tick();
		notagame.clear();

		timeValue += 1.f / 600.f;
		glm::mat4 camView = glm::lookAt(glm::vec3(0, 0, -10), glm::vec3(sin(timeValue * 3), cos(timeValue * 3), 0), glm::vec3(0, 1, 0));

		triModel = glm::rotate(triModel, glm::radians(5.f), glm::vec3(0, 1, 0));
		setUniform(basicShad, 0, camProj);
		setUniform(basicShad, 1, camView);
		setUniform(basicShad, 2, triModel);
		setUniform(basicShad, 4, glm::vec3(0, -0.1f, 0));


		/*float timeValue = game.getTime();
		float redValue = sin(timeValue * 3) / 2.0f + 0.5f;
		float greenValue = sin(timeValue) / 1 + 0.5f;
		float blueValue = sin(timeValue * 4) / 4.0f + 0.5f;
		glUniform4f(vertexColorLocation, redValue, greenValue, blueValue, 1.0f);*/

		draw(basicShad, triangle);

		//assert(glGetError() == GL_NO_ERROR);
	}

	notagame.term();
	return 0;
}
#else
int main()							  
{									  
	context thisIsNotAGame;			  
	thisIsNotAGame.init(640, 480, "Source3");
#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glDebugMessageCallback(errorCallback, 0);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, 0, true);
#endif
	vertex triVerts[] = {			  //lmao never again
{{  2.229769f,   0.084498f, - 3.378793f, 1.0f}},
{{  0.677445f, - 0.501550f, - 4.567297f, 1.0f}},
{{  0.787563f, - 0.104688f, - 4.530283f, 1.0f}},
{{  0.239930f, - 0.803727f, - 4.666746f, 1.0f}},
{{  0.481220f, - 0.299962f, - 4.893909f, 1.0f}},
{{  0.301354f, - 0.366772f, - 4.921114f, 1.0f}},
{{  0.259932f, - 0.511213f, - 4.782341f, 1.0f}},
{{  0.583998f,   0.150124f, - 4.726919f, 1.0f}},
{{  0.297705f,   0.069242f, - 4.890442f, 1.0f}},
{{  0.413156f,   0.427405f, - 4.752587f, 1.0f}},
{{  0.828260f,   0.528503f, - 4.559581f, 1.0f}},
{{  0.974105f,   0.687815f, - 4.196025f, 1.0f}},
{{  1.196562f,   0.386493f, - 4.036527f, 1.0f}},
{{  0.442425f,   0.911049f, - 4.242062f, 1.0f}},
{{  0.401719f, - 0.519407f, - 4.817513f, 1.0f}},
{{  0.780991f, - 0.253832f, - 4.630212f, 1.0f}},
{{  0.623517f,   1.367286f, - 3.124986f, 1.0f}},
{{  0.969808f,   1.115791f, - 3.326386f, 1.0f}},
{{  1.048142f,   0.454550f, - 3.271591f, 1.0f}},
{{  0.595159f,   0.485343f, - 3.099741f, 1.0f}},
{{  0.598871f,   0.176346f, - 4.691103f, 1.0f}},
{{  0.363557f,   0.122505f, - 4.807135f, 1.0f}},
{{  0.481883f,   0.368337f, - 4.700356f, 1.0f}},
{{  0.715207f,   0.449241f, - 4.580332f, 1.0f}},
{{  0.612471f,   0.216512f, - 2.999963f, 1.0f}},
{{  0.958314f,   1.223429f, - 2.334825f, 1.0f}},
{{  0.942578f,   1.024823f, - 2.346653f, 1.0f}},
{{  0.762324f,   1.348518f, - 2.250350f, 1.0f}},
{{  0.716461f,   0.978210f, - 2.332627f, 1.0f}},
{{  0.797070f,   1.389613f, - 1.551973f, 1.0f}},
{{  1.238028f, - 0.024112f, - 3.749292f, 1.0f}},
{{  1.247200f, - 0.214399f, - 3.834987f, 1.0f}},
{{  1.156050f, - 0.160639f, - 3.931836f, 1.0f}},
{{  1.160428f,   0.143317f, - 3.264752f, 1.0f}},
{{  0.098284f, - 0.549189f, - 4.863210f, 1.0f}},
{{  0.274800f, - 0.040163f, - 4.877608f, 1.0f}},
{{  0.444885f,   0.112167f, - 4.812418f, 1.0f}},
{{  0.134383f, - 0.438716f, - 4.998058f, 1.0f}},
{{  0.486460f,   0.169420f, - 4.748035f, 1.0f}},
{{  0.458982f,   0.880311f, - 3.724452f, 1.0f}},
{{  0.533963f,   1.138954f, - 3.683234f, 1.0f}},
{{  1.002200f,   0.921174f, - 3.661572f, 1.0f}},
{{  1.156368f,   0.480241f, - 3.439216f, 1.0f}},
{{  1.189277f,   0.084769f, - 3.448216f, 1.0f}},
{{  0.114340f, - 0.476191f, - 4.796559f, 1.0f}},
{{  0.239353f, - 0.444085f, - 4.553168f, 1.0f}},
{{  0.089539f, - 0.597017f, - 4.785580f, 1.0f}},
{{  0.161951f, - 0.614031f, - 4.601945f, 1.0f}},
{{  0.055872f, - 0.679170f, - 4.657986f, 1.0f}},
{{  0.136848f, - 0.703886f, - 4.511986f, 1.0f}},
{{  0.228390f, - 0.814076f, - 4.275360f, 1.0f}},
{{  0.401288f, - 0.806873f, - 4.272674f, 1.0f}},
{{  0.853902f, - 0.715699f, - 4.001434f, 1.0f}},
{{  0.605070f, - 0.773697f, - 4.183734f, 1.0f}},
{{  0.596547f, - 0.285192f, - 2.883051f, 1.0f}},
{{  0.508587f, - 0.680667f, - 3.121039f, 1.0f}},
{{  1.057746f, - 0.268636f, - 3.183540f, 1.0f}},
{{  0.918745f, - 0.581156f, - 3.301216f, 1.0f}},
{{  1.219896f, - 0.261527f, - 3.648209f, 1.0f}},
{{  1.161621f, - 0.308655f, - 3.496541f, 1.0f}},
{{  1.036427f, - 0.615710f, - 3.475638f, 1.0f}},
{{  1.056282f, - 0.639114f, - 3.595489f, 1.0f}},
{{  0.532609f, - 0.074919f, - 4.810413f, 1.0f}},
{{  0.148853f,   0.077151f, - 4.893420f, 1.0f}},
{{  0.206578f,   0.456526f, - 4.756276f, 1.0f}},
{{  0.807911f,   0.211907f, - 4.544932f, 1.0f}},
{{  1.085333f,   0.537154f, - 4.116276f, 1.0f}},
{{  0.221213f,   1.018614f, - 4.254435f, 1.0f}},
{{  1.008975f,   0.785170f, - 3.298989f, 1.0f}},
{{  0.297579f,   0.512621f, - 3.103828f, 1.0f}},
{{  0.306236f,   0.217541f, - 2.931911f, 1.0f}},
{{  0.950446f,   1.124126f, - 2.340739f, 1.0f}},
{{  0.359843f,   0.036002f, - 4.845013f, 1.0f}},
{{  0.229491f,   0.959522f, - 3.701892f, 1.0f}},
{{  1.079284f,   0.700708f, - 3.550395f, 1.0f}},
{{  0.298273f, - 0.281934f, - 2.789148f, 1.0f}},
{{  0.254294f, - 0.714459f, - 3.069232f, 1.0f}},
{{  0.821650f,   0.469946f, - 3.185667f, 1.0f}},
{{  0.829520f,   1.001516f, - 2.339640f, 1.0f}},
{{  0.886450f,   0.179914f, - 3.132358f, 1.0f}},
{{  0.827146f, - 0.276914f, - 3.033296f, 1.0f}},
{{  0.713666f, - 0.630912f, - 3.211128f, 1.0f}},
{{  0.194808f, - 0.797871f, - 3.820771f, 1.0f}},
{{  0.275357f, - 0.821872f, - 3.776423f, 1.0f}},
{{  0.422914f, - 0.761996f, - 3.720878f, 1.0f}},
{{  0.334616f, - 0.824207f, - 3.751066f, 1.0f}},
{{  0.296988f, - 0.749221f, - 3.399824f, 1.0f}},
{{  0.446560f, - 0.712932f, - 3.465529f, 1.0f}},
{{  0.489475f, - 0.725533f, - 3.529135f, 1.0f}},
{{  0.496716f, - 0.734068f, - 3.572842f, 1.0f}},
{{  0.204254f, - 0.761544f, - 3.380931f, 1.0f}},
{{  0.371774f, - 0.731077f, - 3.432677f, 1.0f}},
{{- 2.229769f,   0.084498f, - 3.378793f, 1.0f}},
{{- 0.677445f, - 0.501550f, - 4.567297f, 1.0f}},
{{- 0.787563f, - 0.104688f, - 4.530283f, 1.0f}},
{{- 0.239930f, - 0.803727f, - 4.666746f, 1.0f}},
{{- 0.481220f, - 0.299962f, - 4.893909f, 1.0f}},
{{  0.000000f, - 0.521418f, - 4.907984f, 1.0f}},
{{- 0.301354f, - 0.366772f, - 4.921114f, 1.0f}},
{{- 0.259932f, - 0.511213f, - 4.782341f, 1.0f}},
{{  0.000000f,   0.485647f, - 4.759966f, 1.0f}},
{{  0.000000f,   0.085060f, - 4.896398f, 1.0f}},
{{- 0.583998f,   0.150124f, - 4.726919f, 1.0f}},
{{- 0.297705f,   0.069242f, - 4.890442f, 1.0f}},
{{- 0.413156f,   0.427405f, - 4.752587f, 1.0f}},
{{- 0.828260f,   0.528503f, - 4.559581f, 1.0f}},
{{- 0.974105f,   0.687815f, - 4.196025f, 1.0f}},
{{- 1.196562f,   0.386493f, - 4.036527f, 1.0f}},
{{  0.000000f,   1.126178f, - 4.266807f, 1.0f}},
{{- 0.442425f,   0.911049f, - 4.242062f, 1.0f}},
{{  0.000000f, - 0.399684f, - 5.031036f, 1.0f}},
{{- 0.401719f, - 0.519407f, - 4.817513f, 1.0f}},
{{- 0.780991f, - 0.253832f, - 4.630212f, 1.0f}},
{{- 0.623517f,   1.367286f, - 3.124986f, 1.0f}},
{{- 0.969808f,   1.115791f, - 3.326386f, 1.0f}},
{{- 1.048142f,   0.454550f, - 3.271591f, 1.0f}},
{{- 0.595159f,   0.485343f, - 3.099741f, 1.0f}},
{{  0.000000f,   0.539899f, - 3.107915f, 1.0f}},
{{- 0.598871f,   0.176346f, - 4.691103f, 1.0f}},
{{- 0.363557f,   0.122505f, - 4.807135f, 1.0f}},
{{- 0.481883f,   0.368337f, - 4.700356f, 1.0f}},
{{- 0.715207f,   0.449241f, - 4.580332f, 1.0f}},
{{  0.000000f,   0.218570f, - 2.863860f, 1.0f}},
{{- 0.612471f,   0.216512f, - 2.999963f, 1.0f}},
{{- 0.958314f,   1.223429f, - 2.334825f, 1.0f}},
{{- 0.942578f,   1.024823f, - 2.346653f, 1.0f}},
{{- 0.762324f,   1.348518f, - 2.250350f, 1.0f}},
{{- 0.716461f,   0.978210f, - 2.332627f, 1.0f}},
{{- 0.797070f,   1.389613f, - 1.551973f, 1.0f}},
{{- 1.238028f, - 0.024112f, - 3.749292f, 1.0f}},
{{- 1.247200f, - 0.214399f, - 3.834987f, 1.0f}},
{{- 1.156050f, - 0.160639f, - 3.931836f, 1.0f}},
{{- 1.160428f,   0.143317f, - 3.264752f, 1.0f}},
{{- 0.098284f, - 0.549189f, - 4.863210f, 1.0f}},
{{- 0.274800f, - 0.040163f, - 4.877608f, 1.0f}},
{{- 0.444885f,   0.112167f, - 4.812418f, 1.0f}},
{{- 0.134383f, - 0.438716f, - 4.998058f, 1.0f}},
{{- 0.486460f,   0.169420f, - 4.748035f, 1.0f}},
{{- 0.458982f,   0.880311f, - 3.724452f, 1.0f}},
{{  0.000000f,   1.038733f, - 3.679331f, 1.0f}},
{{- 0.533963f,   1.138954f, - 3.683234f, 1.0f}},
{{- 1.002200f,   0.921174f, - 3.661572f, 1.0f}},
{{- 1.156368f,   0.480241f, - 3.439216f, 1.0f}},
{{- 1.189277f,   0.084769f, - 3.448216f, 1.0f}},
{{  0.000000f, - 0.481409f, - 4.851078f, 1.0f}},
{{- 0.114340f, - 0.476191f, - 4.796559f, 1.0f}},
{{- 0.239353f, - 0.444085f, - 4.553168f, 1.0f}},
{{  0.000000f, - 0.592933f, - 4.820268f, 1.0f}},
{{- 0.089539f, - 0.597017f, - 4.785580f, 1.0f}},
{{- 0.161951f, - 0.614031f, - 4.601945f, 1.0f}},
{{  0.000000f, - 0.673943f, - 4.702011f, 1.0f}},
{{- 0.055872f, - 0.679170f, - 4.657986f, 1.0f}},
{{- 0.136848f, - 0.703886f, - 4.511986f, 1.0f}},
{{  0.000000f, - 0.824778f, - 4.312728f, 1.0f}},
{{- 0.228390f, - 0.814076f, - 4.275360f, 1.0f}},
{{- 0.401288f, - 0.806873f, - 4.272674f, 1.0f}},
{{- 0.853902f, - 0.715699f, - 4.001434f, 1.0f}},
{{- 0.605070f, - 0.773697f, - 4.183734f, 1.0f}},
{{  0.000000f, - 0.278675f, - 2.695245f, 1.0f}},
{{  0.000000f, - 0.748251f, - 3.017424f, 1.0f}},
{{- 0.596547f, - 0.285192f, - 2.883051f, 1.0f}},
{{- 0.508587f, - 0.680667f, - 3.121039f, 1.0f}},
{{- 1.057746f, - 0.268636f, - 3.183540f, 1.0f}},
{{- 0.918745f, - 0.581156f, - 3.301216f, 1.0f}},
{{- 1.219896f, - 0.261527f, - 3.648209f, 1.0f}},
{{- 1.161621f, - 0.308655f, - 3.496541f, 1.0f}},
{{- 1.036427f, - 0.615710f, - 3.475638f, 1.0f}},
{{- 1.056282f, - 0.639114f, - 3.595489f, 1.0f}},
{{- 0.532609f, - 0.074919f, - 4.810413f, 1.0f}},
{{- 0.148853f,   0.077151f, - 4.893420f, 1.0f}},
{{- 0.206578f,   0.456526f, - 4.756276f, 1.0f}},
{{- 0.807911f,   0.211907f, - 4.544932f, 1.0f}},
{{- 1.085333f,   0.537154f, - 4.116276f, 1.0f}},
{{- 0.221213f,   1.018614f, - 4.254435f, 1.0f}},
{{- 1.008975f,   0.785170f, - 3.298989f, 1.0f}},
{{- 0.297579f,   0.512621f, - 3.103828f, 1.0f}},
{{- 0.306236f,   0.217541f, - 2.931911f, 1.0f}},
{{- 0.950446f,   1.124126f, - 2.340739f, 1.0f}},
{{- 0.359843f,   0.036002f, - 4.845013f, 1.0f}},
{{- 0.229491f,   0.959522f, - 3.701892f, 1.0f}},
{{- 1.079284f,   0.700708f, - 3.550395f, 1.0f}},
{{- 0.298273f, - 0.281934f, - 2.789148f, 1.0f}},
{{- 0.254294f, - 0.714459f, - 3.069232f, 1.0f}},
{{- 0.821650f,   0.469946f, - 3.185667f, 1.0f}},
{{- 0.829520f,   1.001516f, - 2.339640f, 1.0f}},
{{- 0.886450f,   0.179914f, - 3.132358f, 1.0f}},
{{- 0.827146f, - 0.276914f, - 3.033296f, 1.0f}},
{{- 0.713666f, - 0.630912f, - 3.211128f, 1.0f}},
{{  0.000000f, - 0.801774f, - 3.834398f, 1.0f}},
{{- 0.194808f, - 0.797871f, - 3.820771f, 1.0f}},
{{- 0.275357f, - 0.821872f, - 3.776423f, 1.0f}},
{{- 0.422914f, - 0.761996f, - 3.720878f, 1.0f}},
{{- 0.334616f, - 0.824207f, - 3.751066f, 1.0f}},
{{  0.000000f, - 0.773867f, - 3.362039f, 1.0f}},
{{- 0.296988f, - 0.749221f, - 3.399824f, 1.0f}},
{{- 0.446560f, - 0.712932f, - 3.465529f, 1.0f}},
{{- 0.489475f, - 0.725533f, - 3.529135f, 1.0f}},
{{- 0.496716f, - 0.734068f, - 3.572842f, 1.0f}},
{{- 0.204254f, - 0.761544f, - 3.380931f, 1.0f}},
{{- 0.371774f, - 0.731077f, - 3.432677f, 1.0f}}
	};
	unsigned int triIndices[] = { 
15,2,16	   ,
1,16,2	   ,
7,38,35	   ,
5,16,3	   ,
36,111,38  ,
63,37,73   ,
65,9,10	   ,
63,3,66	   ,
67,11,66   ,
68,101,65  ,
14,11,12   ,
15,7,4	   ,
6,15,5	   ,
16,1,3	   ,
69,42,75   ,
14,42,41   ,
74,14,40   ,
43,34,19   ,
10,24,11   ,
11,21,8	   ,
10,22,23   ,
8,39,37	   ,
71,118,70  ,
69,27,72   ,
29,28,30   ,
17,29,20   ,
20,79,78   ,
17,26,28   ,
32,33,1	   ,
72,30,26   ,
79,30,27   ,
28,26,30   ,
3,1,33	   ,
1,2,32	   ,
13,33,31   ,
43,31,44   ,
78,25,20   ,
37,22,9	   ,
73,9,64	   ,
36,6,5	   ,
98,38,111  ,
70,40,20   ,
41,18,17   ,
67,42,12   ,
40,14,41   ,
20,41,17   ,
47,145,45  ,
48,45,46   ,
48,49,47   ,
47,151,148 ,
50,51,49   ,
49,154,151 ,
46,54,48   ,
48,52,50   ,
76,123,71  ,
2,53,32	   ,
76,56,77   ,
81,25,80   ,
81,58,82   ,
44,57,34   ,
44,59,60   ,
31,32,59   ,
32,62,59   ,
62,60,59   ,
58,60,61   ,
160,76,77  ,
55,71,25   ,
13,75,67   ,
118,74,70  ,
36,64,102  ,
18,72,26   ,
71,20,25   ,
140,68,74  ,
43,69,75   ,
14,65,10   ,
13,66,3	   ,
8,66,11	   ,
102,65,101 ,
36,63,73   ,
55,82,56   ,
57,80,34   ,
19,80,78   ,
78,27,19   ,
53,90,62   ,
56,92,87   ,
58,89,88   ,
77,87,91   ,
51,189,154 ,
54,85,86   ,
82,88,92   ,
77,194,160 ,
61,90,89   ,
52,83,51   ,
54,84,52   ,
94,112,113 ,
93,94,113  ,
137,100,134,
113,97,95  ,
135,111,102,
136,169,179,
171,104,170,
95,169,172 ,
173,106,107,
101,174,171,
106,110,107,
100,112,96 ,
99,97,112  ,
113,95,93  ,
142,175,181,
142,110,141,
180,110,174,
133,143,116,
122,105,106,
119,106,103,
105,120,104,
138,103,136,
177,118,123,
175,126,116,
128,129,127,
128,114,117,
185,117,184,
114,125,115,
131,93,132 ,
129,178,125,
129,185,126,
127,129,125,
95,132,93  ,
93,131,94  ,
108,132,95 ,
143,130,108,
184,124,186,
120,136,104,
104,179,170,
99,135,97  ,
98,137,134 ,
176,139,180,
115,141,114,
173,142,181,
139,141,110,
141,117,114,
145,149,146,
146,150,147,
152,150,149,
151,149,148,
155,153,152,
154,152,151,
158,147,150,
150,156,158,
123,182,177,
157,94,131 ,
162,182,183,
124,187,186,
164,187,188,
163,144,133,
144,165,130,
130,131,132,
131,168,157,
166,168,165,
164,166,163,
160,182,159,
177,161,124,
108,181,143,
118,180,140,
170,135,102,
115,178,175,
117,177,124,
140,174,109,
143,175,116,
171,110,105,
108,172,173,
172,103,106,
171,102,101,
135,169,97 ,
188,161,162,
186,163,133,
116,186,133,
126,184,116,
198,157,168,
162,200,188,
164,197,167,
183,195,162,
189,155,154,
158,192,157,
188,196,164,
194,183,160,
167,198,168,
190,156,155,
191,158,156,
15,4,2	   ,
7,6,38	   ,
5,15,16	   ,
36,102,111 ,
63,8,37	   ,
65,64,9	   ,
63,5,3	   ,
67,12,11   ,
68,109,101 ,
14,10,11   ,
15,6,7	   ,
69,18,42   ,
14,12,42   ,
74,68,14   ,
43,44,34   ,
10,23,24   ,
11,24,21   ,
10,9,22	   ,
8,21,39	   ,
71,123,118 ,
69,19,27   ,
17,28,29   ,
20,29,79   ,
17,18,26   ,
72,27,30   ,
79,29,30   ,
13,3,33	   ,
43,13,31   ,
78,80,25   ,
37,39,22   ,
73,37,9	   ,
36,38,6	   ,
98,35,38   ,
70,74,40   ,
41,42,18   ,
67,75,42   ,
20,40,41   ,
47,148,145 ,
48,47,45   ,
48,50,49   ,
47,49,151  ,
50,52,51   ,
49,51,154  ,
46,53,54   ,
48,54,52   ,
76,159,123 ,
2,46,53	   ,
76,55,56   ,
81,55,25   ,
81,57,58   ,
44,60,57   ,
44,31,59   ,
31,33,32   ,
32,53,62   ,
62,61,60   ,
58,57,60   ,
160,159,76 ,
55,76,71   ,
13,43,75   ,
118,140,74 ,
36,73,64   ,
18,69,72   ,
71,70,20   ,
140,109,68 ,
43,19,69   ,
14,68,65   ,
13,67,66   ,
8,63,66	   ,
102,64,65  ,
36,5,63	   ,
55,81,82   ,
57,81,80   ,
19,34,80   ,
78,79,27   ,
53,85,90   ,
56,82,92   ,
58,61,89   ,
77,56,87   ,
51,83,189  ,
54,53,85   ,
82,58,88   ,
77,91,194  ,
61,62,90   ,
52,84,83   ,
54,86,84   ,
94,96,112  ,
137,99,100 ,
113,112,97 ,
135,137,111,
136,103,169,
171,105,104,
95,97,169  ,
173,172,106,
101,109,174,
106,105,110,
100,99,112 ,
142,115,175,
142,107,110,
180,139,110,
133,144,143,
122,121,105,
119,122,106,
105,121,120,
138,119,103,
177,176,118,
175,178,126,
128,127,114,
185,128,117,
114,127,125,
129,126,178,
129,128,185,
108,130,132,
143,144,130,
184,117,124,
120,138,136,
104,136,179,
99,137,135 ,
98,111,137 ,
176,117,139,
115,142,141,
173,107,142,
141,139,117,
145,148,149,
146,149,150,
152,153,150,
151,152,149,
155,156,153,
154,155,152,
158,157,147,
150,153,156,
123,159,182,
157,147,94 ,
162,161,182,
124,161,187,
164,163,187,
163,166,144,
144,166,165,
130,165,131,
131,165,168,
166,167,168,
164,167,166,
160,183,182,
177,182,161,
108,173,181,
118,176,180,
170,179,135,
115,125,178,
117,176,177,
140,180,174,
143,181,175,
171,174,110,
108,95,172 ,
172,169,103,
171,170,102,
135,179,169,
188,187,161,
186,187,163,
116,184,186,
126,185,184,
198,192,157,
162,195,200,
164,196,197,
183,199,195,
189,190,155,
158,193,192,
188,200,196,
194,199,183,
167,197,198,
190,191,156,
191,193,158 };
for (int i = 0; i < sizeof(triIndices) / sizeof(*triIndices); i++) {
	triIndices[i]--;
}
	geometry triangle = makeGeometry(triVerts, sizeof(triVerts)/sizeof(*triVerts), triIndices, sizeof(triIndices) / sizeof(*triIndices));
	const char * basicVert = "#version 430\n"
		"layout (location = 0) in vec4 position;\n"
		"layout (location = 0) uniform mat4 proj;\n"
		"layout (location = 1) uniform mat4 view;\n"
		"layout (location = 2) uniform mat4 model;\n"
		"void main() {gl_Position=proj * view * model * position;}";
	const char * basicFrag =
		"#version 330\n"
		"out vec4 vertColor;\n"
		"void main() { gl_FragColor = vec4(gl_FragCoord.w, gl_FragCoord.w, gl_FragCoord.w, 1.0);}";
	shader basicShader = makeShader(basicVert, basicFrag);

	glm::mat4 triModel = glm::identity<glm::mat4>();
	glm::mat4 camProj = glm::perspective(1.f,640.f/480.f,0.1f, 100.f);
	float x = 0;
	glm::mat4 camView = glm::lookAt(glm::vec3(0, 0, -10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	
	while (!thisIsNotAGame.shouldClose()) {
		hsv color;
		color.h = x*360;
		color.s = 1;
		color.v = 1;
		rgb newcolor = hsv2rgb(color);
		camView = glm::rotate(camView, 1.0f / 60.0f, glm::vec3(0, 1, 0));
		x += 1.f / 60.f;
		x=std::fmod(x, 1);
		thisIsNotAGame.tick();
		//camView = glm::lookAt(glm::vec3(0, 0, x), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		thisIsNotAGame.clear();
		setUniform(basicShader, 0, camProj);
		setUniform(basicShader, 1, camView);
		setUniform(basicShader, 2, triModel);
		std::printf("%f, %f, %f \n", newcolor.r, newcolor.g, newcolor.b);
		glClearColor(newcolor.r, newcolor.g, newcolor.b, 1.0f);
		draw(basicShader, triangle);
	}
	thisIsNotAGame.term();
	return 0;
}
#endif
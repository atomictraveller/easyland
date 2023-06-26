
#include <cmath>
#include <windows.h>
#include <stdio.h>
#include <GL/gl.h>

#pragma comment(lib, "opengl32.lib")


#include "framework.h"	//	default msvc win32 header
#include "ogl.h"	//	my opengl header
#include "easyland.h"	//	header for this project


#define MAX_LOADSTRING 100

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING];
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_EASYLAND, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);
    if (!InitInstance (hInstance, nCmdShow)) return FALSE;

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EASYLAND));
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EASYLAND));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_EASYLAND);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd) return FALSE;


//	begin opengl
	hOGLWnd = hWnd;
	HDC hDC = GetDC(hOGLWnd);

	DWORD pixelFormatFlags = PFD_SUPPORT_OPENGL | PFD_SUPPORT_COMPOSITION | PFD_GENERIC_ACCELERATED | PFD_DRAW_TO_WINDOW | PFD_DOUBLEBUFFER;
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),
		1, pixelFormatFlags, PFD_TYPE_RGBA, 32,	//Colordepth of the framebuffer.
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		24,                       //Number of bits for the depthbuffer
		8,                        //Number of bits for the stencilbuffer
		0,                        //Number of Aux buffers in the framebuffer.
		PFD_MAIN_PLANE,
		0, 0, 0, 0
	};
	int pixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, pixelFormat, &pfd);

	hrc0 = wglCreateContext(hDC);
	hrc1 = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hrc1);

	HMODULE hOpenGL = LoadLibrary(L"opengl32.dll");	//	begin loading "modern" opengl API
	if (hOpenGL == NULL) return 0;
	//glCreateShader = (PFNGLCREATESHADERPROC)GetProcAddress(hOpenGL, "glCreateShader");
	glCreateShader = reinterpret_cast<PFNGLCREATESHADERPROC>(wglGetProcAddress("glCreateShader"));
	glShaderSource = reinterpret_cast<PFNGLSHADERSOURCEPROC>(wglGetProcAddress("glShaderSource"));
	glCompileShader = reinterpret_cast<PFNGLCOMPILESHADERPROC>(wglGetProcAddress("glCompileShader"));
	glGetShaderiv = reinterpret_cast<PFNGLGETSHADERIVPROC>(wglGetProcAddress("glGetShaderiv"));
	glGetShaderInfoLog = reinterpret_cast<PFNGLGETSHADERINFOLOGPROC>(wglGetProcAddress("glGetShaderInfoLog"));
	glCreateProgram = reinterpret_cast<PFNGLCREATEPROGRAMPROC>(wglGetProcAddress("glCreateProgram"));
	glAttachShader = reinterpret_cast<PFNGLATTACHSHADERPROC>(wglGetProcAddress("glAttachShader"));
	glLinkProgram = reinterpret_cast<PFNGLLINKPROGRAMPROC>(wglGetProcAddress("glLinkProgram"));
	glValidateProgram = reinterpret_cast<PFNGLVALIDATEPROGRAMPROC>(wglGetProcAddress("glValidateProgram"));
	glGetProgramiv = reinterpret_cast<PFNGLGETPROGRAMIVPROC>(wglGetProcAddress("glGetProgramiv"));
	glGetProgramInfoLog = reinterpret_cast<PFNGLGETPROGRAMINFOLOGPROC>(wglGetProcAddress("glGetProgramInfoLog"));////was missing
	glGenBuffers = reinterpret_cast<PFNGLGENBUFFERSPROC>(wglGetProcAddress("glGenBuffers"));
	glGenVertexArrays = reinterpret_cast<PFNGLGENVERTEXARRAYSPROC>(wglGetProcAddress("glGenVertexArrays"));
	glGetAttribLocation = reinterpret_cast<PFNGLGETATTRIBLOCATIONPROC>(wglGetProcAddress("glGetAttribLocation"));
	glBindVertexArray = reinterpret_cast<PFNGLBINDVERTEXARRAYPROC>(wglGetProcAddress("glBindVertexArray"));
	glEnableVertexAttribArray = reinterpret_cast<PFNGLENABLEVERTEXATTRIBARRAYPROC>(wglGetProcAddress("glEnableVertexAttribArray"));
	glVertexAttribPointer = reinterpret_cast<PFNGLVERTEXATTRIBPOINTERPROC>(wglGetProcAddress("glVertexAttribPointer"));
	glBindBuffer = reinterpret_cast<PFNGLBINDBUFFERPROC>(wglGetProcAddress("glBindBuffer"));
	glBufferData = reinterpret_cast<PFNGLBUFFERDATAPROC>(wglGetProcAddress("glBufferData"));
	glGetVertexAttribPointerv = reinterpret_cast<PFNGLGETVERTEXATTRIBPOINTERVPROC>(wglGetProcAddress("glGetVertexAttribPointerv"));
	glUseProgram = reinterpret_cast<PFNGLUSEPROGRAMPROC>(wglGetProcAddress("glUseProgram"));
	glDeleteVertexArrays = reinterpret_cast<PFNGLDELETEVERTEXARRAYSPROC>(wglGetProcAddress("glDeleteVertexArrays"));
	glDeleteBuffers = reinterpret_cast<PFNGLDELETEBUFFERSPROC>(wglGetProcAddress("glDeleteBuffers"));
	glDeleteProgram = reinterpret_cast<PFNGLDELETEPROGRAMPROC>(wglGetProcAddress("glDeleteProgram"));
	glDeleteShader = reinterpret_cast<PFNGLDELETESHADERPROC>(wglGetProcAddress("glDeleteShader"));

	glActiveTexture = reinterpret_cast<PFNGLACTIVETEXTUREPROC>(wglGetProcAddress("glActiveTexture"));

	glUniform1i = reinterpret_cast<PFNGLUNIFORM1IPROC>(wglGetProcAddress("glUniform1i"));
	glUniform2i = reinterpret_cast<PFNGLUNIFORM2IPROC>(wglGetProcAddress("glUniform2i"));
	glUniform3f = reinterpret_cast<PFNGLUNIFORM3FPROC>(wglGetProcAddress("glUniform3f"));
	glUniform4f = reinterpret_cast<PFNGLUNIFORM4FPROC>(wglGetProcAddress("glUniform4f"));
	glUniform1fv = reinterpret_cast<PFNGLUNIFORM1FVPROC>(wglGetProcAddress("glUniform1fv"));
	glUniform2fv = reinterpret_cast<PFNGLUNIFORM2FVPROC>(wglGetProcAddress("glUniform2fv"));
	glUniform3fv = reinterpret_cast<PFNGLUNIFORM3FVPROC>(wglGetProcAddress("glUniform3fv"));
	glUniform4fv = reinterpret_cast<PFNGLUNIFORM4FVPROC>(wglGetProcAddress("glUniform4fv"));
	glUniformMatrix3fv = reinterpret_cast<PFNGLUNIFORMMATRIX3FVPROC>(wglGetProcAddress("glUniformMatrix3fv"));
	glUniformMatrix4fv = reinterpret_cast<PFNGLUNIFORMMATRIX4FVPROC>(wglGetProcAddress("glUniformMatrix4fv"));
	glGetUniformLocation = reinterpret_cast<PFNGLGETUNIFORMLOCATIONPROC>(wglGetProcAddress("glGetUniformLocation"));

	glCreateVertexArrays = reinterpret_cast<PFNGLCREATEVERTEXARRAYSPROC>(wglGetProcAddress("glCreateVertexArrays"));
	glGenFramebuffers = reinterpret_cast<PFNGLGENFRAMEBUFFERSPROC>(wglGetProcAddress("glGenFramebuffers"));

	glBindFramebuffer = reinterpret_cast<PFNGLBINDFRAMEBUFFERPROC>(wglGetProcAddress("glBindFramebuffer"));
	glFramebufferTexture1D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE1DPROC>(wglGetProcAddress("glFramebufferTexture1D"));
	glFramebufferTexture2D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE2DPROC>(wglGetProcAddress("glFramebufferTexture2D"));
	glFramebufferTexture3D = reinterpret_cast<PFNGLFRAMEBUFFERTEXTURE3DPROC>(wglGetProcAddress("glFramebufferTexture3D"));
	glFramebufferRenderbuffer = reinterpret_cast<PFNGLFRAMEBUFFERRENDERBUFFERPROC>(wglGetProcAddress("glFramebufferRenderbuffer"));

	FreeLibrary(hOpenGL);
	if (glCreateShader == NULL) return 0;
	if (glShaderSource == NULL) return 0;
	if (glCompileShader == NULL) return 0;
	if (glGetShaderiv == NULL) return 0;
	if (glGetShaderInfoLog == NULL) return 0;
	if (glCreateProgram == NULL) return 0;
	if (glAttachShader == NULL) return 0;
	if (glLinkProgram == NULL) return 0;
	if (glValidateProgram == NULL) return 0;
	if (glGetProgramiv == NULL) return 0;
	if (glGetProgramInfoLog == NULL) return 0;	//	
	if (glGenBuffers == NULL) return 0;
	if (glGenVertexArrays == NULL) return 0;
	if (glGetAttribLocation == NULL) return 0;
	if (glBindVertexArray == NULL) return 0;
	if (glEnableVertexAttribArray == NULL) return 0;
	if (glVertexAttribPointer == NULL) return 0;
	if (glBindBuffer == NULL) return 0;
	if (glBufferData == NULL) return 0;
	if (glGetVertexAttribPointerv == NULL) return 0;
	if (glUseProgram == NULL) return 0;
	if (glDeleteVertexArrays == NULL) return 0;
	if (glDeleteBuffers == NULL) return 0;
	if (glDeleteProgram == NULL) return 0;
	if (glDeleteShader == NULL) return 0;

	if (glActiveTexture == NULL) return 0;

	if (glUniform1i == NULL) return 0;
	if (glUniform2i == NULL) return 0;
	if (glUniform3f == NULL) return 0;
	if (glUniform4f == NULL) return 0;
	if (glUniform1fv == NULL) return 0;
	if (glUniform2fv == NULL) return 0;
	if (glUniform3fv == NULL) return 0;
	if (glUniform4fv == NULL) return 0;
	if (glUniformMatrix3fv == NULL) return 0;
	if (glUniformMatrix4fv == NULL) return 0;
	if (glGetUniformLocation == NULL) return 0;

	if (glCreateVertexArrays == NULL) return 0;
	if (glGenFramebuffers == NULL) return 0;

	if (glBindFramebuffer == NULL) return 0;
	if (glFramebufferTexture1D == NULL) return 0;
	if (glFramebufferTexture2D == NULL) return 0;
	if (glFramebufferTexture3D == NULL) return 0;
	if (glFramebufferRenderbuffer == NULL) return 0;


	const char* shadowVertexShaderSource = R"(
		#version 330 core
		layout (location = 0) in vec3 pos;
		uniform mat4 sunMatrix;
		uniform mat4 model;
		void main() {
			gl_Position = sunMatrix * model * vec4(pos.x, pos.y, pos.z, 1.0);
		}
	)";
	const char* shadowFragmentShaderSource = R"(
		#version 330 core
		void main() {  
			// gl_FragDepth = gl_FragCoord.z;
		}   
	)";


	const char* vertexShaderSource = R"(
		#version 330 core
		layout (location = 0) in vec3 pos;
		layout (location = 1) in vec3 col;
		layout (location = 2) in vec3 norm;
		
		out vec3 texloc;
		out vec4 color;
		out vec3 normal;
		out float depth;	//	for perlin
		out vec3 fragpos;
		out vec4 sunfragpos;

		uniform mat4 modelMatrix; 
		uniform mat4 viewMatrix;
		uniform mat4 projectionMatrix;
		uniform mat4 sunMatrix;

		void main() {
			fragpos = vec3(modelMatrix * vec4(pos.x, pos.y, pos.z, 1.0));
			sunfragpos = sunMatrix * vec4(fragpos.x, fragpos.y, fragpos.z, 1.0);
			gl_Position = projectionMatrix * viewMatrix * vec4(fragpos, 1.0);
			texloc = pos;
			color = vec4(col, 1.0);			
			normal = norm;
			depth = (gl_Position.z - .001) * (gl_Position.z + .025);	//	for perlin octaves
		}
	)";




	const char* fragmentShaderSource = R"(
		#version 330 core
		out vec4 FragColor;
		
		in vec3 texloc;
		in vec4 color;
		in vec3 normal;
		in float depth;
		in vec3 fragpos;
		in vec4 sunfragpos;

		uniform float rv[49];
		uniform sampler2D deepMap;

		uniform int material;
		uniform vec3 ambcol;
		uniform vec3 suncol;
		uniform vec3 mistcol;
		uniform vec3 sun;
		uniform vec3 cam;
		uniform float time;
		uniform vec2 offset;
float mist;
		float perlin() {
			uint ix, iy, iz;
			float scale = 0.01;
			float gain = 0.9;
			float fdeep = 2.0 + .005 / max(depth * 0.04, .001);
			int octaves = int(fdeep);
			float octd = fdeep - float(octaves);
			float hf = 0.0;
			float hfval = 0.0;
			vec3 ploc = texloc;	ploc.x += offset.x;	ploc.z += offset.y;
			for (int i = 0; i < octaves; i++) {
				hf += hfval * gain;
				float hfx = ploc.x * scale;
				float hfy = ploc.y * scale;
				float hfz = ploc.z * scale;
				hfx = fract(hfx) * 25.0;	hfy = fract(hfy) * 25.0;	hfz = fract(hfz) * 25.0;
				ix = uint(hfx);	iy = uint(hfy);	iz = uint(hfz);
				float dx = hfx - ix;	float dy = hfy - iy;	float dz = hfz - iz;
				hfx = rv[ix];	hfy = rv[iy];	hfz = rv[iz];
				ix++;	iy++;	iz++;
				float vt0 = fract(hfx + hfy + hfz);	//	vertices of cube for interpolation
				float vt1 = fract(rv[ix] + hfy + hfz);
				float vt2 = fract(hfx + hfy + rv[iz]);
				float vt3 = fract(rv[ix] + hfy + rv[iz]);
				float vt4 = fract(hfx + rv[iy] + hfz);
				float vt5 = fract(rv[ix] + rv[iy] + hfz);
				float vt6 = fract(hfx + rv[iy] + rv[iz]);
				float vt7 = fract(rv[ix] + rv[iy] + rv[iz]);
				dx *= dx * (3.0 - dx - dx);
				dy *= dy * (3.0 - dy - dy);
				dz *= dz * (3.0 - dz - dz);
				//float vi0 = vt0 + dx * (vt1 - vt0);
				//float vi1 = vt2 + dx * (vt3 - vt2);
				//float vi2 = vt4 + dx * (vt5 - vt4);
				//float vi3 = vt6 + dx * (vt7 - vt6);
				float vi0 = mix(vt0, vt1, dx);
				float vi1 = mix(vt2, vt3, dx);
				float vi2 = mix(vt4, vt5, dx);
				float vi3 = mix(vt6, vt7, dx);

				//vi0 += dz * (vi1 - vi0);
				//vi2 += dz * (vi3 - vi2);
				//vi0 += dy * (vi2 - vi0);
				vi0 = mix(vi0, vi1, dz);
				vi2 = mix(vi2, vi3, dz);
				vi0 = mix(vi0, vi2, dy);
			//	vi0 = abs(1.0 - vi0 - vi0);
			//	hf += vi0 * gain
				hfval = vi0;
				scale *= 2.1872;
				gain *= .72;//.6
			}
			hf += octd * hfval * gain;
			return hf * 0.722;	//hf *= 0.25;
		}

		float ShadowCalculation(vec4 sunfragpos) {
			vec3 projCoords = sunfragpos.xyz;// / sunfragpos.w;
			float shadow;
			if(projCoords.z > 1.0) shadow = 0.56;	//	objects out of buffer not shadowed
			else {
				projCoords = projCoords * 0.5 + 0.5;
				float closestDepth = texture(deepMap, projCoords.xy).r; 
				float currentDepth = projCoords.z;
				float bias = .025 * dot(normal, sun);
				vec2 ts = 1.0 / textureSize(deepMap, 0);
			shadow = currentDepth - bias > closestDepth  ? 0.0 : 0.14;	//	the rest is interpolation

				float sh0 = shadow;	float sh1 = shadow;	float sh2 = shadow;	float sh3 = shadow;	//	tl tr bl br
				vec2 td = vec2(projCoords.xy) * textureSize(deepMap, 0);	td -= floor(td);
				closestDepth = texture(deepMap, projCoords.xy + ts).r;	sh1 += currentDepth - bias > closestDepth ? 0.0 : 0.14;   
				closestDepth = texture(deepMap, projCoords.xy - ts).r;	sh2 += currentDepth - bias > closestDepth ? 0.0 : 0.14;	ts.x = -ts.x;
				closestDepth = texture(deepMap, projCoords.xy + ts).r;	sh0 += currentDepth - bias > closestDepth ? 0.0 : 0.14;   
				closestDepth = texture(deepMap, projCoords.xy - ts).r;	sh3 += currentDepth - bias > closestDepth ? 0.0 : 0.14;	ts.x = 0.0;
				closestDepth = texture(deepMap, projCoords.xy + ts).r;	shadow = currentDepth - bias > closestDepth ? 0.0 : 0.14;
				sh0 += shadow;	sh1 += shadow;
				closestDepth = texture(deepMap, projCoords.xy - ts).r;	shadow = currentDepth - bias > closestDepth ? 0.0 : 0.14; 
				sh2 += shadow;	sh3 += shadow;	ts.x = ts.y;	ts.y = 0.0;
				closestDepth = texture(deepMap, projCoords.xy + ts).r;	shadow = currentDepth - bias > closestDepth ? 0.0 : 0.14;	sh1 += shadow;	sh3 += shadow; 
				closestDepth = texture(deepMap, projCoords.xy - ts).r;	shadow = currentDepth - bias > closestDepth ? 0.0 : 0.14;	sh0 += shadow;	sh2 += shadow;

				sh0 = mix(sh0, sh1, td.x);	sh2 = mix(sh2, sh3, td.x);
				shadow = mix(sh2, sh0, td.y);
			}
			return shadow;
		}  
		
		vec2 rval(vec2 loc) {	//	return fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453);
			vec2 floc = fract(loc * 7.596) + time * 0.1;
			return vec2(cos(fract(dot(loc, floc * 7.596)) * 6.2831853), sin(fract(dot(loc, floc * 7.596)) * 6.2831853));
		}
		vec2 rval2(vec2 loc) {
			vec2 floc = fract(loc * 7.596) + time * 0.1479;
			return vec2(cos(fract(dot(loc, floc * 7.596)) * 6.2831853), sin(fract(dot(loc, floc * 7.596)) * 6.2831853));
		}
		vec3 waves(vec2 loc) {
			vec2 tloc = loc;
			vec2 floc = floor(tloc);
			vec2 dloc = fract(tloc);
			dloc *= dloc * (3.0 - dloc - dloc);
			vec2 a = rval(floc);
			vec2 b = rval(floc + vec2(1.0, 0.0));
			vec2 c = rval(floc + vec2(0.0, 1.0));
			vec2 d = rval(floc + vec2(1.0, 1.0));
			a = mix(a, b, dloc.x);
			c = mix(c, d, dloc.x);
			vec2 sum = mix(a, c, dloc.y) * -0.035 - 0.035;
			tloc = loc * 2.7;
			floc = floor(tloc);
			dloc = fract(tloc);
			dloc *= dloc * (3.0 - dloc - dloc);
			a = rval2(floc);
			b = rval2(floc + vec2(1.0, 0.0));
			c = rval2(floc + vec2(0.0, 1.0));
			d = rval2(floc + vec2(1.0, 1.0));
			a = mix(a, b, dloc.x);
			c = mix(c, d, dloc.x);
			sum += (abs(mix(a, c, dloc.y)) - 1.0) * 0.014 + 0.014;
			sum = mix(a, c, dloc.y) * 0.014 + 0.014;
			return vec3(sum.y, 1.5, sum.x);
		}


		vec2 rc(vec2 loc) {	//	return fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453);
			vec2 floc = fract(loc * .7596);// + time * 0.2;
			float tt = time * 5.0;
			return normalize(vec2(cos(fract(dot(loc, floc * .7596)) * 6.2831853 + tt), sin(fract(dot(loc, floc * .7596)) * 6.2831853 + tt)));
		}
		vec2 rc2(vec2 loc) {	//	return fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453);
			vec2 floc = fract(loc * .7596);
			float tt = time * 27.0;
			return normalize(vec2(cos(fract(dot(loc, floc * .7596)) * 6.2831853 + tt), sin(fract(dot(loc, floc * .7596)) * 6.2831853 + tt)));
		}

		float squareclouds(vec2 loc) {
			vec2 tloc = loc * 0.0025;// - vec2(0.0, time * 4.7);//1.6;
			vec2 floc = floor(tloc);
			vec2 a = rc(floc);
			return max(a.x + a.y - 0.8, 0.0);
		}
		float clouds(vec2 loc) {
			vec2 tloc = loc * 0.0025 - vec2(0.0, time * 4.7);//1.6;
			vec2 floc = floor(tloc);
			vec2 dloc = fract(tloc);
			dloc *= dloc * (3.0 - dloc - dloc);
			vec2 a = rc(floc);
			vec2 b = rc(floc + vec2(1.0, 0.0));
			vec2 c = rc(floc + vec2(0.0, 1.0));
			vec2 d = rc(floc + vec2(1.0, 1.0));
			a = mix(a, b, dloc.x);
			c = mix(c, d, dloc.x);
			vec2 sum = mix(a, c, dloc.y);
			if (mist < 6.0) {
				vec2 t = tloc;
				tloc.x = t.x * .8660254 - t.y * 0.5;
				tloc.y = t.x * 0.5 + t.y * .8660254;
				tloc *= 5.7;//1.6;
				floc = floor(tloc);
				dloc = fract(tloc);
				dloc *= dloc * (3.0 - dloc - dloc);
				a = rc2(floc);
				b = rc2(floc + vec2(1.0, 0.0));
				c = rc2(floc + vec2(0.0, 1.0));
				d = rc2(floc + vec2(1.0, 1.0));
				a = mix(a, b, dloc.x);
				c = mix(c, d, dloc.x);
				a = mix(a, c, dloc.y) * -0.2;
				sum -= a * (sum - 0.5);

				t = tloc;
				tloc.x = t.x * .8660254 - t.y * 0.5;
				tloc.y = t.x * 0.5 + t.y * .8660254;
				tloc *= 4.7;//1.6;
				floc = floor(tloc);
				dloc = fract(tloc);
				dloc *= dloc * (3.0 - dloc - dloc);
				a = rc2(floc);
				b = rc2(floc + vec2(1.0, 0.0));
				c = rc2(floc + vec2(0.0, 1.0));
				d = rc2(floc + vec2(1.0, 1.0));
				a = mix(a, b, dloc.x);
				c = mix(c, d, dloc.x);
				sum -= mix(a, c, dloc.y) * sum * -0.12;
			}
			return max(sum.x * sum.y, -0.01);
		}



		void main() {

			float diffuse;
			float shadow;
//			float mist;
			vec3 col;
			vec3 vray = texloc + cam;
			float occ = color.w;

			float hf;
			float water = -14.0;
			vec3 sealoc;	//	projected coordinate from skybox


			switch (material) {
				case 0://skybox
					if (vray.y < 0.0) {					//	below horizon mostly invisible, shows as gray band
				hf = 0.2;
						col = (ambcol + suncol) * hf;// + diffuse);
						sealoc = ((water + cam.y) / vray.y) * vray - cam;
						mist = distance(sealoc, -cam) * 0.05;
						float norm = dot(sun, vec3(0.0, 1.0, 0.0)) * 5.0 - 1.0;
						col += norm;
						col *= vec3(color);
					}
					else {								//	above horizon
						sealoc = ((200.0 + cam.y) / vray.y) * vray - cam;	//	SKY HEIGHT DEFINED
						mist = exp(distance(vec3(sealoc.x, 7000.0, sealoc.z), -cam) * 0.00025);
					col = vec3(0.5);
						float sol = pow(max(dot(normalize(vray), sun), 0.0), 48);
						col += 4.0 * sol * suncol;
						col *= vec3(color);
						if (mist < 25.0) {
							float cl = clouds(vec2(sealoc.xz) + offset);
							//col += (cl + cl * sun.y * vec3(0.5, 0.8, 1.0)) * (1.0 - sol * 0.7);
							col += (cl + cl * sun.y * vec3(0.3, 0.6, 0.8)) * (1.0 - sol * 0.7);
						}
						else mist = 25.0;
						mist *= 1.0 - sol * 0.96;
					}
					break;

				case 1:
					//hf += 0.7;
					hf += perlin();// * 1.6;		//	height field
				//	diffuse = max(0.0, dot(normal, sun) * 1.7);
diffuse = dot(normal, sun) * 1.7;
//diffuse > 0.0 ? diffuse *= 1.7 : diffuse *= 0.8;
					shadow = ShadowCalculation(sunfragpos);
					col = ambcol * hf + suncol * shadow * (hf + diffuse);
					mist = exp(length(vray) * 0.017);
					if (texloc.y < water) {
						//sealoc = ((water + cam.y) / vray.y) * vray - cam;
						//occ = max(0.0, 1.0 - distance(sealoc, texloc) * 0.03);
						occ = max(0.0, 1.0 - (water - texloc.y) * 0.5);
					}
					col *= vec3(color);
					break;

				case 2:	hf = 0.2;											//	water quad
					//diffuse = max(0.0, dot(normal, sun) * 1.7);
					shadow = ShadowCalculation(sunfragpos);
					col = ambcol * hf + suncol * shadow * hf;// + diffuse);
					sealoc = texloc;
					sealoc.x += offset.x;	sealoc.z += offset.y;
					mist = exp(length(vray) * 0.017);
					occ = 0.7;
					float t = sealoc.x + sealoc.z - time * 300.0;	//	two almost aligned rolling waves 1000, 7.0
					float u = sealoc.x * -1.4 - sealoc.z - time * 0.37;
					float x = (cos(t) - sin(u - t)) * 0.01;
					float z = (sin(time - t) - cos(-u)) * 0.01;
					if (mist > 3.7) x = z = 0.0;

					vec3 wn = waves(vec2(sealoc.xz) * 0.4 - 
						vec2(sin((sealoc.z + sealoc.x * 0.5 - time * 177.0) * 0.73),
							cos((sealoc.x - sealoc.z) * 0.3 - time * 177.0)) * 0.125
						);
					//vec3 wn = waves(vec2(sealoc.xz) * 0.4 - vec2(0.0, time * 15.0));
					
					wn.x += x;	wn.z += z;
					wn = normalize(wn);
					float norm = dot(normalize(vec3(sun.x, 0.4, sun.z)), wn);
					col += (norm - 0.2) * 10.0;
					col *= vec3(color) * norm * (2.0 - norm);

					vec3 reflectDir = reflect(sun, wn);
					vec3 viewDir = normalize(vray);
					float specular = pow(max(dot(viewDir, reflectDir), 0.0), 24);
					mist *= 1.0 - shadow * specular * 0.7;//this widens specular at sunrise
					col *= 0.5 + shadow * 0.5;
					col += 4.0 * shadow * (specular + norm * 0.1) * suncol;
					break;

				default:	sealoc = texloc;	mist = distance(sealoc, -cam) * 0.05;	col *= vec3(color);	break;
			}
			
			//float corona = max(0.0, dot(normalize(vray), sun) * 0.3);
			//col += col * corona * sun.y;

			float hr = exp(-mist * .18);	//.174 pow(420.0 / 650.0, 4.0);
			float hg = exp(-mist * .4);		//.394 pow(420.0 / 530.0, 4.0);
			float hb = exp(-mist * .953);	//.953 pow(420.0 / 425.0, 4.0);420/440 = .83
			mist = 0.0;//sun[1] * 0.05;
			hr = mist + mix(mistcol.r, col.x, hr);
			hg = mist + mix(mistcol.g, col.y, hg);
			hb = mist + mix(mistcol.b, col.z, hb);
			FragColor = vec4(hr, hg, hb, occ);
		} 
	)";


	int  success;
	char infoLog[512];
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);	//	fault checking luxury version
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);	//	do something with infolog
	}
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);	//	do something with infolog
	}
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);	//	do something with infolog
	}
	glUseProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);





	shadowvertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(shadowvertexShader, 1, &shadowVertexShaderSource, NULL);
	glCompileShader(shadowvertexShader);
	glGetShaderiv(shadowvertexShader, GL_COMPILE_STATUS, &success);	//	fault checking luxury version
	if (!success) {
		glGetShaderInfoLog(shadowvertexShader, 512, NULL, infoLog);	//	do something with infolog
	}
	shadowfragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(shadowfragmentShader, 1, &shadowFragmentShaderSource, NULL);
	glCompileShader(shadowfragmentShader);
	glGetShaderiv(shadowfragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(shadowfragmentShader, 512, NULL, infoLog);	//	do something with infolog
	}
	shadowshaderProgram = glCreateProgram();
	glAttachShader(shadowshaderProgram, shadowvertexShader);
	glAttachShader(shadowshaderProgram, shadowfragmentShader);
	glLinkProgram(shadowshaderProgram);
	glGetProgramiv(shadowshaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shadowshaderProgram, 512, NULL, infoLog);	//	do something with infolog
	}
	//glUseProgram(shadowshaderProgram);
	glDeleteShader(shadowvertexShader);
	glDeleteShader(shadowfragmentShader);

glUseProgram(shaderProgram);

	for (int j = 0; j < 16; j++) for (int i = 0; i < 16; i++) {	//	for height field
		rnd = 196314165 * rnd + 907633515;
		float temp = (float)rnd * 1.462918e-9f;	// 2.3283e-10f;
		perlin0[j][i] = cos(temp);	perlin1[j][i] = sin(temp);
	}

	float rvals[] = {
	0.28f, 0.76f, 0.12f, 0.48f, 0.84f, 0.2f, 0.92f, 0.64f, 0.04f, 0.32f,
	0.8f, 0.16f, 0.52f, 0.24f, 0.72f, 0.4f, 0.56f, 0.0f, 0.36f, 0.96f,
	0.08f, 0.68f, 0.88f, 0.44f, 0.6f,
	0.28f, 0.76f, 0.12f, 0.48f, 0.84f, 0.2f, 0.92f, 0.64f, 0.04f, 0.32f,
	0.8f, 0.16f, 0.52f, 0.24f, 0.72f, 0.4f, 0.56f, 0.0f, 0.36f, 0.96f,
	0.08f, 0.68f, 0.88f, 0.44f
	};

	int rvalsLocation = glGetUniformLocation(shaderProgram, "rv");
	glUniform1fv(rvalsLocation, 49, &rvals[0]);


	float tcol[] = { .25f, .22f, .32f };
	//float tcol[] = { .38f, .33f, .48f };
	//float tcol[] = { .50f, .44f, .64f };
	int temp = glGetUniformLocation(shaderProgram, "ambcol");	glUniform3fv(temp, 1, &tcol[0]);
	tcol[0] = .992f;	tcol[1] = .6847f;	tcol[2] = .342f;
	temp = glGetUniformLocation(shaderProgram, "suncol");	glUniform3fv(temp, 1, &tcol[0]);
//	tcol[0] = .67f;		tcol[1] = .63f;		tcol[2] = .77f;
	tcol[0] = .61f;		tcol[1] = .63f;		tcol[2] = .77f;

	temp = glGetUniformLocation(shaderProgram, "mistcol");	glUniform3fv(temp, 1, &tcol[0]);

	makeheightfield(-1);
	makesky();


	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);	//	set as current vao
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);	//	this has to be after vao binding
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glGenBuffers(1, &ebo);	//	element buffer object
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

	//index of uniform/attribute, vecn,,[0,1] normalise flag! :),vecn or 0 to let gl decide, offset
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);	// pointer to the vertex data
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));	//	offset for color
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));	//	offset for normal
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);



	glGenVertexArrays(1, &envao);		//	vao/vbo for skybox
	glBindVertexArray(envao);
	glGenBuffers(1, &envbo);
	glBindBuffer(GL_ARRAY_BUFFER, envbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(envertices), envertices, GL_DYNAMIC_DRAW);	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);					glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenVertexArrays(1, &quadvao);		//	vao/vbo for water quad
	glBindVertexArray(quadvao);
	glGenBuffers(1, &quadvbo);
	glBindBuffer(GL_ARRAY_BUFFER, quadvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadvertices), quadvertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);					glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);





	
	glGenFramebuffers(1, &depthMapFBO);
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.f, 1.f, 1.f, 1.f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);






	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);	//glDisable(GL_CULL_FACE);

glEnable(GL_BLEND);
glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ShowWindow(hOGLWnd, SW_SHOWDEFAULT);	//ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:	{
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }	break;
	case WM_KEYDOWN: {
		if ((HIWORD(lParam) & KF_REPEAT) < 1) {
			switch (LOWORD(wParam)) {
			case 'H': {
				//makeheightfield();
				//glBindBuffer(GL_ARRAY_BUFFER, vbo);	//	this has to be after vao binding
				//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
				//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
				//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);

			}	break;

			case 'Q':		sun[0] -= .1f;	break;
			case 'W':		sun[0] += .1f;	break;
			case 'S':		sun[2] -= .1f;	break;
			case 'X':		sun[2] += .1f;	break;
			case 'D':		sun[1] -= .1f;	break;
			case 'C':		sun[1] += .1f;	break;

			case 'A':		xrv += .005f;	if (xrv > 1.f) xrv = 1.f;	break;
			case 'Z':		xrv -= .005f;	if (xrv < -1.f) xrv = -1.f;	break;
			case VK_LEFT:	yrv += .005f;	if (yrv > 1.f) yrv = 1.f;	break;
			case VK_RIGHT:	yrv -= .005f;	if (yrv < -1.f) yrv = -1.f;	break;
			case VK_UP:		vel += .001f;	if (vel > 1.f) vel = 1.f;	break;
			case VK_DOWN:	vel -= .001f;	if (vel < -1.f) vel = -1.f;	break;
			case VK_ESCAPE:	PostMessage(hWnd, WM_CLOSE, 0, 0);	break;
			case VK_SPACE:  break;
			}
		}
	}   break;
	case WM_LBUTTONDOWN: {
		POINT mouloc;	GetCursorPos(&mouloc);	ScreenToClient(hWnd, &mouloc);
	}   break;
	case WM_CREATE: {
		RECT clientRect;	GetClientRect(hOGLWnd, &clientRect);
		dispx = clientRect.right - clientRect.left;	dispy = clientRect.bottom - clientRect.top;
		//glViewport(0, 0, dispx, dispy);
		cam[1] = -.0145f;
		//cam[1] = .071f;//
		//cam[1] = .025f;


		SetTimer(hWnd, 1, 20, NULL);
	}	break;
    case WM_PAINT:	{
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
		display();


		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(10, 10, 10));

		//char str[32];
		//sprintf_s(str, "%.7f", (float)offx);	TextOutA(hdc, 10, 10, str, strlen(str));
		//sprintf_s(str, "%.7f", (float)offy);	TextOutA(hdc, 10, 25, str, strlen(str));

		//sprintf_s(str, "%.7f", floor(cam[0] * 200.f));    TextOutA(hdc, 10, 10, str, strlen(str));
		//sprintf_s(str, "%.7f", cam[0] * 200.f);    TextOutA(hdc, 10, 25, str, strlen(str));
		//float t = (int)floor(cam[0] * 200.f) >> 3;
		//sprintf_s(str, "%.7f", t);    TextOutA(hdc, 10, 40, str, strlen(str));

        EndPaint(hWnd, &ps);
    }	break;
	case WM_TIMER: {
		InvalidateRgn(hWnd, 0, 0);	UpdateWindow(hWnd);
	}   break;
	case WM_SIZE: {
		RECT clientRect;	GetClientRect(hOGLWnd, &clientRect);
		dispx = clientRect.right - clientRect.left;	dispy = clientRect.bottom - clientRect.top;
		glViewport(0, 0, dispx, dispy);
	}	break;
    case WM_DESTROY:
		glUseProgram(0);
		glDeleteBuffers(1, &vao);
		glDeleteBuffers(1, &vbo);
		if (HDC currentDC = wglGetCurrentDC()) wglMakeCurrent(currentDC, NULL);
		DestroyWindow(hOGLWnd);
		wglDeleteContext(hrc0);
		wglDeleteContext(hrc1);
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

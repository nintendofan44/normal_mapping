// Bump mapping (normal mapping) example
// Texture borrowed from here:
// http://polycount.com/discussion/89636/free-textures-to-use-i-only-ask-one-thing

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <3ds.h>
#include <citro3d.h>
#include <tex3ds.h>
//#include <string.h>
#include "vshader_shbin.h"
#include "diffuse_t3x.h"
#include "normal_t3x.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "inc/modelos/bola.obj.h"

#define CLEAR_COLOR 0x68B0D8FF

#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

namespace {
	//typedef struct { float position[3]; float uv[2]; float normal[3]; float tangent[3]; } vertex;

	//vertex vertex_list[10000] = {};

	/*vertex vertex_list[] =
	{
		// First face (PZ)
		// First triangle
		{ {-0.5f, -0.5f, +0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, +1.0f}, {} },
		{ {+0.5f, -0.5f, +0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, +1.0f}, {} },
		{ {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, +1.0f}, {} },
		// Second triangle
		{ {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, +1.0f}, {} },
		{ {-0.5f, +0.5f, +0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, +1.0f}, {} },
		{ {-0.5f, -0.5f, +0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, +1.0f}, {} },

		// Second face (MZ)
		// First triangle
		{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {} },
		{ {-0.5f, +0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {} },
		{ {+0.5f, +0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {} },
		// Second triangle
		{ {+0.5f, +0.5f, -0.5f}, {1.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {} },
		{ {+0.5f, -0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {} },
		{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, 0.0f, -1.0f}, {} },

		// Third face (PX)
		// First triangle
		{ {+0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {+1.0f, 0.0f, 0.0f}, {} },
		{ {+0.5f, +0.5f, -0.5f}, {1.0f, 0.0f}, {+1.0f, 0.0f, 0.0f}, {} },
		{ {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {+1.0f, 0.0f, 0.0f}, {} },
		// Second triangle
		{ {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {+1.0f, 0.0f, 0.0f}, {} },
		{ {+0.5f, -0.5f, +0.5f}, {0.0f, 1.0f}, {+1.0f, 0.0f, 0.0f}, {} },
		{ {+0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {+1.0f, 0.0f, 0.0f}, {} },

		// Fourth face (MX)
		// First triangle
		{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {} },
		{ {-0.5f, -0.5f, +0.5f}, {1.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {} },
		{ {-0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {} },
		// Second triangle
		{ {-0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {} },
		{ {-0.5f, +0.5f, -0.5f}, {0.0f, 1.0f}, {-1.0f, 0.0f, 0.0f}, {} },
		{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f}, {} },

		// Fifth face (PY)
		// First triangle
		{ {-0.5f, +0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, +1.0f, 0.0f}, {} },
		{ {-0.5f, +0.5f, +0.5f}, {1.0f, 0.0f}, {0.0f, +1.0f, 0.0f}, {} },
		{ {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, +1.0f, 0.0f}, {} },
		// Second triangle
		{ {+0.5f, +0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, +1.0f, 0.0f}, {} },
		{ {+0.5f, +0.5f, -0.5f}, {0.0f, 1.0f}, {0.0f, +1.0f, 0.0f}, {} },
		{ {-0.5f, +0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, +1.0f, 0.0f}, {} },

		// Sixth face (MY)
		// First triangle
		{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {} },
		{ {+0.5f, -0.5f, -0.5f}, {1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {} },
		{ {+0.5f, -0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {} },
		// Second triangle
		{ {+0.5f, -0.5f, +0.5f}, {1.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {} },
		{ {-0.5f, -0.5f, +0.5f}, {0.0f, 1.0f}, {0.0f, -1.0f, 0.0f}, {} },
		{ {-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {} },
	};*/

	DVLB_s* vshader_dvlb;
	shaderProgram_s program;
	int uLoc_projection, uLoc_modelView;
	C3D_Mtx projection;

	C3D_LightEnv lightEnv;
	C3D_LightLut lut_Phong;

	C3D_Light light;
	C3D_Light light2;
	C3D_Light light3;

	void* vbo_data;
	//void* index_data;
	C3D_Tex diffuse_tex, normal_tex;
	float angleX = 0.0, angleY = 0.0;

	// Helper function for loading a texture from memory
	bool loadTextureFromMem(C3D_Tex* tex, C3D_TexCube* cube, const void* data, size_t size)
	{
		Tex3DS_Texture t3x = Tex3DS_TextureImport(data, size, tex, cube, false);
		if (!t3x)
			return false;

		// Delete the t3x object since we don't need it
		Tex3DS_TextureFree(t3x);
		return true;
	}

	float calcTangents(float pos1[3], float pos2[3], float pos3[3], float uv1[3], float uv2[2], float uv3[2], float n[3]) {
		float xp2p1 = (float)(pos2[0] - pos1[0]);
		float yp2p1 = (float)(pos2[1] - pos1[1]);
		float zp2p1 = (float)(pos2[2] - pos1[2]);

		float v2v1[3] = { xp2p1, yp2p1, zp2p1 };

		float xp3p1 = (float)(pos3[0] - pos1[0]);
		float yp3p1 = (float)(pos3[1] - pos1[1]);
		float zp3p1 = (float)(pos3[2] - pos1[2]);

		float v3v1[3] = { xp3p1, yp3p1, zp3p1 };

		float c2c1b = (float)(uv2[1] - uv1[1]);
		float c3c1b = (float)(uv3[1] - uv1[1]);

		float xcmul = v2v1[0] * c3c1b;
		float ycmul = v2v1[1] * c3c1b;
		float zcmul = v2v1[2] * c3c1b;

		float v2v1mulc3c1b[3] = { xcmul, ycmul, zcmul };

		float xcmul2 = v3v1[0] * c2c1b;
		float ycmul2 = v3v1[1] * c2c1b;
		float zcmul2 = v3v1[2] * c2c1b;

		float v3v1mulc2c1b[3] = { xcmul2, ycmul2, zcmul2 };

		float xt = (float)(v2v1mulc3c1b[0] - v3v1mulc2c1b[0]);
		float yt = (float)(v2v1mulc3c1b[1] - v3v1mulc2c1b[1]);
		float zt = (float)(v2v1mulc3c1b[2] - v3v1mulc2c1b[2]);

		float t[3] = { xt, yt, zt };

		float xb = (n[1] * t[2]) - (n[2] * t[1]);
		float yb = (n[2] * t[0]) - (n[0] * t[2]);
		float zb = (n[0] * t[1]) - (n[1] * t[0]);

		float b[3] = { xb, yb, zb };

		float xbcn = (b[1] * n[2]) - (b[2] * n[1]);
		float ybcn = (b[2] * n[0]) - (b[0] * n[2]);
		float zbcn = (b[0] * n[1]) - (b[1] * n[0]);

		float bcrossn[3] = { xbcn, ybcn, zbcn };

		float magnitude = sqrt(bcrossn[0] * bcrossn[0] + bcrossn[1] * bcrossn[1] + bcrossn[2] * bcrossn[2]);
		if (magnitude > +0.00001f) {
			float xret = bcrossn[0] / magnitude;
			float yret = bcrossn[1] / magnitude;
			float zret = bcrossn[2] / magnitude;

			float returnval[3] = { xret, yret, zret };
			return (float)*((float*)returnval);
		}
		else {
			float zeros[3] = { 0.0f, 0.0f, 0.0f };
			return (float)*((float*)zeros);
		}
	}

	vertex get_vertex(vertex _vert[], int idx) {
		return _vert[idx];
	}

	void add_vert(vertex array[], int size, vertex value) {
		array[size] = value;
	}

	void sceneInit(void)
	{
		// Load the vertex shader, create a shader program and bind it
		vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
		shaderProgramInit(&program);
		shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);
		C3D_BindProgram(&program);

		// Get the location of the uniforms
		uLoc_projection = shaderInstanceGetUniformLocation(program.vertexShader, "projection");
		uLoc_modelView = shaderInstanceGetUniformLocation(program.vertexShader, "modelView");

		// Configure attributes for use with the vertex shader
		C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
		AttrInfo_Init(attrInfo);
		AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0=position
		AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // v1=texcoord / uv
		AttrInfo_AddLoader(attrInfo, 2, GPU_FLOAT, 3); // v2=normal
		AttrInfo_AddLoader(attrInfo, 3, GPU_FLOAT, 3); // v3=tangent

		for (unsigned int i = 0; i < (sizeof(vertex_list) / sizeof(vertex_list[0]) / 3) + 1; i++) {
			float tang[3] = { calcTangents(get_vertex(vertex_list, i).position, get_vertex(vertex_list, i + 1).position, get_vertex(vertex_list, i + 2).position, get_vertex(vertex_list, i).uv, get_vertex(vertex_list, i + 1).uv, get_vertex(vertex_list, i + 2).uv, get_vertex(vertex_list, i + 2).normal) };

			std::memcpy(get_vertex(vertex_list, i).tangent, tang, sizeof(tang));
			std::memcpy(get_vertex(vertex_list, i + 1).tangent, tang, sizeof(tang));
			std::memcpy(get_vertex(vertex_list, i + 2).tangent, tang, sizeof(tang));
		}

		// Create the VBO (vertex buffer object)
		vbo_data = linearAlloc(sizeof(vertex_list));
		std::memcpy(vbo_data, vertex_list, sizeof(vertex_list));
		/*index_data = linearAlloc(sizeof(index_list));
		std::memcpy(index_data, index_list, sizeof(index_list));*/

		// Configure buffers
		C3D_BufInfo* bufInfo = C3D_GetBufInfo();
		BufInfo_Init(bufInfo);
		BufInfo_Add(bufInfo, vbo_data, sizeof(vertex), 4, 0x3210);

		// Load the textures and bind them to their respective texture units
		if (!loadTextureFromMem(&diffuse_tex, NULL, diffuse_t3x, diffuse_t3x_size))
			svcBreak(USERBREAK_PANIC);
		if (!loadTextureFromMem(&normal_tex, NULL, normal_t3x, normal_t3x_size))
			svcBreak(USERBREAK_PANIC);
		C3D_TexSetFilter(&diffuse_tex, GPU_LINEAR, GPU_NEAREST);
		C3D_TexSetFilter(&normal_tex, GPU_LINEAR, GPU_NEAREST);
		C3D_TexBind(0, &diffuse_tex);
		C3D_TexBind(1, &normal_tex);

		// Configure the texenv stages:
		// 1) Combine primary fragment lighting color with texture color.
		// 2) Add secondary fragment lighting color (specular component).
		C3D_TexEnv* env = C3D_GetTexEnv(0);
		C3D_TexEnvInit(env);
		C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_FRAGMENT_PRIMARY_COLOR, GPU_CONSTANT); // use constant color instead of 0 for src3
		C3D_TexEnvFunc(env, C3D_RGB, GPU_MODULATE);
		env = C3D_GetTexEnv(1);
		C3D_TexEnvInit(env);
		C3D_TexEnvSrc(env, C3D_Both, GPU_PREVIOUS, GPU_FRAGMENT_SECONDARY_COLOR, GPU_CONSTANT); // use constant color instead of 0 for src3
		C3D_TexEnvFunc(env, C3D_RGB, GPU_ADD);

		const C3D_Material material =
		{
			{ 0.0f, 0.0f, 0.0f }, //ambient
			{ 1.0f, 1.0f, 1.0f }, //diffuse
			{ 0.4f, 0.4f, 0.4f }, //specular0
			{ 0.0f, 0.0f, 0.0f }, //specular1
			{ 0.0f, 0.0f, 0.0f }, //emission
		};

		C3D_LightEnvInit(&lightEnv);
		C3D_LightEnvBind(&lightEnv);
		C3D_LightEnvMaterial(&lightEnv, &material);
		C3D_LightEnvBumpMode(&lightEnv, GPU_BUMP_AS_BUMP);
		C3D_LightEnvBumpNormalZ(&lightEnv, false);
		C3D_LightEnvBumpSel(&lightEnv, 1);

		LightLut_Phong(&lut_Phong, 30);
		C3D_LightEnvLut(&lightEnv, GPU_LUT_D0, GPU_LUTINPUT_LN, false, &lut_Phong);

		C3D_FVec lightVec = FVec4_New(1.0f, -1.0f, -1.25f, 1.0f);
		C3D_FVec lightVec2 = FVec4_New(-1.0f, -1.0f, -1.25f, 1.0f);
		C3D_FVec lightVec3 = FVec4_New(-1.0f, 1.0f, -1.25f, 1.0f);

		C3D_LightInit(&light, &lightEnv);
		C3D_LightColor(&light, 0, 0.9, 0);
		C3D_LightPosition(&light, &lightVec);

		C3D_LightInit(&light2, &lightEnv);
		C3D_LightColor(&light2, 0, 0, 0.9);
		C3D_LightPosition(&light2, &lightVec2);

		C3D_LightInit(&light3, &lightEnv);
		C3D_LightColor(&light3, 0.9, 0, 0);
		C3D_LightPosition(&light3, &lightVec3);
	}

	void sceneRender(float iod)
	{
		// Compute the projection matrix
		Mtx_PerspStereoTilt(&projection, C3D_AngleFromDegrees(40.0f), C3D_AspectRatioTop, 0.01f, 1000.0f, iod, 2.0f, false);

		// Calculate the modelView matrix
		C3D_Mtx modelView;
		Mtx_Identity(&modelView);
		Mtx_Translate(&modelView, 0.0, 0.0, -5.0/* + sinf(angleX)*/, true);
		Mtx_RotateX(&modelView, angleX, true);
		Mtx_RotateY(&modelView, angleY, true);
		Mtx_Scale(&modelView, 1.0f, 1.0f, 1.0f);

		// Update the uniforms
		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projection);
		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView, &modelView);

		// Draw the VBO
		C3D_DrawArrays(GPU_GEOMETRY_PRIM, 0, (sizeof(vertex_list) / sizeof(vertex_list[0])));
		//C3D_DrawElements(GPU_GEOMETRY_PRIM, (sizeof(index_list) / sizeof(index_list[0])), C3D_UNSIGNED_SHORT, index_data); // GPU_TRIANGLES GPU_GEOMETRY_PRIM GPU_TRIANGLE_STRIP GPU_TRIANGLE_FAN
	}

	void sceneExit(void)
	{
		// Free the textures
		C3D_TexDelete(&diffuse_tex);
		C3D_TexDelete(&normal_tex);

		// Free the VBO
		linearFree(vbo_data);
		//linearFree(index_data);

		// Free the shader program
		shaderProgramFree(&program);
		DVLB_Free(vshader_dvlb);
	}
}

int main()
{
	//Init fs
	romfsInit();

	// Initialize graphics
	gfxInitDefault();
	gfxSet3D(true); // Enable stereoscopic 3D
	consoleInit(GFX_BOTTOM, nullptr);
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);

	// Initialize the render targets
	C3D_RenderTarget* targetLeft = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTarget* targetRight = C3D_RenderTargetCreate(240, 400, GPU_RB_RGBA8, GPU_RB_DEPTH24_STENCIL8);
	C3D_RenderTargetSetOutput(targetLeft, GFX_TOP, GFX_LEFT, DISPLAY_TRANSFER_FLAGS);
	C3D_RenderTargetSetOutput(targetRight, GFX_TOP, GFX_RIGHT, DISPLAY_TRANSFER_FLAGS);

	// Initialize the scene
	sceneInit();

	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();

		// Respond to user input
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		float slider = osGet3DSliderState();
		float iod = slider / 3;

		// Rotate the cube each frame
		angleX += C3D_AngleFromDegrees(1.0);
		angleY += C3D_AngleFromDegrees(0.5);

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		{
			C3D_RenderTargetClear(targetLeft, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
			C3D_FrameDrawOn(targetLeft);
			sceneRender(-iod);

			if (iod > 0.0f)
			{
				C3D_RenderTargetClear(targetRight, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
				C3D_FrameDrawOn(targetRight);
				sceneRender(iod);
			}
		}
		C3D_FrameEnd(0);
	}

	// Deinitialize the scene
	sceneExit();

	// Deinitialize graphics
	C3D_Fini();
	gfxExit();

	// De-init fs
	romfsExit();
	return 0;
}
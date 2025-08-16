// Bump mapping (normal mapping) example
// Texture borrowed from here:
// http://polycount.com/discussion/89636/free-textures-to-use-i-only-ask-one-thing

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

// 3d

#include <3ds.h>
#include <citro3d.h>
#include <tex3ds.h>
//#include <string.h>
#include "vshader_shbin.h"

#include "diffuse_t3x.h"
#include "normal_t3x.h"
/*#include "ches_t3x.h"
#include "chesn_t3x.h"*/
#include "beecat_t3x.h"
#include "beecatn_t3x.h"
#include "bloxi_t3x.h"
#include "bloxin_t3x.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "inc/modelos/vert_typedef.h"
#include "inc/modelos/default_cube.obj.h"
#include "inc/modelos/chess_cube.obj.h"
#include "inc/modelos/bola.obj.h"
//#include "inc/modelos/bloxiade.obj.h"
#include "inc/modelos/bloxycola.obj.h"

// 2d

#include <citro2d.h>

#define CLEAR_COLOR 0x68B0D800
#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240
#define DISPLAY_TRANSFER_FLAGS \
	(GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) | \
	GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) | GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) | \
	GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO))

namespace {
	// shader
	DVLB_s* vshader_dvlb;
	shaderProgram_s program;

	// lights
	C3D_LightEnv lightEnv;
	C3D_LightLut lut_Phong;

	C3D_Light light;
	C3D_Light light2;
	C3D_Light light3;

	// model 1
	C3D_Tex diffuse_tex, normal_tex;
	float angleX, angleY = 0.0;
	int uLoc_projection, uLoc_modelView;
	C3D_Mtx projection;

	C3D_Tex beecat_tex;
	C3D_Tex beecatn_tex;
	C3D_Tex bloxi_tex;
	C3D_Tex bloxin_tex;

	C3D_FVec bloxy_rotation = FVec3_New(C3D_AngleFromDegrees(0.0), C3D_AngleFromDegrees(0.0), C3D_AngleFromDegrees(0.0));

	// model 2
	//C3D_Tex ches_tex, chesn_tex;

	unsigned int __pos = 0;
	float xAngle = 12.0;
	float zAngle = 0.0;
	float yOffsetMult = -0.37;
	//int objAmt = 15;

	C2D_TextBuf staticTextBuf;

	C2D_Text txt_helloWorldOutl;
	C2D_Text txt_helloWorld;

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

	C3D_FVec calcTangents(C3D_FVec pos1, C3D_FVec pos2, C3D_FVec pos3, C3D_FVec uv1, C3D_FVec uv2, C3D_FVec uv3, C3D_FVec n) {
		C3D_FVec v2v1 = FVec3_Subtract(pos2, pos1);
		C3D_FVec v3v1 = FVec3_Subtract(pos3, pos1);

		float c2c1b = uv2.y - uv1.y;
		float c3c1b = uv3.y - uv1.y;

		C3D_FVec v2v1mulc3c1b = FVec3_Scale(v2v1, c3c1b);
		C3D_FVec v3v1mulc2c1b = FVec3_Scale(v3v1, c2c1b);

		C3D_FVec t = FVec3_Subtract(v2v1mulc3c1b, v3v1mulc2c1b);
		C3D_FVec b = FVec3_Cross(n, t);
		C3D_FVec bcrossn = FVec3_Cross(b, n);

		if (FVec3_Magnitude(bcrossn) > 0.00001f)
			return FVec3_Normalize(bcrossn);
		else
			return FVec3_New(0.0f, 0.0f, 0.0f);
	}

	vertex get_vertex(std::vector<vertex> _vert, int idx) {
		return _vert[idx];
	}

	void sendAttribs(vertex info) {
		C3D_ImmSendAttrib(info.position[0], info.position[1], info.position[2], 0.0f); // v0 - pos
		C3D_ImmSendAttrib(info.uv[0], info.uv[1], 0.0f, 0.0f); // v1 - uv
		C3D_ImmSendAttrib(info.normal[0], info.normal[1], info.normal[2], 0.0f); // v2 - norm
		C3D_ImmSendAttrib(info.tangent[0], info.tangent[1], info.tangent[2], 0.0f); // v3 - tang
	}

	void populateTangents(std::vector<vertex> _arr, std::vector<unsigned int> indlist = {}) {
		if (indlist.empty()) {
			for (int i = 0; i < (int)_arr.size(); i++) {
				indlist.push_back((unsigned int)i);
			}
		}
		for (int i = 0; i < ((int)indlist.size() / 3); i++) {
			C3D_FVec tangent_vec = calcTangents(
				FVec3_New(get_vertex(_arr, indlist[i]).position[0], get_vertex(_arr, indlist[i]).position[1], get_vertex(_arr, indlist[i]).position[2]),
				FVec3_New(get_vertex(_arr, indlist[i + 1]).position[0], get_vertex(_arr, indlist[i + 1]).position[1], get_vertex(_arr, indlist[i + 1]).position[2]),
				FVec3_New(get_vertex(_arr, indlist[i + 2]).position[0], get_vertex(_arr, indlist[i + 2]).position[1], get_vertex(_arr, indlist[i + 2]).position[2]),
				FVec3_New(get_vertex(_arr, indlist[i]).uv[0], get_vertex(_arr, indlist[i]).uv[1], 0.0f),
				FVec3_New(get_vertex(_arr, indlist[i + 1]).uv[0], get_vertex(_arr, indlist[i + 1]).uv[1], 0.0f),
				FVec3_New(get_vertex(_arr, indlist[i + 2]).uv[0], get_vertex(_arr, indlist[i + 2]).uv[1], 0.0f),
				FVec3_New(get_vertex(_arr, indlist[i]).normal[0], get_vertex(_arr, indlist[i]).normal[1], get_vertex(_arr, indlist[i]).normal[2])
			);
			std::vector<float> tang = {tangent_vec.x, tangent_vec.y, tangent_vec.z};

			std::memcpy(get_vertex(_arr, indlist[i]).tangent, &tang, sizeof(tang));
			std::memcpy(get_vertex(_arr, indlist[i + 1]).tangent, &tang, sizeof(tang));
			std::memcpy(get_vertex(_arr, indlist[i + 2]).tangent, &tang, sizeof(tang));
		}
	}

	void sceneInit(void)
	{
		// Load the vertex shader, create a shader program and bind it
		vshader_dvlb = DVLB_ParseFile((u32*)vshader_shbin, vshader_shbin_size);
		shaderProgramInit(&program);
		shaderProgramSetVsh(&program, &vshader_dvlb->DVLE[0]);

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

		populateTangents(blox_list);
		populateTangents(cube_list);
		populateTangents(sphere_list);

		// Load the textures and bind them to their respective texture units
		if (!loadTextureFromMem(&diffuse_tex, nullptr, diffuse_t3x, diffuse_t3x_size))
			svcBreak(USERBREAK_PANIC);
		if (!loadTextureFromMem(&normal_tex, nullptr, normal_t3x, normal_t3x_size))
			svcBreak(USERBREAK_PANIC);
		/*if (!loadTextureFromMem(&ches_tex, nullptr, ches_t3x, ches_t3x_size))
			svcBreak(USERBREAK_PANIC);
		if (!loadTextureFromMem(&chesn_tex, nullptr, chesn_t3x, chesn_t3x_size))
			svcBreak(USERBREAK_PANIC);*/

		if (!loadTextureFromMem(&beecat_tex, nullptr, beecat_t3x, beecat_t3x_size))
			svcBreak(USERBREAK_PANIC);
		if (!loadTextureFromMem(&beecatn_tex, nullptr, beecatn_t3x, beecatn_t3x_size))
			svcBreak(USERBREAK_PANIC);
		if (!loadTextureFromMem(&bloxi_tex, nullptr, bloxi_t3x, bloxi_t3x_size))
			svcBreak(USERBREAK_PANIC);
		if (!loadTextureFromMem(&bloxin_tex, nullptr, bloxin_t3x, bloxin_t3x_size))
			svcBreak(USERBREAK_PANIC);

		const C3D_Material material =
		{
			{ 0.0f, 0.0f, 0.0f }, //ambient
			{ 1.0f, 1.0f, 1.0f }, //diffuse
			{ 0.6f, 0.6f, 0.6f }, //specular0
			{ 0.0f, 0.0f, 0.0f }, //specular1
			{ 0.906f, 0.486f, 0.561f }, //emission
		};

		C3D_LightEnvInit(&lightEnv);
		C3D_LightEnvBind(&lightEnv);
		C3D_LightEnvMaterial(&lightEnv, &material);
		C3D_LightEnvBumpMode(&lightEnv, GPU_BUMP_AS_BUMP);
		C3D_LightEnvBumpNormalZ(&lightEnv, true);
		C3D_LightEnvBumpSel(&lightEnv, 1);

		LightLut_Phong(&lut_Phong, 30);
		C3D_LightEnvLut(&lightEnv, GPU_LUT_D0, GPU_LUTINPUT_NH, false, &lut_Phong); // we GPU_LUTINPUT_NH in this house

		C3D_FVec lightVec = FVec4_New(1.0f, -1.0f, -1.25f, 1.0f);
		C3D_FVec lightVec2 = FVec4_New(-1.0f, -1.0f, -1.25f, 1.0f);
		C3D_FVec lightVec3 = FVec4_New(-1.0f, 1.0f, -1.25f, 1.0f);

		C3D_LightInit(&light, &lightEnv);
		C3D_LightColor(&light, 0, 0.9, 0);
		C3D_LightPosition(&light, &lightVec);
		C3D_LightShadowEnable(&light, true);

		C3D_LightInit(&light2, &lightEnv);
		C3D_LightColor(&light2, 0, 0, 0.9);
		C3D_LightPosition(&light2, &lightVec2);
		C3D_LightShadowEnable(&light2, true);

		C3D_LightInit(&light3, &lightEnv);
		C3D_LightColor(&light3, 0.9, 0, 0);
		C3D_LightPosition(&light3, &lightVec3);
		C3D_LightShadowEnable(&light3, true);

		staticTextBuf = C2D_TextBufNew(4096);

		char buf[160] = "normal map";

		C2D_TextParse(&txt_helloWorldOutl, staticTextBuf, buf);
		C2D_TextOptimize(&txt_helloWorldOutl);

		C2D_TextParse(&txt_helloWorld, staticTextBuf, buf);
		C2D_TextOptimize(&txt_helloWorld);
	}

	void drawIndices(std::vector<vertex> vt_l, std::vector<unsigned int> ind, bool useTex, bool bump, const C3D_Material mat, C3D_FVec pos, C3D_FVec rot, C3D_FVec scale, C3D_Tex _diffuse, C3D_Tex _bumpmap, GPU_TEXTURE_WRAP_PARAM wrap1, GPU_TEXTURE_WRAP_PARAM wrap2) {
		// Calculate the modelView matrix
		C3D_Mtx modelView;
		Mtx_Identity(&modelView);
		Mtx_Translate(&modelView, pos.x, pos.y, pos.z/* + sinf(angleX)*/, true);
		Mtx_RotateX(&modelView, rot.x, true);
		Mtx_RotateY(&modelView, rot.y, true);
		Mtx_RotateZ(&modelView, rot.z, true);
		Mtx_Scale(&modelView, scale.x, scale.y, scale.z);

		C3D_LightEnvMaterial(&lightEnv, &mat);

		if (useTex) {
			C3D_TexSetWrap(&_diffuse, wrap1, wrap2);
			C3D_TexSetFilter(&_diffuse, GPU_LINEAR, GPU_NEAREST);
			C3D_TexBind(0, &_diffuse);
		}

		if (bump) { 
			C3D_TexSetWrap(&_bumpmap, wrap1, wrap2);
			C3D_TexSetFilter(&_bumpmap, GPU_LINEAR, GPU_NEAREST);
			C3D_TexBind(1, &_bumpmap);
			C3D_LightEnvBumpMode(&lightEnv, GPU_BUMP_AS_BUMP); 
		}
		else { C3D_LightEnvBumpMode(&lightEnv, GPU_BUMP_NOT_USED); }

		// Update the uniforms
		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView, &modelView);

		// GPU_TRIANGLES GPU_GEOMETRY_PRIM GPU_TRIANGLE_STRIP GPU_TRIANGLE_FAN

		C3D_ImmDrawBegin(GPU_TRIANGLES);
		for (unsigned int i = 0; i < ind.size(); i++) {
			sendAttribs(vt_l[ind[i]]);
		}
		C3D_ImmDrawEnd();
	}

	void drawVerticesOnly(std::vector<vertex> vt_l, bool useTex, bool bump, const C3D_Material mat, C3D_FVec pos, C3D_FVec rot, C3D_FVec scale, C3D_Tex _diffuse, C3D_Tex _bumpmap, GPU_TEXTURE_WRAP_PARAM wrap1, GPU_TEXTURE_WRAP_PARAM wrap2) {
		// Calculate the modelView matrix
		C3D_Mtx modelView;
		Mtx_Identity(&modelView);
		Mtx_Translate(&modelView, pos.x, pos.y, pos.z/* + sinf(angleX)*/, true);
		Mtx_RotateX(&modelView, rot.x, true);
		Mtx_RotateY(&modelView, rot.y, true);
		Mtx_RotateZ(&modelView, rot.z, true);
		Mtx_Scale(&modelView, scale.x, scale.y, scale.z);

		C3D_LightEnvMaterial(&lightEnv, &mat);

		if (useTex) {
			C3D_TexSetWrap(&_diffuse, wrap1, wrap2);
			C3D_TexSetFilter(&_diffuse, GPU_LINEAR, GPU_NEAREST);
			C3D_TexBind(0, &_diffuse);
		}

		if (bump) { 
			C3D_TexSetWrap(&_bumpmap, wrap1, wrap2);
			C3D_TexSetFilter(&_bumpmap, GPU_LINEAR, GPU_NEAREST);
			C3D_TexBind(1, &_bumpmap);
			C3D_LightEnvBumpMode(&lightEnv, GPU_BUMP_AS_BUMP); 
		}
		else { C3D_LightEnvBumpMode(&lightEnv, GPU_BUMP_NOT_USED); }

		// Update the uniforms
		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_modelView, &modelView);

		// GPU_TRIANGLES GPU_GEOMETRY_PRIM GPU_TRIANGLE_STRIP GPU_TRIANGLE_FAN

		C3D_ImmDrawBegin(GPU_TRIANGLES);
		for (unsigned int i = 0; i < vt_l.size(); i++) {
			sendAttribs(vt_l[i]);
		}
		C3D_ImmDrawEnd();
	}

	void SceneBind(void) {
		C3D_BindProgram(&program);
		C3D_LightEnvBind(&lightEnv);
		C3D_DepthTest(true, GPU_GREATER, GPU_WRITE_ALL);
		C3D_FragOpMode(GPU_FRAGOPMODE_GL);
		C3D_FragOpShadow(1.0, 0.1);
		C3D_CullFace(GPU_CULL_BACK_CCW);

		// Configure the texenv stages:
		// 1) Combine primary fragment lighting color with texture color.
		// 2) Add secondary fragment lighting color (specular component).
		C3D_TexEnvInit(C3D_GetTexEnv(0));
		C3D_TexEnvSrc(C3D_GetTexEnv(0), C3D_Both, GPU_TEXTURE0, GPU_FRAGMENT_PRIMARY_COLOR, GPU_CONSTANT); // use constant color instead of 0 for src3
		C3D_TexEnvFunc(C3D_GetTexEnv(0), C3D_RGB, GPU_MODULATE);
		C3D_TexEnvInit(C3D_GetTexEnv(1));
		C3D_TexEnvSrc(C3D_GetTexEnv(1), C3D_Both, GPU_PREVIOUS, GPU_FRAGMENT_SECONDARY_COLOR, GPU_CONSTANT); // use constant color instead of 0 for src3
		C3D_TexEnvFunc(C3D_GetTexEnv(1), C3D_RGB, GPU_ADD);

		C3D_TexEnvInit(C3D_GetTexEnv(2));
		C3D_TexEnvInit(C3D_GetTexEnv(3));
		C3D_TexEnvInit(C3D_GetTexEnv(4));
		C3D_TexEnvInit(C3D_GetTexEnv(5));
	}

	void sceneRender(float iod)
	{
		SceneBind();

		// cosf sinf
		C3D_FVec green = FVec4_New(4.0f * cosf(angleY), -(4.0f * cosf(angleY)), 1.25f, 1.0f);
		C3D_FVec red = FVec4_New(-sinf(angleX), 4.0f * cosf(angleY), 1.25f, 1.0f);
		C3D_FVec blue = FVec4_New(-(4.0f * cosf(angleY)), -(4.0f * cosf(angleY)), 1.25f, 1.0f);

		C3D_LightPosition(&light, &green);
		C3D_LightPosition(&light2, &blue);
		C3D_LightPosition(&light3, &red);

		// Compute the projection matrix
		Mtx_PerspStereoTilt(&projection, C3D_AngleFromDegrees(40.0f), C3D_AspectRatioTop, 0.01f, 1000.0f, iod, 2.0f, false);
		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, uLoc_projection, &projection);

		C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA);

		drawIndices(cube_list, cube_index, true, true, {
			{ 0.0f, 0.0f, 0.0f }, //ambient
			{ 1.0f, 1.0f, 1.0f }, //diffuse
			{ 0.6f, 0.6f, 0.6f }, //specular0
			{ 0.0f, 0.0f, 0.0f }, //specular1
			{ 0.906f, 0.486f, 0.561f }, //emission
		}, FVec3_New(0.0, -1.0, -3.0), FVec3_New(C3D_AngleFromDegrees(15.0), C3D_AngleFromDegrees(0.0), C3D_AngleFromDegrees(0.0)), FVec3_New(3.0f, 0.25f, 3.0f), diffuse_tex, normal_tex, GPU_REPEAT, GPU_REPEAT);

		//C3D_FVec shared_rotation = FVec3_New(C3D_AngleFromDegrees(xAngle), angleY, C3D_AngleFromDegrees(zAngle));
		C3D_FVec shared_scale = FVec3_New(0.6f, 0.6f, 0.6f);
		//C3D_FVec shared_scale = FVec3_New(0.125f, 0.125f, 0.125f);

		/*float _y = 0.0;
		for (int i = 1; i <= 15; i++) {
			float aaa = 0.0;
			if (i % 5 == 0) { 
				_y += 1.0;
				aaa = 1.0;
			}
			drawIndices(vertex_list, index_list, true, true, {
				{ 0.0f, 0.0f, 0.0f }, //ambient
				{ 1.0f, 1.0f, 1.0f }, //diffuse
				{ 0.6f, 0.6f, 0.6f }, //specular0
				{ 0.0f, 0.0f, 0.0f }, //specular1
				{ 0.906f, 0.486f, 0.561f }, //emission
			}, FVec3_New(-((i % 5) * 0.4) + 0.8, ((_y - (1.0 * aaa)) * yOffsetMult) + (-yOffsetMult), -3.0), shared_rotation, shared_scale, beecat_tex, beecatn_tex, GPU_REPEAT, GPU_REPEAT);
		}*/
		drawIndices(blox_list, blox_index, true, true, {
				{ 0.0f, 0.0f, 0.0f }, //ambient
				{ 1.0f, 1.0f, 1.0f }, //diffuse
				{ 0.6f, 0.6f, 0.6f }, //specular0
				{ 0.0f, 0.0f, 0.0f }, //specular1
				{ 0.906f, 0.486f, 0.561f }, //emission
		}, FVec3_New(0.0, 0.0, -3.0), bloxy_rotation, shared_scale, bloxi_tex, bloxin_tex, GPU_REPEAT, GPU_REPEAT);

		// Draw the 2d scene
		C2D_Prepare();
		//C2D_TextBufClear(g_dynamicBuf);

		//C2D_DrawRectangle(SCREEN_WIDTH / 2, 0, 0, SCREEN_WIDTH / 50, SCREEN_HEIGHT, C2D_Color32(0x9A, 0x6C, 0xB9, 0xFF), C2D_Color32(0xFF, 0xFF, 0x2C, 0xFF), C2D_Color32(0xD8, 0xF6, 0x0F, 0xFF), C2D_Color32(0x40, 0xEA, 0x87, 0xFF));

		float _cos = /*(cosf(angleY * 1.5f) * 5.0f)*/0.0f;
		float _sen = /*(sinf(angleY * 1.5f) * 5.0f)*/0.0f;

		C2D_DrawText(&txt_helloWorldOutl, C2D_AlignCenter, (SCREEN_WIDTH / 2) + (_cos + 2.85f), 6.0f + _sen, 0.0f, 1.05f, 1.2f);
		C2D_DrawText(&txt_helloWorld, C2D_AlignCenter | C2D_WithColor, (SCREEN_WIDTH / 2) + _cos, 8.0f + _sen, 0.0f, 1.0f, 1.0f, C2D_Color32f(1, 0.8, 0, 1));
		C2D_Flush();
	}

	void sceneExit(void)
	{
		// Free the textures
		C3D_TexDelete(&diffuse_tex);
		C3D_TexDelete(&normal_tex);
		/*C3D_TexDelete(&ches_tex);
		C3D_TexDelete(&chesn_tex);*/
		C3D_TexDelete(&beecat_tex);
		C3D_TexDelete(&beecatn_tex);
		C3D_TexDelete(&bloxi_tex);
		C3D_TexDelete(&bloxin_tex);

		C2D_TextBufDelete(staticTextBuf);

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
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE * 2);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);

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
		u32 kHeld = hidKeysHeld();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

		if (kHeld & KEY_LEFT)
			angleY -= C3D_AngleFromDegrees(2.0f);
		else if (kHeld & KEY_RIGHT)
			angleY += C3D_AngleFromDegrees(2.0f);

		/*/if (kDown & KEY_A)
			yOffsetMult += 0.0625;
		else if (kDown & KEY_B)
			yOffsetMult -= 0.0625;

		if ((kDown & KEY_Y) && objAmt < 15)
			objAmt++;
		else if ((kDown & KEY_X) && objAmt > 1)
			objAmt--;*/

		/*if (kHeld & KEY_A)
			xAngle -= 0.1f;
		else if (kHeld & KEY_B)
			xAngle += 0.1f;
		if (kHeld & KEY_L)
			zAngle -= 0.1f;
		else if (kHeld & KEY_R)
			zAngle += 0.1f;*/

		float slider = osGet3DSliderState();
		float iod = slider / 3;

		// Rotate the cube each frame
		//angleX += C3D_AngleFromDegrees(1.0);
		angleY += C3D_AngleFromDegrees(1.0);

		/*std::printf("\x1b[2;1Hx_Angle:     %6.2f%\x1b[K", xAngle);
		std::printf("\x1b[3;1Hz_Angle:     %6.2f%\x1b[K", zAngle);

		std::printf("\x1b[4;1H--------------------------");

		std::printf("\x1b[5;1HangleX:      %6.2f%\x1b[K", angleX * (M_PI / 180));
		std::printf("\x1b[6;1HangleY:      %6.2f%\x1b[K", angleY * (M_PI / 180));*/

		/*std::printf("\x1b[7;1H--------------------------");*/

		if (kDown & KEY_L)
		{
			/*angleX -= C3D_AngleFromDegrees(2.0f);
			if (angleX < C3D_AngleFromDegrees(-90.0f))
				angleX = C3D_AngleFromDegrees(-90.0f);*/
			if (__pos < cube_list.size())
				__pos += 1;
		}
		else if (kDown & KEY_R)
		{
			/*angleX += C3D_AngleFromDegrees(2.0f);
			if (angleX > C3D_AngleFromDegrees(90.0f))
				angleX = C3D_AngleFromDegrees(90.0f);*/
			if (__pos > 0)
				__pos -= 1;
		}

		if (kHeld) {
			if (KEY_CPAD_UP || KEY_CPAD_DOWN || KEY_CPAD_LEFT || KEY_CPAD_RIGHT) {
				circlePosition circlePad;
				hidCircleRead(std::addressof(circlePad));
				C3D_FVec cpad = FVec3_Normalize(FVec3_New((float)circlePad.dx, (float)circlePad.dy, 0.0));

				std::printf("\x1b[8;1H        dX: %6.2f%\x1b[K", cpad.x);
				std::printf("\x1b[9;1H        dY: %6.2f%\x1b[K", cpad.y);
			}
		}
		else {
			std::printf("\x1b[8;1H        Not holding cpad");
			std::printf("\x1b[9;1H                        ");
		}

		std::printf("\x1b[11;1H        ------------------------");
		std::printf("\x1b[12;1H        | CPU:         %6.2f%%\x1b[K |", C3D_GetProcessingTime() * 6.0f);
		std::printf("\x1b[13;1H        | GPU:         %6.2f%%\x1b[K |", C3D_GetDrawingTime() * 6.0f);
		std::printf("\x1b[14;1H        | CmdBuf:      %6.2f%%\x1b[K |", C3D_GetCmdBufUsage() * 100.0f);
		std::printf("\x1b[15;1H        | Frames:          %6.2ld%\x1b[K |", C3D_FrameCounter(0));
		std::printf("\x1b[16;1H        ------------------------");
		std::printf(std::string("\x1b[17;1H        | Idx:         %6.2f%\x1b[K  |").c_str(), (float)__pos);
		std::printf(std::string("\x1b[18;1H        | Tangent X:   %6.2f%\x1b[K  |").c_str(), (float)cube_list[__pos].tangent[0]);
		std::printf(std::string("\x1b[19;1H        | Tangent Y:   %6.2f%\x1b[K  |").c_str(), (float)cube_list[__pos].tangent[1]);
		std::printf(std::string("\x1b[20;1H        | Tangent Z:   %6.2f%\x1b[K  |").c_str(), (float)cube_list[__pos].tangent[2]);
		std::printf("\x1b[21;1H        ------------------------");

		// spent a good amount of time doing this thing thats under this text just to decide to go back to the older "version" LOL (keeping it here in case i need it again)
		/*std::printf((std::string("\x1b[15;1H Tangent-") + std::to_string(__pos) + std::string(" X: %6.2f%\x1b[K")).c_str(), (float)cube_list[__pos].tangent[0]);
		std::printf((std::string("\x1b[16;1H Tangent-") + std::to_string(__pos) + std::string(" Y: %6.2f%\x1b[K")).c_str(), (float)cube_list[__pos].tangent[1]);
		std::printf((std::string("\x1b[17;1H Tangent-") + std::to_string(__pos) + std::string(" Z: %6.2f%\x1b[K")).c_str(), (float)cube_list[__pos].tangent[2]);*/

		// Render the scene
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
		{
			C2D_SceneTarget(targetLeft);
			C3D_RenderTargetClear(targetLeft, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
			C3D_FrameDrawOn(targetLeft);
			//C2D_TargetClear(targetLeft, C2D_Color32(0xFF, 0xFF, 0xFF, 0x00));
			sceneRender(-iod);

			if (iod > 0.0f)
			{
				C2D_SceneTarget(targetRight);
				C3D_RenderTargetClear(targetRight, C3D_CLEAR_ALL, CLEAR_COLOR, 0);
				C3D_FrameDrawOn(targetRight);
				//C2D_TargetClear(targetRight, C2D_Color32(0xFF, 0xFF, 0xFF, 0x00));
				sceneRender(iod);
			}
		}
		C3D_FrameEnd(0);
	}

	// Deinitialize the scene
	sceneExit();
	C3D_RenderTargetDelete(targetLeft);
	C3D_RenderTargetDelete(targetRight);

	// Deinitialize graphics
	C2D_Fini();
	C3D_Fini();
	gfxExit();

	// De-init fs
	romfsExit();
	return 0;
}
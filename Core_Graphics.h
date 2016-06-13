#pragma once

#ifndef _CORE_GRAPHICS_H_
#define _CORE_GRAPHICS_H_

#define ReleaseCOM(x) if(x) { x->Release(); x = NULL;}

class cGraphics;

class cGraphics
{
protected:
	HWND m_hWnd;
	IDirect3D9 * m_pD3D;
	IDirect3DDevice9 * m_pD3DDevice;
	ID3DXSprite * m_pSprite;

	D3DDISPLAYMODE m_d3ddm;

	BOOL m_Windowed;
	BOOL m_ZBuffer;
	BOOL m_HAL;

	long m_Width;
	long m_Height;
	char m_BPP;

	char m_AmbientRed;
	char m_AmbientGreen;
	char m_AmbientBlue;

public:
	cGraphics();
	~cGraphics();

	IDirect3D9	*GetDirect3DCOM();
	IDirect3DDevice9	*GetDeviceCOM();
	ID3DXSprite		*GetSpriteCOM();

	BOOL Init();
	BOOL SetMode(HWND hWnd, BOOL Windowed, BOOL UseZBuffer, long Width, long Height, char BPP);
	BOOL Shutdown();

	long GetNumDisplayModes(D3DFORMAT format);
	BOOL GetDisplayModeInfo(long Num, D3DDISPLAYMODE *Mode, D3DFORMAT Format);

	char GetFormatBPP(D3DFORMAT Format);
	BOOL CheckFormat(D3DFORMAT Format, BOOL Windowed, BOOL HAL);

	BOOL Display();

	BOOL BeginScene();
	BOOL EndScene();

	BOOL BeginSprite();
	BOOL EndSprite();

	BOOL Clear(long Color = 0, float ZBuffer = 1.0f);
	BOOL ClearDisplay(long Color = 0);
	BOOL ClearZBuffer(float ZBuffer = 1.0f);

	long GetWidth();
	long GetHeight();
	char GetBPP();
	BOOL GetHAL();
	BOOL GetZBuffer();

	BOOL SetPerspective(float FOV = D3DX_PI / 4.0f, float Aspect = 1.3333f, float Near = 1.0f, float Far = 10000.0f);

	BOOL SetWorldPosition(cWorldPosition *WorldPos);
	BOOL SetCamera(cCamera *Camera);
	BOOL SetLight(long Num, cLight *Light);
	BOOL SetMaterial(cMaterial *Material);
	BOOL SetTexture(short Num, cTexture *Texture);

	BOOL SetAmbientLight(char Red, char Green, char Blue);
	BOOL GetAmbientLight(char *Red, char *Green, char *Blue);

	BOOL EnableLight(long Num, BOOL Enable = TRUE);
	BOOL EnableLighting(BOOL Enable = TRUE);
	BOOL EnableZBuffer(BOOL Enable = TRUE);
	BOOL EnableAlphaBlending(BOOL Enable = TRUE, DWORD Src = D3DBLEND_SRCALPHA, DWORD Dest = D3DBLEND_INVSRCALPHA);
	BOOL EnableAlphaTesting(BOOL Enable = TRUE);

};
//纹理类
class cTexture
{
protected:
	cGraphics		*m_Graphics;
	IDirect3DTexture9	*m_Texture;
	unsigned long m_Width, m_Height;

public:
	cTexture();
	~cTexture();

	IDirect3DTexture9	*GetTextureCOM();

	BOOL Load(cGraphics *Graphics, char *Filename, DWORD Transparent = 0,
		D3DFORMAT Format = D3DFMT_UNKNOWN);

	BOOL Create(cGraphics *Graphics, DWORD Width, DWORD Height, D3DFORMAT Format);

	BOOL Create(cGraphics *Graphics, IDirect3DTexture9 *Texture);

	BOOL Free();

	BOOL IsLoaded();

	long GetWidth();
	long GetHeight();

	D3DFORMAT GetFormat();

	BOOL Blit(long DestX, long DestY, long SrcX = 0,
		long SrcY = 0, long Width = 0, long Height = 0,
		float XScale = 1.0f, float YScale = 1.0f,
		D3DCOLOR Color = 0XFFFFFFFF);

};

class cMaterial
{
protected:
	D3DMATERIAL9	m_Material;
public:
	cMaterial();
	D3DMATERIAL9 *GetMaterial();

	BOOL SetDiffuseColor(char Red, char Green, char Blue);
	BOOL GetDiffuseColor(char *Red, char *Green, char *Blue);

	BOOL SetAmbientColor(char Red, char Green, char Blue);
	BOOL GetAmbientColor(char *Red, char *Green, char *Blue);

	BOOL SetSpecularColor(char Red, char Green, char Blue);
	BOOL GetSpecularColor(char *Red, char *Green, char *Blue);

	BOOL SetEmissiveColor(char Red, char Green, char Blue);
	BOOL GetEmissiveColor(char *Red, char *Green, char *Blue);

	BOOL SetPower(float Power);
	float GetPower(float Power);

};

class cLight
{
protected:
	D3DLIGHT9 m_Light;

public:
	cLight();

	D3DLIGHT9 *GetLight();

	BOOL SetType(D3DLIGHTTYPE Type);

	BOOL Move(float XPos, float YPos, float ZPos);
	BOOL MoveRel(float XPos, float YPos, float ZPos);
	BOOL GetPos(float *XPos, float *YPos, float *ZPos);

	BOOL Point(float XFrom, float YFrom, float ZFrom,
		float XAt, float YAt, float ZAt);
	BOOL GetDirection(float *XDir, float *YDir, float *ZDir);

	BOOL SetDiffuseColor(char Red, char Green, char Blue);
	BOOL GetDiffuseColor(char *Red, char *Green, char *Blue);

	BOOL SetSpecularColor(char Red, char Green, char Blue);
	BOOL GetSpecularColor(char *Red, char *Green, char *Blue);

	BOOL SetAmbientColor(char Red, char Green, char Blue);
	BOOL GetAmbientColor(char *Red, char *Green, char *Blue);

	BOOL SetRange(float Range);
	float GetRange();

	BOOL SetFalloff(float Falloff);
	float GetFalloff();

	BOOL SetAttenuation0(float Attenuation);
	float GetAttenuation0();

	BOOL SetAttenuation1(float Attenuation);
	float GetAttenuation1();

	BOOL SetAttenuation2(float Attenuation);
	float GetAttenuation2();

	BOOL SetTheta(float Theta);
	float GetTheta();

	BOOL SetPhi(float Phi);
	float GetPhi();
};








#endif

#include "Core_Global.h"

cGraphics::cGraphics()
{
	m_hWnd = NULL;
	
	m_pD3D = NULL;
	m_pD3DDevice = NULL;
	m_pSprite = NULL;
	m_AmbientRed = m_AmbientGreen = m_AmbientBlue = 255;

	m_Width = 0;
	m_Height = 0;
	m_BPP = 0;

	m_Windowed = TRUE;
	m_ZBuffer = FALSE;
	m_HAL = FALSE;

}

cGraphics::~cGraphics()
{
	Shutdown();
}

BOOL cGraphics::Init()
{
	Shutdown();
}

BOOL cGraphics::Init()
{
	Shutdown();

	if ((m_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
		return FALSE;

	return TRUE;
}

BOOL cGraphics::SetMode(HWND hWnd, BOOL Windowed, BOOL UseZBuffer, long Width,
	long Height, char BPP)
{
	D3DPRESENT_PARAMETERS	d3dpp;
	D3DFORMAT	Format, AltFormat;  //颜色格式
	RECT	WndRect, ClientRect;
	long	WndWidth, WndHeight;
	float	Aspect;

	if ((m_hWnd = hWnd) == NULL)
		return FALSE;
	if (m_pD3D == NULL)
		return FALSE;

	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &m_d3ddm)))
		return FALSE;

	if (!Width)
	{
		if (Windowed == FALSE)
		{
			m_Width = m_d3ddm.Width;
		}
		else
		{
			GetClientRect(m_hWnd, &ClientRect);
			m_Width = ClientRect.right;
		}
	}
	else
	{
		m_Width = Width;
	}

	if (!Height)
	{
		if (Windowed == FALSE)
		{
			m_Height = m_d3ddm.Height;
		}
		else
		{
			GetClientRect(m_hWnd, &ClientRect);
			m_Height = ClientRect.bottom;
		}
	}
	else
	{
		m_Height = Height;
	}

	if (!(m_BPP = BPP) || Windowed == TRUE)
	{
		if (!(m_BPP = GetFormatBPP(m_d3ddm.Format)))
			return FALSE;
	}

	if (Windowed == TRUE)
	{
		GetWindowRect(m_hWnd, &WndRect);
		GetClientRect(m_hWnd, &ClientRect);

		WndWidth = (WndRect.right - (ClientRect.right - m_Width)) - WndRect.left;
		WndHeight = (WndRect.bottom - (ClientRect.bottom - m_Height)) - WndRect.top;

		MoveWindow(m_hWnd, WndRect.left, WndRect.top, WndWidth, WndHeight, TRUE);
	}
	
	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	m_HAL = FALSE;

	if ((m_Windowed = Windowed) == TRUE)
	{
		//窗口模式
		d3dpp.Windowed = TRUE;
		d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;  //指定如何将后台缓冲区内容复制到前台
		d3dpp.BackBufferCount = m_d3ddm.Format; //后台缓冲区的宽度

		if (CheckFormat(m_d3ddm.Format, TRUE, TRUE) == TRUE)
		{
			m_HAL = TRUE;
		}
		else
		{
			if (CheckFormat(m_d3ddm.Format, TRUE, TRUE) == FALSE)
				return FALSE;
		}
	}
	else
	{
		d3dpp.Windowed = FALSE;  //FALSE全屏模式
		d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;

		d3dpp.BackBufferWidth = m_Width;
		d3dpp.BackBufferHeight = m_Height;
		d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;//全屏模式下的刷新率
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	//指定前后台缓冲区的最大交换频率

		if (m_BPP == 32)
		{
			Format = D3DFMT_X8R8G8B8;
			AltFormat = D3DFMT_X8R8G8B8;
		}
		if (m_BPP == 24)
		{
			Format = D3DFMT_R8G8B8;
			AltFormat = D3DFMT_R8G8B8;
		}
		if (m_BPP == 16)
		{
			Format = D3DFMT_R5G6B5;
			AltFormat = D3DFMT_X1R5G5B5;
		}
		if (m_BPP == 8)
		{
			Format = D3DFMT_P8;
			AltFormat = D3DFMT_P8;
		}
		
		if (CheckFormat(Format, FALSE, TRUE) == TRUE)
		{
			m_HAL = TRUE;
		}
		else
		{
			if (CheckFormat(AltFormat, FALSE, TRUE) == TRUE)
			{
				m_HAL = TRUE;
				Format = AltFormat;
			}
			else
			{
				if (CheckFormat(Format, FALSE, FALSE) == FALSE)
				{
					if (CheckFormat(Format, FALSE, FALSE) == FALSE)
						return FALSE;
					else
						Format = AltFormat;
				}
			}
		}
		d3dpp.BackBufferFormat = Format;  //格式终于指定完了
	}

	if ((m_ZBuffer = UseZBuffer) == TRUE)
	{
		d3dpp.EnableAutoDepthStencil = TRUE;
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	}
	else
	{
		d3dpp.EnableAutoDepthStencil = FALSE;
	}

	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		(m_HAL == TRUE) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,
		hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &m_pD3DDevice)))
	{
		// Try to create Direct3D without ZBuffer support 
		if (m_ZBuffer == TRUE)
		{
			m_ZBuffer = FALSE;
			d3dpp.EnableAutoDepthStencil = FALSE;

			if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				(m_HAL == TRUE) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF,
				hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &m_pD3DDevice)))
				return FALSE;
		}
		else
			return FALSE;
	}




}

BOOL cGraphics::CheckFormat(D3DFORMAT format, BOOL Windowed, BOOL HAL)
{
	if (FAILED(m_pD3D->CheckDeviceType(D3DADAPTER_DEFAULT, 
		(HAL == TRUE) ? D3DDEVTYPE_HAL : D3DDEVTYPE_REF, format, format, Windowed)))
		return FALSE;
	
	return TRUE;
}

BOOL cGraphics::Shutdown()
{
	ReleaseCOM(m_pSprite);
	ReleaseCOM(m_pD3DDevice);
	ReleaseCOM(m_pD3D);

	return TRUE;
}

IDirect3D9 *cGraphics::GetDirect3DCOM()
{
	return m_pD3D;
}

IDirect3DDevice9 *cGraphics::GetDeviceCOM()
{
	return m_pD3DDevice;
}

ID3DXSprite *cGraphics::GetSpriteCOM()
{
	return m_pSprite;
}

long cGraphics::GetNumDisplayModes(D3DFORMAT format)
{
	if (m_pD3D == NULL)
		return 0;
	return (long)m_pD3D->GetAdapterModeCount(D3DADAPTER_DEFAULT, format);
}

BOOL cGraphics::GetDisplayModeInfo(long Num, D3DDISPLAYMODE * Mode, D3DFORMAT format)
{
	long Max;

	if (m_pD3D == NULL)
		return FALSE;

	Max = GetNumDisplayModes(format);
	if (Num >= Max)
		return FALSE;

	if (FAILED(m_pD3D->EnumAdapterModes(D3DADAPTER_DEFAULT, format, Num, Mode)))
		return FALSE;

	return TRUE;
}


char cGraphics::GetFormatBPP(D3DFORMAT Format)
{
	switch (Format) {
		// 32 bit modes
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
		return 32;
		break;

		// 24 bit modes
	case D3DFMT_R8G8B8:
		return 24;
		break;

		// 16 bit modes
	case D3DFMT_R5G6B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
	case D3DFMT_A4R4G4B4:
		return 16;
		break;

		// 8 bit modes
	case D3DFMT_A8P8:
	case D3DFMT_P8:
		return 8;
		break;

	default:
		return 0;
	}
}
	
BOOL cGraphics::BeginScene()
{
	if (m_pD3DDevice == NULL)
		return FALSE;

	if (FAILED(m_pD3DDevice->BeginScene()))
		return FALSE;

	return TRUE;
}

BOOL cGraphics::EndScene()
{
	short i;
	if (m_pD3DDevice == NULL)
		return FALSE;

	for (i = 0; i < 8; i++)
		m_pD3DDevice->SetTexture(i, NULL);

	if (FAILED(m_pD3DDevice->EndScene()))
		return FALSE;

	return TRUE;
}
//ID3DXSprite接口渲染2D精灵
BOOL cGraphics::BeginSprite()
{
	if (m_pSprite == NULL)
		return FALSE;

	if (FAILED(m_pSprite->Begin()))
		return FALSE;

	return TRUE;
}

BOOL cGraphics::EndSprite()	
{
	if (m_pSprite == NULL)
		return FALSE;

	if (FAILED(m_pSprite->End()))
		return FALSE;

	return TRUE;
}
//画面翻转
BOOL cGraphics::Display()
{
	if (m_pD3DDevice == NULL)
		return FALSE;

	if (FAILED(m_pD3DDevice->Present(NULL, NULL, NULL, NULL)))
		return FALSE;

	return TRUE;
}
//画面清除
BOOL cGraphics::Clear(long Color, float ZBuffer)
{
	if (m_pD3DDevice == NULL)
		return FALSE;
	// Only clear screen if no zbuffer
	if (m_ZBuffer == FALSE)
		return ClearDisplay(Color);

	//zbuffer和屏幕全清除
	if (FAILED(m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
		Color, ZBuffer, 0)))
		return FALSE;

	return TRUE;
}

BOOL cGraphics::ClearDisplay(long Color)
{
	if (m_pD3DDevice == NULL)
		return FALSE;

	if (FAILED(m_pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET, Color, 1.0f, 0)))
		return FALSE;
	return TRUE;
}

BOOL cGraphics::ClearZBuffer(float ZBuffer)
{
	if (m_pD3DDevice == NULL || m_ZBuffer == FALSE)
		return FALSE;

	if (FAILED(m_pD3DDevice->Clear(0, NULL, D3DCLEAR_ZBUFFER, 0, ZBuffer, 0)))
		return FALSE;

	return TRUE;
}

long cGraphics::GetWidth()
{
	return m_Width;
}

long cGraphics::GetHeight()
{
	return m_Height;
}

char cGraphics::GetBPP()
{
	return m_BPP;
}

BOOL cGraphics::GetHAL()
{
	return m_HAL;
}

BOOL cGraphics::GetZBuffer()
{
	return m_ZBuffer;
}
//环境光
BOOL cGraphics::SetAmbientLight(char Red, char Green, char Blue)
{
	D3DCOLOR Color;

	if (m_pD3DDevice == NULL)
		return FALSE;

	Color = D3DCOLOR_XRGB((m_AmbientRed = Red), (m_AmbientGreen = Green),
		(m_AmbientBlue = Blue));
	if (FAILED(m_pD3DDevice->SetRenderState(D3DRS_AMBIENT, Color)))
		return FALSE;
	return TRUE;
}

BOOL cGraphics::GetAmbientLight(char *Red, char *Green, char *Blue)
{
	if (Red != NULL)
		*Red = m_AmbientRed;
	if (Green != NULL)
		*Green = m_AmbientGreen;
	if (Blue != NULL)
		*Blue = m_AmbientBlue;
	return TRUE;
}

//BOOL cGraphics::SetCamera()

//BOOL cGraphics::SetMaterial()

BOOL cGraphics::SetPerspective(float FOV, float Aspect, float Near, float Far)
{
	D3DXMATRIX	matProjection;

	if (m_pD3DDevice == NULL)
		return FALSE;

	D3DXMatrixPerspectiveFovLH(&matProjection, FOV, Aspect, Near, Far);
	if (FAILED(m_pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProjection)))
		return FALSE;
	return TRUE;
}

BOOL cGraphics::EnableLight(long Num, BOOL Enable)
{
	if (m_pD3DDevice == NULL)
		return FALSE;

	if (FAILED(m_pD3DDevice->LightEnable(Num, Enable)))
		return FALSE;

	return TRUE;
}

BOOL cGraphics::EnableLighting(BOOL Enable)
{
	if (m_pD3DDevice == NULL)
		return FALSE;

	if (FAILED(m_pD3DDevice->SetRenderState(D3DRS_LIGHTING, Enable)))
		return FALSE;
	return TRUE;
}

BOOL cGraphics::EnableZBuffer(BOOL Enable)
{
	if (m_pD3DDevice == NULL || m_ZBuffer == FALSE)
		return FALSE;
	if (FAILED(m_pD3DDevice->SetRenderState(D3DRS_ZENABLE, (Enable == TRUE) ? D3DZB_TRUE : D3DZB_FALSE)))
		return FALSE;
	return TRUE;
}

BOOL cGraphics::EnableAlphaBlending(BOOL Enable, DWORD Src, DWORD Dest)
{
	if (m_pD3DDevice == NULL)
		return FALSE;
	
	if (FAILED(m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, Enable)))
		return FALSE;
	
	if (Enable == TRUE)
	{
		m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, Src);
		m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, Dest);
	}
	return TRUE;
}

BOOL cGraphics::EnableAlphaTesting(BOOL Enable)
{
	if (m_pD3DDevice == NULL)
		return FALSE;

	if (FAILED(m_pD3DDevice->SetRenderState(D3DRS_ALPHATESTENABLE, Enable)))
		return FALSE;

	if (Enable == TRUE)
	{
		m_pD3DDevice->SetRenderState(D3DRS_ALPHAREF, 0x08);
		m_pD3DDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	}
	return TRUE;
}

cTexture::cTexture()
{
	m_Graphics = NULL;
	m_Texture = NULL;
	m_Width = m_Height = 0;
}

cTexture::~cTexture()
{
	Free();
}

BOOL cTexture::Free()
{
	ReleaseCOM(m_Texture);
	m_Graphics = NULL;
	m_Width = m_Height = 0;
	return TRUE;
}

BOOL cTexture::Load(cGraphics *Graphics, char *Filename, DWORD Transparent = 0,
	D3DFORMAT Format)
{
	Free();

	if ((m_Graphics = Graphics) == NULL)
		return FALSE;
	if ((Graphics->GetDeviceCOM() == NULL))
		return FALSE;
	if (Filename == NULL)
		return FALSE;

	if (FAILED(D3DXCreateTextureFormFileEx(Graphics->GetDeviceCOM(),
		Filename, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
		Format, D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE, D3DX_FILTER_TRIANGLE,
		Transparent, NULL, NULL, &m_Texture)))
		return FALSE;

	m_Width = GetWidth();
	m_Height = GetHeight();
	return TRUE;
}

BOOL cTexture::Create(cGraphics *Graphics, DWORD Width, DWORD Height, D3DFORMAT Format)
{
	Free();

	if ((m_Graphics = Graphics) == NULL)
		return FALSE;

	if (FAILED(m_Graphics->GetDeviceCOM()->CreateTexture(Width, Height, 0, 0, Format,
		&m_Texture, NULL)))
		return FALSE;
	return TRUE;
}

BOOL cTexture::Create(cGraphics *Graphics, IDirect3DTexture9 *Texture)
{
	D3DLOCKED_RECT SrcRect, DestRect;
	D3DSURFACE_DESC Desc;

	Free();

	if ((m_Graphics = Graphics) == NULL)
		return FALSE;

	if (Texture == NULL)
		return TRUE;

	Texture->GetLevelDesc(0, &Desc);
	m_Width = Desc.Width;
	m_Height = Desc.Height;
	m_Graphics->GetDeviceCOM()->CreateTexture(m_Width, m_Height, 0, 0, Desc.Format,
		D3DPOOL_MANAGED, &m_Texture, NULL);

	Texture->LockRect(0, &SrcRect, NULL, D3DLOCK_READONLY);
	m_Texture->LockRect(0, &DestRect, NULL, 0);

	memcpy(DestRect.pBits, SrcRect.pBits, SrcRect.Pitch * m_Height);

	m_Texture->UnlockRect(0);
	Texture->UnlockRect(0);
	return TRUE;
}

IDirect3DTexture9 *cTexture::GetTextureCOM()
{
	return m_Texture;
}
//返回纹理的宽度
long cTexture::GetWidth()
{
	D3DSURFACE_DESC d3dsd;

	if (m_Texture == NULL)
		return 0;

	if (FAILED(m_Texture->GetLevelDesc(0, &d3dsd)))
		return 0;

	return d3dsd.Width;

}
//高度
long cTexture::GetHeight()
{
	D3DSURFACE_DESC d3dsd;

	if (m_Texture == NULL)
		return 0;

	if (FAILED(m_Texture->GetLevelDesc(0, &d3dsd)))
		return 0;

	return d3dsd.Height;
}

BOOL cTexture::IsLoaded()
{
	if (m_Texture == NULL)
		return FALSE;
	return TRUE;
}


BOOL cTexture::Blit(long DestX, long DestY, long SrcX, long SrcY,
	long Width, long Height, float XScale, float YScale, D3DCOLOR Color)
{
	RECT Rect;
	ID3DXSprite *pSprite;

	if (m_Texture == NULL)
		return FALSE;
	if (m_Graphics == NULL)
		return FALSE;
	if ((pSprite = m_Graphics->GetSpriteCOM()) == NULL)
		return FALSE;
	
	if (!Width)
		Width = m_Width;
	if (!Height)
		Height = m_Height;

	Rect.left = SrcX;
	Rect.top = SrcY;
	Rect.right = Rect.left + Width;
	Rect.bottom = Rect.top + Height;

	if (FAILED(pSprite->Draw(m_Texture, &Rect, &D3DXVERTOR2(XScale, YScale),
		NULL, 0.0f, &D3DXVECTOR2((float)DestX, (float)DestY),
		Color)))
		return FALSE;
	return TRUE;
}

D3DFORMAT cTexture::GetFormat()
{
	D3DSURFACE_DESC d3dsd;

	if (m_Texture == NULL)
		return D3DFMT_UNKNOWN;

	if (FAILED(m_Texture->GetLevelDesc(0, &d3dsd)))
		return D3DFMT_UNKNOWN;

	return d3dsd.Format;
}

cMaterial::cMaterial()
{
	ZeroMemory(&m_Material, sizeof(D3DMATERIAL9));
	SetDiffuseColor(255, 255, 255);
	SetAmbientColor(255, 255, 255);
	SetSpecularColor(255, 255, 255);
	SetEmissiveColor(0, 0, 0);
	SetPower(1.0f);
}	

BOOL cMaterial::SetDiffuseColor(char Red, char Green, char Blue)
{
	m_Material.Diffuse.r = 1.0f / 255.0f * (float)Red;
	m_Material.Diffuse.g = 1.0f / 255.0f * (float)Green;
	m_Material.Diffuse.b = 1.0f / 255.0f * (float)Blue;
	
	return TRUE;
}

BOOL cMaterial::GetDiffuseColor(char *Red, char *Green, char *Blue)
{
	if (Red != NULL)
		*Red = (char)(255.0f * m_Material.Diffuse.r);
	if (Green != NULL)
		*Green = (char)(255.0f * m_Material.Diffuse.g);
	if (Blue != NULL)
		*Blue = (char)(255.0f * m_Material.Diffuse.b);
	return TRUE;
}

BOOL cMaterial::SetAmbientColor(char Red, char Green, char Blue)
{
	m_Material.Ambient.r = 1.0f / 255.0f * (float)Red;
	m_Material.Ambient.g = 1.0f / 255.0f * (float)Green;
	m_Material.Ambient.b = 1.0f / 255.0f * (float)Blue;

	return TRUE;
}

BOOL cMaterial::GetAmbientColor(char *Red, char *Green, char *Blue)
{
	if (Red != NULL)
		*Red = (char)(255.0f * m_Material.Ambient.r);

	if (Green != NULL)
		*Green = (char)(255.0f * m_Material.Ambient.g);

	if (Blue != NULL)
		*Blue = (char)(255.0f * m_Material.Ambient.b);

	return TRUE;
}

BOOL cMaterial::SetSpecularColor(char Red, char Green, char Blue)
{
	m_Material.Specular.r = 1.0f / 255.0f * (float)Red;
	m_Material.Specular.g = 1.0f / 255.0f * (float)Green;
	m_Material.Specular.b = 1.0f / 255.0f * (float)Blue;

	return TRUE;
}

BOOL cMaterial::GetSpecularColor(char *Red, char *Green, char *Blue)
{
	if (Red != NULL)
		*Red = (char)(255.0f * m_Material.Specular.r);

	if (Green != NULL)
		*Green = (char)(255.0f * m_Material.Specular.g);

	if (Blue != NULL)
		*Blue = (char)(255.0f * m_Material.Specular.b);

	return TRUE;
}

BOOL cMaterial::SetEmissiveColor(char Red, char Green, char Blue)
{
	m_Material.Emissive.r = 1.0f / 255.0f * (float)Red;
	m_Material.Emissive.g = 1.0f / 255.0f * (float)Green;
	m_Material.Emissive.b = 1.0f / 255.0f * (float)Blue;

	return TRUE;
}

BOOL cMaterial::GetEmissiveColor(char *Red, char *Green, char *Blue)
{
	if (Red != NULL)
		*Red = (char)(255.0f * m_Material.Emissive.r);

	if (Green != NULL)
		*Green = (char)(255.0f * m_Material.Emissive.g);

	if (Blue != NULL)
		*Blue = (char)(255.0f * m_Material.Emissive.b);

	return TRUE;
}

BOOL cMaterial::SetPower(float Power)
{
	m_Material.Power = Power;
	return TRUE;
}

float cMaterial::GetPower(float Power)
{
	return m_Material.Power;
}

D3DMATERIAL9 *cMaterial::GetMaterial()
{
	return &m_Material;
}

cLight::cLight()
{
	ZeroMemory(&m_Light, sizeof(D3DLIGHT9));
	SetType(D3DLIGHT_POINT);
	Move(0.0f, 0.0f, 0.0f);
	SetDiffuseColor(255, 255, 255);
	SetAmbientColor(255, 255, 255);
	SetRange(1000.0f);
	SetAttenuation0(1.0f);
}

BOOL cLight::SetType(D3DLIGHTTYPE Type)
{
	m_Light.Type = Type;
	return TRUE;
}

BOOL cLight::Move(float XPos, float YPos, float ZPos)
{
	m_Light.Postion.x = XPos;

}

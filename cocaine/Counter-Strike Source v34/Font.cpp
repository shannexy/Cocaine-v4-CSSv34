#include "Font.hpp"
#include "Vector4.hpp"
#include "Color.hpp"
#include "Memory.hpp"

namespace Direct3D9
{
	struct FontVertex
	{
		Vector4 p;
		std::uint32_t color;
		float tu, tv;
	};

	inline FontVertex CreateFontVertex(const Vector4& p, const Color& color, float tu, float tv)
	{
		FontVertex v =
		{
			p,
			color.Code(),
			tu,
			tv
		};

		return v;
	}

	Font::Font(const char* szName, std::uint32_t uHeight, std::uint32_t uFlags)
	{
		strcpy_s(m_szFontName, sizeof(m_szFontName), szName);

		m_uFontHeight = uHeight;
		m_uFontFlags = uFlags;

		m_uSpacing = 0;

		m_pDevice = nullptr;
		m_pTexture = nullptr;
		m_pVertexList = nullptr;
	}

	Font::~Font()
	{
		InvalidateDeviceObjects();
		DeleteDeviceObjects();
	}

	HRESULT Font::DrawText(float x, float y, const Color& color, const char* szText, std::uint32_t uFlags)
	{
		if (!m_pDevice)
			return E_FAIL;

		m_pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		m_pDevice->SetPixelShader(nullptr);
		m_pDevice->SetTexture(0, m_pTexture);
		m_pDevice->SetStreamSource(0, m_pVertexList, 0, sizeof(FontVertex));

		m_pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
		m_pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

		x -= (float)m_uSpacing;

		float fStartX = x;

		FontVertex* pVertList;

		int triangles = 0;

		m_pVertexList->Lock(0, 0, (void**)&pVertList, D3DLOCK_DISCARD);

		while (*szText)
		{
			char c = *szText++;

			if (c == '\n')
			{
				x = fStartX;
				y += (m_fTexCoords[0][3] - m_fTexCoords[0][1]) * (float)m_uTexHeight;
			}

			if ((c - 32) < 0 || (c - 32) >= 128 - 32)
				continue;

			float tx0 = m_fTexCoords[c - 32][0];
			float ty0 = m_fTexCoords[c - 32][1];
			float tx1 = m_fTexCoords[c - 32][2];
			float ty1 = m_fTexCoords[c - 32][3];

			float w = (tx1 - tx0) * (float)m_uTexWidth / m_fTextScale;
			float h = (ty1 - ty0) * (float)m_uTexHeight / m_fTextScale;

			if (c != ' ')
			{
				*pVertList++ = CreateFontVertex(Vector4(x + 0 - 0.5f, y + h - 0.5f, 0.9f, 1.0f), color, tx0, ty1);
				*pVertList++ = CreateFontVertex(Vector4(x + 0 - 0.5f, y + 0 - 0.5f, 0.9f, 1.0f), color, tx0, ty0);
				*pVertList++ = CreateFontVertex(Vector4(x + w - 0.5f, y + h - 0.5f, 0.9f, 1.0f), color, tx1, ty1);
				*pVertList++ = CreateFontVertex(Vector4(x + w - 0.5f, y + 0 - 0.5f, 0.9f, 1.0f), color, tx1, ty0);
				*pVertList++ = CreateFontVertex(Vector4(x + w - 0.5f, y + h - 0.5f, 0.9f, 1.0f), color, tx1, ty1);
				*pVertList++ = CreateFontVertex(Vector4(x + 0 - 0.5f, y + 0 - 0.5f, 0.9f, 1.0f), color, tx0, ty0);

				triangles += 2;

				if (triangles * 3 > 768)
				{
					m_pVertexList->Unlock();

					m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, triangles);

					pVertList = nullptr;

					m_pVertexList->Lock(0, 0, (void**)&pVertList, D3DLOCK_DISCARD);

					triangles = 0;
				}
			}

			x += w - (2 * m_uSpacing);
		}

		m_pVertexList->Unlock();

		if (triangles)
			m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, triangles);

		return S_OK;
	}

	HRESULT Font::GetTextExtent(const char* szText, SIZE* pSize)
	{
		if (!szText || !pSize)
			return E_FAIL;

		float fRowWidth = 0.0f;
		float fRowHeight = (m_fTexCoords[0][3] - m_fTexCoords[0][1]) * (float)m_uTexHeight;
		float fWidth = 0.0f;
		float fHeight = fRowHeight;

		while (*szText)
		{
			char c = *szText++;

			if (c == '\n')
			{
				fRowWidth = 0.0f;
				fHeight += fRowHeight;
			}

			if ((c - 32) < 0 || (c - 32) >= 128 - 32)
				continue;

			float tx0 = m_fTexCoords[c - 32][0];
			float tx1 = m_fTexCoords[c - 32][2];

			fRowWidth += (tx1 - tx0) * (float)m_uTexWidth - 2.0f * (float)m_uSpacing;

			if (fRowWidth > fWidth)
				fWidth = fRowWidth;
		}

		pSize->cx = (int)fWidth;
		pSize->cy = (int)fHeight;

		return S_OK;
	}

	HRESULT Font::Create(IDirect3DDevice9* pDevice)
	{
		HRESULT hResult = S_OK;
		HFONT hFont = nullptr;
		HFONT hFontPrev = nullptr;
		HDC hDC = nullptr;
		HBITMAP hBitmap = nullptr;
		HGDIOBJ hPrev = nullptr;
		BYTE* pDestRow = nullptr;

		m_pDevice = pDevice;

		m_fTextScale = 1.0f;

		hDC = CreateCompatibleDC(NULL);

		SetMapMode(hDC, MM_TEXT);

		hResult = CreateGdiFont(hDC, &hFont);

		if (FAILED(hResult))
			goto Cleanup;

		hFontPrev = (HFONT)SelectObject(hDC, hFont);

		m_uTexWidth = m_uTexHeight = 128;

		while ((hResult = PaintAlphabet(hDC, true)) == D3DERR_MOREDATA)
		{
			m_uTexWidth *= 2;
			m_uTexHeight *= 2;
		}

		if (FAILED(hResult))
			goto Cleanup;

		D3DCAPS9 Caps;

		m_pDevice->GetDeviceCaps(&Caps);

		if (m_uTexWidth > Caps.MaxTextureWidth)
		{
			m_fTextScale = (float)Caps.MaxTextureWidth / (float)m_uTexHeight;
			m_uTexWidth = m_uTexHeight = Caps.MaxTextureWidth;

			bool bFirstRun = true;

			do
			{
				if (!bFirstRun)
					m_fTextScale *= 0.9f;

				DeleteObject(SelectObject(hDC, hFontPrev));

				hResult = CreateGdiFont(hDC, &hFont);

				if (FAILED(hResult))
					goto Cleanup;

				hFontPrev = (HFONT)SelectObject(hDC, hFont);

				bFirstRun = false;
			} while ((hResult = PaintAlphabet(hDC, true)) == D3DERR_MOREDATA);
		}

		hResult = m_pDevice->CreateTexture(m_uTexWidth, m_uTexHeight, 1, 0, D3DFMT_A4R4G4B4, D3DPOOL_MANAGED, &m_pTexture, NULL);

		if (FAILED(hResult))
			goto Cleanup;

		DWORD* pBitmapBit;

		BITMAPINFO bmi;
		ZeroMemory(&bmi.bmiHeader, sizeof(BITMAPINFOHEADER));

		bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bmi.bmiHeader.biWidth = (int)m_uTexWidth;
		bmi.bmiHeader.biHeight = -(int)m_uTexHeight;
		bmi.bmiHeader.biPlanes = 1;
		bmi.bmiHeader.biCompression = BI_RGB;
		bmi.bmiHeader.biBitCount = 32;

		hBitmap = CreateDIBSection(hDC, &bmi, DIB_RGB_COLORS, (void**)&pBitmapBit, NULL, NULL);

		hPrev = SelectObject(hDC, hBitmap);

		SetTextColor(hDC, RGB(255, 255, 255));
		SetBkColor(hDC, RGB(0, 0, 0));
		SetTextAlign(hDC, TA_TOP);

		hResult = PaintAlphabet(hDC, false);

		if (FAILED(hResult))
			goto Cleanup;

		D3DLOCKED_RECT rect;

		m_pTexture->LockRect(0, &rect, nullptr, 0);

		pDestRow = (BYTE*)rect.pBits;
		WORD* pDest16;
		BYTE bAlpha;

		std::uint32_t x, y;

		for (y = 0; y < m_uTexHeight; y++)
		{
			pDest16 = (WORD*)pDestRow;

			for (x = 0; x < m_uTexWidth; x++)
			{
				bAlpha = (BYTE)((pBitmapBit[m_uTexWidth * y + x] & 0xFF) >> 4);

				if (bAlpha > 0)
				{
					*pDest16++ = (WORD)((bAlpha << 12) | 0xFFF);
				}
				else
				{
					*pDest16++ = NULL;
				}
			}
			pDestRow += rect.Pitch;
		}

		hResult = S_OK;

	Cleanup:
		if (m_pTexture)
			m_pTexture->UnlockRect(0);

		SelectObject(hDC, hPrev);
		SelectObject(hDC, hFontPrev);
		DeleteObject(hBitmap);
		DeleteObject(hFont);
		DeleteDC(hDC);

		return hResult;
	}

	HRESULT Font::RestoreDeviceObjects()
	{
		HRESULT hResult = S_OK;

		if (FAILED(hResult = m_pDevice->CreateVertexBuffer(768 * sizeof(FontVertex), D3DUSAGE_WRITEONLY | D3DUSAGE_DYNAMIC, NULL, D3DPOOL_DEFAULT, &m_pVertexList, NULL)))
			return hResult;

		return S_OK;
	}

	HRESULT Font::InvalidateDeviceObjects()
	{
		Memory::SafeRelease(m_pVertexList);

		return S_OK;
	}

	HRESULT Font::DeleteDeviceObjects()
	{
		Memory::SafeRelease(m_pTexture);

		m_pDevice = nullptr;

		return S_OK;
	}

	std::uint32_t Font::GetFlags() const
	{
		return m_uFontFlags;
	}

	HRESULT Font::CreateGdiFont(HDC hDC, HFONT* pFont)
	{
		auto height = -MulDiv(m_uFontHeight, (int)(GetDeviceCaps(hDC, LOGPIXELSY) * m_fTextScale), 72);

		auto bold = (m_uFontFlags & FONT_CREATE_BOLD) ? FW_BOLD : FW_NORMAL;
		auto italic = (m_uFontFlags & FONT_CREATE_ITALIC) ? TRUE : FALSE;

		*pFont = CreateFontA(height, 0, 0, 0, bold, italic, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY, VARIABLE_PITCH, m_szFontName);

		if (*pFont == NULL)
			return E_FAIL;

		return S_OK;
	}

	HRESULT Font::PaintAlphabet(HDC hDC, bool bMeasureOnly)
	{
		SIZE size;
		char temp[2] = "x";

		if (!GetTextExtentPoint32(hDC, temp, 1, &size))
			return E_FAIL;

		m_uSpacing = (std::uint32_t)std::ceil(size.cy * 0.3f);

		std::uint32_t x = m_uSpacing;
		std::uint32_t y = 0;

		for (char c = 32; c < 127; c++)
		{
			temp[0] = c;

			if (!GetTextExtentPoint32(hDC, temp, 1, &size))
				return E_FAIL;

			if ((std::uint32_t)(x + size.cx + m_uSpacing) > m_uTexWidth)
			{
				x = m_uSpacing;
				y += size.cy + 1;
			}

			if (y + size.cy > m_uTexHeight)
				return D3DERR_MOREDATA;

			if (!bMeasureOnly)
			{
				if (!ExtTextOut(hDC, x + 0, y + 0, ETO_OPAQUE, NULL, temp, 1, NULL))
					return E_FAIL;

				m_fTexCoords[c - 32][0] = (float)(x + 0 - m_uSpacing) / (float)m_uTexWidth;
				m_fTexCoords[c - 32][1] = (float)(y + 0 + 0) / (float)m_uTexHeight;
				m_fTexCoords[c - 32][2] = (float)(x + size.cx + m_uSpacing) / (float)m_uTexWidth;
				m_fTexCoords[c - 32][3] = (float)(y + size.cy + 0) / (float)m_uTexHeight;
			}

			x += size.cx + (2 * m_uSpacing);
		}

		return S_OK;
	}
}

#define GREEN D3DCOLOR_ARGB(0xFF, 0x00, 0xFF, 0x00)
#define RED D3DCOLOR_ARGB(0xFF, 0xFF, 0x00, 0x00)
#define BLACK D3DCOLOR_ARGB(0, 0, 0, 0)
#define PURPLE D3DCOLOR_ARGB(255, 128, 0, 128)



ID3DXFont* CreateFont_(IDirect3DDevice9* pDevice , int h) {
    ID3DXFont* pFont;
    D3DXCreateFont(pDevice, h, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &pFont);
    return pFont;
}



void DrawLine(int x1, int y1, int x2, int y2, ID3DXLine* Line , D3DCOLOR color) {

    D3DXVECTOR2 TWO[2];
    TWO[0] = D3DXVECTOR2(x1, y1);
    TWO[1] = D3DXVECTOR2(x2, y2);

    Line->Draw(TWO, 2, color);
}


void Text(ID3DXFont* pFont , const char* text , RECT rect , D3DCOLOR color) {
    // 使用D3DX绘制文本
    pFont->DrawTextA(NULL, text, -1, &rect, DT_LEFT | DT_NOCLIP, color);
}

//宽字符
void Text(ID3DXFont* pFont, const wchar_t* text, RECT rect, D3DCOLOR color) {
    
    pFont->DrawTextW(NULL, text, -1, &rect, DT_LEFT | DT_NOCLIP, color);
}

//绘制圆圈
void DrawCircle(float centerX , float centerY , int R , D3DCOLOR color, IDirect3DDevice9* pDevice) {

    for (int i = 0; i < 360; i++) {
        int x = centerX + R * cos(i);
        int y = centerY + R * sin(i);
        D3DRECT rectangle = { x, y, x + 2, y + 2 };
        pDevice->Clear(1, &rectangle, D3DCLEAR_TARGET, color, 0.0f, 0);
    }
}


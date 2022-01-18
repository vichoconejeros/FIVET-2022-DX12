#include "pch.h"
#include "XBoxOneBumper.h"
#include "../Common/DirectXHelper.h"

void XBoxOneBumper::Initialize(ComPtr<ID2D1DeviceContext5> d2d1DeviceContext, ComPtr<ID2D1Factory6>d2d1Factory, ComPtr<IDWriteFactory> dWriteFactory, std::wstring label, D2D1::ColorF labelColor) {
	const D2D1_POINT_2F points[] = {
	 D2D1::Point2F( 8,  0),
	 D2D1::Point2F(32,  0),
	 D2D1::Point2F(41,  9),
	 D2D1::Point2F(41, 26),
	 D2D1::Point2F(22, 26),
	 D2D1::Point2F( 0, 16),
	 D2D1::Point2F( 0,  8),
	};

	ID2D1GeometrySink* sink = NULL;

	DX::ThrowIfFailed(d2d1Factory->CreatePathGeometry(&buttonGeometry));
	DX::ThrowIfFailed(buttonGeometry->Open(&sink));

	sink->SetFillMode(D2D1_FILL_MODE_WINDING);
	sink->BeginFigure(points[0], D2D1_FIGURE_BEGIN_FILLED);
	sink->AddLines(points + 1, _countof(points) - 1);
	sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	DX::ThrowIfFailed(sink->Close());

	DX::ThrowIfFailed(d2d1DeviceContext->CreateSolidColorBrush(labelColor, &textBrush));
	DX::ThrowIfFailed(d2d1DeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &buttonBrush));
	DX::ThrowIfFailed(d2d1DeviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF(0xecdb33)), &pressedBrush));
	DX::ThrowIfFailed(dWriteFactory->CreateTextFormat(L"Verdana", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 10, L"pl-PL", &textFormat));
	DX::ThrowIfFailed(textFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER));
	DX::ThrowIfFailed(textFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER));
	buttonLabel = label;
}

void XBoxOneBumper::Render(ComPtr<ID2D1DeviceContext5> d2d1DeviceContext, FLOAT x, FLOAT y, BOOLEAN pressed, BOOL LB) {
	d2d1DeviceContext->BeginDraw();
	
	D2D1::Matrix3x2F scale = D2D1::Matrix3x2F::Scale((LB)?1.0f:-1.0f, 1.0f, { 0.0f,0.0f });
	D2D1::Matrix3x2F translation = D2D1::Matrix3x2F::Translation(x, y);
	D2D1::Matrix3x2F transformation = scale * translation;
	d2d1DeviceContext->SetTransform(transformation);

	d2d1DeviceContext->FillGeometry(buttonGeometry.Get(), buttonBrush.Get());
	if (pressed) {
		d2d1DeviceContext->DrawGeometry(buttonGeometry.Get(), pressedBrush.Get(),3.0f);
	}
	
	D2D1_RECT_F Wnd{};

	FLOAT yOffset = 0.0f;
	Wnd = D2D1::RectF((LB)?0.0f:-80.0f, yOffset, 40.0f, yOffset + 20.0f);
	d2d1DeviceContext->SetTransform(translation);
	d2d1DeviceContext->DrawText(buttonLabel.c_str(), static_cast<UINT32>(buttonLabel.length()), textFormat.Get(), &Wnd, textBrush.Get());

	DX::ThrowIfFailed(d2d1DeviceContext->EndDraw());
}
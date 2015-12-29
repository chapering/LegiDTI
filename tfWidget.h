// ----------------------------------------------------------------------------
// tfWidget.h : a Qt-based widget for transfer function customization
//
// Creation : Nov. 10th 2011
// revision : 
//		- Nov. 25th 2011 Add member TransferFunctionWidget::AddPoint()
//			for customizing transfer function presets (in container/controller 
//			class)
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#ifndef _TF_WIDGET_H_
#define _TF_WIDGET_H_

#include <Qt/QtCore>
#include <Qt/QtGui>

#include <vector>
#include <algorithm>
#include <point.h>

typedef _point_t<float> color_t;

class TransferFunctionWidget;
class imgVolRender;

class TransferPoint {
public:
	friend class TransferFunctionWidget;
	friend class imgVolRender;
	TransferPoint();
	TransferPoint(const TransferPoint& pt);
    TransferPoint(float value, color_t color, float alpha, bool fixed = false);

	~TransferPoint();

public:
	TransferPoint& operator = (const TransferPoint& pt);
	bool operator == (const TransferPoint& pt) const;
	bool operator > (const TransferPoint& pt) const;
	bool operator < (const TransferPoint& pt) const;
    
    float get_alpha();
    const color_t get_rgba() const;
    bool is_selected();

    void set_selected(bool selected);

private:
	bool selected;
	bool fixed;
	bool init;
	int pix_size;
	float value;
	float alpha;
	color_t color;

};

inline bool pointComp(const TransferPoint& pt1, const TransferPoint& pt2 )
{
	return pt1 < pt2;
}

class TransferFunctionWidget : public QDialog {
	Q_OBJECT
public:
	TransferFunctionWidget(QWidget* parent = 0, Qt::WindowFlags f = 0);
	~TransferFunctionWidget();

	const std::vector< TransferPoint >& getPoints();

	void place(int x, int y, int width, int height);

	void updateSize(int x, int y, int width, int height);

	void resizeEvent( QResizeEvent* event );

	void keyPressEvent (QKeyEvent* event);

	void paintEvent(QPaintEvent* event);
        
	void DrawPoints(QPainter*& dc);
	void DrawGrid(QPainter*& dc);
	void DrawAxis(QPainter*& dc);
    void DrawFill(QPainter*& dc);
	void Draw(QPainter*& dc);
           
	void mousePressEvent( QMouseEvent* event );
	void mouseReleaseEvent ( QMouseEvent* event );
	void mouseMoveEvent( QMouseEvent* event );

    // Manipulation functions
    int x_from_value(float value);
    
    float value_from_x(int xc);

    int y_from_alpha(float alpha);

    float alpha_from_y(int yc);

	bool hit_test(int x, int y, TransferPoint& pt);
     
	color_t rgba_from_value( float value );
    
    color_t interpolate(float target, const color_t& val1, const color_t& val2);

	void update();

    void AddPoint(float value, color_t color, float alpha, bool fixed = false);

signals:
	void onTfChanged();
	void close();

private:
	bool mouse_down;
	int m_BorderLeft;
	int m_BorderUp;
	int m_BorderRight;
	int m_BorderDown;

	int m_GridLines;
	int m_TickSize;
	int m_TickBorder;
	int m_labelFontPt;

	int r_fieldHeight;
	int r_fieldWidth;
	int r_rangeWidth;

	int x,y, width, height;
	int prev_x, prev_y;

	float pixel_per_value;

	color_t m_MinMax;
	TransferPoint *cur_pt;
	std::vector< TransferPoint > points;
};

#endif // _TF_WIDGET_H_

/* sts=8 ts=8 sw=80 tw=8 */

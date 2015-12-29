// ----------------------------------------------------------------------------
// tfWidget.cpp : a Qt-based widget for transfer function customization
//
// Creation : Nov. 12th 2011
// revision : 
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#include "tfWidget.h"

using namespace std;

//////////////////////////////////////////////////////////////////////
// implementation of class TransferPoint
//////////////////////////////////////////////////////////////////////
TransferPoint::TransferPoint()
{
	this->value = .0;
	this->alpha = 1.0;

	this->selected = true;
	this->pix_size = 4;
	this->fixed = true;
	this->init = false;
}

TransferPoint::TransferPoint(const TransferPoint& pt)
{
	this->value = pt.value;
	this->color = pt.color;
	this->alpha = pt.alpha;

	this->selected = pt.selected;
	this->pix_size = pt.pix_size;
	this->fixed = pt.fixed;
	this->init = pt.init;
}

TransferPoint::TransferPoint(float value, color_t color, float alpha, bool fixed)
{
	this->value = value;
	// Color is 0 to 255
	this->color = color;
	// Alpha is 0.0 to 1.0
	this->alpha = alpha;

	this->selected = false;
	this->pix_size = 4;
	this->fixed = fixed;
	this->init = false;
}

TransferPoint::~TransferPoint()
{
}

TransferPoint& TransferPoint::operator = (const TransferPoint& pt)
{
	this->value = pt.value;
	this->color = pt.color;
	this->alpha = pt.alpha;

	this->selected = pt.selected;
	this->pix_size = pt.pix_size;
	this->fixed = pt.fixed;
	this->init = pt.init;
	return *this;
}

bool TransferPoint::operator == (const TransferPoint& pt) const
{
	return this->value == pt.value;
}

bool TransferPoint::operator > (const TransferPoint& pt) const
{
	return this->value > pt.value;
}

bool TransferPoint::operator < (const TransferPoint& pt) const
{
	return this->value < pt.value;
}

float TransferPoint::get_alpha()
{
	return this->alpha;
}

const color_t TransferPoint::get_rgba() const
{
	return color_t( this->color.x, this->color.y, this->color.z, this->alpha );
}

bool TransferPoint::is_selected()
{
	return this->selected;
}

void TransferPoint::set_selected(bool selected)
{
	this->selected = selected;
}

//////////////////////////////////////////////////////////////////////
// implementation of class TransferFunctionWidget
//////////////////////////////////////////////////////////////////////
TransferFunctionWidget::TransferFunctionWidget(QWidget* parent, Qt::WindowFlags f)
	: QDialog( parent, f )
{
	// Local Variables
	this->m_BorderLeft = 20;
	this->m_BorderUp = 5;
	this->m_BorderRight = 5;
	this->m_BorderDown = 13;

	this->m_GridLines = 10;
	this->m_TickSize = 4;
	this->m_TickBorder = 2;
	this->m_labelFontPt = 10;

	// The transfer points
	this->points.push_back(TransferPoint(0, color_t(255, 255, 255), 0, true));
	this->points.push_back(TransferPoint(125, color_t(0, 0, 0), 0, false));
	this->points.push_back(TransferPoint(255, color_t(0, 0, 0), 1.0, true));

	this->m_MinMax = color_t(0.0, 255.0);
	
	this->mouse_down = false; 
	this->prev_x = 0;
	this->prev_y = 0;
	this->pixel_per_value = 1.0;
	this->cur_pt = NULL;

	updateSize(0,0,400,150);

}

TransferFunctionWidget::~TransferFunctionWidget()
{
}

const vector< TransferPoint >& TransferFunctionWidget::getPoints() 
{
	return this->points;
}

void TransferFunctionWidget::place(int x, int y, int width, int height)
{
	updateSize( x, y, width, height );
	this->setGeometry( x,y,width,height );
}

void TransferFunctionWidget::updateSize(int x, int y, int width, int height)
{
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;

	move(x,y);
	setFixedSize( width, height );

	// The number of usable pixels on the graph
	this->r_fieldWidth = (width - (this->m_BorderRight + this->m_BorderLeft));
	this->r_fieldHeight = (height - (this->m_BorderUp + this->m_BorderDown));

	// The number of value data points
	this->r_rangeWidth = (this->m_MinMax[1] - this->m_MinMax[0]);
	// Pixels per value
	this->pixel_per_value = float(this->r_fieldWidth) / this->r_rangeWidth;
}

void TransferFunctionWidget::resizeEvent( QResizeEvent* event )
{
	QDialog::resizeEvent( event );
	//window()->layout()->setSizeConstraint( QLayout::SetFixedSize );
	/*
	QRect rect = this->geometry();
	updateSize( rect.x(), rect.y(), rect.width(), rect.height() );
	width = event->size().width();
	height = event->size().height();
	*/
}

void TransferFunctionWidget::keyPressEvent (QKeyEvent* event)
{
	if (event->key() == Qt::Key_Space)
	{
		std::cout << "Reset.\n";
		// The spacebar has been pressed: toggle our state
		this->points.clear();
		this->points.push_back(TransferPoint(0, color_t(255, 255, 255), 0, true));
		this->points.push_back(TransferPoint(255, color_t(0, 0, 0), 1.0, true));
		update();
	}
}

void TransferFunctionWidget::paintEvent(QPaintEvent* event)
{
	QDialog::paintEvent( event );
	QPainter *dc = new QPainter(this);
	this->Draw(dc);
	delete dc;
}
	
void TransferFunctionWidget::DrawPoints(QPainter*& dc)
{
	float x,y, x_c, y_c;

	QBrush brush ( QColor(255,0,0) );
	brush.setStyle( Qt::SolidPattern );
	dc->setBrush( brush );
	
	x = this->x_from_value(this->m_MinMax[0]);
	y = this->y_from_alpha(0.0);
	
	vector< TransferPoint >::iterator pt = this->points.begin();
	for (; pt != this->points.end(); pt++)
	{
		x_c = this->x_from_value(pt->value);
		y_c = this->y_from_alpha(pt->get_alpha());

		dc->setPen(Qt::black);
		dc->drawLine(x, y, x_c, y_c);

		if (pt->selected)
		{
			QPen pen (QColor(0,0,255));
			pen.setWidth (2);
			dc->setPen(pen);
			dc->drawRect(x_c - 3, y_c -3, 6, 6);
		}

		dc->setPen(Qt::red);
		dc->drawRect(x_c - 2, y_c -2, 4, 4);   

		x = x_c;
		y = y_c;
	}
}
		
void TransferFunctionWidget::DrawGrid(QPainter*& dc)
{
	int spacing = this->height/(this->m_GridLines + 1);
	
	dc->setPen( QPen( QColor(218, 218, 218) ) );

	for (int i = 0; i< this->m_GridLines; i++)
	{
		dc->drawLine(x + this->m_BorderLeft, y + (1 + i)*spacing, 
					x + this->m_BorderLeft + this->r_fieldWidth, 
					y + (1 + i)*spacing);
	}
}

void TransferFunctionWidget::DrawAxis(QPainter*& dc)
{
	dc->setPen(QPen(QColor(218,218,218)));
	
	// Horizontal
	dc->drawLine(this->x + this->m_BorderLeft - 2, this->y + this->m_BorderUp, 
				this->x + this->m_BorderLeft + this->r_fieldWidth, this->y + this->m_BorderUp);
	dc->drawLine(this->x + this->m_BorderLeft - 2, this->y + this->r_fieldHeight + this->m_BorderUp, 
				this->x + this->m_BorderLeft + this->r_fieldWidth, this->y + this->r_fieldHeight + this->m_BorderUp);
	// Vertical
	dc->drawLine(this->x + this->m_BorderLeft, this->y + this->m_BorderUp, 
				this->x + this->m_BorderLeft, this->y + this->m_BorderUp + this->r_fieldHeight);
	dc->drawLine(this->x + this->m_BorderLeft + this->r_fieldWidth, this->y + this->m_BorderUp, 
				this->x + this->m_BorderLeft + this->r_fieldWidth, this->y + this->m_BorderUp + this->r_fieldHeight);

	QFont font;
	font.setPointSize( this->m_labelFontPt );
	font.setStyle ( QFont::StyleNormal );
	font.setWeight ( QFont::Normal );
	font.setStyleHint ( QFont::Times );
	dc->setFont( font );

	dc->setPen(QPen(QColor(0,0,0)));
	dc->drawText(this->x , this->y + this->m_labelFontPt, "1.0");
	dc->drawText(this->x , this->y + this->m_BorderUp +  this->r_fieldHeight, "0.0");

	const char * const Opacity[] = {"O", "p", "a", "c", "i", "t", "y"};
	for ( int i = 0; i<7; i++ )
	{
		dc->drawText(this->x + 3, this->y + this->m_BorderUp + (2+i)*this->m_labelFontPt, 
				QString(Opacity[i]));
	}

	dc->drawText((this->m_BorderLeft + this->r_fieldWidth)/2, 
			this->y + this->m_BorderUp + this->r_fieldHeight + this->m_BorderDown, "Values");
}

void TransferFunctionWidget::DrawFill(QPainter*& dc)
{
	float yat = this->y_from_alpha(0.0);
	int x;
	color_t rgba;

	for (int i=0; i < this->r_fieldWidth; i++)
	{
		x = this->m_BorderLeft + this->x + i;
		rgba = this->rgba_from_value(this->value_from_x(x));
		dc->setPen(QPen(QColor(rgba[0], rgba[1], rgba[2])));
		dc->drawLine(x, this->y_from_alpha(rgba[3]), x, yat);
	}
}

void TransferFunctionWidget::Draw(QPainter*& dc)
{
	if (!dc) return;

	// Initialize the wx.BufferedPaintDC, assigning a background
	// colour and a foreground colour (to draw the text)
	dc->setBackground( Qt::white );
	dc->eraseRect( this->x, this->y, this->width, this->height);

	// Draw the transfer function fill
	this->DrawFill(dc);
	// Draw the Grid
	this->DrawGrid(dc);
	// Draw the Axis
	this->DrawAxis(dc);
	// Draw Points
	this->DrawPoints(dc);
}
	   
void TransferFunctionWidget::mousePressEvent( QMouseEvent* event )
{
	int x = event->x();
	int y = event->y();
	this->mouse_down = true;

	vector< TransferPoint >::iterator pt = this->points.begin();

	for (size_t i=0; i < this->points.size(); i++)
	{
		if (this->hit_test(x, y, this->points[i]))
		{
			this->cur_pt = &this->points[i];
			this->points[i].selected = ! this->points[i].selected;
			this->prev_x = x;
			this->prev_y = y;
			return;
		}
	}
	
	this->cur_pt = NULL;
}
		
void TransferFunctionWidget::mouseReleaseEvent ( QMouseEvent* event )
{
	int x = event->x();
	int y = event->y();

	if (!this->cur_pt)
	{
		QColorDialog color_picker(this);
		color_picker.setModal( true );
		color_picker.setOption( QColorDialog::ShowAlphaChannel );
		color_picker.exec();
		if (color_picker.result() == QDialog::Accepted)
		{
			QColor qcolor = color_picker.currentColor();
			this->points.push_back(TransferPoint(this->value_from_x(x), 
						color_t(qcolor.red(), qcolor.green(), qcolor.blue()), 
						this->alpha_from_y(y)));
			std::sort( this->points.begin(), this->points.end(), pointComp );
			this->update();
		}
	}

	this->mouse_down = false; 
}

void TransferFunctionWidget::mouseMoveEvent( QMouseEvent* event )
{
	int x = event->x();
	int y = event->y();

	if (this->cur_pt && this->mouse_down)
	{
		this->cur_pt->alpha = this->alpha_from_y(y);

		if (!this->cur_pt->fixed)
		{
			this->cur_pt->value = this->value_from_x(x);
			std::sort( this->points.begin(), this->points.end(), pointComp );
		}
		
		this->update();
	}
}

// Manipulation functions
int TransferFunctionWidget::x_from_value(float value)
{
	if (value > this->m_MinMax[1])
		return this->r_fieldWidth + this->m_BorderLeft + this->x;
	
	if (value < this->m_MinMax[0])
		return this->m_BorderLeft + this->x;
	
	return this->m_BorderLeft + this->x + round(this->pixel_per_value * value);
}

float TransferFunctionWidget::value_from_x(int xc)
{
	if (xc <= this->x + this->m_BorderLeft)
		return float(this->m_MinMax[0]);
	if (xc >= this->x + this->r_fieldWidth + this->m_BorderLeft)
		return float(this->m_MinMax[1]);

	return float(this->m_MinMax[0]) + 
		this->r_rangeWidth*float(float(xc - this->x - this->m_BorderLeft)/((this->r_fieldWidth)));
}

int TransferFunctionWidget::y_from_alpha(float alpha)
{
	if (alpha < 0)
		return this->m_BorderUp + this->y;
	if (alpha > 1.0)
		return this->y;

	return this->m_BorderUp + this->y + int(this->r_fieldHeight*(1 - alpha));
}

float TransferFunctionWidget::alpha_from_y(int yc)
{
	if (yc < this->y + this->m_BorderUp)
		return 1.0;
	if (yc > this->y + this->m_BorderUp + this->r_fieldHeight)
		return 0.0;

	return 1.0 - float(yc - this->y - this->m_BorderUp)/this->r_fieldHeight;
}

bool TransferFunctionWidget::hit_test(int x, int y, TransferPoint& pt)
{
	int x_c = this->x_from_value(pt.value);
	int y_c = this->y_from_alpha(pt.get_alpha());
	int sz = pt.pix_size;

	if (x <= x_c + sz / 2 && x >= x_c - sz 
		&& y <= y_c + sz && y >= y_c - sz)
			return true; 

	return false; 
}
 
color_t TransferFunctionWidget::rgba_from_value( float value )
{
	color_t ret;
	if (!(value >= this->m_MinMax[0] and value <= this->m_MinMax[1]))
	{
		std::cerr << "Value ouf of Range\n";
		return ret;
	}

	TransferPoint pt_cur, pt_next;
	float value_cur, value_next;
	color_t cur_color, next_color;

	for (size_t i = 0; i < this->points.size()-1; i++)
	{
		pt_cur = this->points[i];
		pt_next = this->points[i+1];
		
		value_cur = pt_cur.value;
		value_next = pt_next.value;

		if (value_cur < value)
		{
			if (value_next > value)
			{
				float target = (value - value_cur)/float(value_next - value_cur);
				
				cur_color = pt_cur.get_rgba();
				next_color = pt_next.get_rgba();
				return this->interpolate(target, cur_color, next_color);
			}
			else if (value_next == value)
			{
				return pt_next.get_rgba();
			}
		}
		else if (value_cur == value)
			return pt_cur.get_rgba();
		else
		{
			std::cerr << "Value " << value << " is weird\n";
			return ret;
		}
	}

	return ret;
}

color_t TransferFunctionWidget::interpolate(float target, const color_t& val1, const color_t& val2)
{
	color_t ret;
	ret.update( 
			target*(val2[0]) + (1.0 - target)*val1[0],
			target*(val2[1]) + (1.0 - target)*val1[1],
			target*(val2[2]) + (1.0 - target)*val1[2],
			target*(val2[3]) + (1.0 - target)*val1[3]
			);
	return ret;
}

void TransferFunctionWidget::update()
{
	onTfChanged();
	QDialog::update();
}

void TransferFunctionWidget::AddPoint(float value, color_t color, float alpha, bool fixed)
{
	this->points.push_back(TransferPoint(value, color, alpha, fixed));
}

/* sts=8 ts=8 sw=80 tw=8 */


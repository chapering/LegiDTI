// ----------------------------------------------------------------------------
// transfer_function.h : a Qt-based widget for transfer function customization
//
// Creation : Nov. 10th 2011
// revision : 
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#include <Qt/QtCore>
#include <Qt/QtGui>

#include <vector>
#include <algorithm>
#include <point.h>

using namespace std;

typedef _point_t<float> color_t;

class TransferFunctionWidget;

class TransferPoint {
private:
	bool selected;
	bool fixed;
	bool init;
	int pix_size;
	float value;
	float alpha;
	color_t color;

protected:
public:
	friend class TransferFunctionWidget;
	TransferPoint()
	{
        this->value = .0;
        this->alpha = 1.0;
    
        this->selected = true;
        this->pix_size = 4;
        this->fixed = true;
		this->init = false;
	}

	TransferPoint(const TransferPoint& pt)
	{
        this->value = pt.value;
        this->color = pt.color;
        this->alpha = pt.alpha;
    
        this->selected = pt.selected;
        this->pix_size = pt.pix_size;
        this->fixed = pt.fixed;
		this->init = pt.init;
	}

    TransferPoint(float value, color_t color, float alpha, bool fixed = false)
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

	~TransferPoint()
	{
	}

	TransferPoint& operator = (const TransferPoint& pt)
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

	bool operator == (const TransferPoint& pt) const
	{
		return this->value == pt.value;
	}

	bool operator > (const TransferPoint& pt) const
	{
		return this->value > pt.value;
	}

	bool operator < (const TransferPoint& pt) const
	{
		return this->value < pt.value;
	}
    
    float get_alpha()
	{
        return this->alpha;
	}

    color_t get_rgba()
	{
        return color_t( this->color.x, this->color.y, this->color.z, this->alpha );
	}
   
    bool is_selected()
	{
        return this->selected;
	}
    
    void set_selected(bool selected)
	{
        this->selected = selected;
	}
};

inline bool pointComp(const TransferPoint& pt1, const TransferPoint& pt2 )
{
	return pt1 > pt2;
}

class TransferFunctionWidget : public QDialog {
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

	int x,y;
	int prev_x, prev_y;

	float pixel_per_value;

	color_t m_MinMax;
	TransferPoint cur_pt;
	std::vector< TransferPoint > points;

	Q_OBJECT
public:
	TransferFunctionWidget(QWidget* parent = 0, Qt::WindowFlags f = 0)
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
        this->points.push_back(TransferPoint(255, color_t(0, 0, 0), 1.0, true));

        this->m_MinMax = color_t(0.0, 255.0);
        
        this->mouse_down = false; 
        this->prev_x = 0;
        this->prev_y = 0;
		this->pixel_per_value = 1.0;
        this->cur_pt.init = false;

		updateSize(0,0,400,200);
	}

	~TransferFunctionWidget()
	{
	}

	void place(int x, int y, int width, int height)
	{
		this->setGeometry( QRect(x,y,width,height) );
		updateSize( x, y, width, height );
	}

	void updateSize(int x, int y, int width, int height)
	{
		this->x = x;
		this->y = y;

        // The number of usable pixels on the graph
        this->r_fieldWidth = (width - (this->m_BorderRight + this->m_BorderLeft));
        this->r_fieldHeight = (height - (this->m_BorderUp + this->m_BorderDown));

        // The number of value data points
        this->r_rangeWidth = (this->m_MinMax[1] - this->m_MinMax[0]);
        // Pixels per value
        this->pixel_per_value = float(this->r_fieldWidth) / this->r_rangeWidth;
	}

	void resizeEvent( QResizeEvent* event )
	{
		QDialog::resizeEvent( event );
		QRect rect = this->geometry();
		updateSize( rect.x(), rect.y(), rect.width(), rect.height() );
	}

	void keyPressEvent (QKeyEvent* event)
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

	void paintEvent(QPaintEvent* event)
	{
		QDialog::paintEvent( event );
		QPainter *dc = new QPainter(this);
        this->Draw(dc);
		delete dc;
	}
        
	void DrawPoints(QPainter*& dc)
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
            
	void DrawGrid(QPainter*& dc)
	{
		QRect rect = this->geometry();
        this->x = rect.x();
		this->y = rect.y();
		int height = rect.height();
        
        int spacing = height/(this->m_GridLines + 1);
        
        dc->setPen( QPen( QColor(218, 218, 218) ) );

		for (int i = 0; i< this->m_GridLines; i++)
		{
            dc->drawLine(x + this->m_BorderLeft, y + (1 + i)*spacing, 
                        x + this->m_BorderLeft + this->r_fieldWidth, 
						y + (1 + i)*spacing);
		}
	}

	void DrawAxis(QPainter*& dc)
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

        dc->drawText(this->x + 2, this->y + this->m_labelFontPt/2, "1.0");
        dc->drawText(this->x + 2, this->y + this->m_BorderUp +  this->r_fieldHeight, "0.0");
    
		const char * const Opacity[] = {"O", "p", "a", "c", "i", "t", "y"};
		for ( int i = 0; i<7; i++ )
		{
            dc->drawText(this->x + 6, this->y + this->m_BorderUp + (2+i)*this->m_labelFontPt, 
					QString(Opacity[i]));
		}

        dc->drawText((this->m_BorderLeft + this->r_fieldWidth)/2, 
				this->y + this->m_BorderUp + this->r_fieldHeight,"Values");
	}

    void DrawFill(QPainter*& dc)
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

	void Draw(QPainter*& dc)
	{
		if (!dc) return;

        // Initialize the wx.BufferedPaintDC, assigning a background
        // colour and a foreground colour (to draw the text)
        dc->setBackground( Qt::white );
        dc->eraseRect( this->geometry() );

        // Draw the transfer function fill
        this->DrawFill(dc);
        // Draw the Grid
        this->DrawGrid(dc);
        // Draw the Axis
        this->DrawAxis(dc);
        // Draw Points
        this->DrawPoints(dc);
	}
           
	void mousePressEvent( QMouseEvent* event )
	{
        int x = event->x();
        int y = event->y();
        this->mouse_down = true;

		vector< TransferPoint >::iterator pt = this->points.begin();
		for (; pt != this->points.end(); pt++)
		{
            if (this->hit_test(x, y, *pt))
			{
                this->cur_pt = *pt;
				this->cur_pt.init = true;
                pt->selected = !pt->selected;
                this->prev_x = x;
                this->prev_y = y;
                return;
			}
		}
        
        this->cur_pt.init = false;
	}
            
	void mouseReleaseEvent ( QMouseEvent* event )
	{
        int x = event->x();
        int y = event->y();

        if (!this->cur_pt.init)
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
				//this->SendChangedEvent();
				this->update();
			}
		}

        this->mouse_down = false; 
	}

	void mouseMoveEvent( QMouseEvent* event )
	{
        int x = event->x();
        int y = event->y();

        if (this->cur_pt.init && this->mouse_down)
		{
            this->cur_pt.alpha = this->alpha_from_y(y);

            if (!this->cur_pt.fixed)
			{
                this->cur_pt.value = this->value_from_x(x);
				std::sort( this->points.begin(), this->points.end(), pointComp );
			}
            
            //this->SendChangedEvent();
            this->update();
		}
	}

	/*
    void SendChangedEvent()
	{
        event = wx.CommandEvent(wx.wxEVT_COMMAND_SLIDER_UPDATED, this->GetId())

        this->GetEventHandler().ProcessEvent(event)
		QEvent 
		QCoreApplication::postEvent( this, event );
	}
	*/

    // Manipulation functions
    int x_from_value(float value)
	{
        if (value > this->m_MinMax[1])
            return this->r_fieldWidth + this->m_BorderLeft + this->x;
        
        if (value < this->m_MinMax[0])
            return this->m_BorderLeft + this->x;
        
        return this->m_BorderLeft + this->x + round(this->pixel_per_value * value);
	}
    
    float value_from_x(int xc)
	{
        if (xc <= this->x + this->m_BorderLeft)
            return float(this->m_MinMax[0]);
        if (xc >= this->x + this->r_fieldWidth + this->m_BorderLeft)
            return float(this->m_MinMax[1]);

        return float(this->m_MinMax[0]) + 
			this->r_rangeWidth*float(float(xc - this->x - this->m_BorderLeft)/((this->r_fieldWidth)));
	}

    int y_from_alpha(float alpha)
	{
        if (alpha < 0)
            return this->m_BorderUp + this->y;
        if (alpha > 1.0)
            return this->y;

        return this->m_BorderUp + this->y + int(this->r_fieldHeight*(1 - alpha));
	}

    float alpha_from_y(int yc)
	{
        if (yc < this->y + this->m_BorderUp)
            return 1.0;
        if (yc > this->y + this->m_BorderUp + this->r_fieldHeight)
            return 0.0;

        return 1.0 - float(yc - this->y - this->m_BorderUp)/this->r_fieldHeight;
	}

	bool hit_test(int x, int y, TransferPoint& pt)
	{
        int x_c = this->x_from_value(pt.value);
        int y_c = this->y_from_alpha(pt.get_alpha());
        int sz = pt.pix_size;

        if (x <= x_c + sz / 2 && x >= x_c - sz 
            && y <= y_c + sz && y >= y_c - sz)
                return true; 

        return false; 
	}
     
	color_t rgba_from_value( float value )
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
    
    color_t interpolate(float target, const color_t& val1, const color_t& val2)
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
};


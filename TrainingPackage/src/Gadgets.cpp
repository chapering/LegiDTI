// ----------------------------------------------------------------------------
// VTKgadget.cpp : a set of helper gadget used in visualization experimental study
//				built upon available widget supports of VTK
//
// Creation : Dec. 8th 2011
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#include "Gadgets.h"
#include "cppmoth.h"

#include <vtkObjectFactory.h>
#include <vtkTextProperty.h>
#include <vtkSmartPointer.h>
#include <vmRenderWindow.h>

using namespace std;

vtkStandardNewMacro(CVTKTextbox);
///////////////////////////////////////////////////////////////////////////////
//
// implementation of the CVTKTextbox class
//
CVTKTextbox::CVTKTextbox() : 
	m_x (0),
	m_y (0),
	m_width(200),
	m_height(100)
{
	QRect parentRect = qApp->desktop()->screenGeometry();
	m_x = parentRect.x();
	m_y = parentRect.y();
	m_width = parentRect.width();
	m_height = parentRect.height();
}

CVTKTextbox::~CVTKTextbox()
{
}

void CVTKTextbox::setUseType(char type)
{
	int x = 100, y = 100;
	vtkSmartPointer<vtkTextProperty> textprop = vtkSmartPointer<vtkTextProperty>::New();

	int w = 50, h= 50;
	//GetMinimumSize(w,h);
	SetMinimumSize(w,h);

	switch (type) {
		case UT_TASKBOX:
			{
				textprop->SetFontFamilyToTimes();
				textprop->SetColor(1,1,1);
				textprop->SetFontSize( 30 );
				textprop->ShadowOn();
				textprop->BoldOn();
				textprop->SetJustificationToCentered();
				textprop->SetVerticalJustificationToBottom();

				// middle bottom
				x = m_x + (m_width - w)/2;
				//y = m_y + m_height - h*10;
				y = m_y;
			}
			break;
		case UT_HELPTEXT:
			{
				textprop->SetFontFamilyToCourier();
				textprop->SetColor(1,1,0);
				textprop->SetFontSize( 20 );
				textprop->ItalicOn();
				textprop->SetJustificationToLeft();
				textprop->SetVerticalJustificationToTop();

				// upper left
				x = m_x;
				y = m_y + m_height - h*1.5;
			}
			break;
		case UT_ANSTEXT:
			{
				textprop->SetFontFamilyToTimes();
				textprop->SetColor(1,0,0);
				textprop->SetFontSize( 50 );
				textprop->ItalicOff();
				textprop->SetJustificationToLeft();
				textprop->SetVerticalJustificationToTop();

				// upper center
				x = m_x + w*2.5;
				y = m_y + m_height - h*2.0;
			}
			break;
		default:
			break;
	}

	SetDisplayPosition(x, y); 

	//SetTextScaleModeToProp();
	SetTextProperty( textprop );

	/*
	GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
	GetPositionCoordinate()->SetValue(0.6, 0.1);
	*/
}

void CVTKTextbox::setRenderWindow(vtkRenderWindow* renwin)
{
	if ( !renwin ) return;

	int *pos = renwin->GetPosition();
	//int *size = renwin->GetSize();
	int *size = renwin->GetScreenSize();

	m_x = pos[0], m_y = pos[1];
	m_width = size[0], m_height = size[1];

	//cout << m_x << "," << m_y << "," << m_width << "," << m_height << endl;
}

void CVTKTextbox::setText(const char* text)
{
	SetInput( text );
}

int CVTKTextbox::loadFromFile(const char* fn)
{
	if ( 0 != access ( fn, F_OK|R_OK ) ) { 
		cerr << "Can not access to file " << fn << "\n";
		return -1;
	}

	ifstream ifs(fn);
	string line;
	ostringstream ostr;
	int icnt = 0;
	while ( ifs.good() ) {
		std::getline(ifs, line);
		// skip the annotated but the empty lines
		if (0 == line.compare(0, 1, "#") ||
			0 == line.compare(0, 2, "//") ||
			0 == line.compare(0, 2, "/*") ) {
			continue;
		}
		ostr << line << endl;
		icnt ++;
	}

	SetInput( ostr.str().c_str() );

	return icnt;
}

///////////////////////////////////////////////////////////////////////////////
//
// implementation of the COptionPanel class
//
COptionPanel::COptionPanel(const QString& title, QWidget* parent) : 
	QGroupBox(title, parent)
{
	m_boxlayout = new QVBoxLayout ( parent );
	m_btnNext = new QPushButton(tr("Next"), this);

	this->setStyleSheet( " QRadioButton{ font: serif; font-size: 25pt; color: #ffffff; background-color: black } "
						 " QRadioButton::indicator:unchecked{ font-size: 25pt; color: #000000; background-color: black } "
						 " QRadioButton::indicator:checked{ font-size: 30pt; font-weight: bold; color: #ff0fff; background-color: red } "
						 " QRadioButton::indicator:unchecked:hover{ font-size: 30pt; font-weight: bold; color: #ffff00; background-color: blue } "
						 " QRadioButton::indicator:checked:hover{ font-size: 25pt; font-weight: bold; color: #00ffff; background-color: white } "
						 " QPushButton{ font-size: 20pt; font-weight: bold; color: #ffffff; background-color: green } "
						 " QGroupBox{ font-size: 20pt; color : #ffffff; background-color: #70808f }" );

	m_bInit = false;
	m_nSelectedIdx = -1;
	m_nKey = 0;
	m_strExtra = "";
	m_bPrompt = false;
	OutputStream = &cout;
}

COptionPanel::~COptionPanel()
{
	for (size_t idx = 0; idx < m_buttons.size(); idx++) {
		delete m_buttons[idx];
	}

	delete m_boxlayout;
	delete m_btnNext;
}

void COptionPanel::show()
{
	if ( !m_bInit ) {
		_calcBtnLayout();
		m_bInit = true;
	}
	QGroupBox::show();
}

void COptionPanel::addButton(const std::string& btntext)
{
	QRadioButton* rbtn = new QRadioButton(tr(btntext.c_str()), this);
	m_buttons.push_back( rbtn );

	m_boxlayout->addWidget( rbtn );
}

int COptionPanel::getSelectedBtn()
{
	return m_nSelectedIdx;
}

void COptionPanel::setKey(int optId)
{
	m_nKey = optId;
}

void COptionPanel::setExtraString(const std::string& extrastr)
{
	m_strExtra = extrastr;
}

void COptionPanel::setOutputStream(ostream* os)
{
	OutputStream = os;
}

void COptionPanel::onOptionButtonPressed()
{
	if ( ! parent() || ! parent()->parent()  ) return;
	for (size_t idx = 0; idx < m_buttons.size(); idx++) {
		if ( m_buttons[idx]->isDown() ) {
			(*(MyCout*)OutputStream) << "option " << idx << " hightlighted.\n";
			break;
		}
	}
}

void COptionPanel::onOptionButtonReleased()
{
	if ( ! parent() || ! parent()->parent()  ) return;
	for (int idx = 0; idx < int(m_buttons.size()); idx++) {
		if ( m_buttons[idx]->isChecked() ) {
			if ( m_nSelectedIdx == idx ) break;
			m_nSelectedIdx = idx;

			//((CLegiMainWindow*)parent()->parent())->getcout() << m_nSelectedIdx << " was clicked." << "\n";
			(*(MyCout*)OutputStream) << "option " << m_nSelectedIdx << " clicked." << "\n";

			/* answer prompt */
			if ( m_bPrompt ) {
				string m_strAnswer("");
				ostringstream ostrprmpt;
				m_strAnswer += ('1' + m_nSelectedIdx);

				if ( m_nKey == atoi(m_strAnswer.c_str()) ) {
					m_strAnswer += " (correct).";
					ostrprmpt << "Correct, Nice Job!\n";
				}
				else {
					m_strAnswer += " (wrong).";
					ostrprmpt << "not quite right, the key is ";
				    if (m_bHintBtnText) {
						ostrprmpt << "\"" << m_buttons[m_nKey-1]->text().toStdString() << "\"";
					}
					else {
						ostrprmpt << m_nKey;
					}
					ostrprmpt << ".\n";
				}
				((CLegiMainWindow*)parent()->parent())->getAnsText()->VisibilityOn();
				((CLegiMainWindow*)parent()->parent())->getAnsText()->setText( ostrprmpt.str().c_str() );
				((CLegiMainWindow*)parent()->parent())->getRenderView()->update();
			}
			break;
		}
	}
	/*
	onNextButtonReleased();
	if ( m_nSelectedIdx == -1 ) return;
	cout << m_nSelectedIdx << " clicked.\n";

	m_buttons[m_nSelectedIdx]->setAutoFillBackground(true);

	QPalette myPalette = m_buttons[m_nSelectedIdx]->palette();
	myPalette.setBrush(QPalette::ButtonText, Qt::red);
	myPalette.setBrush(QPalette::Button, Qt::blue);
	m_buttons[m_nSelectedIdx]->setPalette(myPalette);
	*/
}

void COptionPanel::onNextButtonReleased()
{
	if ( ! parent() || ! parent()->parent()  ) return;

	if ( ((CLegiMainWindow*)parent()->parent())->onTaskFinished() == 0 ) {
		//((QWidget*)parent()->parent())->close();
		return;
	}

	if ( m_strExtra.length() >= 1 ) {
		(*(MyCout*)OutputStream) << m_strExtra;
		(*(MyCout*)OutputStream).switchtime(false);
		(*(MyCout*)OutputStream) << "\n";

		((QWidget*)parent()->parent())->close();
		return;
	}
	/*
	for (size_t idx = 0; idx < m_buttons.size(); idx++) {
		if ( m_buttons[idx]->isChecked() ) {
			m_nSelectedIdx = idx;

			if ( ! parent()  ) return;
			((QWidget*)parent()->parent())->close();
		}
	}
	m_nSelectedIdx = -1;
	*/
	if ( m_nSelectedIdx == -1 ) {
		//((CLegiMainWindow*)parent()->parent())->getcout() << "Intend to left without answer.." << "\n";
		(*(MyCout*)OutputStream) << "Intend to leave without answer.." << "\n";
	}
	else {
		//((CLegiMainWindow*)parent()->parent())->getcout() << m_nSelectedIdx << " was chosen.";
		(*(MyCout*)OutputStream) << m_nSelectedIdx << " was chosen.\n";
		//((CLegiMainWindow*)parent()->parent())->getcout().switchtime(false);
		//((CLegiMainWindow*)parent()->parent())->getcout() << "\n";

		string m_strAnswer("");
		m_strAnswer += ('1' + m_nSelectedIdx);

		if ( m_nKey == atoi(m_strAnswer.c_str()) ) {
			m_strAnswer += " (correct).";
		}
		else {
			m_strAnswer += " (wrong).";
		}
		(*(MyCout*)OutputStream) << "Task completed with Answer : " << m_strAnswer;
		(*(MyCout*)OutputStream).switchtime(false);
		(*(MyCout*)OutputStream) << "\n";

		((QWidget*)parent()->parent())->close();
	}
}

int COptionPanel::_calcBtnLayout()
{
	m_boxlayout->addStretch( 2 );
	m_boxlayout->setSpacing( 10 );
	m_boxlayout->addWidget( m_btnNext );

	this->setLayout( m_boxlayout );
	this->setAlignment( Qt::AlignLeft );
	//this->setFlat( true );

	if ( ! parent()  ) return -1;

	//QRect parentRect = ((QWidget*)parent())->geometry();
	QRect parentRect = qApp->desktop()->screenGeometry();
	this->adjustSize();
	QSize selfsz( 250, 250 );
	//selfsz = this->frameSize();

	/*
	cout << parentRect.x() << "," << parentRect.y() << "," << parentRect.width() << "," << parentRect.height() << endl;
	cout << selfsz.width() << "," << selfsz.height() << endl;
	*/

	this->setGeometry(  parentRect.x() + parentRect.width() - selfsz.width(),
				parentRect.y() + (parentRect.height() - selfsz.height())/2,
				selfsz.width(), 
				selfsz.height() );

	// add event response mapping
	for (size_t idx = 0; idx < m_buttons.size(); idx++) {
		connect ( m_buttons[idx], SIGNAL ( released() ), this, SLOT ( onOptionButtonReleased() ) );
		connect ( m_buttons[idx], SIGNAL ( pressed() ), this, SLOT ( onOptionButtonPressed() ) );
	}

	connect ( m_btnNext, SIGNAL ( released() ), this, SLOT (onNextButtonReleased()) );

	return 0;
}

/* set ts=4 sts=4 tw=80 sw=4 */


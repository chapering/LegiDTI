// ----------------------------------------------------------------------------
// VTKgadget.h : a set of helper gadget used in visualization experimental study
//				built upon available widget supports of VTK
//
// Creation : Dec. 8th 2011
//
// Revisions:
//	- Dec. 9th Add Optional panel implemented upon Qt controls
//	- Dec. 14th Add Answer option ID to unify the task answer log output in this class
//	- Dec. 14th Add OutputStream pointer member to connect with customized ostream outside
//			in order to remove the dependency of parenthood between Qt Windows
//	- Dec. 17th	add onOptionButtonPressed to record user's attempt for specific option
//	- Dec. 19th add m_strExtra to offer the interface for the invoker to insert extra info to dump before 
//				closing the parent window when the "next" buttion is clicked
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#ifndef _VTKGADGET_H_
#define _VTKGADGET_H_

#include <stdint.h>

#include <iostream>
#include <string>
#include <fstream>
#include <strstream>
#include <vector>
#include <ostream>

#include "glrand.h"
#include "point.h"

#include <vtkTextWidget.h>
#include <vtkTextActor.h>
#include <vtkRenderWindow.h>

#include <QGroupBox>
#include <QRadioButton>
#include <QPushButton>
#include <QVBoxLayout>
#include <QPalette>

enum {
	UT_TASKBOX = 0,
	UT_HELPTEXT
};

class VTK_RENDERING_EXPORT CVTKTextbox : public vtkTextActor {
public:
	vtkTypeMacro(CVTKTextbox, vtkTextActor);
	static CVTKTextbox *New();

	/*
	 * @brief load text lines from a file
	 * @param fn a string giving file name with full path 
	 * @return number of lines read correctly or -1 if failed to read anything
	 */
	virtual int loadFromFile(const char* fn);

	void setRenderWindow(vtkRenderWindow* renwin);
	void setUseType(char type);
protected:
	CVTKTextbox();
	~CVTKTextbox();

private:
	char m_type;
	int m_x, m_y;
	int m_width, m_height;
};

class COptionPanel : public QGroupBox {
	Q_OBJECT
public:
	COptionPanel(const QString& title, QWidget* parent=0);
	~COptionPanel();

	void show();

	void addButton(const std::string& btntext);

	int getSelectedBtn();

	void setKey(int optId);
	void setExtraString( const std::string& extrastr );

	void setOutputStream(ostream* os);

public slots:
	void onOptionButtonPressed();
	void onOptionButtonReleased();
	void onNextButtonReleased();

private:
	// the button store
	std::vector<QRadioButton*>	m_buttons;

	// the constant "Next" button
	QPushButton*	m_btnNext;

	QVBoxLayout*	m_boxlayout;
	bool			m_bInit;
	int				m_nSelectedIdx;

	int				m_nKey;

	ostream*		OutputStream;

	std::string		m_strExtra;

private:
	int _calcBtnLayout();
};

#endif // _VTKGADGET_H_

/* set ts=4 sts=4 tw=80 sw=4 */


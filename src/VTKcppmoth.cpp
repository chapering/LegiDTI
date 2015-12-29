// ----------------------------------------------------------------------------
// VTKcppmoth.cpp: a nutshell for building VTK application with "Console" support
//
// Creation : Dec. 7th 2011
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#include "VTKcppmoth.h"

using std::ostream_iterator;
using std::copy;
using std::ofstream;
using std::ifstream;
using std::ends;

//////////////////////////////////////////////////////////////////////////////
//
//	implementation of the Class CSituberRender
//
CVTKApp* CVTKApp::m_psitInstance = NULL;
CVTKApp::CVTKApp(int argc, char **argv) : CApplication(argc, argv),
    m_loader(),
	m_strfnsrc(""),
	m_strfnhelp(""),
	m_strfntask(""),
	m_nselbox(1),
	m_bShowHelp(true),
	m_strAnswer(""),
	m_nKey(1),
	m_strfnskeleton(""),
	m_bSkeletonPrjInitialized(false),
	m_nFlip(0)
{
	addOption('f', true, "input-file-name", "the name of source file"
			" containing geometry and in the format of tgdata");
	addOption('g', true, "output-file-name", "the name of target file"
			" to store the geometry of streamtubes produced");
	addOption('r', true, "tube-radius", "fixed radius of the tubes"
			" to generate");
	addOption('l', true, "lod", "level ot detail controlling the tube"
			" generation, it is expected to impact the smoothness of tubes");
	addOption('b', true, "box-num", "number of selection box"
			" which is 1 by default");
	addOption('p', true, "prompt-text", "a file of interaction help prompt");
	addOption('t', true, "task-list", "a file containing a list of "
			"visualization tasks");
	addOption('s', true, "skeletonic-geometry", "a file containing geometry"
		    " of bundle skeletons also in the format of tgdata");
	addOption('j', true, "boxpos", "files of box position");
	addOption('k', true, "task key", "number indicating the task key, the order of correct box");
	addOption('i', true, "flip model", "boolean 0/1 indicating if to flip the model initially");
}

CVTKApp::~CVTKApp()
{
}

/* 
 * read a file in the format of "tgdata", a .data ouput from tubegen into a
 * series of linear array as vectors, each holding a streamline including
 * its vertices and colors, then all these vectors are indexed in a headlist
 * in which elements are pointers to these vectors respectively
 *
 * After the streamline geometry loaded, streamtube geometry was calculated
 * by wrapping around each streamline a slew of rings that is expected to
 * simulate a shape of tube only in the geometrical sense, not pertinent to
 * visually encoding tensor attributes like the eigenvectors and eigenvalue
 * in each voxel in the tensor field
 */
int CVTKApp::loadGeometry() 
{
	if ( 0 != m_loader.load(m_strfnsrc) ) {
		m_cout << "Loading geometry failed - GLApp aborted abnormally.\n";
		return -1;
	}

	return 0;
}

int CVTKApp::handleOptions(int optv) 
{
	switch( optv ) {
		case 'f':
			m_strfnsrc = optarg;
			return 0;
		case 'p':
			{
				m_strfnhelp = optarg;
				return 0;
			}
			break;
		case 't':
			{
				m_strfntask = optarg;
				return 0;
			}
			break;
		case 's':
			{
				m_strfnskeleton = optarg;
				return 0;
			}
			break;
		case 'j':
			{
				m_strfnTumorBoxes.push_back(optarg);
				return 0;
			}
			break;
		case 'k':
			{
				m_nKey = atoi(optarg);
				if ( m_nKey <= 0 ) {
					cerr << "value for task key is illict, "
						"should be in [1,2].\n";
					return -1;
				}
				return 0;
			}
			break;
		case 'i':
			{
				m_nFlip = atoi(optarg);
				return 0;
			}
			break;
		default:
			return CApplication::handleOptions( optv );
	}
	return 1;
}

int CVTKApp::mainstay()
{
	// geometry must be loaded successfully in the first place in order the
	// openGL pipeline could be launched.
	if ( 0 != loadGeometry() ) {
		return -1;
	}

	m_nselbox = m_strfnTumorBoxes.size();
	// add initial number of selection boxes
	for (int i=0; i<m_nselbox; ++i) {
		addBox();
	}

	if ( 0 != m_nselbox && 0 != _loadTumorBoxpos() ) {
		cerr << "FATAL: failed to load tumor bounding box information.\n";
		return -1;
	}

	// add the help prompt text box if requested
	if ( "" != m_strfnhelp && m_bShowHelp ) { //yes, requested
		// we are not that serious to quit just owing to the failure in loading
		// help text since it is just something optional
		if ( 0 < m_helptext.loadFromFile( m_strfnhelp.c_str() ) ) {
			m_helptext.setVertexCoordRange(
				( m_minCoord[0] + m_maxCoord[0] )/2,
				( m_minCoord[1] + m_maxCoord[1] )/2,
				( m_minCoord[2] + m_maxCoord[2] )/2);
			m_helptext.setColor(1.0, 1.0, 0.6);
			m_helptext.setRasterPosOffset(35, 2);
		}
	}

	// associate with a task list if requested
	if ( "" != m_strfntask ) { // yes, requested
		// loading a task list is also optional
		if ( 0 < m_taskbox.loadFromFile( m_strfntask.c_str() ) ) {
			m_taskbox.setVertexCoordRange(
				( m_minCoord[0] + m_maxCoord[0] )/2,
				( m_minCoord[1] + m_maxCoord[1] )/2,
				( m_minCoord[2] + m_maxCoord[2] )/2);
			m_taskbox.setColor(1.0, 1.0, 1.0);
			//m_taskbox.turncover(true);
			m_taskbox.turncover(false);
			m_taskbox.setRasterPosOffset(0, -80);
			m_bIboxEnabled = false;
			//m_bGadgetEnabled = false;
		}
	}

	addOptBtn("Similar");
	addOptBtn("1 is higher");
	addOptBtn("2 is higher");
	addOptBtn("Next");

	m_psitInstance = this;

	if ( m_bSuspended ) {
		m_cout << "Event loop entrance suspened.\n";
		return 0;
	}

	return	CApplication::mainstay();
}

void CVTKApp::suspend(bool bSuspend)
{
	m_bSuspended = bSuspend;
	if ( !bSuspend ) {
		CApplication::mainstay();
	}
}

bool CVTKApp::switchhelp(bool bon)
{
	bool ret = m_bShowHelp;
	m_bShowHelp = bon;
	return ret;
}

int CVTKApp::dumpRegions(const char* fnRegion)
{
	string strfn(fnRegion);
	if ("" == strfn) {
		// split directory and file name in the input file m_strfnsrc
		string fnsrc(m_strfnsrc), fndir("./");
		size_t loc = fnsrc.rfind('/');
		if ( string::npos != loc ) {
			fndir.assign(m_strfnsrc.begin(), m_strfnsrc.begin() + loc + 1);
			fnsrc.assign(m_strfnsrc.begin() + loc + 1, m_strfnsrc.end());
		}

		// automate the file name if not offered explicitly
		ostringstream ostrfn;
		ostrfn << fndir << "region_" << time(NULL) 
			<< "_" << fnsrc << ends;
		strfn = ostrfn.str();
	}

	if ( 0 != m_loader.dump(strfn.c_str(), &m_edgeflags) ) {
		cerr << "FATAL: error in dumping regions.\n";
		return -1;
	}
	m_cout << "regions selected dumped into " << strfn << ".\n";
	return 0;
}

bool CVTKApp::_getAnswer(unsigned char key)
{
	/*
	if ( m_taskbox.iscovered() ) {
		return false;
	}

	if ( '1' != key && '2' != key && '3' != key ) {
		return false;
	}
	*/
	if ( m_curseloptidx == -1 ) {
		return true;
	}

	if ( m_curseloptidx == int(m_optPanel.m_buttons.size()) - 1 ) {
		if ( m_nTaskAnswer == -1 ) {
			m_cout << "illegal intention to moving next: no response received.\n";
			return false;
		}

		m_strAnswer += ('1' + m_nTaskAnswer);

		if ( m_nKey == atoi(m_strAnswer.c_str()) ) {
			m_strAnswer += " (correct).";
		}
		else {
			m_strAnswer += " (wrong).";
		}
		m_cout << "Task completed with Answer : " << m_strAnswer << "\n";
		cleanup();
		exit(EXIT_SUCCESS);
	}

	//m_strAnswer += key;
	return true;
}

int CVTKApp::_loadTumorBoxpos()
{
	int nBoxes = (int)m_strfnTumorBoxes.size();
	int nLineInbox = 0;
	m_tumorBoxMin.resize( nBoxes );
	m_tumorBoxMax.resize( nBoxes );

	string discard;
	double maxFA = .0, curFA;
	//cout << "FAInfo: ";
	for (int i = 0; i < nBoxes; i++) {
		ifstream ifs(m_strfnTumorBoxes[i].c_str());
		if ( !ifs.is_open() ) {
			return -1;
		}

		// we only care about the 3 lines of the file
		// 1st line: the task key, but not used in this version, so discarded
		if ( !ifs ) return -1;
		ifs >> discard;

		// 2nd line: the coordinate of the minor corner of the tumor bounding box
		if ( !ifs ) return -1;
		ifs >> fixed >> setprecision(6);
		ifs >> m_tumorBoxMin[i].x >> m_tumorBoxMin[i].y >> m_tumorBoxMin[i].z;

		// 3rd line: the coordinate of the major corner of the tumor bounding box
		if ( !ifs ) return -1;
		ifs >> m_tumorBoxMax[i].x >> m_tumorBoxMax[i].y >> m_tumorBoxMax[i].z;

		ifs.close();

		/*
		cout << "box " << i << " loaded:";
		cout << m_tumorBoxMax[i] << m_tumorBoxMin[i] << "\n";
		*/

		m_boxes[i].setMinMax( m_tumorBoxMin[i].x, m_tumorBoxMin[i].y, m_tumorBoxMin[i].z,
				m_tumorBoxMax[i].x, m_tumorBoxMax[i].y, m_tumorBoxMax[i].z);

		curFA = _calBlockAvgFA(i, nLineInbox);
		//cout << curFA << " " << nLineInbox << " ";
		if ( curFA > maxFA ) {
			maxFA= curFA;
			m_nKey = i+2;
		}
	}

	double faDiff = fabs(_calBlockAvgFA(0, nLineInbox) - _calBlockAvgFA(1, nLineInbox));
	if (faDiff < 1e-2 ) {
		//cout << "box 0: " << _calBlockAvgFA(0) << "\n";
		//cout << "box 1: " << _calBlockAvgFA(1) << "\n";
		m_nKey = 1;
	}
	//cout << faDiff << "\n";
	//exit(0);

	//cout << "max FA: " << maxFA << " key box: " << m_nKey << "\n";

	return 0;
}

/* set ts=4 sts=4 tw=80 sw=4 */


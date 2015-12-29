// ----------------------------------------------------------------------------
// VTKcppmoth.cpp: a nutshell for building VTK application with "Console" support
//
// Creation : Dec. 7th 2011
//
// Revision:
//
// Copyright(C) 2011-2012 Haipeng Cai
//
// ----------------------------------------------------------------------------
#ifndef  _VTKCPPMOTH_H_

#include "cppmoth.h"
#include "GLoader.h"
#include "glrand.h"

#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

using std::cout;
using std::cerr;
using std::vector;
using std::string;

class CVTKApp: public CApplication {
protected:
public:

	CVTKApp(int argc, char **argv);
	virtual ~CVTKApp();

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
	int loadGeometry();

	int mainstay();

	// update the suspense switch
	void suspend(bool bSuspend = false);

	// help text switch
	bool switchhelp(bool bon);

	// dump the geometry of streamlines associated with streamtubes currently
	// visible( i.e. selected ) into a file of the given name in the format of
	// "tgdata" as exactly the same as that of the file where the streamline
	// geometry is loaded;
	// when the file name is empty string, a fabricated name will be used - the
	// value of -f option plus current time in seconds (this can make unique the
	// naming)
	int dumpRegions(const char* fnRegion="");

private:
	/* the contained sub-object used for source geometry loading and parsing */
	CTgdataLoader m_loader;

	/* used for loading the skeleton geometry */
	CTgdataLoader m_skeletonLoader;
	CTgdataLoader m_orgSkeleton, m_staticSkeleton;

	/* source file holding the most essential streamline geometry */
	string m_strfnsrc;

	string m_strfnhelp;

	/* text file holding a list of tasks for a single session */
	string m_strfntask;

	/* number of selection box */
	GLubyte				m_nselbox;

	/* help text box presence switch */
	bool m_bShowHelp;

	// record what the user typed as the answer to the task 
	std::string m_strAnswer;

	// the expected answer, i.e. the key
	int m_nKey;

	/* another streamline geometry as the fiber bundle skeleton associated with
	 * the primary geometries in m_strfnsrc
	 */
	string m_strfnskeleton;

	/* if the skeleton projection has been initializd */
	bool m_bSkeletonPrjInitialized;

	std::vector< std::string > m_strfnTumorBoxes;
	// the opposite corner of the tumor bounding box
	std::vector< _point_t<GLfloat> > m_tumorBoxMin, m_tumorBoxMax;

	int m_nFlip;
private:

	bool _getAnswer(unsigned char key);
	// load bounding box information for the tumor potato
	int _loadTumorBoxpos();

	// calculate block average FA and compare, thus decide upon the key box
	double _calBlockAvgFA(int boxIdx, int& nLineInbox);
};

#endif // _VTKCPPMOTH_H_

/* set ts=4 sts=4 tw=80 sw=4 */


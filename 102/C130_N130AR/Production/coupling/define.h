/*
-------------------------------------------------------------------------
OBJECT NAME:	define.h

DESCRIPTION:	Header File for Nimbus Skeleton.
-------------------------------------------------------------------------
*/

#ifndef DEFINE_H
#define DEFINE_H

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <Xm/Xm.h>

#include "constants.h"

/* This is the number of records that you wish to have access to.  This is
 * used to determine the size of the circular buffer.  This number should
 * be odd.
 */
#define NBUFFERS		1
#define CURRENT_BUFFER	-((NBUFFERS >> 1) + 1))


#define NAMELEN			NAMLEN
#define MAX_VARIABLES	2000
#define MAX_TIME_SLICES	5


/* Values for "VariableType"	*/
#define SDI				0
#define RAW				1
#define DERIVED			2


/* PauseWhatToDo values.		*/
#define P_QUIT			0
#define P_CONTINUE		1


#define BEG_OF_TAPE		(-1)
#define END_OF_TAPE		(-1)

/* OpenProjectFile action values	*/
#define RETURN			0
#define EXIT			1


#define LOW_RATE	1
#define HIGH_RATE	25


/* Nimbus Record Info		*/
#define NR_TYPE float
#define NR_SIZE sizeof(NR_TYPE)


typedef struct
	{
	char	name[NAMELEN];
	int		inVarID;
	int		outVarID;
	int		SampleRate;
	int		OutputRate;
	int		VectorLength;
	bool	Output;

	void	(*Average)();
	void	(*Compute)();
	} VARTBL;

typedef struct
	{
	char	*Name;
	void	(*Compute)();
	} DERVAR;


/*		Global Variables
 */
extern char		*ProjectNumber, *ProjectName;
extern char		*Aircraft, *FlightNumber, *FlightDate, *Defaults;
extern VARTBL	*Variable[];
extern bool		PauseFlag, Interactive;
extern int		nVariables, InputFile, OutputFile, PauseWhatToDo;
extern char		buffer[];
extern long		nRecords, CurrentInputRecordNumber, CurrentOutputRecordNumber;


/* Setup window widgets */
extern Widget	Shell001;
extern Widget	SetupWindow, EditWindow, TimeSliceWindow;
extern Widget	inputFileText, outputFileText;
extern Widget	timeDisplayText;
extern Widget	list1, goButton, menuBar, readHeaderButton;

/* Edit Variable window widgets */
extern Widget	varNameText;
extern Widget	outputVarYes, outputVarNo;
extern Widget	outputLRbutton, outputSRbutton, outputHRbutton;
extern Widget	lagText, ev_text[], ts_text[];


double	atof();

/*		Local Functions
 */
char	*SearchList(), *GetMemory();
int		ReadTextFile(), AccessProjectFile();
void	FreeTextFile();
void	Average(), Sum(), SumVector(), AverageVector();

void	CreateNetCDF(), WriteNetCDF(), CloseNetCDF(),
		SetBaseTime(), FlushXEvents(), GetUserTimeIntervals(), Initialize(),
		LogMessage(), FormatTimeSegmentsForOutputFile(), AverageSampledData(),
		AllocateDataArrays(), FreeDataArrays(), HandleError(), HandleWarning();

/* Callbacks	*/
void	CancelWarning(), ApplyVariableMods(), CancelSetup(), EditTimeSlices(),
		DismissEditWindow(), EditVariable(), MapCalCof(), Quit(), ReadHeader(),
		StartProcessing(), ValidateTime(), Proceed(), ToggleOutput(),
		PauseProcessing(), VerifyLagText(),
		PauseStop(), PauseContinue(), DismissTimeSliceWindow();


#endif

/* END DEFINE.H */

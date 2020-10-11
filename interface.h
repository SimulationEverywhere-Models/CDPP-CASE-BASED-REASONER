/*******************************************************************
*
*  DESCRIPTION: Atomic Model Interface
*
*  AUTHOR: Michael W. Floyd
*
*  EMAIL: mfloyd@sce.carleton.ca
*
*  DATE: 01/11/2009
*
*******************************************************************/

#ifndef __INTERFACE_H
#define __INTERFACE_H

#include "atomic.h"

/** forward declarations **/
class Distribution ;

class Interface : public Atomic
{
public:
	Interface( const string &name = "Interface" );					//Default constructor
	~Interface();
	virtual string className() const ;

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const InternalMessage & );

private:
	const Port &input;
	const Port &solution;
	Port &output;
	Port &query;
	Port &feedback;
	Distribution * dist;
	const static float percentCorrect = 0.65;
	enum ActiveState { WAITING = 0, PROCESSING = 1, EVALUATING = 2, DONE = 3};
	int currentState;
	int solutionCorrect;

	Distribution &distribution()
		{return *dist;}

};	// class Interface

// ** inline ** //
inline
string Interface::className() const
{
	return "Interface" ;
}

#endif   //__INTERFACE_H

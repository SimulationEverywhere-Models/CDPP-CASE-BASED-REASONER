/*******************************************************************
*
*  DESCRIPTION: Atomic Model Retrieve
*
*  AUTHOR: Michael W. Floyd
*
*  EMAIL: mfloyd@sce.carleton.ca
*
*  DATE: 01/11/2009
*
*******************************************************************/

#ifndef __RETRIEVE_H
#define __RETRIEVE_H

#include "atomic.h"

/** forward declarations **/
class Distribution ;

class Retrieve : public Atomic
{
public:
	Retrieve( const string &name = "Retrieve" );					//Default constructor
	~Retrieve();
	virtual string className() const ;

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const InternalMessage & );

private:
	const Port &query;
	const Port &provide;
	Port &retrieve;
	Port &proposed;
	Distribution * dist;
	int casebaseSize;
	enum ActiveState { SEARCHING = 0, ACCESSING = 1};
	int currentState;

	Distribution &distribution()
		{return *dist;}

};	// class Retrieve

// ** inline ** //
inline
string Retrieve::className() const
{
	return "Retrieve" ;
}

#endif   //__RETRIEVE_H

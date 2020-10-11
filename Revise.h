/*******************************************************************
*
*  DESCRIPTION: Atomic Model Revise
*
*  AUTHOR: Michael W. Floyd
*
*  EMAIL: mfloyd@sce.carleton.ca
*
*  DATE: 01/11/2009
*
*******************************************************************/

#ifndef __REVISE_H
#define __REVISE_H

#include "atomic.h"

/** forward declarations **/
class Distribution ;

class Revise : public Atomic
{
public:
	Revise( const string &name = "Revise" );					//Default constructor
	~Revise();
	virtual string className() const ;

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const InternalMessage & );

private:
	const Port &adapted;
	const Port &feedback;
	Port &solution;
	Port &fixed;
	Distribution * dist;
	const static float percentFixed = 0.20;
	int significantlyChanged;
	enum ActiveState { WAITING = 0, FIXING = 1, DONE = 2};
	int currentState;

	Distribution &distribution()
		{return *dist;}

};	// class Revise

// ** inline ** //
inline
string Revise::className() const
{
	return "Revise" ;
}

#endif   //__REVISE_H

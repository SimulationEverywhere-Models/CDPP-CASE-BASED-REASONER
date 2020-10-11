/*******************************************************************
*
*  DESCRIPTION: Atomic Model Reuse
*
*  AUTHOR: Michael W. Floyd
*
*  EMAIL: mfloyd@sce.carleton.ca
*
*  DATE: 01/11/2009
*
*******************************************************************/

#ifndef __REUSE_H
#define __REUSE_H

#include "atomic.h"

/** forward declarations **/
class Distribution ;

class Reuse : public Atomic
{
public:
	Reuse( const string &name = "Reuse" );					//Default constructor
	~Reuse();
	virtual string className() const ;

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const InternalMessage & );

private:
	const Port &proposed;
	Port &solution;
	Port &adapted;
	Distribution * dist;
	const static float percentAdapted = 0.30;

	Distribution &distribution()
		{return *dist;}

};	// class Reuse

// ** inline ** //
inline
string Reuse::className() const
{
	return "Reuse" ;
}

#endif   //__REUSE_H

/*******************************************************************
*
*  DESCRIPTION: Atomic Model Retain
*
*  AUTHOR: Michael W. Floyd
*
*  EMAIL: mfloyd@sce.carleton.ca
*
*  DATE: 26/10/2009
*
*******************************************************************/

#ifndef __RETAIN_H
#define __RETAIN_H

#include "atomic.h"

/** forward declarations **/
class Distribution ;

class Retain : public Atomic
{
public:
	Retain( const string &name = "Retain" );					//Default constructor
	~Retain();
	virtual string className() const ;

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const InternalMessage & );

private:
	const Port &in;
	Port &out;
	Distribution * dist;

	Distribution &distribution()
		{return *dist;}

};	// class Retain

// ** inline ** //
inline
string Retain::className() const
{
	return "Retain" ;
}

#endif   //__RETAIN_H

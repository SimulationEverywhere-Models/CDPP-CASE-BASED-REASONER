/*******************************************************************
*
*  DESCRIPTION: Atomic Model CaseBase
*
*  AUTHOR: Michael W. Floyd
*
*  EMAIL: mfloyd@sce.carleton.ca
*
*  DATE: 26/10/2009
*
*******************************************************************/

#ifndef __CASEBASE_H
#define __CASEBASE_H

#include "atomic.h"

/** forward declarations **/
class Distribution ;

class CaseBase : public Atomic
{
public:
	CaseBase( const string &name = "CaseBase" );					//Default constructor
	~CaseBase();
	virtual string className() const ;

protected:
	Model &initFunction();
	Model &externalFunction( const ExternalMessage & );
	Model &internalFunction( const InternalMessage & );
	Model &outputFunction( const InternalMessage & );

private:
	const Port &store;
	const Port &retrieve;
	Port &provide;
	Distribution * dist;
	int casebaseSize;
	enum ActiveState { RETRIEVE = 0, STORE = 1};
	int currentState;

	Distribution &distribution()
		{return *dist;}

};	// class CaseBase

// ** inline ** //
inline
string CaseBase::className() const
{
	return "CaseBase" ;
}

#endif   //__CASEBASE_H

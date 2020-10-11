/*******************************************************************
*
*  DESCRIPTION: Atomic Model CaseBase
*
*  AUTHOR: Michael W. Floyd
*
*  EMAIL:mfloyd@sce.carleton.ca
*
*  DATE: 26/10/2009
*
*******************************************************************/

/** include files **/
#include "casebase.h"
#include "message.h"    // class ExternalMessage, InternalMessage
#include "mainsimu.h"   // MainSimulator::Instance().getParameter( ... )
#include "distri.h"        // class Distribution
#include "strutil.h"       // str2float( ... )

/** public functions **/

/*******************************************************************
* Function Name: CaseBase
* Description: Represents the data structure, the casebase
********************************************************************/
CaseBase::CaseBase( const string &name )
: Atomic( name )
, store( addInputPort( "store" ) )
, retrieve( addInputPort( "retrieve" ) )
, provide( addOutputPort( "provide" ) )
{
		//all we are doing here is making sure the user properly defined our distribution.
		try
		{
			dist = Distribution::create( MainSimulator::Instance().getParameter( description(), "distribution" ) );

			MASSERT( dist );

			for ( register int i = 0 ; i < dist->varCount() ; i++ )
			{
				string parameter( MainSimulator::Instance().getParameter( description(), dist->getVar(i) ) ) ;
				dist->setVar( i, str2float( parameter ) ) ;
			}
		} catch( InvalidDistribution &e )
		{
			e.addText( "The model " + description() + " has distribution problems!" ) ;
			e.print(cerr);
			MTHROW( e ) ;
		} catch( MException &e )
		{
			MTHROW( e ) ;
		}

		//by default our casebase is empty. We could also have this a parameter, but will use 0 for now.
		casebaseSize = 0;

}

/*******************************************************************
* Function Name: initFunction
* Description: By default we are in a passive state waiting for things to
*              happen.
********************************************************************/
Model &CaseBase::initFunction()
{
	this-> passivate();
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description:
********************************************************************/
Model &CaseBase::externalFunction( const ExternalMessage &msg )
{
	//if we are already active we will just ignore the new request
	if(this->state() == passive){

		if( msg.port() == retrieve ){
			int msgVal = static_cast < int > (msg.value());
			if(msgVal == 1){
				currentState = RETRIEVE;
				holdIn(active, Time(0,0,0, static_cast<float>( fabs( distribution().get() ) ) ) );
			}else{
				//we have a problem! Unknown input...
				MException e("Unexpected input on CaseBase input port!");
				e.print(cerr);
				MTHROW( e ) ;
			}
		}else if( msg.port() == store){
			int msgVal = static_cast < int > (msg.value());
			if(msgVal == 1){
				casebaseSize++;
				currentState = STORE;
				holdIn(active, Time(0,0,0, static_cast<float>( fabs( distribution().get() ) ) ) );
			}else{
				//we have a problem! Unknown input...
				MException e("Unexpected input on CaseBase input port!");
				e.print(cerr);
				MTHROW( e ) ;
			}
		}else{
			//we have a problem! I have never heard of this port...
			MException e("Input came in on an unknown input port!");
			e.print(cerr);
			MTHROW( e ) ;
		}
	}else{
		//we could possible enqueue things, but for now lets just ignore
	}
	return *this;
}

/*******************************************************************
* Function Name: internalFunction
* Description: After we are complete we go right back to passive. This
*              is because there is no queue of incoming messages. If any
*              came when we were waiting for time to advance we ignored them.
********************************************************************/
Model &CaseBase::internalFunction( const InternalMessage & )
{
	passivate();
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
* Description: If we produce output, that means we say our casebase size.
********************************************************************/
Model &CaseBase::outputFunction( const InternalMessage &msg )
{

	if(currentState == RETRIEVE){
		sendOutput( msg.time(), provide, casebaseSize) ;
	}else if(currentState == STORE){
		//we produce no output in this situation
	}else{
		MException e("The model is in an invalid state!");
		e.print(cerr);
		MTHROW( e ) ;
	}
}


/*******************************************************************
* Function Name: ~CaseBase
* Description: Get rid of the distribution we created in constructor.
********************************************************************/
CaseBase::~CaseBase()
{
	delete dist;
}


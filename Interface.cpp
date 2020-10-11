/*******************************************************************
*
*  DESCRIPTION: Atomic Model Interface
*
*  AUTHOR: Michael W. Floyd
*
*  EMAIL:mfloyd@sce.carleton.ca
*
*  DATE: 01/11/2009
*
*******************************************************************/

/** include files **/
#include "interface.h"
#include "message.h"    // class ExternalMessage, InternalMessage
#include "mainsimu.h"   // MainSimulator::Instance().getParameter( ... )
#include "distri.h"        // class Distribution
#include "strutil.h"       // str2float( ... )
#include <cstdlib>			//for random numbers

/** public functions **/

/*******************************************************************
* Function Name: Interface
* Description: Interfaces the CBR system with the environment
********************************************************************/
Interface::Interface( const string &name )
: Atomic( name )
, input( addInputPort( "input" ) )
, solution( addInputPort( "solution") )
, feedback( addOutputPort( "feedback" ) )
, output( addOutputPort( "output" ) )
, query( addOutputPort( "query" ) )
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

}

/*******************************************************************
* Function Name: initFunction
* Description: By default we are in a passive state waiting for things to
*              happen.
********************************************************************/
Model &Interface::initFunction()
{
	this-> passivate();
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description:
********************************************************************/
Model &Interface::externalFunction( const ExternalMessage &msg )
{
	//if we are already active we will just ignore the new request
	if(this->state() == passive){

		if( msg.port() == input ){
			int msgVal = static_cast < int > (msg.value());
			if(msgVal != 1){
				//we have a problem! I have never heard of this value...
				MException e("Input came in on an unknown input value!");
				e.print(cerr);
				MTHROW( e ) ;
			}
			solutionCorrect = 0;
			currentState = PROCESSING;
			holdIn(active, Time(0,0,0, static_cast<float>( fabs( distribution().get() ) ) ) );
		}else if( msg.port() == solution ){
			//do nothing, this this is unexpected and unimportant
		}else{
			//we have a problem! I have never heard of this port...
			MException e("Input came in on an unknown input port!");
			e.print(cerr);
			MTHROW( e ) ;
		}
	}else{
		if(currentState == WAITING){
			if( msg.port() == solution ){
				int msgVal = static_cast < int > (msg.value());
				if(msgVal != 1){
					//we have a problem! I have never heard of this ...
					MException e("Input invalid!");
					e.print(cerr);
					MTHROW( e ) ;
				}
				currentState = EVALUATING;
				holdIn(active, Time(0,0,0, static_cast<float>( fabs( distribution().get() ) ) ) );
			}else if( msg.port() == input ){
					//do nothing, this this is unexpected and unimportant
			}else{
				//we have a problem! I have never heard of this port...
				MException e("Input came in on an unknown input port!");
				e.print(cerr);
				MTHROW( e ) ;
			}
		}else if(currentState == EVALUATING || currentState == PROCESSING || currentState == DONE){
			//we will just ignore, it means inputs are coming in too fast
		}else{
			//we have a problem! I have never heard of this state...
			MException e("I am in an invalid state!");
			e.print(cerr);
			MTHROW( e ) ;
		}
	}
	return *this;
}

/*******************************************************************
* Function Name: internalFunction
* Description:
********************************************************************/
Model &Interface::internalFunction( const InternalMessage & )
{

	if(currentState == PROCESSING){
		currentState = WAITING;
		holdIn(active, Time::Inf );
	}else if(currentState == EVALUATING){
		if(solutionCorrect == 1){
			currentState = DONE;
			holdIn(active, Time::Zero);
		}else{
			currentState = WAITING;
			holdIn(active, Time::Inf );
		}
	}else if(currentState == DONE){
		passivate();
	}
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
* Description:
********************************************************************/
Model &Interface::outputFunction( const InternalMessage &msg )
{
	if(currentState == DONE){
		sendOutput( msg.time(), output, 1) ;
	}else if(currentState == EVALUATING){
		float rndval = rand()/(float)RAND_MAX;
		if(rndval <= percentCorrect){
			solutionCorrect = 1;
		}
		sendOutput( msg.time(), feedback, solutionCorrect ) ;
	}else if(currentState == PROCESSING){
		sendOutput( msg.time(), query, 1 ) ;
	}else{
		//do nothing
	}

}


/*******************************************************************
* Function Name: ~Interface
* Description: Get rid of the distribution we created in constructor.
********************************************************************/
Interface::~Interface()
{
	delete dist;
}


/*******************************************************************
*
*  DESCRIPTION: Atomic Model Revise
*
*  AUTHOR: Michael W. Floyd
*
*  EMAIL:mfloyd@sce.carleton.ca
*
*  DATE: 01/11/2009
*
*******************************************************************/

/** include files **/
#include "revise.h"
#include "message.h"    // class ExternalMessage, InternalMessage
#include "mainsimu.h"   // MainSimulator::Instance().getParameter( ... )
#include "distri.h"        // class Distribution
#include "strutil.h"       // str2float( ... )
#include <cstdlib>			//for random numbers

/** public functions **/

/*******************************************************************
* Function Name: Revise
* Description: Represents the fixing of the solution if it did not solve the problem
********************************************************************/
Revise::Revise( const string &name )
: Atomic( name )
, adapted( addInputPort( "adapted" ) )
, feedback( addInputPort( "feedback") )
, solution( addOutputPort( "solution" ) )
, fixed( addOutputPort( "fixed" ) )
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

		significantlyChanged = 0;
}

/*******************************************************************
* Function Name: initFunction
* Description: By default we are in a passive state waiting for things to
*              happen.
********************************************************************/
Model &Revise::initFunction()
{
	this-> passivate();
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description:
********************************************************************/
Model &Revise::externalFunction( const ExternalMessage &msg )
{
	//if we are already active we will just ignore the new request
	if(this->state() == passive){

		if( msg.port() == adapted ){
			int msgVal = static_cast < int > (msg.value());
			currentState = WAITING;
			significantlyChanged = msgVal;
			holdIn(active, Time::Inf );
		}else if( msg.port() == feedback ){
			//do nothing, this this is unexpected and unimportant
		}else{
			//we have a problem! I have never heard of this port...
			MException e("Input came in on an unknown input port!");
			e.print(cerr);
			MTHROW( e ) ;
		}
	}else{
		if(currentState == WAITING){
			if( msg.port() == feedback ){
					int msgVal = static_cast < int > (msg.value());
					if(msgVal == 0){
						//the solution was incorrect
						currentState = FIXING;
						float rndval = rand()/(float)RAND_MAX;
						if(rndval <= percentFixed){
							significantlyChanged = 1;
						}
						holdIn(active, Time(0,0,0, static_cast<float>( fabs( distribution().get() ) ) ) );
					}else if(msgVal == 1){
						//the solution was correct
						currentState = DONE;
						holdIn(active, Time(0,0,0, static_cast<float>( fabs( distribution().get() ) ) ) );
					}else{
						//we have a problem! I have never heard of this ...
						MException e("Input invalid!");
						e.print(cerr);
						MTHROW( e ) ;
					}

			}else if( msg.port() == adapted ){
					//do nothing, this this is unexpected and unimportant
			}else{
				//we have a problem! I have never heard of this port...
				MException e("Input came in on an unknown input port!");
				e.print(cerr);
				MTHROW( e ) ;
			}
		}else if(currentState == FIXING || currentState == DONE){
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
Model &Revise::internalFunction( const InternalMessage & )
{

	if(currentState == FIXING){
		currentState = WAITING;
		holdIn(active, Time::Inf );
	}else if(currentState == DONE){
		passivate();
	}
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
* Description:
********************************************************************/
Model &Revise::outputFunction( const InternalMessage &msg )
{
	if(currentState == DONE){
		sendOutput( msg.time(), fixed, significantlyChanged) ;
	}else if(currentState == FIXING){
		sendOutput( msg.time(), solution, 1 ) ;
	}else{
		//do nothing
	}

}


/*******************************************************************
* Function Name: ~Revise
* Description: Get rid of the distribution we created in constructor.
********************************************************************/
Revise::~Revise()
{
	delete dist;
}


/*******************************************************************
*
*  DESCRIPTION: Atomic Model Retrieve
*
*  AUTHOR: Michael W. Floyd
*
*  EMAIL:mfloyd@sce.carleton.ca
*
*  DATE: 01/11/2009
*
*******************************************************************/

/** include files **/
#include "retrieve.h"
#include "message.h"    // class ExternalMessage, InternalMessage
#include "mainsimu.h"   // MainSimulator::Instance().getParameter( ... )
#include "distri.h"        // class Distribution
#include "strutil.h"       // str2float( ... )

/** public functions **/

/*******************************************************************
* Function Name: Retrieve
* Description: Represents the time required to search the casebase
********************************************************************/
Retrieve::Retrieve( const string &name )
: Atomic( name )
, provide( addInputPort( "provide" ) )
, query( addInputPort( "query" ) )
, retrieve( addOutputPort( "retrieve" ) )
, proposed( addOutputPort( "proposed" ) )
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

		//we dont know the size, since we have yet to query
		casebaseSize = -1;

}

/*******************************************************************
* Function Name: initFunction
* Description: By default we are in a passive state waiting for things to
*              happen.
********************************************************************/
Model &Retrieve::initFunction()
{
	this-> passivate();
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description:
********************************************************************/
Model &Retrieve::externalFunction( const ExternalMessage &msg )
{
	//if we are already active we will just ignore the new request
	if(this->state() == passive){

		if( msg.port() == query ){
			int msgVal = static_cast < int > (msg.value());
			if(msgVal == 1){
				currentState = ACCESSING;
				holdIn(active, Time(0,0,0, static_cast<float>( fabs( distribution().get() ) ) ) );
			}else{
				//we have a problem! Unknown input...
				MException e("Unexpected input on Retrieve input port!");
				e.print(cerr);
				MTHROW( e ) ;
			}
		}else if( msg.port() == provide){
			int msgVal = static_cast < int > (msg.value());
			casebaseSize = msgVal;
			currentState = SEARCHING;
			holdIn(active, Time(0,0,0, casebaseSize*static_cast<float>( fabs( distribution().get() ) ) ) );

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
Model &Retrieve::internalFunction( const InternalMessage & )
{
	passivate();
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
* Description: Depending on the state, we either get case base size or propose a solution
********************************************************************/
Model &Retrieve::outputFunction( const InternalMessage &msg )
{

	if(currentState == ACCESSING){
		sendOutput( msg.time(), retrieve, 1) ;
	}else if(currentState == SEARCHING){
		sendOutput( msg.time(), proposed, 1) ;
	}else{
		MException e("The model is in an invalid state!");
		e.print(cerr);
		MTHROW( e ) ;
	}
}


/*******************************************************************
* Function Name: ~Retrieve
* Description: Get rid of the distribution we created in constructor.
********************************************************************/
Retrieve::~Retrieve()
{
	delete dist;
}


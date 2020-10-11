/*******************************************************************
*
*  DESCRIPTION: Atomic Model Reuse
*
*  AUTHOR: Michael W. Floyd
*
*  EMAIL:mfloyd@sce.carleton.ca
*
*  DATE: 01/11/2009
*
*******************************************************************/

/** include files **/
#include "reuse.h"
#include "message.h"    // class ExternalMessage, InternalMessage
#include "mainsimu.h"   // MainSimulator::Instance().getParameter( ... )
#include "distri.h"        // class Distribution
#include "strutil.h"       // str2float( ... )
#include <cstdlib>			//for random numbers

/** public functions **/

/*******************************************************************
* Function Name: Reuse
* Description: Represents the adaptation of proposed cases
********************************************************************/
Reuse::Reuse( const string &name )
: Atomic( name )
, proposed( addInputPort( "proposed" ) )
, solution( addOutputPort( "solution" ) )
, adapted( addOutputPort( "adapted" ) )
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
Model &Reuse::initFunction()
{
	this-> passivate();
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description:
********************************************************************/
Model &Reuse::externalFunction( const ExternalMessage &msg )
{
	//if we are already active we will just ignore the new request
	if(this->state() == passive){

		if( msg.port() == proposed ){
			int msgVal = static_cast < int > (msg.value());
			if(msgVal == 1){
				holdIn(active, Time(0,0,0, static_cast<float>( fabs( distribution().get() ) ) ) );
			}else{
				//we have a problem! Unknown input...
				MException e("Unexpected input on Reuse input port!");
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
Model &Reuse::internalFunction( const InternalMessage & )
{
	passivate();
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
* Description: Output both the solution and if it was a significant adaptation
********************************************************************/
Model &Reuse::outputFunction( const InternalMessage &msg )
{
	sendOutput( msg.time(), solution, 1) ;
	float rndval = rand()/(float)RAND_MAX;
	int sigAdapt = 0;
	if(rndval <= percentAdapted){
		sigAdapt = 1;
	}
	sendOutput( msg.time(), adapted, sigAdapt ) ;

}


/*******************************************************************
* Function Name: ~Reuse
* Description: Get rid of the distribution we created in constructor.
********************************************************************/
Reuse::~Reuse()
{
	delete dist;
}


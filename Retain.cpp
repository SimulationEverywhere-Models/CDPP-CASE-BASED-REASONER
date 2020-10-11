/*******************************************************************
*
*  DESCRIPTION: Atomic Model Retain
*
*  AUTHOR: Michael W. Floyd
*
*  EMAIL:mfloyd@sce.carleton.ca
*
*  DATE: 26/10/2009
*
*******************************************************************/

/** include files **/
#include "retain.h"
#include "message.h"    // class ExternalMessage, InternalMessage
#include "mainsimu.h"   // MainSimulator::Instance().getParameter( ... )
#include "distri.h"        // class Distribution
#include "strutil.h"       // str2float( ... )

/** public functions **/

/*******************************************************************
* Function Name: Retain
* Description: Depending on the external input, a case might be retained
*              in the CaseBase.
********************************************************************/
Retain::Retain( const string &name )
: Atomic( name )
, in( addInputPort( "in" ) )
, out( addOutputPort( "out" ) )
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
Model &Retain::initFunction()
{
	this-> passivate();
	return *this ;
}

/*******************************************************************
* Function Name: externalFunction
* Description:
********************************************************************/
Model &Retain::externalFunction( const ExternalMessage &msg )
{
	//if we are already active we will just ignore the new request
	if(this->state() == passive){

		if( msg.port() == in ){
			int msgVal = static_cast < int > (msg.value());
			if(msgVal == 1){
				holdIn(active, Time(0,0,0, static_cast<float>( fabs( distribution().get() ) ) ) );
			}else if(msgVal == 0){
				//the case is not novel, do nothing
			}else{
				//we have a problem! Unknown input...
				MException e("Unexpected input on Retain input port!");
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
Model &Retain::internalFunction( const InternalMessage & )
{
	passivate();
	return *this ;
}

/*******************************************************************
* Function Name: outputFunction
* Description: If we produce output, that means we want to tell the
* 				casebase to store a case.
********************************************************************/
Model &Retain::outputFunction( const InternalMessage &msg )
{

	sendOutput( msg.time(), out, 1) ;
	return *this ;
}


/*******************************************************************
* Function Name: ~Retain
* Description: Get rid of the distribution we created in constructor.
********************************************************************/
Retain::~Retain()
{
	delete dist;
}


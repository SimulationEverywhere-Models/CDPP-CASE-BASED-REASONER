#include "modeladm.h"
#include "mainsimu.h"
#include "retain.h"        // class Retain
#include "casebase.h"	   // class CaseBase
#include "retrieve.h"      // class Retrieve
#include "reuse.h"		   // class Reuse
#include "revise.h"        // class Revise
#include "interface.h"	   // class Interface

void MainSimulator::registerNewAtomics()
{
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Retain>(), "Retain" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<CaseBase>(), "CaseBase" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Retrieve>(), "Retrieve" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Reuse>(), "Reuse" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Revise>(), "Revise" ) ;
	SingleModelAdm::Instance().registerAtomic( NewAtomicFunction<Interface>(), "Interface" ) ;

}

//
//   Project Name:        KratosDelaunayMeshingApplication $
//   Created by:          $Author:             JMCarbonell $
//   Last modified by:    $Co-Author:                      $
//   Date:                $Date:                April 2018 $
//   Revision:            $Revision:                   0.0 $
//
//

#if !defined(KRATOS_REFINE_MESH_ELEMENTS_ON_THRESHOLD_PROCESS_H_INCLUDED )
#define  KRATOS_REFINE_MESH_ELEMENTS_ON_THRESHOLD_PROCESS_H_INCLUDED


// External includes

// System includes

// Project includes
#include "containers/variables_list_data_value_container.h"
#include "spatial_containers/spatial_containers.h"

#include "includes/model_part.h"
#include "custom_utilities/mesh_error_calculation_utilities.hpp"
#include "custom_utilities/modeler_utilities.hpp"

///VARIABLES used:
//Data:     
//StepData: 
//Flags:    (checked) BOUNDARY
//          (set)     TO_REFINE(nodes)(set)
//          (modified)  
//          (reset)   
//(set):=(set in this process)

namespace Kratos
{

///@name Kratos Classes
///@{

/// Refine Mesh Elements Process 2D and 3D
/** The process labels the nodes to be refined (TO_REFINE)
    if the ThresholdVariable  is larger than a ReferenceThreshold
*/

class RefineMeshElementsOnThresholdProcess
  : public Process
{
public:
    ///@name Type Definitions
    ///@{

    /// Pointer definition of Process
    KRATOS_CLASS_POINTER_DEFINITION( RefineMeshElementsOnThresholdProcess );

    typedef ModelPart::ConditionType         ConditionType;
    typedef ModelPart::PropertiesType       PropertiesType;
    typedef ConditionType::GeometryType       GeometryType;

    ///@}
    ///@name Life Cycle
    ///@{

    /// Default constructor.
    RefineMeshElementsOnThresholdProcess(ModelPart& rModelPart,
					 ModelerUtilities::MeshingParameters& rRemeshingParameters,
					 int EchoLevel) 
      : mrModelPart(rModelPart),
	mrRemesh(rRemeshingParameters)
    {
      mEchoLevel = EchoLevel;
    }


    /// Destructor.
    virtual ~RefineMeshElementsOnThresholdProcess() {}


    ///@}
    ///@name Operators
    ///@{

    /// This operator is provided to call the process as a function and simply calls the Execute method.
    void operator()()
    {
        Execute();
    }


    ///@}
    ///@name Operations
    ///@{


    /// Execute method is used to execute the Process algorithms.
    void Execute() override
    {
      KRATOS_TRY

	if( ( mrRemesh.Refine->RefiningOptions.Is(ModelerUtilities::REFINE_ADD_NODES) ||  mrRemesh.Refine->RefiningOptions.Is(ModelerUtilities::REFINE_INSERT_NODES) ) && (mrRemesh.Refine->RefiningOptions.Is(ModelerUtilities::REFINE_ELEMENTS_ON_THRESHOLD) ) ){
	  
	  SetNodesToRefine();

	}

      KRATOS_CATCH(" ")
    }


    /// this function is designed for being called at the beginning of the computations
    /// right after reading the model and the groups
    void ExecuteInitialize() override
    {
    }

    /// this function is designed for being execute once before the solution loop but after all of the
    /// solvers where built
    void ExecuteBeforeSolutionLoop() override
    {
    }

    /// this function will be executed at every time step BEFORE performing the solve phase
    void ExecuteInitializeSolutionStep() override
    {	
    }

    /// this function will be executed at every time step AFTER performing the solve phase
    void ExecuteFinalizeSolutionStep() override
    {
    }

    /// this function will be executed at every time step BEFORE  writing the output
    void ExecuteBeforeOutputStep() override
    {
    }

    /// this function will be executed at every time step AFTER writing the output
    void ExecuteAfterOutputStep() override
    {
    }

    /// this function is designed for being called at the end of the computations
    /// right after reading the model and the groups
    void ExecuteFinalize() override
    {
    }


    ///@}
    ///@name Access
    ///@{


    ///@}
    ///@name Inquiry
    ///@{


    ///@}
    ///@name Input and output
    ///@{

    /// Turn back information as a string.
    std::string Info() const override
    {
        return "RefineMeshElementsOnThresholdProcess";
    }

    /// Print information about this object.
    void PrintInfo(std::ostream& rOStream) const override
    {
        rOStream << "RefineMeshElementsOnThresholdProcess";
    }

    /// Print object's data.
    void PrintData(std::ostream& rOStream) const override
    {
    }


    ///@}
    ///@name Friends
    ///@{

    ///@}


private:
    ///@name Static Member Variables
    ///@{

    ///@}
    ///@name Static Member Variables
    ///@{
    ModelPart& mrModelPart;
 
    ModelerUtilities::MeshingParameters& mrRemesh;

    ModelerUtilities mModelerUtilities;  

    int mEchoLevel;

    ///@}
    ///@name Un accessible methods
    ///@{

    //**************************************************************************
    //**************************************************************************

    void SetNodesToRefine()
    {
      KRATOS_TRY
    
      ProcessInfo& CurrentProcessInfo = mrModelPart.GetProcessInfo();
      
      double max_value = 0;
      double critical_value = mrRemesh.Refine->ReferenceThreshold; 

      int counter = 0;
      //set label refine in elements that must be refined due to dissipation
      for(ModelPart::ElementsContainerType::const_iterator iii = mrModelPart.ElementsBegin();
	  iii != mrModelPart.ElementsEnd(); iii++)
	{
	  double variable_value=0;
	  std::vector<double> Value(1);

	  (iii)->GetValueOnIntegrationPoints(mrRemesh.Refine->GetThresholdVariable(),Value,CurrentProcessInfo);
	    
	  //the expected returned value is an "specific" value (per unit of Area) (usually PlasticPower)
	  //variable_value = Value[0] * iii->GetGeometry().Area();
	  variable_value = Value[0] * iii->GetGeometry().DomainSize(); //Area() or Volume()


	
	  if( variable_value > max_value )
	    max_value = variable_value;
 
	  // if(variable_value>0)
	  //   std::cout<<" Element ["<<iii->Id()<<"] "<<mrRemesh.Refine->GetThresholdVariable()<<": "<<variable_value<<" CriticalValue "<<critical_value<<" Area "<<iii->GetGeometry().DomainSize()<<std::endl;
	 
	  if( variable_value > critical_value )
	    {
	      //std::cout<<" Refine element "<<std::endl;
	      Geometry< Node<3> >& rGeometry = iii->GetGeometry();
	      for(unsigned int i = 0; i<rGeometry.size(); i++)
		{
		  if(rGeometry[i].IsNot(BOUNDARY))
		    rGeometry[i].Set(TO_REFINE);
		}
	      counter ++;
	    }
		    
	}
    
      if( mEchoLevel >= 1 ){	
	if( max_value < critical_value )
	  std::cout<<" Threshold Value not REACHED ::  max_value  "<< max_value<<std::endl;

	if( counter > 0 )
	  std::cout<<" Threshold reached "<<counter<<" times "<<std::endl;
      }
      

      if( mEchoLevel >= 1 )
	std::cout<<"   Refine Elements On Threshold [number:"<<counter<<"]"<<std::endl;

      KRATOS_CATCH( "" )
 
    }

    /// Assignment operator.
    RefineMeshElementsOnThresholdProcess& operator=(RefineMeshElementsOnThresholdProcess const& rOther);


    /// this function is a private function


    /// Copy constructor.
    //Process(Process const& rOther);


    ///@}

}; // Class Process

///@}

///@name Type Definitions
///@{


///@}
///@name Input and output
///@{


/// input stream function
inline std::istream& operator >> (std::istream& rIStream,
                                  RefineMeshElementsOnThresholdProcess& rThis);

/// output stream function
inline std::ostream& operator << (std::ostream& rOStream,
                                  const RefineMeshElementsOnThresholdProcess& rThis)
{
    rThis.PrintInfo(rOStream);
    rOStream << std::endl;
    rThis.PrintData(rOStream);

    return rOStream;
}
///@}


}  // namespace Kratos.

#endif // KRATOS_REFINE_MESH_ELEMENTS_ON_THRESHOLD_PROCESS_H_INCLUDED  defined 



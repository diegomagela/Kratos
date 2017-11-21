// ==============================================================================
/*
 KratosALEApllication
 A library based on:
 Kratos
 A General Purpose Software for Multi-Physics Finite Element Analysis
 (Released on march 05, 2007).

 Copyright (c) 2016: Pooyan Dadvand, Riccardo Rossi, Andreas Winterstein
                     pooyan@cimne.upc.edu
                     rrossi@cimne.upc.edu
                     a.winterstein@tum.de
- CIMNE (International Center for Numerical Methods in Engineering),
  Gran Capita' s/n, 08034 Barcelona, Spain
- Chair of Structural Analysis, Technical University of Munich
  Arcisstrasse 21 80333 Munich, Germany

 Permission is hereby granted, free  of charge, to any person obtaining
 a  copy  of this  software  and  associated  documentation files  (the
 "Software"), to  deal in  the Software without  restriction, including
 without limitation  the rights to  use, copy, modify,  merge, publish,
 distribute,  sublicense and/or  sell copies  of the  Software,  and to
 permit persons to whom the Software  is furnished to do so, subject to
 the following condition:

 Distribution of this code for  any  commercial purpose  is permissible
 ONLY BY DIRECT ARRANGEMENT WITH THE COPYRIGHT OWNERS.

 The  above  copyright  notice  and  this permission  notice  shall  be
 included in all copies or substantial portions of the Software.

 THE  SOFTWARE IS  PROVIDED  "AS  IS", WITHOUT  WARRANTY  OF ANY  KIND,
 EXPRESS OR  IMPLIED, INCLUDING  BUT NOT LIMITED  TO THE  WARRANTIES OF
 MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 IN NO EVENT  SHALL THE AUTHORS OR COPYRIGHT HOLDERS  BE LIABLE FOR ANY
 CLAIM, DAMAGES OR  OTHER LIABILITY, WHETHER IN AN  ACTION OF CONTRACT,
 TORT  OR OTHERWISE, ARISING  FROM, OUT  OF OR  IN CONNECTION  WITH THE
 SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
//==============================================================================

/* ****************************************************************************
 *  Projectname:         $KratosALEApplication
 *  Last Modified by:    $Author: A.Winterstein@tum.de $
 *  Date:                $Date: June 2016 $
 *  Revision:            $Revision: 1.5 $
 * ***************************************************************************/

#if !defined(KRATOS_NEW_STRUCTURAL_MESHMOVING_STRATEGY )
#define  KRATOS_NEW_STRUCTURAL_MESHMOVING_STRATEGY

/* System includes */

/* External includes */
#include "spaces/ublas_space.h"

/* Project includes */
#include "includes/define.h"
#include "includes/model_part.h"
#include "solving_strategies/strategies/solving_strategy.h"
#include "solving_strategies/strategies/residualbased_linear_strategy.h"
#include "solving_strategies/schemes/residualbased_incrementalupdate_static_scheme.h"
#include "solving_strategies/builder_and_solvers/residualbased_block_builder_and_solver.h"
#include "custom_elements/structural_meshmoving_element.h"

#include "ale_application.h"

namespace Kratos {

/**@name Kratos Globals */
/*@{ */

/*@} */
/**@name Type Definitions */
/*@{ */

/*@} */

/**@name  Enum's */
/*@{ */

/*@} */
/**@name  Functions */
/*@{ */

/*@} */
/**@name Kratos Classes */
/*@{ */

/// Short class definition.
/**   Detail class definition.
 */
template<class TSparseSpace, class TDenseSpace,  //= DenseSpace<double>,
    class TLinearSolver  //= LinearSolver<TSparseSpace,TDenseSpace>
>
class StructuralMeshMovingStrategy : public SolvingStrategy<TSparseSpace,
    TDenseSpace, TLinearSolver> {
 public:
  /**@name Type Definitions */
  /*@{ */

  /** Counted pointer of ClassName */
  KRATOS_CLASS_POINTER_DEFINITION( StructuralMeshMovingStrategy );

  typedef SolvingStrategy<TSparseSpace,TDenseSpace,TLinearSolver> BaseType;

  /*@} */
  /**@name Life Cycle
   */
  /*@{ */

  /** Constructor.
   */
  StructuralMeshMovingStrategy(
      ModelPart& model_part,
      typename TLinearSolver::Pointer pNewLinearSolver,
      int velocity_order = 2,
      bool reform_dof_at_every_step = false,
      bool compute_reactions = false
  )
  : SolvingStrategy<TSparseSpace,TDenseSpace,TLinearSolver>(model_part)
  {
    KRATOS_TRY

    //Generating Mesh Part
    GenerateMeshPart();
    mvel_order = velocity_order;
    mreform_dof_at_every_step = reform_dof_at_every_step;

    typedef Scheme< TSparseSpace, TDenseSpace > SchemeType;
    //typedef ConvergenceCriteria<TSparseSpace, TDenseSpace> TConvergenceCriteriaType;

    typename SchemeType::Pointer pscheme = typename SchemeType::Pointer
        ( new ResidualBasedIncrementalUpdateStaticScheme< TSparseSpace,TDenseSpace >() );
    typedef typename BuilderAndSolver<TSparseSpace,TDenseSpace,TLinearSolver>::Pointer BuilderSolverTypePointer;

    //bool CalculateReactions = false;
    bool ReformDofAtEachIteration = false;
    bool CalculateNormDxFlag = false;
    bool compute_reactions = false;

    BuilderSolverTypePointer pBuilderSolver = BuilderSolverTypePointer
        (new ResidualBasedBlockBuilderAndSolver<TSparseSpace,TDenseSpace,TLinearSolver>(pNewLinearSolver) );

    mstrategy = typename BaseType::Pointer
        ( new ResidualBasedLinearStrategy<TSparseSpace,TDenseSpace,TLinearSolver >
    (*mpMeshModelPart,pscheme,pNewLinearSolver,pBuilderSolver,compute_reactions,ReformDofAtEachIteration,CalculateNormDxFlag) );

    KRATOS_CATCH("")
  }


  virtual ~StructuralMeshMovingStrategy() {}

  void Initialize() override
  {}

  double Solve() override
  {
    KRATOS_TRY;


    // Setting mesh to initial configuration
    for(ModelPart::NodeIterator i = (*mpMeshModelPart).NodesBegin(); i != (*mpMeshModelPart).NodesEnd(); ++i){

      (i)->X() = (i)->X0();
      (i)->Y() = (i)->Y0();
      (i)->Z() = (i)->Z0();
    }


    // Solve for the mesh movement
    mstrategy->Solve();

    // Update FEM-base
    CalculateMeshVelocities();
    // Optimization iteration: MoveMesh changes the reference
    MoveMesh();

    // Clearing the system if needed
    if(mreform_dof_at_every_step == true)
    mstrategy->Clear();

    return 0.0;

    KRATOS_CATCH("");
  }


  void CalculateMeshVelocities()
  {
    KRATOS_TRY;

    double DeltaTime = BaseType::GetModelPart().GetProcessInfo()[DELTA_TIME];

    //TODO Delta Time for adjoint always negative
    if (DeltaTime <= 0.0)
    DeltaTime = DeltaTime*(-1.0);
    //KRATOS_THROW_ERROR(std::logic_error, "Invalid DELTA_TIME.","");

    double coeff = 1/DeltaTime;
    if( mvel_order == 1)  //mesh velocity calculated as (x(n+1)-x(n))/Dt
    {
      for(ModelPart::NodeIterator i = (*mpMeshModelPart).NodesBegin();
          i != (*mpMeshModelPart).NodesEnd(); ++i){

        array_1d<double,3>& mesh_v = (i)->FastGetSolutionStepValue(MESH_VELOCITY);
        array_1d<double,3>& disp = (i)->FastGetSolutionStepValue(MESH_DISPLACEMENT);
        array_1d<double,3>& dispold = (i)->FastGetSolutionStepValue(MESH_DISPLACEMENT,1);
        noalias(mesh_v) = disp - dispold;
        mesh_v *= coeff;
      }
    }
    else  //mesh velocity calculated as (3*x(n+1)-4*x(n)+x(n-1))/(2*Dt)
    {
      double c1 = 1.50*coeff;
      double c2 = -2.0*coeff;
      double c3 = 0.50*coeff;

      for(ModelPart::NodeIterator i = (*mpMeshModelPart).NodesBegin();
          i != (*mpMeshModelPart).NodesEnd(); ++i){

        array_1d<double,3>& mesh_v = (i)->FastGetSolutionStepValue(MESH_VELOCITY);
        noalias(mesh_v) = c1 * (i)->FastGetSolutionStepValue(MESH_DISPLACEMENT);
        noalias(mesh_v) += c2 * (i)->FastGetSolutionStepValue(MESH_DISPLACEMENT,1);
        noalias(mesh_v) += c3 * (i)->FastGetSolutionStepValue(MESH_DISPLACEMENT,2);
      }
    }

    KRATOS_CATCH("");
  }

  void SetEchoLevel(int Level) override
  {
    mstrategy->SetEchoLevel(Level);
  }

  void MoveMesh() override
  {
    for (ModelPart::NodeIterator i = BaseType::GetModelPart().NodesBegin();
            i != BaseType::GetModelPart().NodesEnd(); ++i)
    {
        (i)->X() = (i)->X0() + i->GetSolutionStepValue(MESH_DISPLACEMENT_X);
        (i)->Y() = (i)->Y0() + i->GetSolutionStepValue(MESH_DISPLACEMENT_Y);
        (i)->Z() = (i)->Z0() + i->GetSolutionStepValue(MESH_DISPLACEMENT_Z);
    }
  }


  void UpdateReferenceMesh()
{
  for (ModelPart::NodeIterator i = BaseType::GetModelPart().NodesBegin();
          i != BaseType::GetModelPart().NodesEnd(); ++i)
  {
      (i)->X0() = (i)->X();
      (i)->Y0() = (i)->Y();
      (i)->Z0() = (i)->Z();
  }
}

  /*@} */
  /**@name Operators
   */
  /*@{ */

  /*@} */
  /**@name Operations */
  /*@{ */

  /*@} */
  /**@name Access */
  /*@{ */

  /*@} */
  /**@name Inquiry */
  /*@{ */

  /*@} */
  /**@name Friends */
  /*@{ */

  /*@} */

protected:
  /**@name Protected static Member Variables */
  /*@{ */

  /*@} */
  /**@name Protected member Variables */
  /*@{ */

  /*@} */
  /**@name Protected Operators*/
  /*@{ */

  /*@} */
  /**@name Protected Operations*/
  /*@{ */

  /*@} */
  /**@name Protected  Access */
  /*@{ */

  /*@} */
  /**@name Protected Inquiry */
  /*@{ */

  /*@} */
  /**@name Protected LifeCycle */
  /*@{ */

  /*@} */

private:
  /**@name Static Member Variables */
  /*@{ */

  /*@} */
  /**@name Member Variables */
  /*@{ */
  ModelPart::Pointer mpMeshModelPart;

  typename BaseType::Pointer mstrategy;

  int mvel_order;
  bool mreform_dof_at_every_step;

  /*@} */
  /**@name Private Operators*/
  /*@{ */

  /*@} */
  /**@name Private Operations*/
  /*@{ */

  void GenerateMeshPart()
  {
    mpMeshModelPart = ModelPart::Pointer( new ModelPart("MeshPart",1) );

    mpMeshModelPart->Nodes() = BaseType::GetModelPart().Nodes();

    mpMeshModelPart->GetNodalSolutionStepVariablesList() = BaseType::GetModelPart().GetNodalSolutionStepVariablesList();
    mpMeshModelPart->SetBufferSize( BaseType::GetModelPart().GetBufferSize() );

    // Creating mesh elements
    ModelPart::ElementsContainerType& MeshElems = mpMeshModelPart->Elements();
    Element::Pointer pElem;

    for(ModelPart::ElementsContainerType::iterator it = BaseType::GetModelPart().ElementsBegin();
        it != BaseType::GetModelPart().ElementsEnd(); ++it){

      pElem = Element::Pointer(new StructuralMeshMovingElement(
              (*it).Id(),
              (*it).pGetGeometry(),
              (*it).pGetProperties() ) );
      MeshElems.push_back(pElem);
    }
  }

  /*@} */
  /**@name Private  Access */
  /*@{ */

  /*@} */
  /**@name Private Inquiry */
  /*@{ */

  /*@} */
  /**@name Un accessible methods */
  /*@{ */

  /** Copy constructor.
   */
  StructuralMeshMovingStrategy(const StructuralMeshMovingStrategy& Other);

  /*@} */

}; /* Class StructuralMeshMovingStrategy */

/*@} */

/**@name Type Definitions */
/*@{ */

/*@} */

}
/* namespace Kratos.*/

#endif /* KRATOS_NEW_STRUCTURAL_MESHMOVING_STRATEGY  defined */
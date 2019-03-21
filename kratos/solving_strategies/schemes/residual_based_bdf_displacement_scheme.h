//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Vicente Mataix Ferrandiz
//


#if !defined(KRATOS_RESIDUAL_BASED_BDF_DISPLACEMENT_SCHEME )
#define  KRATOS_RESIDUAL_BASED_BDF_DISPLACEMENT_SCHEME

/* System includes */

/* External includes */

/* Project includes */
#include "solving_strategies/schemes/residual_based_bdf_scheme.h"
#include "includes/variables.h"
#include "includes/checks.h"

namespace Kratos
{
///@name Kratos Globals
///@{
///@}
///@name Type Definitions
///@{
///@}
///@name  Enum's
///@{
///@}
///@name  Functions
///@{
///@}
///@name Kratos Classes
///@{

/**
 * @class ResidualBasedBDFDisplacementScheme
 * @ingroup KratosCore
 * @brief BDF integration scheme (displacement based)
 * @details The \f$ n \f$ order Backward Differentiation Formula (BDF) method is a two step \f$ n \f$ order accurate method.
 * Look at the base class for more details
 * @see ResidualBasedBDFScheme
 * @author Vicente Mataix Ferrandiz
 */
template<class TSparseSpace,  class TDenseSpace>
class ResidualBasedBDFDisplacementScheme
    : public ResidualBasedBDFScheme<TSparseSpace, TDenseSpace>
{
public:
    ///@name Type Definitions
    ///@{

    /// Pointer definition of ResidualBasedBDFDisplacementScheme
    KRATOS_CLASS_POINTER_DEFINITION( ResidualBasedBDFDisplacementScheme );

    /// Base class definition
    typedef Scheme<TSparseSpace,TDenseSpace>                                  BaseType;
    typedef ResidualBasedImplicitTimeScheme<TSparseSpace,TDenseSpace> ImplicitBaseType;
    typedef ResidualBasedBDFScheme<TSparseSpace,TDenseSpace>               BDFBaseType;

    /// Data type definition
    typedef typename BDFBaseType::TDataType                                  TDataType;
    /// Matrix type definition
    typedef typename BDFBaseType::TSystemMatrixType                  TSystemMatrixType;
    /// Vector type definition
    typedef typename BDFBaseType::TSystemVectorType                  TSystemVectorType;
    /// Local system matrix type definition
    typedef typename BDFBaseType::LocalSystemVectorType          LocalSystemVectorType;
    /// Local system vector type definition
    typedef typename BDFBaseType::LocalSystemMatrixType          LocalSystemMatrixType;

    /// DoF array type definition
    typedef typename BDFBaseType::DofsArrayType                          DofsArrayType;
    /// DoF vector type definition
    typedef typename Element::DofsVectorType                            DofsVectorType;

    /// Nodes containers definition
    typedef ModelPart::NodesContainerType                               NodesArrayType;
    /// Elements containers definition
    typedef ModelPart::ElementsContainerType                         ElementsArrayType;
    /// Conditions containers definition
    typedef ModelPart::ConditionsContainerType                     ConditionsArrayType;

    ///@}
    ///@name Life Cycle
    ///@{

    /**
     * @brief Constructor. The BDF method (parameters)
     * @param ThisParameters Parameters with the integration order
     */
    explicit ResidualBasedBDFDisplacementScheme(Parameters ThisParameters)
        : ResidualBasedBDFDisplacementScheme(ThisParameters.Has("integration_order") ? static_cast<std::size_t>(ThisParameters["integration_order"].GetInt()) : 2)
    {
        // Validate default parameters
        Parameters default_parameters = Parameters(R"(
        {   "name"              : "ResidualBasedBDFDisplacementScheme",
            "integration_order" : 2
        })" );
        ThisParameters.ValidateAndAssignDefaults(default_parameters);
    }

    /**
     * @brief Constructor. The BDF method
     * @param Order The integration order
     * @todo The ideal would be to use directly the dof or the variable itself to identify the type of variable and is derivatives
     */
    explicit ResidualBasedBDFDisplacementScheme(const std::size_t Order = 2)
        :BDFBaseType(Order)
    {
    }

    /** Copy Constructor.
     */
    explicit ResidualBasedBDFDisplacementScheme(ResidualBasedBDFDisplacementScheme& rOther)
        :BDFBaseType(rOther)
    {
    }

    /**
     * Clone
     */
    typename BaseType::Pointer Clone() override
    {
        return Kratos::make_shared<ResidualBasedBDFDisplacementScheme>(*this);
    }

    /** Destructor.
     */
    ~ResidualBasedBDFDisplacementScheme
    () override {}

    ///@}
    ///@name Operators
    ///@{

    ///@}
    ///@name Operations
    ///@{

    /**
     * @brief It initializes time step solution. Only for reasons if the time step solution is restarted
     * @param rModelPart The model part of the problem to solve
     * @param rA LHS matrix
     * @param rDx Incremental update of primary variables
     * @param rb RHS Vector
     */
    void InitializeSolutionStep(
        ModelPart& rModelPart,
        TSystemMatrixType& rA,
        TSystemVectorType& rDx,
        TSystemVectorType& rb
        ) override
    {
        KRATOS_TRY;

        BDFBaseType::InitializeSolutionStep(rModelPart, rA, rDx, rb);

        // Updating time derivatives (nodally for efficiency)
        const int num_nodes = static_cast<int>( rModelPart.Nodes().size() );
        const auto it_node_begin = rModelPart.Nodes().begin();

        // Getting position
        const int velpos_x = it_node_begin->HasDofFor(VELOCITY_X) ? it_node_begin->GetDofPosition(VELOCITY_X) : -1;
        const int accelpos_x = it_node_begin->HasDofFor(ACCELERATION_X) ? it_node_begin->GetDofPosition(ACCELERATION_X) : -1;
        const int velpos_y = it_node_begin->HasDofFor(VELOCITY_Y) ? it_node_begin->GetDofPosition(VELOCITY_Y) : -1;
        const int accelpos_y = it_node_begin->HasDofFor(ACCELERATION_Y) ? it_node_begin->GetDofPosition(ACCELERATION_Y) : -1;
        const int velpos_z = it_node_begin->HasDofFor(VELOCITY_Z) ? it_node_begin->GetDofPosition(VELOCITY_Z) : -1;
        const int accelpos_z = it_node_begin->HasDofFor(ACCELERATION_Z) ? it_node_begin->GetDofPosition(ACCELERATION_Z) : -1;

        bool fixed_x, fixed_y, fixed_z;

        #pragma omp parallel for private(fixed_x, fixed_y, fixed_z)
        for(int i = 0;  i < num_nodes; ++i) {
            auto it_node = it_node_begin + i;

            fixed_x = false;
            fixed_y = false;
            fixed_z = false;

            if (accelpos_x > -1) {
                if (it_node->GetDof(ACCELERATION_X, accelpos_x).IsFixed()) {
                    it_node->Fix(DISPLACEMENT_X);
                    fixed_x = true;
                }
            }
            if (velpos_x > -1 && !fixed_x) {
                if (it_node->GetDof(VELOCITY_X, velpos_x).IsFixed() && !fixed_x) {
                    it_node->Fix(DISPLACEMENT_X);
                }
            }
            if (accelpos_y > -1) {
                if (it_node->GetDof(ACCELERATION_Y, accelpos_y).IsFixed()) {
                    it_node->Fix(DISPLACEMENT_Y);
                    fixed_y = true;
                }
            }
            if (velpos_y > -1 && !fixed_y) {
                if (it_node->GetDof(VELOCITY_Y, velpos_y).IsFixed() && !fixed_y) {
                    it_node->Fix(DISPLACEMENT_Y);
                }
            }
            if (accelpos_z > -1) {
                if (it_node->GetDof(ACCELERATION_Z, accelpos_z).IsFixed()) {
                    it_node->Fix(DISPLACEMENT_Z);
                    fixed_z = true;
                }
            }
            if (velpos_z > -1 && !fixed_z) {
                if (it_node->GetDof(VELOCITY_Z, velpos_z).IsFixed() && !fixed_z) {
                    it_node->Fix(DISPLACEMENT_Z);
                }
            }
        }

        KRATOS_CATCH("ResidualBasedBDFDisplacementScheme.InitializeSolutionStep");
    }

    /**
     * @brief Performing the prediction of the solution
     * @details It predicts the solution for the current step x = xold + vold * Dt
     * @param rModelPart The model of the problem to solve
     * @param rDofSet Set of all primary variables
     * @param A LHS matrix
     * @param Dx Incremental update of primary variables
     * @param b RHS Vector
     */
    void Predict(
        ModelPart& rModelPart,
        DofsArrayType& rDofSet,
        TSystemMatrixType& A,
        TSystemVectorType& Dx,
        TSystemVectorType& b
        ) override
    {
        KRATOS_TRY;

        // The current process info
        const ProcessInfo& r_current_process_info = rModelPart.GetProcessInfo();
        const double delta_time = r_current_process_info[DELTA_TIME];

        // Updating time derivatives (nodally for efficiency)
        const int num_nodes = static_cast<int>( rModelPart.Nodes().size() );
        const auto it_node_begin = rModelPart.Nodes().begin();

        // Getting position
        const int disppos_x = it_node_begin->HasDofFor(DISPLACEMENT_X) ? it_node_begin->GetDofPosition(DISPLACEMENT_X) : -1;
        const int velpos_x = it_node_begin->HasDofFor(VELOCITY_X) ? it_node_begin->GetDofPosition(VELOCITY_X) : -1;
        const int accelpos_x = it_node_begin->HasDofFor(ACCELERATION_X) ? it_node_begin->GetDofPosition(ACCELERATION_X) : -1;
        const int disppos_y = it_node_begin->HasDofFor(DISPLACEMENT_Y) ? it_node_begin->GetDofPosition(DISPLACEMENT_Y) : -1;
        const int velpos_y = it_node_begin->HasDofFor(VELOCITY_Y) ? it_node_begin->GetDofPosition(VELOCITY_Y) : -1;
        const int accelpos_y = it_node_begin->HasDofFor(ACCELERATION_Y) ? it_node_begin->GetDofPosition(ACCELERATION_Y) : -1;
        const int disppos_z = it_node_begin->HasDofFor(DISPLACEMENT_Z) ? it_node_begin->GetDofPosition(DISPLACEMENT_Z) : -1;
        const int velpos_z = it_node_begin->HasDofFor(VELOCITY_Z) ? it_node_begin->GetDofPosition(VELOCITY_Z) : -1;
        const int accelpos_z = it_node_begin->HasDofFor(ACCELERATION_Z) ? it_node_begin->GetDofPosition(ACCELERATION_Z) : -1;

        // Auxiliar variables
        bool predicted_x, predicted_y, predicted_z;

        #pragma omp parallel for private(predicted_x, predicted_y, predicted_z)
        for(int i = 0;  i< num_nodes; ++i) {
            auto it_node = it_node_begin + i;

            predicted_x = false;
            predicted_y = false;
            predicted_z = false;


            const array_1d<double, 3>& dot2un1 = it_node->FastGetSolutionStepValue(ACCELERATION, 1);
            const array_1d<double, 3>& dotun1 = it_node->FastGetSolutionStepValue(VELOCITY,     1);
            const array_1d<double, 3>& un1 = it_node->FastGetSolutionStepValue(DISPLACEMENT, 1);
            const array_1d<double, 3>& dot2un0 = it_node->FastGetSolutionStepValue(ACCELERATION);
            array_1d<double, 3>& dotun0 = it_node->FastGetSolutionStepValue(VELOCITY);
            array_1d<double, 3>& un0 = it_node->FastGetSolutionStepValue(DISPLACEMENT);

            if (accelpos_x > -1) {
                if (it_node->GetDof(ACCELERATION_X, accelpos_x).IsFixed()) {
                    dotun0[0] = dot2un0[0];
                    for (std::size_t i_order = 1; i_order < BDFBaseType::mOrder + 1; ++i_order)
                        dotun0[0] -= BDFBaseType::mBDF[i_order] * it_node->FastGetSolutionStepValue(VELOCITY_X, i_order);
                    dotun0[0] /= BDFBaseType::mBDF[0];

                    un0[0] = dotun0[0];
                    for (std::size_t i_order = 1; i_order < BDFBaseType::mOrder + 1; ++i_order)
                        un0[0] -= BDFBaseType::mBDF[i_order] * it_node->FastGetSolutionStepValue(DISPLACEMENT_X, i_order);
                    un0[0] /= BDFBaseType::mBDF[0];
                    predicted_x = true;
                }
            }
            if (velpos_x > -1 && !predicted_x) {
                if (it_node->GetDof(VELOCITY_X, velpos_x).IsFixed() && !predicted_x) {
                    un0[0] = dotun0[0];
                    for (std::size_t i_order = 1; i_order < BDFBaseType::mOrder + 1; ++i_order)
                        un0[0] -= BDFBaseType::mBDF[i_order] * it_node->FastGetSolutionStepValue(DISPLACEMENT_X, i_order);
                    un0[0] /= BDFBaseType::mBDF[0];
                    predicted_x = true;
                }
            }
            if (disppos_x > -1 && !predicted_x) {
                if (!it_node->GetDof(DISPLACEMENT_X, disppos_x).IsFixed() && !predicted_x) {
                    un0[0] = un1[0] + delta_time * dotun1[0] + 0.5 * std::pow(delta_time, 2) * dot2un1[0];
                }
            }

            if (accelpos_y > -1) {
                if (it_node->GetDof(ACCELERATION_Y, accelpos_y).IsFixed()) {
                    dotun0[1] = dot2un0[1];
                    for (std::size_t i_order = 1; i_order < BDFBaseType::mOrder + 1; ++i_order)
                        dotun0[1] -= BDFBaseType::mBDF[i_order] * it_node->FastGetSolutionStepValue(VELOCITY_Y, i_order);
                    dotun0[1] /= BDFBaseType::mBDF[0];

                    un0[1] = dotun0[1];
                    for (std::size_t i_order = 1; i_order < BDFBaseType::mOrder + 1; ++i_order)
                        un0[1] -= BDFBaseType::mBDF[i_order] * it_node->FastGetSolutionStepValue(DISPLACEMENT_Y, i_order);
                    un0[1] /= BDFBaseType::mBDF[0];
                    predicted_y = true;
                }
            }
            if (velpos_y > -1 && !predicted_y) {
                if (it_node->GetDof(VELOCITY_Y, velpos_y).IsFixed() && !predicted_y) {
                    un0[1] = dotun0[1];
                    for (std::size_t i_order = 1; i_order < BDFBaseType::mOrder + 1; ++i_order)
                        un0[1] -= BDFBaseType::mBDF[i_order] * it_node->FastGetSolutionStepValue(DISPLACEMENT_Y, i_order);
                    un0[1] /= BDFBaseType::mBDF[0];
                    predicted_y = true;
                }
            }
            if (disppos_y > -1 && !predicted_y) {
                if (!it_node->GetDof(DISPLACEMENT_Y, disppos_y).IsFixed() && !predicted_y) {
                    un0[1] = un1[1] + delta_time * dotun1[1] + 0.5 * std::pow(delta_time, 2) * dot2un1[1];
                }
            }

            // For 3D cases
            if (accelpos_z > -1) {
                if (it_node->GetDof(ACCELERATION_Z, accelpos_x + 2).IsFixed()) {
                    dotun0[2] = dot2un0[2];
                    for (std::size_t i_order = 1; i_order < BDFBaseType::mOrder + 1; ++i_order)
                        dotun0[2] -= BDFBaseType::mBDF[i_order] * it_node->FastGetSolutionStepValue(VELOCITY_Z, i_order);
                    dotun0[2] /= BDFBaseType::mBDF[0];

                    un0[2] = dotun0[2];
                    for (std::size_t i_order = 1; i_order < BDFBaseType::mOrder + 1; ++i_order)
                        un0[2] -= BDFBaseType::mBDF[i_order] * it_node->FastGetSolutionStepValue(DISPLACEMENT_Z, i_order);
                    un0[2] /= BDFBaseType::mBDF[0];
                    predicted_z = true;
                }
            }
            if (velpos_z > -1 && !predicted_z) {
                if (it_node->GetDof(VELOCITY_Z, velpos_z).IsFixed() && !predicted_z) {
                    un0[2] = dotun0[2];
                    for (std::size_t i_order = 1; i_order < BDFBaseType::mOrder + 1; ++i_order)
                        un0[2] -= BDFBaseType::mBDF[i_order] * it_node->FastGetSolutionStepValue(DISPLACEMENT_Z, i_order);
                    un0[2] /= BDFBaseType::mBDF[0];
                    predicted_z = true;
                }
            }
            if (disppos_z > -1 && !predicted_z) {
                if (!it_node->GetDof(DISPLACEMENT_Z, disppos_z).IsFixed() && !predicted_z) {
                    un0[2] = un1[2] + delta_time * dotun1[2] + 0.5 * std::pow(delta_time, 2) * dot2un1[2];
                }
            }

            // Updating time derivatives
            UpdateFirstDerivative(it_node);
            UpdateSecondDerivative(it_node);
        }

        KRATOS_CATCH( "" );
    }

    /**
     * @brief This function is designed to be called once to perform all the checks needed
     * on the input provided.
     * @details Checks can be "expensive" as the function is designed
     * to catch user's errors.
     * @param rModelPart The model of the problem to solve
     * @return Zero means  all ok
     */
    int Check(ModelPart& rModelPart) override
    {
        KRATOS_TRY;

        const int err = BDFBaseType::Check(rModelPart);
        if(err!=0) return err;

        // Check for variables keys
        // Verify that the variables are correctly initialized
        KRATOS_CHECK_VARIABLE_KEY(DISPLACEMENT)
        KRATOS_CHECK_VARIABLE_KEY(VELOCITY)
        KRATOS_CHECK_VARIABLE_KEY(ACCELERATION)

        // Check that variables are correctly allocated
        for(auto& rnode : rModelPart.Nodes()) {
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(DISPLACEMENT,rnode)
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(VELOCITY,rnode)
            KRATOS_CHECK_VARIABLE_IN_NODAL_DATA(ACCELERATION,rnode)

            KRATOS_CHECK_DOF_IN_NODE(DISPLACEMENT_X, rnode)
            KRATOS_CHECK_DOF_IN_NODE(DISPLACEMENT_Y, rnode)
            KRATOS_CHECK_DOF_IN_NODE(DISPLACEMENT_Z, rnode)
        }

        KRATOS_CATCH( "" );

        return 0;
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
        return "ResidualBasedBDFDisplacementScheme";
    }

    /// Print information about this object.
    void PrintInfo(std::ostream& rOStream) const override
    {
        rOStream << Info();
    }

    /// Print object's data.
    void PrintData(std::ostream& rOStream) const override
    {
        rOStream << Info();
    }

    ///@}
    ///@name Friends
    ///@{

protected:

    ///@name Protected static Member Variables
    ///@{

    ///@}
    ///@name Protected member Variables
    ///@{

    ///@}
    ///@name Protected Operators
    ///@{

    ///@}
    ///@name Protected Operations
    ///@{

    /**
     * @brief Updating first time derivative (velocity)
     * @param itNode the node interator
     */
    inline void UpdateFirstDerivative(NodesArrayType::iterator itNode) override
    {
        array_1d<double, 3>& dotun0 = itNode->FastGetSolutionStepValue(VELOCITY);
        noalias(dotun0) = BDFBaseType::mBDF[0] * itNode->FastGetSolutionStepValue(DISPLACEMENT);
        for (std::size_t i_order = 1; i_order < BDFBaseType::mOrder + 1; ++i_order)
            noalias(dotun0) += BDFBaseType::mBDF[i_order] * itNode->FastGetSolutionStepValue(DISPLACEMENT, i_order);
    }

    /**
     * @brief Updating second time derivative (acceleration)
     * @param itNode the node interator
     */
    inline void UpdateSecondDerivative(NodesArrayType::iterator itNode) override
    {
        array_1d<double, 3>& dot2un0 = itNode->FastGetSolutionStepValue(ACCELERATION);
        noalias(dot2un0) = BDFBaseType::mBDF[0] * itNode->FastGetSolutionStepValue(VELOCITY);
        for (std::size_t i_order = 1; i_order < BDFBaseType::mOrder + 1; ++i_order)
            noalias(dot2un0) += BDFBaseType::mBDF[i_order] * itNode->FastGetSolutionStepValue(VELOCITY, i_order);
    }

    ///@}
    ///@name Protected  Access
    ///@{

    ///@}
    ///@name Protected Inquiry
    ///@{

    ///@}
    ///@name Protected LifeCycle
    ///@{
    ///@{

private:

    ///@name Static Member Variables
    ///@{
    ///@}
    ///@name Member Variables
    ///@{

    ///@}
    ///@name Private Operators
    ///@{

    ///@}
    ///@name Private Operations
    ///@{

    ///@}
    ///@name Private  Access
    ///@{
    ///@}

    ///@}
    ///@name Serialization
    ///@{

    ///@name Private Inquiry
    ///@{
    ///@}
    ///@name Un accessible methods
    ///@{
    ///@}
}; /* Class ResidualBasedBDFDisplacementScheme */
///@}
///@name Type Definitions
///@{
///@}
///@name Input and output
///@{
///@}
}  /* namespace Kratos.*/

#endif /* KRATOS_RESIDUAL_BASED_BDF_DISPLACEMENT_SCHEME defined */

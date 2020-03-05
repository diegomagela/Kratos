//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ \.
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics FemDem Application
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Alejandro Cornejo Velazquez
//

#if !defined(KRATOS_AITKEN_RELAXATION_UTILITY)
#define  KRATOS_AITKEN_RELAXATION_UTILITY

// System includes

// External includes
#include "utilities/math_utils.h"
#include "spaces/ublas_space.h"

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

/** @brief Aitken relaxation technique for FSI PFEM-FEM-DEM coupling
 */
class AitkenRelaxationUtility
{
public:

    ///@name Type Definitions
    ///@{
    KRATOS_CLASS_POINTER_DEFINITION(AitkenRelaxationUtility);

    typedef UblasSpace<double, Matrix, Vector> TSpace;
    typedef typename TSpace::VectorType VectorType;
    typedef typename TSpace::VectorPointerType VectorPointerType;

    ///@}
    ///@name Life Cycle
    ///@{

   /**
     * Constructor.
     * AitkenRelaxationUtility
     */
    AitkenRelaxationUtility(const double OmegaOld = 0.825)
    {
        mOmegaOld = OmegaOld;
    }

    /**
     * Copy Constructor.
     */
    AitkenRelaxationUtility(const AitkenRelaxationUtility& rOther)
    {
        mOmegaOld = rOther.mOmegaOld;
    }

    /**
     * Destructor.
     */
    virtual ~AitkenRelaxationUtility() {}


    ///@name Operators
    ///@{
    ///@}

    ///@name Operations
    ///@{

    /**
     * Initialize the internal iteration counter
     */
    void InitializeSolutionStep()
    {
        KRATOS_TRY;

        mConvergenceAcceleratorIteration = 1;

        KRATOS_CATCH( "" );
    }

    /**
     * Performs the solution update
     * The correction is done as u_i+1 = u_i + w_i+1*r_i+1 where w_i+1 is de relaxation parameter computed using the Aitken's formula.
     * @param rResidualVector: Residual vector from the residual evaluation
     * @param rIterationGuess: Current iteration guess
     */
    void UpdateSolution(
        const Vector& rResidualVector,
        Vector& rIterationGuess
        )
    {

    }

    /**
     * Updates the Aitken iteration values for the next non-linear iteration
     */
    void FinalizeNonLinearIteration()
    {
        KRATOS_TRY;
        // mpResidualVector_0 = mpResidualVector_1;
        std::swap(mpResidualVectorOld, mpResidualVectorNew);
        mConvergenceAcceleratorIteration += 1;
        KRATOS_CATCH("");
    }

    /**
     * Reset the convergence accelerator iterations counter
     */
    void FinalizeSolutionStep()
    {
        KRATOS_TRY;
        mConvergenceAcceleratorIteration = 1;
        KRATOS_CATCH("");
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
    ///@}

    ///@name Friends
    ///@{
    ///@}

protected:

    ///@name Protected static Member Variables
    ///@{
    ///@}

    ///@name Protected member Variables
    ///@{

    unsigned int mConvergenceAcceleratorIteration;

    double mOmegaOld;
    double mOmegaNew;

    VectorPointerType mpResidualVectorOld;
    VectorPointerType mpResidualVectorNew;

    ///@}

    ///@name Protected Operators
    ///@{
    ///@}

    ///@name Protected Operations
    ///@{
    ///@}

    ///@name Protected  Access
    ///@{
    ///@}

    ///@name Protected Inquiry
    ///@{
    ///@}

    ///@name Protected LifeCycle
    ///@{
    ///@}

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

    ///@name Serialization
    ///@{
    ///@}

    ///@name Private Inquiry
    ///@{
    ///@}

    ///@name Un accessible methods
    ///@{
    ///@}

}; /* Class AitkenConvergenceAccelerator */

///@}

///@name Type Definitions
///@{
///@}

///@name Input and output
///@{
///@}

} // namespace Kratos

#endif /* KRATOS_AITKEN_RELAXATION_UTILITY defined */
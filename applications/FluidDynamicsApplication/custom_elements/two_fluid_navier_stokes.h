//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:         BSD License
//                   Kratos default license: kratos/license.txt
//
//  Main author:     Daniel Diez
//  Co-authors:      Ruben Zorrilla
//



#if !defined(KRATOS_TWO_FLUID_NAVIER_STOKES)
#define  KRATOS_TWO_FLUID_NAVIER_STOKES

// System includes


// Project includes
#include "includes/define.h"
#include "includes/element.h"
#include "includes/variables.h"
#include "includes/serializer.h"

#include "utilities/geometry_utilities.h"
#include "includes/cfd_variables.h"
#include "utilities/split_tetrahedra.h"
#include "custom_utilities/fluid_element_utilities.h"
#include "custom_elements/fluid_element.h"



#include "utilities/enrichment_utilities_duplicate_dofs.h"

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


template< class TElementData >
class TwoFluidNavierStokes : public FluidElement<TElementData>
{
public:

    /// Counted pointer of
    KRATOS_CLASS_POINTER_DEFINITION(TwoFluidNavierStokes);

    ///@name Type Definitions
    ///@{
    /// Node type (default is: Node<3>)
    typedef Node<3> NodeType;

    /// Geometry type (using with given NodeType)
    typedef Geometry<NodeType> GeometryType;

    /// Definition of nodes container type, redefined from GeometryType
    typedef Geometry<NodeType>::PointsArrayType NodesArrayType;

    /// Vector type for local contributions to the linear system
    typedef Vector VectorType;

    /// Matrix type for local contributions to the linear system
    typedef Matrix MatrixType;

    typedef std::size_t IndexType;

    typedef std::size_t SizeType;

    typedef std::vector<std::size_t> EquationIdVectorType;

    typedef std::vector< Dof<double>::Pointer > DofsVectorType;

    typedef PointerVectorSet<Dof<double>, IndexedObject> DofsArrayType;

    /// Type for shape function values container
    typedef typename FluidElement<TElementData>::ShapeFunctionsType ShapeFunctionsType;

    /// Type for a matrix containing the shape function gradients
    typedef typename FluidElement<TElementData>::ShapeFunctionDerivativesType ShapeFunctionDerivativesType;

    /// Type for an array of shape function gradient matrices
    typedef typename FluidElement<TElementData>::ShapeFunctionDerivativesArrayType ShapeFunctionDerivativesArrayType;

    constexpr static unsigned int Dim = FluidElement<TElementData>::Dim;
    constexpr static unsigned int NumNodes = FluidElement<TElementData>::NumNodes;
    constexpr static unsigned int BlockSize = FluidElement<TElementData>::BlockSize;
    constexpr static unsigned int LocalSize = FluidElement<TElementData>::LocalSize;

    constexpr static unsigned int StrainSize = (Dim - 1) * 3;


    ///@}
    ///@name Life Cycle
    ///@{

    //Constructors.

    /// Default constuctor.
    /**
    * @param NewId Index number of the new element (optional)
    */
    TwoFluidNavierStokes(IndexType NewId = 0);

    /// Constructor using an array of nodes.
    /**
    * @param NewId Index of the new element
    * @param ThisNodes An array containing the nodes of the new element
    */
    TwoFluidNavierStokes(IndexType NewId, const NodesArrayType& ThisNodes);

    /// Constructor using a geometry object.
    /**
    * @param NewId Index of the new element
    * @param pGeometry Pointer to a geometry object
    */
    TwoFluidNavierStokes(IndexType NewId, GeometryType::Pointer pGeometry);

    /// Constuctor using geometry and properties.
    /**
    * @param NewId Index of the new element
    * @param pGeometry Pointer to a geometry object
    * @param pProperties Pointer to the element's properties
    */
    TwoFluidNavierStokes(IndexType NewId, GeometryType::Pointer pGeometry, Properties::Pointer pProperties);

    /// Destructor.
    virtual ~TwoFluidNavierStokes();


    ///@}
    ///@name Operators
    ///@{


    ///@}
    ///@name Operations
    ///@{


    /// Create a new element of this type
    /**
    * Returns a pointer to a new TwoFluidNavierStokes element, created using given input.
    * @param NewId the ID of the new element
    * @param ThisNodes the nodes of the new element
    * @param pProperties the properties assigned to the new element
    * @return a Pointer to the new element
    */
    Element::Pointer Create(IndexType NewId,
        NodesArrayType const& ThisNodes,
        Properties::Pointer pProperties) const override;

    /// Create a new element of this type using given geometry
    /**
    * Returns a pointer to a new FluidElement element, created using given input.
    * @param NewId the ID of the new element
    * @param pGeom a pointer to the geomerty to be used to create the element
    * @param pProperties the properties assigned to the new element
    * @return a Pointer to the new element
    */
    Element::Pointer Create(IndexType NewId,
        GeometryType::Pointer pGeom,
        Properties::Pointer pProperties) const override;

    ///@}
    ///@name Inquiry
    ///@{

    int Check(const ProcessInfo &rCurrentProcessInfo) override;

    ///@}
    ///@name Input and output
    ///@{

    /// Turn back information as a string.
    std::string Info() const override;


    /// Print information about this object.
    void PrintInfo(std::ostream& rOStream) const override;

    ///@}
	void CalculateLocalSystem(MatrixType& rLeftHandSideMatrix,
		VectorType& rRightHandSideVector,
		ProcessInfo& rCurrentProcessInfo) override;

	void CalculateRightHandSide(VectorType& rRightHandSideVector,
		ProcessInfo& rCurrentProcessInfo) override;


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

protected:

    ///@}
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
    void AddTimeIntegratedSystem(
        TElementData& rData,
        MatrixType& rLHS,
        VectorType& rRHS) override;

    void AddTimeIntegratedLHS(
        TElementData& rData,
        MatrixType& rLHS) override;

    void AddTimeIntegratedRHS(
        TElementData& rData,
        VectorType& rRHS) override;

    void ComputeGaussPointLHSContribution(
        TElementData& rData,
        MatrixType& rLHS);

    void ComputeGaussPointRHSContribution(
        TElementData& rData,
        VectorType& rRHS);

	void ComputeGaussPointEnrichmentContributions(
		TElementData& rData,
		MatrixType& rV,
		MatrixType& rH,
		MatrixType& rKee,
		VectorType& rRHS_ee);



private:

	unsigned int ComputeSplitting(
		TElementData& data,
		MatrixType& shape_functions,
		ShapeFunctionDerivativesArrayType& shape_derivatives,
		std::vector<MatrixType>& DNenr,
		MatrixType& Nner);

	void CondenseEnrichment(
		TElementData& data,
		Matrix& rLeftHandSideMatrix,
		VectorType& rRightHandSideVector,
		MatrixType& V,
		MatrixType& H,
		MatrixType& K_ee,
		VectorType& rhs_ee);

	void CalculateMaterialPropertiesAtGaussPoint(TElementData& data);

	template<class T>
	bool InvertMatrix(const T& input, T& inverse);


    

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
///@name Static Member Variables
///@{

///@}
///@name Member Variables
///@{

///@}
///@name Serialization
///@{
    friend class Serializer;

    void save(Serializer& rSerializer) const override;

    void load(Serializer& rSerializer) override;

///@}

///@name Private Operations
///@{
    //virtual void ComputeConstitutiveResponse_AIR(element_data<4,3>& data, const double rho, const double nu,  ProcessInfo& rCurrentProcessInfo)
    //{
    //    //const unsigned int nnodes = 4;
    //    //const unsigned int dim = 3;
    //    const unsigned int strain_size = 6;

    //    if(data.C.size1() != strain_size)
    //        data.C.resize(strain_size,strain_size,false);
    //    if(data.stress.size() != strain_size)
    //        data.stress.resize(strain_size,false);

    //    //compute strain
    //    Vector strain(strain_size);
    //    ComputeStrain(data, strain_size, strain);

    //    //here we shall call the constitutive law
    //    data.C.clear();
    //    data.C(0,0) = 2.0*nu;
    //    data.C(1,1) = 2.0*nu;
    //    data.C(2,2) = 2.0*nu;
    //    data.C(3,3) = nu;
    //    data.C(4,4) = nu;
    //    data.C(5,5) = nu;

    //    const double c2 = nu;
    //    const double c1 = 2.0*c2;
    //    data.stress[0] =  c1*strain[0];
    //    data.stress[1] =  c1*strain[1];
    //    data.stress[2] =  c1*strain[2];
    //    data.stress[3] =  c2*strain[3];
    //    data.stress[4] =  c2*strain[4];
    //    data.stress[5] =  c2*strain[5];
    //}


///@}
///@name Private  Access
///@{


///@}
///@name Private Inquiry
///@{


///@}
///@name Un accessible methods
///@{
/// Assignment operator.
    TwoFluidNavierStokes& operator=(TwoFluidNavierStokes const& rOther);

    /// Copy constructor.
    TwoFluidNavierStokes(TwoFluidNavierStokes const& rOther);




///@}

}; // Class TwoFluidNavierStokes

///@}

///@name Type Definitions
///@{


///@}
///@name Input and output
///@{


/// input stream function
template< class TElementData >
inline std::istream& operator >> (std::istream& rIStream,
    TwoFluidNavierStokes<TElementData>& rThis)
{
    return rIStream;
}

/// output stream function
template< class TElementData >
inline std::ostream& operator <<(std::ostream& rOStream,
    const TwoFluidNavierStokes<TElementData>& rThis)
{
    rThis.PrintInfo(rOStream);
    rOStream << std::endl;
    rThis.PrintData(rOStream);

    return rOStream;
}
///@}

} // namespace Kratos.

#endif // KRATOS_TWO_FLUID_NAVIER_STOKES



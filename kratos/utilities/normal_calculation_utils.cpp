//    |  /           |
//    ' /   __| _` | __|  _ \   __|
//    . \  |   (   | |   (   |\__ `
//   _|\_\_|  \__,_|\__|\___/ ____/
//                   Multi-Physics
//
//  License:		 BSD License
//					 Kratos default license: kratos/license.txt
//
//  Main authors:    Pooyan Dadvand
//                   Riccardo Rossi
//                   Vicente Mataix Ferrandiz
//
//

// System includes

// External includes

// Project includes
#include "utilities/normal_calculation_utils.h"

namespace Kratos
{
template<>
void NormalCalculationUtils::InitializeNormals<Condition>(ModelPart& rModelPart)
{
    // Resetting the normals
    const array_1d<double,3> zero = ZeroVector(3);

    if (rModelPart.GetCommunicator().GetDataCommunicator().IsDistributed()) {
        // If Parallel make sure normals are reset in all partitions
        VariableUtils().SetFlag(VISITED, false, rModelPart.Nodes());

        for(auto& r_cond : rModelPart.Conditions()) {
            for(auto& r_node: r_cond.GetGeometry()) {
                r_node.Set(VISITED, true);
            }
        }

        rModelPart.GetCommunicator().SynchronizeOrNodalFlags(VISITED);

        VariableUtils().SetVariable(NORMAL, zero, rModelPart.Nodes(), VISITED);
    } else {
        // In serial iteratre normally over the condition nodes
        for(auto& r_cond: rModelPart.Conditions()) {
            for(auto& r_node: r_cond.GetGeometry()) {
                noalias(r_node.FastGetSolutionStepValue(NORMAL)) = zero;
            }
        }
    }
}

/***********************************************************************************/
/***********************************************************************************/

template<>
void NormalCalculationUtils::InitializeNormals<Element>(ModelPart& rModelPart)
{
    // Resetting the normals
    const array_1d<double,3> zero = ZeroVector(3);

    if (rModelPart.GetCommunicator().GetDataCommunicator().IsDistributed()) {
        // If Parallel make sure normals are reset in all partitions
        VariableUtils().SetFlag(VISITED, false, rModelPart.Nodes());

        for(auto& r_cond : rModelPart.Elements()) {
            for(auto& r_node: r_cond.GetGeometry()) {
                r_node.Set(VISITED, true);
            }
        }

        rModelPart.GetCommunicator().SynchronizeOrNodalFlags(VISITED);

        VariableUtils().SetVariable(NORMAL, zero, rModelPart.Nodes(), VISITED);
    } else {
        // In serial iteratre normally over the element nodes
        for(auto& r_cond: rModelPart.Elements()) {
            for(auto& r_node: r_cond.GetGeometry()) {
                noalias(r_node.FastGetSolutionStepValue(NORMAL)) = zero;
            }
        }
    }
}

/***********************************************************************************/
/***********************************************************************************/

template<>
void NormalCalculationUtils::CalculateNormals<Condition>(ModelPart& rModelPart)
{
    // Initialize the normals
    InitializeNormals<Condition>(rModelPart);
}

/***********************************************************************************/
/***********************************************************************************/

template<>
void NormalCalculationUtils::CalculateNormals<Element>(ModelPart& rModelPart)
{
    // Initialize the normals
    InitializeNormals<Element>(rModelPart);
}

/***********************************************************************************/
/***********************************************************************************/

template<>
void NormalCalculationUtils::CalculateUnitNormals<Condition>(ModelPart& rModelPart)
{
    // Compute area normals
    CalculateNormals<Condition>(rModelPart);

    // Compute unit normals
    ComputeUnitNormalsFromAreaNormals(rModelPart);
}

/***********************************************************************************/
/***********************************************************************************/

template<>
void NormalCalculationUtils::CalculateUnitNormals<Element>(ModelPart& rModelPart)
{
    // Compute area normals
    CalculateNormals<Element>(rModelPart);

    // Compute unit normals
    ComputeUnitNormalsFromAreaNormals(rModelPart);
}

/***********************************************************************************/
/***********************************************************************************/

void NormalCalculationUtils::CalculateOnSimplex(
    ConditionsArrayType& rConditions,
    const std::size_t Dimension
    )
{
    KRATOS_TRY

    // Calculating the normals and storing on the conditions
    array_1d<double,3> An;
    if(Dimension == 2)  {
        for(ConditionsArrayType::iterator it =  rConditions.begin(); it !=rConditions.end(); it++) {
            if (it->GetGeometry().PointsNumber() == 2)
                CalculateNormal2D(it,An);
        }
    } else if(Dimension == 3) {
        array_1d<double,3> v1, v2;
        for(ConditionsArrayType::iterator it =  rConditions.begin(); it !=rConditions.end(); it++) {
            // Calculate the normal on the given condition
            if (it->GetGeometry().PointsNumber() == 3)
                CalculateNormal3D(it,An,v1,v2);
        }
    }

    // Adding the normals to the nodes
    for(ConditionsArrayType::iterator it =  rConditions.begin(); it !=rConditions.end(); it++) {
        Geometry<Node<3> >& pGeometry = (it)->GetGeometry();
        double coeff = 1.0/pGeometry.size();
        const array_1d<double,3>& r_normal = it->GetValue(NORMAL);
        for(unsigned int i = 0; i<pGeometry.size(); i++) {
            noalias(pGeometry[i].FastGetSolutionStepValue(NORMAL)) += coeff * r_normal;
        }
    }


    KRATOS_CATCH("")

}

/***********************************************************************************/
/***********************************************************************************/

void NormalCalculationUtils::CalculateOnSimplex(
    ModelPart& rModelPart,
    const std::size_t Dimension
    )
{
    // Initialize the normals
    InitializeNormals<Condition>(rModelPart);

    // Calling CalculateOnSimplex for conditions
    const auto& r_process_info = rModelPart.GetProcessInfo();
    const bool has_domain_size = r_process_info.Has(DOMAIN_SIZE);
    KRATOS_ERROR_IF(has_domain_size && Dimension == 0) << "Dimension not defined" << std::endl;
    const SizeType dimension_in_model_part = has_domain_size ? r_process_info.GetValue(DOMAIN_SIZE) : Dimension;
    KRATOS_WARNING_IF("NormalCalculationUtils", dimension_in_model_part != Dimension) << "Inconsistency between DOMAIN_SIZE and Dimension provided" << std::endl;
    this->CalculateOnSimplex(rModelPart.Conditions(), dimension_in_model_part);

    // Synchronize the normal
    rModelPart.GetCommunicator().AssembleCurrentData(NORMAL);
}

/***********************************************************************************/
/***********************************************************************************/

void NormalCalculationUtils::SwapNormals(ModelPart& rModelPart)
{
    KRATOS_TRY

    for(auto& r_cond : rModelPart.Conditions()) {
        GeometryType& r_geometry = r_cond.GetGeometry();
        Node<3>::Pointer paux = r_geometry(0);
        r_geometry(0) = r_geometry(1);
        r_geometry(1) = paux;
    }

    KRATOS_CATCH("")
}

/***********************************************************************************/
/***********************************************************************************/

void NormalCalculationUtils::ComputeUnitNormalsFromAreaNormals(ModelPart& rModelPart)
{
    // We iterate over nodes
    auto& r_nodes_array = rModelPart.Nodes();
    const auto it_node_begin = r_nodes_array.begin();
    const int num_nodes = static_cast<int>(r_nodes_array.size());

    #pragma omp parallel for
    for (int i = 0; i < num_nodes; ++i) {
        auto it_node = it_node_begin + i;

        array_1d<double, 3>& r_normal = it_node->FastGetSolutionStepValue(NORMAL);
        const double norm_normal = norm_2(r_normal);

        if (norm_normal > std::numeric_limits<double>::epsilon()) r_normal /= norm_normal;
        else KRATOS_ERROR_IF(it_node->Is(INTERFACE)) << "ERROR:: ZERO NORM NORMAL IN NODE: " << it_node->Id() << std::endl;
    }

    // For MPI: correct values on partition boundaries
    rModelPart.GetCommunicator().AssembleCurrentData(NORMAL);
}

/***********************************************************************************/
/***********************************************************************************/

void NormalCalculationUtils::CalculateNormal2D(
    ConditionsArrayType::iterator itCond,
    array_1d<double,3>& rAn
    )
{
    const GeometryType& r_geometry = itCond->GetGeometry();

    rAn[0] =    r_geometry[1].Y() - r_geometry[0].Y();
    rAn[1] = - (r_geometry[1].X() - r_geometry[0].X());
    rAn[2] =    0.00;

    array_1d<double,3>& r_normal = itCond->GetValue(NORMAL);
    noalias(r_normal) = rAn;
}

/***********************************************************************************/
/***********************************************************************************/

void NormalCalculationUtils::CalculateNormal3D(
    ConditionsArrayType::iterator itCond,
    array_1d<double,3>& rAn,
    array_1d<double,3>& rv1,
    array_1d<double,3>& rv2
    )
{
    const GeometryType& r_geometry = itCond->GetGeometry();

    rv1[0] = r_geometry[1].X() - r_geometry[0].X();
    rv1[1] = r_geometry[1].Y() - r_geometry[0].Y();
    rv1[2] = r_geometry[1].Z() - r_geometry[0].Z();

    rv2[0] = r_geometry[2].X() - r_geometry[0].X();
    rv2[1] = r_geometry[2].Y() - r_geometry[0].Y();
    rv2[2] = r_geometry[2].Z() - r_geometry[0].Z();

    MathUtils<double>::CrossProduct(rAn,rv1,rv2);
    rAn *= 0.5;

    array_1d<double,3>& r_normal = itCond->GetValue(NORMAL);
    noalias(r_normal) = rAn;
}

} // namespace Kratos

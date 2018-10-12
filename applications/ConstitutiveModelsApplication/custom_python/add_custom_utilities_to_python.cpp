//
//   Project Name:        KratosConstitutiveModelsApplication $
//   Created by:          $Author:                JMCarbonell $
//   Last modified by:    $Co-Author:                         $
//   Date:                $Date:                 October 2018 $
//   Revision:            $Revision:                      0.0 $
//
//

// System includes

// External includes

// Project includes
#include "custom_python/add_custom_utilities_to_python.h"

// Utilities
#include "custom_utilities/properties_layout.hpp"

namespace Kratos
{

namespace Python
{

void  AddCustomUtilitiesToPython(pybind11::module& m)
{

  using namespace pybind11;
  class_<PropertiesLayout, PropertiesLayout::Pointer >(m,"PropertiesLayout")
      .def(init<>())
      .def(init<>(Properties&))
      .def("Clone", &PropertiesLayout::Clone)
      .def("__repr__", &PropertiesLayout::Info )
      DECLARE_ADD_THIS_TYPE_TO_PROPERTIES_PYTHON_AS_POINTER(PropertiesLayout)
      DECLARE_GET_THIS_TYPE_FROM_PROPERTIES_PYTHON_AS_POINTER(PropertiesLayout)
      ;

  class_<Variable<PropertiesLayout::Pointer>, VariableData>(m,"PropertiesLayoutVariable")
      ;


}

}  // namespace Python.

} // Namespace Kratos


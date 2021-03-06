#include <pybind11/pybind11.h>
#include <pybind11/detail/init.h>

#include "Binding_Base.h"
#include "Binding_PythonController.h"
#include "DataHelper.h"

#include <sofa/core/objectmodel/Event.h>
using sofa::core::objectmodel::Event;

PYBIND11_DECLARE_HOLDER_TYPE(PythonController,
                             sofapython3::py_shared_ptr<PythonController>, true)

namespace sofapython3
{

    void PythonController::init() {
    }

    void PythonController::reinit() {
    }

    class PythonController_Trampoline : public PythonController, public PythonTrampoline
    {    
    public:
        PythonController_Trampoline()
        {
        }

        virtual ~PythonController_Trampoline()
        {
        }

        virtual std::string getClassName() const override
        {
            return pyobject->ob_type->tp_name;
        }

        virtual void init() override ;
        virtual void reinit() override ;
        virtual void handleEvent(Event* event) override ;
    };

    void PythonController_Trampoline::init()
    {
        PYBIND11_OVERLOAD(void, PythonController, init, );
    }

    void PythonController_Trampoline::reinit()
    {
        PYBIND11_OVERLOAD(void, PythonController, reinit, );
    }

    void PythonController_Trampoline::handleEvent(Event* event)
    {
        py::object self = py::cast(this);
        std::string name = std::string("on")+event->getClassName();
        if( py::hasattr(self, name.c_str()) )
        {
            py::object fct = self.attr(name.c_str());
            py::object ret = fct("Hello");
        }
    }

    void moduleAddPythonController(py::module &m) {
        py::class_<PythonController, BaseObject,
                PythonController_Trampoline,
                py_shared_ptr<PythonController>> f(m, "PythonController");

        f.def(py::init([](py::args& args, py::kwargs& kwargs)
        {
                  PythonController_Trampoline* c = new PythonController_Trampoline();
                  c->f_listening.setValue(true);

                  if(args.size() != 0)
                  {
                      if(args.size()==1) c->setName(py::cast<std::string>(args[0]));
                      else throw py::type_error("Only one un-named arguments can be provided.");
                  }

                  for(auto& kv : kwargs)
                  {
                      std::string key = py::cast<std::string>(kv.first);
                      py::object value = py::object(kv.second, true);
                      if( key == "name")
                      {
                          if( args.size() != 0 )
                          {
                              throw py::type_error("The name is setted twice as a "
                                                   "named argument='"+py::cast<std::string>(value)+"' and as a"
                                                   "positional argument='"+py::cast<std::string>(args[0])+"'.");
                          }
                      }
                      BindingBase::SetAttr(*c, key, value);
                  }

                  return c;
              }));

        f.def("init", &PythonController::init);
        f.def("reinit", &PythonController::reinit);
    }


}

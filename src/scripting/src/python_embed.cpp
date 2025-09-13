
#include <avs/scripting/python_embed.h>
#ifdef WITH_PYTHON
#include <Python.h>
#endif
namespace avs { namespace scripting {
bool Python::init(const PyConfig& cfg){
#ifdef WITH_PYTHON
    Py_SetProgramName(Py_DecodeLocale(cfg.program_name.c_str(), nullptr));
    Py_Initialize();
    return Py_IsInitialized();
#else
    (void)cfg; return false;
#endif
}
bool Python::run(const std::string& code){
#ifdef WITH_PYTHON
    if(!Py_IsInitialized()) return false;
    return PyRun_SimpleString(code.c_str())==0;
#else
    (void)code; return false;
#endif
}
void Python::shutdown(){
#ifdef WITH_PYTHON
    if(Py_IsInitialized()) Py_Finalize();
#endif
}
}} // namespace

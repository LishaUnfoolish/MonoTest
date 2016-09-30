#include "MonoTest.h"

int main(int argc, char *argv[])
{
    argc;argv;
    // Init domain
    MonoDomain* domain;
    domain = mono_jit_init("MonoTest Main Domain");

    // Load assembly
    MonoAssembly* assembly;
    assembly = mono_domain_assembly_open(domain, "MonoGlue.dll");
    if (!assembly)
    {
        std::cout << "Couldn't create assembly" << std::endl;
        return -3;
    }
    std::cout << "Created assembly" << std::endl;

    // Create the image of the assembly
    MonoImage* monoImage = mono_assembly_get_image(assembly);
    if (!monoImage)
    {
        std::cout << "Couldn't create assembly image" << std::endl;
        return -3;
    }
    std::cout << "Created assembly image" << std::endl;

    // Create the class from the image
    MonoClass* entityClass = mono_class_from_name(monoImage,
                                                  "MonoGlue",
                                                  "ATestClass");
    if (!entityClass)
    {
        std::cout << "Couldn't create ATestClass" << std::endl;
        return -3;
    }
    std::cout << "Created ATestClass" << std::endl;

    // Get the constructor method
    MonoMethod* constructorMethod = mono_class_get_method_from_name(entityClass,
                                                                    ".ctor",
                                                                    0);
    if (!constructorMethod)
    {
        std::cout << "Couldn't get constructor" << std::endl;
        return -3;
    }
    std::cout << "Got constructor" << std::endl;
    // Allocate space for the object
    MonoObject* entityInstance = mono_object_new(domain, entityClass);
    if (!entityInstance)
    {
        std::cout << "Couldn't allocate instance space" << std::endl;
        return -3;
    }
    std::cout << "Allocated instance space" << std::endl;
    // Invoke constructor
    MonoObject* exception = NULL;
    MonoObject* resultObject = mono_runtime_invoke(constructorMethod, entityInstance, NULL, &exception);
    if (exception)
    {
        std::cout << "Constructor call exception:\n" << monoExceptionToString(exception) << std::endl;
        return -3;
    }
    std::cout << "Constructor called" << std::endl;

    // Get method
    MonoMethod* multiplyMethod = mono_class_get_method_from_name(   entityClass,
                                                                    "multiply",
                                                                    2);
    if (!multiplyMethod)
    {
        std::cout << "Couldn't get multiply method" << std::endl;
        return -3;
    }
    std::cout << "Got multiply method" << std::endl;
    // Create two numbers as arguments
    void* args[2];
    int arg1 = 5;
    int arg2 = 7;
    args[0] = &arg1;
    args[1] = &arg2;
    // Execute method
    exception = NULL;
    resultObject = mono_runtime_invoke(multiplyMethod, entityInstance, args, &exception);
    if (exception)
    {
        std::cout << "multiply call exception:\n" << monoExceptionToString(exception) << std::endl;
        return -3;
    }
    int int_result = *(int*)mono_object_unbox(resultObject);
    std::cout << "Multiply called. Result is: " << int_result << std::endl;


    // Execute a C function using MethodImplAttribute
    // Register methods BEFORE functions needing them are called.
    methodImplTest();
    mono_add_internal_call ("MonoGlue.ATestClass::methodImplTest", (const void*)methodImplTest);
    // Register function
    // Execute a C function using DllImport
    // No registration required - this requires _declspec(dllexport) on Windows and does NOT allow static C functions
    std::cout << dllImportTest() << std::endl;
    // Get test method
    MonoMethod* testMethod = mono_class_get_method_from_name(   entityClass,
                                                                "testCFunctions",
                                                                0);
    if (!testMethod)
    {
        std::cout << "Couldn't get test method" << std::endl;
        return -3;
    }
    std::cout << "Got test method" << std::endl;
    // Execute it
    exception = NULL;
    resultObject = mono_runtime_invoke(testMethod, entityInstance, NULL, &exception);
    if (exception)
    {
        std::cout << mono_string_to_utf8(mono_object_to_string(exception,NULL)) << std::endl;
        return -3;
    }
    std::cout << "test method called." << std::endl;

    // Execute a function pointer returned from C# and vice versa
    mono_add_internal_call ("MonoGlue.ATestClass::setCallback", (const void*)setCallback);
    // Get test method
    MonoMethod* callbacksMethod = mono_class_get_method_from_name(  entityClass,
                                                                    "testCallbacks",
                                                                    1);
    if (!callbacksMethod)
    {
        std::cout << "Couldn't get callbacks method" << std::endl;
        return -3;
    }
    std::cout << "Got callbacks method" << std::endl;
    // Get C++-function as an argument
    // TODO: here
    // Execute it
    exception = NULL;
    resultObject = mono_runtime_invoke(callbacksMethod, entityInstance, NULL, &exception);
    if (exception)
    {
        std::cout << mono_string_to_utf8(mono_object_to_string(exception,NULL)) << std::endl;
        return -3;
    }
    std::cout << "Callbacks method called." << std::endl;

    return 0;
}

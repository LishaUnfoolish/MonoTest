#include <iostream>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

const char* getStringProperty(const char* propertyName, MonoClass* classType, MonoObject* classObject)
{
    MonoProperty *messageProperty;
    MonoMethod *messageGetter;
    MonoString *messageString;

    messageProperty = mono_class_get_property_from_name(classType, propertyName);
    messageGetter = mono_property_get_get_method(messageProperty);
    messageString = (MonoString *)mono_runtime_invoke(messageGetter, classObject, NULL, NULL);
    return mono_string_to_utf8(messageString);
}

std::string monoExceptionToString(MonoObject* exception)
{
    if (exception)
    {
        MonoClass *exceptionClass;
        MonoType *exceptionType;
        const char *typeName, *message, *source, *stackTrace;

        exceptionClass = mono_object_get_class(exception);
        exceptionType = mono_class_get_type(exceptionClass);
        typeName = mono_type_get_name(exceptionType);
        message = getStringProperty("Message", exceptionClass, exception);
        source = getStringProperty("Source", exceptionClass, exception);
        stackTrace = getStringProperty("StackTrace", exceptionClass, exception);

        std::string error = "Message: ";
        error.append(message);
        error.append("\nSource: ");
        error.append(source);
        error.append("\nStackTrace: ");
        error.append(stackTrace);
        return error;
    }

    return "Null exception object passed";
}

int main(int argc, char *argv[])
{
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
    MonoObject* result = mono_runtime_invoke(multiplyMethod, entityInstance, args, &exception);
    if (exception)
    {
        std::cout << "multiply call exception:\n" << monoExceptionToString(exception) << std::endl;
        return -3;
    }
    int int_result = *(int*)mono_object_unbox(result);
    std::cout << "Multiply called. Result is: " << int_result << std::endl;

    return 0;
}

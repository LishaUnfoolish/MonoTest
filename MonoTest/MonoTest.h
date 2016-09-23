#ifndef MONOTEST_H
#define MONOTEST_H
#include <iostream>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

#ifdef _WIN32_
#    define EXPORT __declspec(dllexport)
#else
#    define EXPORT __attribute__((noinline))
#endif

const char* getStringProperty(const char* propertyName, MonoClass* classType, MonoObject* classObject)
{
    MonoProperty *messageProperty;
    MonoMethod *messageGetter;
    MonoString *messageString;

    messageProperty = mono_class_get_property_from_name(classType, propertyName);
    messageGetter = mono_property_get_get_method(messageProperty);
    messageString = (MonoString *)mono_runtime_invoke(messageGetter, classObject, NULL, NULL);
    if (!messageString) {
        return "";
    }
    return mono_string_to_utf8(messageString);
}

std::string monoExceptionToString(MonoObject* exception)
{
    if (exception)
    {
        MonoClass* exceptionClass;
        MonoType* exceptionType;
        const char* typeName;
        std::string message, source, stackTrace;

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

extern "C"
{

    // Example for calling a C function via dllImport
    // Note: Only global namespace and no static functions
    int EXPORT dllImportTest()
    {
        return 66;
    }

    // Example for calling a C function via MethodImpl
    static MonoString* methodImplTest()
    {
      return mono_string_new(mono_domain_get (), "methodImplTest from C++\n");
    }
}

#endif // MONOTEST_H

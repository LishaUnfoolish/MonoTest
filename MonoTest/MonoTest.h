#ifndef MONOTEST_H
#define MONOTEST_H
#include <iostream>

#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>
#include <mono/metadata/appdomain.h>
#include <mono/metadata/attrdefs.h>
#include <mono/metadata/blob.h>
#include <mono/metadata/class.h>
#include <mono/metadata/debug-helpers.h>
#include <mono/metadata/environment.h>
#include <mono/metadata/exception.h>
#include <mono/metadata/image.h>
#include <mono/metadata/loader.h>
#include <mono/metadata/metadata.h>
#include <mono/metadata/mono-config.h>
#include <mono/metadata/mono-debug.h>
#include <mono/metadata/mono-gc.h>
#include <mono/metadata/object.h>
#include <mono/metadata/opcodes.h>
#include <mono/metadata/profiler.h>
#include <mono/metadata/reflection.h>
#include <mono/metadata/row-indexes.h>
#include <mono/metadata/sgen-bridge.h>
#include <mono/metadata/threads.h>
#include <mono/metadata/tokentype.h>

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
}

// Example for calling a C function via MethodImpl
// Note: CAN be static, but doesn't have to be
//       Also does not have to be "extern "C""
static MonoString* methodImplTest()
{
  return mono_string_new(mono_domain_get (), "methodImplTest from C++\n");
}

// Callback from C# can be received already in the "correct format" as a function pointer
typedef void (*CallbackFunction)(MonoString*);
void setCallback(CallbackFunction delegate)
{
    std::cout << &delegate << std::endl;
    delegate(mono_string_new(mono_domain_get(), "Test string set in C++"));
}

#endif // MONOTEST_H

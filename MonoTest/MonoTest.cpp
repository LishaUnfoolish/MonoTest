// MonoTest.cpp : main project file.

#include "stdafx.h"
#include <mono/jit/jit.h>
#include <mono/metadata/assembly.h>

using namespace System;

int main(array<System::String ^> ^args)
{
    Console::WriteLine(L"Init Mono and load assembly...");

	// Init domain
	MonoDomain* domain;
	domain = mono_jit_init("MonoTest Main Domain");

	// Load assembly
	MonoAssembly* assembly;
	assembly = mono_domain_assembly_open(domain, "MonoGlue.dll");
	if (!assembly)
		error();

    return 0;
}

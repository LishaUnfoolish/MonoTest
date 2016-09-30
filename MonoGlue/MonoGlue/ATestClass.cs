using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Runtime.CompilerServices;

namespace MonoGlue
{
    public class ATestClass
    {
        public ATestClass()
        {
            System.Console.Write("C#: ATestClass was created\n");
        }

        public int multiply(int a, int b)
        {
            return a * b;
        }

		[DllImport("__Internal")]
		public static extern int dllImportTest();

		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		extern public string methodImplTest();

		public void testCFunctions()
		{
			System.Console.Write("Getting C functions...\n");
			int dllImport = dllImportTest();
			System.Console.Write("Got dllImport: " + dllImport + "\n");
			string methodImpl = methodImplTest();
			System.Console.Write("Got methodImpl: " + methodImpl + "\n");
		}


		public delegate void CallbackDelegate(string message);
		[MethodImplAttribute(MethodImplOptions.InternalCall)]
		public static extern void setCallback(IntPtr aCallback);

		public string name = "test";
		private CallbackDelegate del; 
		public void testCallbacks()
		{
			System.Console.Write("Registering C# callback...\n");
			name = "test22";
			del = new CallbackDelegate(callback01);
			setCallback(Marshal.GetFunctionPointerForDelegate(del));

			System.Console.Write("Calling passed C++ callback...\n");
		}

		public void callback01(string message)
		{
			System.Console.Write("callback 01 called. Message: " + message + " Printed from: " + name + "\n");
		}
    }
}

using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

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
    }
}

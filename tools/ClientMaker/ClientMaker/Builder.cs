using Mono.Cecil;
using Mono.Cecil.Cil;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace ClientMaker
{
    class Builder
    {
        Data _data;
        public Builder(Data data)
        {
            _data = data;
        }
        public static bool ChangeField<T>(TypeDefinition t, string name, T value)
        {
            var field = t.Fields.SingleOrDefault(c => c.Name == name);
            if (field == null)
                return false;

            var cctor = t.Methods.SingleOrDefault(m => m.Name == ".cctor");

            if (cctor == null)
            {
                Console.WriteLine("no field initialization... in {0}", t.FullName);
                return false;
            }

            var store = cctor.Body.Instructions.SingleOrDefault(i => i.OpCode == OpCodes.Stsfld && i.Operand == field);

            if (store.Previous.Operand == null)
            {
                Console.WriteLine($"[WARNING] The value of {name} is null");
            }
            else
            {
                if (store.Previous.Operand.GetType() != typeof(T))
                {
                    Console.WriteLine($"[ERROR]   The type of {name} must be {store.Previous.Operand.GetType().ToString()} but {typeof(T).ToString()} was passed");
                    return false;
                }
            }
            Console.WriteLine("[INFO]    {0} Changed from '{1}' to '{2}'", name, 
                store.Previous.Operand ?? "null", value.ToString());

            store.Previous.Operand = Convert.ChangeType(value, typeof(T));
            return true;
        }

        public bool Modify()
        {
            using (var a = AssemblyDefinition.ReadAssembly(_data.Input))
            {
                var dataClass = a.MainModule.GetType("Client.Data");
                ChangeField<int>(dataClass, "NeuronPort", _data.Port.Value);
                ChangeField<int>(dataClass, "WebServerPort", _data.WebServerPort.Value);
                ChangeField<string>(dataClass, "Server", _data.ServerAddress);
                //ChangeField<string>(dataClass, "Arch", "x86");
                ChangeField<int>(dataClass, "Ready", 1);
                ChangeField<int>(dataClass, "CreateIniFile", 1);

                a.Write(_data.Output);
            }
            return true;
            try
            {
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.Message);
                return false;
            }
        }

    }
}

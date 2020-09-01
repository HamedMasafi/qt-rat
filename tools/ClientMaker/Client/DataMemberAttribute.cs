using System;

namespace Client
{
    internal class DataMemberAttribute : Attribute
    {
        public DataMemberAttribute(string name)
        {
            Name = name;
        }
        public string Name { get; internal set; }
    }
}
﻿using Microsoft.CodeAnalysis.CSharp;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using System.Reflection;

namespace EduEngine
{
    internal static class ScriptParser
    {
        public static Type FindComponent(string scriptPath)
        {
            string fileContent = File.ReadAllText(scriptPath);
            var syntaxTree = CSharpSyntaxTree.ParseText(fileContent);
            var root = syntaxTree.GetRoot();

            var classDeclarations = root
               .DescendantNodes()
               .OfType<ClassDeclarationSyntax>();

            var scriptAssembly = Assembly.LoadFrom(AssetDataBase.DllPath);

            foreach (var classDeclaration in classDeclarations)
            {
                var className = classDeclaration.Identifier.Text;

                var namespaceDeclaration = classDeclaration
                    .Ancestors()
                    .OfType<NamespaceDeclarationSyntax>()
                    .FirstOrDefault();

                var namespaceName = (namespaceDeclaration?.Name.ToString() + ".") ?? string.Empty;

                var type = scriptAssembly.GetType(namespaceName + className);

                if (type != null && type.IsSubclassOf(typeof(Component)))
                    return type;
            }

            return null;
        }

        public static string GetScriptGUID(Type scriptType)
        {
            var scripts = AssetDataBase.AllAssets.Where(asset => asset.Value.Type == AssetType.Script);

            foreach (var script in scripts)
            {
                var type = FindComponent(script.Value.GlobalPath);

                if (type == scriptType)
                    return script.Key;
            }

            return null;
        }
    }
}

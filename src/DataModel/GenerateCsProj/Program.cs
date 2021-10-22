using System;
using System.Collections.Generic;
using System.IO;

namespace GenerateCsProj
{
    class Program
    {
        const string SEARCH_KEYWORD = "$PROTOBUF_INCLUDES";

        const string TAG_START = "<Compile Include=\"";
        const string TAG_END = "\" />";

        static int Main(string[] args)
        {
            if ( args.Length < 3 )
            {
                System.Console.WriteLine("Usage: GenerateCsProj.exe <preProtoCsProjPath> <pathToCsOut> [listOfProtoFiles]");
                return 1;
            }

            var csProjFile = args[0];
            var csOutDirectory = args[1];
            var protos = new List<string>();

            var csProjFileAbsolutePath = System.IO.Path.GetFullPath(csProjFile);
            var pathToCsProj = csProjFileAbsolutePath.Substring(0, csProjFileAbsolutePath.LastIndexOf("\\"));
            if ( !System.IO.File.Exists( csProjFile ) )
            { 
                System.Console.WriteLine("Invalid File: " + csProjFileAbsolutePath);
                return 1;
            }

            var csOutAbsolutePath = System.IO.Path.GetFullPath(csOutDirectory);
            if ( !System.IO.Directory.Exists(csOutDirectory) )
            {
                System.Console.WriteLine("Invalid directory: " + csOutAbsolutePath);
                return 1;
            }

            var relativePathFromProjectFileToOutput = GetRelativePath(pathToCsProj, csOutAbsolutePath);

            for (int i = 2; i < args.Length; i++)
            {
                protos.Add(args[i]);
            }

            var originalFile = System.IO.File.ReadAllLines(csProjFile);
            var newFileName = csProjFile.Replace(".preProto", "");
            var newFile = new List<string>();

            var newFileAbsolutePath = System.IO.Path.GetFullPath(newFileName);
            System.Console.WriteLine("Generated project from " + protos.Count + " protobuf file(s): " + newFileAbsolutePath);

            foreach( var line in originalFile )
            {
                if ( line.Contains(SEARCH_KEYWORD) )
                {
                    var indent = line.Replace(SEARCH_KEYWORD, "");
                    foreach ( var proto in protos )
                    {
                        var newTag = indent + TAG_START + relativePathFromProjectFileToOutput + proto.Replace(".proto", ".cs") + TAG_END;
                        newFile.Add( newTag );
                    }
                }
                else
                {
                    newFile.Add(line);
                }
            }

            System.IO.File.WriteAllLines(newFileName, newFile);

            return 0;
        }

        public static string GetRelativePath(string fromPath, string toPath)
        {
            if (string.IsNullOrEmpty(fromPath))
            {
                throw new ArgumentNullException("fromPath");
            }

            if (string.IsNullOrEmpty(toPath))
            {
                throw new ArgumentNullException("toPath");
            }

            Uri fromUri = new Uri(AppendDirectorySeparatorChar(fromPath));
            Uri toUri = new Uri(AppendDirectorySeparatorChar(toPath));

            if (fromUri.Scheme != toUri.Scheme)
            {
                return toPath;
            }

            Uri relativeUri = fromUri.MakeRelativeUri(toUri);
            string relativePath = Uri.UnescapeDataString(relativeUri.ToString());

            if (string.Equals(toUri.Scheme, Uri.UriSchemeFile, StringComparison.OrdinalIgnoreCase))
            {
                relativePath = relativePath.Replace(Path.AltDirectorySeparatorChar, Path.DirectorySeparatorChar);
            }

            return relativePath;
        }

        private static string AppendDirectorySeparatorChar(string path)
        {
            // Append a slash only if the path is a directory and does not have a slash.
            if (!Path.HasExtension(path) &&
                !path.EndsWith(Path.DirectorySeparatorChar.ToString()))
            {
                return path + Path.DirectorySeparatorChar;
            }

            return path;
        }
    }
}

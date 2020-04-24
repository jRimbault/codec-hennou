using System;
using CommandLine;

namespace codech
{
    public class Options
    {
        public Options()
        {
            KeyFile = string.Empty;
            Source = string.Empty;
            Dest = string.Empty;
            Encode = false;
            Decode = false;
            Timings = false;
        }

        public Options(
            string keyFile,
            string source,
            string dest,
            bool encode,
            bool decode,
            bool timings)
        {
            KeyFile = keyFile;
            Source = source;
            Dest = dest;
            Encode = encode;
            Decode = decode;
            Timings = timings;
        }

        [Value(0, Required = true, HelpText = "File containing a G4C key")]
        public string KeyFile { get; private set; }

        [Value(1, Required = true, HelpText = "File to encode or decode")]
        public string Source { get; private set; }

        [Value(2, Required = true, HelpText = "Output file")]
        public string Dest { get; private set; }

        [Option('e', "encode",
            SetName = "action-encode",
            HelpText = "Action to execute on the source file"
        )]
        public bool Encode { get; private set; }

        [Option('d', "decode",
            SetName = "action-decode",
            HelpText = "Action to execute on the source file"
        )]
        public bool Decode { get; private set; }

        [Option('t', "timings",
            HelpText = "Output codec execution duration (seconds)"
        )]
        public bool Timings { get; private set; }

        public override string ToString()
        {
            string props = string.Join(
                $",{Environment.NewLine}   ",
                $"{nameof(KeyFile)}({KeyFile})",
                $"{nameof(Source)}({Source})",
                $"{nameof(Dest)}({Dest})",
                $"{nameof(Encode)}({Encode})",
                $"{nameof(Decode)}({Decode})",
                $"{nameof(Timings)}({Timings})"
            );
            return $"{nameof(Options)} {{{Environment.NewLine}   {props}{Environment.NewLine}}}";
        }
    }
}

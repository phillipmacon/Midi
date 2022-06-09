using System;
using System.CommandLine;
using System.CommandLine.Invocation;
using System.IO;

using System.Resources;

namespace Microsoft.Midi.Console
{
    class Program
    {

        // Rather than enums, it would be better to get these through reflection from the classes themselves.
        // Perhaps update this to use that rather than the enums. Need to support help at runtime, of course.
        //
        enum SettablePortProperties
        {
            Name,
            Description,
            Notes,

            AppHintWantsMidiClockMessages,
            AppHintWantsMidiStartStopMessages,
            AppHintWantsMidiTimecodeMessages,
            SupportsMpe,
            SupportsMultiClient
        };

        enum GettablePortProperties
        {
            Id,
            ParentDeviceId,

            Name,
            Description,
            Notes,

            OriginalPortName,
            WinRTPortName,
            DriverProviderName,
            DriverDate,
            DriverVersion,
            ManufacturerName,
            AppHintWantsMidiClockMessages,
            AppHintWantsMidiStartStopMessages,
            AppHintWantsMidiTimecodeMessages,
            SupportsMpe,
            SupportsMultiClient
        };


        enum GettableDeviceProperties
        {
            Id,

            Name,
            Description,
            Notes,

            DeviceSerialNumber,                /* USB Serial Number for USB devices */

            ReportedDescription,
            ReportedDisplayName,
            ReportedFriendlyName,
            DeviceManufacturerName,

            DriverProviderName,
            DriverDate,
            DriverVersion

        };

        enum SettableDeviceProperties
        {
            Name,
            Description,
            Notes
        };


        enum GettableSessionProperties
        {
            Name,
            Description,
            ApplicationName,
            ProcessName,
            ProcessId
        };
        enum SettableSessionProperties
        {
            Name,
            Description,
            ApplicationName
        };




        enum ListablePortTypes
        {
            Input,
            Output,
            Both
        };

        enum ListSortDirection
        {
            Ascending,
            Descending
        };

        enum ListFormat
        {
            Screen,
            Csv,
            TabDelimited,
            PipeDelimited,
        };



        static int Main(string[] args)
        {
            // root of all

            var rootCommand = new RootCommand
            {
                // todo: some commands to send MIDI clock, test perf, etc.

                BuildServicesCommand(),

                BuildListCommand(),

                BuildPanicCommand(),
                BuildDeviceCommand(),
                BuildInputPortCommand(),
                BuildOutputPortCommand(),

                BuildSessionCommand(),

                new Option<bool>("--silent", "Suppress all output."),
                new Option<bool>("--verbose", "Provide additional detail when supported.")
            };

            // TODO: Move to localized resource file
            var appDescription = "The MIDI Console enables interaction with the Windows MIDI Services including " +
                                 "the devices, ports, and sessions associated with it. The intent is to enable access to " +
                                 "everything which can be done through the settings app or an API. For additional " +
                                 "information, please see aka.ms/midi.";

            rootCommand.Description = appDescription;


            return rootCommand.InvokeAsync(args).Result;
        }







        static Command BuildServicesCommand()
        {
            var command = new Command("services", "Perform an operation, such as stopping, getting status, loading configuration, etc.  on the services.")
            {
                new Command("start", "Start the MIDI services if not already running.")
                {
                },
                new Command("pause", "Pause the MIDI services so that no messages are sent or received, but keep everything in memory.")
                {
                },
                new Command("stop", "Stop the MIDI services, losing all in-memory data and any configuration changes.")
                {
                },
                new Command("get", "Get information about the service using one of the sub commands.")
                {
                    new Command("status", "Get the current running status of the MIDI services.")
                    {
                        // This will return running status
                        // Running, Paused, Not Running

                        new Option<bool> (
                            "--return-error", "Return an error code to the comamnd line if the service is paused or stopped")

                    },
                    new Command("statistics", "Get statistics about the MIDI services.")
                    {
                        // Return information such as:
                        // - Time service has been running
                        // - Counts of messages sent/received since service started
                        // - Number of connected devices by type (USB, BLE, etc.)
                        // - Number of ports currently open
                        // - Which applications have which ports open
                        // etc
                    },
                },
                new Command("configuration", "Load or save a configuration file. Loading a new configuration file effectively stops and restarts the services, clearing all queues.")
                {
                    new Command("load")
                    {
                        new Option<FileInfo> (
                            "--file", "The configuration file to load")
                    },
                    new Command("save")
                    {
                        new Option<FileInfo> (
                            "--file", "The configuration file to save (default is current)")

                    }
                },

            };

            return command;
        }

        static Command BuildListCommand()
        {
            var command = new Command("list", "Get information about one or more ports or devices")
            {
                new Command("devices")
                {
                    new Option<GettableDeviceProperties> (
                        "--sort-by", "List devices in sorted order by the specified property."),
                    new Option<GettableDeviceProperties> (
                        "--filter-by", "Filter by this device property."),
                    new Option<string> (
                        "--filter-include-value", "Filter value to use include results. Wildcards accepted for string properties."),
                    new Option<string> (
                        "--filter-exclude-value", "Filter value to use to exclude results. Wildcards accepted for string properties."),

                },

                new Command("input-ports")
                {
                    new Option<GettablePortProperties> (
                        "--sort-by", "List ports in sorted order by the specified property."),
                    new Option<string> (
                        "--parent-device-id", "List ports for a specific device."),
                    new Option<GettablePortProperties> (
                        "--filter-by", "Filter by this port property."),
                    new Option<string> (
                        "--filter-include-value", "Filter value to use include results. Wildcards accepted for string properties."),
                    new Option<string> (
                        "--filter-exclude-value", "Filter value to use to exclude results. Wildcards accepted for string properties."),

                },

                new Command("output-ports")
                {
                    new Option<GettablePortProperties> (
                        "--sort-by", "List ports in sorted order by the specified property."),
                    new Option<string> (
                        "--parent-device-id", "List ports for a specific device."),
                    new Option<GettablePortProperties> (
                        "--filter-by", "Filter by this port property."),
                    new Option<string> (
                        "--filter-include-value", "Filter value to use include results. Wildcards accepted for string properties."),
                    new Option<string> (
                        "--filter-exclude-value", "Filter value to use to exclude results. Wildcards accepted for string properties."),
                },

                new Command("sessions")
                {
                    new Option<GettableSessionProperties> (
                        "--sort-by", "List sessions in sorted order by the specified property."),
                    new Option<GettableSessionProperties> (
                        "--filter-by", "Filter by this session property."),
                    new Option<string> (
                        "--filter-include-value", "Filter value to use include results. Wildcards accepted for string properties."),
                    new Option<string> (
                        "--filter-exclude-value", "Filter value to use to exclude results. Wildcards accepted for string properties."),
                },

                new Option<ListSortDirection> (
                    "--sort-direction", "Use the specified sort direction (Ascending or Descending)."),
                new Option<ListFormat> (
                    "--format", "Use the specified format for the output (default is screen-formatted output)."),
                new Option<bool> (
                    "--no-headers", "Do not include column headers."),
                new Option<bool> (
                    "--no-summary", "Do not include summary information."),
                new Option<bool> (
                    "--wide-format", "Uses wide list format, showing only names."),
            };

            return command;
        }

        static Command BuildPanicCommand()
        {
            var command = new Command("panic", "Send note off / panic information to an output port or device and also clear its queue.")
            {
                new Option<String> (
                    "--port-id", "Get the port by the unique Id. Use this to guarantee a single port match. This is the fastest approach, but note that the ID may change if the device is plugged into a different port and there's no USB serial."),
                new Option<GettablePortProperties> (
                    "--find-by-property", "Get the port by one of its properties. If more than one port matches, the first will be picked."),
                new Option<String> (
                    "--find-value", "The value to match for the port property. (Wildcards accepted)"),
            };

            return command;
        }


        static Command BuildOutputPortCommand()
        {
            // set port properties or send messages

            var command = new Command("output-port", "Perform an operation on an individual output port.")
            {
                    new Option<String> (
                        "--port-id", "Get the port by the unique Id. Use this to guarantee a single port match. This is the fastest approach, but note that the ID may change if the device is plugged into a different port and there's no USB serial."),
                    new Option<GettablePortProperties> (
                        "--find-by-property", "Get the port by one of its properties. If more than one port matches, the first will be picked."),
                    new Option<String> (
                        "--find-value", "The value to match for the port property. (Wildcards accepted)"),


                    new Command("set")
                    {
                        new Option<SettablePortProperties> (
                            "--property", "The name of the property to set"),
                        new Option<String> (
                            "--value", "The new value for the property")

                    },
                    new Command("send-midi1-message")
                    {
                        new Option<byte> (
                            "--channel", "Channel to send the message on"),
                        new Option<String> (
                            "--message-type", "The type of MIDI 1.0 message to send"),
                        new Option<byte> (
                            "--byte1", "Value of the first byte, if appropriate, for the message"), 
                        /* TODO: make this a single option that has 1-2 bytes of data. 
                         * Maybe --bytes or --data and hex (ex 0x0F) or decimal (ex 127) auto-parsed */
                        new Option<byte> (
                            "--byte2", "Value of the second byte, if appropriate, for the message")

                    },
                    new Command("send-midi2-message")
                    {
                        new Option<int> (
                            "--channel", "Channel to send the message on"),
                        new Option<String> (
                            "--group", "MIDI 2 group")

                        /* todo */
                    },
                    new Command("reset") /* alias as panic as well */
                    {

                    },
                    new Command("send-midi1-sysex-file")
                    {
                        new Option<int> (
                            "--channel", "The MIDI channel to send on (1-16)"),
                        new Option<FileInfo> (
                            "--file", "The SysEx file to send"),
                        new Option<int> (
                            "--delay", "The delay in between SysEx messages, in milliseconds")
                    },


            };

            command.Handler = CommandHandler.Create<string, string>((portId, newName) =>
            {
                Console.Write("Port id: {portId}");
                Console.WriteLine("New Name: {newName}");
            });

            return command;

        }


        static Command BuildInputPortCommand()
        {
            var command = new Command("input-port", "Perform an operation on an individual input port.")
            {
                    new Option<String> (
                        "--port-id", "Get the port by the unique Id. Use this to guarantee a single port match. This is the fastest approach, but note that the ID may change if the device is plugged into a different port and there's no USB serial."),
                    new Option<GettablePortProperties> (
                        "--find-by-property", "Get the port by one of its properties. If more than one port matches, the first will be picked."),
                    new Option<String> (
                        "--find-value", "The value to match for the port property. (Wildcards accepted)"),

                    new Command("set")
                    {
                        new Option<String> (
                            "--property", "The name of the property to set"), /* TODO: This should be an enum */
                        new Option<String> (
                            "--value", "The new value for the property")

                    },
                    new Command("peek") /* show messages in its queue */
                    {

                    },
                    new Command("reset") /* alias as panic as well */
                    {

                    },
                    new Command("receive-sysex-file")
                    {
                        new Option<FileInfo> (
                            "--file", "The SysEx file to receive. Defaults to a unique file name in the downloads folder."),

                    },

            };

            command.Handler = CommandHandler.Create<string, string>((portId, newName) =>
            {
                Console.Write("Port id: {portId}");
                Console.WriteLine("New Name: {newName}");
            });

            return command;

        }

        static Command BuildDeviceCommand()
        {
            // get / set device properties

            var command = new Command("device", "Perform an operation on an individual device")
            {
                    new Option<String> (
                        "--port-id", "The Id of the port we'll set a property on"),
                    new Option<GettableDeviceProperties> (
                        "--find-by-property", "Get the device by one of its properties. If more than one device matches, the first will be picked."),
                    new Option<String> (
                        "--find-value", "The value to match for the property. (Wildcards accepted)"),


                    new Command("set")
                    {
                        new Option<SettableDeviceProperties> (
                            "--property", "The name of the property to set"), /* TODO: This should be an enum */
                        new Option<String> (
                            "--value", "The new value for the property")

                    },
                    new Command("get")
                    {
                        new Option<GettableDeviceProperties> (
                            "--property", "The name of the property to set"),

                    },

            };

            return command;
        }



        static Command BuildSessionCommand()
        {
            var command = new Command("session", "Perform an operation on a session")
            {
                    new Option<String> (
                        "--session-id", "The Id of the port we'll set a property on"),
                    new Option<GettableSessionProperties> (
                        "--find-by-property", "Get the session by one of its properties. If more than one session matches, the first will be picked."),
                    new Option<String>(
                        "--find-value", "The value to match for the property. (Wildcards accepted)"),

                    new Command("set")
                    {
                        new Option<SettableSessionProperties> (
                            "--property", "The name of the property to set"), /* TODO: This should be an enum */
                        new Option<String> (
                            "--value", "The new value for the property")

                    },
                    new Command("get")
                    {
                        new Option<GettableSessionProperties> (
                            "--property", "The name of the property to set"),

                    },
                    new Command("terminate", "Destroys the specified session. If an application is actively using it, this may cause errors.")
                    {

                    },

            };

            return command;
        }




    }
}

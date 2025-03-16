using System.Collections.Generic;
using OrbToSlfConsole;
using Xunit;

namespace OrbToSlfConsole.Tests
{
    public class CommandLineArgumentsParserTests
    {
        [Fact]
        public void ParseCommandLineArguments_ShouldParseArgumentsCorrectly()
        {
            // Arrange
            var args = new string[]
            {
                "ASTORB",
                "NEOFirst",
                "AddDuplicate",
                "MaxBodies=100",
                "NumOort=50",
                "XOffset=1.0",
                "YOffset=2.0",
                "VXOffset=3.0",
                "VYOffset=4.0"
            };
            var parser = new CommandLineArgumentsParser();

            // Act
            var result = parser.ParseCommandLineArguments(args);

            // Assert
            Assert.Equal("True", result["Astorb"]);
            Assert.Equal("True", result["NEOFirst"]);
            Assert.Equal("True", result["AddDuplicate"]);
            Assert.Equal("100", result["MaxBodies"]);
            Assert.Equal("50", result["NumOort"]);
            Assert.Equal("1.0", result["XOffset"]);
            Assert.Equal("2.0", result["YOffset"]);
            Assert.Equal("3.0", result["VXOffset"]);
            Assert.Equal("4.0", result["VYOffset"]);
        }
    }
}

// --------------------------------------------------------------------------------------------------------------------
// <copyright file="CustomConsoleLoggerProvider.cs" company="Michael William Simmons">
//   (c) 2013-2025 Michael William Simmons
//   Licensed under the Apache License, Version 2.0 (the "License");
//   you may not use this file except in compliance with the License.
//   You may obtain a copy of the License at
//   
//      http://www.apache.org/licenses/LICENSE-2.0
//   
//   Unless required by applicable law or agreed to in writing, software
//   distributed under the License is distributed on an "AS IS" BASIS,
//   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//   See the License for the specific language governing permissions and
//   limitations under the License.
// </copyright>
// <summary>
//   Routines to solve Kepler's equation
// </summary>
// --------------------------------------------------------------------------------------------------------------------
// I believe I based this on https://github.com/dotnet/runtime/blob/main/src/libraries/Microsoft.Extensions.Logging.Console/src/SimpleConsoleFormatter.cs
namespace OrbToSlfConsole
{
  using System;
  using Microsoft.Extensions.Logging;

  public class CustomConsoleLoggerProvider : ILoggerProvider
  {
    public ILogger CreateLogger(string categoryName)
    {
      return new CustomConsoleLogger(categoryName);
    }

    public void Dispose()
    {
    }

    public class CustomConsoleLogger : ILogger
    {
      private readonly string _categoryName;

      private readonly ConsoleColor? DefaultConsoleColor = null;

      public CustomConsoleLogger(string categoryName)
      {
        this._categoryName = categoryName;
      }

      public bool DisableColors { get; set; }

      public IDisposable BeginScope<TState>(TState state)
      {
        return null;
      }

      public bool IsEnabled(LogLevel logLevel)
      {
        return true;
      }

      public void Log<TState>(LogLevel logLevel, EventId eventId, TState state, Exception exception, Func<TState, Exception, string> formatter)
      {
        if (!this.IsEnabled(logLevel))
        {
          return;
        }

        var consoleColors = this.GetLogLevelConsoleColors(logLevel);
        Console.BackgroundColor = consoleColors.Background.Value;
        Console.ForegroundColor = consoleColors.Foreground.Value;
        Console.Write($"{logLevel}: ");
        Console.ResetColor();
        Console.WriteLine($"{this._categoryName}[{eventId.Id}]: {formatter(state, exception)}");
      }

      private ConsoleColors GetLogLevelConsoleColors(LogLevel logLevel)
      {
        if (this.DisableColors)
        {
          return new ConsoleColors(null, null);
        }

        switch (logLevel)
        {
          case LogLevel.Critical:
            return new ConsoleColors(ConsoleColor.White, ConsoleColor.Red);
          case LogLevel.Error:
            return new ConsoleColors(ConsoleColor.Black, ConsoleColor.Red);
          case LogLevel.Warning:
            return new ConsoleColors(ConsoleColor.Yellow, ConsoleColor.Black);
          case LogLevel.Information:
            return new ConsoleColors(ConsoleColor.DarkGreen, ConsoleColor.Black);
          case LogLevel.Debug:
            return new ConsoleColors(ConsoleColor.Gray, ConsoleColor.Black);
          case LogLevel.Trace:
            return new ConsoleColors(ConsoleColor.Gray, ConsoleColor.Black);
          default:
            return new ConsoleColors(this.DefaultConsoleColor, this.DefaultConsoleColor);
        }
      }

      private readonly struct ConsoleColors
      {
        public ConsoleColors(ConsoleColor? foreground, ConsoleColor? background)
        {
          this.Foreground = foreground;
          this.Background = background;
        }

        public ConsoleColor? Foreground { get; }
        public ConsoleColor? Background { get; }
      }
    }
  }
}

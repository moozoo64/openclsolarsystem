namespace OrbToSlfConsole
{
  using Microsoft.Extensions.Configuration;
  using Microsoft.Extensions.DependencyInjection;
  using Microsoft.Extensions.Logging;
  using OrbToSlf;
  internal class Program
  {
    static void Main(string[] args)
    {
      var servicesProvider = ConfigureServices(args);
      var process = servicesProvider.GetRequiredService<IProcess>();
      process.DoProcess();
    }
    private static IServiceProvider ConfigureServices(string[] args)
    {
      bool fetchFromAstorb;
      var commandLineArgumentsParser = new CommandLineArgumentsParser();
      var commandLineConfig = commandLineArgumentsParser.ParseCommandLineArguments(args);

      var builder = new ConfigurationBuilder();
      builder.SetBasePath(Directory.GetCurrentDirectory());
      builder.AddInMemoryCollection(commandLineConfig);
      builder.AddJsonFile("appsettings.json");
      var configuration = builder.Build();
      var logLevelSetting = configuration["LogLevel"];
      if (!LogLevel.TryParse(logLevelSetting, out LogLevel logLevel))
      {
        logLevel = LogLevel.Information;
      }
      fetchFromAstorb = bool.Parse(configuration["Astorb"]);
      var services = new ServiceCollection();
      services.AddLogging(
          loggingBuilder =>
          {
            loggingBuilder.ClearProviders();
            loggingBuilder.SetMinimumLevel(logLevel);
            loggingBuilder.AddProvider(new CustomConsoleLoggerProvider());
          });
      services.AddSingleton<IConfiguration>(configuration);
      services.AddSingleton<IUtilities, Utilities>();
      services.AddSingleton<IPlanets, JPLHorizons>();
      services.AddSingleton<IProcess, Process>();
      if (fetchFromAstorb)
      {
        services.AddSingleton<IRemoteBodyInfosSource, AstroHandler>();
      }
      else
      {
        services.AddSingleton<IRemoteBodyInfosSource, MpcHandler>();
      }
      return services.BuildServiceProvider();
    }
  }
}

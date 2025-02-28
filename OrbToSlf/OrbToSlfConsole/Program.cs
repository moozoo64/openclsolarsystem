

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
      var servicesProvider = ConfigureServices();
      var utilities = servicesProvider.GetRequiredService<IUtilities>();
      utilities.Process(args);
    }
    private static IServiceProvider ConfigureServices()
    {
      var builder = new ConfigurationBuilder();
      builder.SetBasePath(Directory.GetCurrentDirectory());
      builder.AddJsonFile("appsettings.json");

      var configuration = builder.Build();
      var services = new ServiceCollection();
      services.AddLogging(configure => configure.AddConsole());
      services.AddSingleton<IConfiguration>(configuration);
      services.AddSingleton<IUtilities,Utilities>();
      services.AddSingleton<IPlanets, JPLHorizons>();
      return services.BuildServiceProvider();
    }
  }
}

using Xunit;
using OrbToSlf;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace OrbToSlf.Tests
{
  public class UtilitiesTests
  {
    [Theory]
    [InlineData(0, 0)]
    [InlineData(90, Math.PI / 2)]
    [InlineData(180, Math.PI)]
    [InlineData(270, 3 * Math.PI / 2)]
    [InlineData(360, 2 * Math.PI)]
    public void DegreeToRadianTest(double degrees, double expectedRadians)
    {
      // Arrange
      var utilities = new Utilities(null, null, null);

      // Act
      var result = utilities.DegreeToRadian(degrees);

      // Assert
      Assert.Equal(expectedRadians, result, 10);
    }

    [Fact]
    public void MassFromGMRoundTripsThroughSlfG()
    {
      // OpenCLSolarSystem computes GM = mass * G_SLF (6.6725985e-11, Constants.GravitationalConstant); that must give back the GM we started from.
      const double gmSun = 1.32712440018e11; // km^3/s^2
      var mass = Utilities.MassFromGM(gmSun);
      Assert.Equal(1.0, mass * 6.6725985e-11 / 1e9 / gmSun, 14);
    }

    [Fact]
    public void ReadDetailsPrefersGMOverMass()
    {
      var path = System.IO.Path.GetTempFileName();
      try
      {
        System.IO.File.WriteAllLines(path, new[]
        {
          "Number,Name,Radius,Type,GM,Density,Mass,Mass 1e24",
          "0,WithGM,1000,,126686534,,1.0E+20,",       // Mass deliberately wrong: GM must win
          "0,MassOnly,500,,,,3.0E+21,",
          "0,Neither,10,,,,,",
          "1008,La Paz,20.5,,0.0043,,6.5e16,,\"notes, with a comma\"",
        });
        var utilities = new Utilities(null, null, null);

        var infos = utilities.ReadDetails(path);

        Assert.Equal(4, infos.Count);
        Assert.Equal(Utilities.MassFromGM(126686534), infos["WITHGM"].Mass);
        Assert.Equal(3.0e21, infos["MASSONLY"].Mass);
        Assert.Equal(0.0, infos["NEITHER"].Mass);
        Assert.Equal("LaPaz", infos["LAPAZ"].Name); // matched against MPC names, which have no spaces
      }
      finally
      {
        System.IO.File.Delete(path);
      }
    }
  }
}
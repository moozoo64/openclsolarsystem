using OrbToSlf;

namespace OrbToSlf.Tests
{
  public class KeplerTests
  {
    public static IEnumerable<object[]> Cases()
    {
      var eccentricities = new[] { 0.0, 0.01, 0.2056, 0.5, 0.9, 0.99, 0.999, 0.999999 };
      var meanAnomalies = new[] { -3.0, -1.0, -1e-6, 0.0, 1e-6, 0.1, 1.0, 2.5, Math.PI - 1e-9, 3.14159, 7.0, -20.0, 100.0 };
      foreach (var e in eccentricities)
      {
        foreach (var m in meanAnomalies)
        {
          yield return new object[] { m, e };
        }
      }
    }

    // E - e sin(E) - M, with M reduced to the same revolution as E
    private static double Residual(double eccentricAnomaly, double meanAnomaly, double e)
    {
      var r = eccentricAnomaly - (e * Math.Sin(eccentricAnomaly)) - meanAnomaly;
      return r - (Math.Round(r / (2 * Math.PI)) * 2 * Math.PI);
    }

    [Theory]
    [MemberData(nameof(Cases))]
    public void SolveSatisfiesKeplersEquation(double m, double e)
    {
      var eccentricAnomaly = new Kepler().Solve(m, e);

      Assert.True(Math.Abs(Residual(eccentricAnomaly, m, e)) < 1e-12, $"residual {Residual(eccentricAnomaly, m, e)}");
    }

    [Theory]
    [MemberData(nameof(Cases))]
    public void SolveStaysInTheSameRevolutionAsTheMeanAnomaly(double m, double e)
    {
      var eccentricAnomaly = new Kepler().Solve(m, e);

      Assert.True(Math.Abs(eccentricAnomaly - m) <= Math.PI + 1e-9, $"E {eccentricAnomaly} M {m}");
    }

    [Theory]
    [MemberData(nameof(Cases))]
    public void SinnottSatisfiesKeplersEquation(double m, double e)
    {
      var eccentricAnomaly = new Kepler().Sinnott(m, e);

      Assert.True(Math.Abs(Residual(eccentricAnomaly, m, e)) < 1e-12, $"residual {Residual(eccentricAnomaly, m, e)}");
    }

    [Theory]
    [MemberData(nameof(Cases))]
    public void SolveAgreesWithSinnott(double m, double e)
    {
      var kepler = new Kepler();
      var difference = kepler.Solve(m, e) - kepler.Sinnott(m, e);
      difference -= Math.Round(difference / (2 * Math.PI)) * 2 * Math.PI;

      // Near E = 0 with e close to 1, dE/dM = 1 / (1 - e cos E) is large, so E is less well determined
      Assert.True(Math.Abs(difference) < 1e-7, $"difference {difference}");
    }

    [Theory]
    [InlineData(-0.1)]
    [InlineData(1.0)]
    [InlineData(1.5)]
    [InlineData(double.NaN)]
    public void SolveRejectsNonEllipticEccentricity(double e)
    {
      Assert.Throws<ArgumentOutOfRangeException>(() => new Kepler().Solve(1.0, e));
    }
  }
}

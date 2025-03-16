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
  }
}
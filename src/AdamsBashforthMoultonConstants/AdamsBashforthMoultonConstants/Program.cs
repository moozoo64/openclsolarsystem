// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Program.cs" company="Michael Simmons">
//   (c) Michael Simmons
// </copyright>
// <summary>
//   Computes the Co-efficents for the Adams-Bashforth and Adams-Moulton method of given order
//   The formulars of which can be found in Fundamentals of Celestrial Mechanics by J.M.A Danby page 303-304 equations
//   10.7.8 and 10.7.9
//   A basic language implementation can also be found in lines 500-780 on page 305
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace AdamConstants
{
    #region

    using System;
    using System.Numerics;

  using ExtendedNumerics;

    #endregion

    /// <summary>
    ///     Computes the Co-efficents for the Adams-Bashforth and Adams-Moulton method of given order
    ///     The formulars of which can be found in Fundamentals of Celestrial Mechanics by J.M.A Danby page 303-304 equations
    ///     10.7.8 and 10.7.9
    ///     A basic language implementation can also be found in lines 500-780 on page 305
    /// </summary>
    internal class Program
    {
    #region Methods

    /// <summary>
    /// Computes the Co-efficents for the Adams-Bashforth and Adams-Moulton method of given order
    /// </summary>
    private static void Main()
    {

      int[] orders = new int[] { 2, 4, 8, 10, 11, 12, 16 };

      foreach (int order in orders)
      {
        Coefficents(order);
        Console.WriteLine();
      }
    }
    static void Coefficents( int order)
    {
      var p = new BigRational[order + 1, order + 1];
      var b = new BigRational[order + 1];
      var bs = new BigRational[order + 1];

      for (var j = 1; j <= order; j++)
      {
        b[j] = BigRational.Zero;
        bs[j] = BigRational.Zero;
        p[1, j] = BigRational.One;
      }

      for (var i = 2; i <= order; i++)
      {
        for (var j = i; j <= order; j++)
        {
          var fi = new BigInteger(i);
          var fj = new BigInteger(j);

          p[i, j] = -p[i - 1, j] * new BigRational(fj - fi + BigInteger.One, fi - BigInteger.One);
        }
      }

      var g = new BigRational[order + 1];
      var gg = new BigRational[order + 1];
      g[1] = BigRational.One;
      gg[1] = BigRational.One;

      for (var i = 2; i <= order; i++)
      {
        g[i] = BigRational.One;
        gg[i] = BigRational.Zero;
        for (var j = 2; j <= i; j++)
        {
          var fj = new BigInteger(j);
          g[i] = g[i] - (g[i - j + 1] / fj);
          gg[i] = gg[i] - (gg[i - j + 1] / fj);
        }
      }

      for (var m = 1; m <= order; m++)
      {
        for (var j = m; j <= order; j++)
        {
          b[m] = b[m] + (p[m, j] * g[j]);
          bs[m] = bs[m] + (p[m, j] * gg[j]);
        }
      }

      var bigTenDigits = 36;
      var bigTen = BigInteger.Parse("1" + new string('0', bigTenDigits));

      for (var index = 1; index < order + 1; index++)
      {
        var sign = b[index].Sign;
        BigInteger number;
        BigInteger numerator = (b[index].WholePart * b[index].FractionalPart.Denominator) + b[index].FractionalPart.Numerator;
        BigInteger denominator = b[index].FractionalPart.Denominator;
        if (sign < 0)
        {
          number = -(numerator * bigTen) / denominator;
        }
        else
        {
          number = (numerator * bigTen) / denominator;
        }

        var numberString = number.ToString();
        var decimalPoint = numberString.Length - bigTenDigits;
        while (decimalPoint < 1)
        {
          numberString = "0" + numberString;
          decimalPoint++;
        }

        numberString = numberString.Insert(decimalPoint, ".");
        numberString = sign < 0 ? "-" + numberString : numberString;

        var formatString = "#define B{0,2:0#}C{1,2:0#} {2," + (bigTenDigits + 10).ToString("##") + "}\t// {3}";
        Console.WriteLine(formatString, order, index, numberString, (float)b[index]);
      }

      for (var index = 1; index < order + 1; index++)
      {
        var sign = bs[index].Sign;
        BigInteger number;
        BigInteger numerator = (bs[index].WholePart * bs[index].FractionalPart.Denominator)+ bs[index].FractionalPart.Numerator;
        BigInteger denominator = bs[index].FractionalPart.Denominator;
        if (sign < 0)
        {
          number = -(numerator * bigTen) / denominator;
        }
        else
        {
          number = (numerator * bigTen) / denominator;
        }

        var numberString = number.ToString();
        var decimalPoint = numberString.Length - bigTenDigits;
        while (decimalPoint < 1)
        {
          numberString = "0" + numberString;
          decimalPoint++;
        }

        numberString = numberString.Insert(decimalPoint, ".");
        numberString = sign < 0 ? "-" + numberString : numberString;

        var formatString = "#define M{0,2:0#}C{1,2:0#} {2," + (bigTenDigits + 10).ToString("##") + "}\t// {3}";
        Console.WriteLine(formatString, order, index, numberString, (float)bs[index]);
      }

    }

    #endregion
  }
}
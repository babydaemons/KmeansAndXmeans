using System;
using System.Collections.Generic;
using System.Linq;
using System.Drawing;

namespace xMeans
{
	public class Kmeans
	{
		public int K;
		public List<ClusteringPoint> ClusterPoints = new List<ClusteringPoint>();
		public List<Point> CenterList = new List<Point>();
		public List<Point> BeforeCenterList = new List<Point>();
		public List<double> CenterDistAvg = new List<double>();
		public List<double> CenterDistDispersion = new List<double>();

		public List<double> BICs = new List<double>();

		public Kmeans(int k, List<Point> points)
		{
			K = k;
			var rnd = new Random();
			foreach (var p in points)
			{
				//First is random
				var cluster = rnd.Next(k);
				ClusterPoints.Add(new ClusteringPoint(p, cluster));
			}

			for (int i = 0; i < K; i++)
			{
				CenterDistAvg.Add(int.MaxValue);
				CenterDistDispersion.Add(int.MaxValue);
				BICs.Add(int.MaxValue);
				CenterList.Add(new Point(int.MaxValue, int.MaxValue));
			}

			BeforeCenterList = new List<Point>(CenterList);
		}

		private void OneAct()
		{
			//Update Center
			for (int i = 0; i < K; i++)
			{
				BeforeCenterList[i] = new Point(CenterList[i].X, CenterList[i].Y);

				var cv = ClusterPoints.Where(x => x.ClusterIndex == i);
				if (cv.Count() > 0)
				{
					var avgX = cv.Average(x => x.Point.X);
					var avgY = cv.Average(x => x.Point.Y);
					CenterList[i] = new Point((int)avgX, (int)avgY);
				}
				//error exception
				else
				{
					BeforeCenterList[i] = new Point(int.MaxValue, int.MaxValue);
					CenterList[i] = new Point(int.MaxValue, int.MaxValue);
				}
			}

			//Update Nearest Cluster
			foreach (var p in ClusterPoints)
			{
				var updateIndex = GetNearestIndex(p.Point, CenterList);
				p.ClusterIndex = updateIndex;
			}

			//Update Avg Center Dist
			for (int i = 0; i < K; i++)
			{
				var cPoints = ClusterPoints.Where(x => x.ClusterIndex == i);

				//error exception
				if (cPoints.Count() == 0)
				{
					CenterDistAvg[i] = int.MaxValue;
					CenterDistDispersion[i] = int.MaxValue;
					BICs[i] = int.MaxValue;
					continue;
				}

				var list = new List<double>();
				foreach (var p in cPoints)
				{
					var dist = Math.Sqrt(Math.Pow(CenterList[i].X - p.Point.X, 2) + Math.Pow(CenterList[i].Y - p.Point.Y, 2));
					list.Add(dist);
				}
				CenterDistAvg[i] = list.Average();

				var sum = 0.0;
				foreach (var l in list)
				{
					sum += Math.Pow((l - CenterDistAvg[i]), 2);
				}
				CenterDistDispersion[i] = sum / (double)list.Count;

				//Cal BICs
				//http://stackoverflow.com/questions/15839774/how-to-calculate-bic-for-k-means-clustering-in-r

				//cal witness
				var sumX = cPoints.Sum(x => x.Point.X);
				var sumY = cPoints.Sum(x => x.Point.Y);
				var sumX2 = cPoints.Sum(x => x.Point.X * x.Point.X);
				var sumY2 = cPoints.Sum(x => x.Point.Y * x.Point.Y);
				var squX = sumX2 - (sumX * sumX / (double)cPoints.Count());
				var squY = sumY2 - (sumY * sumY / (double)cPoints.Count());
				var totWitness = squX + squY;

				var m = 2; //point dimension
				var n = cPoints.Count();
				var d = totWitness;
				BICs[i] = d + Math.Log(n) * m * K;
			}
		}

		public int Calculation(double permitErrorDist)
		{
			var loopCount = 0;
			while (true)
			{
				loopCount++;
				OneAct();
				var ok = true;
				//judge all center change value is less than error dist
				for (int i = 0; i < CenterList.Count; i++)
				{
					var dist = Math.Sqrt(Math.Pow(CenterList[i].X - BeforeCenterList[i].X, 2)
									   + Math.Pow(CenterList[i].Y - BeforeCenterList[i].Y, 2));
					if (permitErrorDist < dist)
					{
						ok = false;
						break;
					}
				}
				if (ok) break;
			}
			return loopCount;
		}

		private int GetNearestIndex(Point p, List<Point> centerList)
		{
			var value = Math.Sqrt(Math.Pow(centerList[0].X - p.X, 2) + Math.Pow(centerList[0].Y - p.Y, 2));
			var index = 0;

			for (int i = 1; i < centerList.Count; i++)
			{
				var v = Math.Sqrt(Math.Pow(centerList[i].X - p.X, 2) + Math.Pow(centerList[i].Y - p.Y, 2));
				if (v < value)
				{
					value = v;
					index = i;
				}
			}
			return index;
		}
	}
}

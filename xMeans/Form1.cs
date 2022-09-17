using System;
using System.Collections.Generic;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Windows.Forms;

namespace xMeans
{
	public partial class Form1 : Form
	{
		public Form1()
		{
			InitializeComponent();
		}

		private int PlotDrawCount = 0;

		private void Form1_Load(object sender, EventArgs e)
		{
			chart1.Series.Clear();
		}

		private void ReadFileButton_Click(object sender, EventArgs e)
		{
			var points = new List<Point>();

			//File Dialog
			OpenFileDialog ofd = new OpenFileDialog();
			ofd.Filter = "(*.txt)|*.txt";
			ofd.FilterIndex = 2;
			ofd.Title = "Select 2D Data";
			ofd.RestoreDirectory = true;

			if (ofd.ShowDialog() == DialogResult.OK)
			{
				var path = ofd.FileName;
				var lines = File.ReadAllLines(path);
				foreach (var line in lines)
				{
					var sp = line.Split(',');
					points.Add(new Point(int.Parse(sp[0]), int.Parse(sp[1])));
				}
				ActMain(points);
			}
		}

		private void RndBtn_Click(object sender, EventArgs e)
		{
			var rndPoints = GetRndPoint(50);
			ActMain(rndPoints);
		}

		private void ActMain(List<Point> points)
		{
			PlotDrawCount = 0;
			chart1.Series.Clear();

			//kemans draw
			/*
			var k = 4;
			var kmeans = new Kmeans(k, points);
			var loopCount = kmeans.Calculation(1);
			DrawGraph(kmeans);
			*/

			//xmenas draw

			var xMeans = new Xmeans(points);
			var kmeansResults = xMeans.Calculation();
			var cSum = kmeansResults.Sum(x => x.ClusterPoints.Count()); //All Point Number

			//Console.WriteLine("{0}", cSum); //debug

			foreach (var mean in kmeansResults)
				DrawGraph(mean);
		}

		private List<ClusteringPoint> Kmeans(int k, List<Point> points)
		{
			var cList = new List<ClusteringPoint>();
			var rnd = new Random();
			foreach (var p in points)
			{
				var cluster = rnd.Next(k);
				cList.Add(new ClusteringPoint(p, cluster));
			}

			return cList;
		}

		private void DrawGraph(Kmeans kmeans)
		{
			var plotName = "p" + PlotDrawCount;

			//Init
			for (int i = 0; i < kmeans.K; i++)
			{
				if (kmeans.CenterList[i].X != int.MaxValue)
				{
					chart1.Series.Add(plotName + i);
					chart1.Series[plotName + i].IsVisibleInLegend = false;
					chart1.Series[plotName + i].ChartType = System.Windows.Forms.DataVisualization.Charting.SeriesChartType.Point;
				}
			}

			//plot Normal Point
			foreach (var p in kmeans.ClusterPoints)
			{
				chart1.Series[plotName + p.ClusterIndex].Points.AddXY(p.Point.X, p.Point.Y);
			}

			//Clusterling Average Plot
			for (int i = 0; i < kmeans.CenterList.Count; i++)
			{
				if (kmeans.CenterList[i].X != int.MaxValue)
				{
					chart1.Series[plotName + i].Points.AddXY(kmeans.CenterList[i].X, kmeans.CenterList[i].Y);
					var len = chart1.Series[plotName + i].Points.Count;
					chart1.Series[plotName + i].Points[len - 1].MarkerSize = 15;
				}
			}
			PlotDrawCount++;
		}

		private List<Point> GetRndPoint(int num)
		{
			var list = new List<Point>();
			var rnd = new System.Random();
			for (var i = 0; i < num; i++)
				list.Add(new Point((int)rnd.Next(100), (int)rnd.Next(100)));
			return list;
		}
	}
}
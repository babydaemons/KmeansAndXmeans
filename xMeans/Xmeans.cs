using System.Collections.Generic;
using System.Linq;
using System.Drawing;

namespace xMeans
{
	public class Xmeans
	{
		public Kmeans TopKm;
		private int _k = 2;

		public Xmeans(List<Point> points)
		{
			TopKm = new Kmeans(_k, points);
		}

		private List<Kmeans> RecursiveProcessing(Kmeans parent, List<Kmeans> register)
		{
			var child1 = new Kmeans(_k, parent.ClusterPoints.Where(x => x.ClusterIndex == 0).Select(x => x.Point).ToList());
			var child2 = new Kmeans(_k, parent.ClusterPoints.Where(x => x.ClusterIndex == 1).Select(x => x.Point).ToList());
			child1.Calculation(1);
			child2.Calculation(1);
			if (parent.CenterDistDispersion[0] > child1.CenterDistDispersion[0] + child1.CenterDistDispersion[1]
				//	if( parent.BICs[0] > child1.BICs[0] + child1.BICs[1]
				&& child1.ClusterPoints.Count > 1)
			{
				RecursiveProcessing(child1, register);
			}
			else register.Add(child1);
			if (parent.CenterDistDispersion[1] > child2.CenterDistDispersion[0] + child2.CenterDistDispersion[1]
				//	if(parent.BICs[1] > child2.BICs[0] + child2.BICs[1]
				&& child2.ClusterPoints.Count > 1)
			{
				RecursiveProcessing(child2, register);
			}
			else register.Add(child2);

			return register;
		}

		public List<Kmeans> Calculation()
		{
			TopKm.Calculation(1);
			var src = new List<Kmeans>();
			var result = RecursiveProcessing(TopKm, src);
			return result;
		}
	}
}

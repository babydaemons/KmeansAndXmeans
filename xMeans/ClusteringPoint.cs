using System.Drawing;

namespace xMeans
{
	public class ClusteringPoint
	{
		public int ClusterIndex;
		public Point Point;

		public ClusteringPoint(Point p, int cluserIndex)
		{
			ClusterIndex = cluserIndex;
			Point = p;
		}
	}
}

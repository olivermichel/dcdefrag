#ifndef CLOUDSIM_EDGE_H
#define CLOUDSIM_EDGE_H

namespace cloudsim {

	class Edge {

	public:
		Edge();
		Edge(const Edge& copy_from) = default;
		unsigned long type() const;
		virtual ~Edge();

	protected:
		unsigned long _type;
	};
}

#endif

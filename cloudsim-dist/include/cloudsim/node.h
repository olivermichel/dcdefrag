#ifndef CLOUDSIM_NODE_H
#define CLOUDSIM_NODE_H

namespace cloudsim {

	class Node {

	public:
		Node(unsigned long type = 0);
		Node(const Node& copy_from) = default;
		unsigned long type() const;
		virtual ~Node();

	protected:
		unsigned long _type;
	};
}
#endif

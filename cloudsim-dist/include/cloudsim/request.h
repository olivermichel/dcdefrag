
#ifndef CLOUDSIM_REQUEST_H
#define CLOUDSIM_REQUEST_H

namespace cloudsim {
	class Request {

	public:
		Request() = delete;
		Request(unsigned long id);
		unsigned long id() const;
		virtual ~Request();

	private:
		unsigned long _id;
	};
}

#endif

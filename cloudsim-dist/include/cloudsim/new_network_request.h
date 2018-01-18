#ifndef CLOUDSIM_NEW_NETWORK_REQUEST_H
#define CLOUDSIM_NEW_NETWORK_REQUEST_H

#include "request.h"

namespace cloudsim {

	class NewNetworkRequest : public Request
	{
	public:
		NewNetworkRequest() = delete;
		NewNetworkRequest(unsigned long id);
		virtual ~NewNetworkRequest();
	};
}

#endif

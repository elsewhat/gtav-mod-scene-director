#include "Network.h"
#include "tinyxml2.h"
#include "script.h"
#include <vector>
#include <algorithm>
#include <cpprest/filestream.h>
#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/uri.h>       


using namespace utility;                    // Common utilities like string conversions
using namespace web;                        // Common features like URIs.
using namespace web::http;                  // Common HTTP functionality
using namespace web::http::client;          // HTTP client features
using namespace concurrency::streams;       // Asynchronous streams

using namespace web::json;

void testRequest()
{
	log_to_file("Send request to google");
	auto fileStream = std::make_shared<ostream>();
	// Open stream to output file.
	pplx::task<void> requestTask = fstream::open_ostream(U("scenedirector_network.txt")).then([=](ostream outFile)
	{
		*fileStream = outFile;

		// Create http_client to send the request.
		http_client client(U("http://www.google.com/"));

		// Build request URI and start the request.
		uri_builder builder(U("/search"));
		builder.append_query(U("q"), U("scene director"));
		return client.request(methods::GET, builder.to_string());
	})

		// Handle response headers arriving.
		.then([=](http_response response)
	{
		log_to_file("Received response status code :" + std::to_string(response.status_code()));
		printf("Received response status code:%u\n", response.status_code());

		// Write response body into the file.
		return response.body().read_to_end(fileStream->streambuf());
	})

		// Close the file stream.
		.then([=](size_t)
	{
		return fileStream->close();
	});

	// Wait for all the outstanding I/O to complete and handle any exceptions
	try
	{
		requestTask.wait();
	}
	catch (const std::exception &e)
	{
		log_to_file("Received response status code :" + std::string(e.what()));
		printf("Error exception:%s\n", e.what());
	}
}
